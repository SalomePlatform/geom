//NOTE: This is an intreface to a function for the vector creation.


#include "GEOM_Function.hxx"

#define LINE_ARG_PNT1 1
#define LINE_ARG_PNT2 2

class GEOMImpl_ILine
{
 public:

  GEOMImpl_ILine(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetPoint1(Handle(GEOM_Function) theRef) { _func->SetReference(LINE_ARG_PNT1, theRef); }
  void SetPoint2(Handle(GEOM_Function) theRef) { _func->SetReference(LINE_ARG_PNT2, theRef); }

  Handle(GEOM_Function) GetPoint1() { return _func->GetReference(LINE_ARG_PNT1); }
  Handle(GEOM_Function) GetPoint2() { return _func->GetReference(LINE_ARG_PNT2); }

 private:

  Handle(GEOM_Function) _func;
};
