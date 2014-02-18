// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOMImpl_SplineDriver.hxx"

#include "GEOMImpl_ISpline.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOMImpl_ICurveParametric.hxx"

#include "GEOM_Function.hxx"
#include "GEOMUtils.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <Geom_BezierCurve.hxx>
#include <GeomAPI_Interpolate.hxx>

#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <Standard_NullObject.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_SplineDriver::GetID()
{
  static Standard_GUID aSplineDriver("FF1BBB33-5D14-4df2-980B-3A668264EA16");
  return aSplineDriver;
}


//=======================================================================
//function : GEOMImpl_SplineDriver
//purpose  :
//=======================================================================
GEOMImpl_SplineDriver::GEOMImpl_SplineDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_SplineDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISpline aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == SPLINE_BEZIER ||
      aType == SPLINE_INTERPOLATION ||
      aType == SPLINE_INTERPOL_TANGENTS) {

    bool useCoords = aCI.GetConstructorType() == COORD_CONSTRUCTOR;

    Handle(TColStd_HArray1OfReal) aCoordsArray; // parametric case
    Handle(TColStd_HSequenceOfTransient) aPoints; // points case

    int aLen = 0;
    if (useCoords) {
      aCoordsArray = aCI.GetCoordinates();
      aLen = aCoordsArray->Length() / 3;
    }
    else {
      aPoints = aCI.GetPoints();
      aLen = aPoints->Length();
    }

    if (aLen < 2) return 0;

    TColgp_Array1OfPnt points (1, (useCoords ? aLen : 1));
    if (useCoords) {
      int anArrayLength = aCoordsArray->Length();
      for (int i = 0, j = 1; i <= (anArrayLength-3); i += 3) {
        gp_Pnt aPnt = gp_Pnt(aCoordsArray->Value(i+1), aCoordsArray->Value(i+2), aCoordsArray->Value(i+3));
        points.SetValue(j, aPnt);
        j++;
      }
    }

    int aRealLen = aLen;

    if (aType == SPLINE_BEZIER && aCI.GetIsClosed()) {
      TopoDS_Vertex aV1;
      if (useCoords) {
        aV1 = BRepBuilderAPI_MakeVertex(points.Value(1));
      }
      else {
        Handle(GEOM_Function) aFPoint = Handle(GEOM_Function)::DownCast(aPoints->Value(1));
        TopoDS_Shape aFirstPnt = aFPoint->GetValue();
        aV1 = TopoDS::Vertex(aFirstPnt);
      }

      TopoDS_Vertex aV2;
      if (useCoords) {
        aV2 = BRepBuilderAPI_MakeVertex(points.Value(aLen));
      }
      else {
        Handle(GEOM_Function) aLPoint = Handle(GEOM_Function)::DownCast(aPoints->Value(aLen));
        TopoDS_Shape aLastPnt = aLPoint->GetValue();
        aV2 = TopoDS::Vertex(aLastPnt);
      }

      if (!aV1.IsNull() && !aV2.IsNull() && !aV1.IsSame(aV2)) {
        aRealLen++;
      }
    }

    int ind;
    Standard_Boolean isSeveral = Standard_False;
    gp_Pnt aPrevP;

    TColgp_Array1OfPnt CurvePoints (1, aRealLen);
    for (ind = 1; ind <= aLen; ind++) {
      gp_Pnt aP;
      if (useCoords) {
        aP = points.Value(ind);
        if (!isSeveral && ind > 1) {
          if (aP.Distance(aPrevP) > Precision::Confusion()) {
            isSeveral = Standard_True;
          }
        }
        CurvePoints.SetValue(ind, aP);
        aPrevP = aP;
      }
      else {
        Handle(GEOM_Function) aRefPoint = Handle(GEOM_Function)::DownCast(aPoints->Value(ind));
        TopoDS_Shape aShapePnt = aRefPoint->GetValue();
        if (aShapePnt.ShapeType() == TopAbs_VERTEX) {
          aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
          if (!isSeveral && ind > 1) {
            if (aP.Distance(aPrevP) > Precision::Confusion()) {
              isSeveral = Standard_True;
            }
          }
          CurvePoints.SetValue(ind, aP);
          aPrevP = aP;
        }
      }
    }

    if (aType == SPLINE_BEZIER) {
      if (!isSeveral) {
        Standard_ConstructionError::Raise("Points for Bezier Curve are too close");
      }
      if (aRealLen > aLen) { // set last point equal to first for the closed curve
        CurvePoints.SetValue(aRealLen, CurvePoints.Value(1));
      }
      Handle(Geom_BezierCurve) GBC = new Geom_BezierCurve (CurvePoints);
      aShape = BRepBuilderAPI_MakeEdge(GBC).Edge();
    }
    else {
      //GeomAPI_PointsToBSpline GBC (CurvePoints);
      //aShape = BRepBuilderAPI_MakeEdge(GBC).Edge();

      if (aCI.GetDoReordering()) {
        for (int curInd = 1; curInd < aLen - 1; curInd++) {
          gp_Pnt curPnt = CurvePoints.Value(curInd);
          int nearInd = 0;
          double nearDist = RealLast();
          for (ind = curInd + 1; ind <= aLen; ind++) {
            double dist = curPnt.SquareDistance(CurvePoints.Value(ind));
            if (dist < nearDist && (nearDist - dist) > Precision::Confusion()) {
              nearInd = ind;
              nearDist = dist;
            }
          }
          if (nearInd > 0 && nearInd != curInd + 1) {
            // Keep given order of points to use it in case of equidistant candidates
            //               .-<---<-.
            //              /         \
            // o  o  o  c  o->o->o->o->n  o  o
            //          |  |           |
            //     curInd  curInd+1    nearInd
            gp_Pnt nearPnt = CurvePoints.Value(nearInd);
            for (ind = nearInd; ind > curInd + 1; ind--) {
              CurvePoints.SetValue(ind, CurvePoints.Value(ind - 1));
            }
            CurvePoints.SetValue(curInd + 1, nearPnt);
          }
        }
      }

      Handle(TColgp_HArray1OfPnt) aHCurvePoints = new TColgp_HArray1OfPnt (1, aLen);
      for (ind = 1; ind <= aLen; ind++) {
        aHCurvePoints->SetValue(ind, CurvePoints.Value(ind));
      }

      bool isClosed = Standard_False;
      if (aType == SPLINE_INTERPOLATION)
        isClosed = aCI.GetIsClosed();

      GeomAPI_Interpolate GBC (aHCurvePoints, isClosed, gp::Resolution());

      if (aType == SPLINE_INTERPOL_TANGENTS) {
        Handle(GEOM_Function) aVec1Ref  = aCI.GetFirstVector();
        Handle(GEOM_Function) aVec2Ref  = aCI.GetLastVector();

        if (aVec1Ref.IsNull() || aVec2Ref.IsNull())
          Standard_NullObject::Raise("Null object is given for a vector");

        TopoDS_Shape aVec1Sh = aVec1Ref->GetValue();
        TopoDS_Shape aVec2Sh = aVec2Ref->GetValue();

        // take orientation of edge into account to avoid regressions, as it was implemented so
        gp_Vec aV1 = GEOMUtils::GetVector(aVec1Sh, Standard_True);
        gp_Vec aV2 = GEOMUtils::GetVector(aVec2Sh, Standard_True);

        GBC.Load(aV1, aV2, /*Scale*/Standard_True);
      }

      GBC.Perform();
      if (GBC.IsDone())
        aShape = BRepBuilderAPI_MakeEdge(GBC.Curve()).Edge();
      else
        return 0;
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_SplineDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISpline          aCI( function );
  GEOMImpl_ICurveParametric aPI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "CURVE";

  switch ( aType ) {
  case SPLINE_BEZIER:
  case SPLINE_INTERPOLATION:
  case SPLINE_INTERPOL_TANGENTS:

    AddParam( theParams, "Type", ( aType == SPLINE_BEZIER ? "Bezier" : "Interpolation"));

    if ( aPI.HasData() )
    {
      AddParam( theParams, "X(t) equation", aPI.GetExprX() );
      AddParam( theParams, "Y(t) equation", aPI.GetExprY() );
      AddParam( theParams, "Z(t) equation", aPI.GetExprZ() );
      AddParam( theParams, "Min t", aPI.GetParamMin() );
      AddParam( theParams, "Max t", aPI.GetParamMax() );
      if ( aPI.GetParamNbStep() )
        AddParam( theParams, "Number of steps", aPI.GetParamNbStep() );
      else
        AddParam( theParams, "t step", aPI.GetParamStep() );
    }
    else
    {
      if ( aCI.GetConstructorType() == COORD_CONSTRUCTOR )
      {
        Handle(TColStd_HArray1OfReal) coords = aCI.GetCoordinates();
        GEOM_Param& pntParam = AddParam( theParams, "Points");
        pntParam << ( coords->Length() ) / 3 << " points: ";
        for ( int i = coords->Lower(), nb = coords->Upper(); i <= nb; )
          pntParam << "( " << coords->Value( i++ )
                   << ", " << coords->Value( i++ )
                   << ", " << coords->Value( i++ ) << " ) ";
      }
      else
      {
        AddParam( theParams, "Points", aCI.GetPoints() );
      }
      Handle(GEOM_Function) v1 = aCI.GetFirstVector();
      Handle(GEOM_Function) v2 = aCI.GetLastVector();
      if ( !v1.IsNull() ) AddParam( theParams, "First tangent vector", v1 );
      if ( !v2.IsNull() ) AddParam( theParams, "Last tangent vector", v2 );
    }
    break;
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_SplineDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_SplineDriver,GEOM_BaseDriver);
