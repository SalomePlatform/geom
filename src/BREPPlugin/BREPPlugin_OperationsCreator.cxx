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
#include "BREPPlugin_OperationsCreator.hxx"
#include "BREPPlugin_IOperations_i.hh"
#include "BREPPlugin_IOperations.hxx"
#include "BREPPlugin_ExportDriver.hxx"
#include "BREPPlugin_ImportDriver.hxx"
#include "BREPPlugin_IECallBack.hxx"

// KERNEL includes
#include <Utils_ExceptHandlers.hxx>
#include <utilities.h>

// OCCT includes
#include <TFunction_DriverTable.hxx>

std::map <int, BREPPlugin_IOperations*> BREPPlugin_OperationsCreator::_mapOfOperations;

BREPPlugin_OperationsCreator::BREPPlugin_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver( BREPPlugin_ExportDriver::GetID(),
					   new BREPPlugin_ExportDriver() );
  TFunction_DriverTable::Get()->AddDriver( BREPPlugin_ImportDriver::GetID(),
					   new BREPPlugin_ImportDriver() );
  // Register callback
  BREPPlugin_IECallBack* callback = new BREPPlugin_IECallBack();
  GEOMImpl_IECallBack::Register( "BREP", callback );
}

BREPPlugin_OperationsCreator::~BREPPlugin_OperationsCreator()
{
}

GEOM_IOperations_i* BREPPlugin_OperationsCreator::Create( PortableServer::POA_ptr thePOA,
                                                          int                     theStudyId,
                                                          GEOM::GEOM_Gen_ptr      theEngine,
                                                          ::GEOMImpl_Gen*         theGenImpl )
{
  Unexpect aCatch( SALOME_SalomeException );
  MESSAGE( "BREPPlugin_OperationsCreator::Create" );
  return new BREPPlugin_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

BREPPlugin_IOperations* BREPPlugin_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
							   int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new BREPPlugin_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
