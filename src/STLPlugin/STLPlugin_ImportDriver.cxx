// Copyright (C) 2014-2024  CEA, EDF, OPEN CASCADE
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
#include "STLPlugin_ImportDriver.hxx"
#include "STLPlugin_IImport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>
#include <Basics_OCCTVersion.hxx>

// GEOM includes
#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

// OOCT includes
#include <StlAPI_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>
#include <Precision.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& STLPlugin_ImportDriver::GetID()
{
  static Standard_GUID aGUID("69e33dc7-9630-4a73-ac19-cc4da1f659b8");
  return aGUID;
}

//=======================================================================
//function : STLPlugin_ImportDriver
//purpose  :
//=======================================================================
STLPlugin_ImportDriver::STLPlugin_ImportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer STLPlugin_ImportDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  STLPlugin_IImport aData( aFunction );

  TCollection_AsciiString aFileName = aData.GetFileName().ToCString();

  MESSAGE( "Import STL from file " << aFileName );

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  StlAPI_Reader aReader;
  TopoDS_Shape aShape;

  aReader.Read( aShape, aData.GetFileName().ToCString() );

  if( aShape.IsNull() ) return 0;

#if OCC_VERSION_LARGE >= 0x07070000
  BRepBuilderAPI_Sewing aSewingTool;
  aSewingTool.Init(1.0e-06, Standard_True);
  aSewingTool.Load(aShape);
  aSewingTool.Perform();
  TopoDS_Shape aSewedShape = aSewingTool.SewedShape();
  if (!aSewedShape.IsNull())
    aShape = aSewedShape;
#endif

  // Fix the orientation of closed shell or solid.
  if( BRep_Tool::IsClosed( aShape ) ) {
    TopAbs_ShapeEnum aType = aShape.ShapeType();
    if( aType == TopAbs_SHELL || aType == TopAbs_SOLID ) {
      TopoDS_Solid aSolid;
      if( aType == TopAbs_SHELL ) {
        // Create a solid.
        BRep_Builder aBuilder;
        aBuilder.MakeSolid( aSolid );
        aBuilder.Add(aSolid, aShape );
      }
      else
        aSolid = TopoDS::Solid( aShape );

      // Classify infinite point against solid.
      BRepClass3d_SolidClassifier aClassifier( aSolid );
      aClassifier.PerformInfinitePoint(Precision::Confusion());

      if( aClassifier.State() == TopAbs_IN ) {
        // The shape is inverted. Reverse it.
        aShape.Reverse();
      }
    }
  }

  aFunction->SetValue( aShape );

  log->SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool STLPlugin_ImportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction( Label() );

  STLPlugin_IImport aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "ImportSTL";

  switch ( aType ) {
  case IMPORT_SHAPE:
    AddParam( theParams, "File name", aCI.GetFileName() );
    break;
  default:
    return false;
  }
  return true;
}

IMPLEMENT_STANDARD_RTTIEXT( STLPlugin_ImportDriver, GEOM_BaseDriver )
