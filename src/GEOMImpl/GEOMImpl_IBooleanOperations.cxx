using namespace std; 

#include "GEOMImpl_IBooleanOperations.hxx"

#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

#include "GEOMImpl_BooleanDriver.hxx"
#include "GEOMImpl_IBoolean.hxx"

#include "GEOMImpl_PartitionDriver.hxx"
#include "GEOMImpl_IPartition.hxx"

#include <TDF_Tool.hxx>

#include "utilities.h"

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IBooleanOperations::GEOMImpl_IBooleanOperations (GEOM_Engine* theEngine, int theDocID) 
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IBooleanOperations::GEOMImpl_IBooleanOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IBooleanOperations::~GEOMImpl_IBooleanOperations()
{
  MESSAGE("GEOMImpl_IBooleanOperations::~GEOMImpl_IBooleanOperations");
}


//=============================================================================
/*!
 *  MakeBoolean
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeBoolean (Handle(GEOM_Object) theShape1,
                                                              Handle(GEOM_Object) theShape2,
                                                              Standard_Integer    theOp)
{
  SetErrorCode(KO);

  if (theShape1.IsNull() || theShape2.IsNull()) return NULL;
 
  //Add a new Boolean object  
  Handle(GEOM_Object) aBool = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);
 
  //Add a new Boolean function
  Handle(GEOM_Function) aFunction;
  if (theOp == 1) {
    aFunction = aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_COMMON);
  } else if (theOp == 2) {
    aFunction = aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_CUT);
  } else if (theOp == 3) {
    aFunction = aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_FUSE);
  } else if (theOp == 4) {
    aFunction = aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_SECTION);
  } else {
  }
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_BooleanDriver::GetID()) return NULL; 

  GEOMImpl_IBoolean aCI (aFunction);
  
  Handle(GEOM_Function) aRef1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aRef2 = theShape2->GetLastFunction();

  if (aRef1.IsNull() || aRef2.IsNull()) return NULL;

  aCI.SetShape1(aRef1);
  aCI.SetShape2(aRef2);

  //Compute the Boolean value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Boolean driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command 
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aBool->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IBooleanOperations.MakeBoolean(";
  TDF_Tool::Entry(theShape1->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(theShape2->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  aDescr += (TCollection_AsciiString(theOp)+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aBool; 
}

//=============================================================================
/*!
 *  MakePartition
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakePartition
                             (const Handle(TColStd_HSequenceOfTransient)& theShapes, 
                              const Handle(TColStd_HSequenceOfTransient)& theTools,
                              const Handle(TColStd_HSequenceOfTransient)& theKeepIns,
                              const Handle(TColStd_HSequenceOfTransient)& theRemoveIns,
                              const Standard_Integer           theLimit,
                              const Standard_Boolean           theRemoveWebs,
                              const Handle(TColStd_HArray1OfInteger)& theMaterials)
{
  SetErrorCode(KO);

  //Add a new Partition object  
  Handle(GEOM_Object) aPartition = GetEngine()->AddObject(GetDocID(), GEOM_PARTITION);
 
  //Add a new Partition function
  Handle(GEOM_Function) aFunction =
    aPartition->AddFunction(GEOMImpl_PartitionDriver::GetID(), PARTITION_PARTITION);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PartitionDriver::GetID()) return NULL; 

  GEOMImpl_IPartition aCI (aFunction);
  
//  int aLen = theShapes.size();
//  aCI.SetLength(aLen);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq  = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aToolsSeq   = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aKeepInsSeq = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aRemInsSeq  = new TColStd_HSequenceOfTransient;

  Standard_Integer ind, aLen;
  TCollection_AsciiString anEntry;
  TCollection_AsciiString aShapesDescr, aToolsDescr, aKeepInsDescr, aRemoveInsDescr;

  // Shapes
  aLen = theShapes->Length();
  for (ind = 1; ind <= aLen; ind++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(theShapes->Value(ind));
    Handle(GEOM_Function) aRefSh = anObj->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL shape for Partition");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);

    // For Python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    if (ind > 1) aShapesDescr += ", ";
    aShapesDescr += anEntry;
  }
  aCI.SetShapes(aShapesSeq);

  // Tools
  aLen = theTools->Length();
  for (ind = 1; ind <= aLen; ind++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(theTools->Value(ind));
    Handle(GEOM_Function) aRefSh = anObj->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL tool shape for Partition");
      return NULL;
    }
    aToolsSeq->Append(aRefSh);

    // For Python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    if (ind > 1) aToolsDescr += ", ";
    aToolsDescr += anEntry;
  }
  aCI.SetTools(aToolsSeq);

  // Keep Inside
  aLen = theKeepIns->Length();
  for (ind = 1; ind <= aLen; ind++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(theKeepIns->Value(ind));
    Handle(GEOM_Function) aRefSh = anObj->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL <keep inside> shape for Partition");
      return NULL;
    }
    aKeepInsSeq->Append(aRefSh);

    // For Python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    if (ind > 1) aKeepInsDescr += ", ";
    aKeepInsDescr += anEntry;
  }
  aCI.SetKeepIns(aKeepInsSeq);

  // Remove Inside
  aLen = theRemoveIns->Length();
  for (ind = 1; ind <= aLen; ind++) {
    Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast(theRemoveIns->Value(ind));
    Handle(GEOM_Function) aRefSh = anObj->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL <remove inside> shape for Partition");
      return NULL;
    }
    aRemInsSeq->Append(aRefSh);

    // For Python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    if (ind > 1) aRemoveInsDescr += ", ";
    aRemoveInsDescr += anEntry;
  }
  aCI.SetRemoveIns(aRemInsSeq);

  // Limit
  aCI.SetLimit(theLimit);

  // Materials
  if (theRemoveWebs) {
    if (theMaterials.IsNull()) {
      Handle(TColStd_HArray1OfInteger) aMaterials =
        new TColStd_HArray1OfInteger (1, aShapesSeq->Length());
      aCI.SetMaterials(aMaterials);
    } else {
      aCI.SetMaterials(theMaterials);
    }
  }

  //Compute the Partition
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Partition driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command 
  TCollection_AsciiString aDescr;
  TDF_Tool::Entry(aPartition->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IBooleanOperations.MakePartition([";
  // Shapes
  aDescr += aShapesDescr + "], [";
  // Tools
  aDescr += aToolsDescr + "], [";
  // Keep Ins
  aDescr += aKeepInsDescr + "], [";
  // Remove Ins
  aDescr += aRemoveInsDescr + "], ";
  // Limit
  aDescr += TCollection_AsciiString(theLimit) + ", ";
  // Remove Webs
  if (theRemoveWebs) aDescr += "1, [";
  else aDescr += "0, [";
  // Materials
  if (theMaterials->Length() > 0) {
    int i = theMaterials->Lower();
    aDescr += TCollection_AsciiString(theMaterials->Value(i));
    i++;
    for (; i <= theMaterials->Upper(); i++) {
      aDescr += ", ";
      aDescr += TCollection_AsciiString(theMaterials->Value(i));
    }
  }
  aDescr += "])";

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aPartition; 
}

//=============================================================================
/*!
 *  MakeHalfPartition
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeHalfPartition
       (Handle(GEOM_Object) theShape, Handle(GEOM_Object) thePlane)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || thePlane.IsNull()) return NULL;
 
  //Add a new Boolean object  
  Handle(GEOM_Object) aPart = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);
 
  //Add a new Partition function
  Handle(GEOM_Function) aFunction =
    aPart->AddFunction(GEOMImpl_PartitionDriver::GetID(), PARTITION_HALF);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PartitionDriver::GetID()) return NULL; 

  GEOMImpl_IPartition aCI (aFunction);
  
  Handle(GEOM_Function) aRef1 = theShape->GetLastFunction();
  Handle(GEOM_Function) aRef2 = thePlane->GetLastFunction();

  if (aRef1.IsNull() || aRef2.IsNull()) return NULL;

  aCI.SetShape(aRef1);
  aCI.SetPlane(aRef2);

  //Compute the Partition value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Partition driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command 
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aPart->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IBooleanOperations.MakePartition(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePlane->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aPart; 
}
