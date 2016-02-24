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

// Below macro specifies how the closed point set is processed (issue 0022885).
// See below for more information.
// Currently solution 4 is chosen!
#define BSPLINE_PROCESS_CLOSED_PNTSET 2

namespace
{
  /*!
    \brief Generate list of points from the list of (x,y,z) coordinates
    \param coords list of values specifying (x y z) coordinates of points
    \return list of points
    \internal
  */
  Handle(TColgp_HArray1OfPnt) pointsFromCoords(Handle(TColStd_HArray1OfReal) coords)
  {
    Standard_Integer length = coords->Length() / 3;

    Handle(TColgp_HArray1OfPnt) points = new TColgp_HArray1OfPnt(1, length);
    
    for (int i = 0; i < length; i ++) {
      Standard_Real x = coords->Value( i*3+1 );
      Standard_Real y = coords->Value( i*3+2 );
      Standard_Real z = coords->Value( i*3+3 );
      points->SetValue(i+1, gp_Pnt(x, y, z));
    }

    return points;
  }

  /*!
    \brief Generate list of points from the sequence of input objects
    \param coords list of objects as it is stored within the CAF tree
    \return list of points
    \internal
  */
  Handle(TColgp_HArray1OfPnt) pointsFromObjs(Handle(TColStd_HSequenceOfTransient) objects)
  {
    Standard_Integer length = objects->Length();

    Handle(TColgp_HArray1OfPnt) points = new TColgp_HArray1OfPnt(1, length);
    
    for (int i = 1; i <= length; i ++) {
      TopoDS_Shape shape = Handle(GEOM_Function)::DownCast(objects->Value(i))->GetValue();
      if (shape.ShapeType() != TopAbs_VERTEX)
        // error: only vertices are allowed in the input
        Standard_ConstructionError::Raise("Input should contain only vertices");
      points->SetValue(i, BRep_Tool::Pnt(TopoDS::Vertex(shape)));
    }

    return points;
  }
}

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
Standard_Integer GEOMImpl_SplineDriver::Execute(LOGBOOK& log) const
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

    // collect points from input parameters: objects or coordinates
    Handle(TColgp_HArray1OfPnt) points = useCoords ? pointsFromCoords(aCI.GetCoordinates()) : pointsFromObjs(aCI.GetPoints());
    int length = points->Length();

    if (length < 2) return 0; // error: not enough points in the input list

    // reorder points if required (bspline only)
    if ((aType == SPLINE_INTERPOLATION || aType == SPLINE_INTERPOL_TANGENTS) && aCI.GetDoReordering()) {
      int nbDup = 0;
      gp_Pnt pPrev = points->Value(1);
      for (int i = 1; i < length - 1; i++) {
        gp_Pnt pi = points->Value(i);
        int nearest = 0;
        double minDist = RealLast();
        for (int j = i+1; j <= length; j++) {
          double dist = pi.SquareDistance(points->Value(j));
          if (dist < minDist && (minDist - dist) > Precision::Confusion()) {
            nearest = j;
            minDist = dist;
          }
        }
        if (nearest > 0 && nearest != i + 1) {
          // Keep given order of points to use it in case of equidistant candidates
          //               .-<---<-.
          //              /         \
          // o  o  o  c  o->o->o->o->n  o  o
          //          |  |           |
          //          i i+1       nearest
          gp_Pnt p = points->Value(nearest);
          for (int j = nearest; j > i+1; j--)
            points->SetValue(j, points->Value(j-1));
          points->SetValue(i+1, p);
        }
        if ( pPrev.Distance(points->Value(i+1)) <= Precision::Confusion() )
          nbDup++;
        else
          pPrev = points->Value(i+1);
      }
      if ( nbDup > 0 ) {
        Handle(TColgp_HArray1OfPnt) tmpPoints = new TColgp_HArray1OfPnt(1, length-nbDup);
        int j = 1;
        for (int i = 1; i <= length; i++) {
          if (i == 1 || pPrev.Distance(points->Value(i)) > Precision::Confusion() ) {
            tmpPoints->SetValue(j++, points->Value(i));
            pPrev = points->Value(i);
          }
        }
        points = tmpPoints;
        length = points->Length();
      }
    } // end of reordering

    bool closed = points->Value(1).Distance(points->Value(length)) <= gp::Resolution();

    if (aType == SPLINE_BEZIER) {
      // for Bezier curve we should append first point to the list if:
      // a) "closed" flag is set, and
      // b) first and last vertices are not too close
      bool addFirst = aCI.GetIsClosed() && !closed;
      
      // re-fill points and check that there's enough points to create a curve
      bool isValid = false;
      TColgp_Array1OfPnt curvePoints(1, length + (addFirst ? 1 : 0));
      gp_Pnt pp;
      for (int i = 1; i <= length; i++) {
        gp_Pnt p = points->Value(i);
        if (!isValid && i > 1 && p.Distance(pp) > Precision::Confusion())
          isValid = true;
        curvePoints.SetValue(i, p);
        pp = p;
      }

      if (!isValid)
        // error: not enough points to create curve
        Standard_ConstructionError::Raise("Points for Bezier Curve are too close");
      
      // set last point equal to first for the closed Bezier curve
      if (addFirst) curvePoints.SetValue(length+1, curvePoints.Value(1));
      
      // create Bezier curve
      Handle(Geom_BezierCurve) GBC = new Geom_BezierCurve(curvePoints);
      aShape = BRepBuilderAPI_MakeEdge(GBC).Edge();
    }
    else {
      // Below described processing of closed points set case
      // is not done for constrained bsplined
      bool typeok = aType == SPLINE_INTERPOLATION;
#if BSPLINE_PROCESS_CLOSED_PNTSET == 1
      // Last point is removed from the list if:
      // a) first and last vertices are equal;
      // b) "closed" flag is not taken into account.
      // If first and last points are equal, we force "closed" flag to be set to true.
      // For the case when first and last vertices are equal, this approach causes
      // result different that would be if last point had NOT be removed and "closed" flag is false.
      bool isClosed = typeok && (aCI.GetIsClosed() || closed);
      bool removeLast = typeok && closed;
      bool addFirst = false;
#elif BSPLINE_PROCESS_CLOSED_PNTSET == 2
      // Last point is removed from the list if:
      // a) first and last vertices are equal;
      // b) "closed" flag is set to true.
      // Flag "closed" is taken "as is".
      // For the case when first and last vertices are equal, this approach causes
      // different results with "closed" flag set to true and false.
      bool isClosed = typeok && aCI.GetIsClosed();
      bool removeLast = typeok && aCI.GetIsClosed() && closed;
      bool addFirst = false;
#elif BSPLINE_PROCESS_CLOSED_PNTSET == 3
      // Points are passed "as is" to the creator.
      // If first and last points are equal, we force "closed" flag to be set to false.
      // For the case when first and last vertices are equal, this approach gives
      // the same results with "closed" flag set to true and false.
      bool isClosed = typeok && aCI.GetIsClosed() && !closed;
      bool removeLast = false;
      bool addFirst = false;
#elif BSPLINE_PROCESS_CLOSED_PNTSET == 4
      // First point is added to the list if:
      // a) first and last vertices are not equal;
      // b) "closed" flag is set to true.
      // In this case "closed" flag is forcidly set to false - bspline creator is 
      // capable to create closed edge in this case.
      // This approach gives the same results with "closed" flag set to true not
      // depending on if set of points is closed or no.
      // Also, it gives equal reqults in both case if set of points is closed or not
      // and "closed" flag is set to true, in contrast to solution 3 above.
      bool isClosed = false;
      bool removeLast = false;
      bool addFirst = typeok && aCI.GetIsClosed() && !closed;
#else
      // Points are passed "as is" to the creator.
      // This causes an error when first point is equal to last one and
      // "closed" flag is set to true; see bug 0022885.
      bool isClosed = typeok && aCI.GetIsClosed();
      bool removeLast = false;
      bool addFirst = false;
#endif

      // remove last point or append first one if the conditions are observed (see above)
      if (removeLast || addFirst) {
        int extra = removeLast ? -1 : (addFirst ? 1 : 0 );
        int nb = removeLast ? length-1 : length;
        Handle(TColgp_HArray1OfPnt) curvePoints = new TColgp_HArray1OfPnt (1, length+extra);
        for (int i = 1; i <= nb; i++)
          curvePoints->SetValue(i, points->Value(i));
        if (addFirst) curvePoints->SetValue(length+1, points->Value(1));
        points = curvePoints;
      }

      // initial set-up of curve creator
      GeomAPI_Interpolate GBC(points, isClosed, gp::Resolution());

      // add tangent vectors constraints
      if (aType == SPLINE_INTERPOL_TANGENTS) {
        Handle(GEOM_Function) aVec1Ref = aCI.GetFirstVector();
        Handle(GEOM_Function) aVec2Ref = aCI.GetLastVector();

        if (aVec1Ref.IsNull() || aVec2Ref.IsNull())
          // error: bad vector parameter is specified
          Standard_NullObject::Raise("Null object is given for a vector");

        // take orientation of edge into account to avoid regressions, as it was implemented so
        gp_Vec aV1 = GEOMUtils::GetVector(aVec1Ref->GetValue(), Standard_True);
        gp_Vec aV2 = GEOMUtils::GetVector(aVec2Ref->GetValue(), Standard_True);

        // push constraint vectors to the curve creator
        GBC.Load(aV1, aV2, /*Scale*/Standard_True);
      }

      // create bspline curve
      GBC.Perform();
      if (GBC.IsDone())
        aShape = BRepBuilderAPI_MakeEdge(GBC.Curve()).Edge();
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0; // error: bad result

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

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_SplineDriver,GEOM_BaseDriver);
