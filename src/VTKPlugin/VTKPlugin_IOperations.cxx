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
#include "VTKPlugin_IOperations.hxx"
#include "VTKPlugin_ExportDriver.hxx"
#include "VTKPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>

// GEOM includes
#include "GEOM_PythonDump.hxx"
#include "GEOMImpl_Types.hxx"

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
VTKPlugin_IOperations::VTKPlugin_IOperations( GEOM_Engine* theEngine, int theDocID )
: GEOMImpl_IBaseIEOperations( theEngine, theDocID )
{
  MESSAGE( "VTKPlugin_IOperations::VTKPlugin_IOperations" );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
VTKPlugin_IOperations::~VTKPlugin_IOperations()
{
  MESSAGE( "VTKPlugin_IOperations::~VTKPlugin_IOperations" );
}

//=============================================================================
/*!
 *  ExportVTK
 *  Export a shape to VTK format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the file to exported
 *  \param theDeflection The deflection of the shape to exported
 */
//=============================================================================
void VTKPlugin_IOperations::ExportVTK( const Handle(GEOM_Object)      theOriginal,
				       const TCollection_AsciiString& theFileName,
				       const double                   theDeflection )
{
  SetErrorCode(KO);
  if( theOriginal.IsNull() ) return;

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if( aRefFunction.IsNull() ) return;  //There is no function which creates an object to be exported

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject( GetDocID(), GEOM_IMPORT);

  //Add an Export function
  Handle(GEOM_Function) aFunction = result->AddFunction( VTKPlugin_ExportDriver::GetID(), EXPORT_SHAPE );
  if( aFunction.IsNull() ) return;

  //Check if the function is set correctly
  if( aFunction->GetDriverGUID() != VTKPlugin_ExportDriver::GetID() ) return;

  //Set parameters
  VTKPlugin_IExport aCI( aFunction );
  aCI.SetOriginal( aRefFunction );
  aCI.SetFileName( theFileName );
  aCI.SetDeflection( theDeflection );

  //Perform the Export
  try {
    OCC_CATCH_SIGNALS;
    if( !GetSolver()->ComputeFunction( aFunction ) ) {
      SetErrorCode( "Not enough space on disk, or you haven't permissions to write this directory" );
      return;
    }
  }
  catch( Standard_Failure ) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode( aFail->GetMessageString() );
    return;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.ExportVTK(" << theOriginal << ", \""
    << theFileName.ToCString() << "\", " << theDeflection << ")";

  SetErrorCode(OK);
}
