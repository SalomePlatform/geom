
#ifndef _GEOM_IBlocksOperations_i_HeaderFile
#define _GEOM_IBlocksOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBlocksOperations.hxx"

class GEOM_IBlocksOperations_i :
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
				  const CORBA::Double   theX,
				  const CORBA::Double   theY,
				  const CORBA::Double   theZ,
				  const CORBA::Double   theEpsilon);

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

  // Check blocks compound
  CORBA::Boolean IsCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
				     const CORBA::Long     theMinNbFaces,
				     const CORBA::Long     theMaxNbFaces,
				     CORBA::Long&          theNbBlocks);

  CORBA::Boolean CheckCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
					GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors);

  char* PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
		       const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors);

  // Extract blocks from blocks compounds
  GEOM::ListOfGO* ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
					   const CORBA::Long     theMinNbFaces,
					   const CORBA::Long     theMaxNbFaces);

  GEOM::GEOM_Object_ptr GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
					   GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
					 const GEOM::ListOfGO& theParts);

  GEOM::ListOfGO* GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
				    const GEOM::ListOfGO& theParts);

  // Operations on blocks with gluing of result
  GEOM::GEOM_Object_ptr MakeMultiTransformation1D (GEOM::GEOM_Object_ptr theBlock,
						   const CORBA::Long     theDirFace1,
						   const CORBA::Long     theDirFace2,
						   const CORBA::Long     theNbTimes);

  GEOM::GEOM_Object_ptr MakeMultiTransformation2D (GEOM::GEOM_Object_ptr theBlock,
						   const CORBA::Long     theDirFace1U,
						   const CORBA::Long     theDirFace2U,
						   const CORBA::Long     theNbTimesU,
						   const CORBA::Long     theDirFace1V,
						   const CORBA::Long     theDirFace2V,
						   const CORBA::Long     theNbTimesV);

  ::GEOMImpl_IBlocksOperations* GetOperations()
  { return (::GEOMImpl_IBlocksOperations*)GetImpl(); }
};

#endif
