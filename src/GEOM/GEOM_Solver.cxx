
#include "GEOM_Solver.hxx"
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TFunction_DriverTable.hxx>
#include "GEOM_Function.hxx"
#include <Standard_GUID.hxx>

//=============================================================================
/*!
 *  Update
 */
//=============================================================================
bool GEOM_Solver::Update(int theDocID, TDF_LabelSequence& theSeq)
{
  return false;
} 

//=============================================================================
/*!
 *  UpdateObject
 */
//=============================================================================
bool GEOM_Solver::UpdateObject(Handle(GEOM_Object) theObject, TDF_LabelSequence& theSeq)
{
  return false;
}

//=============================================================================
/*!
 *  ComputeFunction
 */
//=============================================================================  
bool GEOM_Solver::ComputeFunction(Handle(GEOM_Function) theFunction)
{
  if(theFunction == NULL) return false;
  Standard_GUID aGUID = theFunction->GetDriverGUID();
      
  Handle(TFunction_Driver) aDriver;
  if(!TFunction_DriverTable::Get()->FindDriver(aGUID, aDriver)) return false;
	  
  aDriver->Init(theFunction->GetEntry());
	    
  TFunction_Logbook aLog;
  if(aDriver->Execute(aLog) == 0) return false;
		
  return true;     
}
