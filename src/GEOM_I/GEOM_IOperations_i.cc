using namespace std; 

#include "GEOM_IOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"

#include "GEOM_Gen_i.hh"

#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

//=============================================================================
/*!
 *  default constructor:
 */
//=============================================================================

GEOM_IOperations_i::GEOM_IOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOM_IOperations* theImpl)
:SALOME::GenericObj_i( thePOA ), _impl(theImpl), _engine(theEngine)
{
  thePOA->activate_object(this);
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_IOperations_i::~GEOM_IOperations_i()
{}


//=============================================================================
/*!  
 *  IsDone
 */
//============================================================================= 
CORBA::Boolean GEOM_IOperations_i::IsDone()
{
  return _impl->IsDone();
}  


//=============================================================================
/*!    
 *  SetErrorCode
 */
//============================================================================= 
void GEOM_IOperations_i::SetErrorCode(const char* theErrorCode)
{
  _impl->SetErrorCode((char*)theErrorCode);
}
 
//=============================================================================
/*!
 *  GetErrorCode
 */
//============================================================================= 
char* GEOM_IOperations_i::GetErrorCode() 
{
  return _impl->GetErrorCode();    
}
   
//=============================================================================
/*!
 *  GetStudyID
 */
//============================================================================= 
CORBA::Long GEOM_IOperations_i::GetStudyID() 
{
  return _impl->GetDocID();    
}
   
//=============================================================================
/*!
 *  StartOperation
 */
//=============================================================================
void GEOM_IOperations_i::StartOperation()
{
  _impl->StartOperation();
}

//=============================================================================
/*!
 *  FinishOperation
 */
//=============================================================================
void GEOM_IOperations_i::FinishOperation()
{
  _impl->FinishOperation();
}


//=============================================================================
/*!
 *  AboutOperation
 */
//=============================================================================
void GEOM_IOperations_i::AbortOperation()
{
  _impl->AbortOperation();
}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IOperations_i::GetObject(Handle(GEOM_Object) theObject)         
{
  if(theObject.IsNull()) return NULL;
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theObject->GetEntry(), anEntry);
  GEOM::GEOM_Object_var GO = GEOM::GEOM_Object::_duplicate(_engine->GetObject(theObject->GetDocID(), anEntry.ToCString()));
  return GO._retn();
}  

