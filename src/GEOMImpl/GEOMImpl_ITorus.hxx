//NOTE: This is an intreface to a function for the Torus creation.


#include "GEOM_Function.hxx"

#define TORUS_ARG_CC   1
#define TORUS_ARG_VV   2
#define TORUS_ARG_RMAJ 3
#define TORUS_ARG_RMIN 4

class GEOMImpl_ITorus
{
 public:

  GEOMImpl_ITorus(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetCenter(Handle(GEOM_Function) theP) { _func->SetReference(TORUS_ARG_CC, theP); }
  void SetVector(Handle(GEOM_Function) theV) { _func->SetReference(TORUS_ARG_VV, theV); }

  void SetRMajor(double theR) { _func->SetReal(TORUS_ARG_RMAJ, theR); }
  void SetRMinor(double theR) { _func->SetReal(TORUS_ARG_RMIN, theR); }

  Handle(GEOM_Function) GetCenter() { return _func->GetReference(TORUS_ARG_CC); }
  Handle(GEOM_Function) GetVector() { return _func->GetReference(TORUS_ARG_VV); }

  double GetRMajor() { return _func->GetReal(TORUS_ARG_RMAJ); }
  double GetRMinor() { return _func->GetReal(TORUS_ARG_RMIN); }

 private:

  Handle(GEOM_Function) _func;
};
