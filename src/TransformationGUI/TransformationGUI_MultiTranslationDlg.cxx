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
//  File   : TransformationGUI_MultiTranslationDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "TransformationGUI_MultiTranslationDlg.h"

#include <gp_Lin.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Compound.hxx>
#include "QAD_Config.h"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_MultiTranslationDlg()
// purpose  : Constructs a TransformationGUI_MultiTranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiTranslationDlg::TransformationGUI_MultiTranslationDlg(QWidget* parent,  const char* name, TransformationGUI* theTransformationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_MULTITRANSLATION_SIMPLE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_MULTITRANSLATION_DOUBLE")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_MULTITRANSLATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_MULTITRANSLATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2Spin1Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_MULTITRANSLATION_SIMPLE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR_U"));
  GroupPoints->TextLabel3->setText(tr("GEOM_STEP_U"));
  GroupPoints->TextLabel4->setText(tr("GEOM_NB_TIMES_U"));
  GroupPoints->CheckButton1->setText(tr("GEOM_REVERSE_U"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_3Sel4Spin2Check(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_MULTITRANSLATION_DOUBLE"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_VECTOR_U"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_VECTOR_V"));
  GroupDimensions->TextLabel4->setText(tr("GEOM_STEP_U"));
  GroupDimensions->TextLabel5->setText(tr("GEOM_NB_TIMES_U"));
  GroupDimensions->TextLabel6->setText(tr("GEOM_STEP_V"));
  GroupDimensions->TextLabel7->setText(tr("GEOM_NB_TIMES_V"));
  GroupDimensions->CheckButton1->setText(tr("GEOM_REVERSE_U"));
  GroupDimensions->CheckButton2->setText(tr("GEOM_REVERSE_V"));
  GroupDimensions->PushButton1->setPixmap(image2);
  GroupDimensions->PushButton2->setPixmap(image2);
  GroupDimensions->PushButton3->setPixmap(image2);

  Layout1->addWidget(GroupPoints, 1, 0);
  Layout1->addWidget(GroupDimensions, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myTransformationGUI = theTransformationGUI;
  Init();
}


//=================================================================================
// function : ~TransformationGUI_MultiTranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiTranslationDlg::~TransformationGUI_MultiTranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::Init()
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myStep1 = 50.0;
  myStep2 = 50.0;
  myNbTimes1 = 2;
  myNbTimes2 = 2;
  myOkBase = myOkDir1 = myOkDir2 = false;

  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  double SpecificStep = 1;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(1.0, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(myStep1);
  GroupPoints->SpinBox_DY->SetValue(myNbTimes1);

  GroupDimensions->SpinBox_DX1->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DY1->RangeStepAndValidator(1.0, 999.999, SpecificStep, 3);
  GroupDimensions->SpinBox_DX2->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DY2->RangeStepAndValidator(1.0, 999.999, SpecificStep, 3);
  GroupDimensions->SpinBox_DX1->SetValue(myStep1);
  GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
  GroupDimensions->SpinBox_DX2->SetValue(myStep2);
  GroupDimensions->SpinBox_DY2->SetValue(myNbTimes2);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX2, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY2, SLOT(SetStep(double)));

  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle1(int)));
  connect(GroupDimensions->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle1(int)));
  connect(GroupDimensions->CheckButton2, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle2(int)));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupDimensions->hide();
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  mySelection->ClearFilters();
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  disconnect(mySelection, 0, this, 0);
  myStep1 = 50.0;
  myStep2 = 50.0;
  myNbTimes1 = 2;
  myNbTimes2 = 2;
  myOkBase = myOkDir1 = myOkDir2  = false;

  switch (constructorId)
    {
    case 0: /* Translate simple */
      {
	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();

	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText("");
 	GroupPoints->LineEdit2->setText("");

	GroupPoints->SpinBox_DX->SetValue(myStep1);
	GroupPoints->SpinBox_DY->SetValue(myNbTimes1);

	/* filter for next selection */
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    case 1: /* Translate double */
      {
	GroupPoints->hide();
	resize(0, 0);
	GroupDimensions->show();

	myEditCurrentArgument = GroupDimensions->LineEdit1;
	GroupDimensions->LineEdit1->setText("");
 	GroupDimensions->LineEdit2->setText("");
	GroupDimensions->LineEdit3->setText("");

	GroupDimensions->SpinBox_DX1->SetValue(myStep1);
	GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
	GroupDimensions->SpinBox_DX2->SetValue(myStep2);
	GroupDimensions->SpinBox_DY2->SetValue(myNbTimes2);

	/* filter for next selection */
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    }
  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myConstructorId == 0) {
    if(myOkBase && myOkDir1)
      myTransformationGUI->MakeMultiTranslation1DAndDisplay(myGeomShape, myDir1, myStep1, myNbTimes1);
  }
  else if(myConstructorId == 1) {
    if(myOkBase && myOkDir1 && myOkDir2)
      myTransformationGUI->MakeMultiTranslation2DAndDisplay(myGeomShape, myDir1, myStep1, myNbTimes1, myDir2, myStep2, myNbTimes2);
  }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_MultiTranslationDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1 || myEditCurrentArgument == GroupDimensions->LineEdit1)
      myOkBase = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2 || myEditCurrentArgument == GroupDimensions->LineEdit2)
      myOkDir1 = false;
    else if(myEditCurrentArgument == GroupDimensions->LineEdit3)
      myOkDir2 = false;
    return;
  }

  // nbSel == 1
  TopoDS_Shape S; 
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  switch(myConstructorId)
    {
    case 0 :
      {
	if(myEditCurrentArgument == GroupPoints->LineEdit1) {
	  myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
	  if(!testResult)
	    return;
	  myEditCurrentArgument->setText(aString);
	  myBase = S;
	  myOkBase = true;
	}    
	else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir1 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString);
	  myOkDir1 = true;
	}

	if(myOkBase && myOkDir1)
	  this->MakeMultiTranslationSimulationAndDisplay();
	break;
      }
    case 1 :
      {	
	if(myEditCurrentArgument == GroupDimensions->LineEdit1) {
	  myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
	  if(!testResult)
	    return;
	  myEditCurrentArgument->setText(aString);
	  myBase = S;
	  myOkBase = true;
	}    
	else if(myEditCurrentArgument == GroupDimensions->LineEdit2) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir1 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString);
	  myOkDir1 = true;
	}
	else if(myEditCurrentArgument == GroupDimensions->LineEdit3) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir2 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString);
	  myOkDir2 = true;
	}

	if(myOkBase && myOkDir1 && myOkDir2)
	  this->MakeMultiTranslationSimulationAndDisplay();
	break;
      }
    }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::SetEditCurrentArgument()
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
  else if(send == GroupDimensions->PushButton1) {
    GroupDimensions->LineEdit1->setFocus();
    myEditCurrentArgument = GroupDimensions->LineEdit1;
  }
  else if(send == GroupDimensions->PushButton2) {
    GroupDimensions->LineEdit2->setFocus();
    myEditCurrentArgument = GroupDimensions->LineEdit2;
    mySelection->AddFilter(myEdgeFilter);
  }
  else if(send == GroupDimensions->PushButton3) {
    GroupDimensions->LineEdit3->setFocus();
    myEditCurrentArgument = GroupDimensions->LineEdit3;
    mySelection->AddFilter(myEdgeFilter);
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if (send == GroupPoints->LineEdit2)
    myEditCurrentArgument = GroupPoints->LineEdit2;
  else if (send == GroupDimensions->LineEdit1)
    myEditCurrentArgument = GroupDimensions->LineEdit1;
  else if (send == GroupDimensions->LineEdit2)
    myEditCurrentArgument = GroupDimensions->LineEdit2;
  else if (send == GroupDimensions->LineEdit3)
    myEditCurrentArgument = GroupDimensions->LineEdit3;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  if(myConstructorId == 0) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(myConstructorId == 1) {
    GroupDimensions->LineEdit1->setFocus();
    myEditCurrentArgument = GroupDimensions->LineEdit1;
  }

  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::enterEvent(QEvent* e)
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
void TransformationGUI_MultiTranslationDlg::ValueChangedInSpinBox(double newValue)
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  QObject* send = (QObject*)sender();

  switch(myConstructorId)
    {
    case 0 :
      {	
	if(send == GroupPoints->SpinBox_DX)
	  myStep1 = newValue;
	else if(send == GroupPoints->SpinBox_DY)
	  myNbTimes1 = (int)newValue;
	if(myOkBase && myOkDir1)
	  this->MakeMultiTranslationSimulationAndDisplay();
	break;
      }
    case 1 :
      {	
	if(send == GroupDimensions->SpinBox_DX1)
	  myStep1 = newValue;
	else if(send == GroupDimensions->SpinBox_DY1)
	  myNbTimes1 = (int)newValue;
	else if(send == GroupDimensions->SpinBox_DX2)
	  myStep2 = newValue;
	else if(send == GroupDimensions->SpinBox_DY2)
	  myNbTimes2 = (int)newValue;
	if(myOkBase && myOkDir1 && myOkDir2)
	  this->MakeMultiTranslationSimulationAndDisplay();
	break;
      }
    }
  return;
}


//=================================================================================
// function : ReverseAngle1()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseAngle1(int state)
{
  myStep1 = -myStep1;
  if(myConstructorId == 0) {
    GroupPoints->SpinBox_DX->SetValue(myStep1);
    if(myOkBase && myOkDir1)
      this->MakeMultiTranslationSimulationAndDisplay();
  }
  else if(myConstructorId == 1) {
    GroupDimensions->SpinBox_DX1->SetValue(myStep1);
    if(myOkBase && myOkDir1 && myOkDir2)
      this->MakeMultiTranslationSimulationAndDisplay();
  }
  return;
}


//=================================================================================
// function : ReverseAngle2()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseAngle2(int state)
{
  myStep2 = -myStep2;
  GroupDimensions->SpinBox_DX2->SetValue(myStep2);
  if(myOkBase && myOkDir1 && myOkDir2)
    MakeMultiTranslationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeMultiTranslationSimulationAndDisplay()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::MakeMultiTranslationSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  int i, j;
  Standard_Real DX, DY, DZ;
  gp_Trsf theTransformation;
  GProp_GProps System;
  TopoDS_Compound compound;

  BRep_Builder B;
  B.MakeCompound(compound);
    
  try {
    BRepGProp::LinearProperties(myBase, System);
    gp_Pnt myPoint = System.CentreOfMass();
    TopoDS_Shape S = BRepBuilderAPI_MakeVertex(myPoint).Shape();

    switch (myConstructorId)
      {
      case 0 :
	{ 
	  gp_Vec Vec(myDir1);
	  Vec.Normalize();

	  for(i = 0; i < myNbTimes1; i++) {
	    DX = i * myStep1 * Vec.X();
	    DY = i * myStep1 * Vec.Y();
	    DZ = i * myStep1 * Vec.Z();
	    myVec.SetCoord(DX, DY, DZ);
	    
	    theTransformation.SetTranslation(myVec);
	    BRepBuilderAPI_Transform myBRepTransformation(S, theTransformation, Standard_False);
	    B.Add(compound, myBRepTransformation.Shape());
	  }
	  mySimulationTopoDs = compound;
	  myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
	  break;
	}
      case 1 :
	{	
	  gp_Vec Vec1(myDir1);
	  Vec1.Normalize();
	  gp_Vec Vec2(myDir2);
	  Vec2.Normalize();

	  for(i = 0; i < myNbTimes1; i++) {
	    for(j = 0; j < myNbTimes2; j++) {
	      DX = i * myStep1 * Vec1.X() + j * myStep2 * Vec2.X();
	      DY = i * myStep1 * Vec1.Y() + j * myStep2 * Vec2.Y();
	      DZ = i * myStep1 * Vec1.Z() + j * myStep2 * Vec2.Z();
	      myVec.SetCoord(DX, DY, DZ);
	      
	      theTransformation.SetTranslation(myVec);
	      BRepBuilderAPI_Transform myBRepTransformation(S, theTransformation, Standard_False);
	      B.Add(compound, myBRepTransformation.Shape());
	    }
	  }
	  mySimulationTopoDs = compound;
	  myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
	  break;
	}
      }
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeMultitranslationSimulationAndDisplay");
    return;
  }
  return;
}
