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

#ifndef _XAOPlugin_IOperations_i_HeaderFile
#define _XAOPlugin_IOperations_i_HeaderFile

// idl includes
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( GEOM_Gen )
#include CORBA_SERVER_HEADER( XAOPlugin )

// internal includes
#include "XAOPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_IOperations_i.hh"

class XAOPlugin_IOperations;

class XAOPLUGINENGINE_EXPORT XAOPlugin_IOperations_i :
      public virtual POA_GEOM::IXAOOperations,
      public virtual GEOM_IOperations_i
{
public:
  XAOPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
			   GEOM::GEOM_Gen_ptr theEngine,
			   XAOPlugin_IOperations* theImpl );
  ~XAOPlugin_IOperations_i();

  CORBA::Boolean ExportXAO( GEOM::GEOM_Object_ptr shape,
                            const GEOM::ListOfGO& groups,
                            const GEOM::ListOfFields& fields,
                            const char* author,
                            const char* fileName );

  CORBA::Boolean ImportXAO( const char* fileName,
                            GEOM::GEOM_Object_out shape,
                            GEOM::ListOfGO_out subShapes,
                            GEOM::ListOfGO_out groups,
                            GEOM::ListOfFields_out fields );

  XAOPlugin_IOperations* GetOperations();
};

#endif
