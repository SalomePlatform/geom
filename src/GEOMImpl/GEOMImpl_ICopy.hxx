//NOTE: This is an intreface to a function for the Copy operation.


#include "GEOM_Function.hxx"

#define COPY_ARG_REF  1

class GEOMImpl_ICopy
{
 public:

  GEOMImpl_ICopy(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetOriginal(Handle(GEOM_Function) theOriginal) { _func->SetReference(COPY_ARG_REF, theOriginal); }

  Handle(GEOM_Function) GetOriginal() { return _func->GetReference(COPY_ARG_REF); }

 private:

  Handle(GEOM_Function) _func;
};
