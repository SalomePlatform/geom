// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef _GEOMUtils_HXX_
#define _GEOMUtils_HXX_

#include <TopoDS_Shape.hxx>

#include <TopTools_ListOfShape.hxx>

#include <TopAbs.hxx>

#include <gp_Ax3.hxx>
#include <gp_Vec.hxx>

#include <NCollection_DataMap.hxx>

#include <functional>

inline Standard_Boolean IsEqual (const TopoDS_Shape& S1, const TopoDS_Shape& S2)
{
  return S1.IsSame(S2);
}

class GEOMUtils {

 public:
  /*!
   * \brief Get Local Coordinate System, corresponding to the given shape.
   *
   * Origin of the LCS is situated at the shape's center of mass.
   * Axes of the LCS are obtained from shape's location or,
   * if the shape is a planar face, from position of its plane.
   */
  Standard_EXPORT static gp_Ax3 GetPosition (const TopoDS_Shape& theShape);

  /*!
   * \brief Get vector, defined by the given edge.
   */
  Standard_EXPORT static gp_Vec GetVector (const TopoDS_Shape& theShape);

  /*!
   * \brief Sort shapes in the list by their coordinates.
   * \param SL The list of shapes to sort.
   */
  struct CompareShapes : public std::binary_function<TopoDS_Shape, TopoDS_Shape, bool>
  {
    CompareShapes (bool isOldSorting)
      : myIsOldSorting(isOldSorting) {}

    bool operator() (const TopoDS_Shape& lhs, const TopoDS_Shape& rhs);

    typedef NCollection_DataMap<TopoDS_Shape, std::pair<double, double> > GEOMUtils_DataMapOfShapeDouble;
    GEOMUtils_DataMapOfShapeDouble myMap;
    bool myIsOldSorting;
  };

  /*!
   * \brief Sort shapes by their centers of mass, using formula X*999 + Y*99 + Z*0.9
   */
  Standard_EXPORT static void SortShapes (TopTools_ListOfShape& SL,
                                          const Standard_Boolean isOldSorting = Standard_True);

  /*!
   * \brief Convert TopoDS_COMPSOLID to TopoDS_COMPOUND.
   *
   * If the argument shape is not of type TopoDS_COMPSOLID, this method returns it as is.
   *
   * \param theCompsolid The compsolid to be converted.
   * \retval TopoDS_Shape Returns the resulting compound.
   */
  Standard_EXPORT static TopoDS_Shape CompsolidToCompound (const TopoDS_Shape& theCompsolid);

  /*!
   * \brief Build a triangulation on \a theShape if it is absent.
   * \param theShape The shape to check/build triangulation on.
   * \retval bool Returns false if the shape has no faces, i.e. impossible to build triangulation.
   */
  Standard_EXPORT static bool CheckTriangulation (const TopoDS_Shape& theShape);

  /*!
   * \brief Return type of shape for explode. In case of compound it will be a type of its first sub shape.
   * \param theShape The shape to get type of.
   * \retval TopAbs_ShapeEnum Return type of shape for explode.
   */
  Standard_EXPORT static TopAbs_ShapeEnum GetTypeOfSimplePart (const TopoDS_Shape& theShape);

};

#endif
