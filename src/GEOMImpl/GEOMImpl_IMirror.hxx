//NOTE: This is an intreface to a function for the Mirror creation.


#include "GEOM_Function.hxx"

#define MIRROR_ARG_ORIGINAL 1
#define MIRROR_ARG_PLANE    2
#define MIRROR_ARG_AXIS     3
#define MIRROR_ARG_POINT    4

class GEOMImpl_IMirror
{
 public:

  GEOMImpl_IMirror(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetOriginal(Handle(GEOM_Function) theShape) { _func->SetReference(MIRROR_ARG_ORIGINAL, theShape); }
  void SetPlane   (Handle(GEOM_Function) thePlane) { _func->SetReference(MIRROR_ARG_PLANE, thePlane); }
  void SetAxis    (Handle(GEOM_Function) theAxis ) { _func->SetReference(MIRROR_ARG_AXIS , theAxis ); }
  void SetPoint   (Handle(GEOM_Function) thePoint) { _func->SetReference(MIRROR_ARG_POINT, thePoint); }

  Handle(GEOM_Function) GetOriginal() { return _func->GetReference(MIRROR_ARG_ORIGINAL); }
  Handle(GEOM_Function) GetPlane()    { return _func->GetReference(MIRROR_ARG_PLANE); }
  Handle(GEOM_Function) GetAxis ()    { return _func->GetReference(MIRROR_ARG_AXIS ); }
  Handle(GEOM_Function) GetPoint()    { return _func->GetReference(MIRROR_ARG_POINT); }
  
 private:

  Handle(GEOM_Function) _func;
};
