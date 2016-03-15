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
#include "BREPPlugin_IOperations.hxx"
#include "BREPPlugin_ExportDriver.hxx"
#include "BREPPlugin_ImportDriver.hxx"
#include "BREPPlugin_IExport.hxx"
#include "BREPPlugin_IImport.hxx"

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
BREPPlugin_IOperations::BREPPlugin_IOperations( GEOM_Engine* theEngine, int theDocID )
: GEOMImpl_IBaseIEOperations( theEngine, theDocID )
{
  MESSAGE( "BREPPlugin_IOperations::BREPPlugin_IOperations" );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
BREPPlugin_IOperations::~BREPPlugin_IOperations()
{
  MESSAGE( "BREPPlugin_IOperations::~BREPPlugin_IOperations" );
}

//=============================================================================
/*!
 *  ExportBREP
 *  Export a shape to BREP format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the file to exported
 *  \param theIsASCII The format of the exported file (ASCII or Binary)
 *  \param theDeflection The deflection of the shape to exported
 */
//=============================================================================
void BREPPlugin_IOperations::ExportBREP( const Handle(GEOM_Object)      theOriginal,
					 const TCollection_AsciiString& theFileName )
{
  SetErrorCode(KO);
  if( theOriginal.IsNull() ) return;

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if( aRefFunction.IsNull() ) return;  //There is no function which creates an object to be exported

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject( GetDocID(), GEOM_IMPORT);

  //Add an Export function
  Handle(GEOM_Function) aFunction = result->AddFunction( BREPPlugin_ExportDriver::GetID(), EXPORT_SHAPE );
  if( aFunction.IsNull() ) return;

  //Check if the function is set correctly
  if( aFunction->GetDriverGUID() != BREPPlugin_ExportDriver::GetID() ) return;

  //Set parameters
  BREPPlugin_IExport aCI( aFunction );
  aCI.SetOriginal( aRefFunction );
  aCI.SetFileName( theFileName );

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
  GEOM::TPythonDump(aFunction) << "geompy.ExportBREP(" << theOriginal << ", \""
    << theFileName.ToCString() << "\" )";

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  ImportBREP
 *  Import a shape from BREP format
 *  \param theFileName The name of the file to import
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
BREPPlugin_IOperations::ImportBREP( const TCollection_AsciiString& theFileName )
{
  SetErrorCode(KO);
  if( theFileName.IsEmpty() ) return NULL;

  //Add a new result object
  Handle(GEOM_Object) anImported = GetEngine()->AddObject( GetDocID(), GEOM_IMPORT );

  //Add an Import function
  Handle(GEOM_Function) aFunction =
    anImported->AddFunction( BREPPlugin_ImportDriver::GetID(), IMPORT_SHAPE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != BREPPlugin_ImportDriver::GetID()) return NULL;

  //Set parameters
  BREPPlugin_IImport aCI( aFunction );
  aCI.SetFileName( theFileName );

  //Perform the Import
  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  try {
    OCC_CATCH_SIGNALS;
    if( !GetSolver()->ComputeFunction( aFunction ) ) {
      SetErrorCode( "Import driver failed" );
      return NULL;
    }
    aSeq->Append(anImported);

    // Greate material groups.
    // MakeMaterialGroups( anImported, aSeq );
  }
  catch( Standard_Failure ) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode( aFail->GetMessageString() );
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aSeq << " = geompy.ImportBREP(\"" << theFileName.ToCString() << "\" )";
  SetErrorCode(OK);

  return aSeq;
}
