//NOTE: This is an interface to a function for the Fillet and creation.


#include "GEOM_Function.hxx"

#define FILLET_ARG_SH   1
#define FILLET_ARG_R    2
#define FILLET_ARG_LENG 3
#define FILLET_ARG_LAST 3

class GEOMImpl_IFillet
{
 public:

  GEOMImpl_IFillet(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theRef) { _func->SetReference(FILLET_ARG_SH, theRef); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(FILLET_ARG_SH); }

  void SetR(double theR) { _func->SetReal(FILLET_ARG_R, theR); }

  double GetR() { return _func->GetReal(FILLET_ARG_R); }

  void SetLength(int theLen) { _func->SetInteger(FILLET_ARG_LENG, theLen); }

  int GetLength() { return _func->GetInteger(FILLET_ARG_LENG); }

  void SetEdge(int theInd, int theEdge)
              { _func->SetInteger(FILLET_ARG_LAST + theInd, theEdge); }
  void SetFace(int theInd, int theFace)
              { _func->SetInteger(FILLET_ARG_LAST + theInd, theFace); }

  int GetEdge(int theInd) { return _func->GetInteger(FILLET_ARG_LAST + theInd); }
  int GetFace(int theInd) { return _func->GetInteger(FILLET_ARG_LAST + theInd); }

 private:

  Handle(GEOM_Function) _func;
};
