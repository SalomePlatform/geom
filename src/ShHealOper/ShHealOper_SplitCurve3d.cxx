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

// File:      ShHealOper_SplitCurve3d.cxx
// Created:   05.05.04 12:48:36
// Author:    Galina KULIKOVA
//
#include <ShHealOper_SplitCurve3d.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeExtend.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShHealOper_SplitCurve3d,ShapeUpgrade_SplitCurve3d)

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void ShHealOper_SplitCurve3d::Compute()
{
  if(myValues.IsNull())
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL);
  else {
    SetSplitValues(myValues);
    myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE1);
  }
}


