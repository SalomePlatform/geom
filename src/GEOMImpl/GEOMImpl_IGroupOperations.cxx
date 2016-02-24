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

#include <GEOMImpl_IGroupOperations.hxx>

#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_ISubShape.hxx>
#include <GEOM_PythonDump.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Integer.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

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
Handle(GEOM_Object) GEOMImpl_IGroupOperations::CreateGroup
       (Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum theShapeType)
{
  SetErrorCode(KO);

  if ( theShapeType != TopAbs_VERTEX && theShapeType != TopAbs_EDGE &&
       theShapeType != TopAbs_FACE && theShapeType != TopAbs_SOLID ) {
    SetErrorCode( "Error: You could create group of only next type: vertex, edge, face or solid" );
    return NULL;
  }

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
  anArray->SetValue(1, -1);

  //Add a new Sub-shape object
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theMainShape, anArray);

  //Set a GROUP type
  aGroup->SetType(GEOM_GROUP);

  //Set a sub-shape type
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);

  //Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);

  GEOM::TPythonDump(aFunction) << aGroup
    << " = geompy.CreateGroup(" << theMainShape << ", " << theShapeType << ")";

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

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if(aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // Check sub-shape index validity
  TDF_Label aLabel = aSSI.GetMainShape()->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  TopoDS_Shape aMainShape = aMainObj->GetValue();
  if (aMainShape.IsNull()) return;

  TopTools_IndexedMapOfShape aMapOfShapes;
  TopExp::MapShapes(aMainShape, aMapOfShapes);

  TopAbs_ShapeEnum aGroupType = GetType(theGroup);
  TopAbs_ShapeEnum aShapeType = aMapOfShapes.FindKey(theSubShapeID).ShapeType();
  if ( aGroupType != aShapeType ) {
    SetErrorCode( "Error: You could perform this operation only with object the same type as the type of group." );
    return;
  }

  if (theSubShapeID < 1 || aMapOfShapes.Extent() < theSubShapeID) {
    SetErrorCode("Invalid sub-shape index: out of range");
    return;
  }

  // Add sub-shape index
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
    if ( aFunction->IsLastFuntion() ) {
      aSSI.SetIndices(aNewSeq);
    }
    else {
      aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
      GEOM_ISubShape aSSI2 (aFunction);
      aSSI2.SetIndices(aNewSeq);
      aSSI2.SetMainShape( aSSI.GetMainShape() );
    }
  }

  // As we do not recompute here our group, lets mark it as Modified
  Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
  theGroup->SetTic(aTic - 1);

  //Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "geompy.AddObject(" << theGroup << ", " << theSubShapeID << ")";

  SetErrorCode(OK);
  return;
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
void GEOMImpl_IGroupOperations::RemoveObject (Handle(GEOM_Object) theGroup, int theSubShapeID)
{
  SetErrorCode(KO);
  if(theGroup.IsNull()) return;

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if(aFunction.IsNull()) return;

  GEOM_ISubShape aSSI(aFunction);
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if(aSeq.IsNull()) return;

  if(aSeq->Length() == 1 && aSeq->Value(1) == -1) {
    SetErrorCode(NOT_EXISTS);
    return;
  }

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
    for (Standard_Integer i = 1, k = 1; i <= aLength; i++) {
      if (aSeq->Value(i) == theSubShapeID) {
        isFound = Standard_True;
      } else {
        if (k < aLength) { // this check is to avoid sequence <aNewSeq> overflow
          aNewSeq->SetValue(k, aSeq->Value(i));
          k++;
        }
      }
    }

    if (!isFound) {
      SetErrorCode(NOT_EXISTS);
      return;
    }
  }

  if ( aFunction->IsLastFuntion() ) {
    aSSI.SetIndices(aNewSeq);
  }
  else {
    aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
    GEOM_ISubShape aSSI2 (aFunction);
    aSSI2.SetIndices(aNewSeq);
    aSSI2.SetMainShape( aSSI.GetMainShape() );
  }

  // As we do not recompute here our group, lets mark it as Modified
  TDF_Label aLabel = aSSI.GetMainShape()->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
  theGroup->SetTic(aTic - 1);

  //Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "geompy.RemoveObject(" << theGroup << ", " << theSubShapeID << ")";

  SetErrorCode(OK);
  return;
}

//=============================================================================
/*!
 *  UnionList
 */
//=============================================================================
void GEOMImpl_IGroupOperations::UnionList (Handle(GEOM_Object) theGroup,
                                           const Handle(TColStd_HSequenceOfTransient)& theSubShapes)
{
  SetErrorCode(KO);
  if (theGroup.IsNull()) return;

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Standard_Integer aLen = theSubShapes->Length();
  if (aLen < 1) {
    //SetErrorCode("The list is empty");
    SetErrorCode(OK);
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // New contents of the group
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  // Add current IDs to the list
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer val_j, aLength = aSeq->Length();

  for (Standard_Integer j = 1; j <= aLength; j++) {
    val_j = aSeq->Value(j);
    if (val_j > 0 && mapIDs.Add(val_j)) {
      aNewIDs.Append(val_j);
    }
  }

  // Get Main Shape
  Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
  if (aMainShapeFunc.IsNull()) return;
  TDF_Label aLabel = aMainShapeFunc->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  TopoDS_Shape aMainShape = aMainObj->GetValue();
  if (aMainShape.IsNull()) return;

  TopTools_IndexedMapOfShape mapIndices;
  TopExp::MapShapes(aMainShape, mapIndices);

  // Get group type
  TopAbs_ShapeEnum aType = GetType(theGroup);

  // Get IDs of sub-shapes to add
  Standard_Integer i, new_id;
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));

    TopoDS_Shape aShape_i = anObj_i->GetValue();
    if ( aShape_i.IsNull() ) continue;
    TopAbs_ShapeEnum aType_i = aShape_i.ShapeType();

    // 1. If aShape_i is sub-shape of aMainShape - add it
    if (anObj_i->IsMainShape()) {
      if (aType_i != aType && aType != TopAbs_SHAPE && aType != TopAbs_COMPOUND) {
        SetErrorCode("Operation aborted: one of given objects has a wrong type");
        return;
      }
      if (!mapIndices.Contains(aShape_i)) {
        SetErrorCode("Operation aborted: not a sub-shape given");
        return;
      }
      new_id = mapIndices.FindIndex(aShape_i);
      if (mapIDs.Add(new_id)) {
        aNewIDs.Append(new_id);
      }
    }
    // 2. If type of group is not defined - add all sub-shapes of aShape_i
    else if (aType == TopAbs_SHAPE || aType == TopAbs_COMPOUND) {
      TopTools_IndexedMapOfShape mapIndices_i;
      TopExp::MapShapes(aShape_i, mapIndices_i);
      Standard_Integer ii = 1, nbSubSh = mapIndices_i.Extent();
      Standard_Boolean someGood = Standard_False;
      for (; ii <= nbSubSh; ii++) {
        TopoDS_Shape aSubShape_i = mapIndices_i.FindKey(ii);
        if (mapIndices.Contains(aSubShape_i)) {
          someGood = Standard_True;
          new_id = mapIndices.FindIndex(aSubShape_i);
          if (mapIDs.Add(new_id)) {
            aNewIDs.Append(new_id);
          }
        }
      }
      if (!someGood) {
        SetErrorCode("Operation aborted: not a sub-shape given");
        return;
      }
    }
    // 3. If type of group is defined - add all sub-shapes of aShape_i of that type
    else {
      TopExp_Explorer aSubShapes_i (aShape_i, aType);
      for (; aSubShapes_i.More(); aSubShapes_i.Next()) {
        TopoDS_Shape aSubShape_i = aSubShapes_i.Current();
        if (!mapIndices.Contains(aSubShape_i)) {
          SetErrorCode("Operation aborted: not a sub-shape given");
          return;
        }
        new_id = mapIndices.FindIndex(aSubShape_i);
        if (mapIDs.Add(new_id)) {
          aNewIDs.Append(new_id);
        }
      }
    }
  }

  if (aNewIDs.Extent() > 0) {
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    Handle(TColStd_HArray1OfInteger) aNewSeq = new TColStd_HArray1OfInteger(1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewSeq->SetValue(k, aNewIDsIter.Value());
    }
    if ( aFunction->IsLastFuntion() ) {
      aSSI.SetIndices(aNewSeq);
    }
    else {
      aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
      GEOM_ISubShape aSSI2 (aFunction);
      aSSI2.SetIndices(aNewSeq);
      aSSI2.SetMainShape(aMainShapeFunc);
    }
    // As we do not recompute here our group, lets mark it as Modified
    Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
    theGroup->SetTic(aTic - 1);
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "geompy.UnionList(" << theGroup << ", [";

  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));
    pd << anObj_i << (( i < aLen ) ? ", " : "])");
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  DifferenceList
 */
//=============================================================================
void GEOMImpl_IGroupOperations::DifferenceList (Handle(GEOM_Object) theGroup,
                                                const Handle(TColStd_HSequenceOfTransient)& theSubShapes)
{
  SetErrorCode(KO);
  if (theGroup.IsNull()) return;

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Standard_Integer aLen = theSubShapes->Length();
  if (aLen < 1) {
    //SetErrorCode("The list is empty");
    SetErrorCode(OK);
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // Map of IDs to be removed
  TColStd_MapOfInteger mapIDsToRemove;

  // Map of current IDs
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer aLength = aSeq->Length();

  // VSR 28/04/2011 commented to allow operation even for empty group
  //   if (aLength == 1 && aSeq->Value(1) == -1) // empty group
  //     return;

  TColStd_MapOfInteger mapIDsCurrent;
  Standard_Integer j = 1;
  for (; j <= aLength; j++) {
    mapIDsCurrent.Add(aSeq->Value(j));
  }

  // Get Main Shape
  Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
  if (aMainShapeFunc.IsNull()) return;
  TDF_Label aLabel = aMainShapeFunc->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  TopoDS_Shape aMainShape = aMainObj->GetValue();
  if (aMainShape.IsNull()) return;

  TopTools_IndexedMapOfShape mapIndices;
  TopExp::MapShapes(aMainShape, mapIndices);

  // Get group type
  TopAbs_ShapeEnum aType = GetType(theGroup);

  // Get IDs of sub-shapes to be removed
  Standard_Integer i, rem_id;
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));

    TopoDS_Shape aShape_i = anObj_i->GetValue();

    // 1. If aShape_i is sub-shape of aMainShape - remove it
    if (mapIndices.Contains(aShape_i)) {
      rem_id = mapIndices.FindIndex(aShape_i);
      if (rem_id > 0 && mapIDsCurrent.Contains(rem_id)) {
        mapIDsToRemove.Add(rem_id);
      }
    }
    // 2. If type of group is not defined - remove all sub-shapes of aShape_i
    else if (aType == TopAbs_SHAPE || aType == TopAbs_COMPOUND) {
      TopTools_IndexedMapOfShape mapIndices_i;
      TopExp::MapShapes(aShape_i, mapIndices_i);
      Standard_Integer nbSubSh = mapIndices_i.Extent();
      Standard_Integer ii = 1;
      for (; ii <= nbSubSh; ii++) {
        TopoDS_Shape aSubShape_i = mapIndices_i.FindKey(ii);
        if (mapIndices.Contains(aSubShape_i)) {
          rem_id = mapIndices.FindIndex(aSubShape_i);
          if (rem_id > 0 && mapIDsCurrent.Contains(rem_id)) {
            mapIDsToRemove.Add(rem_id);
          }
        }
      }
    }
    // 3. If type of group is defined - remove all sub-shapes of aShape_i of that type
    else {
      TopExp_Explorer aSubShapes_i (aShape_i, aType);
      for (; aSubShapes_i.More(); aSubShapes_i.Next()) {
        TopoDS_Shape aSubShape_i = aSubShapes_i.Current();
        if (mapIndices.Contains(aSubShape_i)) {
          rem_id = mapIndices.FindIndex(aSubShape_i);
          if (rem_id > 0 && mapIDsCurrent.Contains(rem_id)) {
            mapIDsToRemove.Add(rem_id);
          }
        }
      }
    }
  }

  if (mapIDsToRemove.Extent() > 0) {
    Standard_Integer k = 1, aRemLength = mapIDsToRemove.Extent();
    Handle(TColStd_HArray1OfInteger) aNewSeq;
    if ( aLength - aRemLength > 0 ) {
      aNewSeq = new TColStd_HArray1OfInteger(1, aLength - aRemLength);
      for (j = 1; j <= aLength; j++) {
        if (!mapIDsToRemove.Contains(aSeq->Value(j))) {
          aNewSeq->SetValue(k, aSeq->Value(j));
          k++;
        }
      }
    }
    else {
      aNewSeq = new TColStd_HArray1OfInteger(1,1);
      aNewSeq->SetValue(1, -1);
    }

    if ( aFunction->IsLastFuntion() ) {
      aSSI.SetIndices(aNewSeq);
    }
    else {
      aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
      GEOM_ISubShape aSSI2 (aFunction);
      aSSI2.SetIndices(aNewSeq);
      aSSI2.SetMainShape(aMainShapeFunc);
    }
    // As we do not recompute here our group, lets mark it as Modified
    Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
    theGroup->SetTic(aTic - 1);
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "geompy.DifferenceList(" << theGroup << ", [";

  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));
    pd << anObj_i << (( i < aLen ) ? ", " : "])");
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  UnionIDs
 */
//=============================================================================
void GEOMImpl_IGroupOperations::UnionIDs (Handle(GEOM_Object) theGroup,
                                          const Handle(TColStd_HSequenceOfInteger)& theSubShapes)
{
  SetErrorCode(KO);
  if (theGroup.IsNull()) return;

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Standard_Integer aLen = theSubShapes->Length();
  if (aLen < 1) {
    //SetErrorCode("The list is empty");
    SetErrorCode(OK);
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // New contents of the group
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  // Add current IDs to the list
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer val_j, aLength = aSeq->Length();

  for (Standard_Integer j = 1; j <= aLength; j++) {
    val_j = aSeq->Value(j);
    if (val_j > 0 && mapIDs.Add(val_j)) {
      aNewIDs.Append(val_j);
    }
  }

  // Get Main Shape
  Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
  if (aMainShapeFunc.IsNull()) return;
  TDF_Label aLabel = aMainShapeFunc->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  TopoDS_Shape aMainShape = aMainObj->GetValue();
  if (aMainShape.IsNull()) return;

  TopTools_IndexedMapOfShape mapIndices;
  TopExp::MapShapes(aMainShape, mapIndices);

  // Get group type
  TopAbs_ShapeEnum aType = GetType(theGroup);

  // Get IDs of sub-shapes to add
  Standard_Integer i, new_id;
  for (i = 1; i <= aLen; i++) {
    new_id = theSubShapes->Value(i);

    if (0 < new_id && new_id <= mapIndices.Extent()) {
      //if (mapIDs.Add(new_id)) { IPAL21297. Why we ignore invalid ids silently?
      if (mapIDs.Add(new_id) && mapIndices(new_id).ShapeType()==aType ) {
        aNewIDs.Append(new_id);
      }
    }
  }

  if (aNewIDs.Extent() > 0) {
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    Handle(TColStd_HArray1OfInteger) aNewSeq = new TColStd_HArray1OfInteger(1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewSeq->SetValue(k, aNewIDsIter.Value());
    }
    if ( aFunction->IsLastFuntion() ) {
      aSSI.SetIndices(aNewSeq);
    }
    else {
      aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
      GEOM_ISubShape aSSI2 (aFunction);
      aSSI2.SetIndices(aNewSeq);
      aSSI2.SetMainShape(aMainShapeFunc);
    }
    // As we do not recompute here our group, lets mark it as Modified
    Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
    theGroup->SetTic(aTic - 1);
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "geompy.UnionIDs(" << theGroup << ", [";
  for (i = 1; i < aLen; i++)
    pd << theSubShapes->Value(i) << ", ";
  pd << theSubShapes->Value(aLen) << "])";

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  DifferenceIDs
 */
//=============================================================================
void GEOMImpl_IGroupOperations::DifferenceIDs (Handle(GEOM_Object) theGroup,
                                               const Handle(TColStd_HSequenceOfInteger)& theSubShapes)
{
  SetErrorCode(KO);
  if (theGroup.IsNull()) return;

  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return;
  }

  Standard_Integer aLen = theSubShapes->Length();
  if (aLen < 1) {
    //SetErrorCode("The list is empty");
    SetErrorCode(OK);
    return;
  }

  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // Map of IDs to be removed
  TColStd_MapOfInteger mapIDsToRemove;

  // Map of current IDs
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer aLength = aSeq->Length();

  // VSR 28/04/2011 commented to allow operation even for empty group
  //   if (aLength == 1 && aSeq->Value(1) == -1) // empty group
  //     return;

  TColStd_MapOfInteger mapIDsCurrent;
  Standard_Integer j = 1;
  for (; j <= aLength; j++) {
    mapIDsCurrent.Add(aSeq->Value(j));
  }

  // Get Main Shape
  Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
  if (aMainShapeFunc.IsNull()) return;
  TDF_Label aLabel = aMainShapeFunc->GetOwnerEntry();
  if (aLabel.IsRoot()) return;
  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return;
  TopoDS_Shape aMainShape = aMainObj->GetValue();
  if (aMainShape.IsNull()) return;

  TopTools_IndexedMapOfShape mapIndices;
  TopExp::MapShapes(aMainShape, mapIndices);

  // Get IDs of sub-shapes to be removed
  Standard_Integer i, rem_id;
  for (i = 1; i <= aLen; i++) {
    rem_id = theSubShapes->Value(i);
    if (rem_id > 0 && mapIDsCurrent.Contains(rem_id)) {
      mapIDsToRemove.Add(rem_id);
    }
  }

  if (mapIDsToRemove.Extent() > 0) {
    Standard_Integer k = 1, aRemLength = mapIDsToRemove.Extent();
    Handle(TColStd_HArray1OfInteger) aNewSeq;
    if ( aLength - aRemLength > 0 ) {
      aNewSeq = new TColStd_HArray1OfInteger(1, aLength - aRemLength);
      for (j = 1; j <= aLength; j++) {
        if (!mapIDsToRemove.Contains(aSeq->Value(j))) {
          aNewSeq->SetValue(k, aSeq->Value(j));
          k++;
        }
      }
    }
    else {
      aNewSeq = new TColStd_HArray1OfInteger(1,1);
      aNewSeq->SetValue(1, -1);
    }
    if ( aFunction->IsLastFuntion() ) {
      aSSI.SetIndices(aNewSeq);
    }
    else {
      aFunction = theGroup->AddFunction( GEOM_Object::GetSubShapeID(), 0, true );
      GEOM_ISubShape aSSI2 (aFunction);
      aSSI2.SetIndices(aNewSeq);
      aSSI2.SetMainShape(aMainShapeFunc);
    }
    // As we do not recompute here our group, lets mark it as Modified
    Standard_Integer aTic = aMainObj->GetTic(); // tic of main shape
    theGroup->SetTic(aTic - 1);
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "geompy.DifferenceIDs(" << theGroup << ", [";
  for (i = 1; i < aLen; i++)
    pd << theSubShapes->Value(i) << ", ";
  pd << theSubShapes->Value(aLen) << "])";

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  UnionGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::UnionGroups (Handle(GEOM_Object) theGroup1,
                                                            Handle(GEOM_Object) theGroup2)
{
  SetErrorCode(KO);
  if (theGroup1.IsNull() || theGroup2.IsNull()) return NULL;

  if ( theGroup1->GetType() != GEOM_GROUP || theGroup2->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return NULL;
  }

  // Get group type
  TopAbs_ShapeEnum aType1 = GetType(theGroup1);
  TopAbs_ShapeEnum aType2 = GetType(theGroup2);
  TopAbs_ShapeEnum aType = aType1;
  if (aType1 != aType2) {
    if (aType1 != TopAbs_SHAPE && aType1 != TopAbs_COMPOUND) {
      if (aType2 == TopAbs_SHAPE || aType2 == TopAbs_COMPOUND)
        aType = aType2;
      else {
        SetErrorCode("Error: UnionGroups cannot be performed on groups of different type");
        return NULL;
      }
    }
  }

  // Get Main Shape
  Handle(GEOM_Function) aFunction1 = theGroup1->GetLastFunction();
  Handle(GEOM_Function) aFunction2 = theGroup2->GetLastFunction();
  if (aFunction1.IsNull() || aFunction2.IsNull()) return NULL;

  GEOM_ISubShape aSSI1 (aFunction1);
  GEOM_ISubShape aSSI2 (aFunction2);

  Handle(GEOM_Function) aMainShapeFunc1 = aSSI1.GetMainShape();
  Handle(GEOM_Function) aMainShapeFunc2 = aSSI2.GetMainShape();
  if (aMainShapeFunc1.IsNull() || aMainShapeFunc2.IsNull()) return NULL;

  TDF_Label aLabel1 = aMainShapeFunc1->GetOwnerEntry();
  TDF_Label aLabel2 = aMainShapeFunc2->GetOwnerEntry();
  if (aLabel1.IsRoot() || aLabel2.IsRoot()) {
    SetErrorCode("Error: UnionGroups can be performed only on groups");
    return NULL;
  }

  if (aLabel1 != aLabel2) {
    SetErrorCode("Error: UnionGroups cannot be performed on groups, built on different main shapes");
    return NULL;
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel1);
  if (aMainObj.IsNull()) return NULL;

  // New contents of the group
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  Handle(TColStd_HArray1OfInteger) aSeq1 = aSSI1.GetIndices();
  Handle(TColStd_HArray1OfInteger) aSeq2 = aSSI2.GetIndices();
  if (aSeq1.IsNull() || aSeq2.IsNull()) return NULL;

  Standard_Integer j, val_j;
  Standard_Integer aLength1 = aSeq1->Length();
  Standard_Integer aLength2 = aSeq2->Length();

  for (j = 1; j <= aLength1; j++) {
    val_j = aSeq1->Value(j);
    if (val_j > 0 && mapIDs.Add(val_j)) {
      aNewIDs.Append(val_j);
    }
  }
  for (j = 1; j <= aLength2; j++) {
    val_j = aSeq2->Value(j);
    if (val_j > 0 && mapIDs.Add(val_j)) {
      aNewIDs.Append(val_j);
    }
  }

  if (aNewIDs.Extent() < 1) {
    SetErrorCode("Error: UnionGroups cannot be performed on two empty groups");
    return NULL;
  }

  // Put new indices from the list into an array
  Standard_Integer k = 1;
  TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
  Handle(TColStd_HArray1OfInteger) aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
  for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
    aNewArr->SetValue(k, aNewIDsIter.Value());
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  // Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump(aFunction) << aGroup << " = geompy.UnionGroups("
                               << theGroup1 << ", " << theGroup2 << ")";

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  IntersectGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::IntersectGroups (Handle(GEOM_Object) theGroup1,
                                                                Handle(GEOM_Object) theGroup2)
{
  SetErrorCode(KO);
  if (theGroup1.IsNull() || theGroup2.IsNull()) return NULL;

  if ( theGroup1->GetType() != GEOM_GROUP || theGroup2->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return NULL;
  }

  // Get group type
  TopAbs_ShapeEnum aType1 = GetType(theGroup1);
  TopAbs_ShapeEnum aType2 = GetType(theGroup2);
  TopAbs_ShapeEnum aType = aType1;
  if (aType1 != aType2) {
    if (aType1 != TopAbs_SHAPE && aType1 != TopAbs_COMPOUND) {
      if (aType2 == TopAbs_SHAPE || aType2 == TopAbs_COMPOUND)
        aType = aType2;
      else {
        SetErrorCode("Error: IntersectGroups cannot be performed on groups of different type");
        return NULL;
      }
    }
  }

  // Get Main Shape
  Handle(GEOM_Function) aFunction1 = theGroup1->GetLastFunction();
  Handle(GEOM_Function) aFunction2 = theGroup2->GetLastFunction();
  if (aFunction1.IsNull() || aFunction2.IsNull()) return NULL;

  GEOM_ISubShape aSSI1 (aFunction1);
  GEOM_ISubShape aSSI2 (aFunction2);

  Handle(GEOM_Function) aMainShapeFunc1 = aSSI1.GetMainShape();
  Handle(GEOM_Function) aMainShapeFunc2 = aSSI2.GetMainShape();
  if (aMainShapeFunc1.IsNull() || aMainShapeFunc2.IsNull()) return NULL;

  TDF_Label aLabel1 = aMainShapeFunc1->GetOwnerEntry();
  TDF_Label aLabel2 = aMainShapeFunc2->GetOwnerEntry();
  if (aLabel1.IsRoot() || aLabel2.IsRoot()) {
    SetErrorCode("Error: UnionGroups can be performed only on groups");
    return NULL;
  }

  if (aLabel1 != aLabel2) {
    SetErrorCode("Error: IntersectGroups cannot be performed on groups, built on different main shapes");
    return NULL;
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel1);
  if (aMainObj.IsNull()) return NULL;

  // New contents of the group
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  Handle(TColStd_HArray1OfInteger) aSeq1 = aSSI1.GetIndices();
  Handle(TColStd_HArray1OfInteger) aSeq2 = aSSI2.GetIndices();
  if (aSeq1.IsNull() || aSeq2.IsNull()) return NULL;

  Standard_Integer j, val_j;
  Standard_Integer aLength1 = aSeq1->Length();
  Standard_Integer aLength2 = aSeq2->Length();

  for (j = 1; j <= aLength1; j++) {
    mapIDs.Add(aSeq1->Value(j));
  }
  for (j = 1; j <= aLength2; j++) {
    val_j = aSeq2->Value(j);
    if (val_j > 0 && !mapIDs.Add(val_j)) {
      // add index, if it is in mapIDs (filled from Group_1)
      aNewIDs.Append(val_j);
    }
  }

  Handle(TColStd_HArray1OfInteger) aNewArr;
  if (aNewIDs.Extent() < 1) {
    aNewArr = new TColStd_HArray1OfInteger (1, 1);
    aNewArr->SetValue(1, -1);
  }
  else {
    // Put new indices from the list into an array
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewArr->SetValue(k, aNewIDsIter.Value());
    }
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  // Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump(aFunction) << aGroup << " = geompy.IntersectGroups("
                               << theGroup1 << ", " << theGroup2 << ")";

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  CutGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::CutGroups (Handle(GEOM_Object) theGroup1,
                                                          Handle(GEOM_Object) theGroup2)
{
  SetErrorCode(KO);
  if (theGroup1.IsNull() || theGroup2.IsNull()) return NULL;

  if ( theGroup1->GetType() != GEOM_GROUP || theGroup2->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return NULL;
  }

  // Get group type
  TopAbs_ShapeEnum aType1 = GetType(theGroup1);
  TopAbs_ShapeEnum aType2 = GetType(theGroup2);
  TopAbs_ShapeEnum aType = aType1;
  if (aType1 != aType2) {
    if (aType1 != TopAbs_SHAPE && aType1 != TopAbs_COMPOUND) {
      if (aType2 == TopAbs_SHAPE || aType2 == TopAbs_COMPOUND)
        aType = aType2;
      else {
        SetErrorCode("Error: CutGroups cannot be performed on groups of different type");
        return NULL;
      }
    }
  }

  // Get Main Shape
  Handle(GEOM_Function) aFunction1 = theGroup1->GetLastFunction();
  Handle(GEOM_Function) aFunction2 = theGroup2->GetLastFunction();
  if (aFunction1.IsNull() || aFunction2.IsNull()) return NULL;

  GEOM_ISubShape aSSI1 (aFunction1);
  GEOM_ISubShape aSSI2 (aFunction2);

  Handle(GEOM_Function) aMainShapeFunc1 = aSSI1.GetMainShape();
  Handle(GEOM_Function) aMainShapeFunc2 = aSSI2.GetMainShape();
  if (aMainShapeFunc1.IsNull() || aMainShapeFunc2.IsNull()) return NULL;

  TDF_Label aLabel1 = aMainShapeFunc1->GetOwnerEntry();
  TDF_Label aLabel2 = aMainShapeFunc2->GetOwnerEntry();
  if (aLabel1.IsRoot() || aLabel2.IsRoot()) {
    SetErrorCode("Error: UnionGroups can be performed only on groups");
    return NULL;
  }

  if (aLabel1 != aLabel2) {
    SetErrorCode("Error: CutGroups cannot be performed on groups, built on different main shapes");
    return NULL;
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel1);
  if (aMainObj.IsNull()) return NULL;

  // New contents of the group
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  Handle(TColStd_HArray1OfInteger) aSeq1 = aSSI1.GetIndices();
  Handle(TColStd_HArray1OfInteger) aSeq2 = aSSI2.GetIndices();
  if (aSeq1.IsNull() || aSeq2.IsNull()) return NULL;

  Standard_Integer j, val_j;
  Standard_Integer aLength1 = aSeq1->Length();
  Standard_Integer aLength2 = aSeq2->Length();

  for (j = 1; j <= aLength2; j++) {
    mapIDs.Add(aSeq2->Value(j));
  }
  for (j = 1; j <= aLength1; j++) {
    val_j = aSeq1->Value(j);
    if (val_j > 0 && mapIDs.Add(val_j)) {
      // add index, if it is not in mapIDs (filled from Group_2)
      aNewIDs.Append(val_j);
    }
  }

  Handle(TColStd_HArray1OfInteger) aNewArr;
  if (aNewIDs.Extent() < 1) {
    aNewArr = new TColStd_HArray1OfInteger (1, 1);
    aNewArr->SetValue(1, -1);
  }
  else {
    // Put new indices from the list into an array
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewArr->SetValue(k, aNewIDsIter.Value());
    }
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  // Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump(aFunction) << aGroup << " = geompy.CutGroups("
                               << theGroup1 << ", " << theGroup2 << ")";

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  UnionListOfGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::UnionListOfGroups
                         (const Handle(TColStd_HSequenceOfTransient)& theGList)
{
  SetErrorCode(KO);
  if (theGList.IsNull()) return NULL;

  Standard_Integer i, aLen = theGList->Length();
  if (aLen < 1) {
    SetErrorCode("UnionListOfGroups error: the list of groups is empty");
    return NULL;
  }

  TopAbs_ShapeEnum aType, aType_i;
  TDF_Label aLabel, aLabel_i;
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  // Iterate on the initial groups
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList->Value(i));
    if ( aGr_i->GetType() != GEOM_GROUP ) {
      SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
      return NULL;
    }
    // Get group type
    aType_i = GetType(aGr_i);
    if (i == 1)
      aType = aType_i;
    else {
      if (aType_i != aType) {
        if (aType != TopAbs_SHAPE && aType != TopAbs_COMPOUND) {
          if (aType_i == TopAbs_SHAPE || aType_i == TopAbs_COMPOUND)
            aType = aType_i;
          else {
            SetErrorCode("Error: UnionListOfGroups cannot be performed on groups of different type");
            return NULL;
          }
        }
      }
    }

    // Get Main Shape
    Handle(GEOM_Function) aFunction_i = aGr_i->GetLastFunction();
    if (aFunction_i.IsNull()) return NULL;
    GEOM_ISubShape aSSI (aFunction_i);
    Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
    if (aMainShapeFunc.IsNull()) return NULL;
    aLabel_i = aMainShapeFunc->GetOwnerEntry();
    if (aLabel_i.IsRoot()) {
      SetErrorCode("Error: UnionGroups can be performed only on groups");
      return NULL;
    }
    if (i == 1)
      aLabel = aLabel_i;
    else {
      if (aLabel_i != aLabel) {
        SetErrorCode("Error: UnionListOfGroups cannot be performed on groups, built on different main shapes");
        return NULL;
      }
    }

    // New contents of the group
    Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
    if (aSeq.IsNull()) return NULL;

    Standard_Integer j, val_j, aLength = aSeq->Length();
    for (j = 1; j <= aLength; j++) {
      val_j = aSeq->Value(j);
      if (val_j > 0 && mapIDs.Add(val_j)) {
        aNewIDs.Append(val_j);
      }
    }
  }

  // Check the resulting list of indices
  if (aNewIDs.Extent() < 1) {
    SetErrorCode("Error: UnionListOfGroups cannot be performed on all empty groups");
    return NULL;
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return NULL;

  // Put new indices from the list into an array
  Standard_Integer k = 1;
  TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
  Handle(TColStd_HArray1OfInteger) aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
  for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
    aNewArr->SetValue(k, aNewIDsIter.Value());
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  //Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump pd (aFunction);
  pd << aGroup << " = geompy.UnionListOfGroups([";
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList->Value(i));
    pd << aGr_i << ((i < aLen) ? ", " : "])");
  }

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  IntersectListOfGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::IntersectListOfGroups
                         (const Handle(TColStd_HSequenceOfTransient)& theGList)
{
  SetErrorCode(KO);
  if (theGList.IsNull()) return NULL;

  Standard_Integer i, aLen = theGList->Length();
  if (aLen < 1) {
    SetErrorCode("IntersectListOfGroups error: the list of groups is empty");
    return NULL;
  }

  TopAbs_ShapeEnum aType, aType_i;
  TDF_Label aLabel, aLabel_i;
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  // Iterate on the initial groups
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList->Value(i));
    if ( aGr_i->GetType() != GEOM_GROUP ) {
      SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
      return NULL;
    }
    // Get group type
    aType_i = GetType(aGr_i);
    if (i == 1)
      aType = aType_i;
    else {
      if (aType_i != aType) {
        if (aType != TopAbs_SHAPE && aType != TopAbs_COMPOUND) {
          if (aType_i == TopAbs_SHAPE || aType_i == TopAbs_COMPOUND)
            aType = aType_i;
          else {
            SetErrorCode("Error: IntersectListOfGroups cannot be performed on groups of different type");
            return NULL;
          }
        }
      }
    }

    // Get Main Shape
    Handle(GEOM_Function) aFunction_i = aGr_i->GetLastFunction();
    if (aFunction_i.IsNull()) return NULL;
    GEOM_ISubShape aSSI (aFunction_i);
    Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
    if (aMainShapeFunc.IsNull()) return NULL;
    aLabel_i = aMainShapeFunc->GetOwnerEntry();
    if (aLabel_i.IsRoot()) {
     SetErrorCode("Error: UnionGroups can be performed only on groups");
     return NULL;
    }
    if (i == 1)
      aLabel = aLabel_i;
    else {
      if (aLabel_i != aLabel) {
        SetErrorCode("Error: IntersectListOfGroups cannot be performed on groups, built on different main shapes");
        return NULL;
      }
    }

    // New contents of the group
    Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
    if (aSeq.IsNull()) return NULL;

    Standard_Integer j, val_j, aLength = aSeq->Length();
    for (j = 1; j <= aLength; j++) {
      val_j = aSeq->Value(j);
      if (val_j > 0) {
        if (i == 1) {
          // get all elements of the first group
          if (mapIDs.Add(val_j))
            aNewIDs.Append(val_j);
        }
        else {
          // get only elements, present in all previously processed groups
          if (!mapIDs.Add(val_j))
            aNewIDs.Append(val_j);
        }
      }
    }

    // refill the map with only validated elements
    if (i > 1) {
      mapIDs.Clear();
      TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
      for (; aNewIDsIter.More(); aNewIDsIter.Next()) {
        mapIDs.Add(aNewIDsIter.Value());
      }
    }
    // clear the resulting list before starting the next sycle
    if (i < aLen) {
      aNewIDs.Clear();
    }
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return NULL;

  Handle(TColStd_HArray1OfInteger) aNewArr;
  if (aNewIDs.Extent() < 1) {
    aNewArr = new TColStd_HArray1OfInteger (1, 1);
    aNewArr->SetValue(1, -1);
  }
  else {
    // Put new indices from the list into an array
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewArr->SetValue(k, aNewIDsIter.Value());
    }
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  //Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump pd (aFunction);
  pd << aGroup << " = geompy.IntersectListOfGroups([";
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList->Value(i));
    pd << aGr_i << ((i < aLen) ? ", " : "])");
  }

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  CutListOfGroups
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IGroupOperations::CutListOfGroups
                        (const Handle(TColStd_HSequenceOfTransient)& theGList1,
                         const Handle(TColStd_HSequenceOfTransient)& theGList2)
{
  SetErrorCode(KO);
  if (theGList1.IsNull() || theGList2.IsNull()) return NULL;

  Standard_Integer i;
  Standard_Integer aLen1 = theGList1->Length();
  Standard_Integer aLen2 = theGList2->Length();
  if (aLen1 < 1) {
    SetErrorCode("CutListOfGroups error: the first list of groups is empty");
    return NULL;
  }

  TopAbs_ShapeEnum aType, aType_i;
  TDF_Label aLabel, aLabel_i;
  TColStd_ListOfInteger aNewIDs;
  TColStd_MapOfInteger mapIDs;

  // 1. Collect indices to be excluded (from theGList2) into the mapIDs
  for (i = 1; i <= aLen2; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList2->Value(i));
    if ( aGr_i->GetType() != GEOM_GROUP ) {
      SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
      return NULL;
    }
    // Get group type
    aType_i = GetType(aGr_i);
    if (i == 1)
      aType = aType_i;
    else {
      if (aType_i != aType) {
        if (aType != TopAbs_SHAPE && aType != TopAbs_COMPOUND) {
          if (aType_i == TopAbs_SHAPE || aType_i == TopAbs_COMPOUND)
            aType = aType_i;
          else {
            SetErrorCode("Error: CutListOfGroups cannot be performed on groups of different type");
            return NULL;
          }
        }
      }
    }

    // Get Main Shape
    Handle(GEOM_Function) aFunction_i = aGr_i->GetLastFunction();
    if (aFunction_i.IsNull()) return NULL;
    GEOM_ISubShape aSSI (aFunction_i);
    Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
    if (aMainShapeFunc.IsNull()) return NULL;
    aLabel_i = aMainShapeFunc->GetOwnerEntry();
    if (aLabel_i.IsRoot()) {
      SetErrorCode("Error: UnionGroups can be performed only on groups");
      return NULL;
    }
    if (i == 1)
      aLabel = aLabel_i;
    else {
      if (aLabel_i != aLabel) {
        SetErrorCode("Error: CutListOfGroups cannot be performed on groups, built on different main shapes");
        return NULL;
      }
    }

    // Indiced to exclude
    Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
    if (aSeq.IsNull()) return NULL;

    Standard_Integer j, aLength = aSeq->Length();
    for (j = 1; j <= aLength; j++) {
      mapIDs.Add(aSeq->Value(j));
    }
  }

  // 2. Collect indices from theGList1, avoiding indices from theGList2 (mapIDs)
  for (i = 1; i <= aLen1; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList1->Value(i));
    if ( aGr_i->GetType() != GEOM_GROUP ) {
      SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
      return NULL;
    }
    // Get group type
    aType_i = GetType(aGr_i);
    if (i == 1 && aLen2 < 1)
      aType = aType_i;
    else {
      if (aType_i != aType) {
        if (aType != TopAbs_SHAPE && aType != TopAbs_COMPOUND) {
          if (aType_i == TopAbs_SHAPE || aType_i == TopAbs_COMPOUND)
            aType = aType_i;
          else {
            SetErrorCode("Error: CutListOfGroups cannot be performed on groups of different type");
            return NULL;
          }
        }
      }
    }

    // Get Main Shape
    Handle(GEOM_Function) aFunction_i = aGr_i->GetLastFunction();
    if (aFunction_i.IsNull()) return NULL;
    GEOM_ISubShape aSSI (aFunction_i);
    Handle(GEOM_Function) aMainShapeFunc = aSSI.GetMainShape();
    if (aMainShapeFunc.IsNull()) return NULL;
    aLabel_i = aMainShapeFunc->GetOwnerEntry();
    if (aLabel_i.IsRoot()) {
      SetErrorCode("Error: UnionGroups can be performed only on groups");
      return NULL;
    }
    if (i == 1 && aLen2 < 1)
      aLabel = aLabel_i;
    else {
      if (aLabel_i != aLabel) {
        SetErrorCode("Error: CutListOfGroups cannot be performed on groups, built on different main shapes");
        return NULL;
      }
    }

    // New contents of the group
    Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
    if (aSeq.IsNull()) return NULL;

    Standard_Integer j, val_j, aLength = aSeq->Length();
    for (j = 1; j <= aLength; j++) {
      val_j = aSeq->Value(j);
      if (val_j > 0 && mapIDs.Add(val_j)) {
        // get only elements, not present in mapIDs (theGList2)
        aNewIDs.Append(val_j);
      }
    }
  }

  Handle(GEOM_Object) aMainObj = GEOM_Object::GetObject(aLabel);
  if (aMainObj.IsNull()) return NULL;

  Handle(TColStd_HArray1OfInteger) aNewArr;
  if (aNewIDs.Extent() < 1) {
    aNewArr = new TColStd_HArray1OfInteger (1, 1);
    aNewArr->SetValue(1, -1);
  }
  else {
    // Put new indices from the list into an array
    Standard_Integer k = 1;
    TColStd_ListIteratorOfListOfInteger aNewIDsIter (aNewIDs);
    aNewArr = new TColStd_HArray1OfInteger (1, aNewIDs.Extent());
    for (; aNewIDsIter.More(); aNewIDsIter.Next(), k++) {
      aNewArr->SetValue(k, aNewIDsIter.Value());
    }
  }

  // Create the Group
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(aMainObj, aNewArr);
  aGroup->SetType(GEOM_GROUP);
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aType);

  //Make a Python command
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  GEOM::TPythonDump pd (aFunction);
  pd << aGroup << " = geompy.CutListOfGroups([";
  for (i = 1; i <= aLen1; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList1->Value(i));
    pd << aGr_i << ((i < aLen1) ? ", " : "], [");
  }
  for (i = 1; i <= aLen2; i++) {
    Handle(GEOM_Object) aGr_i = Handle(GEOM_Object)::DownCast(theGList2->Value(i));
    pd << aGr_i << ((i < aLen2) ? ", " : "])");
  }

  SetErrorCode(OK);
  return aGroup;
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
Handle(GEOM_Object) GEOMImpl_IGroupOperations::GetMainShape (Handle(GEOM_Object) theGroup)
{
  SetErrorCode(KO);

  if (theGroup.IsNull()) return NULL;
  if (theGroup->GetType() != GEOM_GROUP &&
      theGroup->GetType() != GEOM_SUBSHAPE) {
    SetErrorCode("Error: You could perform this operation only with a group or a sub-shape.");
    return NULL;
  }

  Handle(GEOM_Function) aGroupFunction = theGroup->GetFunction(1);
  if (aGroupFunction.IsNull()) return NULL;

  GEOM_ISubShape aSSI (aGroupFunction);
  Handle(GEOM_Function) aMainShapeFunction = aSSI.GetMainShape();
  if (aMainShapeFunction.IsNull()) return NULL;

  TDF_Label aLabel = aMainShapeFunction->GetOwnerEntry();
  Handle(GEOM_Object) aMainShape = GEOM_Object::GetObject(aLabel);
  if (aMainShape.IsNull()) return NULL;

  //Make a Python command
  //GEOM::TPythonDump(aGroupFunction, /*append=*/true)
  //  << aMainShape << " = geompy.GetMainShape(" << theGroup << ")";

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
  if ( theGroup->GetType() != GEOM_GROUP ) {
    SetErrorCode( "Error: You could perform this operation only with group. Please select a group." );
    return NULL;
  }
  Handle(GEOM_Function) aFunction = theGroup->GetLastFunction();
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
