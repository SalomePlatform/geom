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
//  File   : MeasureGUI_CenterMassDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_CenterMassDlg.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : MeasureGUI_CenterMassDlg()
// purpose  : Constructs a MeasureGUI_CenterMassDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_CenterMassDlg::MeasureGUI_CenterMassDlg(QWidget* parent, const char* name, MeasureGUI* theMeasureGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CENTERMASS")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CMASS_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CMASS"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupC1 = new MeasureGUI_1Sel3LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_CENTER"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextLabel2->setText(tr("GEOM_X"));
  GroupC1->TextLabel3->setText(tr("GEOM_Y"));
  GroupC1->TextLabel4->setText(tr("GEOM_Z"));
  GroupC1->LineEdit2->setReadOnly(TRUE);
  GroupC1->LineEdit3->setReadOnly(TRUE);
  GroupC1->LineEdit4->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  myMeasureGUI = theMeasureGUI;
  Init();
}


//=================================================================================
// function : ~MeasureGUI_CenterMassDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CenterMassDlg::~MeasureGUI_CenterMassDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupC1->LineEdit1;

  myOkCenterMass = false;

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupC1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupC1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupC1->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkCenterMass)
    myMeasureGUI->MakeCDGAndDisplay(myGeomShape);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_CenterMassDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  myEditCurrentArgument->setText("");
  QString aString = "";

  myOkCenterMass = false;
  GroupC1->LineEdit2->setText("");
  GroupC1->LineEdit3->setText("");
  GroupC1->LineEdit4->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, this->myShape))
    return;  

  Standard_Boolean testResult;
  myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
  if(!testResult)
    return;

  myEditCurrentArgument->setText(aString);

  if(this->CalculateAndDisplayCenterMass())
    myOkCenterMass = true;
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::SetEditCurrentArgument()
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
void MeasureGUI_CenterMassDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupC1->LineEdit1)
    myEditCurrentArgument = GroupC1->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_CenterMassDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateAndDisplayCenterMass()
// purpose  :
//=================================================================================
bool MeasureGUI_CenterMassDlg::CalculateAndDisplayCenterMass()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  try {
    QString resString;     
    GProp_GProps System;

    if(myShape.ShapeType() == TopAbs_VERTEX)
      myGeomBase->VertexToPoint(myShape, myCenterMass);
    else if(myShape.ShapeType() == TopAbs_EDGE || myShape.ShapeType() == TopAbs_WIRE) {
      BRepGProp::LinearProperties(myShape, System);
      myCenterMass = System.CentreOfMass();
    }
    else if(myShape.ShapeType() == TopAbs_FACE || myShape.ShapeType() == TopAbs_SHELL) {
      BRepGProp::SurfaceProperties(myShape, System);
      myCenterMass = System.CentreOfMass();
    }
    else {
      BRepGProp::VolumeProperties(myShape, System);
      myCenterMass = System.CentreOfMass();
    }
    
    BRepBuilderAPI_MakeVertex V(myCenterMass);
    mySimulationTopoDs = V.Shape();
    
    resString = tr("%1").arg(myCenterMass.X(), 12, 'f', 6);    
    GroupC1->LineEdit2->setText(resString);
    
    resString = tr("%1").arg(myCenterMass.Y(), 12, 'f', 6);    
    GroupC1->LineEdit3->setText(resString);
    
    resString = tr("%1").arg(myCenterMass.Z(), 12, 'f', 6);    
    GroupC1->LineEdit4->setText(resString);
    
    
    if(!mySimulationTopoDs.IsNull()) {
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
      return true;
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayCenterMass()");
  }
  return false;
}


