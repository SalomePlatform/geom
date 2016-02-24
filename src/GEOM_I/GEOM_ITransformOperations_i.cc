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

#include "GEOM_ITransformOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TCollection_AsciiString.hxx>
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#define SUBSHAPE_ERROR "Sub shape cannot be transformed"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_ITransformOperations_i::GEOM_ITransformOperations_i (PortableServer::POA_ptr thePOA,
                                                          GEOM::GEOM_Gen_ptr theEngine,
                                                          ::GEOMImpl_ITransformOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_ITransformOperations_i::GEOM_ITransformOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_ITransformOperations_i::~GEOM_ITransformOperations_i()
{
  MESSAGE("GEOM_ITransformOperations_i::~GEOM_ITransformOperations_i");
}


//=============================================================================
/*!
 *  TranslateTwoPoints
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateTwoPoints
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the first point of translation
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point of translation
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->TranslateTwoPoints(anObject, aPoint1, aPoint2);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  TranslateTwoPointsCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateTwoPointsCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the first point of translation
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point of translation
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Create the translated shape
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->TranslateTwoPointsCopy(aBasicObject, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  TranslateDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateDXDYDZ
                      (GEOM::GEOM_Object_ptr theObject,
                       CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->TranslateDXDYDZ(anObject, theDX, theDY, theDZ);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  TranslateDXDYDZCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateDXDYDZCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Create the translated shape
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->TranslateDXDYDZCopy(aBasicObject, theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  TranslateVector
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateVector
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  if (aVector.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->TranslateVector(anObject, aVector);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  TranslateVectorCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateVectorCopy
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  if (aVector.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->TranslateVectorCopy(aBasicObject, aVector);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  TranslateVectorDistance
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateVectorDistance
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector,
                                             CORBA::Double theDistance,
                                             CORBA::Boolean theCopy)
{
  GEOM::GEOM_Object_var aGEOMObject;
  GetOperations()->SetNotDone(); //Set a not done flag

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theCopy && !theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  if (!theCopy)
    aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  if (aVector.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  if (theCopy) {
    HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->
      TranslateVectorDistance(aBasicObject, aVector, theDistance, theCopy);
    if (!GetOperations()->IsDone() || anObject.IsNull())
      return aGEOMObject._retn();

    return GetObject(anObject);
  }

  GetOperations()->TranslateVectorDistance(aBasicObject, aVector, theDistance, theCopy);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  Rotate
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::Rotate (GEOM::GEOM_Object_ptr theObject,
                                                           GEOM::GEOM_Object_ptr theAxis,
                                                           CORBA::Double theAngle)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the axis of revolution
  HANDLE_NAMESPACE(GEOM_Object) anAxis = GetObjectImpl(theAxis);
  if (anAxis.IsNull()) return aGEOMObject._retn();

  //Perform the rotation
  GetOperations()->Rotate(anObject, anAxis, theAngle);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  RotateCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::RotateCopy (GEOM::GEOM_Object_ptr theObject,
                                                               GEOM::GEOM_Object_ptr theAxis,
                                                               CORBA::Double theAngle)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the axis of rotation
  HANDLE_NAMESPACE(GEOM_Object) anAxis = GetObjectImpl(theAxis);
  if (anAxis.IsNull()) return aGEOMObject._retn();

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->RotateCopy(aBasicObject, anAxis, theAngle);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MirrorPlane
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorPlane
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePlane)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the plane
  HANDLE_NAMESPACE(GEOM_Object) aPlane = GetObjectImpl(thePlane);
  if (aPlane.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorPlane(anObject, aPlane);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  MirrorPlaneCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorPlaneCopy
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePlane)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aPlane = GetObjectImpl(thePlane);
  if (aPlane.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MirrorPlaneCopy(aBasicObject, aPlane);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MirrorAxis
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorAxis
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theAxis)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the axis
  HANDLE_NAMESPACE(GEOM_Object) aAxis = GetObjectImpl(theAxis);
  if (aAxis.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorAxis(anObject, aAxis);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  MirrorAxisCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorAxisCopy
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theAxis)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aAxis = GetObjectImpl(theAxis);
  if (aAxis.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MirrorAxisCopy(aBasicObject, aAxis);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MirrorPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorPoint
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if (aPoint.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorPoint(anObject, aPoint);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  MirrorPointCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MirrorPointCopy
                                            (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if (aPoint.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MirrorPointCopy(aBasicObject, aPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  OffsetShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::OffsetShape
                                             (GEOM::GEOM_Object_ptr theObject,
                                              CORBA::Double theOffset)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Create the offset shape
  GetOperations()->OffsetShape(aBasicObject, theOffset);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  OffsetShapeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::OffsetShapeCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              CORBA::Double theOffset)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Create the offset shape
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->OffsetShapeCopy(aBasicObject, theOffset);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ProjectShapeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ProjectShapeCopy
                                             (GEOM::GEOM_Object_ptr theSource,
                                              GEOM::GEOM_Object_ptr theTarget)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the input objects
  HANDLE_NAMESPACE(GEOM_Object) aSource = GetObjectImpl(theSource);
  HANDLE_NAMESPACE(GEOM_Object) aTarget = GetObjectImpl(theTarget);
  if (aSource.IsNull() || aTarget.IsNull()) return aGEOMObject._retn();

  //Create the projection
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->ProjectShapeCopy(aSource, aTarget);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ProjectPointOnWire
 */
//=============================================================================
CORBA::Double GEOM_ITransformOperations_i::ProjectPointOnWire
                               (GEOM::GEOM_Object_ptr thePoint,
                                GEOM::GEOM_Object_ptr theWire,
                                GEOM::GEOM_Object_out thePointOnEdge,
                                CORBA::Long&          theEdgeInWireIndex)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  HANDLE_NAMESPACE(GEOM_Object) aWire = GetObjectImpl(theWire);

  if (aPoint.IsNull() || aWire.IsNull()) {
    return -1.0;
  }

  HANDLE_NAMESPACE(GEOM_Object) aPointOnEdge;
  Standard_Integer anEdgeIndex;
  CORBA::Double aResult = GetOperations()->ProjectPointOnWire
    (aPoint, aWire, aPointOnEdge, anEdgeIndex);

  if (!aPointOnEdge.IsNull()) {
    thePointOnEdge = GetObject(aPointOnEdge);
    theEdgeInWireIndex = anEdgeIndex;
  }

  return aResult;
}

//=============================================================================
/*!
 *  ScaleShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ScaleShape
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint,
                                              CORBA::Double         theFactor)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theObject->_is_nil()) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  HANDLE_NAMESPACE(GEOM_Object) aPoint;
  if (!thePoint->_is_nil()) {
    aPoint = GetObjectImpl(thePoint);
    if (aPoint.IsNull()) return aGEOMObject._retn();
  }

  //Perform the scale
  GetOperations()->ScaleShape(anObject, aPoint, theFactor);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return  aGEOMObject._retn();
}

//=============================================================================
/*!
 *  ScaleShapeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ScaleShapeCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint,
                                              CORBA::Double         theFactor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  HANDLE_NAMESPACE(GEOM_Object) aPoint;
  if (!thePoint->_is_nil()) {
    aPoint = GetObjectImpl(thePoint);
    if (aPoint.IsNull()) return aGEOMObject._retn();
  }

  //Perform the scale
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->ScaleShapeCopy(aBasicObject, aPoint, theFactor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ScaleShapeAlongAxes
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ScaleShapeAlongAxes
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint,
                                              CORBA::Double         theFactorX,
                                              CORBA::Double         theFactorY,
                                              CORBA::Double         theFactorZ)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theObject->_is_nil()) return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  HANDLE_NAMESPACE(GEOM_Object) aPoint;
  if (!thePoint->_is_nil()) {
    aPoint = GetObjectImpl(thePoint);
    if (aPoint.IsNull()) return aGEOMObject._retn();
  }

  //Perform the scale
  GetOperations()->ScaleShapeAlongAxes
    (anObject, aPoint, theFactorX, theFactorY, theFactorZ, /*doCopy*/false);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return  aGEOMObject._retn();
}

//=============================================================================
/*!
 *  ScaleShapeAlongAxesCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ScaleShapeAlongAxesCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePoint,
                                              CORBA::Double         theFactorX,
                                              CORBA::Double         theFactorY,
                                              CORBA::Double         theFactorZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  HANDLE_NAMESPACE(GEOM_Object) aPoint;
  if (!thePoint->_is_nil()) {
    aPoint = GetObjectImpl(thePoint);
    if (aPoint.IsNull()) return aGEOMObject._retn();
  }

  //Perform the scale
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->ScaleShapeAlongAxes
    (aBasicObject, aPoint, theFactorX, theFactorY, theFactorZ, /*doCopy*/true);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  PositionShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::PositionShape
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theStartLCS,
                                              GEOM::GEOM_Object_ptr theEndLCS)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObject) || CORBA::is_nil(theEndLCS))
    return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the Start LCS (may be NULL for positioning from global LCS)
  HANDLE_NAMESPACE(GEOM_Object) aStartLCS;
  if (!CORBA::is_nil(theStartLCS)) {
    aStartLCS = GetObjectImpl(theStartLCS);
    if (aStartLCS.IsNull()) return aGEOMObject._retn();
  }

  //Get the End LCS
  HANDLE_NAMESPACE(GEOM_Object) aEndLCS = GetObjectImpl(theEndLCS);
  if (aEndLCS.IsNull()) return aGEOMObject._retn();

  //Perform the Position
  GetOperations()->PositionShape(anObject, aStartLCS, aEndLCS);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return  aGEOMObject._retn();
}

//=============================================================================
/*!
 *  PositionShapeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::PositionShapeCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theStartLCS,
                                              GEOM::GEOM_Object_ptr theEndLCS)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the Start LCS (may be NULL for positioning from global LCS)
  HANDLE_NAMESPACE(GEOM_Object) aStartLCS;
  if (!CORBA::is_nil(theStartLCS)) {
    aStartLCS = GetObjectImpl(theStartLCS);
    if (aStartLCS.IsNull()) return aGEOMObject._retn();
  }

  //Get the End LCS
  HANDLE_NAMESPACE(GEOM_Object) aEndLCS = GetObjectImpl(theEndLCS);
  if (aEndLCS.IsNull()) return aGEOMObject._retn();

  //Perform the position
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->PositionShapeCopy(aBasicObject, aStartLCS, aEndLCS);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  PositionAlongPath
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::PositionAlongPath
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr thePath,
                                              CORBA::Double         theDistance,
                                              CORBA::Boolean        theCopy,
                                              CORBA::Boolean        theReverse)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the basic object
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the path object
  HANDLE_NAMESPACE(GEOM_Object) aPathObject = GetObjectImpl(thePath);
  if (aPathObject.IsNull()) return aGEOMObject._retn();

  //Perform the position
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->PositionAlongPath(aBasicObject, aPathObject, theDistance, theCopy, theReverse);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  if (!theCopy) {
    // Update GUI.
    UpdateGUIForObject(theObject);
  }

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiTranslate1D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiTranslate1D
                           (GEOM::GEOM_Object_ptr theObject,
                            GEOM::GEOM_Object_ptr theVector,
                            CORBA::Double theStep, CORBA::Long theNbTimes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DX by default

  //Perform the translation
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->Translate1D(aBasicObject, aVector, theStep, theNbTimes);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiTranslate2D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiTranslate2D (GEOM::GEOM_Object_ptr theObject,
                                                                     GEOM::GEOM_Object_ptr theVector1,
                                                                     CORBA::Double theStep1,
                                                                     CORBA::Long theNbTimes1,
                                                                     GEOM::GEOM_Object_ptr theVector2,
                                                                     CORBA::Double theStep2,
                                                                     CORBA::Long theNbTimes2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector1 of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector1 = GetObjectImpl(theVector1);
  //if (aVector1.IsNull()) return aGEOMObject._retn(); // DX by default

  //Get the vector2 of translation
  HANDLE_NAMESPACE(GEOM_Object) aVector2 = GetObjectImpl(theVector2);
  //if (aVector2.IsNull()) return aGEOMObject._retn(); // DY by default

  //Perform the translation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Translate2D
    (aBasicObject, aVector1, theStep1, theNbTimes1, aVector2, theStep2, theNbTimes2);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiRotate1D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
                                                                  GEOM::GEOM_Object_ptr theVector,
                                                                  CORBA::Long theNbTimes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DZ by default

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Rotate1D(aBasicObject, aVector, theNbTimes);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiRotate1DByStep
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiRotate1DByStep (GEOM::GEOM_Object_ptr theObject,
                                                                        GEOM::GEOM_Object_ptr theVector,
                                                                        CORBA::Double theAngleStep,
                                                                        CORBA::Long theNbSteps)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DZ by default

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Rotate1D(aBasicObject, aVector, theAngleStep, theNbSteps);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiRotate2DNbTimes
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiRotate2DNbTimes (GEOM::GEOM_Object_ptr theObject,
                                                                         GEOM::GEOM_Object_ptr theVector,
                                                                         CORBA::Long theNbObjects,
                                                                         CORBA::Double theRadialStep,
                                                                         CORBA::Long theNbSteps)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DZ by default

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Rotate2D
    (aBasicObject, aVector, theNbObjects, theRadialStep, theNbSteps);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiRotate2DByStep
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiRotate2DByStep (GEOM::GEOM_Object_ptr theObject,
                                                                        GEOM::GEOM_Object_ptr theVector,
                                                                        CORBA::Double theAngle,
                                                                        CORBA::Long theNbTimes1,
                                                                        CORBA::Double theStep,
                                                                        CORBA::Long theNbTimes2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DZ by default

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Rotate2D
    (aBasicObject, aVector, theAngle, theNbTimes1, theStep, theNbTimes2);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiRotate2D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiRotate2D (GEOM::GEOM_Object_ptr theObject,
                                                                  GEOM::GEOM_Object_ptr theVector,
                                                                  CORBA::Double theAngle,
                                                                  CORBA::Long theNbTimes1,
                                                                  CORBA::Double theStep,
                                                                  CORBA::Long theNbTimes2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::GEOM_Object_var aGEOMObject;

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);
  //if (aVector.IsNull()) return aGEOMObject._retn(); // DZ by default

  double anAngle = M_PI * theAngle / 180.;

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Rotate2D
    (aBasicObject, aVector, anAngle, theNbTimes1, theStep, theNbTimes2);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  RotateThreePoints
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::RotateThreePoints
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theCentPoint,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject))
    return aGEOMObject._retn();

  //check if the object is a sub-shape
  if (!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the central point of rotation
  HANDLE_NAMESPACE(GEOM_Object) aCentPoint = GetObjectImpl(theCentPoint);
  if (aCentPoint.IsNull()) return aGEOMObject._retn();

  //Get the first point
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->RotateThreePoints(anObject, aCentPoint, aPoint1, aPoint2);

  // Update GUI.
  UpdateGUIForObject(theObject);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  RotateThreePointsCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::RotateThreePointsCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theCentPoint,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theObject);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the central point of rotation
  HANDLE_NAMESPACE(GEOM_Object) aCentPoint = GetObjectImpl(theCentPoint);
  if (aCentPoint.IsNull()) return aGEOMObject._retn();

  //Get the first point
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Perform the rotation
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->RotateThreePointsCopy(aBasicObject, aCentPoint, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  TransformLikeOtherCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TransformLikeOtherCopy
                                             (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theSample)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the sample object
  HANDLE_NAMESPACE(GEOM_Object) aSample = GetObjectImpl(theSample);
  if (aSample.IsNull()) return aGEOMObject._retn();

  //Perform the transformation
  HANDLE_NAMESPACE(GEOM_Object) aResObject =
    GetOperations()->TransformLikeOtherCopy(anObject, aSample);
  if (!GetOperations()->IsDone() || aResObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(aResObject);
}

//=============================================================================
/*!
 *  RecomputeObject
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::RecomputeObject
                                             (GEOM::GEOM_Object_ptr theObject)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  GEOM::GEOM_Object_var aGEOMObject;

  if (CORBA::is_nil(theObject)) return aGEOMObject._retn();

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Perform the recomputation
  HANDLE_NAMESPACE(GEOM_Function) aLastFunction = anObject->GetLastFunction();
  if (aLastFunction.IsNull()) return aGEOMObject._retn();
  GetOperations()->GetSolver()->ComputeFunction(aLastFunction);

  return aGEOMObject._retn();
}

//=============================================================================
/*!
 *  MakeProjectionOnCylinder
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MakeProjectionOnCylinder
                                   (GEOM::GEOM_Object_ptr theObject,
                                    CORBA::Double         theRadius,
                                    CORBA::Double         theStartAngle,
                                    CORBA::Double         theAngleLength,
                                    CORBA::Double         theAngleRotation)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetObjectImpl(theObject);

  if (anObject.IsNull()) {
    return aGEOMObject._retn();
  }

  //Perform the transformation
  HANDLE_NAMESPACE(GEOM_Object) aResObject = GetOperations()->MakeProjectionOnCylinder
    (anObject, theRadius, theStartAngle, theAngleLength, theAngleRotation);

  if (!GetOperations()->IsDone() || aResObject.IsNull()) {
    return aGEOMObject._retn();
  }

  return GetObject(aResObject);
}
