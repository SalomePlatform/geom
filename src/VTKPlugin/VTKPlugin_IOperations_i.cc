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
#include "VTKPlugin_IOperations_i.hh"
#include "VTKPlugin_IOperations.hxx"

// KERNEL includes
#include <utilities.h>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
VTKPlugin_IOperations_i::VTKPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
						  GEOM::GEOM_Gen_ptr theEngine,
						  VTKPlugin_IOperations* theImpl )
:GEOM_IOperations_i( thePOA, theEngine, theImpl )
{
  MESSAGE( "VTKPlugin_IOperations_i::VTKPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
VTKPlugin_IOperations_i::~VTKPlugin_IOperations_i()
{
  MESSAGE( "VTKPlugin_IOperations_i::~VTKPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  ExportVTK
 *  Export a shape to VTK format
 *  \param theOriginal The shape to export
 *  \param theFileName The name of the exported file
 *  \param theDeflection The deflection of the exported shape
 */
//=============================================================================
void VTKPlugin_IOperations_i::ExportVTK( GEOM::GEOM_Object_ptr theOriginal,
					 const char*           theFileName,
					 const double          theDeflection )
{
  // duplicate the original shape
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate( theOriginal );

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal = GetObjectImpl( theOriginal );
  if (anOriginal.IsNull()) return;

  //Export the shape to the file
  GetOperations()->ExportVTK( anOriginal, theFileName, theDeflection );
}

VTKPlugin_IOperations* VTKPlugin_IOperations_i::GetOperations()
{
  return (VTKPlugin_IOperations*)GetImpl();
}
