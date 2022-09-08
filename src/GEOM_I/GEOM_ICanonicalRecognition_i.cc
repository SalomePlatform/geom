// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOM_ICanonicalRecognition_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Field.hxx"

#include "gp_Pln.hxx"
#include "gp_Sphere.hxx"
#include "gp_Cone.hxx"
#include "gp_Cylinder.hxx"
#include "gp_Circ.hxx"
#include "gp_Elips.hxx"

//=============================================================================
/*
 *   constructor:
 */
//=============================================================================
GEOM_ICanonicalRecognition_i::GEOM_ICanonicalRecognition_i (PortableServer::POA_ptr thePOA,
                                                  GEOM::GEOM_Gen_ptr theEngine,
                                                  ::GEOMImpl_ICanonicalRecognition* theImpl)
  :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_ICanonicalRecognition_i::GEOM_ICanonicalRecognition_i");
}

//=============================================================================
/*
 *  destructor
 */
//=============================================================================
GEOM_ICanonicalRecognition_i::~GEOM_ICanonicalRecognition_i()
{
  MESSAGE("GEOM_ICanonicalRecognition_i::~GEOM_ICanonicalRecognition_i");
}

static bool isValidDirection(const GEOM::ListOfDouble& theDir)
{
  return (theDir.length() == 3) && (gp_Vec(theDir[0], theDir[1], theDir[2]).Magnitude() > 0.);
}

//=============================================================================
/*
 *  \brief Check if the shape is planar
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isPlane(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theOrigin)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theShape);

  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.length() == 3;
  gp_Pln aPln;
  if (aIsValidNormal && aIsValidOrigin) {
    aPln = gp_Pln(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]),
      gp_Dir(theNormal[0], theNormal[1], theNormal[2]));
  }
  bool aResult = GetOperation()->isPlane(go, theTolerance, aPln);
  gp_Pnt aOrig = aPln.Location();
  theOrigin[0] = aOrig.X();
  theOrigin[1] = aOrig.Y();
  theOrigin[2] = aOrig.Z();

  gp_Dir aNorm = aPln.Axis().Direction();
  theNormal[0] = aNorm.X();
  theNormal[1] = aNorm.Y();
  theNormal[2] = aNorm.Z();

  return aResult;
}

//=============================================================================
/*
 * \brief Check if shape is spherical
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isSphere(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theOrigin, CORBA::Double& theRadius)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theShape);

  bool aIsValidOrigin = theOrigin.length() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Sphere aSphere;
  if (aIsValidOrigin && aIsValidRadius)
  {
    aSphere.SetLocation(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]));
    aSphere.SetRadius(theRadius);
  }
  else
    aSphere.SetRadius(1.0);
  bool aResult = GetOperation()->isSphere(go, theTolerance, aSphere);
  gp_Pnt aLoc = aSphere.Location();
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();
  theRadius = aSphere.Radius();

  return aResult;
}

//=============================================================================
/*
 * \brief Check if shape is conical
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isCone(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theAxis, GEOM::ListOfDouble& theApex, CORBA::Double& theHalfAngle)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theShape);

  bool aIsValidAxis = isValidDirection(theAxis);
  bool aIsValidApex = theApex.length() == 3;
  bool aIsValidAngle = theHalfAngle > 0;
  gp_Cone aCone;
  if (aIsValidAxis && aIsValidApex && aIsValidAngle)
  {
    gp_Pnt aLoc(theApex[0], theApex[1], theApex[2]);
    gp_Ax3 aAx3(aLoc, gp_Dir(theAxis[0], theAxis[1], theAxis[2]));
    aCone.SetPosition(aAx3);
  }
  else
    aCone.SetRadius(1.0);
  bool aResult = GetOperation()->isCone(go, theTolerance, aCone);
  gp_Dir aDir = aCone.Axis().Direction();
  theAxis[0] = aDir.X();
  theAxis[1] = aDir.Y();
  theAxis[2] = aDir.Z();

  gp_Pnt aApex = aCone.Apex();
  theApex[0] = aApex.X();
  theApex[1] = aApex.Y();
  theApex[2] = aApex.Z();

  theHalfAngle = aCone.SemiAngle();
  return aResult;
}

//=============================================================================
/*!
 * \brief Check if shape is cylinder
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isCylinder(GEOM::GEOM_Object_ptr theShape, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theAxis, GEOM::ListOfDouble& theOrigin, CORBA::Double& theRadius)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theShape);

  bool aIsValidAxis = isValidDirection(theAxis);
  bool aIsValidOrigin = theOrigin.length() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Cylinder aCylinder;
  if (aIsValidAxis && aIsValidOrigin && aIsValidRadius)
  {
    gp_Pnt aLoc(theOrigin[0], theOrigin[0], theOrigin[0]);
    gp_Ax3 aAx3(aLoc, gp_Dir(theAxis[0], theAxis[1], theAxis[2]));
    aCylinder.SetPosition(aAx3);
    aCylinder.SetRadius(theRadius);
  }
  else
    aCylinder.SetRadius(1.0);
  bool aResult = GetOperation()->isCylinder(go, theTolerance, aCylinder);
  gp_Dir aDir = aCylinder.Axis().Direction();
  theAxis[0] = aDir.X();
  theAxis[1] = aDir.Y();
  theAxis[2] = aDir.Z();

  gp_Pnt aLoc = aCylinder.Location();
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  theRadius = aCylinder.Radius();

  return aResult;
}

//=============================================================================
/*!
 * \brief Check if edge / wire is line
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isLine(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theDir, GEOM::ListOfDouble& theOrigin)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theEdge);

  bool aIsValidDir = isValidDirection(theDir);
  bool aIsValidOrigin = theOrigin.length() == 3;
  gp_Lin aLine;
  if (aIsValidDir && aIsValidOrigin)
  {
    aLine.SetLocation(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]));
    aLine.SetDirection(gp_Dir(theDir[0], theDir[1], theDir[2]));
  }
  bool aResult = GetOperation()->isLine(go, theTolerance, aLine);
  gp_Pnt aLoc = aLine.Location();
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aDir = aLine.Direction();
  theDir[0] = aDir.X();
  theDir[1] = aDir.Y();
  theDir[2] = aDir.Z();

  return aResult;
}

//=============================================================================
/*!
 * \brief Check if edge / wire is circle
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isCircle(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theOrigin, CORBA::Double& theRadius)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theEdge);
  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.length() == 3;
  bool aIsValidRadius = theRadius > 0;
  gp_Circ aCircle;
  if (aIsValidNormal && aIsValidOrigin && aIsValidRadius)
  {
    gp_Ax2 aAx2(gp_Pnt(theOrigin[0], theOrigin[1], theOrigin[2]),
      gp_Dir(theNormal[0], theNormal[1], theNormal[2]));
    aCircle.SetPosition(aAx2);
    aCircle.SetRadius(theRadius);
  }
  else
    aCircle.SetRadius(1.0);
  bool aResult = GetOperation()->isCircle(go, theTolerance, aCircle);
  gp_Pnt aLoc = aCircle.Location();
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aDir = aCircle.Axis().Direction();
  theNormal[0] = aDir.X();
  theNormal[1] = aDir.Y();
  theNormal[2] = aDir.Z();
  theRadius = aCircle.Radius();

  return aResult;
}

//=============================================================================
/*!
 * \brief Check if edge / wire is ellipse
 */
 //=============================================================================
CORBA::Boolean GEOM_ICanonicalRecognition_i::isEllipse(GEOM::GEOM_Object_ptr theEdge, CORBA::Double theTolerance,
  GEOM::ListOfDouble& theNormal, GEOM::ListOfDouble& theDirX, GEOM::ListOfDouble& theOrigin,
  CORBA::Double& theMajorRadius, CORBA::Double& theMinorRadius)
{
  Handle(::GEOM_Object) go = GetObjectImpl(theEdge);
  bool aIsValidNormal = isValidDirection(theNormal);
  bool aIsValidOrigin = theOrigin.length() == 3;
  bool aIsValidDirX = isValidDirection(theDirX);
  bool aIsValidRad1 = (theMajorRadius > 0) && (theMajorRadius > theMinorRadius);
  bool aIsValidRad2 = (theMinorRadius > 0) && (theMajorRadius > theMinorRadius);

  gp_Elips aElips;
  if (aIsValidNormal && aIsValidOrigin && aIsValidDirX && aIsValidRad1 && aIsValidRad2)
  {
    gp_Pnt anOrigin(theOrigin[0], theOrigin[1], theOrigin[2]);
    gp_XYZ aNormal(theNormal[0], theNormal[1], theNormal[2]);
    gp_XYZ aDirX(theDirX[0], theDirX[1], theDirX[2]);
    Standard_Boolean isCollinear =
        aNormal.CrossSquareMagnitude(aDirX) < Precision::SquareConfusion();
    gp_Ax2 aAx2 = isCollinear ? gp_Ax2(anOrigin, aNormal) : gp_Ax2(anOrigin, aNormal, aDirX);
    aElips = gp_Elips(aAx2, theMajorRadius, theMinorRadius);
  }
  else
    aElips.SetMajorRadius(1.0);
  bool aResult = GetOperation()->isEllipse(go, theTolerance, aElips);
  gp_Pnt aLoc = aElips.Position().Location();
  if (theOrigin.length() != 3)
    theOrigin.allocbuf(3);
  theOrigin[0] = aLoc.X();
  theOrigin[1] = aLoc.Y();
  theOrigin[2] = aLoc.Z();

  gp_Dir aNorm = aElips.Position().Direction();
  theNormal[0] = aNorm.X();
  theNormal[1] = aNorm.Y();
  theNormal[2] = aNorm.Z();

  gp_Dir aDirX = aElips.Position().XDirection();
  theDirX[0] = aDirX.X();
  theDirX[1] = aDirX.Y();
  theDirX[2] = aDirX.Z();

  theMajorRadius = aElips.MajorRadius();
  theMinorRadius = aElips.MinorRadius();

  return aResult;
}
