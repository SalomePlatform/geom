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

#ifndef CURVECREATOR_UTILS_ICURVE_H
#define CURVECREATOR_UTILS_ICURVE_H

#include "CurveCreator_Macro.hxx"

#include "CurveCreator_ICurve.hxx"

#include <gp_Pnt.hxx>

#include <QMap>
#include <QList>

class CurveCreator_UtilsICurve
{
public:

  /*!
   * Returns a point index in the model curve by the point coordinates in the viewer
   * \param theX the X coordinate of the point
   * \param theY the Y coordinate of the point
   */
  CURVECREATOR_EXPORT static int  findLocalPointIndex( const CurveCreator_ICurve* theCurve,
                                          int theSectionId, double theX, double theY );

  CURVECREATOR_EXPORT static void findSectionsToPoints( const CurveCreator_ICurve* theCurve,
                                          const double theX, const double theY,
                                          CurveCreator_ICurve::SectionToPointList& thePoints );
  CURVECREATOR_EXPORT static void convert( const CurveCreator_ICurve::SectionToPointList& thePoints,
                                           QMap<int, QList<int> >& theConvPoints );

  CURVECREATOR_EXPORT static void getPoint( const CurveCreator_ICurve* theCurve, const int theISection,
                                            const int theIPoint, gp_Pnt& thePoint );

  /*!
   * Returns a unique section name
   * \param theCurve a curve interface
   */
  CURVECREATOR_EXPORT static std::string getUniqSectionName(
                                            CurveCreator_ICurve* theCurve );

  /**
   * Returns whethe the container has the value
   * \param theList a container of values
   * \param theValue a value
   */
  CURVECREATOR_EXPORT static bool contains( const CurveCreator_ICurve::SectionToPointList& theList,
                                            const CurveCreator_ICurve::SectionToPoint& theValue );
};

#endif // CURVECREATOR_UTILS_ICURVE_H
