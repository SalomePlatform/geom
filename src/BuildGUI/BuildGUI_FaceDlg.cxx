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
//  File   : BuildGUI_FaceDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BuildGUI_FaceDlg.h"

//=================================================================================
// class    : BuildGUI_FaceDlg()
// purpose  : Constructs a BuildGUI_FaceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_FaceDlg::BuildGUI_FaceDlg(QWidget* parent, const char* name, BuildGUI* theBuildGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_FACE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_FACE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_FACE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel1Check_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_FACE_FFW"));
  GroupPoints->TextLabel1->setText(tr("GEOM_WIRE"));
  GroupPoints->CheckButton1->setText(tr("GEOM_FACE_OPT"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBuildGUI = theBuildGUI;
  Init();
}


//=================================================================================
// function : ~BuildGUI_FaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_FaceDlg::~BuildGUI_FaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->CheckButton1->setChecked(TRUE);

  myOkShape = false;

  myWireFilter = new GEOM_ShapeTypeFilter(TopAbs_WIRE, myGeom);
  mySelection->AddFilter(myWireFilter); /* first filter used */

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if(myOkShape)	  
    myBuildGUI->MakeFaceAndDisplay(myGeomShape, GroupPoints->CheckButton1->isChecked());
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_FaceDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  myOkShape = false;
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    return;
  }
  
  // nbSel == 1!
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, myShape))
    return;

  myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
  if(!testResult)
    return;

  myEditCurrentArgument->setText(aString);
  myOkShape = true;
  /* no simulation */
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::SetEditCurrentArgument()
{
  GroupPoints->LineEdit1->setFocus();
  mySelection->ClearFilters();
  mySelection->AddFilter(myWireFilter);
  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myWireFilter);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}
