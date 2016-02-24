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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeBoxDXDYDZ(theDX, theDY, theDZ);
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

  HANDLE_NAMESPACE(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);

  if (aPnt1.IsNull() || aPnt2.IsNull()) return aGEOMObject._retn();

  //Create the Box
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeBoxTwoPnt(aPnt1, aPnt2);
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeFaceHW(theH, theW, theOrientation);
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
  HANDLE_NAMESPACE(GEOM_Object) anObj = GetObjectImpl(theObj);

  if (anObj.IsNull())
    return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeFaceObjHW(anObj, theH, theW);
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Disk
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  HANDLE_NAMESPACE(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Disk
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeDiskR(theR, theOrientation);
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCylinderRH(theR, theH);
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCylinderRHA(theR, theH, theA);
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cylinder
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCylinderPntVecRH(aPnt, aVec, theR, theH);
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cylinder
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCylinderPntVecRHA(aPnt, aVec, theR, theH, theA);
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeConeR1R2H(theR1, theR2, theH);
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cone
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeSphereR(theR);
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);

  if (aPnt.IsNull()) return aGEOMObject._retn();

  //Create the Sphere
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Torus
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);

  if (aBase.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
                       CORBA::Boolean theFuse,
                       CORBA::Boolean theInvert)
{
  GEOM::GEOM_Object_var aGEOMObject;
  
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  HANDLE_NAMESPACE(GEOM_Object) aInit   = GetObjectImpl(theInitShape);
  HANDLE_NAMESPACE(GEOM_Object) aBase   = GetObjectImpl(theBase);

  if (aBase.IsNull() || aInit.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeDraftPrism(aInit, aBase, theHeight, theAngle, theFuse, theInvert);
 
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePipe
 */
//=============================================================================
GEOM::ListOfGO *GEOM_I3DPrimOperations_i::MakePipe
                           (GEOM::GEOM_Object_ptr theBase,
                            GEOM::GEOM_Object_ptr thePath,
                            CORBA::Boolean        IsGenerateGroups)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aPath = GetObjectImpl(thePath);

  if (aBase.IsNull() || aPath.IsNull()) return aSeq._retn();

  //Create the Pipe
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipe(aBase, aPath, IsGenerateGroups);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aBase.IsNull() || anAxis.IsNull()) return aGEOMObject._retn();

  //Create the Revolution
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aBase.IsNull() || anAxis.IsNull()) return aGEOMObject._retn();

  //Create the Revolution
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
  std::list< HANDLE_NAMESPACE(GEOM_Object) > aShapes;
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
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeFilling
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
    HANDLE_NAMESPACE(GEOM_Object) aSh = GetObjectImpl(theSeqSections[ind]);
    if (!aSh.IsNull())
      aSeqSections->Append(aSh);
  }
  if (!aSeqSections->Length())
    return aGEOMObject._retn();

  // Make shell or solid
  HANDLE_NAMESPACE(GEOM_Object) anObject =
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
GEOM::ListOfGO *GEOM_I3DPrimOperations_i::MakePipeWithDifferentSections
                           (const GEOM::ListOfGO        &theBases,
                            const GEOM::ListOfGO        &theLocations,
                                  GEOM::GEOM_Object_ptr  thePath,
                                  CORBA::Boolean         theWithContact,
                                  CORBA::Boolean         theWithCorrections,
                                  CORBA::Boolean         IsBySteps,
                                  CORBA::Boolean         IsGenerateGroups)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqLocations = new TColStd_HSequenceOfTransient;
  int ind=0, aNbBases =0,aNbLocs=0;

  //Get the shapes
  aNbBases = theBases.length();
  aNbLocs = theLocations.length();

  if (aNbLocs && aNbBases != aNbLocs)
    return aSeq._retn();

  HANDLE_NAMESPACE(GEOM_Object) aPath = GetObjectImpl(thePath);
  if (aPath.IsNull())
    return aSeq._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs)
    {
      HANDLE_NAMESPACE(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);
  }
  if (!aSeqBases->Length())
    return aSeq._retn();

  // Make pipe
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeWithDifferentSections
              (aSeqBases, aSeqLocations,
               aPath, theWithContact,
               theWithCorrections, IsBySteps, IsGenerateGroups);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}


//=============================================================================
/*!
 *  MakePipeWithShellSections
 */
//=============================================================================
GEOM::ListOfGO *GEOM_I3DPrimOperations_i::MakePipeWithShellSections
                           (const GEOM::ListOfGO        &theBases,
                            const GEOM::ListOfGO        &theSubBases,
                            const GEOM::ListOfGO        &theLocations,
                                  GEOM::GEOM_Object_ptr  thePath,
                                  CORBA::Boolean         theWithContact,
                                  CORBA::Boolean         theWithCorrections,
                                  CORBA::Boolean         IsGenerateGroups)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

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
    return aSeq._retn();

  HANDLE_NAMESPACE(GEOM_Object) aPath = GetObjectImpl(thePath);
  if (aPath.IsNull())
    return aSeq._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs) {
      HANDLE_NAMESPACE(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);

    if (aNbSubBases >= aNbBases) {
      HANDLE_NAMESPACE(GEOM_Object) aSubBase = GetObjectImpl(theSubBases[ind]);
      if (aSubBase.IsNull()) {
        aSeqSubBases->Clear();
        aNbSubBases = 0;
        continue;
      }
      aSeqSubBases->Append(aSubBase);
    }
  }
  if (!aSeqBases->Length())
    return aSeq._retn();

  // Make pipe
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeWithShellSections
                  (aSeqBases, aSeqSubBases,
                   aSeqLocations, aPath,
                   theWithContact, theWithCorrections, IsGenerateGroups);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}


//=============================================================================
/*!
 *  MakePipeShellsWithoutPath
 */
//=============================================================================
GEOM::ListOfGO *GEOM_I3DPrimOperations_i::MakePipeShellsWithoutPath
                           (const GEOM::ListOfGO &theBases,
                            const GEOM::ListOfGO &theLocations,
                                  CORBA::Boolean  IsGenerateGroups)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();
  Handle(TColStd_HSequenceOfTransient) aSeqBases = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqLocations = new TColStd_HSequenceOfTransient;
  int ind=0, aNbBases=0, aNbLocs=0;

  //Get the shapes
  aNbBases = theBases.length();
  aNbLocs = theLocations.length();

  if (aNbLocs && aNbBases != aNbLocs)
    return aSeq._retn();

  for (ind = 0; ind < aNbBases; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBases[ind]);
    if (aBase.IsNull())
      continue;
    if (aNbLocs) {
      HANDLE_NAMESPACE(GEOM_Object) aLoc = GetObjectImpl(theLocations[ind]);
      if (aLoc.IsNull())
        continue;
      aSeqLocations->Append(aLoc);
    }
    aSeqBases->Append(aBase);
  }

  if (!aSeqBases->Length())
    return aSeq._retn();

  // Make pipe
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeShellsWithoutPath
          (aSeqBases, aSeqLocations, IsGenerateGroups);

  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakePipeBiNormalAlongVector
 */
//=============================================================================
GEOM::ListOfGO *GEOM_I3DPrimOperations_i::MakePipeBiNormalAlongVector
                           (GEOM::GEOM_Object_ptr theBase,
                            GEOM::GEOM_Object_ptr thePath,
                            GEOM::GEOM_Object_ptr theVec,
                            CORBA::Boolean        IsGenerateGroups)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase);
  HANDLE_NAMESPACE(GEOM_Object) aPath = GetObjectImpl(thePath);
  HANDLE_NAMESPACE(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aBase.IsNull() || aPath.IsNull() || aVec.IsNull()) return aSeq._retn();

  //Create the Pipe
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeBiNormalAlongVector
          (aBase, aPath, aVec, IsGenerateGroups);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeThickening
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeThickening
                 (GEOM::GEOM_Object_ptr   theObject,
                  const GEOM::ListOfLong &theFacesIDs,
                  CORBA::Double           theOffset,
                  CORBA::Boolean          doCopy,
                  CORBA::Boolean          theInside)
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
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  // Get faces IDs.
  Handle(TColStd_HArray1OfInteger) aFaceIDs;
  Standard_Integer                 aNbIDs = theFacesIDs.length();
  Standard_Integer                 i;

  if (aNbIDs > 0) {
    aFaceIDs = new TColStd_HArray1OfInteger (1, aNbIDs);

    for (i = 0; i < aNbIDs; i++) {
      aFaceIDs->SetValue(i + 1, theFacesIDs[i]);
    }
  }

  //Create the thickened shape
  if (doCopy)
  {
    HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeThickening(
      aBasicObject, aFaceIDs, theOffset, doCopy, theInside);
    if (!GetOperations()->IsDone() || anObject.IsNull())
      return aGEOMObject._retn();
    
    return GetObject(anObject);
  }
  else
  {
    GetOperations()->MakeThickening(aBasicObject, aFaceIDs, theOffset, doCopy, theInside);
    
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
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aBase1 = GetObjectImpl(theBase1);
  HANDLE_NAMESPACE(GEOM_Object) aBase2 = GetObjectImpl(theBase2);

  if (aShape.IsNull() || aBase1.IsNull() || aBase2.IsNull()) return aGEOMObject._retn();

  // Create the Path
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->RestorePath(aShape, aBase1, aBase2);
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
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  Handle(TColStd_HSequenceOfTransient) aSeqBases1 = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aSeqBases2 = new TColStd_HSequenceOfTransient;

  int ind;
  int aNbBases1 = theBase1.length();
  int aNbBases2 = theBase2.length();

  for (ind = 0; ind < aNbBases1; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase1[ind]);
    if (!aBase.IsNull())
      aSeqBases1->Append(aBase);
  }
  for (ind = 0; ind < aNbBases2; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aBase = GetObjectImpl(theBase2[ind]);
    if (!aBase.IsNull())
      aSeqBases2->Append(aBase);
  }

  if (!aSeqBases1->Length() || !aSeqBases2->Length())
    return aGEOMObject._retn();

  // Create the Path
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->RestorePath(aShape, aSeqBases1, aSeqBases2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
