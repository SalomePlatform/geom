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
#include "BREPPlugin_IECallBack.hxx"
#include "BREPPlugin_IOperations.hxx"
#include "BREPPlugin_OperationsCreator.hxx"

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
BREPPlugin_IECallBack::BREPPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
BREPPlugin_IECallBack::~BREPPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool 
BREPPlugin_IECallBack::Export( int                            theDocId,
			       const Handle(GEOM_Object)      theOriginal,
			       const TCollection_AsciiString& theFileName,
			       const TCollection_AsciiString& theFormatName )
{
  BREPPlugin_IOperations* aPluginOperations = BREPPlugin_OperationsCreator::get( GetEngine(), theDocId );
  aPluginOperations->ExportBREP( theOriginal, theFileName );
  return true;
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
BREPPlugin_IECallBack::Import( int                            theDocId,
			       const TCollection_AsciiString& theFormatName,
			       const TCollection_AsciiString& theFileName )
{
  BREPPlugin_IOperations* aPluginOperations = BREPPlugin_OperationsCreator::get( GetEngine(), theDocId );
  return aPluginOperations->ImportBREP( theFileName );
}

