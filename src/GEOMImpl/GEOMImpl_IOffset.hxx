//NOTE: This is an intreface to a function for the Offset creation.


#include "GEOM_Function.hxx"

#define OFF_ARG_SHAPE 1
#define OFF_ARG_VALUE 2

class GEOMImpl_IOffset
{
 public:

  GEOMImpl_IOffset(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(OFF_ARG_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(OFF_ARG_SHAPE); }

  void SetValue(double theValue) { _func->SetReal(OFF_ARG_VALUE, theValue); }

  double GetValue() { return _func->GetReal(OFF_ARG_VALUE); }

 private:

  Handle(GEOM_Function) _func;
};
