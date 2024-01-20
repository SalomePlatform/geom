// Copyright (C) 2014-2024  CEA, EDF, OPEN CASCADE
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

#ifndef _GEOM_BREPPlugin_OperationsCreator_HXX_
#define _GEOM_BREPPlugin_OperationsCreator_HXX_

// internal includes
#include "BREPPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_Gen_i.hh"

// C++ includes
#include <map>

class BREPPlugin_IOperations;

//=====================================================================
// Operations creator
//=====================================================================
class BREPPLUGINENGINE_EXPORT BREPPlugin_OperationsCreator : public GEOM_GenericOperationsCreator
{
public:
  BREPPlugin_OperationsCreator();
  ~BREPPlugin_OperationsCreator();

  GEOM_IOperations_i* Create( PortableServer::POA_ptr thePOA,
			                  GEOM::GEOM_Gen_ptr      theEngine,
			                  ::GEOMImpl_Gen*         theGenImpl );
private:
  static BREPPlugin_IOperations* get(::GEOMImpl_Gen* theGenImpl);

private:
  static BREPPlugin_IOperations* _operation;

  friend class BREPPlugin_IECallBack;
};

#endif
