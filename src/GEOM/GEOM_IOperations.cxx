using namespace std; 

#include "GEOM_IOperations.hxx"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TDataStd_TreeNode.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDF_TagSource.hxx>

//=============================================================================
/*!
 *  default constructor:
 */
//=============================================================================

GEOM_IOperations::GEOM_IOperations(GEOM_Engine* theEngine, int theDocID)
: _engine(theEngine), _docID(theDocID)
{
  _solver = new GEOM_Solver(theEngine);
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_IOperations::~GEOM_IOperations()
{
  delete _solver;
  MESSAGE("GEOM_IOperations::~GEOM_IOperations");
}


//=============================================================================
/*!
 *  StartOperation
 */  
//=============================================================================   
void GEOM_IOperations::StartOperation()
{
  Handle(TDocStd_Document) aDoc = _engine->GetDocument(_docID);
  if(aDoc->GetUndoLimit() > 0) 
    aDoc->NewCommand();
}

 //=============================================================================
 /*!
  *  FinishOperation
  */
//=============================================================================      
void GEOM_IOperations::FinishOperation()
{
  Handle(TDocStd_Document) aDoc = _engine->GetDocument(_docID);
  if(aDoc->GetUndoLimit() > 0) 
    aDoc->CommitCommand();
}

//=============================================================================
/*!
 *  AbortOperation
 */
//=============================================================================   
void GEOM_IOperations::AbortOperation()
{
  Handle(TDocStd_Document) aDoc = _engine->GetDocument(_docID);
  aDoc->AbortCommand();
}
   
   
//=============================================================================
/*!
 *  IsDone
 */
//=============================================================================
bool GEOM_IOperations::IsDone()
{
  return (_errorCode == OK);
}  


