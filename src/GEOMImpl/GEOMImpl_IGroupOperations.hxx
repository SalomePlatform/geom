
#ifndef _GEOMImpl_IGroupOperations_HXX_
#define _GEOMImpl_IGroupOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>
#include <TColStd_HArray1OfInteger.hxx> 
#include <TColStd_HSequenceOfInteger.hxx>
#include <TopAbs.hxx>

class GEOMImpl_IGroupOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IGroupOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IGroupOperations();

  Standard_EXPORT Handle(GEOM_Object) CreateGroup(Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum  theShapeType);

  Standard_EXPORT void AddObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  Standard_EXPORT void RemoveObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  Standard_EXPORT void UnionList (Handle(GEOM_Object) theGroup,
                  const Handle(TColStd_HSequenceOfTransient)& theSubShapes);

  Standard_EXPORT void DifferenceList (Handle(GEOM_Object) theGroup,
                       const Handle(TColStd_HSequenceOfTransient)& theSubShapes);

  Standard_EXPORT void UnionIDs (Handle(GEOM_Object) theGroup,
                 const Handle(TColStd_HSequenceOfInteger)& theSubShapes);

  Standard_EXPORT void DifferenceIDs (Handle(GEOM_Object) theGroup,
                      const Handle(TColStd_HSequenceOfInteger)& theSubShapes);

  Standard_EXPORT TopAbs_ShapeEnum GetType(Handle(GEOM_Object) theGroup);

  Standard_EXPORT Handle(GEOM_Object) GetMainShape(Handle(GEOM_Object) theGroup);

  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetObjects(Handle(GEOM_Object) theGroup);

};

#endif
