
using namespace std;
#include "GEOMImpl_BoxDriver.hxx"
#include "GEOMImpl_IBox.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_BoxDriver::GetID()
{
  static Standard_GUID aBoxDriver("FF1BBB13-5D14-4df2-980B-3A668264EA16");
  return aBoxDriver;
}


//=======================================================================
//function : GEOMImpl_BoxDriver
//purpose  :
//=======================================================================
GEOMImpl_BoxDriver::GEOMImpl_BoxDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_BoxDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IBox aBI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == BOX_DX_DY_DZ) {
    BRepPrimAPI_MakeBox MB (aBI.GetDX(), aBI.GetDY(), aBI.GetDZ());
    MB.Build();

    if (!MB.IsDone()) {
      StdFail_NotDone::Raise("Box with the given dimensions can not be computed");
    }
    aShape = MB.Shape();
  }
  else if (aType == BOX_TWO_PNT) {
    Handle(GEOM_Function) aRefPoint1 = aBI.GetRef1();
    Handle(GEOM_Function) aRefPoint2 = aBI.GetRef2();
    TopoDS_Shape aShape1 = aRefPoint1->GetValue();
    TopoDS_Shape aShape2 = aRefPoint2->GetValue();
    if (aShape1.ShapeType() == TopAbs_VERTEX &&
        aShape2.ShapeType() == TopAbs_VERTEX) {
      gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(aShape1));
      gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(aShape2));
      BRepPrimAPI_MakeBox MB (P1,P2);
      MB.Build();

      if (!MB.IsDone()) {
        StdFail_NotDone::Raise("Box can not be computed from the given point");
      }
      aShape = MB.Shape();
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
//function :  GEOMImpl_BoxDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_BoxDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_BoxDriver",
			                                 sizeof(GEOMImpl_BoxDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_BoxDriver) Handle(GEOMImpl_BoxDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_BoxDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_BoxDriver))) {
       _anOtherObject = Handle(GEOMImpl_BoxDriver)((Handle(GEOMImpl_BoxDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
