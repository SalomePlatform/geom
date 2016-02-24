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

#include "GEOM_ICurvesOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_ICurvesOperations_i::GEOM_ICurvesOperations_i (PortableServer::POA_ptr thePOA,
                                                    GEOM::GEOM_Gen_ptr theEngine,
                                                    ::GEOMImpl_ICurvesOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_ICurvesOperations_i::GEOM_ICurvesOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_ICurvesOperations_i::~GEOM_ICurvesOperations_i()
{
  MESSAGE("GEOM_ICurvesOperations_i::~GEOM_ICurvesOperations_i");
}


//=============================================================================
/*!
 *  MakeCirclePntVecR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeCirclePntVecR
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  // Not set thePnt means origin of global CS,
  // Not set theVec means Z axis of global CS
  //if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the arguments
  HANDLE_NAMESPACE(GEOM_Object) aPnt, aVec;
  if (!CORBA::is_nil(thePnt)) {
    aPnt = GetObjectImpl(thePnt);
    if (aPnt.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVec)) {
    aVec = GetObjectImpl(theVec);
    if (aVec.IsNull()) return aGEOMObject._retn();
  }

  // Make Circle
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeCirclePntVecR(aPnt, aVec, theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCircleThreePnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeCircleThreePnt
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

  // Make Circle
  HANDLE_NAMESPACE(GEOM_Object) anObject =
      GetOperations()->MakeCircleThreePnt(aPnt1, aPnt2, aPnt3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCircleCenter2Pnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeCircleCenter2Pnt
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

  // Make Circle
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCircleCenter2Pnt(aPnt1, aPnt2, aPnt3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeEllipse
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeEllipse
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theRMajor, double theRMinor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  // Not set thePnt means origin of global CS,
  // Not set theVec means Z axis of global CS
  //if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the arguments
  HANDLE_NAMESPACE(GEOM_Object) aPnt, aVec, aVecMaj;
  if (!CORBA::is_nil(thePnt)) {
    aPnt = GetObjectImpl(thePnt);
    if (aPnt.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVec)) {
    aVec = GetObjectImpl(theVec);
    if (aVec.IsNull()) return aGEOMObject._retn();
  }

  // Make Ellipse
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeEllipse(aPnt, aVec, theRMajor, theRMinor, aVecMaj);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeEllipseVec
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeEllipseVec
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
                       CORBA::Double theRMajor, double theRMinor,
                       GEOM::GEOM_Object_ptr theVecMaj)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  // Not set thePnt means origin of global CS,
  // Not set theVec means Z axis of global CS
  // Not set theVecMaj means X axis of global CS
  //if (thePnt == NULL || theVec == NULL || theVecMaj == NULL) return aGEOMObject._retn();

  //Get the arguments
  HANDLE_NAMESPACE(GEOM_Object) aPnt, aVec, aVecMaj;
  if (!CORBA::is_nil(thePnt)) {
    aPnt = GetObjectImpl(thePnt);
    if (aPnt.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVec)) {
    aVec = GetObjectImpl(theVec);
    if (aVec.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVecMaj)) {
    aVecMaj = GetObjectImpl(theVecMaj);
    if (aVecMaj.IsNull()) return aGEOMObject._retn();
  }

  // Make Ellipse
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeEllipse(aPnt, aVec, theRMajor, theRMinor, aVecMaj);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeArc
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeArc
                                                (GEOM::GEOM_Object_ptr thePnt1,
                                                 GEOM::GEOM_Object_ptr thePnt2,
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

  // Make Arc
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeArc(aPnt1, aPnt2, aPnt3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakeArcCenter
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeArcCenter
    (GEOM::GEOM_Object_ptr thePnt1,
     GEOM::GEOM_Object_ptr thePnt2,
     GEOM::GEOM_Object_ptr thePnt3,
     CORBA::Boolean theSense)

{
  GEOM::GEOM_Object_var aGEOMObject;
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  HANDLE_NAMESPACE(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make ArcCenter
  HANDLE_NAMESPACE(GEOM_Object) anObject =
      GetOperations()->MakeArcCenter(aPnt1, aPnt2, aPnt3,theSense);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeArc
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeArcOfEllipse
                                                (GEOM::GEOM_Object_ptr thePnt1,
                                                 GEOM::GEOM_Object_ptr thePnt2,
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

  // Make Arc
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeArcOfEllipse(aPnt1, aPnt2, aPnt3);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePolyline
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakePolyline
                                    (const GEOM::ListOfGO& thePoints,
                                     CORBA::Boolean        theIsClosed)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  std::list<HANDLE_NAMESPACE(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakePolyline(aPoints, theIsClosed);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSplineBezier
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSplineBezier
                                              (const GEOM::ListOfGO& thePoints,
                                               CORBA::Boolean        theIsClosed)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  std::list<HANDLE_NAMESPACE(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Bezier curve
  HANDLE_NAMESPACE(GEOM_Object) anObject =
      GetOperations()->MakeSplineBezier(aPoints, theIsClosed);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSplineInterpolation
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSplineInterpolation
                                              (const GEOM::ListOfGO& thePoints,
                                               CORBA::Boolean        theIsClosed,
                                               CORBA::Boolean        theDoReordering)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  std::list<HANDLE_NAMESPACE(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeSplineInterpolation(aPoints, theIsClosed, theDoReordering);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSplineInterpolWithTangents
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSplineInterpolWithTangents
                                              (const GEOM::ListOfGO& thePoints,
                                               GEOM::GEOM_Object_ptr theFirstVec,
                                               GEOM::GEOM_Object_ptr theLastVec)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  int ind = 0;
  int aLen = thePoints.length();
  std::list<HANDLE_NAMESPACE(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  //Get the reference vectors
  HANDLE_NAMESPACE(GEOM_Object) aVec1 = GetObjectImpl(theFirstVec);
  HANDLE_NAMESPACE(GEOM_Object) aVec2 = GetObjectImpl(theLastVec);

  if (aVec1.IsNull() || aVec2.IsNull()) return aGEOMObject._retn();

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeSplineInterpolWithTangents(aPoints, aVec1, aVec2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCurveParametric
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeCurveParametric
             (const char* thexExpr, const char* theyExpr, const char* thezExpr,
              double theParamMin, double theParamMax, double theParamStep,
              GEOM::curve_type theCurveType)
{
  GEOM::GEOM_Object_var aGEOMObject;
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOMImpl_ICurvesOperations::CurveType aType;
  switch(theCurveType) {
  case GEOM::Polyline:
    aType = GEOMImpl_ICurvesOperations::Polyline;
    break;
  case GEOM::Bezier:
    aType = GEOMImpl_ICurvesOperations::Bezier;
    break;
  case GEOM::Interpolation:
    aType = GEOMImpl_ICurvesOperations::Interpolation;
    break;
  default:
    break;
  }

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeCurveParametric(thexExpr, theyExpr, thezExpr,
                                         theParamMin, theParamMax,
                                         theParamStep, aType);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCurveParametricNew
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeCurveParametricNew
             (const char* thexExpr, const char* theyExpr, const char* thezExpr,
              double theParamMin, double theParamMax, CORBA::Long theParamNbStep,
              GEOM::curve_type theCurveType)
{
  GEOM::GEOM_Object_var aGEOMObject;
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOMImpl_ICurvesOperations::CurveType aType;
  switch(theCurveType) {
  case GEOM::Polyline:
    aType = GEOMImpl_ICurvesOperations::Polyline;
    break;
  case GEOM::Bezier:
    aType = GEOMImpl_ICurvesOperations::Bezier;
    break;
  case GEOM::Interpolation:
    aType = GEOMImpl_ICurvesOperations::Interpolation;
    break;
  default:
    break;
  }

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeCurveParametric(thexExpr, theyExpr, thezExpr,
                       theParamMin, theParamMax,
                       0.0, aType, theParamNbStep, true);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeIsoline
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeIsoline
                                         (GEOM::GEOM_Object_ptr theFace,
                                          CORBA::Boolean        IsUIsoline,
                                          double                theParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  HANDLE_NAMESPACE(GEOM_Object) aFace = GetObjectImpl(theFace);

  // Make isoline
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeIsoline(aFace, IsUIsoline, theParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSketcher
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSketcher
            (const char* theCommand, const GEOM::ListOfDouble& theWorkingPlane)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind = 0;
  int aLen = theWorkingPlane.length();
  std::list<double> aWorkingPlane;
  for (; ind < aLen; ind++)
    aWorkingPlane.push_back(theWorkingPlane[ind]);

  // Make Sketcher
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeSketcher(theCommand, aWorkingPlane);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return GEOM::GEOM_Object::_nil();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSketcherOnPlane
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSketcherOnPlane
                (const char* theCommand, GEOM::GEOM_Object_ptr theWorkingPlane)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  HANDLE_NAMESPACE(GEOM_Object) aWorkingPlane = GetObjectImpl(theWorkingPlane);

  // Make Sketcher
  HANDLE_NAMESPACE(GEOM_Object) anObject =
      GetOperations()->MakeSketcherOnPlane(theCommand, aWorkingPlane);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return GEOM::GEOM_Object::_nil();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Make3DSketcherCommand
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::Make3DSketcherCommand (const char* theCommand)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  // Make 3D Sketcher
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->Make3DSketcherCommand(theCommand);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return GEOM::GEOM_Object::_nil();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Make3DSketcher
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::Make3DSketcher
            (const GEOM::ListOfDouble& theCoordinates)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind = 0;
  int aLen = theCoordinates.length();
  std::list<double> aCoords;
  for (; ind < aLen; ind++)
    aCoords.push_back(theCoordinates[ind]);

  // Make Sketcher
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->Make3DSketcher(aCoords);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return GEOM::GEOM_Object::_nil();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePolyline2D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakePolyline2D
                (const GEOM::ListOfListOfDouble &theCoordsList,
                 const GEOM::string_array       &theNamesList,
                 const GEOM::short_array        &theTypesList,
                 const GEOM::ListOfBool         &theClosedList,
                 const GEOM::ListOfDouble       &theWorkingPlane)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  // Convert input data
  Handle(TColStd_HArray1OfExtendedString) aNames   =
    ConvertStringArray(theNamesList);
  Handle(TColStd_HArray1OfByte)           aTypes   =
    ConvertEnumArray(theTypesList);
  Handle(TColStd_HArray1OfByte)           aCloseds =
    ConvertBoolArray(theClosedList);
  std::list <std::list <double> >         aCoords;

  ConvertListListDouble(theCoordsList, aCoords);

  Handle(TColStd_HArray1OfReal) aWorkingPlane;
  const int                     n = theWorkingPlane.length();
  int                           i;

  if (n > 0) {
    aWorkingPlane = new TColStd_HArray1OfReal(1, n);

    for (i = 0; i < n; i++) {
      aWorkingPlane->SetValue(i + 1, theWorkingPlane[i]);
    }
  }

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakePolyline2D
            (aCoords, aNames, aTypes, aCloseds, aWorkingPlane);

  if (!GetOperations()->IsDone() || anObject.IsNull()) {
    return GEOM::GEOM_Object::_nil();
  }

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePolylineOnPlane
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakePolyline2DOnPlane
                (const GEOM::ListOfListOfDouble &theCoordsList,
                 const GEOM::string_array       &theNamesList,
                 const GEOM::short_array        &theTypesList,
                 const GEOM::ListOfBool         &theClosedList,
                 GEOM::GEOM_Object_ptr           theWorkingPlane)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  // Convert input data
  Handle(TColStd_HArray1OfExtendedString) aNames        =
    ConvertStringArray(theNamesList);
  Handle(TColStd_HArray1OfByte)           aTypes        =
    ConvertEnumArray(theTypesList);
  Handle(TColStd_HArray1OfByte)           aCloseds      =
    ConvertBoolArray(theClosedList);
  std::list <std::list <double> >         aCoords;
  HANDLE_NAMESPACE(GEOM_Object)                     aWorkingPlane =
    GetObjectImpl(theWorkingPlane);

  ConvertListListDouble(theCoordsList, aCoords);

  // Make Polyline
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakePolyline2DOnPlane
            (aCoords, aNames, aTypes, aCloseds, aWorkingPlane);

  if (!GetOperations()->IsDone() || anObject.IsNull()) {
    return GEOM::GEOM_Object::_nil();
  }

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ConvertEnumArray
 */
//=============================================================================
Handle(TColStd_HArray1OfByte) GEOM_ICurvesOperations_i::ConvertEnumArray
                (const GEOM::short_array &theInArray)
{
  Handle(TColStd_HArray1OfByte) anOutArray;
  const int                     n = theInArray.length();
  int                           i;

  if (n <= 0) {
    return anOutArray;
  }

  anOutArray = new TColStd_HArray1OfByte(1, n);

  for (i = 0; i < n; i++) {
    bool                                  isOK = true;
    GEOMImpl_ICurvesOperations::CurveType aType;

    switch(theInArray[i]) {
      case GEOM::Polyline:
        aType = GEOMImpl_ICurvesOperations::Polyline;
        break;
      case GEOM::Bezier:
        aType = GEOMImpl_ICurvesOperations::Bezier;
        break;
      case GEOM::Interpolation:
        aType = GEOMImpl_ICurvesOperations::Interpolation;
        break;
      default:
        isOK = false;
        break;
    }

    if (isOK) {
      anOutArray->SetValue(i + 1, aType);
    } else {
      anOutArray.Nullify();
      break;
    }
  }

  return anOutArray;
}

//=============================================================================
/*!
 *  ConvertBoolArray
 */
//=============================================================================
Handle(TColStd_HArray1OfByte) GEOM_ICurvesOperations_i::ConvertBoolArray
                (const GEOM::ListOfBool &theInArray)
{
  Handle(TColStd_HArray1OfByte) anOutArray;
  const int                     n = theInArray.length();
  int                           i;

  if (n <= 0) {
    return anOutArray;
  }

  anOutArray = new TColStd_HArray1OfByte(1, n);

  for (i = 0; i < n; i++) {
    anOutArray->SetValue(i + 1, theInArray[i]);
  }

  return anOutArray;
}

//=============================================================================
/*!
 *  ConvertListListDouble
 */
//=============================================================================
void GEOM_ICurvesOperations_i::ConvertListListDouble
            (const GEOM::ListOfListOfDouble        &theInList,
                   std::list <std::list <double> > &theOutList)
{
  const int          n = theInList.length();
  int                i;
  std::list <double> anEmptyList;

  for (i = 0; i < n; i++) {
    theOutList.push_back(anEmptyList);

    const int m = theInList[i].length();
    int       j;

    for (j = 0; j < m; j++) {
      theOutList.back().push_back(theInList[i][j]);
    }
  }
}
