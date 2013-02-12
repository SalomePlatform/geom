// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <Standard_Stream.hxx>

#include "GEOM_ICurvesOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

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
  Handle(GEOM_Object) aPnt, aVec;
  if (!CORBA::is_nil(thePnt)) {
    aPnt = GetObjectImpl(thePnt);
    if (aPnt.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVec)) {
    aVec = GetObjectImpl(theVec);
    if (aVec.IsNull()) return aGEOMObject._retn();
  }

  // Make Circle
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Circle
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Circle
  Handle(GEOM_Object) anObject = GetOperations()->MakeCircleCenter2Pnt(aPnt1, aPnt2, aPnt3);
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
  Handle(GEOM_Object) aPnt, aVec, aVecMaj;
  if (!CORBA::is_nil(thePnt)) {
    aPnt = GetObjectImpl(thePnt);
    if (aPnt.IsNull()) return aGEOMObject._retn();
  }
  if (!CORBA::is_nil(theVec)) {
    aVec = GetObjectImpl(theVec);
    if (aVec.IsNull()) return aGEOMObject._retn();
  }

  // Make Ellipse
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt, aVec, aVecMaj;
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
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Arc
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make ArcCenter
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  Handle(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);

  if (aPnt1.IsNull() || aPnt2.IsNull() || aPnt3.IsNull()) return aGEOMObject._retn();

  // Make Arc
  Handle(GEOM_Object) anObject =
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
  std::list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    Handle(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  Handle(GEOM_Object) anObject =
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
  std::list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    Handle(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Bezier curve
  Handle(GEOM_Object) anObject =
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
  std::list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    Handle(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  Handle(GEOM_Object) anObject =
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
  std::list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    Handle(GEOM_Object) aPnt = GetObjectImpl(thePoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  //Get the reference vectors
  Handle(GEOM_Object) aVec1 = GetObjectImpl(theFirstVec);
  Handle(GEOM_Object) aVec2 = GetObjectImpl(theLastVec);

  if (aVec1.IsNull() || aVec2.IsNull()) return aGEOMObject._retn();

  // Make Polyline
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeCurveParametric(thexExpr, theyExpr, thezExpr,
                       theParamMin, theParamMax,
                       0.0, aType, theParamNbStep, true);

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
  Handle(GEOM_Object) anObject =
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

  Handle(GEOM_Object) aWorkingPlane = GetObjectImpl(theWorkingPlane);

  // Make Sketcher
  Handle(GEOM_Object) anObject =
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
  Handle(GEOM_Object) anObject = GetOperations()->Make3DSketcherCommand(theCommand);
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
  Handle(GEOM_Object) anObject =
    GetOperations()->Make3DSketcher(aCoords);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return GEOM::GEOM_Object::_nil();

  return GetObject(anObject);
}
