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

#ifndef _STEPPlugin_IOperations_i_HeaderFile
#define _STEPPlugin_IOperations_i_HeaderFile

// idl includes
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( GEOM_Gen )
#include CORBA_SERVER_HEADER( STEPPlugin )

// internal includes
#include "STEPPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_IOperations_i.hh"

class STEPPlugin_IOperations;

class STEPPLUGINENGINE_EXPORT STEPPlugin_IOperations_i :
      public virtual POA_GEOM::ISTEPOperations,
      public virtual GEOM_IOperations_i
{
public:
  STEPPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
			    GEOM::GEOM_Gen_ptr theEngine,
			    STEPPlugin_IOperations* theImpl );
  ~STEPPlugin_IOperations_i();

  void            ExportSTEP( GEOM::GEOM_Object_ptr, const char*,
                              GEOM::length_unit );
  GEOM::ListOfGO* ImportSTEP( const char*, const bool, const bool );
  char*           ReadValue( const char*, const char* );

  STEPPlugin_IOperations* GetOperations();
};

#endif
