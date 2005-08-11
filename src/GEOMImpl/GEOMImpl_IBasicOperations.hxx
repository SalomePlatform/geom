
#ifndef _GEOMImpl_IBasicOperations_HXX_
#define _GEOMImpl_IBasicOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

class GEOMImpl_IBasicOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IBasicOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IBasicOperations();

  // Point
  Standard_EXPORT Handle(GEOM_Object) MakePointXYZ (double theX, double theY, double theZ);

  Standard_EXPORT Handle(GEOM_Object) MakePointWithReference (Handle(GEOM_Object) theReference,
                                              double theX, double theY, double theZ);

  Standard_EXPORT Handle(GEOM_Object) MakePointOnCurve (Handle(GEOM_Object) theCurve,
                                        double theParameter);

  // Vector
  Standard_EXPORT Handle(GEOM_Object) MakeVectorDXDYDZ (double theDX, double theDY, double theDZ);

  Standard_EXPORT Handle(GEOM_Object) MakeVectorTwoPnt (Handle(GEOM_Object) thePnt1,
                                        Handle(GEOM_Object) thePnt2);

  // Line
  Standard_EXPORT Handle(GEOM_Object) MakeLineTwoPnt (Handle(GEOM_Object) thePnt1,
                                      Handle(GEOM_Object) thePnt2);

  Standard_EXPORT Handle(GEOM_Object) MakeLine (Handle(GEOM_Object) thePnt,
                                Handle(GEOM_Object) theDir);

  // Plane
  Standard_EXPORT Handle(GEOM_Object) MakePlaneThreePnt (Handle(GEOM_Object) thePnt1,
                                         Handle(GEOM_Object) thePnt2,
                                         Handle(GEOM_Object) thePnt3,
                                         double theSize);

  Standard_EXPORT Handle(GEOM_Object) MakePlanePntVec (Handle(GEOM_Object) thePnt,
                                       Handle(GEOM_Object) theVec,
                                       double theSize);

  Standard_EXPORT Handle(GEOM_Object) MakePlaneFace (Handle(GEOM_Object) theFace, double theSize);

  // Marker
  Standard_EXPORT Handle(GEOM_Object) MakeMarker (double theOX,  double theOY,  double theOZ,
                                  double theXDX, double theXDY, double theXDZ,
                                  double theYDX, double theYDY, double theYDZ);
};

#endif
