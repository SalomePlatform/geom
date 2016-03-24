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

//  File   : Sketcher_Utils.h
//  Author : Sergey KHROMOV


#ifndef _Sketcher_Utils_HXX_
#define _Sketcher_Utils_HXX_


#include "Sketcher.hxx"

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

#include <list>


class gp_Ax3;


/**
 * This class represents a set of utils needed to compute sketcher geometry.
 */
class SKETCHER_SALOME_EXPORT Sketcher_Utils
{

public:

  /**
   * This method makes a shape from the list of 2D coordinates on the working
   * plane. The result represents a vertex if there is only one point
   * in the contour. If there are more then one points the result is a wire
   * consisting of linear segments between points. It is either closed or not
   * depending on the flag IsClosed. In case of failure the result is a null
   * shape.
   *
   * \param theCoords2D is the list of coordinates in the form x1, y1, x2, y2,
   *        ..., xN, yN for N 2D points.
   * \param IsClosed if Standard_True the first and last points are connected
   *        to form the closed contour.
   * \param thePlane the working plane coordinate system.
   * \return the result polyline.
   */
  static TopoDS_Shape MakePolyline(const std::list <double> &theCoords2D,
                                   const Standard_Boolean    IsClosed,
                                   const gp_Ax3             &thePlane);

  /**
   * This method makes a shape from the list of 2D coordinates on the working
   * plane. The result represents a vertex if there is only one point
   * in the contour. If there are more then one points the result is a wire
   * consisting of a points interpolation BSpline curve. It is either closed
   * or not depending on the flag IsClosed. In case of failure the result is
   * a null shape.
   *
   * \param theCoords2D is the list of coordinates in the form x1, y1, x2, y2,
   *        ..., xN, yN for N 2D points.
   * \param IsClosed if Standard_True the first and last points are connected
   *        to form the closed contour.
   * \param thePlane the working plane coordinate system.
   * \return the result interpolation wire.
   */
  static TopoDS_Shape MakeInterpolation(const std::list <double> &theCoords2D,
                                        const Standard_Boolean    IsClosed,
                                        const gp_Ax3             &thePlane);

  /**
   * This method converts the list of 2D point coordinates into 3D points
   * basing on the working plane. The result list contains not confused points.
   *
   * \param theCoords2D is the list of coordinates in the form x1, y1, x2, y2,
   *        ..., xN, yN for N 2D points.
   * \param thePlane the working plane coordinate system.
   * \param thePoints the list of 3D points.
   */
  static void To3D(const std::list <double> &theCoords2D,
                   const gp_Ax3             &thePlane,
                         std::list <gp_Pnt> &thePoints);

};

#endif
