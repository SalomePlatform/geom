// NOTE: This is an interface to a function for the Glueing of faces


#include "GEOM_Function.hxx"

#define GLUE_ARG_BASE   1
#define GLUE_ARG_TOLER  2

class GEOMImpl_IGlue
{
 public:

  GEOMImpl_IGlue(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetBase(Handle(GEOM_Function) theRefBase)
  { _func->SetReference(GLUE_ARG_BASE, theRefBase); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(GLUE_ARG_BASE); }

  void SetTolerance(const Standard_Real theTolerance)
  { _func->SetReal(GLUE_ARG_TOLER, theTolerance); }

  Standard_Real GetTolerance() { return _func->GetReal(GLUE_ARG_TOLER); }

 private:

  Handle(GEOM_Function) _func;
};
