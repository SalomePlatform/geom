//NOTE: This is an intreface to a function for the Sphere creation.


#include "GEOM_Function.hxx"

#define SPH_ARG_R   1
#define SPH_ARG_PNT 4

class GEOMImpl_ISphere
{
 public:

  GEOMImpl_ISphere(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetR(double theR) { _func->SetReal(SPH_ARG_R, theR); }

  double GetR() { return _func->GetReal(SPH_ARG_R); }

  void SetPoint(Handle(GEOM_Function) theRefPoint) { _func->SetReference(SPH_ARG_PNT, theRefPoint); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(SPH_ARG_PNT); }

 private:

  Handle(GEOM_Function) _func;
};
