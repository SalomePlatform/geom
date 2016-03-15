// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_IBooleanOperations.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>

#include <GEOMImpl_Types.hxx>

#include <GEOMImpl_BooleanDriver.hxx>
#include <GEOMImpl_IBoolean.hxx>

#include <GEOMImpl_PartitionDriver.hxx>
#include <GEOMImpl_IPartition.hxx>

#include <Basics_OCCTVersion.hxx>

#include <TDF_Tool.hxx>

#include "utilities.h"

#include <Standard_Failure.hxx>
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
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeBoolean
                                  (Handle(GEOM_Object)    theShape1,
                                   Handle(GEOM_Object)    theShape2,
                                   const Standard_Integer theOp,
                                   const Standard_Boolean IsCheckSelfInte)
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
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);

  //Compute the Boolean value
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);
  pd << aBool;
  if      (theOp == 1) pd << " = geompy.MakeCommon(";
  else if (theOp == 2) pd << " = geompy.MakeCut(";
  else if (theOp == 3) pd << " = geompy.MakeFuse(";
  else if (theOp == 4) pd << " = geompy.MakeSection(";
  else {}
  pd << theShape1 << ", " << theShape2;

  if (IsCheckSelfInte) {
    pd << ", True";
  }

  pd << ")";

  SetErrorCode(OK);
  return aBool;
}

//=============================================================================
/*!
 *  MakeFuse
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeFuse
                                  (Handle(GEOM_Object)    theShape1,
                                   Handle(GEOM_Object)    theShape2,
                                   const bool             IsCheckSelfInte,
                                   const bool             IsRmExtraEdges)
{
  SetErrorCode(KO);

  if (theShape1.IsNull() || theShape2.IsNull()) return NULL;

  //Add a new Boolean object
  Handle(GEOM_Object) aBool = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);

  //Add a new Boolean function
  Handle(GEOM_Function) aFunction =
    aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_FUSE);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_BooleanDriver::GetID()) return NULL;

  GEOMImpl_IBoolean aCI (aFunction);

  Handle(GEOM_Function) aRef1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aRef2 = theShape2->GetLastFunction();

  if (aRef1.IsNull() || aRef2.IsNull()) return NULL;

  aCI.SetShape1(aRef1);
  aCI.SetShape2(aRef2);
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);
  aCI.SetRmExtraEdges(IsRmExtraEdges);

  //Compute the Boolean value
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);

  pd << aBool << " = geompy.MakeFuse(";
  pd << theShape1 << ", " << theShape2 << ", "
     << IsCheckSelfInte << ", " << IsRmExtraEdges << ")";

  SetErrorCode(OK);
  return aBool;
}

//=============================================================================
/*!
 *  MakeFuseList
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeFuseList
                  (const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const bool                                  IsCheckSelfInte,
                   const bool                                  IsRmExtraEdges)
{
  SetErrorCode(KO);

  if (theShapes.IsNull()) return NULL;

  //Add a new Boolean object
  Handle(GEOM_Object) aBool = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);

  //Add a new Boolean function
  Handle(GEOM_Function) aFunction =
    aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_FUSE_LIST);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_BooleanDriver::GetID()) return NULL;

  GEOMImpl_IBoolean aCI (aFunction);

  TCollection_AsciiString aDescription;
  Handle(TColStd_HSequenceOfTransient) aShapesSeq =
    getShapeFunctions(theShapes, aDescription);

  if (aShapesSeq.IsNull()) return NULL;

  aCI.SetShapes(aShapesSeq);
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);
  aCI.SetRmExtraEdges(IsRmExtraEdges);

  //Compute the Boolean value
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);

  pd << aBool << " = geompy.MakeFuseList([" << aDescription.ToCString() << "], "
     << IsCheckSelfInte << ", " << IsRmExtraEdges << ")";

  SetErrorCode(OK);
  return aBool;
}

//=============================================================================
/*!
 *  MakeCommonList
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeCommonList
                  (const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const Standard_Boolean IsCheckSelfInte)
{
  SetErrorCode(KO);

  if (theShapes.IsNull()) return NULL;

  //Add a new Boolean object
  Handle(GEOM_Object) aBool = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);

  //Add a new Boolean function
  Handle(GEOM_Function) aFunction =
    aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_COMMON_LIST);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_BooleanDriver::GetID()) return NULL;

  GEOMImpl_IBoolean aCI (aFunction);

  TCollection_AsciiString aDescription;
  Handle(TColStd_HSequenceOfTransient) aShapesSeq =
    getShapeFunctions(theShapes, aDescription);

  if (aShapesSeq.IsNull()) return NULL;

  aCI.SetShapes(aShapesSeq);
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);

  //Compute the Boolean value
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);

  pd << aBool <<
    " = geompy.MakeCommonList([" << aDescription.ToCString() << "]";

  if (IsCheckSelfInte) {
    pd << ", True";
  }

  pd << ")";

  SetErrorCode(OK);
  return aBool;
}

//=============================================================================
/*!
 *  MakeCutList
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBooleanOperations::MakeCutList
                  (Handle(GEOM_Object) theMainShape,
                   const Handle(TColStd_HSequenceOfTransient)& theShapes,
                   const Standard_Boolean IsCheckSelfInte)
{
  SetErrorCode(KO);

  if (theShapes.IsNull()) return NULL;

  //Add a new Boolean object
  Handle(GEOM_Object) aBool = GetEngine()->AddObject(GetDocID(), GEOM_BOOLEAN);

  //Add a new Boolean function
  Handle(GEOM_Function) aFunction =
    aBool->AddFunction(GEOMImpl_BooleanDriver::GetID(), BOOLEAN_CUT_LIST);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_BooleanDriver::GetID()) return NULL;

  GEOMImpl_IBoolean aCI (aFunction);
  Handle(GEOM_Function) aMainRef = theMainShape->GetLastFunction();

  if (aMainRef.IsNull()) return NULL;

  TCollection_AsciiString aDescription;
  Handle(TColStd_HSequenceOfTransient) aShapesSeq =
    getShapeFunctions(theShapes, aDescription);

  if (aShapesSeq.IsNull()) return NULL;

  aCI.SetShape1(aMainRef);
  aCI.SetShapes(aShapesSeq);
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);

  //Compute the Boolean value
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);

  pd << aBool << " = geompy.MakeCutList("
    << theMainShape << ", [" << aDescription.ToCString() << "]";

  if (IsCheckSelfInte) {
    pd << ", True";
  }

  pd << ")";

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
                              const Standard_Integer                      theLimit,
                              const Standard_Boolean                      theRemoveWebs,
                              const Handle(TColStd_HArray1OfInteger)&     theMaterials,
                              const Standard_Integer theKeepNonlimitShapes,
                              const Standard_Boolean thePerformSelfIntersections,
                              const Standard_Boolean IsCheckSelfInte)
{
  SetErrorCode(KO);

  //Add a new Partition object
  Handle(GEOM_Object) aPartition = GetEngine()->AddObject(GetDocID(), GEOM_PARTITION);

  //Add a new Partition function
  Handle(GEOM_Function) aFunction;
  if (thePerformSelfIntersections)
    aFunction = aPartition->AddFunction(GEOMImpl_PartitionDriver::GetID(), PARTITION_PARTITION);
  else
    aFunction = aPartition->AddFunction(GEOMImpl_PartitionDriver::GetID(), PARTITION_NO_SELF_INTERSECTIONS);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PartitionDriver::GetID()) return NULL;

  GEOMImpl_IPartition aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq;
  Handle(TColStd_HSequenceOfTransient) aToolsSeq;
  Handle(TColStd_HSequenceOfTransient) aKeepInsSeq;
  Handle(TColStd_HSequenceOfTransient) aRemInsSeq;
  TCollection_AsciiString aShapesDescr, aToolsDescr, aKeepInsDescr, aRemoveInsDescr;

  // Shapes
  aShapesSeq = getShapeFunctions(theShapes, aShapesDescr);

  if (aShapesSeq.IsNull()) {
    SetErrorCode("NULL shape for Partition");
    return NULL;
  }

  // Tools
  aToolsSeq = getShapeFunctions(theTools, aToolsDescr);

  if (aToolsSeq.IsNull()) {
    SetErrorCode("NULL tool shape for Partition");
    return NULL;
  }

  // Keep Inside
  aKeepInsSeq = getShapeFunctions(theKeepIns, aKeepInsDescr);

  if (aKeepInsSeq.IsNull()) {
    SetErrorCode("NULL <keep inside> shape for Partition");
    return NULL;
  }

  // Remove Inside
  aRemInsSeq  = getShapeFunctions(theRemoveIns, aRemoveInsDescr);

  if (aRemInsSeq.IsNull()) {
    SetErrorCode("NULL <remove inside> shape for Partition");
    return NULL;
  }

  aCI.SetShapes(aShapesSeq);
  aCI.SetTools(aToolsSeq);
  aCI.SetKeepIns(aKeepInsSeq);
  aCI.SetRemoveIns(aRemInsSeq);

  // Limit
  aCI.SetLimit(theLimit);
  aCI.SetKeepNonlimitShapes(theKeepNonlimitShapes);
  aCI.SetCheckSelfIntersection(IsCheckSelfInte);

  // Materials
  if (theRemoveWebs) {
    if (theMaterials.IsNull()) {
      Handle(TColStd_HArray1OfInteger) aMaterials =
        new TColStd_HArray1OfInteger (1, aShapesSeq->Length());
      aMaterials->Init(0);
      aCI.SetMaterials(aMaterials);
    } else {
      aCI.SetMaterials(theMaterials);
    }
  }

  //Compute the Partition
  try {
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);
  if (thePerformSelfIntersections)
    pd << aPartition << " = geompy.MakePartition([";
  else
    pd << aPartition << " = geompy.MakePartitionNonSelfIntersectedShape([";

  // Shapes, Tools
  pd << aShapesDescr.ToCString() << "], [" << aToolsDescr.ToCString() << "], [";
  // Keep Ins, Remove Ins
  pd << aKeepInsDescr.ToCString() << "], [" << aRemoveInsDescr.ToCString() << "], ";
  // Limit, Remove Webs
  pd << TopAbs_ShapeEnum(theLimit) << ", " << (int)theRemoveWebs << ", [";
  // Materials
  if (!theMaterials.IsNull() && theMaterials->Length() > 0) {
    int i = theMaterials->Lower();
    pd << theMaterials->Value(i);
    i++;
    for (; i <= theMaterials->Upper(); i++) {
      pd << ", " << theMaterials->Value(i);
    }
  }
  pd << "], " << theKeepNonlimitShapes;

  if (IsCheckSelfInte && !thePerformSelfIntersections) {
    pd << ", True";
  }

  pd << ")";

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
  Handle(GEOM_Object) aPart = GetEngine()->AddObject(GetDocID(), GEOM_PARTITION);

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
    OCC_CATCH_SIGNALS;
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
  GEOM::TPythonDump pd (aFunction);
  pd << aPart << " = geompy.MakeHalfPartition("
     << theShape << ", " << thePlane << ")";

  SetErrorCode(OK);
  return aPart;
}

//=============================================================================
/*!
 *  getShapeFunctions
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient)
  GEOMImpl_IBooleanOperations::getShapeFunctions
                  (const Handle(TColStd_HSequenceOfTransient)& theObjects,
                         TCollection_AsciiString &theDescription)
{
  Handle(TColStd_HSequenceOfTransient) aResult =
    new TColStd_HSequenceOfTransient;
  Standard_Integer aNbObjects = theObjects->Length();
  Standard_Integer i;
  TCollection_AsciiString anEntry;
  Handle(GEOM_Object) anObj;
  Handle(GEOM_Function) aRefObj;

  // Shapes
  for (i = 1; i <= aNbObjects; i++) {
    anObj = Handle(GEOM_Object)::DownCast(theObjects->Value(i));
    aRefObj = anObj->GetLastFunction();

    if (aRefObj.IsNull()) {
      aResult.Nullify();
      break;
    }

    aResult->Append(aRefObj);

    // For Python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);

    if (i > 1) {
      theDescription += ", ";
    }

    theDescription += anEntry;
  }

  return aResult;
}
