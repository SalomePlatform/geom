//NOTE: This is an intreface to a function for the Cylinder creation.


#include "GEOM_Function.hxx"

#define CYL_ARG_R   1
#define CYL_ARG_H   2
#define CYL_ARG_PNT 3
#define CYL_ARG_VEC 4

class GEOMImpl_ICylinder
{
 public:

  GEOMImpl_ICylinder(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetR(double theR) { _func->SetReal(CYL_ARG_R, theR); }

  double GetR() { return _func->GetReal(CYL_ARG_R); }

  void SetH(double theH) { _func->SetReal(CYL_ARG_H, theH); }

  double GetH() { return _func->GetReal(CYL_ARG_H); }

  void SetPoint(Handle(GEOM_Function) theRefPoint) { _func->SetReference(CYL_ARG_PNT, theRefPoint); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(CYL_ARG_PNT); }

  void SetVector(Handle(GEOM_Function) theRefVector) { _func->SetReference(CYL_ARG_VEC, theRefVector); }

  Handle(GEOM_Function) GetVector() { return _func->GetReference(CYL_ARG_VEC); }

 private:

  Handle(GEOM_Function) _func;
};
