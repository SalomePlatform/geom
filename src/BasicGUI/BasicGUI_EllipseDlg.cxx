//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE 
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
//  File   : BasicGUI_EllipseDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BasicGUI_EllipseDlg.h"

#include <gp_Lin.hxx>
#include <gp_Elips.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAdaptor_Curve.hxx>
#include "QAD_Config.h"

//=================================================================================
// class    : BasicGUI_EllipseDlg()
// purpose  : Constructs a BasicGUI_EllipseDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_EllipseDlg::BasicGUI_EllipseDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ELLIPSE_PV")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ELLIPSE_TITLE"));
  
  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ELLIPSE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_CENTER"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS_MINOR"));
  GroupPoints->TextLabel4->setText(tr("GEOM_RADIUS_MAJOR"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBasicGUI = theBasicGUI;
  Init();
}


//=================================================================================
// function : ~BasicGUI_EllipseDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_EllipseDlg::~BasicGUI_EllipseDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myMajorRadius = 200.0;
  myMinorRadius = 100.0;
  myOkPoint = myOkDir = false;

  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);
  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupPoints->SpinBox_DX->SetValue(myMajorRadius);
  GroupPoints->SpinBox_DY->SetValue(myMinorRadius);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  
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
void BasicGUI_EllipseDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkPoint && myOkDir)
    myBasicGUI->MakeEllipseAndDisplay(myPoint, myDir, myMajorRadius, myMinorRadius); 
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_EllipseDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint = false;
    else if (myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkDir = false;
    return;
  }

  /* nbSel == 1 */
  TopoDS_Shape S;  
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;  
 
  /*  gp_Pnt : not used */
  if(myEditCurrentArgument == GroupPoints->LineEdit1 && myGeomBase->VertexToPoint(S, myPoint)) {
    GroupPoints->LineEdit1->setText(aString);
    myOkPoint = true;
  }    
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    BRepAdaptor_Curve curv(TopoDS::Edge(S));
    myDir = curv.Line().Direction();
    GroupPoints->LineEdit2->setText(aString);
    myOkDir = true;
  }

  if(myOkPoint && myOkDir)
    this->MakeEllipseSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::SetEditCurrentArgument()
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
void BasicGUI_EllipseDlg::LineEditReturnPressed()
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
void BasicGUI_EllipseDlg::ActivateThisDialog()
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
void BasicGUI_EllipseDlg::enterEvent(QEvent* e)
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
void BasicGUI_EllipseDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_DX )
    myMajorRadius = newValue;
  else if(send == GroupPoints->SpinBox_DY)
    myMinorRadius = newValue;

  if (myOkPoint && myOkDir)
    MakeEllipseSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeEllipseSimulationAndDisplay()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::MakeEllipseSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myMajorRadius < myMinorRadius)
    return;

  try {
    gp_Ax2 anAxis(myPoint, myDir);
    gp_Elips ellipse(anAxis, myMajorRadius, myMinorRadius);
    BRepBuilderAPI_MakeEdge MakeEdge(ellipse);
    mySimulationTopoDs = MakeEdge.Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeEllipseSimulationAndDisplay");
  }
  return;
}
