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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : PrimitiveGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$


#include "PrimitiveGUI.h"
#include "GeometryGUI.h"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"

#include "PrimitiveGUI_BoxDlg.h"      // Method BOX
#include "PrimitiveGUI_CylinderDlg.h" // Method CYLINDER
#include "PrimitiveGUI_SphereDlg.h"   // Method SPHERE
#include "PrimitiveGUI_TorusDlg.h"    // Method TORUS
#include "PrimitiveGUI_ConeDlg.h"     // Method CONE
using namespace std;
PrimitiveGUI* PrimitiveGUI::myGUIObject = 0;

//=======================================================================
// function : GetPrimitiveGUI()
// purpose  : Get the only PrimitiveGUI object [ static ]
//=======================================================================
PrimitiveGUI* PrimitiveGUI::GetPrimitiveGUI( GeometryGUI* parent )
{
  if ( myGUIObject == 0 ) {
    // init PrimitiveGUI only once
    myGUIObject = new PrimitiveGUI( parent );
  }
  return myGUIObject;
}

//=======================================================================
// function : PrimitiveGUI()
// purpose  : Constructor
//=======================================================================
PrimitiveGUI::PrimitiveGUI(GeometryGUI* parent) : GEOMGUI(parent)
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
bool PrimitiveGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  getGeometryGUI()->EmitSignalDeactivateDialog();
  
  QDialog* aDlg = NULL;

  switch (theCommandID)
    {
    case 4021: // BOX
      {
	aDlg = new PrimitiveGUI_BoxDlg(getGeometryGUI(), parent, "");
	break;
      }
    case 4022: // CYLINDER
      {
	aDlg = new PrimitiveGUI_CylinderDlg(getGeometryGUI(), parent, "");
	break;
      }
    case 4023: // SPHERE
      {
	aDlg = new PrimitiveGUI_SphereDlg(getGeometryGUI(), parent, "");
	break;
      }
    case 4024: // TORUS
      {
	aDlg = new PrimitiveGUI_TorusDlg(getGeometryGUI(), parent, "");
	break;
      }
    case 4025: // CONE
      {
	aDlg = new PrimitiveGUI_ConeDlg(getGeometryGUI(), parent, "");
	break;
      }
    default:
      {
	SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
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
#ifdef WNT
	__declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return PrimitiveGUI::GetPrimitiveGUI( parent );
  }
}
