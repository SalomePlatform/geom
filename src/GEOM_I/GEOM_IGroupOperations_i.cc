using namespace std; 

#include "GEOM_IGroupOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IGroupOperations_i::GEOM_IGroupOperations_i (PortableServer::POA_ptr thePOA,
						  GEOM::GEOM_Gen_ptr theEngine,
						  ::GEOMImpl_IGroupOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IGroupOperations_i::GEOM_IGroupOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IGroupOperations_i::~GEOM_IGroupOperations_i()
{
  MESSAGE("GEOM_IGroupOperations_i::~GEOM_IGroupOperations_i");
}


//=============================================================================
/*!
 *  CreateGroup
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_IGroupOperations_i::CreateGroup(GEOM::GEOM_Object_ptr theMainShape, CORBA::Long theShapeType)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theMainShape == NULL || theShapeType < 0) return aGEOMObject._retn();

  //Get the reference shape
  Handle(GEOM_Object) aShapeRef = GetOperations()->GetEngine()->GetObject(theMainShape->GetStudyID(), theMainShape->GetEntry());

  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Fillet
  Handle(GEOM_Object) anObject = GetOperations()->CreateGroup(aShapeRef, (TopAbs_ShapeEnum)theShapeType);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  AddObject
 */
//=============================================================================
void GEOM_IGroupOperations_i::AddObject(GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId) 
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theGroup == NULL) return;

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetOperations()->GetEngine()->GetObject(theGroup->GetStudyID(), theGroup->GetEntry());
  if (aGroupRef.IsNull()) return;

  GetOperations()->AddObject(aGroupRef, theSubShapeId);
  return;
}

//=============================================================================
/*!
 *  RemoveObject
 */
//============================================================================= 
void GEOM_IGroupOperations_i::RemoveObject(GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId) 
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theGroup == NULL) return;

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetOperations()->GetEngine()->GetObject(theGroup->GetStudyID(), theGroup->GetEntry());
  if (aGroupRef.IsNull()) return;

  GetOperations()->RemoveObject(aGroupRef, theSubShapeId);

  return;
}

//=============================================================================
/*!
 *  GetType
 */
//============================================================================= 
CORBA::Long GEOM_IGroupOperations_i::GetType(GEOM::GEOM_Object_ptr theGroup)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theGroup == NULL) return -1;

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetOperations()->GetEngine()->GetObject(theGroup->GetStudyID(), theGroup->GetEntry());

  if (aGroupRef.IsNull()) return -1;

  return GetOperations()->GetType(aGroupRef);
}

//=============================================================================
/*!
 *  GetMainShape
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_IGroupOperations_i::GetMainShape(GEOM::GEOM_Object_ptr theGroup)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theGroup == NULL) return aGEOMObject._retn();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetOperations()->GetEngine()->GetObject(theGroup->GetStudyID(), theGroup->GetEntry());
  if (aGroupRef.IsNull()) return aGEOMObject._retn();

  Handle(GEOM_Object) anObject = GetOperations()->GetMainShape(aGroupRef);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetObjects
 */
//============================================================================= 
GEOM::ListOfLong* GEOM_IGroupOperations_i::GetObjects(GEOM::GEOM_Object_ptr theGroup)
{
  GEOM::ListOfLong_var aList;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theGroup == NULL) return aList._retn();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetOperations()->GetEngine()->GetObject(theGroup->GetStudyID(), theGroup->GetEntry());
  if (aGroupRef.IsNull()) return aList._retn();

  aList = new GEOM::ListOfLong;    

  Handle(TColStd_HArray1OfInteger) aSeq = GetOperations()->GetObjects(aGroupRef);
  if (!GetOperations()->IsDone() || aSeq.IsNull()) return aList._retn();
    
  aList->length(aSeq->Length());
  for(int i = 1; i<=aSeq->Length(); i++) aList[i-1] = aSeq->Value(i);

  return aList._retn();
}

