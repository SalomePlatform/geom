// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef CURVECREATOR_UTILS_H
#define CURVECREATOR_UTILS_H

#include "CurveCreator_Macro.hxx"
#include "CurveCreator_ICurve.hxx"

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx> // TODO: remove
#include <V3d_View.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Curve.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>

#include <list>
#include <vector> // TODO: remove

class CurveCreator_Curve;


class CurveCreator_Utils
{
public:

    /*!
   * \brief Returns the point clicked in 3D view.
   *
   * \param x The X coordinate in the view.
   * \param y The Y coordinate in the view.
   * \param theView View where the given point takes place.
   * \retval gp_Pnt Returns the point clicked in 3D view
   */
  CURVECREATOR_EXPORT static void ConvertPointToClick( const gp_Pnt& thePoint,
                                                       Handle(V3d_View) theView,
                                                       int& x, int& y );

  /*!
   * \brief Returns the point clicked in 3D view.
   *
   * \param x The X coordinate in the view.
   * \param y The Y coordinate in the view.
   * \param theView View where the given point takes place.
   * \retval gp_Pnt Returns the point clicked in 3D view
   */
  CURVECREATOR_EXPORT static gp_Pnt ConvertClickToPoint( int x, int y,
                                                         Handle(V3d_View) theView );

  /**
   * Generates shape on the curve
   * \param theCurve a curve object, that contains data
   * \param theShape a generated shape
   */
  CURVECREATOR_EXPORT static void constructShape( const CurveCreator_ICurve* theCurve,
                                                  TopoDS_Shape& theShape );

  /**
   * Generates a curve from a shape.
   * \param theShape a shape to be converted to curve.
   * \param theCurve a curve object to be initialized.
   * \param theLocalCS the local coordinate system of the curve.
   * \return true in case of success; false otherwise. Warning: the curve can
   *         be modified even if the shape is not valid for curve construction.
   */
  CURVECREATOR_EXPORT static bool constructCurve
                      (const TopoDS_Shape        theShape,
                             CurveCreator_Curve *theCurve,
                             gp_Ax3             &theLocalCS);

  /**
   * Find selected points in the context
   * \param theContext the viewer context
   * \param theCurve a curve object, that contains data
   */
  CURVECREATOR_EXPORT static void getSelectedPoints( Handle(AIS_InteractiveContext) theContext,
                                         const CurveCreator_ICurve* theCurve,
                                         CurveCreator_ICurve::SectionToPointList& thePoints );

  /**
   * Set selected points to the context
   * \param theContext the viewer context
   * \param theCurve a curve object, that contains data
   * \param thePoints the curve point indices to be selected in the context
   */
  CURVECREATOR_EXPORT static void setSelectedPoints(
                                         Handle(AIS_InteractiveContext) theContext,
                                         const CurveCreator_ICurve* theCurve,
                                         const CurveCreator_ICurve::SectionToPointList& thePoints =
                                               CurveCreator_ICurve::SectionToPointList() );

  /*!
   * \brief Sets the local point context for the 3D viewer.
   * \param theCurve a curve object, that contains data
   * \param theContext the viewer context
   * \param theOpen The flag to open or close the local context.
   */
  CURVECREATOR_EXPORT static void setLocalPointContext(
                                        const CurveCreator_ICurve* theCurve,
                                        Handle(AIS_InteractiveContext) theContext,
                                        const bool theOpen );

  /**
   * Checks whether the point belongs to the OCC object
   * \param theObject a line or shape with a bspline inside
   * \param theX the X coordinate in the view.
   * \param theY the Y coordinate in the view.
   * \param thePoint the output point to be append to the model curve
   * \param thePoint1 the output point to bound the line where a new point should be inserted
   * \param thePoint2 the output point to bound the line where a new point should be inserted
   */
  CURVECREATOR_EXPORT static bool pointOnObject( Handle(V3d_View) theView,
                                                 Handle(AIS_InteractiveObject) theObject,
                                                 const int theX, const int theY,
                                                 gp_Pnt& thePoint, gp_Pnt& thePoint1,
                                                 gp_Pnt& thePoint2 );

  /**
   * The algorithm builds the cubic B-spline passing through the points that the
   * tangent vector in each given point P is calculated by the following way:
   * if point P is preceded by a point A and is followed by a point B then
   * the tangent vector is equal to (P - A) / |P - A| + (B - P) / |B - P|;
   * if point P is preceded by a point A but is not followed by any point then
   * the tangent vector is equal to P - A;
   * if point P is followed by a point B but is not preceded by any point then
   * the tangent vector is equal to B - P.
   */
  CURVECREATOR_EXPORT static bool constructBSpline( const Handle(TColgp_HArray1OfPnt)& thePoints,
                                                    const Standard_Boolean theIsClosed,
                                                    Handle(Geom_BSplineCurve)& theBSpline );

  /**
   * Constructs the wire corresponding to the section.
   */
  CURVECREATOR_EXPORT static TopoDS_Wire ConstructWire(
    Handle(TColgp_HArray1OfPnt) thePoints,
    const bool theIsPolyline,
    const bool theIsClosed);

protected:
  /*
   * Returns whether the clicked point belong to the curve or has a very near projection
   * \param theX the X coordinate of a point clicked in the OCC viewer
   * \param theY the Y coordinate of a point clicked in the OCC viewer
   * \param theCurve a geometry curve
   * \param theOutPoint a found projected point on the curve
  */
  static bool hasProjectPointOnCurve(
                             Handle(V3d_View) theView,
                             const int theX, const int theY,
                             const Handle(Geom_Curve)& theCurve,
                             Standard_Real& theParameter,
                             int& theDelta );

  /*
   * Returns whether the X and Y coordinates is in the pixel tolerance
   * \param theX the X coordinate of the first point
   * \param theY the Y coordinate of the first point
   * \param theOtherX the X coordinate of the second point
   * \param theOtherY the Y coordinate of the second point
   * \param theTolerance the tolerance to compare
   * \param theDelta the sum of the a square of X and a square of Y
   * \returns whether the points are provide to the pixel tolerance
  */
  static bool isEqualPixels( const int theX, const int theY,
                             const int theOtherX, const int theOtherY,
                             const double theTolerance, int& theDelta );


  /*
   * Returns whether the points are the same
   * \param thePoint the first point
   * \param theOtherPoint the second point
   * \returns whether the points are provide to the pixel tolerance
  */
  static bool isEqualPoints( const gp_Pnt& thePoint,
                             const gp_Pnt& theOtherPoint );

  /**
   * Returns the array of points of a shape to construct a curve section. The
   * shape can be either a wire or a vertex. For vertex a single point in the
   * array is returned.
   *
   * \param theShape the shape. Can be either a wire or a vertex.
   * \param IsClosed closed flag. Output parameter.
   * \param IsBSpline BSpline flag. Output parameter.
   * \return the array of points. Null handle in case of failure.
   */
  static Handle(TColgp_HArray1OfPnt) getPoints
                           (const TopoDS_Shape &theShape,
                                  bool         &IsClosed,
                                  bool         &IsBSpline);

  /**
   * This method computes a plane using the input points. The plane is defined
   * by gp_Pln object and the status. The status can have one of the following
   * values:
   *   - 0 plane is not set.<BR>
   *   - 1 origin of the plane is fixed. The plane is defined by 1 or several
   *       coincident points.<BR>
   *   - 2 origin + OX axis of the plane is fixed. The plane is defined by 2
   *       or more points that lie on a particular line.<BR>
   *   - 3 plane is fixed. Plane is defined by 3 not coincident points.<BR>
   *
   * \param thePoints the points.
   * \param thePlane the current plane on input. It can be modified on output.
   * \param thePlnStatus the current status on input. It can be modified on
   *        output.
   */
  static void FindPlane(const Handle(TColgp_HArray1OfPnt) &thePoints,
                              gp_Pln                     &thePlane,
                              Standard_Integer           &thePlnStatus);

};

#endif // CURVECREATOR_UTILS_H
