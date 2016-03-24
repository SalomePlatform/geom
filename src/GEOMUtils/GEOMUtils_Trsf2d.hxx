// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMUtils_Trsf2d_HXX_
#define _GEOMUtils_Trsf2d_HXX_


#include <Geom2dHatch_Hatcher.hxx>
#include <GeomAbs_IsoType.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS_Face.hxx>


/*!
 *  This class represents a non-persistent transformation in 2D space. 
 *  The transformations can be represented as follow :
 *
 *       V1   V2   T       XY        XY
 *    | a11  a12  a13 |   | x |     | x'|
 *    | a21  a22  a23 |   | y |     | y'|
 *    |  0    0    1  |   | 1 |     | 1 |

 *  where {V1, V2} defines the vectorial part of the transformation
 *  and T defines the translation part of the transformation.
 *  This transformation can change the nature of the objects if it is
 *  anisotropic.
 */
namespace GEOMUtils
{
  class Trsf2d
  {

  public:
    
    /**
     * Constructor. Initializes the object with the transformation parameters.
     * Input parameters are not checked for validity. It is under responsibility
     * of the caller.
     */
    Standard_EXPORT Trsf2d(const Standard_Real a11,
                           const Standard_Real a12,
                           const Standard_Real a13,
                           const Standard_Real a21,
                           const Standard_Real a22,
                           const Standard_Real a23);

    /**
     * Transform the point. The passed parameter is modified to have
     * a transformed value.
     *
     * \param thePnt the point.
     */
    Standard_EXPORT void TransformD0(gp_Pnt2d &thePnt)const;

    /**
     * Transform the point and the first derivative vector. The passed
     * parameters are modified to have a transformed value.
     *
     * \param thePnt the point.
     * \param theVec1 the first derivative vector.
     */
    Standard_EXPORT void TransformD1(gp_Pnt2d &thePnt,
                                     gp_Vec2d &theVec1) const;

    /**
     * Transform the point, the first and second derivative vectors. The passed
     * parameters are modified to have a transformed value.
     *
     * \param thePnt the point.
     * \param theVec1 the first derivative vector.
     * \param theVec2 the second derivative vector.
     */
    Standard_EXPORT void TransformD2(gp_Pnt2d &thePnt,
                                     gp_Vec2d &theVec1,
                                     gp_Vec2d &theVec2) const;

    /**
     * Transform the point, the first, second and third derivative vectors.
     * The passed parameters are modified to have a transformed value.
     *
     * \param thePnt the point.
     * \param theVec1 the first derivative vector.
     * \param theVec2 the second derivative vector.
     * \param theVec2 the third derivative vector.
     */
    Standard_EXPORT void TransformD3(gp_Pnt2d &thePnt,
                                     gp_Vec2d &theVec1,
                                     gp_Vec2d &theVec2,
                                     gp_Vec2d &theVec3) const;

  private:

    /**
     * Transform the vector.
     *
     * \param thePnt the point.
     * \param theVec the vector.
     */
    void TransformVector(const gp_Pnt2d &thePnt,
                               gp_Vec2d &theVec) const;

  private:

    Standard_Real myA11;
    Standard_Real myA12;
    Standard_Real myA13;
    Standard_Real myA21;
    Standard_Real myA22;
    Standard_Real myA23;

  };
}

#endif
