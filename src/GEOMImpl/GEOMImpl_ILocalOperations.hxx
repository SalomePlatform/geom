
#ifndef _GEOMImpl_ILocalOperations_HXX_
#define _GEOMImpl_ILocalOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

#include <list>

class GEOMImpl_ILocalOperations : public GEOM_IOperations {
 public:
  GEOMImpl_ILocalOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_ILocalOperations();

  Handle(GEOM_Object) MakeFilletAll   (Handle(GEOM_Object) theShape, double theR);
  Handle(GEOM_Object) MakeFilletEdges (Handle(GEOM_Object) theShape, double theR,
                                       list<int> theEdges);
  Handle(GEOM_Object) MakeFilletFaces (Handle(GEOM_Object) theShape, double theR,
                                       list<int> theFaces);

  Handle(GEOM_Object) MakeChamferAll   (Handle(GEOM_Object) theShape, double theD);
  Handle(GEOM_Object) MakeChamferEdge  (Handle(GEOM_Object) theShape,
                                        double theD1, double theD2,
                                        int theFace1, int theFace2);
  Handle(GEOM_Object) MakeChamferFaces (Handle(GEOM_Object) theShape,
                                        double theD1, double theD2,
                                        list<int> theFaces);

  Handle(GEOM_Object) MakeArchimede (Handle(GEOM_Object) theShape,
                                     double theWeight, double theWaterDensity,
                                     double theMeshingDeflection);

  Standard_Integer GetSubShapeIndex (Handle(GEOM_Object) theShape,
                                     Handle(GEOM_Object) theSubShape);

  static bool GetSubShape (const TopoDS_Shape& theShape,
                           const int theIndex,
                           TopoDS_Shape& theSubShape);
};

#endif
