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


#include <GEOMUtils_Trsf2d.hxx>


//=======================================================================
//function : Trsf2d
//purpose  :
//=======================================================================
GEOMUtils::Trsf2d::Trsf2d(const Standard_Real a11,
                          const Standard_Real a12,
                          const Standard_Real a13,
                          const Standard_Real a21,
                          const Standard_Real a22,
                          const Standard_Real a23)
: myA11(a11),
  myA12(a12),
  myA13(a13),
  myA21(a21),
  myA22(a22),
  myA23(a23)
{
}

//=======================================================================
//function : TransformD0
//purpose  :
//=======================================================================
void GEOMUtils::Trsf2d::TransformD0(gp_Pnt2d &thePnt) const
{
  const Standard_Real aX = myA11*thePnt.X() + myA12*thePnt.Y() + myA13;
  const Standard_Real aY = myA21*thePnt.X() + myA22*thePnt.Y() + myA23;

  thePnt.SetCoord(aX, aY);
}

//=======================================================================
//function : TransformD1
//purpose  :
//=======================================================================
void GEOMUtils::Trsf2d::TransformD1(gp_Pnt2d &thePnt,
                                    gp_Vec2d &theVec1) const
{
  TransformVector(thePnt, theVec1);
  TransformD0(thePnt);
}

//=======================================================================
//function : TransformD2
//purpose  :
//=======================================================================
void GEOMUtils::Trsf2d::TransformD2(gp_Pnt2d &thePnt,
                                    gp_Vec2d &theVec1,
                                    gp_Vec2d &theVec2) const
{
  TransformVector(thePnt, theVec1);
  TransformVector(thePnt, theVec2);
  TransformD0(thePnt);
}

//=======================================================================
//function : TransformD3
//purpose  :
//=======================================================================
void GEOMUtils::Trsf2d::TransformD3(gp_Pnt2d &thePnt,
                                    gp_Vec2d &theVec1,
                                    gp_Vec2d &theVec2,
                                    gp_Vec2d &theVec3) const
{
  TransformVector(thePnt, theVec1);
  TransformVector(thePnt, theVec2);
  TransformVector(thePnt, theVec3);
  TransformD0(thePnt);
}

//=======================================================================
//function : TransformVector
//purpose  :
//=======================================================================
void GEOMUtils::Trsf2d::TransformVector(const gp_Pnt2d &thePnt,
                                              gp_Vec2d &theVec) const
{
  gp_Pnt2d aP0(thePnt.XY());
  gp_Pnt2d aP1(thePnt.XY().Added(theVec.XY()));

  TransformD0(aP0);
  TransformD0(aP1);
  theVec.SetXY(aP1.XY().Subtracted(aP0.XY()));
}
