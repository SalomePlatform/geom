//NOTE: This is an intreface to a function for the Circle creation.


#include "GEOM_Function.hxx"

#define CIRC_ARG_P1  1
#define CIRC_ARG_P2  2
#define CIRC_ARG_P3  3

#define CIRC_ARG_CC  4
#define CIRC_ARG_VV  5
#define CIRC_ARG_RR  6

class GEOMImpl_ICircle
{
 public:

  GEOMImpl_ICircle(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetPoint1(Handle(GEOM_Function) theP) { _func->SetReference(CIRC_ARG_P1, theP); }
  void SetPoint2(Handle(GEOM_Function) theP) { _func->SetReference(CIRC_ARG_P2, theP); }
  void SetPoint3(Handle(GEOM_Function) theP) { _func->SetReference(CIRC_ARG_P3, theP); }

  void SetCenter(Handle(GEOM_Function) theP) { _func->SetReference(CIRC_ARG_CC, theP); }
  void SetVector(Handle(GEOM_Function) theV) { _func->SetReference(CIRC_ARG_VV, theV); }

  void SetRadius(double theR) { _func->SetReal(CIRC_ARG_RR, theR); }

  Handle(GEOM_Function) GetPoint1() { return _func->GetReference(CIRC_ARG_P1); }
  Handle(GEOM_Function) GetPoint2() { return _func->GetReference(CIRC_ARG_P2); }
  Handle(GEOM_Function) GetPoint3() { return _func->GetReference(CIRC_ARG_P3); }

  Handle(GEOM_Function) GetCenter() { return _func->GetReference(CIRC_ARG_CC); }
  Handle(GEOM_Function) GetVector() { return _func->GetReference(CIRC_ARG_VV); }

  double GetRadius() { return _func->GetReal(CIRC_ARG_RR); }

 private:

  Handle(GEOM_Function) _func;
};
