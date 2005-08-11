
#ifndef _GEOMImpl_I3DPrimOperations_HXX_
#define _GEOMImpl_I3DPrimOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

class GEOMImpl_I3DPrimOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_I3DPrimOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_I3DPrimOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeBoxDXDYDZ (double theDX, double theDY, double theDZ);
  Standard_EXPORT Handle(GEOM_Object) MakeBoxTwoPnt (Handle(GEOM_Object) thePnt1,
                                     Handle(GEOM_Object) thePnt2);

  Standard_EXPORT Handle(GEOM_Object) MakeCylinderRH       (double theR, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderPntVecRH (Handle(GEOM_Object) thePnt,
                                            Handle(GEOM_Object) theVec,
                                            double theR, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakeConeR1R2H       (double theR1, double theR2, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeConePntVecR1R2H (Handle(GEOM_Object) thePnt,
                                           Handle(GEOM_Object) theVec,
                                           double theR1, double theR2, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakeSphereR    (double theR);
  Standard_EXPORT Handle(GEOM_Object) MakeSpherePntR (Handle(GEOM_Object) thePnt, double theR);

  Standard_EXPORT Handle(GEOM_Object) MakeTorusRR (double theRMajor, double theRMinor);

  Standard_EXPORT Handle(GEOM_Object) MakeTorusPntVecRR (Handle(GEOM_Object) thePnt,
                                         Handle(GEOM_Object) theVec,
                                         double theRMajor, double theRMinor);

  Standard_EXPORT Handle(GEOM_Object) MakePrismVecH (Handle(GEOM_Object) theBase,
                                     Handle(GEOM_Object) theVec, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakePrismTwoPnt (Handle(GEOM_Object) theBase,
                                       Handle(GEOM_Object) thePoint1,
                                       Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakePipe (Handle(GEOM_Object) theBase,
                                Handle(GEOM_Object) thePath);

  Standard_EXPORT Handle(GEOM_Object) MakeRevolutionAxisAngle (Handle(GEOM_Object) theBase,
                                               Handle(GEOM_Object) theAxis,
                                               double theAngle);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Standard_EXPORT Handle(GEOM_Object) MakeFilling (Handle(GEOM_Object) theShape, int theMinDeg, int theMaxDeg, double theTol2D, double theTol3D, int theNbIter);

};

#endif
