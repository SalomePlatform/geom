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
//  File   : TransformationGUI_TranslationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_TranslationDlg.h"

#include <BRepBuilderAPI_Transform.hxx>
#include "QAD_Config.h"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_TranslationDlg()
// purpose  : Constructs a TransformationGUI_TranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_TranslationDlg::TransformationGUI_TranslationDlg(QWidget* parent, const char* name, TransformationGUI* theTransformationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_TRANSLATION")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_TRANSLATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_TRANSLATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel3Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_DX"));
  GroupPoints->TextLabel3->setText(tr("GEOM_DY"));
  GroupPoints->TextLabel4->setText(tr("GEOM_DZ"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myTransformationGUI = theTransformationGUI;
  Init();
}


//=================================================================================
// function : ~TransformationGUI_TranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_TranslationDlg::~TransformationGUI_TranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myVec.SetCoord(0.0, 0.0, 0.0);
  myOkBase = false;

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);

  GroupPoints->SpinBox_DX->SetValue(0.0);
  GroupPoints->SpinBox_DY->SetValue(0.0);
  GroupPoints->SpinBox_DZ->SetValue(0.0);

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
void TransformationGUI_TranslationDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkBase)
    myTransformationGUI->MakeTranslationAndDisplay(myGeomShape, myVec);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_TranslationDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkBase = false;
    return;
  }

  /* nbSel == 1 */
  TopoDS_Shape S;
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;  
 
  /*  gp_Pnt : not used */
  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return ;
    GroupPoints->LineEdit1->setText(aString);
    myBase = S; 
    myOkBase = true;
  }

  if(myOkBase)
    this->MakeTranslationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::LineEditReturnPressed()
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
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::SetEditCurrentArgument()
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
void TransformationGUI_TranslationDlg::ActivateThisDialog()
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
void TransformationGUI_TranslationDlg::enterEvent(QEvent* e)
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
void TransformationGUI_TranslationDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();
  Standard_Real Dx, Dy, Dz;

  if(send == GroupPoints->SpinBox_DX) {
    Dx = newValue;
    Dy = GroupPoints->SpinBox_DY->GetValue();
    Dz = GroupPoints->SpinBox_DZ->GetValue();
  }
  else if(send == GroupPoints->SpinBox_DY) {
    Dx = GroupPoints->SpinBox_DX->GetValue();
    Dy = newValue;
    Dz = GroupPoints->SpinBox_DZ->GetValue();
  }
  else if(send == GroupPoints->SpinBox_DZ) {
    Dx = GroupPoints->SpinBox_DX->GetValue();
    Dy = GroupPoints->SpinBox_DY->GetValue();
    Dz = newValue;
  }

  myVec.SetCoord(Dx, Dy, Dz);
  if(myOkBase)
    MakeTranslationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeTranslationSimulationAndDisplay()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::MakeTranslationSimulationAndDisplay()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  
  try {
    gp_Trsf theTransformation;
    theTransformation.SetTranslation(myVec);
    BRepBuilderAPI_Transform myBRepTransformation(myBase, theTransformation, Standard_False);
    mySimulationTopoDs = myBRepTransformation.Shape();
    if(mySimulationTopoDs.IsNull())
      return;
    else
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeTranslationSimulationAndDisplay");
    return;
  }
  return;
}
