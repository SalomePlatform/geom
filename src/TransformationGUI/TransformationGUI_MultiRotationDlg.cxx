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

using namespace std;
#include "TransformationGUI_MultiRotationDlg.h"

#include <gp_Lin.hxx>
#include <Precision.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Compound.hxx>
#include <Geom_Line.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include "QAD_Config.h"

//=================================================================================
// class    : TransformationGUI_MultiRotationDlg()
// purpose  : Constructs a TransformationGUI_MultiRotationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiRotationDlg::TransformationGUI_MultiRotationDlg(QWidget* parent,  const char* name, TransformationGUI* theTransformationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_MULTIROTATION_SIMPLE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_MULTIROTATION_DOUBLE")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_MULTIROTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_MULTIROTATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_SIMPLE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_NB_TIMES"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_2Sel4Spin1Check(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_DOUBLE"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupDimensions->TextLabel4->setText(tr("GEOM_NB_TIMES"));
  GroupDimensions->TextLabel5->setText(tr("GEOM_STEP"));
  GroupDimensions->TextLabel6->setText(tr("GEOM_NB_TIMES"));
  GroupDimensions->CheckButton1->setText(tr("GEOM_REVERSE"));
  GroupDimensions->PushButton1->setPixmap(image2);
  GroupDimensions->PushButton2->setPixmap(image2);

  Layout1->addWidget(GroupPoints, 1, 0);
  Layout1->addWidget(GroupDimensions, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myTransformationGUI = theTransformationGUI;
  Init();
}


//=================================================================================
// function : ~TransformationGUI_MultiRotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiRotationDlg::~TransformationGUI_MultiRotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::Init()
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myAng = 45.0;
  myStep = 50.0;
  myNbTimes1 = 2;
  myNbTimes2 = 2;
  myOkBase = myOkDir = false;

  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  double SpecificStep1 = 5;
  double SpecificStep2 = 1;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupPoints->SpinBox_DX->SetValue(myNbTimes1);

  GroupDimensions->SpinBox_DX1->RangeStepAndValidator(-999.999, 999.999, SpecificStep1, 3);
  GroupDimensions->SpinBox_DY1->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupDimensions->SpinBox_DX2->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DY2->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupDimensions->SpinBox_DX1->SetValue(myAng);
  GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
  GroupDimensions->SpinBox_DX2->SetValue(myStep);
  GroupDimensions->SpinBox_DY2->SetValue(myNbTimes2);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX2, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY2, SLOT(SetStep(double)));

  connect(GroupDimensions->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle(int)));
  
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
void TransformationGUI_MultiRotationDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  mySelection->ClearFilters();
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  disconnect(mySelection, 0, this, 0);
  myAng = 45.0;
  myStep = 50.0;
  myNbTimes1 = 2;
  myNbTimes2 = 2;
  myOkBase = myOkDir = false;

  switch (constructorId)
    {
    case 0: /* Rotate simple */
      {
	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();

	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText("");
 	GroupPoints->LineEdit2->setText("");

	GroupPoints->SpinBox_DX->SetValue(myNbTimes1);

	/* filter for next selection */
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    case 1: /* Rotate double */
      {
	GroupPoints->hide();
	resize(0, 0);
	GroupDimensions->show();

	myEditCurrentArgument = GroupDimensions->LineEdit1;
	GroupDimensions->LineEdit1->setText("");
 	GroupDimensions->LineEdit2->setText("");

	GroupDimensions->SpinBox_DX1->SetValue(myAng);
	GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
	GroupDimensions->SpinBox_DX2->SetValue(myStep);
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
void TransformationGUI_MultiRotationDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myConstructorId == 0) {
    if(myOkBase && myOkDir)
      myTransformationGUI->MakeMultiRotation1DAndDisplay(myGeomShape, myDir, myLoc, myNbTimes1);
  }
  else if(myConstructorId == 1) {
    if(myOkBase && myOkDir)
      myTransformationGUI->MakeMultiRotation2DAndDisplay(myGeomShape, myDir, myLoc, myAng, myNbTimes1, myStep, myNbTimes2);
  }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_MultiRotationDlg::SelectionIntoArgument()
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
      myOkDir = false;
    return;
  }

  // nbSel == 1
  TopoDS_Shape S; 
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1 || myEditCurrentArgument == GroupDimensions->LineEdit1) {
    myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myEditCurrentArgument->setText(aString);
    myBase = S;
    myOkBase = true;
  }    
  else if(myEditCurrentArgument == GroupPoints->LineEdit2 || myEditCurrentArgument == GroupDimensions->LineEdit2) {
    BRepAdaptor_Curve curv(TopoDS::Edge(S));
    myDir = curv.Line().Direction();
    myLoc = curv.Line().Location();
    myEditCurrentArgument->setText(aString);
    myOkDir = true;
  }

  if(myOkBase && myOkDir)
    this->MakeMultiRotationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetEditCurrentArgument()
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
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::LineEditReturnPressed()
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
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ActivateThisDialog()
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
void TransformationGUI_MultiRotationDlg::enterEvent(QEvent* e)
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
void TransformationGUI_MultiRotationDlg::ValueChangedInSpinBox(double newValue)
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_DX || send == GroupDimensions->SpinBox_DY1)
    myNbTimes1 = newValue;
  else if(send == GroupDimensions->SpinBox_DX1)
    myAng = newValue;
  else if(send == GroupDimensions->SpinBox_DX2)
    myStep = newValue;
  else if(send == GroupDimensions->SpinBox_DY2)
    myNbTimes2 = newValue;

  if(myOkBase && myOkDir)
    this->MakeMultiRotationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : ReverseAngle()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiRotationDlg::ReverseAngle(int state)
{
  myAng = -myAng;
  if(myConstructorId == 0)
    GroupPoints->SpinBox_DX->SetValue(myAng);
  else if(myConstructorId == 1)
    GroupDimensions->SpinBox_DX1->SetValue(myAng);

  if(myOkBase && myOkDir)
    this->MakeMultiRotationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeMultiRotationSimulationAndDisplay()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::MakeMultiRotationSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  int i, j;
  Standard_Real DX, DY, DZ;
  gp_Trsf theTransformation;
  gp_Trsf theTransformation1;
  gp_Trsf theTransformation2;
  gp_Pnt myPoint;
  GProp_GProps System;
  TopoDS_Compound compound;

  BRep_Builder B;
  B.MakeCompound(compound);  

  if(myBase.ShapeType() == TopAbs_VERTEX)
    myGeomBase->VertexToPoint(myBase, myPoint);
  else if(myBase.ShapeType() == TopAbs_EDGE || myBase.ShapeType() == TopAbs_WIRE) {
    BRepGProp::LinearProperties(myBase, System);
    myPoint = System.CentreOfMass();
  }
  else if(myBase.ShapeType() == TopAbs_FACE || myBase.ShapeType() == TopAbs_SHELL) {
    BRepGProp::SurfaceProperties(myBase, System);
    myPoint = System.CentreOfMass();
  }
  else {
    BRepGProp::VolumeProperties(myBase, System);
    myPoint = System.CentreOfMass();
  }

  TopoDS_Shape S = BRepBuilderAPI_MakeVertex(myPoint).Shape();

  try {
    switch (myConstructorId)
      {
      case 0 :
	{ 
	  gp_Ax1 AX1(myLoc, myDir);
	  Standard_Real angle = 360/myNbTimes1;
	  for(i = 0; i < myNbTimes1; i++) {
	    theTransformation.SetRotation(AX1, i*angle*PI180);
	    BRepBuilderAPI_Transform myBRepTransformation(S, theTransformation, Standard_False);
	    B.Add(compound, myBRepTransformation.Shape());
	  }
	  mySimulationTopoDs = compound;
	  myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
	  break;
	}
      case 1 :
	{	
	  gp_Ax1 AX2(myLoc, myDir);
	  Handle(Geom_Line) Line = new Geom_Line(AX2);
	  gp_Pnt P2 = GeomAPI_ProjectPointOnCurve(myPoint, Line);
	  if(myPoint.IsEqual(P2, Precision::Confusion()))
	    return;

	  gp_Vec Vec(P2, myPoint);
	  Vec.Normalize();

	  for(i = 0; i < myNbTimes2; i++) {
	    for(j = 0; j < myNbTimes1; j++) {
	      DX = i * myStep * Vec.X();
	      DY = i * myStep * Vec.Y();
	      DZ = i * myStep * Vec.Z();
	      myVec.SetCoord(DX, DY, DZ);

	      theTransformation1.SetTranslation(myVec);
	      theTransformation2.SetRotation(AX2, j*myAng*PI180);
	      BRepBuilderAPI_Transform myBRepTransformation1(S, theTransformation1, Standard_False);
	      BRepBuilderAPI_Transform myBRepTransformation2(myBRepTransformation1.Shape(), theTransformation2, Standard_False);
	      B.Add(compound, myBRepTransformation2.Shape());
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
