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
#include "IGESPlugin_IECallBack.hxx"
#include "IGESPlugin_IOperations.hxx"
#include "IGESPlugin_OperationsCreator.hxx"

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
IGESPlugin_IECallBack::IGESPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
IGESPlugin_IECallBack::~IGESPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool IGESPlugin_IECallBack::Export( int                            theDocId,
				    const Handle(GEOM_Object)      theOriginal,
				    const TCollection_AsciiString& theFileName,
				    const TCollection_AsciiString& theFormatName )
{
  IGESPlugin_IOperations* aPluginOperations = IGESPlugin_OperationsCreator::get( GetEngine(), theDocId );
  TCollection_AsciiString aVersion = ( theFormatName == "IGES_5_3" ) ? "5.3" : "5.1";
  aPluginOperations->ExportIGES( theOriginal, theFileName, aVersion );
  return true;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
IGESPlugin_IECallBack::Import( int                            theDocId,
			       const TCollection_AsciiString& theFormatName,
			       const TCollection_AsciiString& theFileName )
{
  IGESPlugin_IOperations* aPluginOperations = IGESPlugin_OperationsCreator::get( GetEngine(), theDocId );
  bool anIsIgnoreUnits = ( theFormatName == "IGES_SCALE" ) ? true : false;
  return aPluginOperations->ImportIGES( theFileName, anIsIgnoreUnits );
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
TCollection_AsciiString
IGESPlugin_IECallBack::ReadValue( int                            theDocId,
				  const TCollection_AsciiString& theFileName,
				  const TCollection_AsciiString& theFormatName,
				  const TCollection_AsciiString& theParameterName )
{
  IGESPlugin_IOperations* aPluginOperations = IGESPlugin_OperationsCreator::get( GetEngine(), theDocId );
  return aPluginOperations->ReadValue( theFileName, theParameterName );
}
