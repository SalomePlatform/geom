
#ifndef _GEOMImpl_IGroupOperations_HXX_
#define _GEOMImpl_IGroupOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>
#include <TColStd_HArray1OfInteger.hxx> 
#include <TopAbs.hxx>

class GEOMImpl_IGroupOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IGroupOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IGroupOperations();

  Handle(GEOM_Object) CreateGroup(Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum  theShapeType);

  void AddObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  void RemoveObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  TopAbs_ShapeEnum GetType(Handle(GEOM_Object) theGroup);

  Handle(GEOM_Object) GetMainShape(Handle(GEOM_Object) theGroup);

  Handle(TColStd_HArray1OfInteger) GetObjects(Handle(GEOM_Object) theGroup);

};

#endif
