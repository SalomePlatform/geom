//NOTE: This is an intreface to a function for the Revolution creation.


#include "GEOM_Function.hxx"

#define REVOL_ARG_ANGLE 1
#define REVOL_ARG_AXIS  2
#define REVOL_ARG_BASE  3

class GEOMImpl_IRevolution
{
 public:

  GEOMImpl_IRevolution(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetAngle(double theAngle) { _func->SetReal(REVOL_ARG_ANGLE, theAngle); }

  double GetAngle() { return _func->GetReal(REVOL_ARG_ANGLE); }

  void SetAxis(Handle(GEOM_Function) theRefAxis) { _func->SetReference(REVOL_ARG_AXIS, theRefAxis); }

  Handle(GEOM_Function) GetAxis() { return _func->GetReference(REVOL_ARG_AXIS); }

  void SetBase(Handle(GEOM_Function) theRefBase) { _func->SetReference(REVOL_ARG_BASE, theRefBase); }

  Handle(GEOM_Function) GetBase() { return _func->GetReference(REVOL_ARG_BASE); }

 private:

  Handle(GEOM_Function) _func;
};
