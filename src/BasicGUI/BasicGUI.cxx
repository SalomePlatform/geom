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
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BasicGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "BasicGUI.h"
#include "GeometryGUI.h"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewPort3d.h"
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

using namespace std;

//=======================================================================
// function : BasicGUI()
// purpose  : Constructor
//=======================================================================
BasicGUI::BasicGUI( GeometryGUI* parent ) :
  GEOMGUI(parent)
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
bool BasicGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  getGeometryGUI()->EmitSignalDeactivateDialog();
  QDialog* aDlg = NULL;

  switch ( theCommandID )
    {
    case 4011: // POINT
      aDlg = new BasicGUI_PointDlg( getGeometryGUI(), parent, "" ); 
      break;
    case 4012:  // LINE
      aDlg = new BasicGUI_LineDlg( getGeometryGUI(), parent, "" );
      break;
    case 4013:  // CIRCLE
      aDlg = new BasicGUI_CircleDlg( getGeometryGUI(), parent, "");
      break;
    case 4014:  // ELLIPSE
      aDlg = new BasicGUI_EllipseDlg( getGeometryGUI(), parent, "" );
      break;
    case 4015:  // ARC
      aDlg = new BasicGUI_ArcDlg( getGeometryGUI(), parent, "" );
      break ;
    case 4016: // VECTOR
      aDlg = new BasicGUI_VectorDlg( getGeometryGUI(), parent, "" );
      break;
    case 4017: // PLANE
      aDlg = new BasicGUI_PlaneDlg( getGeometryGUI(), parent, "");
      break;
    case 4018: // WORKING PLANE
      aDlg = new BasicGUI_WorkingPlaneDlg( getGeometryGUI(), parent, "" );
      break;
    case 4019: // CURVE
      aDlg = new BasicGUI_CurveDlg( getGeometryGUI(), parent, "" );
      break;
    case 4020: // REPAIR
      aDlg = new BasicGUI_MarkerDlg( getGeometryGUI(), parent );
      break;      
    default:
      SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
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
bool BasicGUI::OnMousePress( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow )
{
  QDialog* aDlg = getGeometryGUI()->GetActiveDialogBox();

  // Create Point dialog, OCC viewer 
  if ( aDlg && aDlg->isA( "BasicGUI_PointDlg" ) &&
       theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->state() != Qt::ControlButton )
  {
    BasicGUI_PointDlg* aPntDlg = (BasicGUI_PointDlg*) aDlg;
    if ( aPntDlg->acceptMouseEvent() )
    {
      OCCViewer_Viewer* anOCCViewer =
        ((OCCViewer_ViewManager*)(theViewWindow->getViewManager()))->getOCCViewer();
      Handle(AIS_InteractiveContext) ic = anOCCViewer->getAISContext();

      gp_Pnt aPnt;    

      ic->InitSelected();
      if ( pe->state() == Qt::ShiftButton )
        ic->ShiftSelect();  // Append selection
      else
        ic->Select();       // New selection

      ic->InitSelected();
      if ( ic->MoreSelected() )
      {
        TopoDS_Shape aShape = ic->SelectedShape();
        if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
          aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
      }
      else
      {
        OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
        aPnt = ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
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
GEOM_BASICGUI_EXPORT
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new BasicGUI( parent );
  }
}
