using namespace std; 

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

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

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

  if (thePnt1 == NULL || thePnt2 == NULL || thePnt3 == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aPnt2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  Handle(GEOM_Object) aPnt3 = GetOperations()->GetEngine()->GetObject
    (thePnt3->GetStudyID(), thePnt3->GetEntry());

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

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Ellipse
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeEllipse(aPnt, aVec, theRMajor, theRMinor);
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

  if (thePnt1 == NULL || thePnt2 == NULL || thePnt3 == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aPnt2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  Handle(GEOM_Object) aPnt3 = GetOperations()->GetEngine()->GetObject
    (thePnt3->GetStudyID(), thePnt3->GetEntry());

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
 *  MakePolyline
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakePolyline
                                    (const GEOM::ListOfGO& thePoints)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    if (thePoints[ind] == NULL) return aGEOMObject._retn();

    Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
      (thePoints[ind]->GetStudyID(), thePoints[ind]->GetEntry());

    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePolyline(aPoints);
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
                                              (const GEOM::ListOfGO& thePoints)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    if (thePoints[ind] == NULL) return aGEOMObject._retn();

    Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
      (thePoints[ind]->GetStudyID(), thePoints[ind]->GetEntry());

    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Bezier curve
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeSplineBezier(aPoints);
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
                                              (const GEOM::ListOfGO& thePoints)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thePoints.length();
  list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    if (thePoints[ind] == NULL) return aGEOMObject._retn();

    Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
      (thePoints[ind]->GetStudyID(), thePoints[ind]->GetEntry());

    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }

  // Make Polyline
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeSplineInterpolation(aPoints);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSketcher
 */
//============================================================================= 
GEOM::GEOM_Object_ptr GEOM_ICurvesOperations_i::MakeSketcher (const char* theCommand, const GEOM::ListOfDouble& theWorkingPlane)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind = 0;
  int aLen = theWorkingPlane.length();
  list<double> aWorkingPlane;
  for (; ind < aLen; ind++)
    aWorkingPlane.push_back(theWorkingPlane[ind]);

  // Make Sketcher
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeSketcher(theCommand, aWorkingPlane);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
