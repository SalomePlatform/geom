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
//  File   : OperationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "OperationGUI.h"

#include "QAD_RightFrame.h"
#include "QAD_Desktop.h"
#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewFrame.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>

#include "OperationGUI_PartitionDlg.h"   // Method PARTITION
#include "OperationGUI_ArchimedeDlg.h"   // Method ARCHIMEDE
#include "OperationGUI_FilletDlg.h"      // Method FILLET
#include "OperationGUI_ChamferDlg.h"     // Method CHAMFER

#include "OperationGUI_ClippingDlg.h"

OperationGUI* OperationGUI::myGUIObject = 0;

//=======================================================================
// function : GetOperationGUI()
// purpose  : Get the only OperationGUI object [ static ]
//=======================================================================
OperationGUI* OperationGUI::GetOperationGUI()
{
  if ( myGUIObject == 0 ) {
    // init OperationGUI only once
    myGUIObject = new OperationGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : OperationGUI()
// purpose  : Constructor
//=======================================================================
OperationGUI::OperationGUI() : GEOMGUI()
{
}


//=======================================================================
// function : ~OperationGUI()
// purpose  : Destructor
//=======================================================================
OperationGUI::~OperationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool OperationGUI::OnGUIEvent( int theCommandID, QAD_Desktop* parent )
{
  OperationGUI* myOperationGUI = GetOperationGUI();
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
  
  switch ( theCommandID )
  {
    case 503: // PARTITION
      ( new OperationGUI_PartitionDlg( parent, "", Sel ) )->show();
    break;
    case 504: // ARCHIMEDE
      new OperationGUI_ArchimedeDlg( parent, Sel );
    break;
    case 505: // FILLET
      new OperationGUI_FilletDlg( parent, Sel );	
    break;
    case 506: // CHAMFER
      new OperationGUI_ChamferDlg( parent, Sel );
    break;
    case 507: // CLIPPING RANGE
      ( new OperationGUI_ClippingDlg( parent, "" ) )->show();
    break;
    default:
      parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }
  
  return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  GEOMGUI* GetLibGUI()
  {
    return OperationGUI::GetOperationGUI();
  }
}
