// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOM_AdvancedEngine.hxx"

#include "AdvancedEngine_OperationsCreator.hh"

#include "GEOM_IAdvancedOperations_i.hh"

// Operations
#include <GEOMImpl_PipeTShapeDriver.hxx>
#include <GEOMImpl_DividedDiskDriver.hxx>
// #include <GEOMImpl_DividedCylinderDriver.hxx>
#include <GEOMImpl_SmoothingSurfaceDriver.hxx>
/*@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@*/

#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

#include "Utils_ExceptHandlers.hxx"
#include "utilities.h"

//============================================================================
// function : Create
// purpose  :
//============================================================================
GEOM_IOperations_i* AdvancedEngine_OperationsCreator::Create (PortableServer::POA_ptr thePOA,
                                                              int                     theStudyId,
                                                              GEOM::GEOM_Gen_ptr      theEngine,
                                                              ::GEOMImpl_Gen*         theGenImpl)
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "AdvancedEngine_OperationsCreator::Create" );

  if (_mapOfOperations.find(theStudyId) == _mapOfOperations.end()) {
    _mapOfOperations[theStudyId] = new GEOMImpl_IAdvancedOperations (theGenImpl, theStudyId);

    // Advanced operations
    TFunction_DriverTable::Get()->AddDriver(GEOMImpl_PipeTShapeDriver::GetID(),
                                            new GEOMImpl_PipeTShapeDriver());
    TFunction_DriverTable::Get()->AddDriver(GEOMImpl_DividedDiskDriver::GetID(),
                                            new GEOMImpl_DividedDiskDriver());
    //TFunction_DriverTable::Get()->AddDriver(GEOMImpl_DividedCylinderDriver::GetID(),
    //                                        new GEOMImpl_DividedCylinderDriver());
    TFunction_DriverTable::Get()->AddDriver(GEOMImpl_SmoothingSurfaceDriver::GetID(),
                                            new GEOMImpl_SmoothingSurfaceDriver());
    /*@@ insert new functions before this line @@ do not remove this line @@ do not remove this line @@*/
  }

  GEOM_IAdvancedOperations_i* aServant =
    new GEOM_IAdvancedOperations_i (thePOA, theEngine, _mapOfOperations[theStudyId]);

  return aServant;
}
