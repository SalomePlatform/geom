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
//  File   : BooleanGUI_FuseDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BooleanGUI_FuseDlg.h"

//=================================================================================
// class    : BooleanGUI_FuseDlg()
// purpose  : Constructs a BooleanGUI_FuseDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BooleanGUI_FuseDlg::BooleanGUI_FuseDlg(QWidget* parent, const char* name, BooleanGUI* theBooleanGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_FUSE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_FUSE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_FUSE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupFuse = new DlgRef_2Sel_QTD(this, "GroupFuse");
  GroupFuse->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupFuse->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  GroupFuse->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  GroupFuse->PushButton1->setPixmap(image1);
  GroupFuse->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupFuse, 1, 0);
  /***************************************************************/

  /* Initialisation */
  myBooleanGUI = theBooleanGUI;
  Init();
}


//=================================================================================
// function : ~BooleanGUI_FuseDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BooleanGUI_FuseDlg::~BooleanGUI_FuseDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupFuse->LineEdit1;

  myShape1.Nullify();
  myShape2.Nullify();
  myOkShape1 = myOkShape2 = false;

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupFuse->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupFuse->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupFuse->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupFuse->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupFuse->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));

  if(myOkShape1 && myOkShape2)
    myBooleanGUI->MakeBooleanAndDisplay(myGeomShape1 ,myGeomShape2, 3);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BooleanGUI_FuseDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupFuse->LineEdit1)
      myOkShape1 = false;
    else if( myEditCurrentArgument == GroupFuse->LineEdit2)
      myOkShape2 = false;
    return;
  }

  /*  nbSel == 1  */
  TopoDS_Shape S;
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();

  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == GroupFuse->LineEdit1) {
    myGeomShape1 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape1 = S;
    GroupFuse->LineEdit1->setText(aString);
    myOkShape1 = true;
  }
  else if(myEditCurrentArgument == GroupFuse->LineEdit2) {
    myGeomShape2 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape2 = S;
    GroupFuse->LineEdit2->setText(aString);
    myOkShape2 = true;
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupFuse->PushButton1) {
    GroupFuse->LineEdit1->setFocus();
    myEditCurrentArgument = GroupFuse->LineEdit1;
  }
  else if(send == GroupFuse->PushButton2) {
    GroupFuse->LineEdit2->setFocus();
    myEditCurrentArgument = GroupFuse->LineEdit2;
  }
  SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupFuse->LineEdit1)
    myEditCurrentArgument = GroupFuse->LineEdit1;
  else if(send == GroupFuse->LineEdit2)
    myEditCurrentArgument = GroupFuse->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BooleanGUI_FuseDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void BooleanGUI_FuseDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}
