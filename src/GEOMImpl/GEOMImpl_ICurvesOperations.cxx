using namespace std;

#include "GEOMImpl_ICurvesOperations.hxx"

#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

#include "GEOMImpl_PolylineDriver.hxx"
#include "GEOMImpl_CircleDriver.hxx"
#include "GEOMImpl_SplineDriver.hxx"
#include "GEOMImpl_EllipseDriver.hxx"
#include "GEOMImpl_ArcDriver.hxx"
#include "GEOMImpl_SketcherDriver.hxx"

#include "GEOMImpl_IPolyline.hxx"
#include "GEOMImpl_ICircle.hxx"
#include "GEOMImpl_ISpline.hxx"
#include "GEOMImpl_IEllipse.hxx"
#include "GEOMImpl_IArc.hxx"
#include "GEOMImpl_ISketcher.hxx"

#include "utilities.h"

#include <TDF_Tool.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_ICurvesOperations::GEOMImpl_ICurvesOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_ICurvesOperations::GEOMImpl_ICurvesOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_ICurvesOperations::~GEOMImpl_ICurvesOperations()
{
  MESSAGE("GEOMImpl_ICurvesOperations::~GEOMImpl_ICurvesOperations");
}


//=============================================================================
/*!
 *  MakePolyline
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakePolyline (list<Handle(GEOM_Object)> thePoints)
{
  SetErrorCode(KO);

  //Add a new Polyline object
  Handle(GEOM_Object) aPolyline = GetEngine()->AddObject(GetDocID(), GEOM_POLYLINE);

  //Add a new Polyline function for creation a polyline relatively to points set
  Handle(GEOM_Function) aFunction =
    aPolyline->AddFunction(GEOMImpl_PolylineDriver::GetID(), POLYLINE_POINTS);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PolylineDriver::GetID()) return NULL;

  GEOMImpl_IPolyline aCI (aFunction);

  int aLen = thePoints.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<Handle(GEOM_Object)>::iterator it = thePoints.begin();
  for (; it != thePoints.end(); it++, ind++) {
    Handle(GEOM_Function) aRefPnt = (*it)->GetLastFunction();
    if (aRefPnt.IsNull()) {
      SetErrorCode("NULL point for Polyline");
      return NULL;
    }
    aCI.SetPoint(ind, aRefPnt);
  }

  //Compute the Polyline value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Polyline driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aPolyline->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakePolyline([";
  it = thePoints.begin();
  TDF_Tool::Entry((*it)->GetEntry(), anEntry);
  it++;
  aDescr += (anEntry+", ");
  for (; it != thePoints.end(); it++) {
    aDescr += ", ";
    TDF_Tool::Entry((*it)->GetEntry(), anEntry);
    aDescr += anEntry;
  }
  aDescr += "])";

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aPolyline;
}

//=============================================================================
/*!
 *  MakeCircleThreePnt
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeCircleThreePnt (Handle(GEOM_Object) thePnt1,
                                                                    Handle(GEOM_Object) thePnt2,
                                                                    Handle(GEOM_Object) thePnt3)
{
  SetErrorCode(KO);

  if (thePnt1.IsNull() || thePnt2.IsNull() || thePnt3.IsNull()) return NULL;

  //Add a new Circle object
  Handle(GEOM_Object) aCircle = GetEngine()->AddObject(GetDocID(), GEOM_CIRCLE);

  //Add a new Circle function for creation a circle relatively to three points
  Handle(GEOM_Function) aFunction =
    aCircle->AddFunction(GEOMImpl_CircleDriver::GetID(), CIRCLE_THREE_PNT);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_CircleDriver::GetID()) return NULL;

  GEOMImpl_ICircle aCI (aFunction);

  Handle(GEOM_Function) aRefPnt1 = thePnt1->GetLastFunction();
  Handle(GEOM_Function) aRefPnt2 = thePnt2->GetLastFunction();
  Handle(GEOM_Function) aRefPnt3 = thePnt3->GetLastFunction();

  if (aRefPnt1.IsNull() || aRefPnt2.IsNull() || aRefPnt3.IsNull()) return NULL;

  aCI.SetPoint1(aRefPnt1);
  aCI.SetPoint2(aRefPnt2);
  aCI.SetPoint3(aRefPnt3);

  //Compute the Circle value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Circle driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aCircle->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeCircleThreePnt(";
  TDF_Tool::Entry(thePnt1->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePnt2->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePnt3->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aCircle;
}

//=============================================================================
/*!
 *  MakeCirclePntVecR
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeCirclePntVecR
       (Handle(GEOM_Object) thePnt, Handle(GEOM_Object) theVec, double theR)
{
  SetErrorCode(KO);

  if (thePnt.IsNull() || theVec.IsNull()) return NULL;

  //Add a new Circle object
  Handle(GEOM_Object) aCircle = GetEngine()->AddObject(GetDocID(), GEOM_CIRCLE);

  //Add a new Circle function for creation a circle relatively to point and vector
  Handle(GEOM_Function) aFunction =
    aCircle->AddFunction(GEOMImpl_CircleDriver::GetID(), CIRCLE_PNT_VEC_R);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_CircleDriver::GetID()) return NULL;

  GEOMImpl_ICircle aCI (aFunction);

  Handle(GEOM_Function) aRefPnt = thePnt->GetLastFunction();
  Handle(GEOM_Function) aRefVec = theVec->GetLastFunction();

  if (aRefPnt.IsNull() || aRefVec.IsNull()) return NULL;

  aCI.SetCenter(aRefPnt);
  aCI.SetVector(aRefVec);
  aCI.SetRadius(theR);

  //Compute the Circle value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Circle driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aCircle->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeCirclePntVecR(";
  TDF_Tool::Entry(thePnt->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(theVec->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  aDescr += (TCollection_AsciiString(theR)+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aCircle;
}

//=============================================================================
/*!
 *  MakeEllipse
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeEllipse
                       (Handle(GEOM_Object) thePnt, Handle(GEOM_Object) theVec,
                        double theRMajor, double theRMinor)
{
  SetErrorCode(KO);

  if (thePnt.IsNull() || theVec.IsNull()) return NULL;

  //Add a new Ellipse object
  Handle(GEOM_Object) anEll = GetEngine()->AddObject(GetDocID(), GEOM_ELLIPSE);

  //Add a new Ellipse function
  Handle(GEOM_Function) aFunction =
    anEll->AddFunction(GEOMImpl_EllipseDriver::GetID(), ELLIPSE_PNT_VEC_RR);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_EllipseDriver::GetID()) return NULL;

  GEOMImpl_IEllipse aCI (aFunction);

  Handle(GEOM_Function) aRefPnt = thePnt->GetLastFunction();
  Handle(GEOM_Function) aRefVec = theVec->GetLastFunction();

  if (aRefPnt.IsNull() || aRefVec.IsNull()) return NULL;

  aCI.SetCenter(aRefPnt);
  aCI.SetVector(aRefVec);
  aCI.SetRMajor(theRMajor);
  aCI.SetRMinor(theRMinor);

  //Compute the Ellipse value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Ellipse driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(anEll->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeEllipse(";
  TDF_Tool::Entry(thePnt->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(theVec->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  aDescr += (TCollection_AsciiString(theRMajor)+", ");
  aDescr += (TCollection_AsciiString(theRMinor)+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return anEll;
}

//=============================================================================
/*!
 *  MakeArc
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeArc (Handle(GEOM_Object) thePnt1,
                                                         Handle(GEOM_Object) thePnt2,
                                                         Handle(GEOM_Object) thePnt3)
{
  SetErrorCode(KO);

  if (thePnt1.IsNull() || thePnt2.IsNull() || thePnt3.IsNull()) return NULL;

  //Add a new Circle Arc object
  Handle(GEOM_Object) anArc = GetEngine()->AddObject(GetDocID(), GEOM_CIRC_ARC);

  //Add a new Circle Arc function
  Handle(GEOM_Function) aFunction =
    anArc->AddFunction(GEOMImpl_ArcDriver::GetID(), CIRC_ARC_THREE_PNT);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ArcDriver::GetID()) return NULL;

  GEOMImpl_IArc aCI (aFunction);

  Handle(GEOM_Function) aRefPnt1 = thePnt1->GetLastFunction();
  Handle(GEOM_Function) aRefPnt2 = thePnt2->GetLastFunction();
  Handle(GEOM_Function) aRefPnt3 = thePnt3->GetLastFunction();

  if (aRefPnt1.IsNull() || aRefPnt2.IsNull() || aRefPnt3.IsNull()) return NULL;

  aCI.SetPoint1(aRefPnt1);
  aCI.SetPoint2(aRefPnt2);
  aCI.SetPoint3(aRefPnt3);

  //Compute the Arc value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Arc driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(anArc->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeArc(";
  TDF_Tool::Entry(thePnt1->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePnt2->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePnt3->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return anArc;
}

//=============================================================================
/*!
 *  MakeSplineBezier
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeSplineBezier
                                          (list<Handle(GEOM_Object)> thePoints)
{
  SetErrorCode(KO);

  //Add a new Spline object
  Handle(GEOM_Object) aSpline = GetEngine()->AddObject(GetDocID(), GEOM_SPLINE);

  //Add a new Spline function for creation a bezier curve relatively to points set
  Handle(GEOM_Function) aFunction =
    aSpline->AddFunction(GEOMImpl_SplineDriver::GetID(), SPLINE_BEZIER);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_SplineDriver::GetID()) return NULL;

  GEOMImpl_ISpline aCI (aFunction);

  int aLen = thePoints.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<Handle(GEOM_Object)>::iterator it = thePoints.begin();
  for (; it != thePoints.end(); it++, ind++) {
    Handle(GEOM_Function) aRefPnt = (*it)->GetLastFunction();

    if (aRefPnt.IsNull()) return NULL;

    aCI.SetPoint(ind, aRefPnt);
  }

  //Compute the Spline value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Spline driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aSpline->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeSplineBezier([";
  it = thePoints.begin();
  TDF_Tool::Entry((*it)->GetEntry(), anEntry);
  it++;
  aDescr += (anEntry+", ");
  for (; it != thePoints.end(); it++) {
    aDescr += ", ";
    TDF_Tool::Entry((*it)->GetEntry(), anEntry);
    aDescr += anEntry;
  }
  aDescr += "])";

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aSpline;
}

//=============================================================================
/*!
 *  MakeSplineInterpolation
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeSplineInterpolation
                                          (list<Handle(GEOM_Object)> thePoints)
{
  SetErrorCode(KO);

  //Add a new Spline object
  Handle(GEOM_Object) aSpline = GetEngine()->AddObject(GetDocID(), GEOM_SPLINE);

  //Add a new Spline function for creation a bezier curve relatively to points set
  Handle(GEOM_Function) aFunction =
    aSpline->AddFunction(GEOMImpl_SplineDriver::GetID(), SPLINE_INTERPOLATION);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_SplineDriver::GetID()) return NULL;

  GEOMImpl_ISpline aCI (aFunction);

  int aLen = thePoints.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<Handle(GEOM_Object)>::iterator it = thePoints.begin();
  for (; it != thePoints.end(); it++, ind++) {
    Handle(GEOM_Function) aRefPnt = (*it)->GetLastFunction();

    if (aRefPnt.IsNull()) return NULL;

    aCI.SetPoint(ind, aRefPnt);
  }

  //Compute the Spline value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Spline driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aSpline->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeSplineInterpolation([";
  it = thePoints.begin();
  TDF_Tool::Entry((*it)->GetEntry(), anEntry);
  it++;
  aDescr += (anEntry+", ");
  for (; it != thePoints.end(); it++) {
    aDescr += ", ";
    TDF_Tool::Entry((*it)->GetEntry(), anEntry);
    aDescr += anEntry;
  }
  aDescr += "])";

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aSpline;
}

//=============================================================================
/*!
 *  MakeSketcher
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ICurvesOperations::MakeSketcher (const char* theCommand,
                                                              list<double> theWorkingPlane)
{
  SetErrorCode(KO);

  if (!theCommand) return NULL;
  if (strcmp(theCommand, "") == 0) return NULL;

  //Add a new Sketcher object
  Handle(GEOM_Object) aSketcher = GetEngine()->AddObject(GetDocID(), GEOM_SKETCHER);

  //Add a new Sketcher function
  Handle(GEOM_Function) aFunction =
    aSketcher->AddFunction(GEOMImpl_SketcherDriver::GetID(), SKETCHER_COMMAND);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_SketcherDriver::GetID()) return NULL;

  GEOMImpl_ISketcher aCI (aFunction);

  TCollection_AsciiString aCommand ((char*) theCommand);
  aCI.SetCommand(aCommand);

  int ind = 1;
  list<double>::iterator it = theWorkingPlane.begin();
  for (; it != theWorkingPlane.end(); it++, ind++)
    aCI.SetWorkingPlane(ind, *it);

  //Compute the Sketcher value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Sketcher driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aSketcher->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = ICurvesOperations.MakeSketcher(";
  aDescr += aCommand+", [";

  it = theWorkingPlane.begin();
  aDescr += TCollection_AsciiString(*it);
  it++;
  for (; it != theWorkingPlane.end(); it++) {
    aDescr += ", ";
    aDescr += TCollection_AsciiString(*it);
  }

  aDescr += "])";

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aSketcher;
}
