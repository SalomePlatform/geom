
using namespace std;
#include "GEOMImpl_RotateDriver.hxx"
#include <GEOMImpl_IRotate.hxx>
#include <GEOMImpl_Types.hxx>
#include "GEOM_Function.hxx"
#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Line.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <Precision.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_RotateDriver::GetID()
{
  static Standard_GUID aRotateDriver("FF1BBB56-5D14-4df2-980B-3A668264EA16");
  return aRotateDriver; 
}


//=======================================================================
//function : GEOMImpl_RotateDriver
//purpose  : 
//=======================================================================

GEOMImpl_RotateDriver::GEOMImpl_RotateDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_RotateDriver::Execute(TFunction_Logbook& log) const
{
  if(Label().IsNull())  return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if(aFunction.IsNull()) return 0;

  GEOMImpl_IRotate RI(aFunction);
  gp_Trsf aTrsf;
  Standard_Integer aType = aFunction->GetType();
  Handle(GEOM_Function) anOriginalFunction = RI.GetOriginal();
  if(anOriginalFunction.IsNull()) return 0;
  TopoDS_Shape aShape, anOriginal = anOriginalFunction->GetValue();
  if(anOriginal.IsNull()) return 0;

  if(aType == ROTATE || aType == ROTATE_COPY) {
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if(anAxis.IsNull()) return 0;
    TopoDS_Shape A = anAxis->GetValue();
    if(A.IsNull() || A.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(A);

    gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));
    gp_Dir aDir(gp_Vec(aP1, aP2));
    gp_Ax1 anAx1(aP1, aDir);
    Standard_Real anAngle = RI.GetAngle();
    aTrsf.SetRotation(anAx1, anAngle);
    
    BRepBuilderAPI_Transform aTransformation(anOriginal, aTrsf, Standard_False);
    aShape = aTransformation.Shape();
  }
  else if(aType == ROTATE_1D) {
    //Get direction
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if(anAxis.IsNull()) return 0;
    TopoDS_Shape A = anAxis->GetValue();
    if(A.IsNull() || A.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(A);

    gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));
    gp_Dir D(gp_Vec(aP1, aP2)) ;

    gp_Ax1 AX1(aP1, D) ;

    Standard_Integer nbtimes = RI.GetNbIter1();
    Standard_Real angle = 360.0/nbtimes ;

    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );
    
    for (int i = 0; i < nbtimes; i++ ) {
      aTrsf.SetRotation(AX1, i*angle*PI180) ;
      BRepBuilderAPI_Transform myBRepTransformation(anOriginal, aTrsf, Standard_False) ;
      B.Add( aCompound, myBRepTransformation.Shape() );
    }

    aShape = aCompound ;
  }
  else if(aType == ROTATE_2D) {
    Standard_Real DX, DY, DZ ;

    //Get direction
    Handle(GEOM_Function) anAxis = RI.GetAxis();
    if(anAxis.IsNull()) return 0;
    TopoDS_Shape A = anAxis->GetValue();
    if(A.IsNull() || A.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(A);
    gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex(anEdge));
    gp_Dir D(gp_Vec(aP1, aP2)) ;

    gp_Ax1 AX1(aP1, D) ;


    gp_Trsf theTransformation1 ;
    gp_Trsf theTransformation2 ;
    gp_Pnt P1 ;
    GProp_GProps System ;
    
    if ( anOriginal.ShapeType() == TopAbs_VERTEX) {
      P1 = BRep_Tool::Pnt(TopoDS::Vertex( anOriginal ));
    } 
    else if ( anOriginal.ShapeType() == TopAbs_EDGE || anOriginal.ShapeType() == TopAbs_WIRE ) {
      BRepGProp::LinearProperties(anOriginal, System);
      P1 = System.CentreOfMass() ;
    }
    else if ( anOriginal.ShapeType() == TopAbs_FACE || anOriginal.ShapeType() == TopAbs_SHELL ) {
      BRepGProp::SurfaceProperties(anOriginal, System);
      P1 = System.CentreOfMass() ;
    }
    else {
      BRepGProp::VolumeProperties(anOriginal, System);
      P1 = System.CentreOfMass() ;
    }
    
    Handle(Geom_Line) Line = new Geom_Line(AX1);
    gp_Pnt P2 = GeomAPI_ProjectPointOnCurve( P1, Line ) ;
    
    if ( P1.IsEqual(P2, Precision::Confusion() ) ) return 0;
    
    gp_Vec Vec(P1.X()-P2.X(), P1.Y()-P2.Y(), P1.Z()-P2.Z()) ;
    Vec.Normalize();
    
    Standard_Integer nbtimes2 = RI.GetNbIter2();
    Standard_Integer nbtimes1 = RI.GetNbIter1();
    Standard_Real step = RI.GetStep();
    Standard_Real ang = RI.GetAngle();

    gp_Vec myVec ;
    TopoDS_Compound aCompound;
    BRep_Builder B;
    B.MakeCompound( aCompound );
    for (int i = 0; i < nbtimes2; i++ ) {
      for (int j = 0; j < nbtimes1; j++ ) {
	DX = i * step * Vec.X() ;
	DY = i * step * Vec.Y() ;
	DZ = i * step * Vec.Z() ;
	myVec.SetCoord( DX, DY, DZ ) ;
	theTransformation1.SetTranslation(myVec) ;
	theTransformation2.SetRotation(AX1, j*ang*PI180) ;
	BRepBuilderAPI_Transform myBRepTransformation1(anOriginal, theTransformation1, Standard_False) ;
	BRepBuilderAPI_Transform myBRepTransformation2(myBRepTransformation1.Shape(), theTransformation2, Standard_False) ;
	B.Add( aCompound, myBRepTransformation2.Shape() );
      }
    }

    aShape = aCompound;

  }
  else return 0;

 


  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;
}


//=======================================================================
//function :  GEOMImpl_RotateDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_RotateDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_RotateDriver",
			                                 sizeof(GEOMImpl_RotateDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 

const Handle(GEOMImpl_RotateDriver) Handle(GEOMImpl_RotateDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_RotateDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_RotateDriver))) {
       _anOtherObject = Handle(GEOMImpl_RotateDriver)((Handle(GEOMImpl_RotateDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}


