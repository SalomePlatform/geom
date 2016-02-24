// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// internal includes
#include "IGESPlugin_ExportDriver.hxx"
#include "IGESPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"

// OOCT includes
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>

//=============================================================================
/*!
 *  KindOfBRep
 *  \return 0 if theShape contains only simple entities (wires, edges and vertices),
 *          1 if theShape contains only complex entities (shells, solids and compsolids)
 *          2 if theShape contains only indifferent entities (faces)
 *         -1 if theShape contains both simple and complex entities (and in this case it
 *            cannot be saved without any loss neither in BRepMode == 0 nor in BRepMode == 1)
 */
//=============================================================================
int KindOfBRep (const TopoDS_Shape& theShape)
{
  int aKind = 2;

  switch (theShape.ShapeType())
  {
  case TopAbs_COMPOUND:
    {
      bool isSimple = false;
      bool isComplex = false;
      TopoDS_Iterator anIt (theShape, Standard_True, Standard_True);
      for (; anIt.More(); anIt.Next()) {
        TopoDS_Shape aS = anIt.Value();
        int aKindSub = KindOfBRep(aS);
        if (aKindSub == 0)
          isSimple = true;
        else if (aKindSub == 1)
          isComplex = true;
        else if (aKindSub == -1) {
          return -1; // heterogeneous
        }
      }
      if (isSimple && isComplex)
        aKind = -1; // heterogeneous
      else if (isSimple)
        aKind = 0;
      else if (isComplex)
        aKind = 1;
    }
    break;
  case TopAbs_COMPSOLID:
  case TopAbs_SOLID:
  case TopAbs_SHELL:
    aKind = 1;
    break;
  case TopAbs_WIRE:
  case TopAbs_EDGE:
  case TopAbs_VERTEX:
    aKind = 0;
    break;
  default:
    aKind = 2;
  }

  return aKind;
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& IGESPlugin_ExportDriver::GetID()
{
  static Standard_GUID aGUID("8fa4111a-24cb-4819-8f03-b0e2890d2a2a");
  return aGUID;
}

//=======================================================================
//function : IGESPlugin_ExportDriver
//purpose  :
//=======================================================================
IGESPlugin_ExportDriver::IGESPlugin_ExportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer IGESPlugin_ExportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  IGESPlugin_IExport aData (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aData.GetOriginal();
  if( aRefFunction.IsNull() ) return 0;
  aShape = aRefFunction->GetValue();
  if( aShape.IsNull() ) return 0;
  // set the result of function to be used by next operations
  aFunction->SetValue( aShape );

  TCollection_AsciiString aFileName = aData.GetFileName();
  TCollection_AsciiString aVersion = aData.GetVersion();
  // define, whether to write only faces (5.1 IGES format)
  // or shells and solids also (5.3 IGES format)
  int aBrepMode = 0;
  if( aVersion.IsEqual( "5.3" ) )
    aBrepMode = 1;

  MESSAGE("Export IGES into file " << aFileName.ToCString());

  // Mantis issue 0021350: check being exported shape, as some standalone
  // entities (edges, wires and vertices) cannot be saved in BRepMode
  if( aBrepMode == 1 ) {
    int aKind = KindOfBRep( aShape );
    if( aKind == -1 )
      Standard_Failure::Raise( "EXPORT_IGES_HETEROGENEOUS_COMPOUND" );
    else if( aKind == 2 )
      aBrepMode = 1;
    else
      aBrepMode = aKind;
  }

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  // initialize writer
  IGESControl_Controller::Init();
  IGESControl_Writer ICW( "M", aBrepMode ); // export explicitly in meters
  Interface_Static::SetCVal( "xstep.cascade.unit", "M" );

  // 09.03.2010 skl for bug 0020726
  // change default value "Average" to "Max"
  Interface_Static::SetCVal( "write.precision.mode", "Max" );

  // perform shape writing
  if( ICW.AddShape( aShape ) ) {
    ICW.ComputeModel();
    return ICW.Write( aFileName.ToCString() );
  }
  return 0;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
bool IGESPlugin_ExportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  return false;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( IGESPlugin_ExportDriver,GEOM_BaseDriver );
