// Copyright (C) 2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
//

// File:        CurveCreator_Curve.hxx
// Author:      Sergey KHROMOV

#ifndef _CurveCreator_Curve_HeaderFile
#define _CurveCreator_Curve_HeaderFile

#include "CurveCreator.hxx"
#include "CurveCreator_ICurve.hxx"
#include "CurveCreator_Macro.hxx"
#include "CurveCreator_Operation.hxx"

class CurveCreator_Section;
class CurveCreator_Listener;

/**
 *  The CurveCreator_Curve object is represented as one or more sets of
 *  connected points; thus CurveCreator_Curve object can contain several
 *  not connected curves (polylines or b-splines), each such curve has two
 *  only ends � start and end points � in other words non-manifold curves
 *  are not supported.
 */
class CURVECREATOR_EXPORT CurveCreator_Curve : public CurveCreator_ICurve
{
public:
  //! Constructor of the curve.
  /** The dimension is explicitly specified in the constructor
   *  and cannot be changed later.
   */
  CurveCreator_Curve(const CurveCreator::Dimension theDimension);

  /** Add points to the specified section (or last section
   *  if \a theISection is -1).
   */
  virtual void addPoints
    (const CurveCreator::Coordinates &thePoints, const int theISection = -1);

  friend class CurveCreator_CurveEditor;
  friend class CurveCreator_Operation;

};

#endif
