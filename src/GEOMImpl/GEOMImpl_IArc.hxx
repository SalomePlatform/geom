//NOTE: This is an intreface to a function for the Arc creation.


#include "GEOM_Function.hxx"

#define ARC_ARG_PI  1
#define ARC_ARG_PC  2
#define ARC_ARG_PE  3

class GEOMImpl_IArc
{
 public:

  GEOMImpl_IArc(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetPoint1(Handle(GEOM_Function) theP) { _func->SetReference(ARC_ARG_PI, theP); }
  void SetPoint2(Handle(GEOM_Function) theP) { _func->SetReference(ARC_ARG_PC, theP); }
  void SetPoint3(Handle(GEOM_Function) theP) { _func->SetReference(ARC_ARG_PE, theP); }

  Handle(GEOM_Function) GetPoint1() { return _func->GetReference(ARC_ARG_PI); }
  Handle(GEOM_Function) GetPoint2() { return _func->GetReference(ARC_ARG_PC); }
  Handle(GEOM_Function) GetPoint3() { return _func->GetReference(ARC_ARG_PE); }

 private:

  Handle(GEOM_Function) _func;
};
