
#ifndef _GEOM_ILocalOperations_i_HeaderFile
#define _GEOM_ILocalOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ILocalOperations.hxx"

class GEOM_ILocalOperations_i : 
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
  GEOM::GEOM_Object_ptr MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
					 const GEOM::ListOfLong& theFaces);

  GEOM::GEOM_Object_ptr MakeChamferAll   (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD);
  GEOM::GEOM_Object_ptr MakeChamferEdge  (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  CORBA::Long theFace1, CORBA::Long theFace2);
  GEOM::GEOM_Object_ptr MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  const GEOM::ListOfLong& theFaces);

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
