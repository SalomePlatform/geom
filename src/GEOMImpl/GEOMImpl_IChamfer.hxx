//NOTE: This is an interface to a function for the Chamfer and creation.


#include "GEOM_Function.hxx"

#define CHAM_ARG_SH   1
#define CHAM_ARG_D1   2
#define CHAM_ARG_D2   3
#define CHAM_ARG_LENG 4
#define CHAM_ARG_LAST 4

class GEOMImpl_IChamfer
{
 public:

  GEOMImpl_IChamfer(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theRef) { _func->SetReference(CHAM_ARG_SH, theRef); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(CHAM_ARG_SH); }

  void SetD (double theD) { _func->SetReal(CHAM_ARG_D1, theD); }
  void SetD1(double theD) { _func->SetReal(CHAM_ARG_D1, theD); }
  void SetD2(double theD) { _func->SetReal(CHAM_ARG_D2, theD); }

  double GetD () { return _func->GetReal(CHAM_ARG_D1); }
  double GetD1() { return _func->GetReal(CHAM_ARG_D1); }
  double GetD2() { return _func->GetReal(CHAM_ARG_D2); }

  void SetLength(int theLen) { _func->SetInteger(CHAM_ARG_LENG, theLen); }

  int GetLength() { return _func->GetInteger(CHAM_ARG_LENG); }

  void SetFace(int theInd, int theFace)
              { _func->SetInteger(CHAM_ARG_LAST + theInd, theFace); }
  void SetFace1(int theFace)
              { _func->SetInteger(CHAM_ARG_LAST + 1, theFace); }
  void SetFace2(int theFace)
              { _func->SetInteger(CHAM_ARG_LAST + 2, theFace); }

  int GetFace(int theInd) { return _func->GetInteger(CHAM_ARG_LAST + theInd); }
  int GetFace1() { return _func->GetInteger(CHAM_ARG_LAST + 1); }
  int GetFace2() { return _func->GetInteger(CHAM_ARG_LAST + 2); }

 private:

  Handle(GEOM_Function) _func;
};
