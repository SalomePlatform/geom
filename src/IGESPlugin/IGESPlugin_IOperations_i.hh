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

#ifndef _IGESPlugin_IOperations_i_HeaderFile
#define _IGESPlugin_IOperations_i_HeaderFile

// idl includes
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( GEOM_Gen )
#include CORBA_SERVER_HEADER( IGESPlugin )

// internal includes
#include "IGESPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_IOperations_i.hh"

class IGESPlugin_IOperations;

class IGESPLUGINENGINE_EXPORT IGESPlugin_IOperations_i :
      public virtual POA_GEOM::IIGESOperations,
      public virtual GEOM_IOperations_i
{
public:
  IGESPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
			    GEOM::GEOM_Gen_ptr theEngine,
			    IGESPlugin_IOperations* theImpl );
  ~IGESPlugin_IOperations_i();

  void            ExportIGES( GEOM::GEOM_Object_ptr, const char*, const char* );
  GEOM::ListOfGO* ImportIGES( const char*, const bool );
  char*           ReadValue( const char*, const char* );

  IGESPlugin_IOperations* GetOperations();
};

#endif
