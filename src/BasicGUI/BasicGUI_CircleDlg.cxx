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
//  File   : BasicGUI_CircleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BasicGUI_CircleDlg.h"

#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include "QAD_Config.h"

//=================================================================================
// class    : BasicGUI_CircleDlg()
// purpose  : Constructs a BasicGUI_CircleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_CircleDlg::BasicGUI_CircleDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CIRCLE_PV")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CIRCLE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CIRCLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_CENTER_POINT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBasicGUI = theBasicGUI;
  Init();
}


//=================================================================================
// function : ~BasicGUI_CircleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_CircleDlg::~BasicGUI_CircleDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myRadius = 100.0;
  myOkPoint1 = myOkDir = false;

  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);
  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupPoints->SpinBox_DX->SetValue(myRadius);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  
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
void BasicGUI_CircleDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkPoint1 && myOkDir)
    myBasicGUI->MakeCircleAndDisplay(myPoint1, myDir, myRadius);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_CircleDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape(); 
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint1 = false;
    else if (myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkDir = false;
    return;
  }

  /* nbSel == 1 */
  TopoDS_Shape S;  
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;  
 
  /*  gp_Pnt : not used */
  if(myEditCurrentArgument == GroupPoints->LineEdit1 && myGeomBase->VertexToPoint(S, myPoint1)) {
    GroupPoints->LineEdit1->setText(aString);
    myOkPoint1 = true;
  }    
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    BRepAdaptor_Curve curv(TopoDS::Edge(S));
    myDir = curv.Line().Direction();
    GroupPoints->LineEdit2->setText(aString);
    myOkDir = true;
  }

  if(myOkPoint1 && myOkDir)
    this->MakeCircleSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    mySelection->AddFilter(myVertexFilter);
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
void BasicGUI_CircleDlg::LineEditReturnPressed()
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
void BasicGUI_CircleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  mySelection->AddFilter(myVertexFilter);
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::enterEvent(QEvent* e)
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
void BasicGUI_CircleDlg::ValueChangedInSpinBox(double newValue)
{
  myRadius = newValue;
  if(myOkPoint1 && myOkDir)
    MakeCircleSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeCircleSimulationAndDisplay()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::MakeCircleSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  try {
    gp_Ax2 anAxis(myPoint1, myDir);
    gp_Circ circ(anAxis, myRadius);
    BRepBuilderAPI_MakeEdge MakeEdge(circ);
    mySimulationTopoDs = MakeEdge.Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeCircleSimulationAndDisplay");
  }
  return;
}
