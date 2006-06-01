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

#include <GEOMImpl_MeasureDriver.hxx>
#include <GEOMImpl_IMeasure.hxx>
#include <GEOMImpl_IMeasureOperations.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

#include <GProp_GProps.hxx>

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_MeasureDriver::GetID()
{
  static Standard_GUID aMeasureDriver("FF1BBB65-5D14-4df2-980B-3A668264EA16");
  return aMeasureDriver; 
}


//=======================================================================
//function : GEOMImpl_MeasureDriver
//purpose  : 
//=======================================================================
GEOMImpl_MeasureDriver::GEOMImpl_MeasureDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_MeasureDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMeasure aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == CDG_MEASURE) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Shape for centre of mass calculation is null");
    }

    GProp_GProps aSystem;
    gp_Pnt aCenterMass;

    if (aShapeBase.ShapeType() == TopAbs_VERTEX) {
      aCenterMass = BRep_Tool::Pnt(TopoDS::Vertex(aShapeBase));
    } else if (aShapeBase.ShapeType() == TopAbs_EDGE || aShapeBase.ShapeType() == TopAbs_WIRE) {
      BRepGProp::LinearProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    } else if (aShapeBase.ShapeType() == TopAbs_FACE || aShapeBase.ShapeType() == TopAbs_SHELL) {
      BRepGProp::SurfaceProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    } else {
      BRepGProp::VolumeProperties(aShapeBase, aSystem);
      aCenterMass = aSystem.CentreOfMass();
    }

    aShape = BRepBuilderAPI_MakeVertex(aCenterMass).Shape();

  } else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label()); 

  return 1;    
}


//=======================================================================
//function :  GEOMImpl_MeasureDriver_Type_
//purpose  :
//======================================================================= 
Standard_EXPORT Handle_Standard_Type& GEOMImpl_MeasureDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared); 
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_MeasureDriver",
			                                 sizeof(GEOMImpl_MeasureDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//======================================================================= 
const Handle(GEOMImpl_MeasureDriver) Handle(GEOMImpl_MeasureDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_MeasureDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_MeasureDriver))) {
       _anOtherObject = Handle(GEOMImpl_MeasureDriver)((Handle(GEOMImpl_MeasureDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
