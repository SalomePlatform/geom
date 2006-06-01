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
//  File   : GroupGUI.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include "GroupGUI.h"
#include "GroupGUI_GroupDlg.h" 

#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "LightApp_SelectionMgr.h"

GroupGUI* GroupGUI::myGUIObject = 0;

//=======================================================================
// function : GetGroupGUI()
// purpose  : Get the only GroupGUI object [ static ]
//=======================================================================
GroupGUI* GroupGUI::GetGroupGUI(GeometryGUI* parent)
{
  if ( myGUIObject == 0 ) 
    myGUIObject = new GroupGUI(parent);

  return myGUIObject;
}

//=======================================================================
// function : GroupGUI()
// purpose  : Constructor
//=======================================================================
GroupGUI::GroupGUI(GeometryGUI* parent)
: GEOMGUI(parent)
{
}

//=======================================================================
// function : ~GroupGUI()
// purpose  : Destructor
//=======================================================================
GroupGUI::~GroupGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  : 
//=======================================================================
bool GroupGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;

  SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(suitApp->activeStudy());
  if ( !appStudy ) return false;
  _PTR(Study) aStudy = appStudy->studyDS();

  if ( aStudy->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warn1 ( parent,
			     QObject::tr("WRN_WARNING"), 
			     QObject::tr("WRN_STUDY_LOCKED"),
			     QObject::tr("BUT_OK") );
    return false;
  }

  switch ( theCommandID ) {
  case 800: 
  case 8001: // CREATE GROUP
    aDlg = new GroupGUI_GroupDlg (GroupGUI_GroupDlg::CreateGroup, getGeometryGUI(), parent); 
    break;
  case 801:  // EDIT GROUP
    {
      SALOME_ListIO aList;
      aList.Clear();

      SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>(suitApp);
      if (app) {
	LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
	if (aSelMgr)
	  aSelMgr->selectedObjects(aList);
      }

      if (aList.Extent() == 1) {
	Standard_Boolean aResult = Standard_False;
	GEOM::GEOM_Object_var anObj =
	  GEOMBase::ConvertIOinGEOMObject( aList.First(), aResult );

	if (aResult && !CORBA::is_nil(anObj) && anObj->GetType() == GEOM_GROUP) {
	  aDlg = new GroupGUI_GroupDlg (GroupGUI_GroupDlg::EditGroup, getGeometryGUI(), parent); 
	  break;
	}
      }
      SUIT_MessageBox::warn1(parent, tr("WRN_WARNING"), tr("NO_GROUP"), tr("BUT_OK") );
      break;
    }
  default: 
    suitApp->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID)); 
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
  GEOMGUI* GetLibGUI(GeometryGUI* p)
  {
    return GroupGUI::GetGroupGUI(p);
  }
}
