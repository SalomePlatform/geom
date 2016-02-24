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

#include <GEOMImpl_PartitionDriver.hxx>
#include <GEOMImpl_IPartition.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMUtils.hxx>
#include <GEOM_Object.hxx>
#include <GEOM_Function.hxx>
#include <GEOMAlgo_Splitter.hxx>

#include <TDataStd_IntegerArray.hxx>
#include <TNaming_CopyShape.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPDS_DS.hxx>

// Depth of self-intersection check (see BOPAlgo_CheckerSI::SetLevelOfCheck() for more details)
// Default value for BOPAlgo_CheckerSI gives very long computation when checking face-to-face intersections;
// here check level is decreased to more appropriate value to avoid problems with performance).
#define BOP_SELF_INTERSECTIONS_LEVEL 4

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PartitionDriver::GetID()
{
  static Standard_GUID aPartitionDriver("FF1BBB22-5D14-4df2-980B-3A668264EA16");
  return aPartitionDriver;
}


//=======================================================================
//function : GEOMImpl_PartitionDriver
//purpose  :
//=======================================================================
GEOMImpl_PartitionDriver::GEOMImpl_PartitionDriver()
{
}

//=======================================================================
//function : SimplifyCompound
//purpose  :
//=======================================================================
static void PrepareShapes (const TopoDS_Shape&   theShape,
                           Standard_Integer      theType,
                           TopTools_ListOfShape& theSimpleList)
{
  if (theType == PARTITION_NO_SELF_INTERSECTIONS ||
      theShape.ShapeType() != TopAbs_COMPOUND) {
    theSimpleList.Append(theShape);
    return;
  }

  // explode compound on simple shapes to allow their intersections
  TopoDS_Iterator It (theShape, Standard_True, Standard_True);
  TopTools_MapOfShape mapShape;
  for (; It.More(); It.Next()) {
    if (mapShape.Add(It.Value())) {
      TopoDS_Shape curSh = It.Value();
      PrepareShapes(curSh, theType, theSimpleList);
    }
  }
}

static void CheckSelfIntersection(const TopoDS_Shape &theShape)
{
  BOPAlgo_CheckerSI aCSI;  // checker of self-interferences
  BOPCol_ListOfShape aList;

  aList.Append(theShape);
  aCSI.SetLevelOfCheck(BOP_SELF_INTERSECTIONS_LEVEL);
  aCSI.SetArguments(aList);
  aCSI.Perform();

  if (aCSI.ErrorStatus() || aCSI.DS().Interferences().Extent() > 0) {
    StdFail_NotDone::Raise("Partition operation will not be performed, because argument shape is self-intersected");
  }
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PartitionDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPartition aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();
  const Standard_Boolean isCheckSelfInte = aCI.GetCheckSelfIntersection();

  TopoDS_Shape aShape;
  GEOMAlgo_Splitter PS;

  TopTools_DataMapOfShapeShape aCopyMap;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;

  if (aType == PARTITION_PARTITION || aType == PARTITION_NO_SELF_INTERSECTIONS)
  {
    Handle(TColStd_HSequenceOfTransient) aShapes  = aCI.GetShapes();
    Handle(TColStd_HSequenceOfTransient) aTools   = aCI.GetTools();
    Handle(TColStd_HSequenceOfTransient) aKeepIns = aCI.GetKeepIns();
    Handle(TColStd_HSequenceOfTransient) aRemIns  = aCI.GetRemoveIns();
    Handle(TColStd_HArray1OfInteger) aMaterials   = aCI.GetMaterials();
    //skl Standard_Boolean DoRemoveWebs = !aMaterials.IsNull();

    unsigned int ind;
    //unsigned int ind, nbshapes = 0;
    //nbshapes += aShapes->Length() + aTools->Length();
    //nbshapes += aKeepIns->Length() + aRemIns->Length();
    //TopTools_MapOfShape ShapesMap(nbshapes), ToolsMap(nbshapes);
    TopTools_MapOfShape ShapesMap, ToolsMap;

    // add object shapes that are in ListShapes;
    for (ind = 1; ind <= aShapes->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a shape is null");
      }

      // Check self-intersection.
      if (isCheckSelfInte && aType == PARTITION_NO_SELF_INTERSECTIONS) {
        CheckSelfIntersection(aShape_i);
      }

      TopoDS_Shape aShape_i_copy;
      TNaming_CopyShape::CopyTool(aShape_i, aMapTShapes, aShape_i_copy);

      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aShape_i_inds;
      TopTools_IndexedMapOfShape aShape_i_copy_inds;
      TopExp::MapShapes(aShape_i, aShape_i_inds);
      TopExp::MapShapes(aShape_i_copy, aShape_i_copy_inds);
      Standard_Integer nbInds = aShape_i_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aShape_i_inds.FindKey(ie), aShape_i_copy_inds.FindKey(ie));
      }
      //
      TopTools_ListOfShape aSimpleShapes;
      //PrepareShapes(aShape_i, aType, aSimpleShapes);
      PrepareShapes(aShape_i_copy, aType, aSimpleShapes);
      TopTools_ListIteratorOfListOfShape aSimpleIter (aSimpleShapes);
      for (; aSimpleIter.More(); aSimpleIter.Next()) {
        const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
        if (ShapesMap.Add(aSimpleSh)) {
          PS.AddArgument(aSimpleSh);
          //skl if (DoRemoveWebs) {
          //skl if (aMaterials->Length() >= ind)
          //skl PS.SetMaterial(aSimpleSh, aMaterials->Value(ind));
          //skl }
        }
      }
    }

    // add tool shapes that are in ListTools and not in ListShapes;
    for (ind = 1; ind <= aTools->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aTools->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a tool shape is null");
      }

      // Check self-intersection.
      if (isCheckSelfInte && aType == PARTITION_NO_SELF_INTERSECTIONS) {
        CheckSelfIntersection(aShape_i);
      }

      //
      //BRepBuilderAPI_Copy aCopyTool (aShape_i);
      TopoDS_Shape aShape_i_copy;
      TNaming_CopyShape::CopyTool(aShape_i, aMapTShapes, aShape_i_copy);
      //if (aCopyTool.IsDone())
      //  aShape_i_copy = aCopyTool.Shape();
      //else
      //  Standard_NullObject::Raise("Bad shape detected");
      //
      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aShape_i_inds;
      TopTools_IndexedMapOfShape aShape_i_copy_inds;
      TopExp::MapShapes(aShape_i, aShape_i_inds);
      TopExp::MapShapes(aShape_i_copy, aShape_i_copy_inds);
      Standard_Integer nbInds = aShape_i_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aShape_i_inds.FindKey(ie), aShape_i_copy_inds.FindKey(ie));
      }
      //
      TopTools_ListOfShape aSimpleShapes;
      //PrepareShapes(aShape_i, aType, aSimpleShapes);
      PrepareShapes(aShape_i_copy, aType, aSimpleShapes);
      TopTools_ListIteratorOfListOfShape aSimpleIter (aSimpleShapes);
      for (; aSimpleIter.More(); aSimpleIter.Next()) {
        const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
        if (!ShapesMap.Contains(aSimpleSh) && ToolsMap.Add(aSimpleSh)) {
          PS.AddTool(aSimpleSh);
        }
      }
    }

    // add shapes that are in ListKeepInside, as object shapes;
    for (ind = 1; ind <= aKeepIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aKeepIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a Keep Inside shape is null");
      }
      //
      //BRepBuilderAPI_Copy aCopyTool (aShape_i);
      TopoDS_Shape aShape_i_copy;
      TNaming_CopyShape::CopyTool(aShape_i, aMapTShapes, aShape_i_copy);
      //if (aCopyTool.IsDone())
      //  aShape_i_copy = aCopyTool.Shape();
      //else
      //  Standard_NullObject::Raise("Bad shape detected");
      //
      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aShape_i_inds;
      TopTools_IndexedMapOfShape aShape_i_copy_inds;
      TopExp::MapShapes(aShape_i, aShape_i_inds);
      TopExp::MapShapes(aShape_i_copy, aShape_i_copy_inds);
      Standard_Integer nbInds = aShape_i_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aShape_i_inds.FindKey(ie), aShape_i_copy_inds.FindKey(ie));
      }
      //
      TopTools_ListOfShape aSimpleShapes;
      //PrepareShapes(aShape_i, aType, aSimpleShapes);
      PrepareShapes(aShape_i_copy, aType, aSimpleShapes);
      TopTools_ListIteratorOfListOfShape aSimpleIter (aSimpleShapes);
      for (; aSimpleIter.More(); aSimpleIter.Next()) {
        const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
        if (!ToolsMap.Contains(aSimpleSh) && ShapesMap.Add(aSimpleSh))
          PS.AddArgument(aSimpleSh);
      }
    }

    // add shapes that are in ListRemoveInside, as object shapes;
    for (ind = 1; ind <= aRemIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aRemIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a Remove Inside shape is null");
      }
      //
      //BRepBuilderAPI_Copy aCopyTool (aShape_i);
      TopoDS_Shape aShape_i_copy;
      TNaming_CopyShape::CopyTool(aShape_i, aMapTShapes, aShape_i_copy);
      //if (aCopyTool.IsDone())
      //  aShape_i_copy = aCopyTool.Shape();
      //else
      //  Standard_NullObject::Raise("Bad shape detected");
      //
      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aShape_i_inds;
      TopTools_IndexedMapOfShape aShape_i_copy_inds;
      TopExp::MapShapes(aShape_i, aShape_i_inds);
      TopExp::MapShapes(aShape_i_copy, aShape_i_copy_inds);
      Standard_Integer nbInds = aShape_i_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aShape_i_inds.FindKey(ie), aShape_i_copy_inds.FindKey(ie));
      }
      //
      TopTools_ListOfShape aSimpleShapes;
      //PrepareShapes(aShape_i, aType, aSimpleShapes);
      PrepareShapes(aShape_i_copy, aType, aSimpleShapes);
      TopTools_ListIteratorOfListOfShape aSimpleIter (aSimpleShapes);
      for (; aSimpleIter.More(); aSimpleIter.Next()) {
        const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();
        if (!ToolsMap.Contains(aSimpleSh) && ShapesMap.Add(aSimpleSh))
          PS.AddArgument(aSimpleSh);
      }
    }

    PS.SetLimitMode(aCI.GetKeepNonlimitShapes());
    PS.SetLimit((TopAbs_ShapeEnum)aCI.GetLimit());
    PS.Perform();

    //skl PS.Compute();
    //skl PS.SetRemoveWebs(!DoRemoveWebs);
    //skl PS.Build((TopAbs_ShapeEnum) aCI.GetLimit());
    /*skl
    // suppress result outside of shapes in KInsideMap
    for (ind = 1; ind <= aKeepIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aKeepIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      PS.KeepShapesInside(aShape_i);
    }

    // suppress result inside of shapes in RInsideMap
    for (ind = 1; ind <= aRemIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aRemIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      PS.RemoveShapesInside(aShape_i);
    }
    */
  }
  else if (aType == PARTITION_HALF)
  {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefPlane = aCI.GetPlane();
    TopoDS_Shape aShapeArg = aRefShape->GetValue();
    TopoDS_Shape aPlaneArg = aRefPlane->GetValue();

    if (aShapeArg.IsNull() || aPlaneArg.IsNull()) {
      Standard_NullObject::Raise("In Half Partition a shape or a plane is null");
    }

    TopoDS_Shape aShapeArg_copy;
    TopoDS_Shape aPlaneArg_copy;
    {
      TNaming_CopyShape::CopyTool(aShapeArg, aMapTShapes, aShapeArg_copy);
      //BRepBuilderAPI_Copy aCopyTool (aShapeArg);
      //if (aCopyTool.IsDone())
      //  aShapeArg_copy = aCopyTool.Shape();
      //else
      //  Standard_NullObject::Raise("Bad shape detected");
      //
      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aShapeArg_inds;
      TopTools_IndexedMapOfShape aShapeArg_copy_inds;
      TopExp::MapShapes(aShapeArg, aShapeArg_inds);
      TopExp::MapShapes(aShapeArg_copy, aShapeArg_copy_inds);
      Standard_Integer nbInds = aShapeArg_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aShapeArg_inds.FindKey(ie), aShapeArg_copy_inds.FindKey(ie));
      }
    }
    {
      TNaming_CopyShape::CopyTool(aPlaneArg, aMapTShapes, aPlaneArg_copy);
      //BRepBuilderAPI_Copy aCopyTool (aPlaneArg);
      //if (aCopyTool.IsDone())
      //  aPlaneArg_copy = aCopyTool.Shape();
      //else
      //  Standard_NullObject::Raise("Bad shape detected");
      //
      // fill aCopyMap for history
      TopTools_IndexedMapOfShape aPlaneArg_inds;
      TopTools_IndexedMapOfShape aPlaneArg_copy_inds;
      TopExp::MapShapes(aPlaneArg, aPlaneArg_inds);
      TopExp::MapShapes(aPlaneArg_copy, aPlaneArg_copy_inds);
      Standard_Integer nbInds = aPlaneArg_inds.Extent();
      for (Standard_Integer ie = 1; ie <= nbInds; ie++) {
        aCopyMap.Bind(aPlaneArg_inds.FindKey(ie), aPlaneArg_copy_inds.FindKey(ie));
      }
    }

    // add object shapes that are in ListShapes;
    TopTools_ListOfShape aSimpleShapes;
    TopTools_MapOfShape aShapesMap;

    PrepareShapes(aShapeArg_copy, aType, aSimpleShapes);

    TopTools_ListIteratorOfListOfShape aSimpleIter (aSimpleShapes);

    for (; aSimpleIter.More(); aSimpleIter.Next()) {
      const TopoDS_Shape& aSimpleSh = aSimpleIter.Value();

      if (aShapesMap.Add(aSimpleSh)) {
        PS.AddArgument(aSimpleSh);
      }
    }

    // add tool shapes that are in ListTools and not in ListShapes;
    PS.AddTool(aPlaneArg_copy);
    //PS.AddTool(aPlaneArg);

    //skl PS.Compute();
    PS.Perform();
    //PS.SetRemoveWebs(Standard_False);
    //PS.Build(aShapeArg.ShapeType());

  } else {
  }

  aShape = PS.Shape();
  if (aShape.IsNull()) {
    // Mantis issue 22009
    if (PS.ErrorStatus() == 100 && PS.Tools().Extent() == 0 && PS.Arguments().Extent() == 1)
      aShape = PS.Arguments().First();
    else
      return 0;
  }

  //Alternative case to check not valid partition IPAL21418
  TopoDS_Iterator It (aShape, Standard_True, Standard_True);
  int nbSubshapes = 0;
  for (; It.More(); It.Next())
    nbSubshapes++;
  if (!nbSubshapes)
    Standard_ConstructionError::Raise("Partition aborted : non valid shape result");
  //end of IPAL21418

  if ( !GEOMUtils::CheckShape(aShape, true) && !GEOMUtils::FixShapeTolerance(aShape) )
    Standard_ConstructionError::Raise("Partition aborted : non valid shape result");

  aFunction->SetValue(aShape);

  // Fill history to be used by GetInPlace functionality
  TopTools_IndexedMapOfShape aResIndices;
  TopExp::MapShapes(aShape, aResIndices);

  // Map: source_shape/images of source_shape in Result
  const BOPCol_IndexedDataMapOfShapeListOfShape& aMR = PS.ImagesResult();
  //const TopTools_IndexedDataMapOfShapeListOfShape& aMR = PS.ImagesResult();

  // history for all argument shapes
  // be sure to use aCopyMap
  TDF_LabelSequence aLabelSeq;
  aFunction->GetDependency(aLabelSeq);
  Standard_Integer nbArg = aLabelSeq.Length();

  for (Standard_Integer iarg = 1; iarg <= nbArg; iarg++) {

    TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

    Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
    TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

    TopTools_IndexedMapOfShape anArgumentIndices;
    TopExp::MapShapes(anArgumentShape, anArgumentIndices);
    Standard_Integer nbArgumentEntities = anArgumentIndices.Extent();

    // Find corresponding label in history
    TDF_Label anArgumentHistoryLabel =
      aFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_True);

    for (Standard_Integer ie = 1; ie <= nbArgumentEntities; ie++) {
      TopoDS_Shape anEntity = anArgumentIndices.FindKey(ie);
      // be sure to use aCopyMap here
      if (aCopyMap.IsBound(anEntity))
        anEntity = aCopyMap.Find(anEntity);
      //
      if (!aMR.Contains(anEntity)) continue;

      const BOPCol_ListOfShape& aModified = aMR.FindFromKey(anEntity);
      //const TopTools_ListOfShape& aModified = aMR.FindFromKey(anEntity);
      Standard_Integer nbModified = aModified.Extent();

      if (nbModified > 0) { // Mantis issue 0021182
        int ih = 1;
        BOPCol_ListIteratorOfListOfShape itM (aModified);
        for (; itM.More() && nbModified > 0; itM.Next(), ++ih) {
          if (!aResIndices.Contains(itM.Value())) {
            nbModified = 0;
          }
        }
      }
      if (nbModified > 0) {
        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(ie, Standard_True);
        Handle(TDataStd_IntegerArray) anAttr =
          TDataStd_IntegerArray::Set(aWhatHistoryLabel, 1, nbModified);

        int ih = 1;
        BOPCol_ListIteratorOfListOfShape itM (aModified);
        //TopTools_ListIteratorOfListOfShape itM (aModified);
        for (; itM.More(); itM.Next(), ++ih) {
          int id = aResIndices.FindIndex(itM.Value());
          anAttr->SetValue(ih, id);
        }
      }
    }
  }

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_PartitionDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPartition aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "PARTITION";

  switch ( aType ) {
  case PARTITION_PARTITION:
  case PARTITION_NO_SELF_INTERSECTIONS:
    AddParam( theParams, "Objects", aCI.GetShapes() );
    AddParam( theParams, "Tool objects", aCI.GetTools() );
    {
      Handle(TColStd_HSequenceOfTransient) objSeq = aCI.GetKeepIns();
      if ( !objSeq.IsNull() && objSeq->Length() > 0 )
        AddParam( theParams, "Objects to keep inside", objSeq );
      objSeq = aCI.GetRemoveIns();
      if ( !objSeq.IsNull() && objSeq->Length() > 0 )
        AddParam( theParams, "Objects to remove inside", objSeq );
      Handle(TColStd_HArray1OfInteger) intSeq = aCI.GetMaterials();
      if ( !intSeq.IsNull() && intSeq->Length() > 0 )
        AddParam( theParams, "Materials", aCI.GetMaterials() );
    }
    AddParam( theParams, "Resulting type", (TopAbs_ShapeEnum) aCI.GetLimit());
    AddParam( theParams, "Keep shapes of lower type", aCI.GetKeepNonlimitShapes());
    AddParam( theParams, "No object intersections", ( aType == PARTITION_NO_SELF_INTERSECTIONS ));

    if (aType == PARTITION_NO_SELF_INTERSECTIONS) {
      AddParam( theParams, "Check self-intersections", aCI.GetCheckSelfIntersection());
    }
    break;
  case PARTITION_HALF:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "Plane", aCI.GetPlane() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PartitionDriver,GEOM_BaseDriver);
