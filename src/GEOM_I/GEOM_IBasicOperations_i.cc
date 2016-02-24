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

#include "GEOM_IBasicOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IBasicOperations_i::GEOM_IBasicOperations_i (PortableServer::POA_ptr thePOA,
                                                  GEOM::GEOM_Gen_ptr theEngine,
                                                  ::GEOMImpl_IBasicOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IBasicOperations_i::GEOM_IBasicOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IBasicOperations_i::~GEOM_IBasicOperations_i()
{
  MESSAGE("GEOM_IBasicOperations_i::~GEOM_IBasicOperations_i");
}


//=============================================================================
/*!
 *  MakePointXYZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointXYZ
  (CORBA::Double theX, CORBA::Double theY, CORBA::Double theZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakePointXYZ(theX, theY, theZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointWithReference
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointWithReference
  (GEOM::GEOM_Object_ptr theReference, CORBA::Double theX, CORBA::Double theY, CORBA::Double theZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theReference);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointWithReference(aReference, theX, theY, theZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnLinesIntersection
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnLinesIntersection
                  (GEOM::GEOM_Object_ptr theLine1,  GEOM::GEOM_Object_ptr theLine2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Lines
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(theLine1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theLine2);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointOnLinesIntersection(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnCurve
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnCurve
                  (GEOM::GEOM_Object_ptr theCurve,
                   CORBA::Double         theParameter,
                   CORBA::Boolean        takeOrientationIntoAccount)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference curve
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theCurve);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakePointOnCurve
        (aReference, theParameter, takeOrientationIntoAccount);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnCurveByLength
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnCurveByLength
                  (GEOM::GEOM_Object_ptr theCurve,  
		   CORBA::Double         theLength,
		   GEOM::GEOM_Object_ptr theStartPoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference curve
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theCurve);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Get the reference point (can be NULL)
  HANDLE_NAMESPACE(GEOM_Object) aRefPoint;
  if (!CORBA::is_nil(theStartPoint)) {
    aRefPoint = GetObjectImpl(theStartPoint);
  }

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointOnCurveByLength(aReference, theLength, aRefPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnCurveByCoord
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnCurveByCoord
                  (GEOM::GEOM_Object_ptr theCurve, 
                   CORBA::Double theXParameter,
                   CORBA::Double theYParameter,
                   CORBA::Double theZParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference curve
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theCurve);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointOnCurveByCoord(aReference, theXParameter,
                                             theYParameter, theZParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnSurface
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnSurface
                                             (GEOM::GEOM_Object_ptr theSurface,
                                              CORBA::Double theUParameter,
                                              CORBA::Double theVParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference surface
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theSurface);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointOnSurface(aReference, theUParameter, theVParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnSurfaceByCoord
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnSurfaceByCoord
                                             (GEOM::GEOM_Object_ptr theSurface,
                                              CORBA::Double theXParameter,
                                              CORBA::Double theYParameter,
                                              CORBA::Double theZParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference surface
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theSurface);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePointOnSurfaceByCoord(aReference, theXParameter,
                                               theYParameter, theZParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnFace (GEOM::GEOM_Object_ptr theFace)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference face
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theFace);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakePointOnFace(aReference);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTangentOnCurve
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeTangentOnCurve
                  (GEOM::GEOM_Object_ptr theCurve,  CORBA::Double theParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference curve
  HANDLE_NAMESPACE(GEOM_Object) aReference = GetObjectImpl(theCurve);
  if (aReference.IsNull()) return aGEOMObject._retn();

  //Create the vector
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeTangentOnCurve(aReference, theParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeVectorDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeVectorDXDYDZ
  (CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Vector

  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeVectorDXDYDZ(theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeVectorTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeVectorTwoPnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(thePnt2);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the vector
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeVectorTwoPnt(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakeLine
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeLine
                   (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theDir)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theDir);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the Line
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeLine(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeLineTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeLineTwoPnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(thePnt2);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the Line
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeLineTwoPnt(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeLineTwoFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeLineTwoFaces
                 (GEOM::GEOM_Object_ptr theFace1, GEOM::GEOM_Object_ptr theFace2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(theFace1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theFace2);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the Line
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeLineTwoFaces(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakePlanePntVec
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlanePntVec
                 (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                  CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the references
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(thePnt);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theVec);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePlanePntVec(aRef1, aRef2, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlaneThreePnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlaneThreePnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2,
                  GEOM::GEOM_Object_ptr thePnt3, CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(thePnt2);
  HANDLE_NAMESPACE(GEOM_Object) aRef3 = GetObjectImpl(thePnt3);
  if (aRef1.IsNull() || aRef2.IsNull() || aRef3.IsNull())
    return aGEOMObject._retn();

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePlaneThreePnt(aRef1, aRef2, aRef3, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlaneFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlaneFace
                     (GEOM::GEOM_Object_ptr theFace, CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference face
  HANDLE_NAMESPACE(GEOM_Object) aRef = GetObjectImpl(theFace);
  if (aRef.IsNull()) return aGEOMObject._retn();

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePlaneFace(aRef, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlane2Vec
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlane2Vec
                 (GEOM::GEOM_Object_ptr theVec1, GEOM::GEOM_Object_ptr theVec2,
                  CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the references
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(theVec1);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theVec2);
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePlane2Vec(aRef1, aRef2, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlaneLCS
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlaneLCS
                 (GEOM::GEOM_Object_ptr theLCS, CORBA::Double theTrimSize,
		  CORBA::Double theOrientation)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the references
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(theLCS);

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePlaneLCS(aRef1, theTrimSize, theOrientation);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeMarker
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeMarker
  (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
   CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
   CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeMarker(theOX , theOY , theOZ,
                                                             theXDX, theXDY, theXDZ,
                                                             theYDX, theYDY, theYDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeMarkerFromShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeMarkerFromShape
                                              (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the referenced object
  HANDLE_NAMESPACE(GEOM_Object) aRef = GetObjectImpl(theShape);
  if (aRef.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeMarkerFromShape(aRef);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeMarkerPntTwoVec
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeMarkerPntTwoVec
                                              (GEOM::GEOM_Object_ptr theOrigin,
                                               GEOM::GEOM_Object_ptr theXVec,
                                               GEOM::GEOM_Object_ptr theYVec)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the referenced objects
  HANDLE_NAMESPACE(GEOM_Object) aRef1 = GetObjectImpl(theOrigin);
  HANDLE_NAMESPACE(GEOM_Object) aRef2 = GetObjectImpl(theXVec);
  HANDLE_NAMESPACE(GEOM_Object) aRef3 = GetObjectImpl(theYVec);
  if (aRef1.IsNull() || aRef2.IsNull() || aRef3.IsNull()) return aGEOMObject._retn();

  //Create the point
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeMarkerPntTwoVec(aRef1,
                                                                      aRef2,
								      aRef3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTangentPlaneOnFace
 */
//=============================================================================

GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeTangentPlaneOnFace
                     (GEOM::GEOM_Object_ptr theFace,
                      CORBA::Double theParameterU,
                      CORBA::Double theParameterV,
                      CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference face
  HANDLE_NAMESPACE(GEOM_Object) aRef = GetObjectImpl(theFace);
  if (aRef.IsNull()) return aGEOMObject._retn();

  //Create the plane
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeTangentPlaneOnFace(aRef, theParameterU,theParameterV,theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
