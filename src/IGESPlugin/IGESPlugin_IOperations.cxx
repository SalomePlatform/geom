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
#include "IGESPlugin_IOperations.hxx"
#include "IGESPlugin_ExportDriver.hxx"
#include "IGESPlugin_ImportDriver.hxx"
#include "IGESPlugin_IExport.hxx"
#include "IGESPlugin_IImport.hxx"

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
IGESPlugin_IOperations::IGESPlugin_IOperations( GEOM_Engine* theEngine, int theDocID )
: GEOMImpl_IBaseIEOperations( theEngine, theDocID )
{
  MESSAGE( "IGESPlugin_IOperations::IGESPlugin_IOperations" );
}

//=============================================================================
/*!
 *  Destructor
 */
//=============================================================================
IGESPlugin_IOperations::~IGESPlugin_IOperations()
{
  MESSAGE( "IGESPlugin_IOperations::~IGESPlugin_IOperations" );
}

//=============================================================================
/*!
 *  ExportIGES
 *  Export a shape to IGES format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the file to exported
 *  \param theIsASCII The format of the exported file (ASCII or Binary)
 *  \param theDeflection The deflection of the shape to exported
 *  \param theVersion The version of IGES format which defines, whether to write
 *                    only faces (5.1 IGES format) or shells and solids also (5.3 IGES format).
 */
//=============================================================================
void IGESPlugin_IOperations::ExportIGES( const Handle(GEOM_Object)      theOriginal,
                                                 const TCollection_AsciiString& theFileName,
                                                 const TCollection_AsciiString& theVersion )
{
  SetErrorCode(KO);
  if( theOriginal.IsNull() ) return;

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if( aRefFunction.IsNull() ) return;  //There is no function which creates an object to be exported

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject( GetDocID(), GEOM_IMPORT);

  //Add an Export function
  Handle(GEOM_Function) aFunction = result->AddFunction( IGESPlugin_ExportDriver::GetID(), EXPORT_SHAPE );
  if( aFunction.IsNull() ) return;

  //Check if the function is set correctly
  if( aFunction->GetDriverGUID() != IGESPlugin_ExportDriver::GetID() ) return;

  //Set parameters
  IGESPlugin_IExport aCI( aFunction );
  aCI.SetOriginal( aRefFunction );
  aCI.SetFileName( theFileName );
  aCI.SetVersion( theVersion );

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
  GEOM::TPythonDump(aFunction) << "geompy.ExportIGES(" << theOriginal << ", \""
    << theFileName.ToCString() << "\", \"" << theVersion.ToCString() << "\" )";

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  ImportIGES
 *  Import a shape from IGES format
 *  \param theFileName The name of the file to import
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
IGESPlugin_IOperations::ImportIGES( const TCollection_AsciiString& theFileName,
                                            const bool theIsIgnoreUnits )
{
  SetErrorCode(KO);
  if( theFileName.IsEmpty() ) return NULL;

  //Add a new result object
  Handle(GEOM_Object) anImported = GetEngine()->AddObject( GetDocID(), GEOM_IMPORT );

  //Add an Import function
  Handle(GEOM_Function) aFunction =
    anImported->AddFunction( IGESPlugin_ImportDriver::GetID(), IMPORT_SHAPE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != IGESPlugin_ImportDriver::GetID()) return NULL;

  //Set parameters
  IGESPlugin_IImport aCI( aFunction );
  aCI.SetFileName( theFileName );
  aCI.SetIsIgnoreUnits( theIsIgnoreUnits );

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
  if( theIsIgnoreUnits )
    pd << aSeq << " = geompy.ImportIGES(\"" << theFileName.ToCString() << "\", True)";
  else
    pd << aSeq << " = geompy.ImportIGES(\"" << theFileName.ToCString() << "\")";
  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
TCollection_AsciiString IGESPlugin_IOperations::ReadValue
                                 ( const TCollection_AsciiString& theFileName,
                                   const TCollection_AsciiString& theParameterName )
{
  SetErrorCode(KO);

  TCollection_AsciiString aValue, anError;

  if (theFileName.IsEmpty() || theParameterName.IsEmpty()) return aValue;

  aValue = IGESPlugin_ImportDriver::GetValue( theFileName, theParameterName, anError );

  if( aValue.IsEmpty() ) {
    if( anError.IsEmpty() )
      anError = theFileName + " doesn't contain requested parameter";
    return aValue;
  }
  if (anError.IsEmpty())
    SetErrorCode(OK);
  else
    SetErrorCode(anError.ToCString());

  return aValue;
}
