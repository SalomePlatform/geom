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
// File      : GEOMImpl_ICurveParametric.hxx
// Created   : Mon Jun 17 14:14:08 2013
// Author    : Edward AGAPOV (eap)

#ifndef __GEOMImpl_ICurveParametric_HXX__
#define __GEOMImpl_ICurveParametric_HXX__

#include "GEOM_Function.hxx"

/*!
 * \brief Interface to data of analitically defined curve
 *
 * WARNING: this data co-exists with data of either GEOMImpl_IPolyline
 *          or GEOMImpl_ISpline
 *
 * GEOMImpl_IPolyline data scheme:
 * -  POLY_ARG_LENG    1 (int)
 * -  POLY_ARG_LAST    1 (GEOM_Function)
 * -  POLY_ARG_CLOS    2 (int)
 * -  POLY_CONSTRUCTOR 3 (int)
 * -  POLY_ARG_ARRAY   4 (HArray1OfReal)
 *
 * GEOMImpl_ISpline data scheme:
 * -  ARG_POINTS  = 1, (HSequenceOfTransient)
 * -  ARG_CLOSED  = 2, (int)
 * -  ARG_REORDER = 3, (int)
 * -  CONSTRUCTOR = 4, (int)
 * -  ARG_ARRAY   = 5, (HArray1OfReal)
 * -  ARG_VEC_1   = 6, (GEOM_Function)
 * -  ARG_VEC_2   = 7  (GEOM_Function)
 */
struct GEOMImpl_ICurveParametric
{
  enum {
    CP_ARG_EXPR_X = 1,
    CP_ARG_EXPR_Y = 2,
    CP_ARG_EXPR_Z = 3,
    CP_ARG_MIN    = 1,
    CP_ARG_MAX    = 2,
    CP_ARG_STEP   = 3,
    CP_ARG_NBSTEP = 4
  };
  GEOMImpl_ICurveParametric(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  bool HasData() const { return !GetExprZ().IsEmpty(); }

  void SetExprX (const char* theExpr) { _func->SetString( CP_ARG_EXPR_X, theExpr ) ; }
  void SetExprY (const char* theExpr) { _func->SetString( CP_ARG_EXPR_Y, theExpr ) ; }
  void SetExprZ (const char* theExpr) { _func->SetString( CP_ARG_EXPR_Z, theExpr ) ; }
  void SetParamMin   (double theMin   ) { _func->SetReal( CP_ARG_MIN   , theMin   ) ; }
  void SetParamMax   (double theMax   ) { _func->SetReal( CP_ARG_MAX   , theMax   ) ; }
  void SetParamStep  (double theStep  ) { _func->SetReal( CP_ARG_STEP  , theStep  ) ; }
  void SetParamNbStep(double theNbStep) { _func->SetReal( CP_ARG_NBSTEP, theNbStep) ; }

  TCollection_AsciiString GetExprX() const { return _func->GetString( CP_ARG_EXPR_X ) ; }
  TCollection_AsciiString GetExprY() const { return _func->GetString( CP_ARG_EXPR_Y ) ; }
  TCollection_AsciiString GetExprZ() const { return _func->GetString( CP_ARG_EXPR_Z ) ; }
  double GetParamMin   () const { return _func->GetReal( CP_ARG_MIN    ) ; }
  double GetParamMax   () const { return _func->GetReal( CP_ARG_MAX    ) ; }
  double GetParamStep  () const { return _func->GetReal( CP_ARG_STEP   ) ; }
  double GetParamNbStep() const { return _func->GetReal( CP_ARG_NBSTEP ) ; }

  Handle(GEOM_Function) _func;
};

#endif
