
using namespace std;
#include "GEOMImpl_OffsetDriver.hxx"
#include "GEOMImpl_IOffset.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_OffsetDriver::GetID()
{
  static Standard_GUID aOffsetDriver("FF1BBB51-5D14-4df2-980B-3A668264EA16");
  return aOffsetDriver;
}


//=======================================================================
//function : GEOMImpl_OffsetDriver
//purpose  :
//=======================================================================
GEOMImpl_OffsetDriver::GEOMImpl_OffsetDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_OffsetDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IOffset aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == OFFSET_SHAPE || aType == OFFSET_SHAPE_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    Standard_Real anOffset = aCI.GetValue();
    Standard_Real aTol = Precision::Confusion();

    if (Abs(anOffset) < aTol) {
      TCollection_AsciiString aMsg ("Absolute value of offset can not be less than the tolerance value (");
      aMsg += TCollection_AsciiString(aTol);
      aMsg += ")";
      StdFail_NotDone::Raise(aMsg.ToCString());
    }

    BRepOffsetAPI_MakeOffsetShape MO (aShapeBase,
                                      aCI.GetValue(),
                                      aTol);
    if (MO.IsDone()) {
      aShape = MO.Shape();
    } else {
      StdFail_NotDone::Raise("Offset construction failed");
    }
  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}


//=======================================================================
//function :  GEOMImpl_OffsetDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_OffsetDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_OffsetDriver",
			                                 sizeof(GEOMImpl_OffsetDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_OffsetDriver) Handle(GEOMImpl_OffsetDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_OffsetDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_OffsetDriver))) {
       _anOtherObject = Handle(GEOMImpl_OffsetDriver)((Handle(GEOMImpl_OffsetDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
