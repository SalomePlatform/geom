
#ifndef _GEOMImpl_ITransformOperations_HXX_
#define _GEOMImpl_ITransformOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

class GEOMImpl_ITransformOperations : public GEOM_IOperations {
 public:
  GEOMImpl_ITransformOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_ITransformOperations();

  Handle(GEOM_Object) TranslateTwoPoints (Handle(GEOM_Object) theObject,
                                          Handle(GEOM_Object) thePoint1,
                                          Handle(GEOM_Object) thePoint2);

  Handle(GEOM_Object) TranslateTwoPointsCopy (Handle(GEOM_Object) theObject,
                                              Handle(GEOM_Object) thePoint1,
                                              Handle(GEOM_Object) thePoint2);

  Handle(GEOM_Object) TranslateDXDYDZ (Handle(GEOM_Object) theObject, double theX, double theY,  double theZ);

  Handle(GEOM_Object) TranslateDXDYDZCopy (Handle(GEOM_Object) theObject, double theX, double theY,  double theZ);

  Handle(GEOM_Object) TranslateVector (Handle(GEOM_Object) theObject,
                                       Handle(GEOM_Object) theVector);

  Handle(GEOM_Object) TranslateVectorCopy (Handle(GEOM_Object) theObject,
                                           Handle(GEOM_Object) theVector);

  Handle(GEOM_Object) Translate1D (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector, double theStep, Standard_Integer theNbTimes);

  Handle(GEOM_Object) Translate2D (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector, double theStep1, Standard_Integer theNbTimes1,
				   Handle(GEOM_Object) theVector2, double theStep2, Standard_Integer theNbTimes2);

  Handle(GEOM_Object) MirrorPlane (Handle(GEOM_Object) theObject,
                                   Handle(GEOM_Object) thePlane);

  Handle(GEOM_Object) MirrorPlaneCopy (Handle(GEOM_Object) theObject,
                                       Handle(GEOM_Object) thePlane);

  Handle(GEOM_Object) MirrorAxis (Handle(GEOM_Object) theObject,
                                  Handle(GEOM_Object) theAxis);

  Handle(GEOM_Object) MirrorAxisCopy (Handle(GEOM_Object) theObject,
                                      Handle(GEOM_Object) theAxis);

  Handle(GEOM_Object) MirrorPoint (Handle(GEOM_Object) theObject,
                                   Handle(GEOM_Object) thePoint);

  Handle(GEOM_Object) MirrorPointCopy (Handle(GEOM_Object) theObject,
                                       Handle(GEOM_Object) thePoint);

  Handle(GEOM_Object) OffsetShape (Handle(GEOM_Object) theObject,
                                   double theOffset);

  Handle(GEOM_Object) OffsetShapeCopy (Handle(GEOM_Object) theObject,
                                       double theOffset);

  Handle(GEOM_Object) ScaleShape (Handle(GEOM_Object) theObject,
                                  Handle(GEOM_Object) thePoint,
                                  double theFactor);

  Handle(GEOM_Object) ScaleShapeCopy (Handle(GEOM_Object) theObject,
                                      Handle(GEOM_Object) thePoint,
                                      double theFactor);

  Handle(GEOM_Object) PositionShape (Handle(GEOM_Object) theObject,
				     Handle(GEOM_Object) theStartLCS,
				     Handle(GEOM_Object) theEndLCS);

  Handle(GEOM_Object) PositionShapeCopy (Handle(GEOM_Object) theObject,
					 Handle(GEOM_Object) theStartLCS,
					 Handle(GEOM_Object) theEndLCS);

  Handle(GEOM_Object) Rotate (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis, double theAngle);

  Handle(GEOM_Object) RotateCopy (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis, double theAngle);

  Handle(GEOM_Object) Rotate1D (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis, Standard_Integer theNbTimes);
  
  Handle(GEOM_Object) Rotate2D (Handle(GEOM_Object) theObject, 
				Handle(GEOM_Object) theAxis, 
				double theAngle, 
				Standard_Integer theNbTimes1,
				double theStep, 
				Standard_Integer theNbTimes2);
};

#endif
