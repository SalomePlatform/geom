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
//  File   : MeasureGUI_InertiaDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

using namespace std;
#include "MeasureGUI_InertiaDlg.h"

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

//=================================================================================
// class    : MeasureGUI_InertiaDlg()
// purpose  : Constructs a MeasureGUI_InertiaDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_InertiaDlg::MeasureGUI_InertiaDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_INERTIA")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));
    
  setCaption(tr("GEOM_INERTIA_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_INERTIA_CONSTR"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel12LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_MATRIX"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextLabel2->setText(tr("GEOM_INERTIA_I").arg("1"));
  GroupC1->TextLabel3->setText(tr("GEOM_INERTIA_I").arg("2"));
  GroupC1->TextLabel4->setText(tr("GEOM_INERTIA_I").arg("3"));
  GroupC1->TextLabel5->setText(tr("GEOM_INERTIA_IXYZ"));
  GroupC1->LineEdit11->setReadOnly(TRUE);
  GroupC1->LineEdit12->setReadOnly(TRUE);
  GroupC1->LineEdit13->setReadOnly(TRUE);
  GroupC1->LineEdit21->setReadOnly(TRUE);
  GroupC1->LineEdit22->setReadOnly(TRUE);
  GroupC1->LineEdit23->setReadOnly(TRUE);
  GroupC1->LineEdit31->setReadOnly(TRUE);
  GroupC1->LineEdit32->setReadOnly(TRUE);
  GroupC1->LineEdit33->setReadOnly(TRUE);
  GroupC1->LineEdit41->setReadOnly(TRUE);
  GroupC1->LineEdit42->setReadOnly(TRUE);
  GroupC1->LineEdit43->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_InertiaDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_InertiaDlg::~MeasureGUI_InertiaDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::Init()
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
void MeasureGUI_InertiaDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit13->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit23->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");
  GroupC1->LineEdit33->setText("");
  GroupC1->LineEdit41->setText("");
  GroupC1->LineEdit42->setText("");
  GroupC1->LineEdit43->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S) || S.IsNull())
    return;

  GroupC1->LineEdit1->setText(aString);

  this->CalculateAndDisplayInertia(S);
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::SetEditCurrentArgument()
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
void MeasureGUI_InertiaDlg::LineEditReturnPressed()
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
void MeasureGUI_InertiaDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateAndDisplayInertia()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::CalculateAndDisplayInertia(const TopoDS_Shape& S)
{
  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit13->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit23->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");
  GroupC1->LineEdit33->setText("");
  GroupC1->LineEdit41->setText("");
  GroupC1->LineEdit42->setText("");
  GroupC1->LineEdit43->setText("");

  if(S.IsNull()) 
    return;

  QString resString;
  GProp_GProps System;

  try {
    if(S.ShapeType() == TopAbs_VERTEX || S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE)
      BRepGProp::LinearProperties(S, System);
    else if(S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL)
      BRepGProp::SurfaceProperties(S, System);
    else
      BRepGProp::VolumeProperties(S, System);

    gp_Mat I = System.MatrixOfInertia() ;
    GProp_PrincipalProps Pr = System.PrincipalProperties();
    Standard_Real Ix,Iy,Iz;
    Pr.Moments(Ix,Iy,Iz);
    
    /* matrix 3x3 */
    resString = tr("%1").arg(I(1,1), 12, 'f', 6);
    GroupC1->LineEdit11->setText(resString);
    resString = tr("%1").arg(I(1,2), 12, 'f', 6);
    GroupC1->LineEdit12->setText(resString);
    resString = tr("%1").arg(I(1,3), 12, 'f', 6);
    GroupC1->LineEdit13->setText(resString);

    resString = tr("%1").arg(I(2,1), 12, 'f', 6);
    GroupC1->LineEdit21->setText(resString);
    resString = tr("%1").arg(I(2,2), 12, 'f', 6);
    GroupC1->LineEdit22->setText(resString);
    resString = tr("%1").arg(I(2,3), 12, 'f', 6);
    GroupC1->LineEdit23->setText(resString);

    resString = tr("%1").arg(I(3,1), 12, 'f', 6);
    GroupC1->LineEdit31->setText(resString);
    resString = tr("%1").arg(I(3,2), 12, 'f', 6);
    GroupC1->LineEdit32->setText(resString);
    resString = tr("%1").arg(I(3,3), 12, 'f', 6);
    GroupC1->LineEdit33->setText(resString);

    /* moments */
    resString = tr("%1").arg(Ix, 12, 'f', 6);
    GroupC1->LineEdit41->setText(resString);
    resString = tr("%1").arg(Ix, 12, 'f', 6);
    GroupC1->LineEdit42->setText(resString);
    resString = tr("%1").arg(Iz, 12, 'f', 6);
    GroupC1->LineEdit43->setText(resString);
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayInertia()");
  }
  return;
}
