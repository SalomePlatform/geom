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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
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

#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "QAD_Desktop.h"
#include "QAD_MessageBox.h"

GroupGUI* GroupGUI::myGUIObject = 0;

//=======================================================================
// function : GetGroupGUI()
// purpose  : Get the only GroupGUI object [ static ]
//=======================================================================
GroupGUI* GroupGUI::GetGroupGUI()
{
  if ( myGUIObject == 0 ) 
    myGUIObject = new GroupGUI();

  return myGUIObject;
}

//=======================================================================
// function : GroupGUI()
// purpose  : Constructor
//=======================================================================
GroupGUI::GroupGUI()
: GEOMGUI()
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
bool GroupGUI::OnGUIEvent( int theCommandID, QAD_Desktop* parent )
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  
  SALOME_Selection* Sel = SALOME_Selection::Selection(
    QAD_Application::getDesktop()->getActiveStudy()->getSelection() );

  QDialog* aDlg = NULL;

  if ( QAD_Application::getDesktop()->getActiveStudy()->isLocked() ) {
    QAD_MessageBox::warn1 ( QAD_Application::getDesktop(),
			   QObject::tr("WRN_WARNING"), 
			   QObject::tr("WRN_STUDY_LOCKED"),
			   QObject::tr("BUT_OK") );
    return false;
  }

  switch ( theCommandID ) {
  case 800: 
    aDlg = new GroupGUI_GroupDlg( GroupGUI_GroupDlg::CreateGroup, parent, "", Sel ); 
    break;
  case 801: 
    {
      if ( Sel->IObjectCount() == 1 ) {
	Standard_Boolean aResult = Standard_False;
	GEOM::GEOM_Object_var anObj =
	  GEOMBase::ConvertIOinGEOMObject( Sel->firstIObject(), aResult );

	if ( aResult && !CORBA::is_nil( anObj ) && anObj->GetType() == GEOM_GROUP ) {
	  aDlg = new GroupGUI_GroupDlg( GroupGUI_GroupDlg::EditGroup,   parent, "", Sel ); 
	  break;
	}
      }
      QAD_MessageBox::warn1 ( QAD_Application::getDesktop(),
			     tr("WRN_WARNING"), 
			     tr("NO_GROUP"),
			     tr("BUT_OK") );
      break;
    }
  default: 
    parent->putInfo( tr( "GEOM_PRP_COMMAND" ).arg( theCommandID ) ); 
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
  GEOMGUI* GetLibGUI()
  {
    return GroupGUI::GetGroupGUI();
  }
}
