// NOTE: This is an interface to a function for the Blocks
// creation and transformation.


#include "GEOM_Function.hxx"

#include "TColStd_HSequenceOfTransient.hxx"

#define BLOCK_ARG_SHAPES 1 // Arguments for creation of block or face of block

class GEOMImpl_IBlocks
{
 public:

  GEOMImpl_IBlocks(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(BLOCK_ARG_SHAPES, theShapes); }

  Handle(TColStd_HSequenceOfTransient) GetShapes()
  { return _func->GetReferenceList(BLOCK_ARG_SHAPES); }

 private:

  Handle(GEOM_Function) _func;
};
