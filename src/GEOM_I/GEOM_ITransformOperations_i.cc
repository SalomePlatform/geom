using namespace std; 

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
  
  if (thePoint1 == NULL || thePoint2 == NULL || theObject == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the first point of translation
  Handle(GEOM_Object) aPoint1 =
    GetOperations()->GetEngine()->GetObject(thePoint1->GetStudyID(), thePoint1->GetEntry());
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point of translation
  Handle(GEOM_Object) aPoint2 =
    GetOperations()->GetEngine()->GetObject(thePoint2->GetStudyID(), thePoint2->GetEntry());
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->TranslateTwoPoints(anObject, aPoint1, aPoint2);

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

  if (thePoint1 == NULL || thePoint2 == NULL || theObject == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the first point of translation
  Handle(GEOM_Object) aPoint1 =
    GetOperations()->GetEngine()->GetObject(thePoint1->GetStudyID(), thePoint1->GetEntry());
  if (aPoint1.IsNull()) return aGEOMObject._retn();

  //Get the second point of translation
  Handle(GEOM_Object) aPoint2 =
    GetOperations()->GetEngine()->GetObject(thePoint2->GetStudyID(), thePoint2->GetEntry());
  if (aPoint2.IsNull()) return aGEOMObject._retn();

  //Create the translated shape
  Handle(GEOM_Object) anObject =
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
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject, 
								    CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  //Set a not done flag
  GetOperations()->SetNotDone(); 
  GEOM::GEOM_Object_var aGEOMObject ;

  if (theObject == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Perform the translation
  GetOperations()->TranslateDXDYDZ(anObject, theDX, theDY, theDZ);

  return aGEOMObject._retn();
}


//=============================================================================
/*!
 *  TranslateDXDYDZCopy
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::TranslateDXDYDZCopy
                                             (GEOM::GEOM_Object_ptr theObject, CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theObject == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();



  //Create the translated shape
  Handle(GEOM_Object) anObject =
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

  if (theObject == NULL || theVector == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

   aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  Handle(GEOM_Object) aVector =
    GetOperations()->GetEngine()->GetObject(theVector->GetStudyID(), theVector->GetEntry());
  if (aVector.IsNull()) return aGEOMObject._retn();
  
  //Perform the translation
  GetOperations()->TranslateVector(anObject, aVector);   

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

  if (theObject == NULL || theVector == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  Handle(GEOM_Object) aVector =
    GetOperations()->GetEngine()->GetObject(theVector->GetStudyID(), theVector->GetEntry());
  if (aVector.IsNull()) return aGEOMObject._retn();
  
  //Perform the translation
  Handle(GEOM_Object) anObject = GetOperations()->TranslateVectorCopy(aBasicObject, aVector);   
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);  
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

  if (theObject == NULL || theAxis == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Get the object itself
  Handle(GEOM_Object) anObject = GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the axis of revolution
  Handle(GEOM_Object) anAxis =
    GetOperations()->GetEngine()->GetObject(theAxis->GetStudyID(), theAxis->GetEntry());
  if (anAxis.IsNull()) return aGEOMObject._retn();
  
  //Perform the rotation
  GetOperations()->Rotate(anObject, anAxis, theAngle);   

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

  if (theObject == NULL || theAxis == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the axis of rotation
  Handle(GEOM_Object) anAxis =
    GetOperations()->GetEngine()->GetObject(theAxis->GetStudyID(), theAxis->GetEntry());
  if (anAxis.IsNull()) return aGEOMObject._retn();
  
  //Perform the rotation
  Handle(GEOM_Object) anObject = GetOperations()->RotateCopy(aBasicObject, anAxis, theAngle);   
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

  if (theObject == NULL || thePlane == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the plane
  Handle(GEOM_Object) aPlane =
    GetOperations()->GetEngine()->GetObject(thePlane->GetStudyID(), thePlane->GetEntry());
  if (aPlane.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorPlane(anObject, aPlane);   

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

  if (theObject == NULL || thePlane == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  Handle(GEOM_Object) aPlane =
    GetOperations()->GetEngine()->GetObject(thePlane->GetStudyID(), thePlane->GetEntry());
  if (aPlane.IsNull()) return aGEOMObject._retn();
  
  //Perform the mirror
  Handle(GEOM_Object) anObject = GetOperations()->MirrorPlaneCopy(aBasicObject, aPlane);   
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

  if (theObject == NULL || theAxis == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the axis
  Handle(GEOM_Object) aAxis =
    GetOperations()->GetEngine()->GetObject(theAxis->GetStudyID(), theAxis->GetEntry());
  if (aAxis.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorAxis(anObject, aAxis);   

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

  if (theObject == NULL || theAxis == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  Handle(GEOM_Object) aAxis =
    GetOperations()->GetEngine()->GetObject(theAxis->GetStudyID(), theAxis->GetEntry());
  if (aAxis.IsNull()) return aGEOMObject._retn();
  
  //Perform the mirror
  Handle(GEOM_Object) anObject = GetOperations()->MirrorAxisCopy(aBasicObject, aAxis);   
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

  if (theObject == NULL || thePoint == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  Handle(GEOM_Object) aPoint =
    GetOperations()->GetEngine()->GetObject(thePoint->GetStudyID(), thePoint->GetEntry());
  if (aPoint.IsNull()) return aGEOMObject._retn();

  //Perform the mirror
  GetOperations()->MirrorPoint(anObject, aPoint);   

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

  if (theObject == NULL || thePoint == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector of translation
  Handle(GEOM_Object) aPoint =
    GetOperations()->GetEngine()->GetObject(thePoint->GetStudyID(), thePoint->GetEntry());
  if (aPoint.IsNull()) return aGEOMObject._retn();
  
  //Perform the mirror
  Handle(GEOM_Object) anObject = GetOperations()->MirrorPointCopy(aBasicObject, aPoint);   
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

  if (theObject == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }


  //Get the basic object
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Create the offset shape
  GetOperations()->OffsetShape(aBasicObject, theOffset);

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

  if (theObject == NULL) return aGEOMObject._retn();

  //Get the basic object
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Create the offset shape
  Handle(GEOM_Object) anObject = GetOperations()->OffsetShapeCopy(aBasicObject, theOffset);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  ScaleShape
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::ScaleShape
                                             (GEOM::GEOM_Object_ptr theObject,
					      GEOM::GEOM_Object_ptr thePoint,
					      CORBA::Double theFactor)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theObject);

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePoint == NULL || theObject == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  Handle(GEOM_Object) aPoint =
    GetOperations()->GetEngine()->GetObject(thePoint->GetStudyID(), thePoint->GetEntry());
  if (aPoint.IsNull()) return aGEOMObject._retn();

  //Perform the scale
  GetOperations()->ScaleShape(anObject, aPoint, theFactor);

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
					      CORBA::Double theFactor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePoint == NULL || theObject == NULL) return aGEOMObject._retn();

  //Get the basic object
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the point
  Handle(GEOM_Object) aPoint =
    GetOperations()->GetEngine()->GetObject(thePoint->GetStudyID(), thePoint->GetEntry());
  if (aPoint.IsNull()) return aGEOMObject._retn();

  //Perform the scale
  Handle(GEOM_Object) anObject =
    GetOperations()->ScaleShapeCopy(aBasicObject, aPoint, theFactor);
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

  if (theObject == NULL || theStartLCS == NULL || theEndLCS == NULL)
    return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the basic object
  Handle(GEOM_Object) anObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return aGEOMObject._retn();

  //Get the Start LCS
  Handle(GEOM_Object) aStartLCS =
    GetOperations()->GetEngine()->GetObject(theStartLCS->GetStudyID(), theStartLCS->GetEntry());
  if (aStartLCS.IsNull()) return aGEOMObject._retn();

  //Get the End LCS
  Handle(GEOM_Object) aEndLCS =
    GetOperations()->GetEngine()->GetObject(theEndLCS->GetStudyID(), theEndLCS->GetEntry());
  if (aEndLCS.IsNull()) return aGEOMObject._retn();

  //Perform the Position
  GetOperations()->PositionShape(anObject, aStartLCS, aEndLCS);

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

  if (theObject == NULL || theStartLCS == NULL || theEndLCS == NULL)
    return aGEOMObject._retn();

  //Get the basic object
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the Start LCS
  Handle(GEOM_Object) aStartLCS =
    GetOperations()->GetEngine()->GetObject(theStartLCS->GetStudyID(), theStartLCS->GetEntry());
  if (aStartLCS.IsNull()) return aGEOMObject._retn();

  //Get the End LCS
  Handle(GEOM_Object) aEndLCS =
    GetOperations()->GetEngine()->GetObject(theEndLCS->GetStudyID(), theEndLCS->GetEntry());
  if (aEndLCS.IsNull()) return aGEOMObject._retn();

  //Perform the position
  Handle(GEOM_Object) anObject =
    GetOperations()->PositionShapeCopy(aBasicObject, aStartLCS, aEndLCS);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MultiTranslate1D
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_ITransformOperations_i::MultiTranslate1D (GEOM::GEOM_Object_ptr theObject, GEOM::GEOM_Object_ptr theVector, CORBA::Double theStep, CORBA::Long theNbTimes)
{
  GEOM::GEOM_Object_var aGEOMObject;

   //Set a not done flag
  GetOperations()->SetNotDone(); 

  if (theObject == NULL || theVector == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
     GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
     return aGEOMObject._retn();
   }
    
   //Get the object itself
   Handle(GEOM_Object) aBasicObject = GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
   if (aBasicObject.IsNull()) return aGEOMObject._retn();
    
   //Get the vector of translation
   Handle(GEOM_Object) aVector =  GetOperations()->GetEngine()->GetObject(theVector->GetStudyID(), theVector->GetEntry());
   if (aVector.IsNull()) return aGEOMObject._retn();
    
   //Perform the translation
   Handle(GEOM_Object) anObject  = GetOperations()->Translate1D(aBasicObject, aVector, theStep, theNbTimes);   
   if (!GetOperations()->IsDone() || anObject.IsNull())  return aGEOMObject._retn();
    
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
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone(); 
  
  if (theObject == NULL || theVector1 == NULL || theVector2 == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) aBasicObject = GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the vector1 of translation
  Handle(GEOM_Object) aVector1 = GetOperations()->GetEngine()->GetObject(theVector1->GetStudyID(), theVector1->GetEntry());
  if (aVector1.IsNull()) return aGEOMObject._retn();

 //Get the vector2 of translation
  Handle(GEOM_Object) aVector2 = GetOperations()->GetEngine()->GetObject(theVector2->GetStudyID(), theVector2->GetEntry());
  if (aVector2.IsNull()) return aGEOMObject._retn();
  
  //Perform the translation
  Handle(GEOM_Object) anObject = GetOperations()->Translate2D(aBasicObject, aVector1, theStep1, theNbTimes1, aVector2, theStep2, theNbTimes2);   
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
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone(); 

  if (theObject == NULL || theVector == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) aBasicObject =
    GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Get the a directon of rotation
  Handle(GEOM_Object) aVector = GetOperations()->GetEngine()->GetObject(theVector->GetStudyID(), theVector->GetEntry());
  if (aVector.IsNull()) return aGEOMObject._retn();

  
  //Perform the rotation
  Handle(GEOM_Object) anObject = GetOperations()->Rotate1D(aBasicObject, aVector, theNbTimes);   
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
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone(); 

  if (theObject == NULL || theVector == NULL) return aGEOMObject._retn();

  //check if the object is a subshape
  if(!theObject->IsMainShape()) {
    GetOperations()->SetErrorCode(SUBSHAPE_ERROR);
    return aGEOMObject._retn();
  }

  //Get the object itself
  Handle(GEOM_Object) aBasicObject = GetOperations()->GetEngine()->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (aBasicObject.IsNull()) return aGEOMObject._retn();


  //Get the a directon of rotation
  Handle(GEOM_Object) aVector = GetOperations()->GetEngine()->GetObject(theVector->GetStudyID(), theVector->GetEntry());
  if (aVector.IsNull()) return aGEOMObject._retn();

  
  //Perform the rotation
  Handle(GEOM_Object) anObject = GetOperations()->Rotate2D(aBasicObject, aVector, theAngle, theNbTimes1, theStep, theNbTimes2);   
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);  
}
