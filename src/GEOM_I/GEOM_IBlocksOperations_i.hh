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

#ifndef _GEOM_IBlocksOperations_i_HeaderFile
#define _GEOM_IBlocksOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBlocksOperations.hxx"

class GEOM_I_EXPORT GEOM_IBlocksOperations_i :
    public virtual POA_GEOM::GEOM_IBlocksOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IBlocksOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IBlocksOperations* theImpl);
  ~GEOM_IBlocksOperations_i();

  // Creation of blocks and block faces
  GEOM::GEOM_Object_ptr MakeQuad (GEOM::GEOM_Object_ptr theEdge1,
				  GEOM::GEOM_Object_ptr theEdge2,
				  GEOM::GEOM_Object_ptr theEdge3,
				  GEOM::GEOM_Object_ptr theEdge4);

  GEOM::GEOM_Object_ptr MakeQuad2Edges (GEOM::GEOM_Object_ptr theEdge1,
					GEOM::GEOM_Object_ptr theEdge2);

  GEOM::GEOM_Object_ptr MakeQuad4Vertices (GEOM::GEOM_Object_ptr thePnt1,
					   GEOM::GEOM_Object_ptr thePnt2,
					   GEOM::GEOM_Object_ptr thePnt3,
					   GEOM::GEOM_Object_ptr thePnt4);

  GEOM::GEOM_Object_ptr MakeHexa (GEOM::GEOM_Object_ptr theFace1,
				  GEOM::GEOM_Object_ptr theFace2,
				  GEOM::GEOM_Object_ptr theFace3,
				  GEOM::GEOM_Object_ptr theFace4,
				  GEOM::GEOM_Object_ptr theFace5,
				  GEOM::GEOM_Object_ptr theFace6);

  GEOM::GEOM_Object_ptr MakeHexa2Faces (GEOM::GEOM_Object_ptr theFace1,
					GEOM::GEOM_Object_ptr theFace2);

  GEOM::GEOM_Object_ptr MakeBlockCompound (GEOM::GEOM_Object_ptr theCompound);

  // Extract elements of blocks and blocks compounds
  GEOM::GEOM_Object_ptr GetPoint (GEOM::GEOM_Object_ptr theShape,
				  CORBA::Double   theX,
				  CORBA::Double   theY,
				  CORBA::Double   theZ,
				  CORBA::Double   theEpsilon);

  GEOM::GEOM_Object_ptr GetVertexNearPoint (GEOM::GEOM_Object_ptr theShape,
                                            GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr GetEdge (GEOM::GEOM_Object_ptr theShape,
				 GEOM::GEOM_Object_ptr thePoint1,
				 GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr GetEdgeNearPoint (GEOM::GEOM_Object_ptr theShape,
					  GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr GetFaceByPoints (GEOM::GEOM_Object_ptr theShape,
					 GEOM::GEOM_Object_ptr thePoint1,
					 GEOM::GEOM_Object_ptr thePoint2,
					 GEOM::GEOM_Object_ptr thePoint3,
					 GEOM::GEOM_Object_ptr thePoint4);

  GEOM::GEOM_Object_ptr GetFaceByEdges (GEOM::GEOM_Object_ptr theShape,
					GEOM::GEOM_Object_ptr theEdge1,
					GEOM::GEOM_Object_ptr theEdge2);

  GEOM::GEOM_Object_ptr GetOppositeFace (GEOM::GEOM_Object_ptr theBlock,
					 GEOM::GEOM_Object_ptr theFace);

  GEOM::GEOM_Object_ptr GetFaceNearPoint (GEOM::GEOM_Object_ptr theShape,
					  GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr GetFaceByNormale (GEOM::GEOM_Object_ptr theBlock,
					  GEOM::GEOM_Object_ptr theVector);

  GEOM::GEOM_Object_ptr GetShapesNearPoint (GEOM::GEOM_Object_ptr theShape,
                                            GEOM::GEOM_Object_ptr thePoint,
                                            CORBA::Long           theShapeType,
                                            CORBA::Double         theTolerance);

  // Check blocks compound
  CORBA::Boolean IsCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
				     CORBA::Long           theMinNbFaces,
				     CORBA::Long           theMaxNbFaces,
				     CORBA::Long&          theNbBlocks);

  CORBA::Boolean CheckCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
                                        const CORBA::Double   theToleranceC1,
					GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors);

  char* PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
		       const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors);

  GEOM::GEOM_Object_ptr GetNonBlocks (GEOM::GEOM_Object_ptr theShape,
                                      const CORBA::Double   theToleranceC1,
                                      GEOM::GEOM_Object_out theNonQuads);

  GEOM::GEOM_Object_ptr RemoveExtraEdges (GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Long           theOptimumNbFaces);

  GEOM::GEOM_Object_ptr UnionFaces (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_Object_ptr CheckAndImprove (GEOM::GEOM_Object_ptr theCompound);

  // Extract blocks from blocks compounds
  GEOM::ListOfGO* ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
					   CORBA::Long     theMinNbFaces,
					   CORBA::Long     theMaxNbFaces);

  GEOM::GEOM_Object_ptr GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
					   GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
					 const GEOM::ListOfGO& theParts);

  GEOM::ListOfGO* GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
				    const GEOM::ListOfGO& theParts);

  // Operations on blocks with gluing of result
  GEOM::GEOM_Object_ptr MakeMultiTransformation1D (GEOM::GEOM_Object_ptr theBlock,
						   CORBA::Long     theDirFace1,
						   CORBA::Long     theDirFace2,
						   CORBA::Long     theNbTimes);

  GEOM::GEOM_Object_ptr MakeMultiTransformation2D (GEOM::GEOM_Object_ptr theBlock,
						   CORBA::Long     theDirFace1U,
						   CORBA::Long     theDirFace2U,
						   CORBA::Long     theNbTimesU,
						   CORBA::Long     theDirFace1V,
						   CORBA::Long     theDirFace2V,
						   CORBA::Long     theNbTimesV);

  // Build groups for Propagation of 1D hypotheses
  GEOM::ListOfGO* Propagate (GEOM::GEOM_Object_ptr theShape);

  ::GEOMImpl_IBlocksOperations* GetOperations()
  { return (::GEOMImpl_IBlocksOperations*)GetImpl(); }
};

#endif
