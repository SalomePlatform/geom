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
#include "STLPlugin_ExportDriver.hxx"
#include "STLPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>
#include <Basics_OCCTVersion.hxx>

// GEOM includes
#include "GEOM_Function.hxx"

// OOCT includes
#include <BRepBuilderAPI_Copy.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

#define MAX2(X, Y)      ( Abs(X) > Abs(Y) ? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)   ( MAX2 ( MAX2(X, Y) , Z ) )

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
Standard_Integer STLPlugin_ExportDriver::Execute(LOGBOOK& log) const
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
    // copy source shape
    BRepBuilderAPI_Copy aCopy( aShape, Standard_False );
    TopoDS_Shape aCopyShape = aCopy.Shape();
    // ASCII mode
    aWriter.ASCIIMode() = anIsASCII;
#if OCC_VERSION_LARGE > 0x06080000
    if ( anIsRelative ) {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      Bnd_Box bndBox;
      BRepBndLib::Add( aShape, bndBox );
      bndBox.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
      aDeflection = MAX3( aXmax-aXmin, aYmax-aYmin, aZmax-aZmin ) * aDeflection;
    }
    //Compute triangulation
    BRepTools::Clean( aCopyShape );
    BRepMesh_IncrementalMesh aMesh( aCopyShape, aDeflection );
#else
    // set relative mode on false for using custom deflection coefficient
    aWriter.RelativeMode( ) = anIsRelative;
    if( anIsRelative )
      aWriter.SetCoefficient( aDeflection );
    else
      aWriter.SetDeflection( aDeflection );
#endif
    aWriter.Write( aCopyShape, aFileName.ToCString() );
#if OCC_VERSION_MAJOR < 7
    log.SetTouched(Label());
#else
    log->SetTouched(Label());
#endif
    return 1;
  }
  catch( Standard_Failure )
  {
    //THROW_SALOME_CORBA_EXCEPTION("Exception catched in ExportSTL", SALOME::BAD_PARAM);
  }
  return 0;
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

OCCT_IMPLEMENT_STANDARD_RTTIEXT( STLPlugin_ExportDriver,GEOM_BaseDriver );
