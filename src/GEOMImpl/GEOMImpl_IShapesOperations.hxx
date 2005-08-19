
#ifndef _GEOMImpl_IShapesOperations_HXX_
#define _GEOMImpl_IShapesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include "GEOMAlgo_State.hxx"

#include <TopTools_ListOfShape.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <list>

class GEOM_Engine;
class Handle(GEOM_Object);
class Handle(TColStd_HArray1OfInteger);

class GEOMImpl_IShapesOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IShapesOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IShapesOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeEdge (Handle(GEOM_Object) thePoint1,
                                Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakeWire (list<Handle(GEOM_Object)> theEdgesAndWires);

  Standard_EXPORT Handle(GEOM_Object) MakeFace (Handle(GEOM_Object) theWire, const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeFaceWires (list<Handle(GEOM_Object)> theWires,
                                     const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeShell (list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShells (list<Handle(GEOM_Object)> theShells);

  Standard_EXPORT Handle(GEOM_Object) MakeCompound (list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueFaces (Handle(GEOM_Object) theShape,
                                     const Standard_Real theTolerance);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakeExplode (Handle(GEOM_Object)    theShape,
                                                    const Standard_Integer theShapeType,
                                                    const Standard_Boolean isSorted);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) SubShapeAllIDs (Handle(GEOM_Object)    theShape,
                                                     const Standard_Integer theShapeType,
                                                     const Standard_Boolean isSorted);

  Standard_EXPORT Handle(GEOM_Object) GetSubShape (Handle(GEOM_Object)    theMainShape,
                                   const Standard_Integer theID);

  Standard_EXPORT Standard_Integer NumberOfFaces (Handle(GEOM_Object) theShape);
  Standard_EXPORT Standard_Integer NumberOfEdges (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(GEOM_Object) ReverseShape(Handle(GEOM_Object) theShapes);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetFreeFacesIDs (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetSharedShapes (Handle(GEOM_Object)    theShape1,
                                                        Handle(GEOM_Object)    theShape2,
                                                        const Standard_Integer theShapeType);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnPlane (const Handle(GEOM_Object)& theShape,
                                                         const Standard_Integer     theShapeType,
                                                         const Handle(GEOM_Object)& theAx1,
                                                         const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnCylinder (const Handle(GEOM_Object)& theShape,
                                                            const Standard_Integer     theShapeType,
                                                            const Handle(GEOM_Object)& theAxis,
                                                            const Standard_Real        theRadius,
                                                            const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnSphere (const Handle(GEOM_Object)& theShape,
                                                          const Standard_Integer     theShapeType,
                                                          const Handle(GEOM_Object)& theCenter,
                                                          const Standard_Real        theRadius,
                                                          const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnPlaneIDs (const Handle(GEOM_Object)& theShape,
                                                          const Standard_Integer     theShapeType,
                                                          const Handle(GEOM_Object)& theAx1,
                                                          const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnCylinderIDs (const Handle(GEOM_Object)& theShape,
                                                             const Standard_Integer     theShapeType,
                                                             const Handle(GEOM_Object)& theAxis,
                                                             const Standard_Real        theRadius,
                                                             const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnSphereIDs (const Handle(GEOM_Object)& theShape,
                                                           const Standard_Integer     theShapeType,
                                                           const Handle(GEOM_Object)& theCenter,
                                                           const Standard_Real        theRadius,
                                                           const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(GEOM_Object) GetShapesOnCylinderOld (Handle(GEOM_Object)    theShape,
                                              const Standard_Integer theShapeType,
                                              Handle(GEOM_Object)    theAxis,
                                              const Standard_Real    theRadius);

  Standard_EXPORT Handle(GEOM_Object) GetShapesOnSphereOld (Handle(GEOM_Object)    theShape,
                                            const Standard_Integer theShapeType,
                                            Handle(GEOM_Object)    theCenter,
                                            const Standard_Real    theRadius);

  Standard_EXPORT Handle(GEOM_Object) GetInPlace (Handle(GEOM_Object) theShapeWhere,
                                  Handle(GEOM_Object) theShapeWhat);

  Standard_EXPORT static void SortShapes (TopTools_ListOfShape& SL);

 private:
  Handle(GEOM_Object) MakeShape (list<Handle(GEOM_Object)>      theShapes,
                                 const Standard_Integer         theObjectType,
                                 const Standard_Integer         theFunctionType,
                                 const TCollection_AsciiString& theMethodName);

  bool CheckTriangulation (const TopoDS_Shape& aShape);
};

#endif
