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
//  File   : BasicGUI_VectorDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BasicGUI_VectorDlg.h"

#include "QAD_Config.h"
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : BasicGUI_VectorDlg()
// purpose  : Constructs a BasicGUI_VectorDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_VectorDlg::BasicGUI_VectorDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_VECTOR_2P")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_VECTOR_DXYZ")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_VECTOR_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_VECTOR"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_3Spin1Check(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_COORDINATES"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_DX"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_DY"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_DZ"));
  GroupDimensions->CheckBox1->setText(tr("GEOM_REVERSE_VECTOR"));

  Layout1->addWidget(GroupPoints, 1, 0);
  Layout1->addWidget(GroupDimensions, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBasicGUI = theBasicGUI;
  Init();
}


//=================================================================================
// function : ~BasicGUI_VectorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_VectorDlg::~BasicGUI_VectorDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::Init()
{
    /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myDx = 0.0;
  myDy = 0.0;
  myDz = 200.0;
  myPoint1.SetCoord(0.0, 0.0, 0.0);
  myPoint2.SetCoord(0.0, 0.0, 0.0);
  myOkPoint1 = myOkPoint2 = false;

  /*  Vertices Filter for all arguments */
  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  /* min, max, step and decimals for spin boxes */
  GroupDimensions->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);

  GroupDimensions->SpinBox_DX->SetValue(myDx);
  GroupDimensions->SpinBox_DY->SetValue(myDy);
  GroupDimensions->SpinBox_DZ->SetValue(myDz);

  GroupDimensions->CheckBox1->setChecked(FALSE);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DZ, SLOT(SetStep(double)));

  connect(GroupDimensions->CheckBox1, SIGNAL(stateChanged(int)), this, SLOT(ReverseVector(int)));

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
void BasicGUI_VectorDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  mySelection->ClearFilters();
  myGeomBase->EraseSimulationShape();
  disconnect(mySelection, 0, this, 0);
  myOkPoint1 = myOkPoint2 = false;

  switch (constructorId)
    {
    case 0:
      {
	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();

	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText("");
 	GroupPoints->LineEdit2->setText("");

	/* filter for next selection */
	mySelection->AddFilter(myVertexFilter);
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    case 1:
      {
	GroupPoints->hide();
	resize( 0, 0 );
	GroupDimensions->show();

	myDx = 0.0;
	myDy = 0.0;
	myDz = 200.0;

	GroupDimensions->SpinBox_DX->SetValue(myDx);
	GroupDimensions->SpinBox_DY->SetValue(myDy);
	GroupDimensions->SpinBox_DZ->SetValue(myDz);

	myPoint1.SetCoord(0.0, 0.0, 0.0) ;
	myPoint2.SetCoord(myDx, myDy, myDz);

	GroupDimensions->CheckBox1->setChecked(FALSE);

	mySimulationTopoDs = BRepBuilderAPI_MakeEdge(myPoint1, myPoint2).Shape();
	/* Add arrow in simulation */
	bool noNeedToTest = AddArrowToSimulation(mySimulationTopoDs);
	myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
	break;
      }
    }
 return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  switch(myConstructorId)
    { 
    case 0 :
      { 
	if(myOkPoint1 && myOkPoint2) 
	  myBasicGUI->MakeVectorAndDisplay(myPoint1, myPoint2);
	break ;
      }
    case 1 :
      {
	/* Recup args and call method */
	myDx = GroupDimensions->SpinBox_DX->GetValue();
	myDy = GroupDimensions->SpinBox_DY->GetValue();
	myDz = GroupDimensions->SpinBox_DZ->GetValue();
	myPoint1.SetCoord(0.0, 0.0, 0.0);
	myPoint2.SetCoord(myDx, myDy, myDz);	
	myBasicGUI->MakeVectorAndDisplay(myPoint1, myPoint2);
	break;
      }
    }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_VectorDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    if (myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint1 = false;
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkPoint2 = false;
    return;
  }

  // nbSel == 1
  TopoDS_Shape S; 
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1 && myGeomBase->VertexToPoint(S, myPoint1)) {
    myEditCurrentArgument->setText(aString);
    myOkPoint1 = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2 && myGeomBase->VertexToPoint(S, myPoint2)) {
    myEditCurrentArgument->setText(aString);
    myOkPoint2 = true;
  }

  if(myOkPoint1 && myOkPoint2 && myPoint1.Distance(myPoint2) > Precision::Confusion()) {
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge(myPoint1, myPoint2).Shape();
    /* Add arrow in simulation */
    bool noNeedToTest = this->AddArrowToSimulation(mySimulationTopoDs);
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::SetEditCurrentArgument()
{
  if(myConstructorId != 0)
    return;

  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
  }
  mySelection->AddFilter(myVertexFilter);
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::LineEditReturnPressed()
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
void BasicGUI_VectorDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(myConstructorId == 0)
    mySelection->AddFilter(myVertexFilter);
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ValueChangedInSpinBox(double newValue)
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  QObject* send = (QObject*)sender();

  if(send == GroupDimensions->SpinBox_DX)
    myDx = newValue;
  else if (send == GroupDimensions->SpinBox_DY)
    myDy = newValue;
  else if (send == GroupDimensions->SpinBox_DZ)
    myDz = newValue;

  myPoint1.SetCoord(0.0, 0.0, 0.0);
  myPoint2.SetCoord(myDx, myDy, myDz);
  
  if(myPoint1.Distance(myPoint2) > Precision::Confusion()) { // mySimulationTopoDs
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge(myPoint1, myPoint2).Shape();
    /* Create simulation vector with an arrow */
    this->AddArrowToSimulation(mySimulationTopoDs);
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  return;
}


//=================================================================================
// function : AddArrowToSimulation()
// purpose  : An arrow (cone topology) is added to 'modifiedShape'
//          : to simulate a vector or an 'oriented line' display. The result is in 'modifiedShape'.
//          : If an arrow can't be added returns false and 'modifiedShape' isn't modified !
//=================================================================================
bool BasicGUI_VectorDlg::AddArrowToSimulation(TopoDS_Shape& modifiedShape)
{
  TopoDS_Shape arrow;
  /* Try to add a cone simulation shape to show direction of a linear edge */
  if(myGeomBase->CreateArrowForLinearEdge(modifiedShape, arrow)) {
    TopoDS_Compound Comp;
    BRep_Builder B;
    B.MakeCompound (Comp);
    B.Add(Comp, modifiedShape);
    B.Add(Comp, arrow);
    modifiedShape = Comp;
    return true;
  }
  return false;
}


//=================================================================================
// function : ReverseVector()
// purpose  : 'state' not used here
//=================================================================================
void BasicGUI_VectorDlg::ReverseVector(int state)
{
  myGeomBase->EraseSimulationShape(); 
  mySimulationTopoDs.Nullify();

  myDx = -myDx;
  GroupDimensions->SpinBox_DX->SetValue(myDx);
  myDy = -myDy;
  GroupDimensions->SpinBox_DY->SetValue(myDy);
  myDz = -myDz;
  GroupDimensions->SpinBox_DZ->SetValue(myDz);
  
  myPoint1.SetCoord(0.0, 0.0, 0.0);
  myPoint2.SetCoord(myDx, myDy, myDz);
  
  /* In the appropriate constructor */
  if(myPoint1.Distance(myPoint2) > Precision::Confusion()) {
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge(myPoint1, myPoint2).Shape();
    /* Add arrow in simulation */
    bool noNeedToTest = this->AddArrowToSimulation(mySimulationTopoDs);
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  return;
}
