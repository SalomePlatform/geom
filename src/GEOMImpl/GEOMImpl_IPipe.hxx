//NOTE: This is an intreface to a function for the Pipe creation.


#include "GEOM_Function.hxx"

#define PIPE_ARG_BASE 1
#define PIPE_ARG_PATH 2

class GEOMImpl_IPipe
{
 public:

  GEOMImpl_IPipe(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetBase (Handle(GEOM_Function) theBase) { _func->SetReference(PIPE_ARG_BASE, theBase); }
  void SetPath (Handle(GEOM_Function) thePath) { _func->SetReference(PIPE_ARG_PATH, thePath); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(PIPE_ARG_BASE); }
  Handle(GEOM_Function) GetPath() { return _func->GetReference(PIPE_ARG_PATH); }

 private:

  Handle(GEOM_Function) _func;
};
