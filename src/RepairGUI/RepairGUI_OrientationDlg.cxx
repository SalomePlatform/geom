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
//  File   : RepairGUI_OrientationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_OrientationDlg.h"

#include <Precision.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <TopExp_Explorer.hxx>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : RepairGUI_OrientationDlg()
// purpose  : Constructs a RepairGUI_OrientationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_OrientationDlg::RepairGUI_OrientationDlg(QWidget* parent, const char* name, RepairGUI* theRepairGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ORIENTATION")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ORIENTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ORIENTATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel1Spin1Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR_LENGTH"));
  GroupPoints->CheckButton1->setText(tr("GEOM_ORIENTATION_OPT"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myRepairGUI = theRepairGUI;
  Init();
}


//=================================================================================
// function : ~RepairGUI_OrientationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_OrientationDlg::~RepairGUI_OrientationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myLength = 25.0;
  myOkShape = false;

  double SpecificStep = 10;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(myLength);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));

  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ReverseAngle(int)));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkShape && GroupPoints->CheckButton1->isChecked()) {	  
    myRepairGUI->MakeOrientationChangeAndDisplay(myGeomShape);
    GroupPoints->CheckButton1->setChecked(FALSE);
  }
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void RepairGUI_OrientationDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkShape = false;
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
    myShape = S;
    myOkShape = true;
  }

  if(myOkShape)
    this->MakeOrientationSimulationAndDisplay();
  return; 
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::LineEditReturnPressed()
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
void RepairGUI_OrientationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::ActivateThisDialog()
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
void RepairGUI_OrientationDlg::enterEvent(QEvent* e)
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
void RepairGUI_OrientationDlg::ValueChangedInSpinBox(double newValue)
{
  myLength = newValue;
  if(myOkShape)
    this->MakeOrientationSimulationAndDisplay();
  return;
}


//=================================================================================
// function : ReverseOrientation()
// purpose  :
//=================================================================================
void RepairGUI_OrientationDlg::ReverseAngle(int state)
{
  if(myOkShape)
    this->MakeOrientationSimulationAndDisplay();
  return;
}


//==================================================================================
// function : MakeOrientationSimulationAndDisplay()
// purpose  : Create 'aCompound1' and 'aCompound2' each contains edges oriented
//	    : respectively FORWARD and REVERSE for all faces of 'aTopoDS'
//	    : These edges represent normal vectors on faces of 'aTopoDS'
//          : For a unique edge an arrow is displayed to show its orientation.
//==================================================================================
void RepairGUI_OrientationDlg::MakeOrientationSimulationAndDisplay()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  
  TopoDS_Compound aCompound1, aCompound2;
  TopoDS_Compound NullComp;
  BRep_Builder aBuilder1, aBuilder2;
  aCompound1 = aCompound2 = NullComp;
  aBuilder1.MakeCompound(aCompound1);
  aBuilder2.MakeCompound(aCompound2);
  
  if(myShape.IsNull())
    return;

  try {
    /* Case of an edge */
    if(myShape.ShapeType() == TopAbs_EDGE) {
      /* Try to display a cone simulation shape to show direction of edge */
      TopoDS_Shape tmpShape = myShape ;
      if(GroupPoints->CheckButton1->isChecked()) {
	if(myShape.Orientation() == TopAbs_FORWARD)
	  tmpShape.Orientation(TopAbs_REVERSED);
	else
	  tmpShape.Orientation(TopAbs_FORWARD);
      }
      if(myGeomBase->CreateArrowForLinearEdge(tmpShape, mySimulationTopoDs))
	myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
      return;
    }

    gp_Pnt P1, P2 ;
    gp_Vec V, V1, V2;
    TopExp_Explorer ex(myShape, TopAbs_FACE);
    int test = 0;
    while(ex.More()) {
      const TopoDS_Face& F = TopoDS::Face(ex.Current());
      /* find the center of the minmax */
      BRepAdaptor_Surface SF(F);
      Standard_Real u, v, x;
    
      u = SF.FirstUParameter();
      x = SF.LastUParameter();
      if(Precision::IsInfinite(u)) {
	if(Precision::IsInfinite(x))
	  u = 0.0;
	else
	  u = x;
      }
      else if(!Precision::IsInfinite(x))
	u = (u+x) / 2.0;
    
      v = SF.FirstVParameter();
      x = SF.LastVParameter();
      if(Precision::IsInfinite(v)) {
	if(Precision::IsInfinite(x))
	  v = 0.0;
	else
	  v = x;
      }
      else if(!Precision::IsInfinite(x))
	v = (v+x) / 2.0;
    
      SF.D1(u, v, P1, V1, V2);
      V = V1.Crossed(V2);
      x = V.Magnitude();
      if(x > 1.e-10)
	V.Multiply(myLength/x);
      else {
	V.SetCoord(myLength/2.0, 0.0, 0.0);
	MESSAGE ("Null normal in Orientation " << endl);
      }
    
      P2 = P1;
      /* test orientation of each face and displays forward (aCompound1) */
      if(F.Orientation() == TopAbs_FORWARD)
	P2.Translate(V);
      else
	P2.Translate(-V);    
      BRepBuilderAPI_MakeEdge anEdge(P1, P2);
      aBuilder1.Add(aCompound1, anEdge);
    
      P2 = P1;
      /* test orientation of each face and displays forward (aCompound2) */
      if(F.Orientation() == TopAbs_FORWARD)
	P2.Translate(-V);
      else
	P2.Translate(V);    
      anEdge = BRepBuilderAPI_MakeEdge(P1, P2);
      aBuilder2.Add(aCompound2, anEdge);
    
      ex.Next();
      test++;
    }
  
    /* display simulation compounds */
    if(test > 0 && GroupPoints->CheckButton1->isChecked())
      mySimulationTopoDs = aCompound1;    
    else if(test > 0 && !GroupPoints->CheckButton1->isChecked())
      mySimulationTopoDs = aCompound2;

    if(!mySimulationTopoDs.IsNull())
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  catch(Standard_Failure)
    MESSAGE("Exception catched in MakeOrientationSimulationAndDisplay");
  return;
}
