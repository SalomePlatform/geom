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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : RepairGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "RepairGUI.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include "SalomeApp_Application.h"

#include "RepairGUI_SewingDlg.h"        // Method SEWING
#include "RepairGUI_SuppressFacesDlg.h" // Method SUPPRESS FACES
#include "RepairGUI_RemoveHolesDlg.h"   // Method SUPPRESS HOLE
#include "RepairGUI_ShapeProcessDlg.h"  // Method PROCESS SHAPE
#include "RepairGUI_CloseContourDlg.h"  // Method CLOSE CONTOUR
#include "RepairGUI_RemoveIntWiresDlg.h"// Method REMOVE INTERNAL WIRES
#include "RepairGUI_DivideEdgeDlg.h"    // Method DEVIDE EDGE
#include "RepairGUI_FreeBoundDlg.h"     // Method FREE BOUNDARIES
#include "RepairGUI_FreeFacesDlg.h"     // Method FREE FACES
#include "RepairGUI_GlueDlg.h"          // Method GLUE FACES
#include "RepairGUI_ChangeOrientationDlg.h" // Method CHANGE ORIENTATION

#include "utilities.h"

using namespace std;

//=======================================================================
// function : RepairGUI()
// purpose  : Constructor
//=======================================================================
RepairGUI::RepairGUI( GeometryGUI* parent ) : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~RepairGUI()
// purpose  : Destructor
//=======================================================================
RepairGUI::~RepairGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool RepairGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;
  switch (theCommandID) {
    case 601: aDlg = new RepairGUI_SewingDlg        (getGeometryGUI(), parent, ""); break;
    case 602: aDlg = new RepairGUI_GlueDlg          (getGeometryGUI(), parent, ""); break;
    case 603: aDlg = new RepairGUI_SuppressFacesDlg (getGeometryGUI(), parent, ""); break;
    case 604: aDlg = new RepairGUI_RemoveHolesDlg   (getGeometryGUI(), parent, ""); break;
    case 605: aDlg = new RepairGUI_ShapeProcessDlg  (getGeometryGUI(), parent, ""); break;
    case 606: aDlg = new RepairGUI_CloseContourDlg  (getGeometryGUI(), parent, ""); break;
    case 607: aDlg = new RepairGUI_RemoveIntWiresDlg(getGeometryGUI(), parent, ""); break;
    case 608: aDlg = new RepairGUI_DivideEdgeDlg    (getGeometryGUI(), parent, ""); break;
    case 609: aDlg = new RepairGUI_FreeBoundDlg     (getGeometryGUI(), parent, ""); break;
    case 610: aDlg = new RepairGUI_FreeFacesDlg     (getGeometryGUI(), parent, ""); break;
    case 611: aDlg = new RepairGUI_ChangeOrientationDlg (getGeometryGUI(), parent, ""); break;
    default:
      app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
      break;
  }

  if ( aDlg )
    aDlg->show();

  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WNT
	__declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new RepairGUI( parent );
  }
}
