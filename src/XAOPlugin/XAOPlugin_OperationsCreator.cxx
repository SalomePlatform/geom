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
#include "XAOPlugin_OperationsCreator.hxx"
#include "XAOPlugin_IOperations_i.hh"
#include "XAOPlugin_IOperations.hxx"
#include "XAOPlugin_Driver.hxx"
#include "XAOPlugin_IECallBack.hxx"

// KERNEL includes
#include <Utils_ExceptHandlers.hxx>
#include <utilities.h>

// OCCT includes
#include <TFunction_DriverTable.hxx>

std::map <int, XAOPlugin_IOperations*> XAOPlugin_OperationsCreator::_mapOfOperations;

XAOPlugin_OperationsCreator::XAOPlugin_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver( XAOPlugin_Driver::GetID(),
					   new XAOPlugin_Driver() );
  
  // Register callback
  GEOMImpl_IECallBack::Register( "XAO", new XAOPlugin_IECallBack() );
}

XAOPlugin_OperationsCreator::~XAOPlugin_OperationsCreator()
{
}

GEOM_IOperations_i* XAOPlugin_OperationsCreator::Create( PortableServer::POA_ptr thePOA,
                                                         int                     theStudyId,
                                                         GEOM::GEOM_Gen_ptr      theEngine,
                                                         ::GEOMImpl_Gen*         theGenImpl )
{
  Unexpect aCatch( SALOME_SalomeException );
  MESSAGE( "XAOPlugin_OperationsCreator::Create" );
  return new XAOPlugin_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

XAOPlugin_IOperations* XAOPlugin_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
							 int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new XAOPlugin_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
