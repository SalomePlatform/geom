
#ifndef _GEOM_Solver_HXX_
#define _GEOM_Solver_HXX_

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Function.hxx"

#include <TDF_LabelSequence.hxx>

class GEOM_Solver
{
 public:
  GEOM_Solver(GEOM_Engine* theEngine) :_engine(theEngine) {}
  ~GEOM_Solver() {}

  //Update the values of all GEOM_Object in the document theDocID, theSeq will contain a list of touched labels
  //Note: not Implemented
  bool Update(int theDocID, TDF_LabelSequence& theSeq); 
 
  //Updates a value of theObject, theSeq will contain a list of touched labels
  //Note: not implemented
  bool UpdateObject(Handle(GEOM_Object) theObject, TDF_LabelSequence& theSeq);

  //Recomputes a function
  bool ComputeFunction(Handle(GEOM_Function) theFunction);

 private:

  GEOM_Engine* _engine;    

};

#endif
