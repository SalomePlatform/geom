// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : GroupGUI.cxx
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#include "GroupGUI.h"
#include "GroupGUI_GroupDlg.h" 
#include "GroupGUI_BooleanDlg.h" 

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>

//=======================================================================
// function : GroupGUI()
// purpose  : Constructor
//=======================================================================
GroupGUI::GroupGUI (GeometryGUI* parent)
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
bool GroupGUI::OnGUIEvent (int theCommandID, SUIT_Desktop* parent)
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = 0;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (!appStudy) return false;
  _PTR(Study) aStudy = appStudy->studyDS();

  if (aStudy->GetProperties()->IsLocked()) {
    SUIT_MessageBox::warning(parent, tr("WRN_WARNING"), tr("WRN_STUDY_LOCKED"));
    return false;
  }

  switch (theCommandID) {
  case GEOMOp::OpGroupCreate: 
  case GEOMOp::OpGroupCreatePopup: // CREATE GROUP
    aDlg = new GroupGUI_GroupDlg (GroupGUI_GroupDlg::CreateGroup, getGeometryGUI(), parent); 
    break;
  case GEOMOp::OpGroupEdit: // EDIT GROUP
    {
      SALOME_ListIO aList;
      aList.Clear();

      LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
      if (aSelMgr)
        aSelMgr->selectedObjects(aList);

      if (aList.Extent() == 1) {
        GEOM::GEOM_Object_var anObj =
          GEOMBase::ConvertIOinGEOMObject(aList.First());

        if (!CORBA::is_nil(anObj) && anObj->GetType() == GEOM_GROUP) {
          aDlg = new GroupGUI_GroupDlg (GroupGUI_GroupDlg::EditGroup, getGeometryGUI(), parent); 
          break;
        }
      }
      SUIT_MessageBox::warning(parent, tr("WRN_WARNING"), tr("NO_GROUP"));
    }
    break;
  case GEOMOp::OpGroupUnion: // UNION GROUPS
    aDlg = new GroupGUI_BooleanDlg (GroupGUI_BooleanDlg::UNION, getGeometryGUI(), parent); 
    break;
  case GEOMOp::OpGroupIntersect: // INTERSECT GROUPS
    aDlg = new GroupGUI_BooleanDlg (GroupGUI_BooleanDlg::INTERSECT, getGeometryGUI(), parent); 
    break;
  case GEOMOp::OpGroupCut: // CUT GROUPS
    aDlg = new GroupGUI_BooleanDlg (GroupGUI_BooleanDlg::CUT, getGeometryGUI(), parent); 
    break;
  default: 
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID)); 
    break;
  }

  if (aDlg) {
    aDlg->updateGeometry();
    aDlg->resize(aDlg->minimumSizeHint());
    aDlg->show();
  }

  return true;
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* p )
  {
    return new GroupGUI( p );
  }
}
