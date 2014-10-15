// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include "GEOM_I3DPrimOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#define SUBSHAPE_ERROR "Sub shape cannot be transformed"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_I3DPrimOperations_i::GEOM_I3DPrimOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOMImpl_I3DPrimOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_I3DPrimOperations_i::GEOM_I3DPrimOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_I3DPrimOperations_i::~GEOM_I3DPrimOperations_i()
{
  MESSAGE("GEOM_I3DPrimOperations_i::~GEOM_I3DPrimOperations_i");
}


//=============================================================================
/*!
 *  MakeBoxDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeBoxDXDYDZ (CORBA::Double theDX,
                                                               CORBA::Double theDY,
                                                               CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Box
  Handle(GEOM_Object) anObject = GetOperations()->MakeBoxDXDYDZ(theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeBoxTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeBoxTwoPnt
                      (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);

  if (aPnt1.IsNull() || aPnt2.IsNull()) return aGEOMObject._retn();

  //Create the Box
  Handle(GEOM_Object) anObject = GetOperations()->MakeBoxTwoPnt(aPnt1, aPnt2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFaceHW
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeFaceHW (CORBA::Double theH,
                                                            CORBA::Double theW,
                                                            CORBA::Short  theOrientation)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theH == 0 || theW == 0)
    return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeFaceHW(theH, theW, theOrientation);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFaceObjHW
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeFaceObjHW
                                               (GEOM::GEOM_Object_ptr theObj,
                                                CORBA::Double theH,
                                                CORBA::Double theW)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference object
  Handle(GEOM_Object) anObj = GetObjectImpl(theObj);

  if (anObj.IsNull())
    return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeFaceObjHW(anObj, theH, theW);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeDiskPntVecR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeDiskPntVecR
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Disk
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeDiskPntVecR(aPnt, aVec, theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeDiskThreePnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeDiskThreePnt
                      (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2,
                       GEOM::GEOM_Object_ptr thePnt3)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Disk
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeDiskThreePnt(aPnt1, aPnt2, aPnt3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeDiskR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeDiskR (CORBA::Double theR,
                                                           CORBA::Short  theOrientation)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theR == 0)
    return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeDiskR(theR, theOrientation);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderRH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderRH (CORBA::Double theR,
                                                                CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderRH(theR, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderRHA
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderRHA (CORBA::Double theR,
								 CORBA::Double theH,
								 CORBA::Double theA)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderRHA(theR, theH, theA);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderPntVecRH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderPntVecRH
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theR, CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderPntVecRH(aPnt, aVec, theR, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderPntVecRHA
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderPntVecRHA
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theR, CORBA::Double theH, CORBA::Double theA)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderPntVecRHA(aPnt, aVec, theR, theH, theA);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeConeR1R2H
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeConeR1R2H (CORBA::Double theR1,
                                                               CORBA::Double theR2,
                                                               CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cone
  Handle(GEOM_Object) anObject = GetOperations()->MakeConeR1R2H(theR1, theR2, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeConePntVecR1R2H
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeConePntVecR1R2H
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theR1, CORBA::Double theR2, CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cone
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeConePntVecR1R2H(aPnt, aVec, theR1, theR2, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSphereR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeSphereR (CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cone
  Handle(GEOM_Object) anObject = GetOperations()->MakeSphereR(theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSpherePntR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeSpherePntR
                      (GEOM::GEOM_Object_ptr thePnt, CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);

  if (aPnt.IsNull()) return aGEOMObject._retn();

  //Create the Sphere
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeSpherePntR(aPnt, theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTorusRR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeTorusRR
                      (CORBA::Double theRMajor, CORBA::Double theRMinor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  // Make Torus
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeTorusRR(theRMajor, theRMinor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTorusPntVecRR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeTorusPntVecRR
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theRMajor, CORBA::Double theRMinor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Torus
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeTorusPntVecRR(aPnt, aVec, theRMajor, theRMinor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismVecH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismVecH
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismVecH(aBase, aVec, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismVecH2Ways
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismVecH2Ways
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
      GetOperations()->MakePrismVecH2Ways(aBase, aVec, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismVecH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismVecHWithScaling
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theH, CORBA::Double theScaleFactor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismVecH(aBase, aVec, theH, theScaleFactor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismTwoPnt
                                             (GEOM::GEOM_Object_ptr theBase,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  Handle(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismTwoPnt(aBase, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismTwoPnt2Ways
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismTwoPnt2Ways
                                             (GEOM::GEOM_Object_ptr theBase,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  Handle(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismTwoPnt2Ways(aBase, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismTwoPntWithScaling
                                             (GEOM::GEOM_Object_ptr theBase,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2,
                                              CORBA::Double         theScaleFactor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  Handle(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismTwoPnt(aBase, aPoint1, aPoint2, theScaleFactor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismDXDYDZ
                      (GEOM::GEOM_Object_ptr theBase, CORBA::Double theDX,
                       CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
      GetOperations()->MakePrismDXDYDZ(aBase, theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismDXDYDZ2Ways
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismDXDYDZ2Ways
                      (GEOM::GEOM_Object_ptr theBase, CORBA::Double theDX,
                       CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
      GetOperations()->MakePrismDXDYDZ2Ways(aBase, theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismDXDYDZWithScaling
                      (GEOM::GEOM_Object_ptr theBase, CORBA::Double theDX,
                       CORBA::Double theDY, CORBA::Double theDZ,
                       CORBA::Double theScaleFactor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismDXDYDZ(aBase, theDX, theDY, theDZ, theScaleFactor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeDraftPrism
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeDraftPrism
                      (GEOM::GEOM_Object_ptr theInitShape, GEOM::GEOM_Object_ptr theBase, 
                       CORBA::Double  theHeight,
                       CORBA::Double  theAngle,
                       CORBA::Boolean theFuse)
{
  GEOM::GEOM_Object_var aGEOMObject;
  
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aInit   = GetObjectImpl(theInitShape);
  Handle(GEOM_Object) aBase   = GetObjectImpl(theBase);

  if (aBase.IsNull() || aInit.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject = GetOperations()->MakeDraftPrism(aInit, aBase, theHeight, theAngle, theFuse);
 
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePipe
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipe
                 (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr thePath)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aPath = GetObjectImpl(thePath);

  if (aBase.IsNull() || aPath.IsNull()) return aGEOMObject._retn();

  //Create the Pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipe(aBase, aPath);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeRevolutionAxisAngle
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeRevolutionAxisAngle
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theAxis,
                       CORBA::Double theAngle)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aBase.IsNull() || anAxis.IsNull()) return aGEOMObject._retn();

  //Create the Revolution
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeRevolutionAxisAngle(aBase, anAxis, theAngle);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeRevolutionAxisAngle2Ways
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeRevolutionAxisAngle2Ways
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theAxis,
                       CORBA::Double theAngle)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aBase.IsNull() || anAxis.IsNull()) return aGEOMObject._retn();

  //Create the Revolution
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeRevolutionAxisAngle2Ways(aBase, anAxis, theAngle);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilling
 */
//=============================================================================
GEOM::GEOM_Object_ptr
GEOM_I3DPrimOperations_i::MakeFilling(const GEOM::ListOfGO&     theContours,
                                      CORBA::Long               theMinDeg,
                                      CORBA::Long               theMaxDeg,
                                      CORBA::Double             theTol2D,
                                      CORBA::Double             theTol3D,
                                      CORBA::Long               theNbIter,
                                      GEOM::filling_oper_method theMethod,
                                      CORBA::Boolean            theApprox)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  std::list< Handle(GEOM_Object) > aShapes;
  if (! GetListOfObjectsImpl( theContours, aShapes ))
    return aGEOMObject._retn();

  int aMethod = 0;
  switch (theMethod) {
  case GEOM::FOM_Default:
    {
      // Default (standard behaviour)
      aMethod = 0;
    }
    break;
  case GEOM::FOM_UseOri:
    {
      // Use edges orientation
      aMethod = 1;
    }
    break;
  case GEOM::FOM_AutoCorrect:
    {
      // Auto-correct edges orientation
      aMethod = 2;
    }
    break;
  default:
    {}
  }

  //Create the Solid
  Handle(GEOM_Object) anObject = GetOperations()->MakeFilling
    (aShapes, theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter, aMethod, theApprox);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeThruSections
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeThruSections(const GEOM::ListOfGO& theSeqSections,
                                                                 CORBA::Boolean theModeSolid,
                                                                 CORBA::Double thePreci,
                                                                 CORBA::Boolean theRuled)
{
   GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqSections = new TColStd_HSequenceOfTransient;
  int ind, aLen;

  //Get the shapes
  aLen = theSeqSections.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theSeqSections[ind]);
    if (!aSh.IsNull())
      aSeqSections->Append(aSh);
  }
  if (!aSeqSections->Length())
    return aGEOMObject._retn();

  // Make shell or solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeThruSections(aSeqSections,theModeSolid,thePreci,theRuled);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePipeWithDifferentSections
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipeWithDifferentSections
                      (const GEOM::ListOfGO& theBases,
                       const GEOM::ListOfGO& theLocations,
                       GEOM::GEOM_Object_ptr thePath,
                       CORBA::Boolean theWithContact,
                       CORBA::Boolean theWithCorrections)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqLocations = new TColStd_HSequenceOfTransient;
  int ind=0, aNbBases =0,aNbLocs=0;

  //Get the shapes
  aNbBases = theBases.length();
  aNbLocs = theLocations.length();

  if (aNbLocs && aNbBases != aNbLocs)
    return aGEOMObject._retn();

  Handle(GEOM_Object) aPath = GetObjectImpl(thePath);
  if (aPath.IsNull())
    return aGEOMObject._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    Handle(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs)
    {
      Handle(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);
  }
  if (!aSeqBases->Length())
    return aGEOMObject._retn();

  // Make pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipeWithDifferentSections(aSeqBases,aSeqLocations ,aPath,
                                                   theWithContact,theWithCorrections);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakePipeWithShellSections
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipeWithShellSections
                 (const GEOM::ListOfGO& theBases,
                  const GEOM::ListOfGO& theSubBases,
                  const GEOM::ListOfGO& theLocations,
                  GEOM::GEOM_Object_ptr thePath,
                  CORBA::Boolean theWithContact,
                  CORBA::Boolean theWithCorrections)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqSubBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqLocations = new TColStd_HSequenceOfTransient;
  int ind=0, aNbBases=0, aNbSubBases=0, aNbLocs=0;

  //Get the shapes
  aNbBases = theBases.length();
  aNbSubBases = theSubBases.length();
  aNbLocs = theLocations.length();

  if (aNbLocs && aNbBases != aNbLocs)
    return aGEOMObject._retn();

  Handle(GEOM_Object) aPath = GetObjectImpl(thePath);
  if (aPath.IsNull())
    return aGEOMObject._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    Handle(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs) {
      Handle(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);

    if (aNbSubBases >= aNbBases) {
      Handle(GEOM_Object) aSubBase = GetObjectImpl(theSubBases[ind]);
      if (aSubBase.IsNull()) {
        aSeqSubBases->Clear();
        aNbSubBases = 0;
        continue;
      }
      aSeqSubBases->Append(aSubBase);
    }
  }
  if (!aSeqBases->Length())
    return aGEOMObject._retn();

  // Make pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipeWithShellSections(aSeqBases, aSeqSubBases,
                                               aSeqLocations, aPath,
                                               theWithContact, theWithCorrections);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakePipeShellsWithoutPath
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipeShellsWithoutPath
                 (const GEOM::ListOfGO& theBases,
                  const GEOM::ListOfGO& theLocations)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqLocations = new TColStd_HSequenceOfTransient;
  int ind=0, aNbBases=0, aNbLocs=0;

  //Get the shapes
  aNbBases = theBases.length();
  aNbLocs = theLocations.length();

  if (aNbLocs && aNbBases != aNbLocs)
    return aGEOMObject._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    Handle(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs) {
      Handle(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);
  }

  if (!aSeqBases->Length())
    return aGEOMObject._retn();

  // Make pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipeShellsWithoutPath(aSeqBases,aSeqLocations);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePipeBiNormalAlongVector
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipeBiNormalAlongVector
                 (GEOM::GEOM_Object_ptr theBase,
                  GEOM::GEOM_Object_ptr thePath,
                  GEOM::GEOM_Object_ptr theVec)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetObjectImpl(theBase);
  Handle(GEOM_Object) aPath = GetObjectImpl(thePath);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aPath.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipeBiNormalAlongVector(aBase, aPath, aVec);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeThickening
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeThickening
                 (GEOM::GEOM_Object_ptr theObject,
                  CORBA::Double theOffset,
                  CORBA::Boolean doCopy)
{
  GEOM::GEOM_Object_var aGEOMObject;
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape() && !doCopy) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }
  
  if (!doCopy)
    aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the basic object
  Handle(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();
  
  //Create the thickened shape
  if (doCopy)
  {
    Handle(GEOM_Object) anObject = GetOperations()->MakeThickening(
      aBasicObject, theOffset, doCopy);
    if (!GetOperations()->IsDone() || anObject.IsNull())
      return aGEOMObject._retn();
    
    return GetObject(anObject);
  }
  else
  {
    GetOperations()->MakeThickening(aBasicObject, theOffset, doCopy);
    
    // Update GUI.
    UpdateGUIForObject(theObject);
    
    return aGEOMObject._retn();
  }
}

//=============================================================================
/*!
 *  RestorePath
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::RestorePath
                 (GEOM::GEOM_Object_ptr theShape,
                  GEOM::GEOM_Object_ptr theBase1,
                  GEOM::GEOM_Object_ptr theBase2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aBase1 = GetObjectImpl(theBase1);
  Handle(GEOM_Object) aBase2 = GetObjectImpl(theBase2);

  if (aShape.IsNull() || aBase1.IsNull() || aBase2.IsNull()) return aGEOMObject._retn();

  // Create the Path
  Handle(GEOM_Object) anObject = GetOperations()->RestorePath(aShape, aBase1, aBase2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  RestorePathEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::RestorePathEdges
                 (GEOM::GEOM_Object_ptr theShape,
                  const GEOM::ListOfGO& theBase1,
                  const GEOM::ListOfGO& theBase2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  Handle(TColStd_HSequenceOfTransient) aSeqBases1 = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqBases2 = new TColStd_HSequenceOfTransient;

  int ind;
  int aNbBases1 = theBase1.length();
  int aNbBases2 = theBase2.length();

  for (ind = 0; ind < aNbBases1; ind++) {
    Handle(GEOM_Object) aBase = GetObjectImpl(theBase1[ind]);
    if (!aBase.IsNull())
      aSeqBases1->Append(aBase);
  }
  for (ind = 0; ind < aNbBases2; ind++) {
    Handle(GEOM_Object) aBase = GetObjectImpl(theBase2[ind]);
    if (!aBase.IsNull())
      aSeqBases2->Append(aBase);
  }

  if (!aSeqBases1->Length() || !aSeqBases2->Length())
    return aGEOMObject._retn();

  // Create the Path
  Handle(GEOM_Object) anObject = GetOperations()->RestorePath(aShape, aSeqBases1, aSeqBases2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
