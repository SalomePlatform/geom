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
//  File   : TransformationGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "TransformationGUI.h"
#include "GeometryGUI.h"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"

#include "SalomeApp_Application.h"

#include "TransformationGUI_MultiTranslationDlg.h"   // Method MULTI TRANSLATION
#include "TransformationGUI_MultiRotationDlg.h"      // Method MULTI ROTATION
#include "TransformationGUI_TranslationDlg.h"        // Method TRANSLATION
#include "TransformationGUI_RotationDlg.h"           // Method ROTATION
#include "TransformationGUI_MirrorDlg.h"             // Method MIRROR
#include "TransformationGUI_ScaleDlg.h"              // Method SCALE
#include "TransformationGUI_OffsetDlg.h"             // Method OFFSET
#include "TransformationGUI_PositionDlg.h"           // Method POSITION

using namespace std;

//=======================================================================
// function : TransformationGUI()
// purpose  : Constructor
//=======================================================================
TransformationGUI::TransformationGUI(GeometryGUI* parent) : GEOMGUI(parent)
{
}

//=======================================================================
// function : ~TransformationGUI()
// purpose  : Destructor
//=======================================================================
TransformationGUI::~TransformationGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool TransformationGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();
  QDialog* aDlg = NULL;

  switch (theCommandID)
  {
  case 5021: // TRANSLATION
    aDlg = new TransformationGUI_TranslationDlg( getGeometryGUI(), parent, "" );
    break;
  case 5022: // ROTATION
    aDlg = new TransformationGUI_RotationDlg( getGeometryGUI(), parent, "" );
    break;
  case 5023: // POSITION
    aDlg = new TransformationGUI_PositionDlg( getGeometryGUI(), parent, "" );
    break;
  case 5024: // MIRROR
    aDlg = new TransformationGUI_MirrorDlg( getGeometryGUI(), parent, "" );
    break;
  case 5025: // SCALE
    aDlg = new TransformationGUI_ScaleDlg( getGeometryGUI(), parent, "" );
    break;
  case 5026: // OFFSET
    aDlg = new TransformationGUI_OffsetDlg( getGeometryGUI(), parent, "" );
    break;
  case 5027: // MULTI TRANSLATION
    aDlg = new TransformationGUI_MultiTranslationDlg( getGeometryGUI(), parent, "" );
    break;
  case 5028: // MULTI ROTATION
    aDlg = new TransformationGUI_MultiRotationDlg( getGeometryGUI(), parent, "" );
    break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }

  if (aDlg != NULL)
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
    return new TransformationGUI( parent );
  }
}
