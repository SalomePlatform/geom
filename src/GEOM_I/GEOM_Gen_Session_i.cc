// Copyright (C) 2021  CEA/DEN, EDF R&D
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

#include "GEOM_Gen_Session_i.hh"

GEOM_Gen_Session_i::GEOM_Gen_Session_i(CORBA::ORB_ptr orb,
                        PortableServer::POA_ptr poa,
                        PortableServer::ObjectId *contId,
                        const char *instanceName,
                        const char *interfaceName):GEOM_Gen_i(orb,poa,contId,instanceName,interfaceName,true)
{
    name_service = new SALOME_NamingService(_orb);
}

GEOM_Gen_Session_i::~GEOM_Gen_Session_i()
{
  delete name_service;
}

void GEOM_Gen_Session_i::register_name(char * name)
{
  GEOM::GEOM_Gen_var g = _this();
  name_service->Register(g, name);
}

SALOMEDS::Study_var GEOM_Gen_Session_i::getStudyServant()
{
  static SALOMEDS::Study_var aStudy;
  if(CORBA::is_nil(aStudy))
  {
    CORBA::Object_ptr anObject = name_service->Resolve("/Study");
    aStudy = SALOMEDS::Study::_narrow(anObject);
  }
  return aStudy;
}

#include "GEOM_Gen_No_Session_i.hh"

extern "C"
{
  GEOM_I_EXPORT  PortableServer::ObjectId* GEOMEngine_factory(CORBA::ORB_ptr orb,
                                               PortableServer::POA_ptr       poa,
                                               PortableServer::ObjectId*     contId,
                                               const char*                   instanceName,
                                               const char*                   interfaceName)
  {
    CORBA::Object_var o = poa->id_to_reference(*contId);
    Engines::Container_var cont = Engines::Container::_narrow(o);
    if(cont->is_SSL_mode())
    {
      GEOM_Gen_No_Session_i* myGEOM_Gen_i = new GEOM_Gen_No_Session_i(orb, poa, contId, instanceName, interfaceName);
      return myGEOM_Gen_i->getId();
    }
    else
    {
      GEOM_Gen_Session_i* myGEOM_Gen_i = new GEOM_Gen_Session_i(orb, poa, contId, instanceName, interfaceName);
      return myGEOM_Gen_i->getId();
    }
  }
}
