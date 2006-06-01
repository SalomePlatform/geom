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
//  File   : BooleanGUI.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "BuildGUI.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"

#include "BuildGUI_EdgeDlg.h"       // Method EDGE
#include "BuildGUI_WireDlg.h"       // Method WIRE
#include "BuildGUI_FaceDlg.h"       // Method FACE
#include "BuildGUI_ShellDlg.h"      // Method SHELL
#include "BuildGUI_SolidDlg.h"      // Method SOLID
#include "BuildGUI_CompoundDlg.h"   // Method COMPOUND

#include "GeometryGUI.h"

BuildGUI* BuildGUI::myGUIObject = 0;

//=======================================================================
// function : GetBuildGUI()
// purpose  : Get the only BuildGUI object [ static ]
//=======================================================================
BuildGUI* BuildGUI::GetBuildGUI( GeometryGUI* parent )
{
  if ( myGUIObject == 0 ) 
    myGUIObject = new BuildGUI( parent );

  return myGUIObject;
}

//=======================================================================
// function : BuildGUI()
// purpose  : Constructor
//=======================================================================
BuildGUI::BuildGUI( GeometryGUI* parent )
: GEOMGUI( parent )
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
bool BuildGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();
  
  QDialog* aDlg = NULL;

  switch ( theCommandID )
  {
    case 4081: aDlg = new BuildGUI_EdgeDlg    ( getGeometryGUI(), parent, "" ); break;
    case 4082: aDlg = new BuildGUI_WireDlg    ( getGeometryGUI(), parent, "" ); break;
    case 4083: aDlg = new BuildGUI_FaceDlg    ( getGeometryGUI(), parent, "" ); break;
    case 4084: aDlg = new BuildGUI_ShellDlg   ( getGeometryGUI(), parent, "" ); break;
    case 4085: aDlg = new BuildGUI_SolidDlg   ( getGeometryGUI(), parent, "" ); break;
    case 4086: aDlg = new BuildGUI_CompoundDlg( getGeometryGUI(), parent, "" ); break;
    
    default: 
      SUIT_Session::session()->activeApplication()->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); 
      break;
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
    return BuildGUI::GetBuildGUI( parent );
  }
}
