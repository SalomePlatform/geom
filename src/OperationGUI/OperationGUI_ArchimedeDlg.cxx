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
//  File   : OperationGUI_ArchimedeDlg.cxx
//  Author : Nicolas REJNERI 
//  Module : GEOM
//  $Header$

using namespace std;
#include "OperationGUI_ArchimedeDlg.h"

#include "QAD_Config.h"

//=================================================================================
// class    : OperationGUI_ArchimedeDlg()
// purpose  : Constructs a OperationGUI_ArchimedeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ArchimedeDlg::OperationGUI_ArchimedeDlg(QWidget* parent,  const char* name, OperationGUI* theOperationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ARCHIMEDE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));
  
  setCaption(tr("GEOM_ARCHIMEDE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ARCHIMEDE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel3Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_WEIGHT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_WATER_DENSITY"));
  GroupPoints->TextLabel4->setText(tr("GEOM_MESHING_DEFLECTION"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myOperationGUI = theOperationGUI;
  Init();
}


//=================================================================================
// function : ~OperationGUI_ArchimedeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ArchimedeDlg::~OperationGUI_ArchimedeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myWeight = 100.0;
  myWaterDensity = 1.0;
  myMeshingDeflection = 0.01;
  myOkIO = false;

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();


  double SpecificStep1 = 0.1;
  double SpecificStep2 = 0.01;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_DZ->RangeStepAndValidator(0.001, 999.999, SpecificStep2, 3);

  GroupPoints->SpinBox_DX->SetValue(myWeight);
  GroupPoints->SpinBox_DY->SetValue(myWaterDensity);
  GroupPoints->SpinBox_DZ->SetValue(myMeshingDeflection);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DZ, SLOT(SetStep(double)));
  
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
void OperationGUI_ArchimedeDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if(myOkIO)
    myOperationGUI->Archimede(myIO, myWeight, myWaterDensity, myMeshingDeflection);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_ArchimedeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    myOkIO = false;
    return;
  }
  else {
    myEditCurrentArgument->setText(aString);
    myIO = mySelection->firstIObject();
    myOkIO = true;
  }
  return; 
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::LineEditReturnPressed()
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::SetEditCurrentArgument()
{    
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    this->SelectionIntoArgument();
  }

  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::enterEvent(QEvent* e)
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
void OperationGUI_ArchimedeDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_DX)
    myWeight = newValue;
  else if(send == GroupPoints->SpinBox_DY)
    myWaterDensity = newValue;
  else if(send == GroupPoints->SpinBox_DZ)
    myMeshingDeflection = newValue;

  return;
}
