// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _GEOM_ADVANCEDENGINE_OPERATIONSCREATOR_HXX_
#define _GEOM_ADVANCEDENGINE_OPERATIONSCREATOR_HXX_

#include "AdvancedEngine.hxx"

#include "GEOM_Gen_i.hh"

#include <map>

class AdvancedEngine_IOperations;

//=====================================================================
// Operations creator
//=====================================================================
class ADVANCEDENGINE_EXPORT AdvancedEngine_OperationsCreator : public GEOM_GenericOperationsCreator
{
public:
  AdvancedEngine_OperationsCreator();
  ~AdvancedEngine_OperationsCreator();

  GEOM_IOperations_i* Create (PortableServer::POA_ptr thePOA,
			      int                     theStudyId,
			      GEOM::GEOM_Gen_ptr      theEngine,
			      ::GEOMImpl_Gen*         theGenImpl);

private:
  static AdvancedEngine_IOperations* get( ::GEOMImpl_Gen* theGenImpl,
					  int             theStudyId );
private:
  static std::map <int, AdvancedEngine_IOperations*>  _mapOfOperations;
};

#endif
