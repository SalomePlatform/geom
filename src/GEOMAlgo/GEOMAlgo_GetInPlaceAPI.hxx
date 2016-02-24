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
// File:        GEOMAlgo_GetInPlaceAPI.hxx
// Created:
// Author:      Sergey KHROMOV

#ifndef _GEOMAlgo_GetInPlaceAPI_HeaderFile
#define _GEOMAlgo_GetInPlaceAPI_HeaderFile

#include <GEOM_Function.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <gp_Vec.hxx>

class GEOMAlgo_GetInPlace;
class BRepExtrema_DistShapeShape;
class TopoDS_Face;
class TopoDS_Shape;

/**
 * This is an API class for all GetInPlace algorithm.
 * It facilitates using different GetInPlace algorithms:
 * a new one(GEOMAlgo_GetInPlace), an old one and
 * GetInPlaceByHistory.
 */
class GEOMAlgo_GetInPlaceAPI
{

public:

  /**
   *  \brief New GetInPlace method implementation.
   *  Initializes the GEOMAlgo_GetInPlace object with correct parameters and
   *  performs computation (calls theGIP's method Perform. Returns
   *  Standard_True in face of success; Standard_False otherwise.
   */
  Standard_EXPORT static Standard_Boolean GetInPlace
                      (const TopoDS_Shape        &theWhere,
                       const TopoDS_Shape        &theWhat,
                             GEOMAlgo_GetInPlace &theGIP);

  /*!
   *  \brief Old implementation of GetInPlace algoritm.
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
   *  \brief GetInPlaceByHistory method implementation.
   *  Returns Standard_True if something is found. Warning: theShapesInPlace
   *  list is not cleared at first.
   */
  Standard_EXPORT static Standard_Boolean GetInPlaceByHistory
                      (const Handle(GEOM_Function)       &theWhereFunction,
                       const TopTools_IndexedMapOfShape &theWhereIndices,
                       const TopoDS_Shape               &theWhat,
                             TopTools_ListOfShape       &theShapesInPlace);

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
