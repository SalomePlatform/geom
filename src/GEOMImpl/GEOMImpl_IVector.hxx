//NOTE: This is an intreface to a function for the vector creation.


#include "GEOM_Function.hxx"

#define VEC_ARG_DX   1
#define VEC_ARG_DY   2
#define VEC_ARG_DZ   3

#define VEC_ARG_PNT1 4
#define VEC_ARG_PNT2 5

class GEOMImpl_IVector
{
 public:

  GEOMImpl_IVector(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetDX(double theDX) { _func->SetReal(VEC_ARG_DX, theDX); }
  void SetDY(double theDY) { _func->SetReal(VEC_ARG_DY, theDY); }
  void SetDZ(double theDZ) { _func->SetReal(VEC_ARG_DZ, theDZ); }

  double GetDX() { return _func->GetReal(VEC_ARG_DX); }
  double GetDY() { return _func->GetReal(VEC_ARG_DY); }
  double GetDZ() { return _func->GetReal(VEC_ARG_DZ); }

  void SetPoint1(Handle(GEOM_Function) theRef) { _func->SetReference(VEC_ARG_PNT1, theRef); }
  void SetPoint2(Handle(GEOM_Function) theRef) { _func->SetReference(VEC_ARG_PNT2, theRef); }

  Handle(GEOM_Function) GetPoint1() { return _func->GetReference(VEC_ARG_PNT1); }
  Handle(GEOM_Function) GetPoint2() { return _func->GetReference(VEC_ARG_PNT2); }

 private:

  Handle(GEOM_Function) _func;
};
