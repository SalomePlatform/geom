// NOTE: This is an intreface to a function for the Shapes
// (Wire, Face, Shell, Solid and Compound) creation.


#include "GEOM_Function.hxx"

#include "TColStd_HSequenceOfTransient.hxx"
#include "TColStd_HArray1OfInteger.hxx"

#define SHAPE_ARG_SHAPES  1 // for Wire, Shell, Solid and Compound

#define SHAPE_ARG_BASE    2 // for Face, Solid and Sub-shape

#define SHAPE_ARG_PLANAR  3 // for Face

#define SHAPE_ARG_SUBTYPE 4 // for Sub-shape
#define SHAPE_ARG_INDICES 5 // for Sub-shape

class GEOMImpl_IShapes
{
 public:

  GEOMImpl_IShapes(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(SHAPE_ARG_SHAPES, theShapes); }

  Handle(TColStd_HSequenceOfTransient) GetShapes()
  { return _func->GetReferenceList(SHAPE_ARG_SHAPES); }

  void SetBase(Handle(GEOM_Function) theRefBase)
  { _func->SetReference(SHAPE_ARG_BASE, theRefBase); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(SHAPE_ARG_BASE); }

  void SetIsPlanar(const Standard_Boolean isPlanarWanted)
  { _func->SetInteger(SHAPE_ARG_PLANAR, isPlanarWanted ? 1 : 0); }

  Standard_Boolean GetIsPlanar() { return (_func->GetInteger(SHAPE_ARG_PLANAR) == 1); }

  void SetSubShapeType(const Standard_Integer theType)
  { _func->SetInteger(SHAPE_ARG_SUBTYPE, theType); }

  Standard_Integer GetSubShapeType() { return _func->GetInteger(SHAPE_ARG_SUBTYPE); }

  void SetIndices(const Handle(TColStd_HArray1OfInteger)& theIndices)
  { _func->SetIntegerArray(SHAPE_ARG_INDICES, theIndices); }

  Handle(TColStd_HArray1OfInteger) GetIndices()
  { return _func->GetIntegerArray(SHAPE_ARG_INDICES); }

 private:

  Handle(GEOM_Function) _func;
};
