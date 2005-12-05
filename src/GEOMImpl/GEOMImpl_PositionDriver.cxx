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
// See http://www.salome-platform.org/
//

#include <Standard_Stream.hxx>

#include <GEOMImpl_PositionDriver.hxx>
#include <GEOMImpl_IPosition.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_PositionDriver::GetID()
{
  static Standard_GUID aPositionDriver("FF1BBB69-5D14-4df2-980B-3A668264EA16");
  return aPositionDriver; 
}


//=======================================================================
//function : GEOMImpl_PositionDriver
//purpose  : 
//=======================================================================
GEOMImpl_PositionDriver::GEOMImpl_PositionDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_PositionDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPosition aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == POSITION_SHAPE || aType == POSITION_SHAPE_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefStartLCS = aCI.GetStartLCS();
    Handle(GEOM_Function) aRefEndLCS = aCI.GetEndLCS();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aShapeStartLCS = aRefStartLCS->GetValue();
    TopoDS_Shape aShapeEndLCS = aRefEndLCS->GetValue();

    if (aShapeBase.IsNull() || aShapeStartLCS.IsNull() || 
	aShapeEndLCS.IsNull() || aShapeEndLCS.ShapeType() != TopAbs_FACE)
      return 0;

    gp_Trsf aTrsf;
    gp_Ax3 aStartAx3, aDestAx3;
    aStartAx3.Transform(aShapeStartLCS.Location().Transformation());
    aDestAx3.Transform(aShapeEndLCS.Location().Transformation());

    Handle(Geom_Surface) aGS2 = BRep_Tool::Surface( TopoDS::Face( aShapeEndLCS ) );
    if (!aGS2.IsNull() && aGS2->IsKind( STANDARD_TYPE( Geom_Plane ) ) ) {
      Handle(Geom_Plane) aGPlane2 = Handle(Geom_Plane)::DownCast( aGS2 );
      gp_Pln aPln2 = aGPlane2->Pln();
      aDestAx3 = aPln2.Position();
    }

    if(aShapeStartLCS.ShapeType() == TopAbs_FACE) {
      Handle(Geom_Surface) aGS = BRep_Tool::Surface( TopoDS::Face( aShapeStartLCS ) );
      if (!aGS.IsNull() && aGS->IsKind( STANDARD_TYPE( Geom_Plane ) ) ) {
	Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast( aGS );
	gp_Pln aPln = aGPlane->Pln();
	aStartAx3 = aPln.Position();
      }
      aTrsf.SetDisplacement(aStartAx3, aDestAx3);
    }
    else {
      gp_Trsf aTrsf1, aTrsf2;
      aTrsf1.SetDisplacement(aStartAx3, aDestAx3); 
      BRepBuilderAPI_Transform aBT (aShapeBase, aTrsf1, Standard_False);
      TopoDS_Shape aNewShape = aBT.Shape();

      gp_Pnt aPnt;
      if (aNewShape.ShapeType() == TopAbs_VERTEX) {
	aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aNewShape));
      } 
      else {
	GProp_GProps aSystem;
	if (aNewShape.ShapeType() == TopAbs_EDGE || aNewShape.ShapeType() == TopAbs_WIRE)
	  BRepGProp::LinearProperties(aNewShape, aSystem);
	else if (aNewShape.ShapeType() == TopAbs_FACE || aNewShape.ShapeType() == TopAbs_SHELL)
	  BRepGProp::SurfaceProperties(aNewShape, aSystem);
	else
	  BRepGProp::VolumeProperties(aNewShape, aSystem);
	
	aPnt = aSystem.CentreOfMass();
      }

      gp_Vec aVec(aPnt, aDestAx3.Location());
      aTrsf2.SetTranslation(aVec);
      aTrsf = aTrsf2 * aTrsf1;
    }

    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else
    return 0;

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;    
}


//=======================================================================
//function :  GEOMImpl_PositionDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_PositionDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_PositionDriver",
			                                 sizeof(GEOMImpl_PositionDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_PositionDriver) Handle(GEOMImpl_PositionDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_PositionDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_PositionDriver))) {
       _anOtherObject = Handle(GEOMImpl_PositionDriver)((Handle(GEOMImpl_PositionDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
