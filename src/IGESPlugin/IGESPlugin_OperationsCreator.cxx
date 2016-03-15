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
#include "IGESPlugin_OperationsCreator.hxx"
#include "IGESPlugin_IOperations_i.hh"
#include "IGESPlugin_IOperations.hxx"
#include "IGESPlugin_ExportDriver.hxx"
#include "IGESPlugin_ImportDriver.hxx"
#include "IGESPlugin_IECallBack.hxx"

// KERNEL includes
#include <Utils_ExceptHandlers.hxx>
#include <utilities.h>

// OCCT includes
#include <TFunction_DriverTable.hxx>

std::map <int, IGESPlugin_IOperations*> IGESPlugin_OperationsCreator::_mapOfOperations;

IGESPlugin_OperationsCreator::IGESPlugin_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver( IGESPlugin_ExportDriver::GetID(),
					   new IGESPlugin_ExportDriver() );
  TFunction_DriverTable::Get()->AddDriver( IGESPlugin_ImportDriver::GetID(),
					   new IGESPlugin_ImportDriver() );
  
  // Register callback
  IGESPlugin_IECallBack* callback = new IGESPlugin_IECallBack();
  GEOMImpl_IECallBack::Register( "IGES",       callback );
  GEOMImpl_IECallBack::Register( "IGES_5_3",   callback );
  GEOMImpl_IECallBack::Register( "IGES_SCALE", callback );
}

IGESPlugin_OperationsCreator::~IGESPlugin_OperationsCreator()
{
}

GEOM_IOperations_i* IGESPlugin_OperationsCreator::Create( PortableServer::POA_ptr thePOA,
                                                          int                     theStudyId,
                                                          GEOM::GEOM_Gen_ptr      theEngine,
                                                          ::GEOMImpl_Gen*         theGenImpl )
{
  Unexpect aCatch( SALOME_SalomeException );
  MESSAGE( "IGESPlugin_OperationsCreator::Create" );
  return new IGESPlugin_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

IGESPlugin_IOperations* IGESPlugin_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
							   int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new IGESPlugin_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
