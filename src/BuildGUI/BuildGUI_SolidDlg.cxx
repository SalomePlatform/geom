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
//  File   : BuildGUI_SolidDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "BuildGUI_SolidDlg.h"

#include "BuildGUI.h"
#include "QAD_Desktop.h"

//=================================================================================
// class    : BuildGUI_SolidDlg()
// purpose  : Constructs a BuildGUI_SolidDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_SolidDlg::BuildGUI_SolidDlg(QWidget* parent, const char* name, BuildGUI* theBuildGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_DLG_BUILD_SOLID")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_SOLID_TITLE"));
    
  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SOLID"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupSolid = new DlgRef_1Sel_QTD(this, "GroupSolid");
  GroupSolid->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupSolid->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupSolid->PushButton1->setPixmap(image1);
  
  Layout1->addWidget(GroupSolid, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBuildGUI = theBuildGUI;
  Init();
}


//=================================================================================
// function : ~BuildGUI_SolidDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_SolidDlg::~BuildGUI_SolidDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupSolid->LineEdit1;
  myOkListShapes = false;

  myShellFilter = new GEOM_ShapeTypeFilter(TopAbs_SHELL, myGeom);
  /* filter for next selection */
  mySelection->AddFilter(myShellFilter);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupSolid->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
 
  /* displays Dialog */
  GroupSolid->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr("")); 

  if(myOkListShapes)  
    myBuildGUI->MakeSolidAndDisplay(myListShapes);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_SolidDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  myOkListShapes = false;
  int nbSel = mySelection->IObjectCount();
  if (nbSel == 0) 
    return;

  aString = tr("%1_objects").arg(nbSel);
  
  myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListShapes);
  myEditCurrentArgument->setText(aString);
  myOkListShapes = true;

  return ;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters() ;
  GroupSolid->LineEdit1->setFocus();
  myEditCurrentArgument = GroupSolid->LineEdit1;
  mySelection->AddFilter(myShellFilter);
  SelectionIntoArgument();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myShellFilter);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;  
  this->ActivateThisDialog();
  return;
}
