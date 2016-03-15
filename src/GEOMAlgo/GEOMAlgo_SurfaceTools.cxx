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

// File:        GEOMAlgo_SurfaceTools.cxx
// Created:     Thu Jan 27 11:05:16 2005
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_SurfaceTools.hxx>

#include <math.h>

#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Vec.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <IntSurf_Quadric.hxx>


//=======================================================================
//function : GetState
//purpose  :
//=======================================================================
 Standard_Integer GEOMAlgo_SurfaceTools::GetState
  (const gp_Pnt& aP,
   const GeomAdaptor_Surface& aGAS,
   const Standard_Real aTol,
   TopAbs_State& aState)
{
  Standard_Integer    iErr  = 0;
  GeomAbs_SurfaceType aType = aGAS.GetType();
  IntSurf_Quadric     aQuad;
  //
  aState = TopAbs_UNKNOWN;
  //
  switch (aType) {
  case GeomAbs_Plane:
    aQuad.SetValue(aGAS.Plane());
    break;

  case GeomAbs_Cylinder:
    aQuad.SetValue(aGAS.Cylinder());
    break;

  case GeomAbs_Sphere:
    aQuad.SetValue(aGAS.Sphere());
    break;

  default:
    iErr=1; // unprocessed surface type
    break;
  }
  //
  if (!iErr) {
    const Standard_Real aDp = aQuad.Distance(aP);
    //
    aState = TopAbs_ON;
    //
    if (aDp > aTol) {
      aState = TopAbs_OUT;
    } else if (aDp < -aTol) {
      aState = TopAbs_IN;
    }
  }
  //
  return iErr;
}
//=======================================================================
//function : GetState
//purpose  :
//=======================================================================
 Standard_Integer GEOMAlgo_SurfaceTools::GetState
  (const gp_Pnt& aP,
   const Handle(Geom_Surface)& aSurf,
   const Standard_Real aTol,
   TopAbs_State& aState)
{
  Standard_Integer iErr;
  GeomAdaptor_Surface aGAS;
  //
  aState=TopAbs_UNKNOWN;
  aGAS.Load(aSurf);
  //
  iErr=GEOMAlgo_SurfaceTools::GetState(aP, aGAS, aTol, aState);
  //
  return iErr;
}
//=======================================================================
//function : ReverseState
//purpose  :
//=======================================================================
 TopAbs_State GEOMAlgo_SurfaceTools::ReverseState
  (const TopAbs_State aState)
{
  TopAbs_State aRSt=aState;
  //
  switch (aState) {
    case TopAbs_IN:
     aRSt=TopAbs_OUT;
     break;
   case TopAbs_OUT:
     aRSt=TopAbs_IN;
     break;
   default:
     break;
  }
  //
  return aRSt;
}
//=======================================================================
//function : IsCoaxial
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_SurfaceTools::IsCoaxial
  (const gp_Pnt& aP1,
   const gp_Pnt& aP2,
   const gp_Cylinder& aCyl,
   const Standard_Real aTol)
{
  const gp_XYZ &aLoc   = aCyl.Location().XYZ();
  const gp_Ax1 &aAxis  = aCyl.Axis();
  const gp_XYZ &aDAxis = aAxis.Direction().XYZ();
  gp_XYZ        aDP1   = aP1.XYZ().Subtracted(aLoc);
  gp_XYZ        aDP2   = aP2.XYZ().Subtracted(aLoc);
  Standard_Real aDot1  = aDP1.Dot(aDAxis);
  Standard_Real aDot2  = aDP1.Dot(aDAxis);
  //Standard_Real aTol2  = aTol*aTol;

  // Project P1 and P2 onto a plane with location aLoc and Norm aDAxis.
  aDP1.Subtract(aDAxis.Multiplied(aDot1));
  aDP2.Subtract(aDAxis.Multiplied(aDot2));

  Standard_Real    aRadius1 = aDP1.Modulus();
  Standard_Real    aRadius2 = aDP2.Modulus();
  Standard_Boolean isOn     = Standard_False;

  if (fabs(aRadius1 - aRadius2) <= aTol) {
    // Check the deflection of the middle point.
    gp_XYZ        aMidP       = 0.5*(aDP1 + aDP2);
    //Standard_Real aMidRadius1 = aMidP.Modulus();

    if (fabs(aRadius1 - aRadius2) <= aTol) {
      isOn = Standard_True;
    }
  }

  return isOn;
}
//=======================================================================
//function : IsAnalytic
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_SurfaceTools::IsAnalytic
  (const Handle(Geom_Surface)& aSurf)
{
  Standard_Boolean bRet;
  GeomAbs_SurfaceType aType;
  GeomAdaptor_Surface aGAS;
  //
  aGAS.Load(aSurf);
  aType=aGAS.GetType();
  bRet=(aType==GeomAbs_Plane ||
        aType==GeomAbs_Cylinder ||
        aType==GeomAbs_Sphere);
  return bRet;
}
//=======================================================================
//function : IsConformState
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_SurfaceTools::IsConformState
  (const TopAbs_State aST1,
   const GEOMAlgo_State aST2)
{
  Standard_Boolean bRet=Standard_False;
  //
  switch (aST2) {
    case GEOMAlgo_ST_IN:
      if (aST1==TopAbs_IN) {
        bRet=!bRet;
      }
      break;
    case GEOMAlgo_ST_OUT:
      if (aST1==TopAbs_OUT) {
        bRet=!bRet;
      }
      break;
    case GEOMAlgo_ST_ON:
      if (aST1==TopAbs_ON) {
        bRet=!bRet;
      }
      break;
    case GEOMAlgo_ST_ONIN:
      if (aST1==TopAbs_ON || aST1==TopAbs_IN) {
        bRet=!bRet;
      }
      break;
    case GEOMAlgo_ST_ONOUT:
      if (aST1==TopAbs_ON || aST1==TopAbs_OUT) {
        bRet=!bRet;
      }
      break;
    default:
      break;
  }
  return bRet;
}
