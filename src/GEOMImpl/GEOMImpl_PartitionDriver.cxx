
using namespace std;
#include "GEOMImpl_PartitionDriver.hxx"
#include "GEOMImpl_IPartition.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <NMTAlgo_Splitter1.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopTools_MapOfShape.hxx>

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

    NMTAlgo_Splitter1 PS;
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

    aShape = PS.Shape();

    if (!BRepAlgo::IsValid(aShape)) {
      Standard_ConstructionError::Raise("Partition aborted : non valid shape result");
    }
  } else if (aType == PARTITION_HALF) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefPlane = aCI.GetPlane();
    TopoDS_Shape aShapeArg = aRefShape->GetValue();
    TopoDS_Shape aPlaneArg = aRefPlane->GetValue();

    if (aShapeArg.IsNull() || aPlaneArg.IsNull()) {
      Standard_NullObject::Raise("In Half Partition a shape or a plane is null");
    }

    NMTAlgo_Splitter1 PS;

    // add object shapes that are in ListShapes;
    PS.AddShape(aShapeArg);

    // add tool shapes that are in ListTools and not in ListShapes;
    PS.AddTool(aPlaneArg);

    PS.Compute();
    PS.SetRemoveWebs(Standard_False);
    PS.Build(aShapeArg.ShapeType());

    aShape = PS.Shape();

    if (!BRepAlgo::IsValid(aShape)) {
      Standard_ConstructionError::Raise("Partition aborted : non valid shape result");
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

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
