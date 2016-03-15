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

//  File   : Sketcher_Utils.cxx
//  Author : Sergey KHROMOV
//  Module : GEOM


#include "Sketcher_Utils.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <ElSLib.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
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
// function : constructBSpline
// purpose  : See function 'constructBSpline' in file 'CurveCreator_Utils.cxx'.
//=======================================================================
static bool constructBSpline(
  const Handle(TColgp_HArray1OfPnt)& thePoints,
  const Standard_Boolean theIsClosed,
  Handle(Geom_BSplineCurve)& theBSpline)
{
  const int aPointCount = thePoints->Length();
  if (aPointCount <= 1)
  {
    return false;
  }

  // Calculate the tangents.
  TColgp_Array1OfVec aTangents(1, aPointCount);
  Handle(TColStd_HArray1OfBoolean) aTangentFlags =
    new TColStd_HArray1OfBoolean(1, aPointCount);
  GeomAPI_Interpolate aInterpolator(thePoints, theIsClosed, 0);
  if (aPointCount == 2)
  {
    aTangentFlags->SetValue(1, Standard_False);
    aTangentFlags->SetValue(2, Standard_False);
  }
  else
  {
    for (Standard_Integer aPN = 1; aPN <= aPointCount; ++aPN)
    {
      gp_Vec aTangent;
      if (aPN != 1 || theIsClosed)
      {
        const Standard_Integer aPN1 = (aPN != 1) ? (aPN - 1) : aPointCount;
        aTangent = gp_Vec(thePoints->Value(aPN1),
          thePoints->Value(aPN)).Normalized();
      }
      if (aPN < aPointCount || theIsClosed)
      {
        const Standard_Integer aPN2 = (aPN != aPointCount) ? (aPN + 1) : 1;
        const gp_Vec aTangent2 = aTangent +
          gp_Vec(thePoints->Value(aPN), thePoints->Value(aPN2)).Normalized();
        if (aTangent2.SquareMagnitude() >= Precision::SquareConfusion())
        {
          aTangent = aTangent2.Normalized();
        }
        else
        {
          aTangent = -aTangent;
        }
      }
      aTangents.SetValue(aPN, aTangent);
      aTangentFlags->SetValue(aPN, Standard_True);
    }
  }

  // Interpolate.
  aInterpolator.Load(aTangents, aTangentFlags, Standard_False);
  aInterpolator.Perform();
  const bool aResult = (aInterpolator.IsDone() == Standard_True);
  if (aResult)
  {
    theBSpline = aInterpolator.Curve();
  }
  return aResult;
}

//=======================================================================
// function : MakeInterpolation
// purpose  : 
//=======================================================================
TopoDS_Shape Sketcher_Utils::MakeInterpolation(
  const std::list<double>& theCoords2D,
  const Standard_Boolean theIsClosed,
  const gp_Ax3& thePlane)
{
  if (theCoords2D.size() == 0)
  {
    return TopoDS_Shape();
  }

  // Get the different points.
  std::list<gp_Pnt> aTmpPoints;
  To3D(theCoords2D, thePlane, aTmpPoints);
  gp_Pnt aFirstPoint = aTmpPoints.front();
  gp_Pnt aPoint = aFirstPoint;
  std::list<gp_Pnt>::iterator aPIt = aTmpPoints.begin();
  for (++aPIt; aPIt != aTmpPoints.end();)
  {
    const gp_Pnt aPoint2 = *aPIt;
    if (!aPoint.IsEqual(aPoint2, POINT_CONFUSION_TOLERANCE))
    {
      aPoint = aPoint2;
      ++aPIt;
    }
    else
    {
      aTmpPoints.erase(aPIt);
    }
  }
  if (theIsClosed)
  {
    while (--aPIt != aTmpPoints.begin() &&
      aFirstPoint.IsEqual(*aPIt, POINT_CONFUSION_TOLERANCE))
    {
      aTmpPoints.erase(aPIt);
    }
  }

  // Process the single point case.
  const int aPointCount = aTmpPoints.size();
  if (aPointCount == 1)
  {
    return BRepBuilderAPI_MakeVertex(aTmpPoints.front());
  }

  // Process the other cases.
  Handle(TColgp_HArray1OfPnt) aPoints =
    new TColgp_HArray1OfPnt(1, aPointCount);
  aPIt = aTmpPoints.begin();
  for (Standard_Integer aPN = 1; aPIt != aTmpPoints.end(); ++aPIt, ++aPN)
  {
    aPoints->SetValue(aPN, *aPIt);
  }
  Handle(Geom_BSplineCurve) aBSpline;
  if (constructBSpline(aPoints, theIsClosed, aBSpline))
  {
    return BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aBSpline));
  }
  return TopoDS_Shape();
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
