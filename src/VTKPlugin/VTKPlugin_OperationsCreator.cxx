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
#include "VTKPlugin_OperationsCreator.hxx"
#include "VTKPlugin_IOperations_i.hh"
#include "VTKPlugin_IOperations.hxx"
#include "VTKPlugin_ExportDriver.hxx"
#include "VTKPlugin_IECallBack.hxx"

// KERNEL includes
#include <Utils_ExceptHandlers.hxx>
#include <utilities.h>

// OCCT includes
#include <TFunction_DriverTable.hxx>

std::map <int, VTKPlugin_IOperations*> VTKPlugin_OperationsCreator::_mapOfOperations;

VTKPlugin_OperationsCreator::VTKPlugin_OperationsCreator()
{
  // Register drivers
  TFunction_DriverTable::Get()->AddDriver( VTKPlugin_ExportDriver::GetID(),
					   new VTKPlugin_ExportDriver() );
  
  // Register callback
  VTKPlugin_IECallBack* callback = new VTKPlugin_IECallBack();
  GEOMImpl_IECallBack::Register( "VTK", callback );
}

VTKPlugin_OperationsCreator::~VTKPlugin_OperationsCreator()
{
}

GEOM_IOperations_i* VTKPlugin_OperationsCreator::Create( PortableServer::POA_ptr thePOA,
                                                         int                     theStudyId,
                                                         GEOM::GEOM_Gen_ptr      theEngine,
                                                         ::GEOMImpl_Gen*         theGenImpl )
{
  Unexpect aCatch( SALOME_SalomeException );
  MESSAGE( "VTKPlugin_OperationsCreator::Create" );
  return new VTKPlugin_IOperations_i( thePOA, theEngine, get( theGenImpl, theStudyId ) );
}

VTKPlugin_IOperations* VTKPlugin_OperationsCreator::get( ::GEOMImpl_Gen* theGenImpl,
							 int             theStudyId )
{
  if (_mapOfOperations.find( theStudyId ) == _mapOfOperations.end() )
    _mapOfOperations[theStudyId] = new VTKPlugin_IOperations( theGenImpl, theStudyId );
  return _mapOfOperations[theStudyId];
}
