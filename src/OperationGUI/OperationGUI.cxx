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
//  File   : OperationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "OperationGUI.h"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SalomeApp_Application.h"

#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <Precision.hxx>

#include "OperationGUI_PartitionDlg.h"   // Method PARTITION
#include "OperationGUI_ArchimedeDlg.h"   // Method ARCHIMEDE
#include "OperationGUI_FilletDlg.h"      // Method FILLET
#include "OperationGUI_ChamferDlg.h"     // Method CHAMFER

#include "OperationGUI_ClippingDlg.h"

using namespace std;

//=======================================================================
// function : OperationGUI()
// purpose  : Constructor
//=======================================================================
OperationGUI::OperationGUI(GeometryGUI* parent) : GEOMGUI(parent)
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
bool OperationGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  switch (theCommandID)
  {
  case 503: (new OperationGUI_PartitionDlg(getGeometryGUI(), parent))->show(); break;
  case 504: (new OperationGUI_ArchimedeDlg(getGeometryGUI(), parent))->show(); break;
  case 505: (new OperationGUI_FilletDlg   (getGeometryGUI(), parent))->show(); break;
  case 506: (new OperationGUI_ChamferDlg  (getGeometryGUI(), parent))->show(); break;
  case 507: (new OperationGUI_ClippingDlg (getGeometryGUI(), parent))->show(); break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
  }

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
  GEOMGUI* GetLibGUI(GeometryGUI* parent)
  {
    return new OperationGUI(parent);
  }
}
