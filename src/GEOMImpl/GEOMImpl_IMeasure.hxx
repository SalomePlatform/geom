// NOTE: This is an interface to a functions for the Measurement


#include "GEOM_Function.hxx"

#define MEASURE_ARG_BASE 1

class GEOMImpl_IMeasure
{
 public:

  GEOMImpl_IMeasure(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetBase(Handle(GEOM_Function) theRefBase)
  { _func->SetReference(MEASURE_ARG_BASE, theRefBase); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(MEASURE_ARG_BASE); }

 private:

  Handle(GEOM_Function) _func;
};
