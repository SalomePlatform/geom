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
//  File   : BooleanGUI_CommonDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BooleanGUI_CommonDlg.h"

//=================================================================================
// class    : BooleanGUI_CommonDlg()
// purpose  : Constructs a BooleanGUI_CommonDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BooleanGUI_CommonDlg::BooleanGUI_CommonDlg(QWidget* parent, const char* name, BooleanGUI* theBooleanGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_COMMON")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_COMMON_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_COMMON"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupCommon = new DlgRef_2Sel_QTD(this, "GroupCommon");
  GroupCommon->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupCommon->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  GroupCommon->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  GroupCommon->PushButton1->setPixmap(image1);
  GroupCommon->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupCommon, 1, 0);
  /***************************************************************/

  /* Initialisation */
  myBooleanGUI = theBooleanGUI;
  Init();
}


//=================================================================================
// function : ~BooleanGUI_CommonDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BooleanGUI_CommonDlg::~BooleanGUI_CommonDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupCommon->LineEdit1;

  myShape1.Nullify();
  myShape2.Nullify();
  myOkShape1 = myOkShape2 = false;

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupCommon->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupCommon->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupCommon->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupCommon->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupCommon->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return ;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));

  if(myOkShape1 && myOkShape2)
    myBooleanGUI->MakeBooleanAndDisplay(myGeomShape1 ,myGeomShape2, 1);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BooleanGUI_CommonDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupCommon->LineEdit1)
      myOkShape1 = false;
    else if( myEditCurrentArgument == GroupCommon->LineEdit2)
      myOkShape2 = false;
    return;
  }

  /*  nbSel == 1  */
  TopoDS_Shape S;
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();

  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == GroupCommon->LineEdit1) {
    myGeomShape1 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape1 = S;
    GroupCommon->LineEdit1->setText(aString);
    myOkShape1 = true;
  }
  else if(myEditCurrentArgument == GroupCommon->LineEdit2) {
    myGeomShape2 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape2 = S;
    GroupCommon->LineEdit2->setText(aString);
    myOkShape2 = true;
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupCommon->PushButton1) {
    GroupCommon->LineEdit1->setFocus();
    myEditCurrentArgument = GroupCommon->LineEdit1;
  }
  else if(send == GroupCommon->PushButton2) {
    GroupCommon->LineEdit2->setFocus();
    myEditCurrentArgument = GroupCommon->LineEdit2;
  }
  SelectionIntoArgument();

  return;
}



//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupCommon->LineEdit1)
    myEditCurrentArgument = GroupCommon->LineEdit1;
  else if(send == GroupCommon->LineEdit2)
    myEditCurrentArgument = GroupCommon->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BooleanGUI_CommonDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void BooleanGUI_CommonDlg::enterEvent(QEvent * e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}
