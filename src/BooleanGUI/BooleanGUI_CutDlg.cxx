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
//  File   : BooleanGUI_CutDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BooleanGUI_CutDlg.h"

//=================================================================================
// class    : BooleanGUI_CutDlg()
// purpose  : Constructs a BooleanGUI_CutDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BooleanGUI_CutDlg::BooleanGUI_CutDlg(QWidget* parent, const char* name, BooleanGUI* theBooleanGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CUT")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CUT_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CUT"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupCut = new DlgRef_2Sel_QTD(this, "GroupCut");
  GroupCut->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupCut->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  GroupCut->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  GroupCut->PushButton1->setPixmap(image1);
  GroupCut->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupCut, 1, 0);
  /***************************************************************/

  /* Initialisation */
  myBooleanGUI = theBooleanGUI;
  Init();
}


//=================================================================================
// function : ~BooleanGUI_CutDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BooleanGUI_CutDlg::~BooleanGUI_CutDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::Init()
{  
  /* init variables */
  myEditCurrentArgument = GroupCut->LineEdit1;

  myShape1.Nullify();
  myShape2.Nullify();
  myOkShape1 = myOkShape2 = false;

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupCut->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupCut->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupCut->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupCut->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupCut->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return ;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));

  if(myOkShape1 && myOkShape2)
    myBooleanGUI->MakeBooleanAndDisplay(myGeomShape1 ,myGeomShape2, 2);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BooleanGUI_CutDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupCut->LineEdit1)
      myOkShape1 = false;
    else if( myEditCurrentArgument == GroupCut->LineEdit2)
      myOkShape2 = false;
    return;
  }

  /*  nbSel == 1  */
  TopoDS_Shape S;
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();

  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == GroupCut->LineEdit1) {
    myGeomShape1 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape1 = S;
    GroupCut->LineEdit1->setText(aString);
    myOkShape1 = true;
  }
  else if(myEditCurrentArgument == GroupCut->LineEdit2) {
    myGeomShape2 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape2 = S;
    GroupCut->LineEdit2->setText(aString);
    myOkShape2 = true;
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupCut->PushButton1) {
    GroupCut->LineEdit1->setFocus();
    myEditCurrentArgument = GroupCut->LineEdit1;
  }
  else if(send == GroupCut->PushButton2) {
    GroupCut->LineEdit2->setFocus();
    myEditCurrentArgument = GroupCut->LineEdit2;
  }
  SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupCut->LineEdit1)
    myEditCurrentArgument = GroupCut->LineEdit1;
  else if(send == GroupCut->LineEdit2)
    myEditCurrentArgument = GroupCut->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BooleanGUI_CutDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void BooleanGUI_CutDlg::enterEvent(QEvent * e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}
