//NOTE: This is an intreface to a function for the Scale creation.


#include "GEOM_Function.hxx"

#define SCALE_ARG_SHAPE 1
#define SCALE_ARG_VALUE 2
#define SCALE_ARG_POINT 3

class GEOMImpl_IScale
{
 public:

  GEOMImpl_IScale(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(SCALE_ARG_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(SCALE_ARG_SHAPE); }

  void SetFactor(double theValue) { _func->SetReal(SCALE_ARG_VALUE, theValue); }

  double GetFactor() { return _func->GetReal(SCALE_ARG_VALUE); }

  void SetPoint(Handle(GEOM_Function) thePoint) { _func->SetReference(SCALE_ARG_POINT, thePoint); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(SCALE_ARG_POINT); }

 private:

  Handle(GEOM_Function) _func;
};
