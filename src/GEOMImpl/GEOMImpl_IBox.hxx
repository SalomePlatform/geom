//NOTE: This is an intreface to a function for the Box creation.


#include "GEOM_Function.hxx"

#define BOX_ARG_DX   1
#define BOX_ARG_DY   2
#define BOX_ARG_DZ   3
#define BOX_ARG_REF1 4
#define BOX_ARG_REF2 5

class GEOMImpl_IBox
{
 public:

  GEOMImpl_IBox(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetDX(double theX) { _func->SetReal(BOX_ARG_DX, theX); }

  double GetDX() { return _func->GetReal(BOX_ARG_DX); }

  void SetDY(double theY) { _func->SetReal(BOX_ARG_DY, theY); }

  double GetDY() { return _func->GetReal(BOX_ARG_DY); }

  void SetDZ(double theZ) { _func->SetReal(BOX_ARG_DZ, theZ); }

  double GetDZ() { return _func->GetReal(BOX_ARG_DZ); }

  void SetRef1(Handle(GEOM_Function) theRefPoint1) { _func->SetReference(BOX_ARG_REF1, theRefPoint1); }

  Handle(GEOM_Function) GetRef1() { return _func->GetReference(BOX_ARG_REF1); }

  void SetRef2(Handle(GEOM_Function) theRefPoint2) { _func->SetReference(BOX_ARG_REF2, theRefPoint2); }

  Handle(GEOM_Function) GetRef2() { return _func->GetReference(BOX_ARG_REF2); }

 private:

  Handle(GEOM_Function) _func;
};
