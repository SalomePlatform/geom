//NOTE: This is an intreface to a function for the Common, Cut and Fuse creation.


#include "GEOM_Function.hxx"

#define BOOL_ARG_SHAPE1  1
#define BOOL_ARG_SHAPE2  2

class GEOMImpl_IBoolean
{
 public:

  GEOMImpl_IBoolean(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape1(Handle(GEOM_Function) theRef) { _func->SetReference(BOOL_ARG_SHAPE1, theRef); }
  void SetShape2(Handle(GEOM_Function) theRef) { _func->SetReference(BOOL_ARG_SHAPE2, theRef); }

  Handle(GEOM_Function) GetShape1() { return _func->GetReference(BOOL_ARG_SHAPE1); }
  Handle(GEOM_Function) GetShape2() { return _func->GetReference(BOOL_ARG_SHAPE2); }

 private:

  Handle(GEOM_Function) _func;
};
