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

#include <GEOMImpl_CircleDriver.hxx>

#include <GEOMImpl_ICircle.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>

#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Circ.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_CircleDriver::GetID()
{
  static Standard_GUID aCircleDriver("FF1BBB32-5D14-4df2-980B-3A668264EA16");
  return aCircleDriver;
}


//=======================================================================
//function : GEOMImpl_CircleDriver
//purpose  :
//=======================================================================
GEOMImpl_CircleDriver::GEOMImpl_CircleDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_CircleDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICircle aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == CIRCLE_PNT_VEC_R) {
    // Center
    gp_Pnt aP = gp::Origin();
    Handle(GEOM_Function) aRefPoint = aCI.GetCenter();
    if (!aRefPoint.IsNull()) {
      TopoDS_Shape aShapePnt = aRefPoint->GetValue();
      if (aShapePnt.ShapeType() != TopAbs_VERTEX) {
        Standard_ConstructionError::Raise
          ("Circle creation aborted: invalid center argument, must be a point");
      }
      aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
    }
    // Normal
    gp_Vec aV = gp::DZ();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    if (!aRefVector.IsNull()) {
      TopoDS_Shape aShapeVec = aRefVector->GetValue();
      // take orientation of edge into account to avoid regressions, as it was implemented so
      aV = GEOMUtils::GetVector(aShapeVec, Standard_True);
    }
    // Axes
    gp_Ax2 anAxes (aP, aV);
    // Radius
    double anR = aCI.GetRadius();
    char aMsg[] = "Circle creation aborted: radius value less than 1e-07 is not acceptable";
    if (anR < Precision::Confusion())
      Standard_ConstructionError::Raise(aMsg);
    // Circle
    gp_Circ aCirc (anAxes, anR);
    aShape = BRepBuilderAPI_MakeEdge(aCirc).Edge();
  }
  else if (aType == CIRCLE_CENTER_TWO_PNT) {
    Handle(GEOM_Function) aRefPoint1 = aCI.GetPoint1();
    Handle(GEOM_Function) aRefPoint2 = aCI.GetPoint2();
    Handle(GEOM_Function) aRefPoint3 = aCI.GetPoint3();
    TopoDS_Shape aShapePnt1 = aRefPoint1->GetValue();
    TopoDS_Shape aShapePnt2 = aRefPoint2->GetValue();
    TopoDS_Shape aShapePnt3 = aRefPoint3->GetValue();
    if (aShapePnt1.ShapeType() == TopAbs_VERTEX &&
        aShapePnt2.ShapeType() == TopAbs_VERTEX &&
        aShapePnt3.ShapeType() == TopAbs_VERTEX)
    {
      gp_Pnt aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt1));
      gp_Pnt aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt2));
      gp_Pnt aP3 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt3));

      if (aP1.Distance(aP2) < gp::Resolution() ||
          aP1.Distance(aP3) < gp::Resolution() ||
          aP2.Distance(aP3) < gp::Resolution())
        Standard_ConstructionError::Raise("Circle creation aborted: coincident points given");

      if (gp_Vec(aP1, aP2).IsParallel(gp_Vec(aP1, aP3), Precision::Angular()))
        Standard_ConstructionError::Raise("Circle creation aborted: points lay on one line");

      double x, y, z, x1, y1, z1, x2, y2, z2, dx, dy, dz, dx2, dy2, dz2, dx3, dy3, dz3, aRadius;
      //Calculations for Radius
      x = aP1.X(); y = aP1.Y(); z = aP1.Z();
      x1 = aP2.X(); y1 = aP2.Y(); z1 = aP2.Z();
      dx = x1 - x;
      dy = y1 - y;
      dz = z1 - z;
      aRadius = sqrt(dx*dx + dy*dy + dz*dz);
      //Calculations for Plane Vector
      x2 = aP3.X(); y2 = aP3.Y(); z2 = aP3.Z();
      dx2 = x2 - x; dy2 = y2 - y; dz2 = z2 - z;
      dx3 = ((dy*dz2) - (dy2*dz))/100;
      dy3 = ((dx2*dz) - (dx*dz2))/100;
      dz3 = ((dx*dy2) - (dx2*dy))/100;
      //Make Plane Vector
      gp_Dir aDir ( dx3, dy3, dz3 );
      //Make Circle
      gp_Ax2 anAxes (aP1, aDir);
      gp_Circ aCirc (anAxes, aRadius);
      aShape = BRepBuilderAPI_MakeEdge(aCirc).Edge();
    }
  }
  else if (aType == CIRCLE_THREE_PNT) {
    Handle(GEOM_Function) aRefPoint1 = aCI.GetPoint1();
    Handle(GEOM_Function) aRefPoint2 = aCI.GetPoint2();
    Handle(GEOM_Function) aRefPoint3 = aCI.GetPoint3();
    TopoDS_Shape aShapePnt1 = aRefPoint1->GetValue();
    TopoDS_Shape aShapePnt2 = aRefPoint2->GetValue();
    TopoDS_Shape aShapePnt3 = aRefPoint3->GetValue();
    if (aShapePnt1.ShapeType() == TopAbs_VERTEX &&
        aShapePnt2.ShapeType() == TopAbs_VERTEX &&
        aShapePnt3.ShapeType() == TopAbs_VERTEX) {
      gp_Pnt aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt1));
      gp_Pnt aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt2));
      gp_Pnt aP3 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt3));
      if (aP1.Distance(aP2) < gp::Resolution() ||
          aP1.Distance(aP3) < gp::Resolution() ||
          aP2.Distance(aP3) < gp::Resolution())
        Standard_ConstructionError::Raise("Circle creation aborted: coincident points given");
      if (gp_Vec(aP1, aP2).IsParallel(gp_Vec(aP1, aP3), Precision::Angular()))
        Standard_ConstructionError::Raise("Circle creation aborted: points lay on one line");
      Handle(Geom_Circle) aCirc = GC_MakeCircle(aP1, aP2, aP3).Value();
      aShape = BRepBuilderAPI_MakeEdge(aCirc).Edge();
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_CircleDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICircle aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CIRCLE";

  switch ( aType ) {
  case CIRCLE_PNT_VEC_R:
    AddParam( theParams, "Center Point", aCI.GetCenter(), "Origin" );
    AddParam( theParams, "Vector", aCI.GetVector(), "Z axis" );
    AddParam( theParams, "Radius", aCI.GetRadius() );
    break;
  case CIRCLE_CENTER_TWO_PNT:
    AddParam( theParams, "Center Point", aCI.GetPoint1() );
    AddParam( theParams, "Point 1", aCI.GetPoint2() );
    AddParam( theParams, "Point 2", aCI.GetPoint3() );
    break;
  case CIRCLE_THREE_PNT:
    AddParam( theParams, "Point 1", aCI.GetPoint1() );
    AddParam( theParams, "Point 2", aCI.GetPoint2() );
    AddParam( theParams, "Point 3", aCI.GetPoint3() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_CircleDriver,GEOM_BaseDriver);
