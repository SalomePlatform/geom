// NOTE: This is an intreface to a function for the Shapes
// (Wire, Face, Shell, Solid and Compound) creation.


#include "GEOM_Function.hxx"

#include "TColStd_HSequenceOfTransient.hxx"
#include "TColStd_HArray1OfInteger.hxx"

#define SHAPE_ARG_MAIN_SHAPE     1 
#define SHAPE_ARG_INDICES             2 
#define SHAPE_ARG_SORTED             3

class GEOM_ISubShape
{
 public:

  GEOM_ISubShape(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetMainShape(Handle(GEOM_Function) theRefBase)
  { _func->SetReference(SHAPE_ARG_MAIN_SHAPE, theRefBase); }

  Handle(GEOM_Function) GetMainShape() { return _func->GetReference(SHAPE_ARG_MAIN_SHAPE); }

  void SetIndices(const Handle(TColStd_HArray1OfInteger)& theIndices)
  { _func->SetIntegerArray(SHAPE_ARG_INDICES, theIndices); }

  Handle(TColStd_HArray1OfInteger) GetIndices()
  { return _func->GetIntegerArray(SHAPE_ARG_INDICES); }

 private:

  Handle(GEOM_Function) _func;
};
