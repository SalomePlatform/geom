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
//  File   : TransformationGUI_RotationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "TransformationGUI_RotationDlg.h"

#include <gp_Lin.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAdaptor_Curve.hxx>

//=================================================================================
// class    : TransformationGUI_RotationDlg()
// purpose  : Constructs a TransformationGUI_RotationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_RotationDlg::TransformationGUI_RotationDlg(QWidget* parent,  const char* name, TransformationGUI* theTransformationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ROTATION")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ROTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ROTATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin1Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupPoints->CheckButton1->setText(tr("GEOM_REVERSE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myTransformationGUI = theTransformationGUI;
  Init();
}


//=================================================================================
// function : ~TransformationGUI_RotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_RotationDlg::~TransformationGUI_RotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myAngle = 0.0;
  myOkBase = myOkAxis = false;

  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);

  double SpecificStep = 5;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(myAngle);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle(int)));
  
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
void TransformationGUI_RotationDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkBase && myOkAxis)  
    myTransformationGUI->MakeRotationAndDisplay(myGeomShape, myLoc, myDir, myAngle*PI180); 
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_RotationDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkBase = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkAxis = false;
    return;
  }
  
  // nbSel == 1
  TopoDS_Shape S; 
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
    
  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myEditCurrentArgument->setText(aString);
    myOkBase = true;
    myBase = S;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    BRepAdaptor_Curve curv(TopoDS::Edge(S));
    myDir = curv.Line().Direction();
    myLoc = curv.Line().Location();
    myEditCurrentArgument->setText(aString);
    myOkAxis = true;
  }

  if(myOkBase && myOkAxis)
    this->MakeRotationSimulationAndDisplay();
  return; 
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
    mySelection->AddFilter(myEdgeFilter);
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::LineEditReturnPressed()
{  
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if (send == GroupPoints->LineEdit2)
    myEditCurrentArgument = GroupPoints->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ValueChangedInSpinBox(double newValue)
{
  myAngle = newValue;
  if(myOkBase && myOkAxis)
    MakeRotationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeRotationSimulationAndDisplay()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::MakeRotationSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  try {
    gp_Ax1 AX(myLoc, myDir);
    gp_Trsf theTransformation;
    theTransformation.SetRotation(AX, myAngle*PI180);
    BRepBuilderAPI_Transform myBRepTransformation(myBase, theTransformation, Standard_False);
    this->mySimulationTopoDs = myBRepTransformation.Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeRotationSimulationAndDisplay");
    return;
  }
  return;
}


//=================================================================================
// function : ReverseAngle()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_RotationDlg::ReverseAngle(int state)
{
  myAngle = -myAngle;
  GroupPoints->SpinBox_DX->SetValue(myAngle);
  if(myOkBase && myOkAxis)
    MakeRotationSimulationAndDisplay();
  return;
}
