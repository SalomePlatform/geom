//NOTE: This is an intreface to a function for the Cone creation.


#include "GEOM_Function.hxx"

#define CONE_ARG_R1  1
#define CONE_ARG_R2  2
#define CONE_ARG_H   3
#define CONE_ARG_PNT 4
#define CONE_ARG_VEC 5

class GEOMImpl_ICone
{
 public:

  GEOMImpl_ICone(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetR1(double theR) { _func->SetReal(CONE_ARG_R1, theR); }

  double GetR1() { return _func->GetReal(CONE_ARG_R1); }

  void SetR2(double theR) { _func->SetReal(CONE_ARG_R2, theR); }

  double GetR2() { return _func->GetReal(CONE_ARG_R2); }

  void SetH(double theH) { _func->SetReal(CONE_ARG_H, theH); }

  double GetH() { return _func->GetReal(CONE_ARG_H); }

  void SetPoint(Handle(GEOM_Function) theRefPoint) { _func->SetReference(CONE_ARG_PNT, theRefPoint); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(CONE_ARG_PNT); }

  void SetVector(Handle(GEOM_Function) theRefVector) { _func->SetReference(CONE_ARG_VEC, theRefVector); }

  Handle(GEOM_Function) GetVector() { return _func->GetReference(CONE_ARG_VEC); }

 private:

  Handle(GEOM_Function) _func;
};
