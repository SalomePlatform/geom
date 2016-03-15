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
#include "STEPPlugin_OperationsCreator.hxx"
#include "STEPPlugin_IOperations_i.hh"
#include "STEPPlugin_IOperations.hxx"
#include "STEPPlugin_ExportDriver.hxx"
#include "STEPPlugin_ImportDriver.hxx"
#include "STEPPlugin_IECallBack.hxx"

// KERNEL includes
#include <Utils_ExceptHandlers.hxx>
#include <utilities.h>

// GEOM includes
#include "GEOMImpl_IECallBack.hxx"

// OCCT includes
#include <TFunction_DriverTable.hxx>

std::map <int, STEPPlugin_IOperations*> STEPPlugin_OperationsCreator::_mapOfOperations;

STEPPlugin_OperationsCreator::STEPPlugin_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver( STEPPlugin_ExportDriver::GetID(),
					   new STEPPlugin_ExportDriver() );
  TFunction_DriverTable::Get()->AddDriver( STEPPlugin_ImportDriver::GetID(),
					   new STEPPlugin_ImportDriver() );
  
  // Register callback
  STEPPlugin_IECallBack* callback = new STEPPlugin_IECallBack();
  GEOMImpl_IECallBack::Register( "STEP",       callback );
  GEOMImpl_IECallBack::Register( "STEP_SCALE", callback );
}

STEPPlugin_OperationsCreator::~STEPPlugin_OperationsCreator()
{
}

GEOM_IOperations_i* STEPPlugin_OperationsCreator::Create( PortableServer::POA_ptr thePOA,
                                                          int                     theStudyId,
                                                          GEOM::GEOM_Gen_ptr      theEngine,
                                                          ::GEOMImpl_Gen*         theGenImpl )
{
  Unexpect aCatch( SALOME_SalomeException );
  MESSAGE( "STEPPlugin_OperationsCreator::Create" );
  return new STEPPlugin_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

STEPPlugin_IOperations* STEPPlugin_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
							   int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new STEPPlugin_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
