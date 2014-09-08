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

//  File   : Sketcher_Utils.cxx
//  Author : Sergey KHROMOV
//  Module : GEOM


#include "Sketcher_Utils.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <ElSLib.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopoDS_Wire.hxx>

const double POINT_CONFUSION_TOLERANCE = 0.0001;


//=======================================================================
// function : MakePolyline
// purpose  : 
//=======================================================================
TopoDS_Shape Sketcher_Utils::MakePolyline
                              (const std::list <double> &theCoords2D,
                               const Standard_Boolean    IsClosed,
                               const gp_Ax3             &thePlane)
{
  std::list <gp_Pnt> aPoints;
  TopoDS_Shape       aResult;

  To3D(theCoords2D, thePlane, aPoints);

  Standard_Integer aNbPnts = aPoints.size();

  if (aNbPnts > 1) {
    if (IsClosed &&
        aPoints.front().IsEqual(aPoints.back(), POINT_CONFUSION_TOLERANCE)) {
      // The polyline should be closed, first and last points are confused.
      // Remove the last point.
      aPoints.pop_back();
      --aNbPnts;
    }
  }

  if (aNbPnts == 1) {
    // The result is vertex.
    aResult = BRepBuilderAPI_MakeVertex(aPoints.front()).Vertex();
  } else if (aNbPnts > 1) {
    // There are several points. Make a polyline.
    std::list <gp_Pnt>::const_iterator anIter    = aPoints.begin();
    TopoDS_Vertex                      aVtxFirst =
                       BRepBuilderAPI_MakeVertex(*anIter).Vertex();
    TopoDS_Vertex                      aVtx[2];
    TopoDS_Edge                        aSegment;
    BRepBuilderAPI_MakeWire            aMkWire;

    aVtx[0] = aVtxFirst;

    for (++anIter; anIter != aPoints.end(); ++anIter) {
      aVtx[1]  = BRepBuilderAPI_MakeVertex(*anIter).Vertex();
      aSegment = BRepBuilderAPI_MakeEdge(aVtx[0], aVtx[1]).Edge();
      aMkWire.Add(aSegment);
      aVtx[0]  = aVtx[1];
    }

    if (IsClosed) {
      // Create a closing segment.
      aSegment = BRepBuilderAPI_MakeEdge(aVtx[0], aVtxFirst).Edge();
      aMkWire.Add(aSegment);
    }

    aResult = aMkWire.Wire();
  }

  return aResult;
}

//=======================================================================
// function : MakeInterpolation
// purpose  : 
//=======================================================================
TopoDS_Shape Sketcher_Utils::MakeInterpolation
                              (const std::list <double> &theCoords2D,
                               const Standard_Boolean    IsClosed,
                               const gp_Ax3             &thePlane)
{
  std::list <gp_Pnt> aPoints;
  TopoDS_Shape       aResult;

  To3D(theCoords2D, thePlane, aPoints);

  Standard_Integer aNbPnts = aPoints.size();

  if (aNbPnts > 1) {
    if (IsClosed &&
        aPoints.front().IsEqual(aPoints.back(), POINT_CONFUSION_TOLERANCE)) {
      // The polyline should be closed, first and last points are confused.
      // Remove the last point.
      aPoints.pop_back();
      --aNbPnts;
    }
  }

  if (aNbPnts == 1) {
    // The result is vertex.
    aResult = BRepBuilderAPI_MakeVertex(aPoints.front()).Vertex();
  } else if (aNbPnts > 1) {
    std::list <gp_Pnt>::const_iterator anIter        = aPoints.begin();
    Handle(TColgp_HArray1OfPnt)        aHCurvePoints =
      new TColgp_HArray1OfPnt(1, aNbPnts);
    Standard_Integer                   i;

    for (i = 1; anIter != aPoints.end(); ++anIter, ++i) {
      aHCurvePoints->SetValue(i, *anIter);
    }

    // Compute BSpline
    Standard_Real       aTol = Precision::Confusion();
    GeomAPI_Interpolate aGBC(aHCurvePoints, IsClosed, aTol);

    aGBC.Perform();

    if (aGBC.IsDone()) {
      TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aGBC.Curve()).Edge();
      aResult = BRepBuilderAPI_MakeWire(anEdge).Wire();
    }
  }

  return aResult;
}

//=======================================================================
// function : To3D
// purpose  : 
//=======================================================================
void Sketcher_Utils::To3D(const std::list <double> &theCoords2D,
                          const gp_Ax3             &thePlane,
                                std::list <gp_Pnt> &thePoints)
{
  thePoints.clear();

  if (theCoords2D.empty() || theCoords2D.size() % 2 == 1) {
    // Odd number of coordinates or empty list. Invalid case.
    return;
  }

  std::list <double>::const_iterator anIter = theCoords2D.begin();
  Standard_Real    aX     = *anIter;
  Standard_Real    aY     = *(++anIter);
  gp_Pnt           aPLast = ElSLib::PlaneValue (aX, aY, thePlane);
  gp_Pnt           aPnt;

  thePoints.push_back(aPLast);

  for (++anIter; anIter != theCoords2D.end(); ++anIter) {
    aX   = *anIter;
    aY   = *(++anIter);
    aPnt =  ElSLib::PlaneValue (aX, aY, thePlane);

    if (!aPLast.IsEqual(aPnt, POINT_CONFUSION_TOLERANCE)) {
      thePoints.push_back(aPnt);
      aPLast = aPnt;
    }
  }
}
