
#ifndef _GEOM_IBooleanOperations_i_HeaderFile
#define _GEOM_IBooleanOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBooleanOperations.hxx"

class GEOM_IBooleanOperations_i : 
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
				       const CORBA::Short      theLimit,
				       const CORBA::Boolean    theRemoveWebs,
				       const GEOM::ListOfLong& theMaterials);


  GEOM::GEOM_Object_ptr MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
					   GEOM::GEOM_Object_ptr thePlane);

  ::GEOMImpl_IBooleanOperations* GetOperations()
  { return (::GEOMImpl_IBooleanOperations*)GetImpl(); }
};

#endif
