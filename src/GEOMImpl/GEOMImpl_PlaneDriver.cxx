
using namespace std;
#include "GEOMImpl_PlaneDriver.hxx"
#include "GEOMImpl_IPlane.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <GC_MakePlane.hxx>
#include <Geom_Surface.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>

#include <Standard_ConstructionError.hxx>
#include <Standard_TypeMismatch.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_PlaneDriver::GetID()
{
  static Standard_GUID aPlaneDriver("FF1BBB05-5D14-4df2-980B-3A668264EA16");
  return aPlaneDriver; 
}


//=======================================================================
//function : GEOMImpl_PlaneDriver
//purpose  : 
//=======================================================================
GEOMImpl_PlaneDriver::GEOMImpl_PlaneDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PlaneDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull())  return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPlane aPI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  double aSize = aPI.GetSize() / 2.0;
  if (aType == PLANE_PNT_VEC) {
    Handle(GEOM_Function) aRefPnt = aPI.GetPoint();
    Handle(GEOM_Function) aRefVec = aPI.GetVector();
    TopoDS_Shape aShape1 = aRefPnt->GetValue();
    TopoDS_Shape aShape2 = aRefVec->GetValue();
    if (aShape1.ShapeType() != TopAbs_VERTEX ||
        aShape2.ShapeType() != TopAbs_EDGE) return 0;
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aShape1));
    TopoDS_Edge anE = TopoDS::Edge(aShape2);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (!V1.IsNull() && !V2.IsNull()) {
      gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
      gp_Pln aPln (aP, aV);
      aShape = BRepBuilderAPI_MakeFace(aPln, -aSize, +aSize, -aSize, +aSize).Shape();
    }
  } else if (aType == PLANE_THREE_PNT) {
    Handle(GEOM_Function) aRefPnt1 = aPI.GetPoint1();
    Handle(GEOM_Function) aRefPnt2 = aPI.GetPoint2();
    Handle(GEOM_Function) aRefPnt3 = aPI.GetPoint3();
    TopoDS_Shape aShape1 = aRefPnt1->GetValue();
    TopoDS_Shape aShape2 = aRefPnt2->GetValue();
    TopoDS_Shape aShape3 = aRefPnt3->GetValue();
    if (aShape1.ShapeType() != TopAbs_VERTEX ||
        aShape2.ShapeType() != TopAbs_VERTEX ||
        aShape3.ShapeType() != TopAbs_VERTEX) return 0;
    gp_Pnt aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aShape1));
    gp_Pnt aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aShape2));
    gp_Pnt aP3 = BRep_Tool::Pnt(TopoDS::Vertex(aShape3));
    if (aP1.Distance(aP2) < gp::Resolution() ||
        aP1.Distance(aP3) < gp::Resolution() ||
        aP2.Distance(aP3) < gp::Resolution())
      Standard_ConstructionError::Raise("Plane creation aborted: coincident points given");
    if (gp_Vec(aP1, aP2).IsParallel(gp_Vec(aP1, aP3), Precision::Angular()))
      Standard_ConstructionError::Raise("Plane creation aborted: points lay on one line");
    GC_MakePlane aMakePlane (aP1, aP2, aP3);
    aShape = BRepBuilderAPI_MakeFace(aMakePlane, -aSize, +aSize, -aSize, +aSize).Shape();
  } else if (aType == PLANE_FACE) {
    Handle(GEOM_Function) aRef = aPI.GetFace();
    TopoDS_Shape aRefShape = aRef->GetValue();
    if (aRefShape.ShapeType() != TopAbs_FACE) return 0;
    Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(aRefShape));
    if (!aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
      Standard_TypeMismatch::Raise("Plane creation aborted: non-planar face given as argument");
    }
    aShape = BRepBuilderAPI_MakeFace(aGS, -aSize, +aSize, -aSize, +aSize).Shape();
  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;
}


//=======================================================================
//function :  GEOMImpl_PlaneDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PlaneDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PlaneDriver",
			                                 sizeof(GEOMImpl_PlaneDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_PlaneDriver) Handle(GEOMImpl_PlaneDriver)::DownCast
       (const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PlaneDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PlaneDriver))) {
       _anOtherObject = Handle(GEOMImpl_PlaneDriver)((Handle(GEOMImpl_PlaneDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
