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

#ifndef _GEOM_IGESPlugin_OperationsCreator_HXX_
#define _GEOM_IGESPlugin_OperationsCreator_HXX_

// internal includes
#include "IGESPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_Gen_i.hh"

// C++ includes
#include <map>

class IGESPlugin_IOperations;

//=====================================================================
// Operations creator
//=====================================================================
class IGESPLUGINENGINE_EXPORT IGESPlugin_OperationsCreator : public GEOM_GenericOperationsCreator
{
public:
  IGESPlugin_OperationsCreator();
  ~IGESPlugin_OperationsCreator();

  GEOM_IOperations_i* Create( PortableServer::POA_ptr thePOA,
			      int                     theStudyId,
			      GEOM::GEOM_Gen_ptr      theEngine,
			      ::GEOMImpl_Gen*         theGenImpl );
private:
  static IGESPlugin_IOperations* get( ::GEOMImpl_Gen*         theGenImpl,
				      int                     theStudyId );

private:
  static std::map <int, IGESPlugin_IOperations*>  _mapOfOperations;

  friend class IGESPlugin_IECallBack;
};

#endif
