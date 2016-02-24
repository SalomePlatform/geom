// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include <GEOMImpl_ConeDriver.hxx>
#include <GEOMImpl_ICone.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <Standard_TypeMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ConeDriver::GetID()
{
  static Standard_GUID aConeDriver("FF1BBB15-5D14-4df2-980B-3A668264EA16");
  return aConeDriver;
}


//=======================================================================
//function : GEOMImpl_ConeDriver
//purpose  :
//=======================================================================
GEOMImpl_ConeDriver::GEOMImpl_ConeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ConeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICone aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  gp_Pnt aP;
  gp_Vec aV;

  Standard_Real aR1 = aCI.GetR1();
  Standard_Real aR2 = aCI.GetR2();

  if (aType == CONE_R1_R2_H) {
    aP = gp::Origin();
    aV = gp::DZ();

  } else if (aType == CONE_PNT_VEC_R1_R2_H) {
    Handle(GEOM_Function) aRefPoint  = aCI.GetPoint();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    if (aShapePnt.IsNull() || aShapeVec.IsNull()) {
      Standard_NullObject::Raise
        ("Cone creation aborted: point or vector is not defined");
    }
    if (aShapePnt.ShapeType() != TopAbs_VERTEX ||
        aShapeVec.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Cone creation aborted: point or vector shapes has wrong type");
    }

    aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));

    TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (V1.IsNull() || V2.IsNull()) {
      Standard_NullObject::Raise
        ("Cylinder creation aborted: vector is not defined");
    }
    aV = gp_Vec(BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));

  } else {
    return 0;
  }

  if (aCI.GetH() < 0.0) aV.Reverse();
  gp_Ax2 anAxes (aP, aV);

  TopoDS_Shape aShape;
  // Cone does not work if same radius
  if (fabs(aR1 - aR2) <= Precision::Confusion()) {
    BRepPrimAPI_MakeCylinder MC (anAxes, (aR1 + aR2)/2.0, Abs(aCI.GetH()));
    MC.Build();
    if (!MC.IsDone()) {
      StdFail_NotDone::Raise("Cylinder can't be computed from the given parameters");
    }
    aShape = MC.Shape();
  } else {
    BRepPrimAPI_MakeCone MC (anAxes, aCI.GetR1(), aCI.GetR2(), Abs(aCI.GetH()));
    MC.Build();
    if (!MC.IsDone()) {
      StdFail_NotDone::Raise("Cylinder can't be computed from the given parameters");
    }
    aShape = MC.Shape();
  }
  if (aShape.IsNull()) return 0;

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  aFunction->SetValue(aShape);
  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_ConeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICone aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CONE";

  switch ( aType ) {
  case CONE_R1_R2_H:
    AddParam( theParams, "Radius 1", aCI.GetR1() );
    AddParam( theParams, "Radius 2", aCI.GetR2() );
    AddParam( theParams, "Height", aCI.GetH() );
    break;
  case CONE_PNT_VEC_R1_R2_H:
    AddParam( theParams, "Base Point", aCI.GetPoint() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Radius 1", aCI.GetR1() );
    AddParam( theParams, "Radius 2", aCI.GetR2() );
    AddParam( theParams, "Height", aCI.GetH() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ConeDriver,GEOM_BaseDriver);
