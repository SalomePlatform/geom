
#ifndef _GEOMImpl_I3DPrimOperations_HXX_
#define _GEOMImpl_I3DPrimOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

class GEOMImpl_I3DPrimOperations : public GEOM_IOperations {
 public:
  GEOMImpl_I3DPrimOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_I3DPrimOperations();

  Handle(GEOM_Object) MakeBoxDXDYDZ (double theDX, double theDY, double theDZ);
  Handle(GEOM_Object) MakeBoxTwoPnt (Handle(GEOM_Object) thePnt1,
                                     Handle(GEOM_Object) thePnt2);

  Handle(GEOM_Object) MakeCylinderRH       (double theR, double theH);
  Handle(GEOM_Object) MakeCylinderPntVecRH (Handle(GEOM_Object) thePnt,
                                            Handle(GEOM_Object) theVec,
                                            double theR, double theH);

  Handle(GEOM_Object) MakeConeR1R2H       (double theR1, double theR2, double theH);
  Handle(GEOM_Object) MakeConePntVecR1R2H (Handle(GEOM_Object) thePnt,
                                           Handle(GEOM_Object) theVec,
                                           double theR1, double theR2, double theH);

  Handle(GEOM_Object) MakeSphereR    (double theR);
  Handle(GEOM_Object) MakeSpherePntR (Handle(GEOM_Object) thePnt, double theR);

  Handle(GEOM_Object) MakeTorusRR (double theRMajor, double theRMinor);

  Handle(GEOM_Object) MakeTorusPntVecRR (Handle(GEOM_Object) thePnt,
                                         Handle(GEOM_Object) theVec,
                                         double theRMajor, double theRMinor);

  Handle(GEOM_Object) MakePrismVecH (Handle(GEOM_Object) theBase,
                                     Handle(GEOM_Object) theVec, double theH);

  Handle(GEOM_Object) MakePrismTwoPnt (Handle(GEOM_Object) theBase,
                                       Handle(GEOM_Object) thePoint1,
                                       Handle(GEOM_Object) thePoint2);

  Handle(GEOM_Object) MakePipe (Handle(GEOM_Object) theBase,
                                Handle(GEOM_Object) thePath);

  Handle(GEOM_Object) MakeRevolutionAxisAngle (Handle(GEOM_Object) theBase,
                                               Handle(GEOM_Object) theAxis,
                                               double theAngle);

  Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Handle(GEOM_Object) MakeFilling (Handle(GEOM_Object) theShape, int theMinDeg, int theMaxDeg, double theTol2D, double theTol3D, int theNbIter);

};

#endif
