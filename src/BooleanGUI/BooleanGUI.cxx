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
//  File   : BooleanGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$


#include "BooleanGUI.h"

#include "SUIT_Desktop.h"
#include "BooleanGUI_Dialog.h"

#include "GeometryGUI.h"

using namespace std;

//=======================================================================
// function : BooleanGUI()
// purpose  : Constructor
//=======================================================================
BooleanGUI::BooleanGUI( GeometryGUI* parent ) : GEOMGUI( parent )
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
bool BooleanGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

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

  QDialog* aDlg = new BooleanGUI_Dialog( anOperation, getGeometryGUI(), parent, "");
  aDlg->show();

  return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
GEOM_BOOLEANGUI_EXPORT
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new BooleanGUI( parent );
  }
}
