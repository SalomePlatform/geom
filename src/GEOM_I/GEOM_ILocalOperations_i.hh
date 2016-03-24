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

#ifndef _GEOM_ILocalOperations_i_HeaderFile
#define _GEOM_ILocalOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ILocalOperations.hxx"

class GEOM_I_EXPORT GEOM_ILocalOperations_i : 
    public virtual POA_GEOM::GEOM_ILocalOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_ILocalOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			   ::GEOMImpl_ILocalOperations* theImpl);
  ~GEOM_ILocalOperations_i();

  GEOM::GEOM_Object_ptr MakeFilletAll   (GEOM::GEOM_Object_ptr theShape,
					 CORBA::Double theR);

  GEOM::GEOM_Object_ptr MakeFilletEdges (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
					 const GEOM::ListOfLong& theEdges);

  GEOM::GEOM_Object_ptr MakeFilletEdgesR1R2 (GEOM::GEOM_Object_ptr theShape,
                                             CORBA::Double theR1,
                                             CORBA::Double theR2,
					     const GEOM::ListOfLong& theEdges);

  GEOM::GEOM_Object_ptr MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
					 const GEOM::ListOfLong& theFaces);

  GEOM::GEOM_Object_ptr MakeFilletFacesR1R2 (GEOM::GEOM_Object_ptr theShape,
                                             CORBA::Double theR1,
                                             CORBA::Double theR2,
				      	     const GEOM::ListOfLong& theFaces);
					     
  GEOM::GEOM_Object_ptr MakeFillet2D (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
				      const GEOM::ListOfLong& theVertexes);

  GEOM::GEOM_Object_ptr MakeFillet1D (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
				      const GEOM::ListOfLong& theVertexes,
                                      CORBA::Boolean doIgnoreSecantVertices);

  GEOM::GEOM_Object_ptr MakeChamferAll   (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD);

  GEOM::GEOM_Object_ptr MakeChamferEdge  (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  CORBA::Long theFace1, CORBA::Long theFace2);

  GEOM::GEOM_Object_ptr MakeChamferEdgeAD  (GEOM::GEOM_Object_ptr theShape,
					    CORBA::Double theD, CORBA::Double theAngle,
					    CORBA::Long theFace1, CORBA::Long theFace2);

  GEOM::GEOM_Object_ptr MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  const GEOM::ListOfLong& theFaces);

  GEOM::GEOM_Object_ptr MakeChamferFacesAD (GEOM::GEOM_Object_ptr theShape,
					    CORBA::Double theD, CORBA::Double theAngle,
					    const GEOM::ListOfLong& theFaces);

  GEOM::GEOM_Object_ptr MakeChamferEdges (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  const GEOM::ListOfLong& theEdges);

  GEOM::GEOM_Object_ptr MakeChamferEdgesAD (GEOM::GEOM_Object_ptr theShape,
					    CORBA::Double theD, CORBA::Double theAngle,
					    const GEOM::ListOfLong& theEdges);

  GEOM::GEOM_Object_ptr MakeArchimede (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Double theWeight,
				       CORBA::Double theWaterDensity,
				       CORBA::Double theMeshingDeflection);

  CORBA::Long GetSubShapeIndex (GEOM::GEOM_Object_ptr theShape,
				GEOM::GEOM_Object_ptr theSubShape);

  ::GEOMImpl_ILocalOperations* GetOperations()
  { return (::GEOMImpl_ILocalOperations*)GetImpl(); }
};

#endif
