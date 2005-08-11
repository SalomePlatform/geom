
#ifndef _GEOM_IOperations_HXX_
#define _GEOM_IOperations_HXX_

#include "GEOM_Engine.hxx"
#include <TDocStd_Document.hxx>
#include <TCollection_AsciiString.hxx>
#include "GEOM_Object.hxx"
#include "GEOM_Solver.hxx"

#define OK "PAL_NO_ERROR"
#define KO "PAL_NOT_DONE_ERROR"
#define ALREADY_PRESENT "PAL_ELEMENT_ALREADY_PRESENT"
#define NOT_EXISTS "PAL_ELEMENT_DOES_NOT_EXISTS"
#define INVALID_TYPE "INVALID_TYPE_OF_ELEMENT"

class GEOM_IOperations
{
 public:
  Standard_EXPORT GEOM_IOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOM_IOperations();

  //Starts a new operation (opens a tansaction)
  Standard_EXPORT void StartOperation();
 
  //Finishes the previously started operation (closes the transaction)
  Standard_EXPORT void FinishOperation();

  //Aborts the operation 
  Standard_EXPORT void AbortOperation();
 
  //Returns true if the last operation succided
  Standard_EXPORT bool IsDone();

  //Sets Not done error code
  Standard_EXPORT void SetNotDone() { _errorCode = KO; }

  //Sets an error code of the operation
  Standard_EXPORT void SetErrorCode(const TCollection_AsciiString& theErrorCode) {
    _errorCode = theErrorCode;    
  } 
 
  //Returns an error code of the last operatioin
  Standard_EXPORT char* GetErrorCode() {
    return _errorCode.ToCString();    
  }
  
  //Returns a pointer to  GEOM_Engine which this operation interface is associated
  Standard_EXPORT GEOM_Engine* GetEngine() { return _engine; }

  //Return a pointer to Solver associated with this operation interface
  Standard_EXPORT GEOM_Solver* GetSolver() { return _solver; }

  //Returns an ID of the OCAF document where this operation stores the data
  Standard_EXPORT int GetDocID() { return _docID; }

 private:
  
  TCollection_AsciiString _errorCode;
  GEOM_Engine*   _engine;
  GEOM_Solver*   _solver;
  int                     _docID;

};

#endif
