// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef _GEOM_IBooleanOperations_i_HeaderFile
#define _GEOM_IBooleanOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBooleanOperations.hxx"

class GEOM_I_EXPORT GEOM_IBooleanOperations_i : 
    public virtual POA_GEOM::GEOM_IBooleanOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IBooleanOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IBooleanOperations* theImpl);
  ~GEOM_IBooleanOperations_i();

  GEOM::GEOM_Object_ptr MakeBoolean (GEOM::GEOM_Object_ptr theShape1,
				     GEOM::GEOM_Object_ptr theShape2,
				     CORBA::Long           theOp);

  GEOM::GEOM_Object_ptr MakePartition (const GEOM::ListOfGO&   theShapes,
				       const GEOM::ListOfGO&   theTools,
				       const GEOM::ListOfGO&   theKeepInside,
				       const GEOM::ListOfGO&   theRemoveInside,
				       CORBA::Short            theLimit,
				       CORBA::Boolean          theRemoveWebs,
				       const GEOM::ListOfLong& theMaterials,
				       CORBA::Short theKeepNonlimitShapes);

  GEOM::GEOM_Object_ptr MakePartitionNonSelfIntersectedShape (const GEOM::ListOfGO&   theShapes,
							      const GEOM::ListOfGO&   theTools,
							      const GEOM::ListOfGO&   theKeepInside,
							      const GEOM::ListOfGO&   theRemoveInside,
							      CORBA::Short            theLimit,
							      CORBA::Boolean          theRemoveWebs,
							      const GEOM::ListOfLong& theMaterials,
							      CORBA::Short theKeepNonlimitShapes);


  GEOM::GEOM_Object_ptr MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
					   GEOM::GEOM_Object_ptr thePlane);

  ::GEOMImpl_IBooleanOperations* GetOperations()
  { return (::GEOMImpl_IBooleanOperations*)GetImpl(); }
};

#endif
