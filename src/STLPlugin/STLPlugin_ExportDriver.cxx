// Copyright (C) 2014-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "STLPlugin_ExportDriver.hxx"
#include "STLPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"

// OOCT includes
#include <BRepBuilderAPI_Copy.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& STLPlugin_ExportDriver::GetID()
{
  static Standard_GUID aGUID("88678a6a-885c-477c-b90b-51934572c61d");
  return aGUID;
}

//=======================================================================
//function : STLPlugin_ExportDriver
//purpose  :
//=======================================================================
STLPlugin_ExportDriver::STLPlugin_ExportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer STLPlugin_ExportDriver::Execute( TFunction_Logbook& log ) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  STLPlugin_IExport aData (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aData.GetOriginal();
  if( aRefFunction.IsNull() ) return 0;
  aShape = aRefFunction->GetValue();
  if( aShape.IsNull() ) return 0;
  // set the result of function to be used by next operations
  aFunction->SetValue( aShape );

  TCollection_AsciiString aFileName = aData.GetFileName();
  double aDeflection = aData.GetDeflection();
  bool anIsASCII = aData.GetIsASCII();
  bool anIsRelative = aData.GetIsRelative();

  MESSAGE( "Export STL into file " << aFileName );

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  try
  {
    StlAPI_Writer aWriter;
    // set relative mode on false for using custom deflection coefficient
    aWriter.RelativeMode( ) = anIsRelative;
    aWriter.ASCIIMode() = anIsASCII;
    if( anIsRelative )
      aWriter.SetCoefficient( aDeflection );
    else
      aWriter.SetDeflection( aDeflection );
    BRepBuilderAPI_Copy aCopy( aShape, Standard_False );
    aWriter.Write( aCopy.Shape(), aFileName.ToCString() );
    log.SetTouched( Label() );
    return 1;
  }
  catch( Standard_Failure )
  {
    //THROW_SALOME_CORBA_EXCEPTION("Exception catched in ExportSTL", SALOME::BAD_PARAM);
  }
  return 0;
}

//=======================================================================
//function : MustExecute
//purpose  :
//=======================================================================
Standard_Boolean STLPlugin_ExportDriver::MustExecute( const TFunction_Logbook& ) const
{
  return Standard_True;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
bool STLPlugin_ExportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  return false;
}

IMPLEMENT_STANDARD_HANDLE( STLPlugin_ExportDriver,GEOM_BaseDriver );
IMPLEMENT_STANDARD_RTTIEXT( STLPlugin_ExportDriver,GEOM_BaseDriver );
