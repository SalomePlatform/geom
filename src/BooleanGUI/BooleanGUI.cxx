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
//  File   : BooleanGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "BooleanGUI.h"

#include "QAD_Desktop.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "BooleanGUI_Dialog.h"

BooleanGUI* BooleanGUI::myGUIObject = 0;

//=======================================================================
// function : GetBooleanGUI()
// purpose  : Get the only BooleanGUI object [ static ]
//=======================================================================
BooleanGUI* BooleanGUI::GetBooleanGUI()
{
  if ( myGUIObject == 0 ) {
    // init BooleanGUI only once
    myGUIObject = new BooleanGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : BooleanGUI()
// purpose  : Constructor
//=======================================================================
BooleanGUI::BooleanGUI() : GEOMGUI()
{
}


//=======================================================================
// function : ~BooleanGUI()
// purpose  : Destructor
//=======================================================================
BooleanGUI::~BooleanGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BooleanGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());

  int anOperation = 0;	
  if      ( theCommandID == 5011 )
    anOperation = FUSE;
  else if ( theCommandID == 5012 )
    anOperation = COMMON;
  else if ( theCommandID == 5013 )
    anOperation = CUT;
  else if ( theCommandID == 5014 )
    anOperation = SECTION;
  else
    return false;

  QDialog* aDlg = new BooleanGUI_Dialog( anOperation, parent, "", Sel);
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
    return BooleanGUI::GetBooleanGUI();
  }
}
