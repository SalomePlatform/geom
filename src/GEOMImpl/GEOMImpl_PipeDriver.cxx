
using namespace std;
#include "GEOMImpl_PipeDriver.hxx"
#include "GEOMImpl_IPipe.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PipeDriver::GetID()
{
  static Standard_GUID aPipeDriver("FF1BBB19-5D14-4df2-980B-3A668264EA16");
  return aPipeDriver;
}


//=======================================================================
//function : GEOMImpl_PipeDriver
//purpose  :
//=======================================================================
GEOMImpl_PipeDriver::GEOMImpl_PipeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PipeDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPipe aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == PIPE_BASE_PATH) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    Handle(GEOM_Function) aRefPath = aCI.GetPath();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    TopoDS_Shape aShapePath = aRefPath->GetValue();
    if (aShapeBase.IsNull() || aShapePath.IsNull()) {
      Standard_NullObject::Raise("MakePipe aborted : null shape argument");
    }

    // Get path contour
    TopoDS_Wire aWire;
    if (aShapePath.ShapeType() == TopAbs_WIRE) {
      aWire = TopoDS::Wire(aShapePath);
    } else {
      if (aShapePath.ShapeType() == TopAbs_EDGE) {
        TopoDS_Edge anEdge = TopoDS::Edge(aShapePath);
        aWire = BRepBuilderAPI_MakeWire(anEdge);
      } else {
        Standard_TypeMismatch::Raise("MakePipe aborted : path shape is neither a wire nor an edge");
      }
    }

    // Make pipe
    aShape = BRepOffsetAPI_MakePipe(aWire, aShapeBase);
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  BRepCheck_Analyzer ana (aShape, Standard_False);
  if (!ana.IsValid()) {
    Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_PipeDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PipeDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PipeDriver",
			                                 sizeof(GEOMImpl_PipeDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_PipeDriver) Handle(GEOMImpl_PipeDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PipeDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PipeDriver))) {
       _anOtherObject = Handle(GEOMImpl_PipeDriver)((Handle(GEOMImpl_PipeDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
