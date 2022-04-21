// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "GEOM_ITestOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_ITestOperations_i::GEOM_ITestOperations_i (PortableServer::POA_ptr thePOA,
						GEOM::GEOM_Gen_ptr theEngine,
						::GEOMImpl_ITestOperations* theImpl)
  :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_ITestOperations_i::GEOM_ITestOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_ITestOperations_i::~GEOM_ITestOperations_i()
{
  MESSAGE("GEOM_ITestOperations_i::~GEOM_ITestOperations_i");
}


//=============================================================================
/*!
 *  Tesselate
 */
//=============================================================================
CORBA::Boolean GEOM_ITestOperations_i::Tesselate(GEOM::GEOM_Object_ptr theShape,
						 CORBA::Double theLinearDeflection,
						 CORBA::Boolean theIsRelative,
						 CORBA::Double theAngularDeflection)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the source object
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return false;

  // Tesselate
  return GetOperations()->Tesselate(aShape, theLinearDeflection, theIsRelative, theAngularDeflection);
}
