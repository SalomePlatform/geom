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
//  File   : MeasureGUI_CheckShapeDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "MeasureGUI_CheckShapeDlg.h"

#include <BRepCheck_Analyzer.hxx>

#include <qtextedit.h>

//=================================================================================
// class    : MeasureGUI_CheckShapeDlg()
// purpose  : Constructs a MeasureGUI_CheckShapeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckShapeDlg::MeasureGUI_CheckShapeDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHECKSHAPE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CHECK_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CHECK_SHAPE"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel1TextView_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_CHECK_INFOS"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextEdit1->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_CheckShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CheckShapeDlg::~MeasureGUI_CheckShapeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupC1->LineEdit1;

   /* signals and slots connections */
  connect(GroupC1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupC1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupC1->show();
  this->show();

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_CheckShapeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  SelectedName = ""; /* future the name of selection */
  GroupC1->TextEdit1->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, SelectedName);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(S.IsNull())
    return;
 
  myEditCurrentArgument->setText(SelectedName);
  this->Check(S);

  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupC1->PushButton1) {
    GroupC1->LineEdit1->setFocus();
    myEditCurrentArgument = GroupC1->LineEdit1;
  }

  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupC1->LineEdit1)
    myEditCurrentArgument = GroupC1->LineEdit1;
  else
    return;

  MeasureGUI_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : Check()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::Check(const TopoDS_Shape S)
{
  if(S.IsNull()) 
    return;
    
  try {
    BRepCheck_Analyzer ana(S,false);
    if(ana.IsValid()) 
      GroupC1->TextEdit1->setText("This Shape seems to be valid.");
    else 
      GroupC1->TextEdit1->setText("This Shape is not valid.");
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in Check()");
  }
  return;
}
