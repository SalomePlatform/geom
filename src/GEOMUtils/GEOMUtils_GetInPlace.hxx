// Copyright (C) 2007-2026  CEA, EDF, OPEN CASCADE
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
// File:        GEOMUtils_GetInPlace.hxx
// Created:
// Author:      Sergey KHROMOV

#ifndef _GEOMUtils_GetInPlace_HeaderFile
#define _GEOMUtils_GetInPlace_HeaderFile

#include <GEOM_Function.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <gp_Vec.hxx>

#include <vector>

class BRepExtrema_DistShapeShape;
class TopoDS_Face;
class TopoDS_Shape;

/**
 * This is an API class for two GetInPlace algorithms:
 * 1. Old GetInPlace algorithm (in contra to new GEOMAlgo_GetInPlace),
 * 2. GetInPlaceByHistory.
 */
class GEOMUtils_GetInPlace
{

public:

  /*!
   *  \brief Old implementation of GetInPlace algorithm.
   *  This method searches among sub shapes of the shape theWhere parts that are
   *  coincident with the shape theWhat. The result list of shape is returned as
   *  an output parameter. It returns the error code with the following possible
   *  values:
   *    0 - Success;
   *    1 - theWhere and/or theWhat TopoDS_Shape are Null;
   *    2 - An attempt to extract a shape of not supported type;
   *    3 - Not found any Results.
   */
  Standard_EXPORT static Standard_Integer GetInPlaceOld
            (const TopoDS_Shape         &theWhere,
             const TopoDS_Shape         &theWhat,
                   TopTools_ListOfShape &theShapesInPlace);

  /**
   *  \brief IsShapeHasHistory method implementation.
   *  Checks whether the given shape has a modification history 
   *  recorded in the context of the specified GEOM function.
   */
   Standard_EXPORT static Standard_Boolean IsShapeHasHistory
            (const Handle(GEOM_Function) &theWhereFunction,
             const TopoDS_Shape &theWhat);

  /**
   *  \brief GetInPlaceByHistory method implementation.
   *  Returns Standard_True if something is found. Warning: theShapesInPlace
   *  list is not cleared at first.
   */
  Standard_EXPORT static Standard_Boolean GetInPlaceByHistory
                      (const Handle(GEOM_Function)      &theWhereFunction,
                       const TopTools_IndexedMapOfShape &theWhereIndices,
                       const TopoDS_Shape               &theWhat,
                             TopTools_ListOfShape       &theShapesInPlace);

  /**
   *  \brief GetInPlaceMap method implementation.
   *  For each sub-shape ID in theWhat fills an array of corresponding
   *  sub-shape IDs in theWhere
   */
  Standard_EXPORT static Standard_Boolean GetInPlaceMap
                      (const Handle(GEOM_Function)       &theWhereFunction,
                       const TopoDS_Shape                &theWhat,
                       std::vector< std::vector< int > > &theResVec);

protected:

  /*!
   * \brief Return normal to face at extrema point
   */
  static gp_Vec GetNormal(const TopoDS_Face                &theFace,
                          const BRepExtrema_DistShapeShape &theExtrema);

  /*!
   * Return the global properties of the shape: center of mass and
   * a size (length, area or volume depending on the shape type).
   */
  static void GetShapeProperties(const TopoDS_Shape  &theShape,
                                       Standard_Real  theTab[],
                                       gp_Pnt        &theVertex);

};



#endif
