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
//  File   : MeasureGUI_BndBoxDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "MeasureGUI_BndBoxDlg.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBndLib.hxx>

//=================================================================================
// class    : MeasureGUI_BndBoxDlg()
// purpose  : Constructs a MeasureGUI_BndBoxDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_BndBoxDlg::MeasureGUI_BndBoxDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_BOUNDING_BOX")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_BNDBOX_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_BNDBOX"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel6LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_BNDBOX_OBJDIM"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextLabel2->setText(tr("GEOM_MIN"));
  GroupC1->TextLabel3->setText(tr("GEOM_MAX"));
  GroupC1->TextLabel4->setText(tr("GEOM_X"));
  GroupC1->TextLabel5->setText(tr("GEOM_Y"));
  GroupC1->TextLabel6->setText(tr("GEOM_Z"));
  GroupC1->LineEdit11->setReadOnly(TRUE);
  GroupC1->LineEdit12->setReadOnly(TRUE);
  GroupC1->LineEdit21->setReadOnly(TRUE);
  GroupC1->LineEdit22->setReadOnly(TRUE);
  GroupC1->LineEdit31->setReadOnly(TRUE);
  GroupC1->LineEdit32->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_BndBoxDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_BndBoxDlg::~MeasureGUI_BndBoxDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::Init()
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
void MeasureGUI_BndBoxDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = "";

  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S) || S.IsNull())
    return;

  GroupC1->LineEdit1->setText(aString);

  this->CalculateAndDisplayBndBox(S);
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::SetEditCurrentArgument()
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
void MeasureGUI_BndBoxDlg::LineEditReturnPressed()
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
void MeasureGUI_BndBoxDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateAndDisplayBndBox()
// purpose  :
//=================================================================================
void MeasureGUI_BndBoxDlg::CalculateAndDisplayBndBox(const TopoDS_Shape& S)
{
  GroupC1->LineEdit11->setText("");
  GroupC1->LineEdit12->setText("");
  GroupC1->LineEdit21->setText("");
  GroupC1->LineEdit22->setText("");
  GroupC1->LineEdit31->setText("");
  GroupC1->LineEdit32->setText("");

  if(S.IsNull()) 
    return;

  Standard_Real axmin, aymin, azmin, axmax, aymax, azmax;
  Bnd_Box B;

  try {
    BRepBndLib::Add(S,B);
    B.Get(axmin, aymin, azmin, axmax, aymax, azmax);
    GroupC1->LineEdit11->setText(tr("%1").arg(axmin, 12, 'f', 6));
    GroupC1->LineEdit12->setText(tr("%1").arg(axmax, 12, 'f', 6));
    GroupC1->LineEdit21->setText(tr("%1").arg(aymin, 12, 'f', 6));
    GroupC1->LineEdit22->setText(tr("%1").arg(aymax, 12, 'f', 6));
    GroupC1->LineEdit31->setText(tr("%1").arg(azmin, 12, 'f', 6));
    GroupC1->LineEdit32->setText(tr("%1").arg(azmax, 12, 'f', 6));
    
    mySimulationTopoDs = BRepPrimAPI_MakeBox(gp_Pnt(axmin, aymin, azmin), gp_Pnt(axmax, aymax, azmax)).Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayBndBox()");
  }
  return;
}
