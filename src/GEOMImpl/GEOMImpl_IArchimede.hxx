//NOTE: This is an intreface to a function for the Archimede operation.


#include "GEOM_Function.hxx"

#define ARG_WEIGHT   1  
#define ARG_DENSITY  2
#define ARG_DEFLECT  3 
#define ARG_SHAPE    4

class GEOMImpl_IArchimede
{
 public:
  
  GEOMImpl_IArchimede(Handle(GEOM_Function) theFunction): _func(theFunction) {}
  
  void SetWeight(Standard_Real theWeight) { _func->SetReal(ARG_WEIGHT, theWeight); }
  
  Standard_Real GetWeight() { return _func->GetReal(ARG_WEIGHT); }
  
  void SetDensity(Standard_Real theDensity) { _func->SetReal(ARG_DENSITY, theDensity); }
  
  Standard_Real GetDensity() { return _func->GetReal(ARG_DENSITY); } 
  
  void SetDeflection(Standard_Real theDeflection) { _func->SetReal(ARG_DEFLECT, theDeflection); }
  
  Standard_Real GetDeflection() { return _func->GetReal(ARG_DEFLECT); }
  
  void SetBasicShape(Handle(GEOM_Function) theShape) { _func->SetReference(ARG_SHAPE, theShape); }

  Handle(GEOM_Function) GetBasicShape() { return _func->GetReference(ARG_SHAPE); }
  
 private:
  
  Handle(GEOM_Function) _func;  
};
