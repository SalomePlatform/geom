// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef _BlockFix_UnionFaces_HeaderFile
#define _BlockFix_UnionFaces_HeaderFile

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard.hxx>
#include <Standard_Macro.hxx>

class TopoDS_Shape;
class TopoDS_Face;

class BlockFix_UnionFaces
{
public:
  Standard_EXPORT BlockFix_UnionFaces();

  Standard_EXPORT Standard_Real& GetTolerance();

  /* \brief To get/set the OptimumNbFaces parameter
   *
   * If a being processed solid has less than OptimumNbFaces
   *    faces, no union will be performed.
   * By default this parameter is set to 6 (to correctly
   *    process blocks - hexahedral solids)
   * Special values: 0 - do all possible unions, regardless the faces quantity,
   *                 negative - do not perform any unions, regardless the faces quantity.
   *
   */
  Standard_EXPORT Standard_Integer& GetOptimumNbFaces();

  Standard_EXPORT TopoDS_Shape Perform(const TopoDS_Shape& Shape);

  Standard_EXPORT virtual Standard_Boolean IsSameDomain(const TopoDS_Face& aFace,
                                                        const TopoDS_Face& aChekedFace) const;

  Standard_EXPORT virtual void MovePCurves(TopoDS_Face& aTarget,
                                           const TopoDS_Face& aSource) const;

private:
  Standard_Real myTolerance;
  Standard_Integer myOptimumNbFaces;
};

#endif
