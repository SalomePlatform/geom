//NOTE: This is an interface to a function for the Sketcher creation.


#include "GEOM_Function.hxx"

#define SKETCH_ARG_CMD 1
#define SKETCH_ARG_WPLANE 2

class GEOMImpl_ISketcher
{
 public:

  GEOMImpl_ISketcher(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetCommand(const TCollection_AsciiString& theCommand)
  { _func->SetString(SKETCH_ARG_CMD, theCommand); }

  TCollection_AsciiString GetCommand() { return _func->GetString(SKETCH_ARG_CMD); }

  void SetWorkingPlane(int theInd, double theValue)
              { _func->SetReal(SKETCH_ARG_WPLANE + theInd, theValue); }

  double GetWorkingPlane(int theInd) { return _func->GetReal(SKETCH_ARG_WPLANE + theInd); }

 private:

  Handle(GEOM_Function) _func;
};
