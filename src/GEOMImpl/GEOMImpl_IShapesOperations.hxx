
#ifndef _GEOMImpl_IShapesOperations_HXX_
#define _GEOMImpl_IShapesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <TopTools_ListOfShape.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <list>

class GEOM_Engine;
class Handle(GEOM_Object);
class Handle(TColStd_HArray1OfInteger);

class GEOMImpl_IShapesOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IShapesOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IShapesOperations();

  Handle(GEOM_Object) MakeEdge (Handle(GEOM_Object) thePoint1,
                                Handle(GEOM_Object) thePoint2);

  Handle(GEOM_Object) MakeWire (list<Handle(GEOM_Object)> theEdgesAndWires);

  Handle(GEOM_Object) MakeFace (Handle(GEOM_Object) theWire, const bool isPlanarWanted);

  Handle(GEOM_Object) MakeFaceWires (list<Handle(GEOM_Object)> theWires,
                                     const bool isPlanarWanted);

  Handle(GEOM_Object) MakeShell (list<Handle(GEOM_Object)> theShapes);

  Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Handle(GEOM_Object) MakeSolidShells (list<Handle(GEOM_Object)> theShells);

  Handle(GEOM_Object) MakeCompound (list<Handle(GEOM_Object)> theShapes);

  Handle(GEOM_Object) MakeGlueFaces (Handle(GEOM_Object) theShape,
                                     const Standard_Real theTolerance);

  Handle(TColStd_HSequenceOfTransient) MakeExplode (Handle(GEOM_Object)    theShape,
                                                    const Standard_Integer theShapeType,
                                                    const Standard_Boolean isSorted);

  Handle(TColStd_HSequenceOfInteger) SubShapeAllIDs (Handle(GEOM_Object)    theShape,
                                                     const Standard_Integer theShapeType,
                                                     const Standard_Boolean isSorted);

  Handle(GEOM_Object) GetSubShape (Handle(GEOM_Object)    theMainShape,
                                   const Standard_Integer theID);

  Standard_Integer NumberOfFaces (Handle(GEOM_Object) theShape);
  Standard_Integer NumberOfEdges (Handle(GEOM_Object) theShape);

  Handle(GEOM_Object) ReverseShape(Handle(GEOM_Object) theShapes);

  Handle(TColStd_HSequenceOfInteger) GetFreeFacesIDs (Handle(GEOM_Object) theShape);

  Handle(TColStd_HSequenceOfTransient) GetSharedShapes (Handle(GEOM_Object)    theShape1,
                                                        Handle(GEOM_Object)    theShape2,
                                                        const Standard_Integer theShapeType);

  Handle(GEOM_Object) GetShapesOnPlane (Handle(GEOM_Object)    theShape,
                                        const Standard_Integer theShapeType,
                                        Handle(GEOM_Object)    thePlane);

  Handle(GEOM_Object) GetShapesOnCylinder (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           Handle(GEOM_Object)    theAxis,
                                           const Standard_Real    theRadius);

  Handle(GEOM_Object) GetShapesOnSphere (Handle(GEOM_Object)    theShape,
                                         const Standard_Integer theShapeType,
                                         Handle(GEOM_Object)    theCenter,
                                         const Standard_Real    theRadius);

  Handle(GEOM_Object) GetInPlace (Handle(GEOM_Object) theShapeWhere,
                                  Handle(GEOM_Object) theShapeWhat);

  static void SortShapes (TopTools_ListOfShape& SL);

 private:
  Handle(GEOM_Object) MakeShape (list<Handle(GEOM_Object)>     theShapes,
                                 const Standard_Integer        theObjectType,
                                 const Standard_Integer        theFunctionType,
                                 const TCollection_AsciiString theMethodName);
};

#endif
