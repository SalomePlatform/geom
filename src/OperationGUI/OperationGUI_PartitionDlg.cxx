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
//  File   : OperationGUI_PartitionDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "OperationGUI_PartitionDlg.h"

//=================================================================================
// class    : OperationGUI_PartitionDlg()
// purpose  : Constructs a OperationGUI_PartitionDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_PartitionDlg::OperationGUI_PartitionDlg(QWidget* parent, const char* name, OperationGUI* theOperationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_PARTITION")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PARTITION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PARTITION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_4Sel1List_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_TOOL_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RECONSTRUCTION_LIMIT"));
  GroupPoints->TextLabel4->setText(tr("GEOM_SUPPRESS_RESULT_INSIDE"));
  GroupPoints->TextLabel5->setText(tr("GEOM_SUPPRESS_RESULT_OUTSIDE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->PushButton3->setPixmap(image1);
  GroupPoints->PushButton4->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myOperationGUI = theOperationGUI;
  Init();
}


//=================================================================================
// function : ~OperationGUI_PartitionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_PartitionDlg::~OperationGUI_PartitionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myOkListShapes = myOkListTools = myOkKeepShape = myOkRemoveShape = false;

  /* type for sub shape selection */
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SOLID"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SHELL"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_FACE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_WIRE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_EDGE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_VERTEX"));

  /* Current item is 'Shape' */
  GroupPoints->ComboBox1->setCurrentItem(0);
  myLimit = GroupPoints->ComboBox1->currentItem();

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit4, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));

  if(myOkListShapes || myOkListTools || myOkKeepShape || myOkRemoveShape) {
    GEOM::shape_type limit;
    switch(myLimit)
      {
      case 0 : 
	{
	  limit = GEOM::SHAPE;
	  break;
	}
      case 1 :
	{
	  limit = GEOM::SHELL;
	  break;
	}
      case 2 :
	{
	  limit = GEOM::FACE;
	  break;
	}
      case 3 :
	{
	  limit = GEOM::WIRE;
	  break;
 	}
      case 4 :
	{
	  limit = GEOM::EDGE;
	  break;
	}
      case 5 :
	{
	  limit = GEOM::VERTEX;
	  break;
	}
      default :
	limit = GEOM::SHAPE;
      }

    myOperationGUI->MakePartitionAndDisplay(myListShapes, myListTools, myListKeepInside, myListRemoveInside, limit);
  }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_PartitionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel < 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkListShapes = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkListTools = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit3)
      myOkKeepShape = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit4)
      myOkRemoveShape = false;
    return;
  }

  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListShapes);
    myEditCurrentArgument->setText(aString);
    myOkListShapes = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListTools);
    myEditCurrentArgument->setText(aString);
    myOkListTools = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit3) {
    myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListRemoveInside);
    myEditCurrentArgument->setText(aString);
    myOkKeepShape = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit4) {
    myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListKeepInside);
    myEditCurrentArgument->setText(aString);
    myOkRemoveShape = true;
  }

  /* no simulation */
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
  }
  else if(send == GroupPoints->PushButton3) {
    GroupPoints->LineEdit3->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit3;
  }
  else if(send == GroupPoints->PushButton4) {
    GroupPoints->LineEdit4->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit4;
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if (send == GroupPoints->LineEdit2)
    myEditCurrentArgument = GroupPoints->LineEdit2;
  else if (send == GroupPoints->LineEdit3)
    myEditCurrentArgument = GroupPoints->LineEdit3;
  else if (send == GroupPoints->LineEdit4)
    myEditCurrentArgument = GroupPoints->LineEdit4;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=======================================================================
//function : ComboTextChanged
//purpose  : 
//=======================================================================

void OperationGUI_PartitionDlg::ComboTextChanged()
{
 myLimit = GroupPoints->ComboBox1->currentItem();

 bool IsEnabled = GroupPoints->ComboBox1->currentItem() < 3;
 GroupPoints->LineEdit3->setEnabled(IsEnabled);
 GroupPoints->LineEdit4->setEnabled(IsEnabled);
 GroupPoints->TextLabel4->setEnabled(IsEnabled);
 GroupPoints->TextLabel5->setEnabled(IsEnabled);
 GroupPoints->PushButton3->setEnabled(IsEnabled);
 GroupPoints->PushButton4->setEnabled(IsEnabled);
 return;
}
