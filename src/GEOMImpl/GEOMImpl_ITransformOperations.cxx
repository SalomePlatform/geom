//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include <Standard_Stream.hxx>

#include <GEOMImpl_ITransformOperations.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>

#include <GEOMImpl_TranslateDriver.hxx>
#include <GEOMImpl_MirrorDriver.hxx>
#include <GEOMImpl_OffsetDriver.hxx>
#include <GEOMImpl_ScaleDriver.hxx>
#include <GEOMImpl_RotateDriver.hxx>
#include <GEOMImpl_PositionDriver.hxx>

#include <GEOMImpl_ITranslate.hxx>
#include <GEOMImpl_IMirror.hxx>
#include <GEOMImpl_IOffset.hxx>
#include <GEOMImpl_IScale.hxx>
#include <GEOMImpl_IRotate.hxx>
#include <GEOMImpl_IPosition.hxx>

#include <GEOMImpl_Types.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOMImpl_ITransformOperations::GEOMImpl_ITransformOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_ITransformOperations::GEOMImpl_ITransformOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOMImpl_ITransformOperations::~GEOMImpl_ITransformOperations()
{
  MESSAGE("GEOMImpl_ITransformOperations::~GEOMImpl_ITransformOperations");
}


//=============================================================================
/*!
 *  TranslateTwoPoints
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateTwoPoints
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint1, Handle(GEOM_Object) thePoint2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePoint1.IsNull() || thePoint2.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  // Get last functions of the arguments
  Handle(GEOM_Function) aP1F = thePoint1->GetLastFunction();
  Handle(GEOM_Function) aP2F = thePoint2->GetLastFunction();

  //Add a translate function
  aFunction = theObject->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_TWO_POINTS);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI (aFunction);
  aTI.SetPoint1(aP1F);
  aTI.SetPoint2(aP2F);
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.TranslateTwoPoints("
    << theObject << ", " << thePoint1 << ", " << thePoint2 << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  TranslateDXDYDZ
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateDXDYDZ
       (Handle(GEOM_Object) theObject, double theX, double theY,  double theZ)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a translate function
  aFunction = theObject->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_XYZ);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetDX(theX);
  aTI.SetDY(theY);
  aTI.SetDZ(theZ);
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TranslateDXDYDZ("
    << theObject << ", " << theX << ", " << theY << ", " << theZ << ")";

  SetErrorCode(OK);
  return theObject;
}


//=============================================================================
/*!
 *  TranslateTwoPointsCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateTwoPointsCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint1, Handle(GEOM_Object) thePoint2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePoint1.IsNull() || thePoint2.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a translate function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_TWO_POINTS_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetPoint1(thePoint1->GetLastFunction());
  aTI.SetPoint2(thePoint2->GetLastFunction());
  //aTI.SetShape(theObject->GetValue());
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeTranslationTwoPoints("
    << theObject << ", " << thePoint1 << ", " << thePoint2 << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  TranslateDXDYDZCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateDXDYDZCopy
       (Handle(GEOM_Object) theObject, double theX, double theY,  double theZ)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a translate function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_XYZ_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetDX(theX);
  aTI.SetDY(theY);
  aTI.SetDZ(theZ);
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeTranslation("
    << theObject << ", " << theX << ", " << theY << ", " << theZ << ")";

  SetErrorCode(OK);
  return aCopy;
}


//=============================================================================
/*!
 *  TranslateVector
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateVector
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theVector.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  // Get last functions of the arguments
  Handle(GEOM_Function) aVF = theVector->GetLastFunction();

  //Add a translate function
  aFunction = theObject->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_VECTOR);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI (aFunction);
  aTI.SetVector(aVF);
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.TranslateVector("
                               << theObject << ", " << theVector << ")";

  SetErrorCode(OK);
  return theObject;
}
//=============================================================================
/*!
 *  TranslateVectorCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateVectorCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theVector.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a translate function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_VECTOR_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetVector(theVector->GetLastFunction());
//  aTI.SetShape(theObject->GetValue());
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeTranslationVector("
                               << theObject << ", " << theVector << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  TranslateVectorDistance
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::TranslateVectorDistance
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector, double theDistance, bool theCopy)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theVector.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  Handle(GEOM_Object) aCopy;   //Add a new Copy object
  Handle(GEOM_Function) aFunction;

  //Add a translate function
  if (theCopy) {
    aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());
    aFunction = aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_VECTOR_DISTANCE);
  }
  else {
    aFunction = theObject->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_VECTOR_DISTANCE);
  }
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetVector(theVector->GetLastFunction());
  aTI.SetDistance(theDistance);
//  aTI.SetShape(theObject->GetValue());
  aTI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  if (theCopy) {
    GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeTranslationVectorDistance("
				 << theObject << ", " << theVector << ", " << theDistance << ")";
    SetErrorCode(OK);
    return aCopy;
  }

  GEOM::TPythonDump(aFunction) << "geompy.TranslateVectorDistance("
			       << theObject << ", " << theVector << ", " << theDistance << ", " << theCopy << ")";
  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  Translate1D
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::Translate1D
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theVector,
        double theStep, Standard_Integer theNbTimes)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theVector.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a translate function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_1D);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetVector(theVector->GetLastFunction());
  aTI.SetOriginal(aLastFunction);
  aTI.SetStep1(theStep);
  aTI.SetNbIter1(theNbTimes);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeMultiTranslation1D("
    << theObject << ", " << theVector << ", " << theStep << ", " << theNbTimes << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  Translate2D
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::Translate2D (Handle(GEOM_Object) theObject,
								Handle(GEOM_Object) theVector,
								double theStep1,
								Standard_Integer theNbTimes1,
								Handle(GEOM_Object) theVector2,
								double theStep2,
								Standard_Integer theNbTimes2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theVector.IsNull() || theVector2.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be moved

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a translate function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_TranslateDriver::GetID(), TRANSLATE_2D);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_TranslateDriver::GetID()) return NULL;

  GEOMImpl_ITranslate aTI(aFunction);
  aTI.SetVector(theVector->GetLastFunction());
  aTI.SetVector2(theVector2->GetLastFunction());
  aTI.SetOriginal(aLastFunction);
  aTI.SetStep1(theStep1);
  aTI.SetNbIter1(theNbTimes1);
  aTI.SetStep2(theStep2);
  aTI.SetNbIter2(theNbTimes2);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Translation driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeMultiTranslation2D("
    << theObject << ", " << theVector  << ", " << theStep1 << ", " << theNbTimes1
      << ", " << theVector2 << ", " << theStep2 << ", " << theNbTimes2 << ")";

  SetErrorCode(OK);
  return aCopy;
}


//=============================================================================
/*!
 *  MirrorPlane
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorPlane
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePlane)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePlane.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be mirrored

  // Get last functions of the arguments
  Handle(GEOM_Function) aPF = thePlane->GetLastFunction();

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_PLANE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetPlane(aPF);
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.MirrorPlane("
                               << theObject << ", " << thePlane << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  MirrorPlaneCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorPlaneCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePlane)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePlane.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be mirrored

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_PLANE_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetPlane(thePlane->GetLastFunction());
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeMirrorByPlane("
                               << theObject << ", " << thePlane << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  MirrorPoint
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorPoint
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePoint.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be mirrored

  // Get last functions of the arguments
  Handle(GEOM_Function) aPF = thePoint->GetLastFunction();

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_POINT);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetPoint(aPF);
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.MirrorPoint("
                               << theObject << ", " << thePoint << ")";

  SetErrorCode(OK);
  return NULL;
}

//=============================================================================
/*!
 *  MirrorPointCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorPointCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePoint.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be mirrored

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_POINT_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetPoint(thePoint->GetLastFunction());
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeMirrorByPoint("
                               << theObject << ", " << thePoint << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  MirrorAxis
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorAxis
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be mirrored

  // Get last functions of the arguments
  Handle(GEOM_Function) anAF = theAxis->GetLastFunction();

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_AXIS);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetAxis(anAF);
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.MirrorAxis("
                               << theObject << ", " << theAxis << ")";

  SetErrorCode(OK);
  return NULL;
}

//=============================================================================
/*!
 *  MirrorAxisCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::MirrorAxisCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL; //There is no function which creates an object to be mirrored

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a mirror function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_MirrorDriver::GetID(), MIRROR_AXIS_COPY);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MirrorDriver::GetID()) return NULL;

  GEOMImpl_IMirror aTI (aFunction);
  aTI.SetAxis(theAxis->GetLastFunction());
  aTI.SetOriginal(aLastFunction);

  //Compute the mirror
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Mirror driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeMirrorByAxis("
                               << theObject << ", " << theAxis << ")";

  SetErrorCode(OK);
  return aCopy;
}


//=============================================================================
/*!
 *  OffsetShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::OffsetShape
                              (Handle(GEOM_Object) theObject, double theOffset)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be offset

  //Add a new Offset function
  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_OffsetDriver::GetID(), OFFSET_SHAPE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_OffsetDriver::GetID()) return NULL;

  GEOMImpl_IOffset aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetValue(theOffset);

  //Compute the offset
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Offset driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.OffsetShape("
                               << theObject << ", " << theOffset << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  OffsetShapeCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::OffsetShapeCopy
                              (Handle(GEOM_Object) theObject, double theOffset)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be offset

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a new Offset function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_OffsetDriver::GetID(), OFFSET_SHAPE_COPY);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_OffsetDriver::GetID()) return NULL;

  GEOMImpl_IOffset aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetValue(theOffset);

  //Compute the offset
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Offset driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeOffset("
                               << theObject << ", " << theOffset << ")";

  SetErrorCode(OK);
  return aCopy;
}


//=============================================================================
/*!
 *  ScaleShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::ScaleShape
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint, double theFactor)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be scaled

  //Add a scale function
  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_ScaleDriver::GetID(), SCALE_SHAPE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ScaleDriver::GetID()) return NULL;

  // Set arguments
  GEOMImpl_IScale aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetFactor(theFactor);

  // Set point argument
  if (!thePoint.IsNull()) {
    Handle(GEOM_Function) aPF = thePoint->GetLastFunction();
    aTI.SetPoint(aPF);
  }

  //Compute the scale
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Scale driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.ScaleShape("
    << theObject << ", " << thePoint << ", " << theFactor << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  ScaleShapeCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::ScaleShapeCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePoint, double theFactor)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be scaled

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a scale function
  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_ScaleDriver::GetID(), SCALE_SHAPE_COPY);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ScaleDriver::GetID()) return NULL;

  // Set arguments
  GEOMImpl_IScale aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetFactor(theFactor);

  // Set point argument
  if (!thePoint.IsNull()) {
    Handle(GEOM_Function) aPF = thePoint->GetLastFunction();
    aTI.SetPoint(aPF);
  }

  //Compute the scale
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Scale driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeScaleTransform("
    << theObject << ", " << thePoint << ", " << theFactor << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  ScaleShapeAlongAxes
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::ScaleShapeAlongAxes (Handle(GEOM_Object) theObject,
                                                                        Handle(GEOM_Object) thePoint,
                                                                        double theFactorX,
                                                                        double theFactorY,
                                                                        double theFactorZ,
                                                                        bool   doCopy)
{
  SetErrorCode(KO);

  if (theObject.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be scaled

  //Add a scale function
  Handle(GEOM_Object) aCopy;   //Add a new Copy object
  Handle(GEOM_Function) aFunction;
  if (doCopy) {
    aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());
    aFunction = aCopy->AddFunction(GEOMImpl_ScaleDriver::GetID(), SCALE_SHAPE_AXES_COPY);
  }
  else {
    aFunction = theObject->AddFunction(GEOMImpl_ScaleDriver::GetID(), SCALE_SHAPE_AXES);
  }
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ScaleDriver::GetID()) return NULL;

  // Set arguments
  GEOMImpl_IScale aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetFactorX(theFactorX);
  aTI.SetFactorY(theFactorY);
  aTI.SetFactorZ(theFactorZ);

  // Set point (optional argument)
  if (!thePoint.IsNull()) {
    Handle(GEOM_Function) aPF = thePoint->GetLastFunction();
    aTI.SetPoint(aPF);
  }

  //Compute the scale
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Scale driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  SetErrorCode(OK);

  //Make a Python command
  if (doCopy) {
    GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeScaleAlongAxes("
                                 << theObject << ", " << thePoint << ", "
                                 << theFactorX << ", " << theFactorY << ", " << theFactorZ << ")";
    return aCopy;
  }

  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.ScaleShapeAlongAxes("
                               << theObject << ", " << thePoint << ", "
                               << theFactorX << ", " << theFactorY << ", " << theFactorZ << ")";
  return theObject;
}

//=============================================================================
/*!
 *  PositionShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::PositionShape
        (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theStartLCS, Handle(GEOM_Object) theEndLCS)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theEndLCS.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be set in position

  //Add a Position function
  Standard_Integer aType = POSITION_SHAPE;
  if (theStartLCS.IsNull()) aType = POSITION_SHAPE_FROM_GLOBAL;

  Handle(GEOM_Function) aFunction =
    theObject->AddFunction(GEOMImpl_PositionDriver::GetID(), aType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PositionDriver::GetID()) return NULL;

  //Set operation arguments
  GEOMImpl_IPosition aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetEndLCS(theEndLCS->GetLastFunction());
  if (!theStartLCS.IsNull())
    aTI.SetStartLCS(theStartLCS->GetLastFunction());

  //Compute the Position
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Position driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.PositionShape("
    << theObject << ", " << theStartLCS << ", " << theEndLCS << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  PositionShapeCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::PositionShapeCopy
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theStartLCS, Handle(GEOM_Object) theEndLCS)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theEndLCS.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be set in position

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a position function
  Standard_Integer aType = POSITION_SHAPE_COPY;
  if (theStartLCS.IsNull()) aType = POSITION_SHAPE_FROM_GLOBAL_COPY;

  Handle(GEOM_Function) aFunction =
    aCopy->AddFunction(GEOMImpl_PositionDriver::GetID(), aType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PositionDriver::GetID()) return NULL;

  GEOMImpl_IPosition aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetEndLCS(theEndLCS->GetLastFunction());
  if (!theStartLCS.IsNull())
    aTI.SetStartLCS(theStartLCS->GetLastFunction());

  //Compute the position
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Position driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakePosition("
    << theObject << ", " << theStartLCS << ", " << theEndLCS << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  PositionAlongPath
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::PositionAlongPath
       (Handle(GEOM_Object) theObject, Handle(GEOM_Object) thePath, 
	double theDistance, bool theCopy, bool theReverse)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || thePath.IsNull()) return NULL;

  Handle(GEOM_Function) anOriginal = theObject->GetLastFunction();
  if (anOriginal.IsNull()) return NULL; //There is no function which creates an object to be set in position

  //Add a position function
  Handle(GEOM_Function) aFunction;
  Handle(GEOM_Object) aCopy;

  if (theCopy) {
    aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());
    aFunction = aCopy->AddFunction(GEOMImpl_PositionDriver::GetID(), POSITION_ALONG_PATH);
  }
  else
    aFunction = theObject->AddFunction(GEOMImpl_PositionDriver::GetID(), POSITION_ALONG_PATH);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PositionDriver::GetID()) return NULL;

  GEOMImpl_IPosition aTI (aFunction);
  aTI.SetShape(anOriginal);
  aTI.SetPath(thePath->GetLastFunction());
  aTI.SetDistance(theDistance);
  aTI.SetReverse(theReverse);

  //Compute the position
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Position driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  if (theCopy) {
    GEOM::TPythonDump(aFunction) << aCopy << " = geompy.PositionAlongPath("
				 << theObject << ", " << thePath << ", " << theDistance << ", " << theCopy << ", " << theReverse << ")";
    SetErrorCode(OK);
    return aCopy;
  }

  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.PositionAlongPath("
    << theObject << ", " << thePath << ", " << theDistance << ", " << theCopy << ", " << theReverse << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  Rotate
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::Rotate (Handle(GEOM_Object) theObject,
                                                           Handle(GEOM_Object) theAxis,
                                                           double theAngle)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  // Get last functions of the arguments
  Handle(GEOM_Function) anAF = theAxis->GetLastFunction();

  //Add a rotate function
  aFunction = theObject->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetAxis(anAF);
  aRI.SetOriginal(aLastFunction);
  aRI.SetAngle(theAngle);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.Rotate(" << theObject
    << ", " << theAxis << ", " << theAngle * 180.0 / PI << "*math.pi/180.0)";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  Rotate
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::RotateCopy (Handle(GEOM_Object) theObject, Handle(GEOM_Object) theAxis, double theAngle)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a rotate function
  aFunction = aCopy->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE_COPY);
  if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetAxis(theAxis->GetLastFunction());
  aRI.SetOriginal(aLastFunction);
  aRI.SetAngle(theAngle);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeRotation(" << theObject
    << ", " << theAxis << ", " << theAngle * 180.0 / PI << "*math.pi/180.0)";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  Rotate1D
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::Rotate1D (Handle(GEOM_Object) theObject,
                                                             Handle(GEOM_Object) theAxis,
                                                             Standard_Integer theNbTimes)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a rotate function
  aFunction = aCopy->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE_1D);
  if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetOriginal(aLastFunction);
  aRI.SetAxis(theAxis->GetLastFunction());
  aRI.SetNbIter1(theNbTimes);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MultiRotate1D("
    << theObject << ", " << theAxis << ", " << theNbTimes << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  Rotate2D
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::Rotate2D (Handle(GEOM_Object) theObject,
							     Handle(GEOM_Object) theAxis,
							     double theAngle,
							     Standard_Integer theNbTimes1,
							     double theStep,
							     Standard_Integer theNbTimes2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theAxis.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a rotate function
  aFunction = aCopy->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE_2D);
  if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetAxis(theAxis->GetLastFunction());
  aRI.SetOriginal(aLastFunction);
  aRI.SetNbIter1(theNbTimes1);
  aRI.SetNbIter2(theNbTimes2);
  aRI.SetAngle(theAngle);
  aRI.SetStep(theStep);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MultiRotate2D("
    << theObject << ", " << theAxis << ", " << theAngle << ", "
      << theNbTimes1 << ", " << theStep << ", " << theNbTimes2 << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  RotateThreePoints
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::RotateThreePoints (Handle(GEOM_Object) theObject,
								      Handle(GEOM_Object) theCentPoint, 
								      Handle(GEOM_Object) thePoint1,
								      Handle(GEOM_Object) thePoint2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theCentPoint.IsNull() || thePoint1.IsNull() || thePoint2.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  // Get last functions of the arguments
  Handle(GEOM_Function) aCPF = theCentPoint->GetLastFunction();
  Handle(GEOM_Function) aP1F = thePoint1->GetLastFunction();
  Handle(GEOM_Function) aP2F = thePoint2->GetLastFunction();


  //Add a rotate function
  aFunction = theObject->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE_THREE_POINTS);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetCentPoint(aCPF);
  aRI.SetPoint1(aP1F);
  aRI.SetPoint2(aP2F);
  aRI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.TrsfOp.RotateThreePoints(" << theObject
			       << ", " << theCentPoint << ", "<<thePoint1 << ", " << thePoint2 << ")";

  SetErrorCode(OK);
  return theObject;
}

//=============================================================================
/*!
 *  RotateThreePointsCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ITransformOperations::RotateThreePointsCopy (Handle(GEOM_Object) theObject, 
							 Handle(GEOM_Object) theCentPoint, 
							 Handle(GEOM_Object) thePoint1,
							 Handle(GEOM_Object) thePoint2)
{
  SetErrorCode(KO);

  if (theObject.IsNull() || theCentPoint.IsNull() || thePoint1.IsNull() || thePoint2.IsNull()) return NULL;

  Handle(GEOM_Function) aFunction, aLastFunction = theObject->GetLastFunction();
  if (aLastFunction.IsNull()) return NULL;  //There is no function which creates an object to be rotated

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), theObject->GetType());

  //Add a rotate function
  aFunction = aCopy->AddFunction(GEOMImpl_RotateDriver::GetID(), ROTATE_THREE_POINTS_COPY);
  if (aFunction.IsNull()) return NULL;

    //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_RotateDriver::GetID()) return NULL;

  GEOMImpl_IRotate aRI(aFunction);
  aRI.SetCentPoint(theCentPoint->GetLastFunction());
  aRI.SetPoint1(thePoint1->GetLastFunction());
  aRI.SetPoint2(thePoint2->GetLastFunction());
  aRI.SetOriginal(aLastFunction);

  //Compute the translation
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Rotate driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeRotationThreePoints(" << theObject
			       << ", " << theCentPoint << ", "<<thePoint1 << ", " << thePoint2 << ")";

  SetErrorCode(OK);
  return aCopy;
}

