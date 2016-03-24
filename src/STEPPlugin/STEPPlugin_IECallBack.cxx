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
#include "STEPPlugin_IECallBack.hxx"
#include "STEPPlugin_IOperations.hxx"
#include "STEPPlugin_OperationsCreator.hxx"

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
STEPPlugin_IECallBack::STEPPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
STEPPlugin_IECallBack::~STEPPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool
STEPPlugin_IECallBack::Export( int                            theDocId,
			       const Handle(GEOM_Object)      theOriginal,
			       const TCollection_AsciiString& theFileName,
			       const TCollection_AsciiString& theFormatName )
{
  STEPPlugin_IOperations* aPluginOperations = STEPPlugin_OperationsCreator::get( GetEngine(), theDocId );
  const STEPPlugin_IOperations::LengthUnit aUnit = STEPPlugin_IOperations::LengthUnit_Meter;

  aPluginOperations->ExportSTEP( theOriginal, theFileName, aUnit );
  return true;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
STEPPlugin_IECallBack::Import( int                            theDocId,
			       const TCollection_AsciiString& theFormatName,
			       const TCollection_AsciiString& theFileName )
{
  STEPPlugin_IOperations* aPluginOperations = STEPPlugin_OperationsCreator::get( GetEngine(), theDocId );
  bool anIsIgnoreUnits = ( theFormatName == "STEP_SCALE" ) ? true : false;
  return aPluginOperations->ImportSTEP( theFileName, anIsIgnoreUnits, false );
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
TCollection_AsciiString
STEPPlugin_IECallBack::ReadValue( int                            theDocId,
				  const TCollection_AsciiString& theFileName,
				  const TCollection_AsciiString& theFormatName,
				  const TCollection_AsciiString& theParameterName )
{
  STEPPlugin_IOperations* aPluginOperations = STEPPlugin_OperationsCreator::get( GetEngine(), theDocId );
  return aPluginOperations->ReadValue( theFileName, theParameterName );
}
