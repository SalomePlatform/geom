
using namespace std;
#include "GEOMImpl_RevolutionDriver.hxx"
#include "GEOMImpl_IRevolution.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_RevolutionDriver::GetID()
{
  static Standard_GUID aRevolutionDriver("FF1BBB18-5D14-4df2-980B-3A668264EA16");
  return aRevolutionDriver; 
}


//=======================================================================
//function : GEOMImpl_RevolutionDriver
//purpose  : 
//=======================================================================
GEOMImpl_RevolutionDriver::GEOMImpl_RevolutionDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_RevolutionDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IRevolution aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == REVOLUTION_BASE_AXIS_ANGLE) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    Handle(GEOM_Function) aRefAxis = aCI.GetAxis();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    TopoDS_Shape aShapeAxis = aRefAxis->GetValue();
    if (aShapeAxis.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise("Revolution Axis must be an edge");
    }

    TopoDS_Edge anE = TopoDS::Edge(aShapeAxis);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (V1.IsNull() || V2.IsNull()) {
      Standard_ConstructionError::Raise("Bad edge for the Revolution Axis given");
    }

    gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
    if (aV.Magnitude() < Precision::Confusion()) {
      Standard_ConstructionError::Raise
        ("End vertices of edge, defining the Revolution Axis, are too close");
    }

    gp_Ax1 anAxis (BRep_Tool::Pnt(V1), aV);
    BRepPrimAPI_MakeRevol MR (aShapeBase, anAxis, aCI.GetAngle(), Standard_False);
    if (!MR.IsDone()) MR.Build();
    if (!MR.IsDone()) StdFail_NotDone::Raise("Revolution algorithm has failed");
    aShape = MR.Shape();
  } else {
  }

  if (aShape.IsNull()) return 0;
  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;    
}


//=======================================================================
//function :  GEOMImpl_RevolutionDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_RevolutionDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_RevolutionDriver",
			                                 sizeof(GEOMImpl_RevolutionDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_RevolutionDriver) Handle(GEOMImpl_RevolutionDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_RevolutionDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_RevolutionDriver))) {
       _anOtherObject = Handle(GEOMImpl_RevolutionDriver)((Handle(GEOMImpl_RevolutionDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
