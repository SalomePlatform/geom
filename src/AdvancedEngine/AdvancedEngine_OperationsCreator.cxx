// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "AdvancedEngine_OperationsCreator.hxx"
#include "AdvancedEngine_IOperations_i.hh"
#include "AdvancedEngine_IOperations.hxx"
#include <AdvancedEngine_PipeTShapeDriver.hxx>
#include <AdvancedEngine_DividedDiskDriver.hxx>
#include <AdvancedEngine_SmoothingSurfaceDriver.hxx>

#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>

#include "Utils_ExceptHandlers.hxx"
#include "utilities.h"

std::map <int, AdvancedEngine_IOperations*>  AdvancedEngine_OperationsCreator::_mapOfOperations;

AdvancedEngine_OperationsCreator::AdvancedEngine_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver(AdvancedEngine_PipeTShapeDriver::GetID(),
					  new AdvancedEngine_PipeTShapeDriver());
  TFunction_DriverTable::Get()->AddDriver(AdvancedEngine_DividedDiskDriver::GetID(),
					  new AdvancedEngine_DividedDiskDriver());
  TFunction_DriverTable::Get()->AddDriver(AdvancedEngine_SmoothingSurfaceDriver::GetID(),
					  new AdvancedEngine_SmoothingSurfaceDriver());
}

AdvancedEngine_OperationsCreator::~AdvancedEngine_OperationsCreator()
{
}

GEOM_IOperations_i* AdvancedEngine_OperationsCreator::Create (PortableServer::POA_ptr thePOA,
                                                              int                     theStudyId,
                                                              GEOM::GEOM_Gen_ptr      theEngine,
                                                              ::GEOMImpl_Gen*         theGenImpl)
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "AdvancedEngine_OperationsCreator::Create" );
  return new AdvancedEngine_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

AdvancedEngine_IOperations* AdvancedEngine_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
								   int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new AdvancedEngine_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
