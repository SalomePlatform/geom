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

#include "BuildGUI.h"

#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "QAD_Desktop.h"

#include "BuildGUI_EdgeDlg.h"       // Method EDGE
#include "BuildGUI_WireDlg.h"       // Method WIRE
#include "BuildGUI_FaceDlg.h"       // Method FACE
#include "BuildGUI_ShellDlg.h"      // Method SHELL
#include "BuildGUI_SolidDlg.h"      // Method SOLID
#include "BuildGUI_CompoundDlg.h"   // Method COMPOUND

BuildGUI* BuildGUI::myGUIObject = 0;

//=======================================================================
// function : GetBuildGUI()
// purpose  : Get the only BuildGUI object [ static ]
//=======================================================================
BuildGUI* BuildGUI::GetBuildGUI()
{
  if ( myGUIObject == 0 ) 
    myGUIObject = new BuildGUI();

  return myGUIObject;
}

//=======================================================================
// function : BuildGUI()
// purpose  : Constructor
//=======================================================================
BuildGUI::BuildGUI()
: GEOMGUI()
{
}


//=======================================================================
// function : ~BuildGUI()
// purpose  : Destructor
//=======================================================================
BuildGUI::~BuildGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool BuildGUI::OnGUIEvent( int theCommandID, QAD_Desktop* parent )
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  QDialog* aDlg = NULL;

  switch ( theCommandID )
  {
    case 4081: aDlg = new BuildGUI_EdgeDlg    ( parent, "", Sel ); break;
    case 4082: aDlg = new BuildGUI_WireDlg    ( parent, "", Sel ); break;
    case 4083: aDlg = new BuildGUI_FaceDlg    ( parent, "", Sel ); break;
    case 4084: aDlg = new BuildGUI_ShellDlg   ( parent, "", Sel ); break;
    case 4085: aDlg = new BuildGUI_SolidDlg   ( parent, "", Sel ); break;
    case 4086: aDlg = new BuildGUI_CompoundDlg( parent, "", Sel ); break;
    
    default: parent->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); break;
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
    return BuildGUI::GetBuildGUI();
  }
}
