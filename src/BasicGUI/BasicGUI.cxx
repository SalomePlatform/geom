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
//  $Header$

using namespace std;
#include "BasicGUI.h"
#include "GeometryGUI.h"

#include "QAD_RightFrame.h"
#include "QAD_Desktop.h"
#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_AISSelector.h"
#include "OCCViewer_ViewPort3d.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "utilities.h"

#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <TopoDS.hxx>

#include "BasicGUI_PointDlg.h"        // Method POINT
#include "BasicGUI_LineDlg.h"         // Method LINE
#include "BasicGUI_CircleDlg.h"       // Method CIRCLE
#include "BasicGUI_EllipseDlg.h"      // Method ELLIPSE
#include "BasicGUI_ArcDlg.h"          // Method ARC
#include "BasicGUI_CurveDlg.h"        // Method CURVE
#include "BasicGUI_VectorDlg.h"       // Method VECTOR
#include "BasicGUI_PlaneDlg.h"        // Method PLANE
#include "BasicGUI_WorkingPlaneDlg.h" // Method WORKING PLANE
#include "BasicGUI_MarkerDlg.h"       // Method REPAIR

BasicGUI* BasicGUI::myGUIObject = 0;

//=======================================================================
// function : GetBasicGUI()
// purpose  : Get the only BasicGUI object [ static ]
//=======================================================================
BasicGUI* BasicGUI::GetBasicGUI()
{
  if ( myGUIObject == 0 ) {
    myGUIObject = new BasicGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : BasicGUI()
// purpose  : Constructor
//=======================================================================
BasicGUI::BasicGUI() :
  GEOMGUI()
{
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
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  QDialog* aDlg = NULL;

  switch ( theCommandID )
    {
    case 4011: // POINT
      {
	Handle(AIS_InteractiveContext) ic;
	QAD_StudyFrame* aFrame = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame();
	if ( aFrame->getTypeView() == VIEW_OCC) 
	{
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)aFrame->getRightFrame()->getViewFrame())->getViewer();
	  ic = v3d->getAISContext();
	}
	aDlg = new BasicGUI_PointDlg( parent, "", Sel ); 
	break;
      }
    case 4012:  // LINE
      aDlg = new BasicGUI_LineDlg(parent, "",  Sel);
      break;
    case 4013:  // CIRCLE
      aDlg = new BasicGUI_CircleDlg(parent, "", Sel);
      break;
    case 4014:  // ELLIPSE
      aDlg = new BasicGUI_EllipseDlg(parent, "", Sel);
      break;
    case 4015:  // ARC
      aDlg = new BasicGUI_ArcDlg(parent, "", Sel);
      break ;
    case 4016: // VECTOR
      aDlg = new BasicGUI_VectorDlg(parent, "", Sel);
      break;
    case 4017: // PLANE
      aDlg = new BasicGUI_PlaneDlg(parent, "", Sel);
      break;
    case 4018: // WORKING PLANE
      aDlg = new BasicGUI_WorkingPlaneDlg(parent, "", Sel);
      break;
    case 4019: // CURVE
      aDlg = new BasicGUI_CurveDlg( parent, "", Sel );
      break;
    case 4020: // REPAIR
      aDlg = new BasicGUI_MarkerDlg( parent, Sel );
      break;      
    default:
      parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
      break;
    }

  if ( aDlg != NULL )
    aDlg->show();

  return true;
}


//=================================================================================
// function : 0nMousePress()
// purpose  : [static] manage mouse events
//=================================================================================
bool BasicGUI::OnMousePress( QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* theFrame )
{
  QDialog* aDlg = GeometryGUI::GetGeomGUI()->GetActiveDialogBox();

  // Create Point dialog, OCC viewer 
  if ( aDlg && aDlg->isA( "BasicGUI_PointDlg" ) && theFrame->getTypeView() == VIEW_OCC )
  {
    BasicGUI_PointDlg* aPntDlg = (BasicGUI_PointDlg*) aDlg;
    if ( aPntDlg->acceptMouseEvent() )
		{
    	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)theFrame->getRightFrame()->getViewFrame())->getViewer();
    	Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
      
    	gp_Pnt aPnt;    

    	ic->InitSelected();
    	if( pe->state() == Qt::ShiftButton )
      	v3d->getAISSelector()->shiftSelect();  // Append selection
    	else
      	v3d->getAISSelector()->select();       // New selection

    	ic->InitSelected();
    	if( ic->MoreSelected() )
    	{
      	TopoDS_Shape aShape = ic->SelectedShape();
      	if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
        	aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
    	}
    	else
     	{
     		OCCViewer_ViewPort* vp = ((OCCViewer_ViewFrame*)theFrame->getRightFrame()->getViewFrame())->getViewPort();
      	aPnt = ConvertClickToPoint( pe->x(), pe->y(), ((OCCViewer_ViewPort3d*)vp)->getView() );
    	}

    	aPntDlg->OnPointSelected( aPnt );  // "feed" the point to point construction dialog
    } // acceptMouseEvent()
  } 
  return false;
}


//=======================================================================
// function : ConvertClickToPoint()
// purpose  : Returns the point clicked in 3D view
//=======================================================================
gp_Pnt BasicGUI::ConvertClickToPoint( int x, int y, Handle(V3d_View) aView)
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
  aView->Convert(x, y, X, Y, Z);
  gp_Pnt ConvertedPoint(X, Y, Z);

  gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);
  gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(), PlaneOfTheView);
  return ResultPoint;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  GEOMGUI* GetLibGUI()
  {
    return BasicGUI::GetBasicGUI();
  }
}
