//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : BasicGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "BasicGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewPort3d.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include "BasicGUI_PointDlg.h"        // Method POINT
#include "BasicGUI_LineDlg.h"         // Method LINE
#include "BasicGUI_CircleDlg.h"       // Method CIRCLE
#include "BasicGUI_EllipseDlg.h"      // Method ELLIPSE
#include "BasicGUI_ArcDlg.h"          // Method ARC
#include "BasicGUI_VectorDlg.h"       // Method VECTOR
#include "BasicGUI_PlaneDlg.h"        // Method PLANE
#include "BasicGUI_WorkingPlaneDlg.h" // Method WORKING PLANE

//=======================================================================
// function : BasicGUI()
// purpose  : Constructor
//=======================================================================
BasicGUI::BasicGUI() :
  QObject()
{
  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeom = myGeomGUI->myComponentGeom;
}


//=======================================================================
// function : ~BasicGUI()
// purpose  : Destructor
//=======================================================================
BasicGUI::~BasicGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BasicGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  BasicGUI* myBasicGUI = new BasicGUI();
  myBasicGUI->myGeomGUI->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  switch (theCommandID)
    {
    case 4011: // POINT
      {
	Handle(AIS_InteractiveContext) ic;
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	BasicGUI_PointDlg *aDlg = new BasicGUI_PointDlg(parent, "", myBasicGUI, Sel, ic); 
	break;
      }
    case 4012:  // LINE
      {
  	BasicGUI_LineDlg *aDlg = new BasicGUI_LineDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    case 4013:  // CIRCLE
      {
  	BasicGUI_CircleDlg *aDlg = new BasicGUI_CircleDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    case 4014:  // ELLIPSE
      {
  	BasicGUI_EllipseDlg *aDlg = new BasicGUI_EllipseDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    case 4015:  // ARC
      {
  	BasicGUI_ArcDlg *aDlg = new BasicGUI_ArcDlg(parent, "", myBasicGUI, Sel);
	break ;
      }
    case 4016: // VECTOR
      {	
  	BasicGUI_VectorDlg *aDlg = new BasicGUI_VectorDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    case 4017: // PLANE
      {	
  	BasicGUI_PlaneDlg *aDlg = new BasicGUI_PlaneDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    case 4018: // WORKING PLANE
      {	
	BasicGUI_WorkingPlaneDlg *aDlg = new BasicGUI_WorkingPlaneDlg(parent, "", myBasicGUI, Sel);
	break;
      }
    default:
      {
	parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
    }
  return true;
}


//=================================================================================
// function : 0nMousePress()
// purpose  : [static] manage mouse events
//=================================================================================
bool BasicGUI::OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
{
  BasicGUI* myBasicGUI = new BasicGUI();

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;

  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)studyFrame->getRightFrame()->getViewFrame())->getViewPort();
  
  /* Get the clicked or selected point */
  gp_Pnt thePoint;

  myBasicGUI->myGeomBase->EraseSimulationShape();
  BasicGUI_PointDlg *DialogPt = (BasicGUI_PointDlg*)(myBasicGUI->myGeomGUI->GetActiveDialogBox());

  if(DialogPt->UseLocalContext()) {
    ic->InitSelected();
    if(pe->state() == Qt::ShiftButton)
      v3d->getAISSelector()->shiftSelect();  /* Append selection */
    else
      v3d->getAISSelector()->select();       /* New selection    */
      
    ic->InitSelected();
    if(ic->MoreSelected())
      thePoint = BRep_Tool::Pnt(TopoDS::Vertex(ic->SelectedShape()));
    else
      thePoint = myBasicGUI->ConvertClickToPoint(pe->x(), pe->y(), ((OCCViewer_ViewPort3d*)vp)->getView());
  } 
  else
    thePoint = myBasicGUI->ConvertClickToPoint(pe->x(), pe->y(), ((OCCViewer_ViewPort3d*)vp)->getView());

  if(DialogPt != 0)
    DialogPt->PointIntoCoordinates(thePoint, true);  /* display point */
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));

  return false;
}


//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt BasicGUI::ConvertClickToPoint(Standard_Real x, Standard_Real y, Handle(V3d_View) aView)
{
  V3d_Coordinate XEye, YEye, ZEye, XAt, YAt, ZAt;
  aView->Eye(XEye, YEye, ZEye);

  aView->At(XAt, YAt, ZAt);
  gp_Pnt EyePoint(XEye, YEye, ZEye);
  gp_Pnt AtPoint(XAt, YAt, ZAt);

  gp_Vec EyeVector(EyePoint, AtPoint);
  gp_Dir EyeDir(EyeVector);

  gp_Pln PlaneOfTheView = gp_Pln(AtPoint,EyeDir);
  Standard_Real X, Y, Z;
  aView->Convert(int(x), int(y), X, Y, Z);
  gp_Pnt ConvertedPoint(X, Y, Z);

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView);
  return ResultPoint;
}


//=======================================================================
// function : MakePointAndDisplay
// purpose  : 
//=======================================================================
void BasicGUI::MakePointAndDisplay(const double x, const double y, const double z) 
{
  try {
    GEOM::GEOM_Shape_var P = myGeom->MakeVertex(x, y, z);
    P->NameType(tr("GEOM_VERTEX"));
    if (myGeomBase->Display(P))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeLineAndDisplay()
// purpose  : Create an infinite oriented line (linear edge in fact) 
//=====================================================================================
void BasicGUI::MakeLineAndDisplay(const gp_Pnt InitPoint, const gp_Pnt LastPoint)
{
  gp_Pnt P1, P2;  
  double dx, dy, dz;
  myGeomBase->GetBipointDxDyDz(InitPoint, LastPoint, dx, dy, dz);
  Standard_Real length = InitPoint.Distance(LastPoint);
  if(length <= Precision::Confusion()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
    return;
  }
  
  Standard_Real coeff = 1E6 / length;
  
  /* To create a line with length = 1E6	    */
  /* Precision::Infinite() is 1E100 in OCC  */
  P1.SetX(InitPoint.X() - (coeff * dx));
  P1.SetY(InitPoint.Y() - (coeff * dy));
  P1.SetZ(InitPoint.Z() - (coeff * dz));
  
  P2.SetX(LastPoint.X() + (coeff * dx));
  P2.SetY(LastPoint.Y() + (coeff * dy));
  P2.SetZ(LastPoint.Z() + (coeff * dz));
  
  try {
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(P2.X(), P2.Y(), P2.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    
    GEOM::GEOM_Shape_ptr result = myGeom->MakeLine(pstruct, dstruct);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_LINE"));
    
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeCircleAndDisplay()
// purpose  :
//=====================================================================================
void BasicGUI::MakeCircleAndDisplay(const gp_Pnt CenterPoint, const gp_Dir dir, const Standard_Real Radius)
{
  try {
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(dir.X(), dir.Y(), dir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    
    GEOM::GEOM_Shape_var result = myGeom->MakeCircle(pstruct, dstruct, Radius);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_CIRCLE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=====================================================================================
// function : MakeEllipseAndDisplay()
// purpose  :
//=====================================================================================
void BasicGUI::MakeEllipseAndDisplay(const gp_Pnt CenterPoint, const gp_Dir dir,
				     const Standard_Real Major_Radius, const Standard_Real Minor_Radius)
{
  try {
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(CenterPoint.X(), CenterPoint.Y(), CenterPoint.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(dir.X(), dir.Y(), dir.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d) ;

    GEOM::GEOM_Shape_var result = myGeom->MakeEllipse(pstruct, dstruct, Major_Radius, Minor_Radius);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_ELLIPSE"));
    if(myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=======================================================================================
// function : MakeArcAndDisplay()
// purpose  : Make an arc of circle from InitPoint to CirclePoint and passing on EndPoint
//=======================================================================================
void BasicGUI::MakeArcAndDisplay(gp_Pnt InitPoint, gp_Pnt CirclePoint, gp_Pnt EndPoint)
{
  gp_Vec v1(CirclePoint, InitPoint);
  gp_Vec v2(CirclePoint, EndPoint);
  if(v1.IsParallel(v2, Precision::Angular()))
    return;

  try { 
    GEOM::PointStruct pI = myGeom->MakePointStruct(InitPoint.X(), InitPoint.Y(), InitPoint.Z());
    GEOM::PointStruct pC = myGeom->MakePointStruct(CirclePoint.X(), CirclePoint.Y(), CirclePoint.Z());
    GEOM::PointStruct pE = myGeom->MakePointStruct(EndPoint.X(), EndPoint.Y(), EndPoint.Z());
    GEOM::GEOM_Shape_var result = myGeom->MakeArc(pI, pC, pE);
    if(result->_is_nil()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
      return;
    }
    result->NameType(tr("GEOM_ARC"));
    if (myGeomBase->Display(result))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }    
  return;
}


//=======================================================================
// function : MakeVectorAndDisplay()
// purpose  : 
//=======================================================================
void BasicGUI::MakeVectorAndDisplay(const gp_Pnt P1, const gp_Pnt P2)
{
  try {
    GEOM::PointStruct pstruct1 = myGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z());
    GEOM::PointStruct pstruct2 = myGeom->MakePointStruct(P2.X(), P2.Y(), P2.Z());
    GEOM::GEOM_Shape_var Vector = myGeom->MakeVector(pstruct1, pstruct2);
    Vector->NameType(tr("GEOM_VECTOR"));
    if(myGeomBase->Display(Vector))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=======================================================================
// function : MakePlaneAndDisplay()
// purpose  : Plane point is P1 and dx, dy, dz define a normal vector
//=======================================================================
void BasicGUI::MakePlaneAndDisplay(const gp_Pnt P1, const Standard_Real dx, const Standard_Real dy,
				   const Standard_Real dz, const Standard_Real TrimSize)
{
  try {
    gp_Dir aDirection;
    aDirection.SetCoord(dx, dy, dz);
    gp_Ax2 Axis(P1, aDirection);
    
    GEOM::PointStruct pstruct = myGeom->MakePointStruct(P1.X(), P1.Y(), P1.Z());
    GEOM::PointStruct d = myGeom->MakePointStruct(aDirection.X(), aDirection.Y(), aDirection.Z());
    GEOM::DirStruct dstruct = myGeom->MakeDirection(d);
    GEOM::GEOM_Shape_ptr plane = myGeom->MakePlane(pstruct, dstruct, TrimSize);
    plane->NameType(tr("GEOM_PLANE"));
    if(myGeomBase->Display(plane))
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  }
  catch(const SALOME::SALOME_Exception& S_ex) {
    QtCatchCorbaException(S_ex);
  }
  return;
}


//=======================================================================
// function : MakeWorkingPlane()
// purpose  : Change the point of view3d
//=======================================================================
void BasicGUI::MakeWorkingPlane(const gp_Pnt P, const gp_Dir D)
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER"));
    return;
  }

  OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewPort();
  Handle(V3d_View) view3d = ((OCCViewer_ViewPort3d*)vp)->getView();
  view3d->SetAt(P.X(), P.Y(), P.Z());
  view3d->SetProj(D.X(), D.Y(), D.Z());
  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  return;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent)
  {return BasicGUI::OnGUIEvent(theCommandID, parent);}

  bool OnMousePress(QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame)
  {return BasicGUI::OnMousePress(pe, parent, studyFrame);}
}
