
using namespace std;
#include "GEOMImpl_EllipseDriver.hxx"
#include "GEOMImpl_IEllipse.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <gp_Pnt.hxx>
#include <gp_Elips.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_EllipseDriver::GetID()
{
  static Standard_GUID aEllipseDriver("FF1BBB34-5D14-4df2-980B-3A668264EA16");
  return aEllipseDriver; 
}


//=======================================================================
//function : GEOMImpl_EllipseDriver
//purpose  : 
//=======================================================================
GEOMImpl_EllipseDriver::GEOMImpl_EllipseDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_EllipseDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IEllipse aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == ELLIPSE_PNT_VEC_RR) {
    Handle(GEOM_Function) aRefPoint  = aCI.GetCenter();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    if (aShapePnt.ShapeType() == TopAbs_VERTEX &&
        aShapeVec.ShapeType() == TopAbs_EDGE) {
      gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
      TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anE, V1, V2, Standard_True);
      if (!V1.IsNull() && !V2.IsNull()) {
        gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
        gp_Ax2 anAxes (aP, aV);
        gp_Elips anEll (anAxes, aCI.GetRMajor(), aCI.GetRMinor());
        aShape = BRepBuilderAPI_MakeEdge(anEll).Edge();
      }
    }
  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;    
}


//=======================================================================
//function :  GEOMImpl_EllipseDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_EllipseDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_EllipseDriver",
			                                 sizeof(GEOMImpl_EllipseDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_EllipseDriver) Handle(GEOMImpl_EllipseDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_EllipseDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_EllipseDriver))) {
       _anOtherObject = Handle(GEOMImpl_EllipseDriver)((Handle(GEOMImpl_EllipseDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
