
#ifndef _GEOM_IBasicOperations_i_HeaderFile
#define _GEOM_IBasicOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IBasicOperations.hxx"

class GEOM_IBasicOperations_i :
    public virtual POA_GEOM::GEOM_IBasicOperations,
    public virtual GEOM_IOperations_i
{
 public:
   GEOM_IBasicOperations_i (PortableServer::POA_ptr thePOA,
			    GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_IBasicOperations* theImpl);
   ~GEOM_IBasicOperations_i();

   GEOM::GEOM_Object_ptr MakePointXYZ (CORBA::Double theX,
				       CORBA::Double theY,
				       CORBA::Double theZ);

   GEOM::GEOM_Object_ptr MakePointWithReference (GEOM::GEOM_Object_ptr theReference,
						 CORBA::Double theX,
						 CORBA::Double theY,
						 CORBA::Double theZ);

   GEOM::GEOM_Object_ptr MakePointOnCurve (GEOM::GEOM_Object_ptr theCurve,
					   CORBA::Double theParameter);

   GEOM::GEOM_Object_ptr MakeVectorDXDYDZ (CORBA::Double theDX,
					   CORBA::Double theDY,
					   CORBA::Double theDZ);

   GEOM::GEOM_Object_ptr MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					   GEOM::GEOM_Object_ptr thePnt2);

   GEOM::GEOM_Object_ptr MakeLine (GEOM::GEOM_Object_ptr thePnt,
				   GEOM::GEOM_Object_ptr theDir);

   GEOM::GEOM_Object_ptr MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					 GEOM::GEOM_Object_ptr thePnt2);

   GEOM::GEOM_Object_ptr MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					    GEOM::GEOM_Object_ptr thePnt2,
					    GEOM::GEOM_Object_ptr thePnt3,
					    CORBA::Double theTrimSize);

   GEOM::GEOM_Object_ptr MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
					  GEOM::GEOM_Object_ptr theVec,
					  CORBA::Double theTrimSize);

   GEOM::GEOM_Object_ptr MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
					CORBA::Double theTrimSize);

   GEOM::GEOM_Object_ptr MakeMarker (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
				     CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
				     CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ);

   ::GEOMImpl_IBasicOperations* GetOperations() { return (::GEOMImpl_IBasicOperations*)GetImpl(); }
};

#endif
