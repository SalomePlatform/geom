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
//  File   : PrimitiveGUI_BoxDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "PrimitiveGUI_BoxDlg.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>
#include "QAD_Config.h"

//=================================================================================
// class    : PrimitiveGUI_BoxDlg()
// purpose  : Constructs a PrimitiveGUI_BoxDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_BoxDlg::PrimitiveGUI_BoxDlg(QWidget* parent, const char* name, PrimitiveGUI* thePrimitiveGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_DLG_BOX_2P")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_DLG_BOX_DXYZ")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_BOX_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_BOX"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_DIAGONAL_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_3Spin(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_DX"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_DY"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_DZ"));

  Layout1->addWidget(GroupPoints, 1, 0);
  Layout1->addWidget(GroupDimensions, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myPrimitiveGUI = thePrimitiveGUI;
  Init();
}


//=================================================================================
// function : ~DialogBox_Box()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_BoxDlg::~PrimitiveGUI_BoxDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::Init()
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = GroupPoints->LineEdit1;

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

  double initValue = 200.0;
  GroupDimensions->SpinBox_DX->SetValue(initValue);
  GroupDimensions->SpinBox_DY->SetValue(initValue);
  GroupDimensions->SpinBox_DZ->SetValue(initValue);

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
void PrimitiveGUI_BoxDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  mySelection->ClearFilters();
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
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
	resize(0, 0);
	GroupDimensions->show();

	double initValue = 200.0;
	GroupDimensions->SpinBox_DX->SetValue(initValue);
	GroupDimensions->SpinBox_DY->SetValue(initValue);
	GroupDimensions->SpinBox_DZ->SetValue(initValue);

	myPoint1.SetCoord(0.0, 0.0, 0.0) ;
	myPoint2.SetCoord(initValue, initValue, initValue);

	mySimulationTopoDs = BRepPrimAPI_MakeBox(myPoint1, myPoint2).Shape();
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
void PrimitiveGUI_BoxDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ClickOnApply()
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
	  myPrimitiveGUI->MakeBoxAndDisplay(myPoint1, myPoint2);
	break;
      }
    case 1 :
      {
	/* Recup args and call method */
	double vx = GroupDimensions->SpinBox_DX->GetValue();
	double vy = GroupDimensions->SpinBox_DY->GetValue();
	double vz = GroupDimensions->SpinBox_DZ->GetValue();
	myPoint1.SetCoord(0.0, 0.0, 0.0);
	myPoint2.SetCoord(vx, vy, vz);
	myPrimitiveGUI->MakeBoxAndDisplay(myPoint1, myPoint2);
	break;
      }
    }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void PrimitiveGUI_BoxDlg::SelectionIntoArgument()
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

  if(myOkPoint1 && myOkPoint2 && TestBoxDimensions(myPoint1, myPoint2)) {
    mySimulationTopoDs = BRepPrimAPI_MakeBox(myPoint1, myPoint2).Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::SetEditCurrentArgument()
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
void PrimitiveGUI_BoxDlg::LineEditReturnPressed()
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
void PrimitiveGUI_BoxDlg::ActivateThisDialog()
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
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::enterEvent(QEvent* e)
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
void PrimitiveGUI_BoxDlg::ValueChangedInSpinBox(double newValue)
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  QObject* send = (QObject*)sender();
  double vx, vy, vz;

  if(send == GroupDimensions->SpinBox_DX) {
    vx = newValue;
    vy = GroupDimensions->SpinBox_DY->GetValue();
    vz = GroupDimensions->SpinBox_DZ->GetValue();
  }
  else if(send == GroupDimensions->SpinBox_DY) {
    vx = GroupDimensions->SpinBox_DX->GetValue();
    vy = newValue;
    vz = GroupDimensions->SpinBox_DZ->GetValue();
  }
  else if(send == GroupDimensions->SpinBox_DZ) {
    vx = GroupDimensions->SpinBox_DX->GetValue();
    vy = GroupDimensions->SpinBox_DY->GetValue();
    vz = newValue;
  }

  myPoint1.SetCoord(0.0, 0.0, 0.0);
  myPoint2.SetCoord(vx, vy, vz);

  if(TestBoxDimensions(myPoint1, myPoint2)) {
    mySimulationTopoDs = BRepPrimAPI_MakeBox(myPoint1, myPoint2).Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  return;
}


//=================================================================================
// function : TestBoxDimensions()
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::TestBoxDimensions(gp_Pnt P1, gp_Pnt P2)
{
  if ((fabs(P1.X() - P2.X()) > Precision::Confusion()) &&
      (fabs(P1.Y() - P2.Y()) > Precision::Confusion()) &&
      (fabs(P1.Z() - P2.Z()) > Precision::Confusion()))
    return true;
  return false;
}
