//NOTE: This is an intreface to a function for the Setting In Position.


#include "GEOM_Function.hxx"

#define POSITION_ARG_SHAPE 1
#define POSITION_ARG_START_LCS 2
#define POSITION_ARG_END_LCS 3

class GEOMImpl_IPosition
{
 public:

  GEOMImpl_IPosition(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(POSITION_ARG_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(POSITION_ARG_SHAPE); }

  void SetStartLCS(Handle(GEOM_Function) theStartLCS)
  { _func->SetReference(POSITION_ARG_START_LCS, theStartLCS); }

  Handle(GEOM_Function) GetStartLCS() { return _func->GetReference(POSITION_ARG_START_LCS); }

  void SetEndLCS(Handle(GEOM_Function) theEndLCS)
  { _func->SetReference(POSITION_ARG_END_LCS, theEndLCS); }

  Handle(GEOM_Function) GetEndLCS() { return _func->GetReference(POSITION_ARG_END_LCS); }

 private:

  Handle(GEOM_Function) _func;
};
