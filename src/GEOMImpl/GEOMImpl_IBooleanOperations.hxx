
#ifndef _GEOMImpl_IBooleanOperations_HXX_
#define _GEOMImpl_IBooleanOperations_HXX_

#include "GEOM_IOperations.hxx"

//#include "TColStd_HSequenceOfTransient.hxx"

#include "list"

class GEOM_Engine;
class Handle(GEOM_Object);
class Handle(TColStd_HArray1OfInteger);

class GEOMImpl_IBooleanOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IBooleanOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IBooleanOperations();

  Handle(GEOM_Object) MakeBoolean (Handle(GEOM_Object) theShape1,
                                   Handle(GEOM_Object) theShape2,
                                   Standard_Integer    theOp);

  Handle(GEOM_Object) MakePartition (const Handle(TColStd_HSequenceOfTransient)& theShapes, 
                                     const Handle(TColStd_HSequenceOfTransient)& theTools,
                                     const Handle(TColStd_HSequenceOfTransient)& theKeepInside,
                                     const Handle(TColStd_HSequenceOfTransient)& theRemoveInside,
                                     const Standard_Integer           theLimit,
                                     const Standard_Boolean           theRemoveWebs,
                                     const Handle(TColStd_HArray1OfInteger)& theMaterials);

  Handle(GEOM_Object) MakeHalfPartition (Handle(GEOM_Object) theShape,
                                         Handle(GEOM_Object) thePlane);
};

#endif
