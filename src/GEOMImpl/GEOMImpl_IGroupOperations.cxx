using namespace std;

#include "GEOMImpl_IGroupOperations.hxx"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Integer.hxx>

#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_ISubShape.hxx"

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IGroupOperations::GEOMImpl_IGroupOperations (GEOM_Engine* theEngine, int theDocID) 
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IGroupOperations::GEOMImpl_IGroupOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IGroupOperations::~GEOMImpl_IGroupOperations()
{
  MESSAGE("GEOMImpl_IGroupOperations::~GEOMImpl_IGroupOperations");
}


//=============================================================================
/*!
 *  CreateGroup
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::CreateGroup(Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum  theShapeType)
{
  SetErrorCode(KO);

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
  anArray->SetValue(1, -1);

  //Add a new Fillet object  
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theMainShape, anArray);

  //Set a GROUP type
  aGroup->SetType(GEOM_GROUP);

  //Set a sub shape type
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);
 
  //Make a Python command 
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
  aDescr = anEntry + " = IGroupOperations.CreateGroup(";
  TDF_Tool::Entry(theMainShape->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  aDescr += (TCollection_AsciiString((int)theShapeType)+")");

  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aGroup; 
}

//=============================================================================
/*!
 *  AddObject
 */
//=============================================================================
void GEOMImpl_IGroupOperations::AddObject(Handle(GEOM_Object) theGroup, int theSubShapeID)
{
  SetErrorCode(KO);
   if(theGroup.IsNull()) return;

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if(aFunction.IsNull()) return;
  
  GEOM_ISubShape aSSI(aFunction);
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if(aSeq.IsNull()) return;
  if(aSeq->Length() == 1 && aSeq->Value(1) == -1) {
    aSeq->SetValue(1, theSubShapeID);
  }
  else {
    Standard_Integer aLength = aSeq->Length();
    Handle(TColStd_HArray1OfInteger) aNewSeq = new TColStd_HArray1OfInteger(1, aLength+1);
    for(Standard_Integer i = 1; i<=aLength; i++) {
      aNewSeq->SetValue(i, aSeq->Value(i));
      if(aSeq->Value(i) == theSubShapeID) {
	SetErrorCode(ALREADY_PRESENT);
	return; //
      }
    }
    aNewSeq->SetValue(aLength+1, theSubShapeID);
    aSSI.SetIndices(aNewSeq);
  }

  SetErrorCode(OK);
  return; 
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
void GEOMImpl_IGroupOperations::RemoveObject(Handle(GEOM_Object) theGroup, int theSubShapeID)
{
  SetErrorCode(KO);
   if(theGroup.IsNull()) return;


 Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if(aFunction.IsNull()) return;
  
 
  GEOM_ISubShape aSSI(aFunction);
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if(aSeq.IsNull()) return;
  if(aSeq->Length() == 1 && aSeq->Value(1) == -1) {
    SetErrorCode(NOT_EXISTS);
    return;
  }
  else {
    Handle(TColStd_HArray1OfInteger) aNewSeq;
    Standard_Integer aLength = aSeq->Length();
    if(aLength == 1) {
      if(aSeq->Value(1) != theSubShapeID) {
	SetErrorCode(NOT_EXISTS);
	return; 	
      }
      aNewSeq = new TColStd_HArray1OfInteger(1,1);
      aNewSeq->SetValue(1, -1);
    }
    else {
      aNewSeq = new TColStd_HArray1OfInteger(1, aLength-1);
      Standard_Boolean isFound = Standard_False;
      for(Standard_Integer i = 1, k=1; i<=aLength; i++) {
	if(i == aLength && !isFound) {
	  SetErrorCode(NOT_EXISTS);
	  return; 
	}
	if(aSeq->Value(i) == theSubShapeID) {
	  isFound = Standard_True;
	  continue;
	}
	aNewSeq->SetValue(k, aSeq->Value(i));
	k++;
      }

      if(!isFound) {
	SetErrorCode(NOT_EXISTS);
	return; 
      }
    }

    aSSI.SetIndices(aNewSeq);
  }


  SetErrorCode(OK);
  return; 
}

//=============================================================================
/*!
 *  GetType
 */
//=============================================================================
TopAbs_ShapeEnum GEOMImpl_IGroupOperations::GetType(Handle(GEOM_Object) theGroup)
{
  SetErrorCode(KO);

  TDF_Label aFreeLabel = theGroup->GetFreeLabel();
  Handle(TDataStd_Integer) anAttrib;
  if(!aFreeLabel.FindAttribute(TDataStd_Integer::GetID(), anAttrib)) return TopAbs_SHAPE;
 
  SetErrorCode(OK);
  return (TopAbs_ShapeEnum) anAttrib->Get(); 
}

//=============================================================================
/*!
 *  GetMainShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::GetMainShape(Handle(GEOM_Object) theGroup)
{
  SetErrorCode(KO);

  if(theGroup.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if(aFunction.IsNull()) return NULL;
  
  GEOM_ISubShape aSSI(aFunction);
  aFunction = aSSI.GetMainShape();
  if(aFunction.IsNull()) return NULL;

  TDF_Label aLabel = aFunction->GetOwnerEntry();
  Handle(GEOM_Object) aMainShape = GEOM_Object::GetObject(aLabel);
  if(aMainShape.IsNull()) return NULL;
  
  SetErrorCode(OK);
  return aMainShape; 
}

//=============================================================================
/*!
 *  GetObjects
 */
//=============================================================================
Handle(TColStd_HArray1OfInteger) GEOMImpl_IGroupOperations::GetObjects(Handle(GEOM_Object) theGroup)
{
  SetErrorCode(KO);
  
   if(theGroup.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if(aFunction.IsNull()) return NULL;
  
  GEOM_ISubShape aSSI(aFunction);
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if(aSeq.IsNull()) return NULL;

  if(aSeq->Length() == 1 && aSeq->Value(1) == -1) {
    SetErrorCode(OK);
    return NULL;
  }

  SetErrorCode(OK);
  return aSeq;
}
