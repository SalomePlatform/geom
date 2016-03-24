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

#ifndef _BREPPlugin_IOperations_i_HeaderFile
#define _BREPPlugin_IOperations_i_HeaderFile

// idl includes
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( GEOM_Gen )
#include CORBA_SERVER_HEADER( BREPPlugin )

// internal includes
#include "BREPPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_IOperations_i.hh"

class BREPPlugin_IOperations;

class BREPPLUGINENGINE_EXPORT BREPPlugin_IOperations_i :
      public virtual POA_GEOM::IBREPOperations,
      public virtual GEOM_IOperations_i
{
public:
  BREPPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
			    GEOM::GEOM_Gen_ptr theEngine,
			    BREPPlugin_IOperations* theImpl );
  ~BREPPlugin_IOperations_i();

  void            ExportBREP( GEOM::GEOM_Object_ptr, const char* );
  GEOM::ListOfGO* ImportBREP( const char* );

  BREPPlugin_IOperations* GetOperations();
};

#endif // _BREPPlugin_IOperations_i_HeaderFile
