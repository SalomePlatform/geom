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
#include "VTKPlugin_IECallBack.hxx"
#include "VTKPlugin_IOperations.hxx"
#include "VTKPlugin_OperationsCreator.hxx"

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
VTKPlugin_IECallBack::VTKPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
VTKPlugin_IECallBack::~VTKPlugin_IECallBack()
{
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
bool VTKPlugin_IECallBack::Export( int                            theDocId,
				   const Handle(GEOM_Object)      theOriginal,
				   const TCollection_AsciiString& theFileName,
				   const TCollection_AsciiString& theFormatName )
{
  VTKPlugin_IOperations* aPluginOperations = VTKPlugin_OperationsCreator::get( GetEngine(), theDocId );
  const double aDeflection = 0.001;
  aPluginOperations->ExportVTK( theOriginal, theFileName, aDeflection );
  return true;
}
