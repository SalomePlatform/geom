// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
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
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Integer.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

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
Handle(GEOM_Object) GEOMImpl_IGroupOperations::CreateGroup
       (Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum  theShapeType)
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
  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  //TCollection_AsciiString anOldDescr = aFunction->GetDescription();

  //GEOM::TPythonDump(aFunction) << anOldDescr.ToCString() << "\n\t" << aGroup
  GEOM::TPythonDump(aFunction) << aGroup
    << " = geompy.CreateGroup(" << theMainShape << ", " << (int)theShapeType << ")";

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

  //Make a Python command 
  TCollection_AsciiString anOldDescr = aFunction->GetDescription();

  GEOM::TPythonDump(aFunction) << anOldDescr.ToCString() << "\n\t"
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

  //Make a Python command 
  TCollection_AsciiString anOldDescr = aFunction->GetDescription();

  GEOM::TPythonDump(aFunction) << anOldDescr.ToCString() << "\n\t"
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

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
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

  // Get IDs of sub-shapes to add
  Standard_Integer i, new_id, aLen = theSubShapes->Length();
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));

    if (anObj_i->IsMainShape()) {
      TopoDS_Shape aShape_i = anObj_i->GetValue();
      if (mapIndices.Contains(aShape_i)) {
        new_id = mapIndices.FindIndex(aShape_i);
        if (mapIDs.Add(new_id)) {
          aNewIDs.Append(new_id);
        }
      } else {
        SetErrorCode("One of given objects can not be added to the group, because it is not a sub-shape of the group's main shape");
        return;
      }
    } else {
      // Check main shape of sub-shape to add
      Handle(GEOM_Function) aFunc_i = anObj_i->GetFunction(1);
      if (aFunc_i.IsNull()) return;

      GEOM_ISubShape aSSI_i (aFunc_i);

      Handle(GEOM_Function) aMainShapeFunc_i = aSSI_i.GetMainShape();
      if (aMainShapeFunc_i.IsNull()) return;
      TDF_Label aLabel_i = aMainShapeFunc_i->GetOwnerEntry();
      if (aLabel_i.IsRoot()) return;
      Handle(GEOM_Object) aMainObj_i = GEOM_Object::GetObject(aLabel);
      if (aMainObj_i.IsNull()) return;
      TopoDS_Shape aMainShape_i = aMainObj_i->GetValue();
      if (aMainShape_i.IsNull()) return;

      if (aMainShape_i.IsSame(aMainShape)) {
        // add all sub-shape IDs to the list

        // Get IDs
        Handle(TColStd_HArray1OfInteger) aSeq_i = aSSI_i.GetIndices();
        if (aSeq_i.IsNull()) return;
        Standard_Integer aLength_i = aSeq_i->Length();

        for (Standard_Integer i_j = 1; i_j <= aLength_i; i_j++) {
          new_id = aSeq_i->Value(i_j);
          if (new_id > 0) {
            if (mapIDs.Add(new_id)) {
              aNewIDs.Append(new_id);
            }
          }
        }

      } else if (mapIndices.Contains(aMainShape_i)) {
        // compute new IDs and add them to the list
        TopTools_IndexedMapOfShape mapIndices_i;
        TopExp::MapShapes(aMainShape_i, mapIndices_i);

        // Get IDs
        Handle(TColStd_HArray1OfInteger) aSeq_i = aSSI_i.GetIndices();
        if (aSeq_i.IsNull()) return;
        Standard_Integer aLength_i = aSeq_i->Length();

        for (Standard_Integer i_j = 1; i_j <= aLength_i; i_j++) {
          if (aSeq_i->Value(i_j) > 0) {
            TopoDS_Shape aShape_i_j = mapIndices_i.FindKey(i_j);
            new_id = mapIndices.FindIndex(aShape_i_j);
            if (mapIDs.Add(new_id)) {
              aNewIDs.Append(new_id);
            }
          }
        }

      } else {
        SetErrorCode("One of given objects can not be added to the group, because it is not a sub-shape of the group's main shape");
        return;
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

    aSSI.SetIndices(aNewSeq);
  }

  //Make a Python command 
  TCollection_AsciiString anOldDescr = aFunction->GetDescription();

  GEOM::TPythonDump pd (aFunction);
  pd << anOldDescr.ToCString() << "\n\t" << "geompy.UnionList(" << theGroup << ", [";

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

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // Map of IDs to be removed
  TColStd_MapOfInteger mapIDsToRemove;

  // Map of current IDs
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer aLength = aSeq->Length();

  if (aLength == 1 && aSeq->Value(1) == -1) // empty group
    return;

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
  Standard_Integer i, rem_id, aLen = theSubShapes->Length();
  for (i = 1; i <= aLen; i++) {
    Handle(GEOM_Object) anObj_i = Handle(GEOM_Object)::DownCast(theSubShapes->Value(i));

    if (anObj_i->IsMainShape()) {
      TopoDS_Shape aShape_i = anObj_i->GetValue();
      if (mapIndices.Contains(aShape_i)) {
        rem_id = mapIndices.FindIndex(aShape_i);
        if (mapIDsCurrent.Contains(rem_id)) {
          mapIDsToRemove.Add(rem_id);
        }
      } else {
        SetErrorCode("One of given objects can not be removed from the group, because it is not a sub-shape of the group's main shape");
        return;
      }
    } else {
      // Check main shape of sub-shape to be removed
      Handle(GEOM_Function) aFunc_i = anObj_i->GetFunction(1);
      if (aFunc_i.IsNull()) return;

      GEOM_ISubShape aSSI_i (aFunc_i);

      Handle(GEOM_Function) aMainShapeFunc_i = aSSI_i.GetMainShape();
      if (aMainShapeFunc_i.IsNull()) return;
      TDF_Label aLabel_i = aMainShapeFunc_i->GetOwnerEntry();
      if (aLabel_i.IsRoot()) return;
      Handle(GEOM_Object) aMainObj_i = GEOM_Object::GetObject(aLabel);
      if (aMainObj_i.IsNull()) return;
      TopoDS_Shape aMainShape_i = aMainObj_i->GetValue();
      if (aMainShape_i.IsNull()) return;

      if (aMainShape_i.IsSame(aMainShape)) {
        // remove all sub-shapes

        // Get IDs
        Handle(TColStd_HArray1OfInteger) aSeq_i = aSSI_i.GetIndices();
        if (aSeq_i.IsNull()) return;
        Standard_Integer aLength_i = aSeq_i->Length();

        for (Standard_Integer i_j = 1; i_j <= aLength_i; i_j++) {
          rem_id = aSeq_i->Value(i_j);
          if (rem_id > 0) {
            if (mapIDsCurrent.Contains(rem_id)) {
              mapIDsToRemove.Add(rem_id);
            }
          }
        }

      } else if (mapIndices.Contains(aMainShape_i)) {
        // compute new IDs and add them to the map of ids to be removed
        TopTools_IndexedMapOfShape mapIndices_i;
        TopExp::MapShapes(aMainShape_i, mapIndices_i);

        // Get IDs
        Handle(TColStd_HArray1OfInteger) aSeq_i = aSSI_i.GetIndices();
        if (aSeq_i.IsNull()) return;
        Standard_Integer aLength_i = aSeq_i->Length();

        for (Standard_Integer i_j = 1; i_j <= aLength_i; i_j++) {
          if (aSeq_i->Value(i_j) > 0) {
            TopoDS_Shape aShape_i_j = mapIndices_i.FindKey(i_j);
            rem_id = mapIndices.FindIndex(aShape_i_j);
            if (mapIDsCurrent.Contains(rem_id)) {
              mapIDsToRemove.Add(rem_id);
            }
          }
        }

      } else {
        SetErrorCode("One of given objects can not be removed from the group, because it is not a sub-shape of the group's main shape");
        return;
      }
    }
  }

  if (mapIDsToRemove.Extent() > 0) {
    Standard_Integer k = 1, aRemLength = mapIDsToRemove.Extent();
    Handle(TColStd_HArray1OfInteger) aNewSeq = new TColStd_HArray1OfInteger(1, aLength - aRemLength);

    for (j = 1; j <= aLength; j++) {
      if (!mapIDsToRemove.Contains(aSeq->Value(j))) {
        aNewSeq->SetValue(k, aSeq->Value(j));
        k++;
      }
    }

    aSSI.SetIndices(aNewSeq);
  }

  //Make a Python command
  TCollection_AsciiString anOldDescr = aFunction->GetDescription();

  GEOM::TPythonDump pd (aFunction);
  pd << anOldDescr.ToCString() << "\n\t" << "geompy.DifferenceList(" << theGroup << ", [";

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

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
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

  // Get IDs of sub-shapes to add
  Standard_Integer i, new_id, aLen = theSubShapes->Length();
  for (i = 1; i <= aLen; i++) {
    new_id = theSubShapes->Value(i);

    if (0 < new_id && new_id <= mapIndices.Extent()) {
      if (mapIDs.Add(new_id)) {
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

    aSSI.SetIndices(aNewSeq);
  }

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

  Handle(GEOM_Function) aFunction = theGroup->GetFunction(1);
  if (aFunction.IsNull()) return;

  GEOM_ISubShape aSSI (aFunction);

  // Map of IDs to be removed
  TColStd_MapOfInteger mapIDsToRemove;

  // Map of current IDs
  Handle(TColStd_HArray1OfInteger) aSeq = aSSI.GetIndices();
  if (aSeq.IsNull()) return;
  Standard_Integer aLength = aSeq->Length();

  if (aLength == 1 && aSeq->Value(1) == -1) // empty group
    return;

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
  Standard_Integer i, rem_id, aLen = theSubShapes->Length();
  for (i = 1; i <= aLen; i++) {
    rem_id = theSubShapes->Value(i);
    if (mapIDsCurrent.Contains(rem_id)) {
      mapIDsToRemove.Add(rem_id);
    }
  }

  if (mapIDsToRemove.Extent() > 0) {
    Standard_Integer k = 1, aRemLength = mapIDsToRemove.Extent();
    Handle(TColStd_HArray1OfInteger) aNewSeq = new TColStd_HArray1OfInteger(1, aLength - aRemLength);

    for (j = 1; j <= aLength; j++) {
      if (!mapIDsToRemove.Contains(aSeq->Value(j))) {
        aNewSeq->SetValue(k, aSeq->Value(j));
        k++;
      }
    }

    aSSI.SetIndices(aNewSeq);
  }

  SetErrorCode(OK);
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

  if(theGroup.IsNull()) return NULL;

  Handle(GEOM_Function) aGroupFunction = theGroup->GetFunction(1);
  if (aGroupFunction.IsNull()) return NULL;

  GEOM_ISubShape aSSI (aGroupFunction);
  Handle(GEOM_Function) aMainShapeFunction = aSSI.GetMainShape();
  if (aMainShapeFunction.IsNull()) return NULL;

  TDF_Label aLabel = aMainShapeFunction->GetOwnerEntry();
  Handle(GEOM_Object) aMainShape = GEOM_Object::GetObject(aLabel);
  if (aMainShape.IsNull()) return NULL;

  //Make a Python command
  TCollection_AsciiString anOldDescr = aGroupFunction->GetDescription();

  GEOM::TPythonDump(aGroupFunction) << anOldDescr.ToCString() << "\n\t"
    << aMainShape << " = geompy.GetMainShape(" << theGroup << ")";

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
