//NOTE: This is an intreface to a function for the Polyline creation.


#include "GEOM_Function.hxx"

#define POLY_ARG_LENG 1
#define POLY_ARG_LAST 1

class GEOMImpl_IPolyline
{
 public:

  GEOMImpl_IPolyline(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetLength(int theLen) { _func->SetInteger(POLY_ARG_LENG, theLen); }

  void SetPoint(int theId, Handle(GEOM_Function) theP) { _func->SetReference(POLY_ARG_LAST + theId, theP); }

  int GetLength() { return _func->GetInteger(POLY_ARG_LENG); }

  Handle(GEOM_Function) GetPoint(int theId) { return _func->GetReference(POLY_ARG_LAST + theId); }

 private:

  Handle(GEOM_Function) _func;
};
