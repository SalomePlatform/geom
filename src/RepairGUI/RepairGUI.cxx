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
//  File   : RepairGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "RepairGUI.h"

#include "QAD_RightFrame.h"
#include "QAD_Desktop.h"
#include "OCCViewer_Viewer3d.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "RepairGUI_SewingDlg.h"        // Method SEWING
#include "RepairGUI_SuppressFacesDlg.h" // Method SUPPRESS FACES
#include "RepairGUI_RemoveHolesDlg.h"   // Method SUPPRESS HOLE
#include "RepairGUI_ShapeProcessDlg.h"  // Method PROCESS SHAPE
#include "RepairGUI_CloseContourDlg.h"  // Method CLOSE CONTOUR
#include "RepairGUI_RemoveIntWiresDlg.h"// Method REMOVE INTERNAL WIRES
#include "RepairGUI_DivideEdgeDlg.h"    // Method DEVIDE EDGE
#include "RepairGUI_FreeBoundDlg.h"     // Method FREE BOUNDARIES
#include "RepairGUI_GlueDlg.h"          // Method GLUE FACES

#include "utilities.h"

using namespace std;

RepairGUI* RepairGUI::myGUIObject = 0;

//=======================================================================
// function : GetRepairGUI()
// purpose  : Get the only RepairGUI object [ static ]
//=======================================================================
RepairGUI* RepairGUI::GetRepairGUI()
{
  if ( myGUIObject == 0 ) {
    // init RepairGUI only once
    myGUIObject = new RepairGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : RepairGUI()
// purpose  : Constructor
//=======================================================================
RepairGUI::RepairGUI() : GEOMGUI()
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
bool RepairGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  QDialog* aDlg = NULL;
  switch (theCommandID) {
    case 601: // SEWING
      aDlg = new RepairGUI_SewingDlg( parent, "", Sel );
      break;
    case 602: // GLUE FACES
      aDlg = new RepairGUI_GlueDlg( parent, "", Sel );
      break;
    case 603: // SUPPRESS FACES
      aDlg = new RepairGUI_SuppressFacesDlg( parent, "", Sel);
      break;
    case 604: // SUPPRESS HOLES
      aDlg = new RepairGUI_RemoveHolesDlg( parent, "", Sel );
      break;
    case 605: // SHAPE PROCESSING
      aDlg = new RepairGUI_ShapeProcessDlg( parent, "", Sel );
      break;
    case 606: // CLOSE CONTOUR
      aDlg = new RepairGUI_CloseContourDlg( parent, "", Sel );
      break;
    case 607: // REMOVE INTERNAL WIRES
      aDlg = new RepairGUI_RemoveIntWiresDlg( parent, "", Sel );
      break;
    case 608: // ADD POINT ON EDGE
      aDlg = new RepairGUI_DivideEdgeDlg( parent, "", Sel );
      break;
    case 609: // FREE BOUNDARIES
      aDlg = new RepairGUI_FreeBoundDlg( parent, Sel );
      break;    
    default:
      parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
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
  GEOMGUI* GetLibGUI()
  {
    return RepairGUI::GetRepairGUI();
  }
}
