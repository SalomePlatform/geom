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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_PartitionDriver.hxx>
#include <GEOMImpl_IPartition.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Object.hxx>
#include <GEOM_Function.hxx>

#include <NMTAlgo_Splitter1.hxx>

#include <TDataStd_IntegerArray.hxx>

#include <BRep_Tool.hxx>
#include <BRepAlgo.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_NullObject.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

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
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PartitionDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPartition aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  NMTAlgo_Splitter1 PS;

  if (aType == PARTITION_PARTITION) {
    Handle(TColStd_HSequenceOfTransient) aShapes  = aCI.GetShapes();
    Handle(TColStd_HSequenceOfTransient) aTools   = aCI.GetTools();
    Handle(TColStd_HSequenceOfTransient) aKeepIns = aCI.GetKeepIns();
    Handle(TColStd_HSequenceOfTransient) aRemIns  = aCI.GetRemoveIns();
    Handle(TColStd_HArray1OfInteger) aMaterials   = aCI.GetMaterials();
    Standard_Boolean DoRemoveWebs = !aMaterials.IsNull();

    unsigned int ind, nbshapes = 0;
    nbshapes += aShapes->Length() + aTools->Length();
    nbshapes += aKeepIns->Length() + aRemIns->Length();

    TopTools_MapOfShape ShapesMap(nbshapes), ToolsMap(nbshapes);

    // add object shapes that are in ListShapes;
    for (ind = 1; ind <= aShapes->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a shape is null");
      }
      if (ShapesMap.Add(aShape_i)) {
        PS.AddShape(aShape_i);
        if (DoRemoveWebs) {
          if (aMaterials->Length() >= ind)
            PS.SetMaterial(aShape_i, aMaterials->Value(ind));
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
      if (!ShapesMap.Contains(aShape_i) && ToolsMap.Add(aShape_i))
        PS.AddTool(aShape_i);
    }

    // add shapes that are in ListKeepInside, as object shapes;
    for (ind = 1; ind <= aKeepIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aKeepIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a Keep Inside shape is null");
      }
      if (!ToolsMap.Contains(aShape_i) && ShapesMap.Add(aShape_i))
        PS.AddShape(aShape_i);
    }

    // add shapes that are in ListRemoveInside, as object shapes;
    for (ind = 1; ind <= aRemIns->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aRemIns->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("In Partition a Remove Inside shape is null");
      }
      if (!ToolsMap.Contains(aShape_i) && ShapesMap.Add(aShape_i))
        PS.AddShape(aShape_i);
    }

    PS.Compute();
    PS.SetRemoveWebs(DoRemoveWebs);
    PS.Build((TopAbs_ShapeEnum) aCI.GetLimit());

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

  } else if (aType == PARTITION_HALF) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefPlane = aCI.GetPlane();
    TopoDS_Shape aShapeArg = aRefShape->GetValue();
    TopoDS_Shape aPlaneArg = aRefPlane->GetValue();

    if (aShapeArg.IsNull() || aPlaneArg.IsNull()) {
      Standard_NullObject::Raise("In Half Partition a shape or a plane is null");
    }

    // add object shapes that are in ListShapes;
    PS.AddShape(aShapeArg);

    // add tool shapes that are in ListTools and not in ListShapes;
    PS.AddTool(aPlaneArg);

    PS.Compute();
    PS.SetRemoveWebs(Standard_False);
    PS.Build(aShapeArg.ShapeType());

  } else {
  }

  aShape = PS.Shape();
  if (aShape.IsNull()) return 0;

  if (!BRepAlgo::IsValid(aShape)) {
    Standard_ConstructionError::Raise("Partition aborted : non valid shape result");
  }

  aFunction->SetValue(aShape);

  // Fill history to be used by GetInPlace functionality
  TopTools_IndexedMapOfShape aResIndices;
  TopExp::MapShapes(aShape, aResIndices);

  // history for all argument shapes
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
      const TopTools_ListOfShape& aModified = PS.Modified(anEntity);
      Standard_Integer nbModified = aModified.Extent();

      if (nbModified > 0) {
        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(ie, Standard_True);
        Handle(TDataStd_IntegerArray) anAttr =
          TDataStd_IntegerArray::Set(aWhatHistoryLabel, 1, nbModified);

        TopTools_ListIteratorOfListOfShape itM (aModified);
        for (int im = 1; itM.More(); itM.Next(), ++im) {
          int id = aResIndices.FindIndex(itM.Value());
          anAttr->SetValue(im, id);
        }
      }
    }
  }

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_PartitionDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PartitionDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PartitionDriver",
			                                 sizeof(GEOMImpl_PartitionDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_PartitionDriver) Handle(GEOMImpl_PartitionDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PartitionDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PartitionDriver))) {
       _anOtherObject = Handle(GEOMImpl_PartitionDriver)((Handle(GEOMImpl_PartitionDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
