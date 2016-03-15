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

#ifndef _VTKPlugin_IOperations_i_HeaderFile
#define _VTKPlugin_IOperations_i_HeaderFile

// idl includes
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER( GEOM_Gen )
#include CORBA_SERVER_HEADER( VTKPlugin )

// internal includes
#include "VTKPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_IOperations_i.hh"

class VTKPlugin_IOperations;

class VTKPLUGINENGINE_EXPORT VTKPlugin_IOperations_i :
      public virtual POA_GEOM::IVTKOperations,
      public virtual GEOM_IOperations_i
{
public:
  VTKPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
			   GEOM::GEOM_Gen_ptr theEngine,
			   VTKPlugin_IOperations* theImpl );
  ~VTKPlugin_IOperations_i();

  void ExportVTK( GEOM::GEOM_Object_ptr, const char*, const double );

  VTKPlugin_IOperations* GetOperations();
};

#endif
