using namespace std; 

#ifndef _GEOMImpl_ICurvesOperations_HXX_
#define _GEOMImpl_ICurvesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <list>

class GEOM_Engine;
class Handle(GEOM_Object);

class GEOMImpl_ICurvesOperations : public GEOM_IOperations {
 public:
  GEOMImpl_ICurvesOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_ICurvesOperations();

  Handle(GEOM_Object) MakePolyline (list<Handle(GEOM_Object)> thePoints);

  Handle(GEOM_Object) MakeCircleThreePnt (Handle(GEOM_Object) thePnt1,
                                          Handle(GEOM_Object) thePnt2,
                                          Handle(GEOM_Object) thePnt3);
  Handle(GEOM_Object) MakeCirclePntVecR  (Handle(GEOM_Object) thePnt,
                                          Handle(GEOM_Object) theVec, double theR);

  Handle(GEOM_Object) MakeEllipse (Handle(GEOM_Object) thePnt,
                                   Handle(GEOM_Object) theVec,
                                   double theRMajor, double theRMinor);

  Handle(GEOM_Object) MakeArc (Handle(GEOM_Object) thePnt1,
                               Handle(GEOM_Object) thePnt2,
                               Handle(GEOM_Object) thePnt3);

  Handle(GEOM_Object) MakeSplineBezier        (list<Handle(GEOM_Object)> thePoints);
  Handle(GEOM_Object) MakeSplineInterpolation (list<Handle(GEOM_Object)> thePoints);

  Handle(GEOM_Object) MakeSketcher (const char* theCommand, list<double> theWorkingPlane);
};

#endif
