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
//  File   : PrimitiveGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "PrimitiveGUI.h"
#include "GeometryGUI.h"

#include "QAD_Application.h"
#include "QAD_Desktop.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "PrimitiveGUI_BoxDlg.h"      // Method BOX
#include "PrimitiveGUI_CylinderDlg.h" // Method CYLINDER
#include "PrimitiveGUI_SphereDlg.h"   // Method SPHERE
#include "PrimitiveGUI_TorusDlg.h"    // Method TORUS
#include "PrimitiveGUI_ConeDlg.h"     // Method CONE

PrimitiveGUI* PrimitiveGUI::myGUIObject = 0;

//=======================================================================
// function : GetPrimitiveGUI()
// purpose  : Get the only PrimitiveGUI object [ static ]
//=======================================================================
PrimitiveGUI* PrimitiveGUI::GetPrimitiveGUI()
{
  if ( myGUIObject == 0 ) {
    // init PrimitiveGUI only once
    myGUIObject = new PrimitiveGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : PrimitiveGUI()
// purpose  : Constructor
//=======================================================================
PrimitiveGUI::PrimitiveGUI() : GEOMGUI()
{
}


//=======================================================================
// function : ~PrimitiveGUI
// purpose  : Destructor
//=======================================================================
PrimitiveGUI::~PrimitiveGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool PrimitiveGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  PrimitiveGUI* aPrimitiveGUI = GetPrimitiveGUI();
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  
  QDialog* aDlg = NULL;

  switch (theCommandID)
    {
    case 4021: // BOX
      {
	aDlg = new PrimitiveGUI_BoxDlg(parent, "", Sel);
	break;
      }
    case 4022: // CYLINDER
      {
	aDlg = new PrimitiveGUI_CylinderDlg(parent, "", Sel);
	break;
      }
    case 4023: // SPHERE
      {
	aDlg = new PrimitiveGUI_SphereDlg(parent, "", Sel);
	break;
      }
    case 4024: // TORUS
      {
	aDlg = new PrimitiveGUI_TorusDlg(parent, "", Sel);
	break;
      }
    case 4025: // CONE
      {
	aDlg = new PrimitiveGUI_ConeDlg(parent, "", Sel);
	break;
      }
    default:
      {
	parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
    }

  if ( aDlg != NULL )
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
    return PrimitiveGUI::GetPrimitiveGUI();
  }
}
