
#ifndef _GEOM_ICurvesOperations_i_HeaderFile
#define _GEOM_ICurvesOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ICurvesOperations.hxx"

class GEOM_ICurvesOperations_i : 
    public virtual POA_GEOM::GEOM_ICurvesOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_ICurvesOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_ICurvesOperations* theImpl);
  ~GEOM_ICurvesOperations_i();

  GEOM::GEOM_Object_ptr MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
					   GEOM::GEOM_Object_ptr theVector,
					   double theR);

  GEOM::GEOM_Object_ptr MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					    GEOM::GEOM_Object_ptr thePnt2,
					    GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
				     GEOM::GEOM_Object_ptr theVector,
				     double theRMajor, double theRMinor);

  GEOM::GEOM_Object_ptr MakeArc (GEOM::GEOM_Object_ptr thePnt1,
				 GEOM::GEOM_Object_ptr thePnt2,
				 GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakePolyline (const GEOM::ListOfGO& thePoints);

  GEOM::GEOM_Object_ptr MakeSplineBezier (const GEOM::ListOfGO& thePoints);

  GEOM::GEOM_Object_ptr MakeSplineInterpolation (const GEOM::ListOfGO& thePoints);

  GEOM::GEOM_Object_ptr MakeSketcher (const char* theCommand, const GEOM::ListOfDouble& theWorkingPlane);

  ::GEOMImpl_ICurvesOperations* GetOperations()
  { return (::GEOMImpl_ICurvesOperations*)GetImpl(); }
};

#endif
