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
//  File   : EntityGUI_SketcherDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "EntityGUI_SketcherDlg.h"
#include "Sketcher_Profile.hxx"
#include "utilities.h"

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

#include <qpushbutton.h>

//=================================================================================
// class    : EntityGUI_SketcherDlg()
// purpose  : Constructs a EntityGUI_SketcherDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_SketcherDlg::EntityGUI_SketcherDlg(QWidget* parent, const char* name, EntityGUI* theEntityGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :EntityGUI_Skeleton_QTD(parent, name, modal, fl)
{
  if (!name)
    setName("EntityGUI_SketcherDlg");

  buttonCancel->setText(tr("GEOM_BUT_CANCEL"));
  buttonEnd->setText(tr("GEOM_BUT_END_SKETCH"));
  buttonClose->setText(tr("GEOM_BUT_CLOSE_SKETCH"));
  GroupVal->close(TRUE);
  GroupDest2->close(TRUE);
  GroupDest3->close(TRUE);
  resize(0, 0);

  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SKETCHER_TITLE"));

  GroupConstructors->setTitle(tr("GEOM_SKETCHER_EL"));
  RadioButton1->setText(tr("GEOM_SKETCHER_SEGMENT"));
  RadioButton2->setText(tr("GEOM_SKETCHER_ARC"));
  GroupDest->setTitle(tr("GEOM_SKETCHER_DEST"));
  GroupDest1->setTitle(tr("GEOM_SKETCHER_TYPE"));
  RB_Dest1->setText(tr("GEOM_SKETCHER_POINT"));
  RB_Dest2->setText(tr("GEOM_SKETCHER_DIR"));

  /***************************************************************/
  GroupPt = new EntityGUI_Point_QTD(GroupDest, "GroupPt");
  GroupPt->GroupPoint->setTitle(tr("GEOM_SKETCHER_POINT"));
  GroupPt->RB_Point1->setText(tr("GEOM_SKETCHER_ABS"));
  GroupPt->RB_Point2->setText(tr("GEOM_SKETCHER_REL"));
  GroupPt->RB_Point3->setText(tr("GEOM_SKETCHER_SEL"));

  GroupD1 = new EntityGUI_Dir1_QTD(GroupDest, "GroupD1");
  GroupD1->GroupDir1->setTitle(tr("GEOM_SKETCHER_DIR"));
  GroupD1->RB_Dir11->setText(tr("GEOM_SKETCHER_ANGLE"));
  GroupD1->RB_Dir12->setText(tr("GEOM_SKETCHER_PER"));
  GroupD1->RB_Dir13->setText(tr("GEOM_SKETCHER_TAN"));
  GroupD1->RB_Dir14->setText(tr("GEOM_SKETCHER_VXVY"));

  GroupD2 = new EntityGUI_Dir2_QTD(GroupDest, "GroupD2");
  GroupD2->GroupDir2->setTitle(tr("GEOM_SKETCHER_DIR"));
  GroupD2->RB_Dir21->setText(tr("GEOM_SKETCHER_LENGTH"));
  GroupD2->RB_Dir22->setText(tr("GEOM_SKETCHER_X"));
  GroupD2->RB_Dir23->setText(tr("GEOM_SKETCHER_Y"));

  Group1Sel = new EntityGUI_1Sel_QTD(this, "Group1Sel");
  Group1Sel->TextLabel1->setText(tr("GEOM_SKETCHER_POINT2"));
  Group1Sel->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group1Sel->buttonUndo->setText(tr("GEOM_SKETCHER_UNDO"));
  Group1Sel->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group1Sel->PushButton1->setPixmap(image0);

  Group1Spin = new EntityGUI_1Spin(this, "Group1Spin");
  Group1Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group1Spin->buttonUndo->setText(tr("GEOM_SKETCHER_UNDO"));
  Group1Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));

  Group2Spin = new EntityGUI_2Spin(this, "Group2Spin");
  Group2Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group2Spin->buttonUndo->setText(tr("GEOM_SKETCHER_UNDO"));
  Group2Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));

  Group3Spin = new EntityGUI_3Spin(this, "Group3Spin");
  Group3Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group3Spin->buttonUndo->setText(tr("GEOM_SKETCHER_UNDO"));
  Group3Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));

  Group4Spin = new EntityGUI_4Spin(this, "Group4Spin");
  Group4Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group4Spin->buttonUndo->setText(tr("GEOM_SKETCHER_UNDO"));
  Group4Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));

  Layout5->addMultiCellWidget(GroupPt, 1, 1, 0, 1);
  Layout5->addWidget(GroupD1, 1, 0);
  Layout5->addWidget(GroupD2, 1, 1);

  Layout1->addWidget(Group1Sel, 2, 0);
  Layout1->addWidget(Group1Spin, 2, 0);
  Layout1->addWidget(Group2Spin, 2, 0);
  Layout1->addWidget(Group3Spin, 2, 0);
  Layout1->addWidget(Group4Spin, 2, 0);
  /***************************************************************/

  mySelection = Sel;
  mySimulationTopoDs1.Nullify();
  mySimulationTopoDs2.Nullify();
  myEntityGUI = theEntityGUI;

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeomGUI->SetActiveDialogBox((QDialog*)this);
  myGeom = myGeomGUI->myComponentGeom;

  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* signals and slots connections */
  connect(buttonEnd, SIGNAL(clicked()), this, SLOT(ClickOnEnd()));
  connect(buttonClose, SIGNAL(clicked()), this, SLOT(ClickOnClose()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));

  connect(Group1Sel->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group1Sel->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group1Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group1Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group2Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group2Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group3Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group3Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group4Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group4Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));

  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(TypeClicked(int)));
  connect(GroupDest1, SIGNAL(clicked(int)), this, SLOT(DestClicked(int)));
  connect(GroupPt->GroupPoint, SIGNAL(clicked(int)), this, SLOT(PointClicked(int)));
  connect(GroupD1->GroupDir1, SIGNAL(clicked(int)), this, SLOT(Dir1Clicked(int)));
  connect(GroupD2->GroupDir2, SIGNAL(clicked(int)), this, SLOT(Dir2Clicked(int)));

  connect(Group1Sel->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1Sel->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group1Spin->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2Spin->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2Spin->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group4Spin->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group4Spin->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group4Spin->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group4Spin->SpinBox_DS, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group1Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group2Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group2Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DZ, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DZ, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DS, SLOT(SetStep(double)));

  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  /* Move widget on the botton right corner of main widget */
//   int x, y;
//   myGeomBase->DefineDlgPosition( this, x, y );

  myEntityGUI = theEntityGUI;
  this->Init();
  this->show();
}


//=================================================================================
// function : ~EntityGUI_SketcherDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_SketcherDlg::~EntityGUI_SketcherDlg()
{
  // no need to delete child widgets, Qt does it all for us
  this->destroy(TRUE, TRUE);
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = Group1Sel->LineEdit1;

  /* min, max, step and decimals for spin boxes */
  Group1Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group2Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group2Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group4Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, 0.1, 3);
  Group4Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, 0.1, 3);
  Group4Spin->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group4Spin->SpinBox_DS->RangeStepAndValidator(-999.999, 999.999, 5., 3);

  Group1Spin->SpinBox_DX->SetValue(0.0);
  Group2Spin->SpinBox_DX->SetValue(0.0);
  Group2Spin->SpinBox_DY->SetValue(0.0);
  Group3Spin->SpinBox_DX->SetValue(0.0);
  Group3Spin->SpinBox_DY->SetValue(0.0);
  Group3Spin->SpinBox_DZ->SetValue(0.0);
  Group4Spin->SpinBox_DX->SetValue(0.0);
  Group4Spin->SpinBox_DY->SetValue(0.0);
  Group4Spin->SpinBox_DZ->SetValue(0.0);
  Group4Spin->SpinBox_DS->SetValue(0.0);

  /* displays Dialog */
  Group1Sel->buttonUndo->setEnabled(false);
  Group1Spin->buttonUndo->setEnabled(false);
  Group2Spin->buttonUndo->setEnabled(false);
  Group3Spin->buttonUndo->setEnabled(false);
  Group4Spin->buttonUndo->setEnabled(false);

  RadioButton1->setChecked(true);

  myLastOp = 0;
  myLastX1 = 0.0;
  myLastY1 = 0.0;
  myLastX2 = 0.0;
  myLastY2 = 0.0;

  mySketchState = FIRST_POINT;
  this->TypeClicked(0);

  return;
}


//=================================================================================
// function : InitClick()
// purpose  : 
//=================================================================================
void EntityGUI_SketcherDlg::InitClick()
{
  disconnect(mySelection, 0, this, 0);

  Group2Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group2Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);

  Group1Sel->hide();
  Group1Spin->hide();
  Group2Spin->hide();
  Group3Spin->hide();
  Group4Spin->hide();

  resize(0, 0);

  return;
}


//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::TypeClicked(int constructorId)
{
  myConstructorId = constructorId;
  if(myConstructorId == 0) {  // SEGMENT
    GroupD2->setEnabled(true);
    RB_Dest1->setEnabled(true);
    RB_Dest1->setChecked(true);
    this->DestClicked(1);
  }
  else if(myConstructorId == 1) {  // ARC
    GroupD2->setEnabled(false);
    RB_Dest1->setEnabled(false);
    RB_Dest2->setChecked(true);
    this->DestClicked(0);
  }
  return;
}


//=================================================================================
// function : DestClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::DestClicked(int constructorId)
{
  GroupPt->hide();
  GroupD1->hide();
  GroupD2->hide();

  if(constructorId == 1) {  // Point
    GroupPt->RB_Point1->setChecked(true);
    GroupPt->show();
    this->PointClicked(1);  // XY
  }
  else if(constructorId == 0) {  // Direction
    GroupD1->RB_Dir11->setChecked(true);
    GroupD1->show();
    GroupD2->show();
    this->Dir1Clicked(2);  // Angle
  }
  return;
}


//=================================================================================
// function : PointClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::PointClicked(int constructorId)
{
  this->InitClick();

  if(myConstructorId == 0) {  // SEGMENT
    if(constructorId == 1) {  // XY
      mySketchType = PT_ABS;
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X2"));
      Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y2"));
      myX = 0.0;
      Group2Spin->SpinBox_DX->SetValue(myX);
      myY = 0.0;
      Group2Spin->SpinBox_DY->SetValue(myY);
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();
      this->MakeSimulationAndDisplay();
    }
    else if(constructorId == 0) {  // DXDY
      mySketchType = PT_RELATIVE;
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_DX2"));
      Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_DY2"));
      myDX = 0.0;
      Group2Spin->SpinBox_DX->SetValue(myDX);
      myDY = 0.0;
      Group2Spin->SpinBox_DY->SetValue(myDY);
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();
      this->MakeSimulationAndDisplay();
    }
    else if(constructorId == 2) {  // Selection
      mySketchType = PT_SEL;
      myEditCurrentArgument = Group1Sel->LineEdit1;
      connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
      Group1Sel->show();
      Group1Sel->buttonApply->setFocus();
      this->SelectionIntoArgument();
    }
  }

  return;
}


//=================================================================================
// function : Dir1Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir1Clicked(int constructorId)
{
  myConstructorDirId = constructorId;
  GroupD2->RB_Dir21->setChecked(true);
  this->Dir2Clicked(2);
  return;
}


//=================================================================================
// function : Dir2Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir2Clicked(int constructorId)
{
  this->InitClick();
  myAngle = 0.0;

  if(myConstructorId == 0) {  // SEGMENT
    myX = 0.0;
    myY = 0.0;
    myLength = 100.0;
    if(myConstructorDirId == 2) {  // Angle
      Group2Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, 5., 3);
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_ANGLE2"));
      Group2Spin->SpinBox_DX->SetValue(myAngle);
      Group2Spin->buttonApply->setFocus();
      Group2Spin->show();

      if(constructorId == 2) {  // Length
	mySketchType = DIR_ANGLE_LENGTH;
	Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_LENGTH2"));
	Group2Spin->SpinBox_DY->SetValue(myLength);
      }
      else if(constructorId == 0) {  // X
	mySketchType = DIR_ANGLE_X;
	Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_X3"));
	Group2Spin->SpinBox_DY->SetValue(myX);
      }
      else if(constructorId == 1) {  // Y
	mySketchType = DIR_ANGLE_Y;
	Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y3"));
	Group2Spin->SpinBox_DY->SetValue(myY);
      }
    }
    else if(myConstructorDirId == 0) {  // Perpendicular
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if(constructorId == 2) {  // Length
	mySketchType = DIR_PER_LENGTH;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_LENGTH2"));
	Group1Spin->SpinBox_DX->SetValue(myLength);
      }
      else if(constructorId == 0) {  // X
	mySketchType = DIR_PER_X;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X3"));
	Group1Spin->SpinBox_DX->SetValue(myX);
      }
      else if(constructorId == 1) {  // Y
	mySketchType = DIR_PER_Y;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_Y3"));
	Group1Spin->SpinBox_DX->SetValue(myY);
      }
    }
    else if(myConstructorDirId == 1) {  // Tangent
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if(constructorId == 2) {  // Length
	mySketchType = DIR_TAN_LENGTH;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_LENGTH2"));
	Group1Spin->SpinBox_DX->SetValue(myLength);
      }
      else if(constructorId == 0) {  // X
	mySketchType = DIR_TAN_X;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X3"));
	Group1Spin->SpinBox_DX->SetValue(myX);
      }
      else if(constructorId == 1) {  // Y
	mySketchType = DIR_TAN_Y;
	Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_Y3"));
	Group1Spin->SpinBox_DX->SetValue(myY);
      }
    }
    else if(myConstructorDirId == 3) {  // DXDY
      Group3Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, 0.1, 3);
      Group3Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, 0.1, 3);
      Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_VX2"));
      Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_VY2"));
      myDX = 0.0;
      Group3Spin->SpinBox_DX->SetValue(myDX);
      myDY = 0.0;
      Group3Spin->SpinBox_DY->SetValue(myDY);
      Group3Spin->show();
      Group3Spin->buttonApply->setFocus();

      if(constructorId == 2) {  // Length
	mySketchType = DIR_DXDY_LENGTH;
	Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_LENGTH2"));
	Group3Spin->SpinBox_DZ->SetValue(myLength);
      }
      else if(constructorId == 0) {  // X
	mySketchType = DIR_DXDY_X;
	Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_X3"));
	Group3Spin->SpinBox_DZ->SetValue(myX);
      }
      else if(constructorId == 1) {  // Y
	mySketchType = DIR_DXDY_Y;
	Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_Y3"));
	Group3Spin->SpinBox_DZ->SetValue(myY);
      }
    }
  }
  else if(myConstructorId == 1) {  // ARC
    if(myConstructorDirId == 2) {  // Angle
      if(constructorId == 2) {  // Length
	mySketchType = DIR_ANGLE_LENGTH;
	Group3Spin->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, 5., 3);
	Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, 5., 3);
	Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_ANGLE2"));
	Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_RADIUS2"));
	Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_ANGLE2"));
	Group3Spin->SpinBox_DX->SetValue(myAngle);
	myRadius = 100.0;
	Group3Spin->SpinBox_DY->SetValue(myRadius);
	myLength = 30.0;
	Group3Spin->SpinBox_DZ->SetValue(myLength);
	Group3Spin->show();
	Group3Spin->buttonApply->setFocus();
      }
    }
    else if(myConstructorDirId == 0) {  // Perpendicular
      if(constructorId == 2) {  // Length
	mySketchType = DIR_PER_LENGTH;
	Group2Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, 5., 3);
	Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_RADIUS2"));
	Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_ANGLE2"));
	myRadius = 100.0;
	Group2Spin->SpinBox_DX->SetValue(myRadius);
	myLength = 30.0;
	Group2Spin->SpinBox_DY->SetValue(myLength);
	Group2Spin->show();
	Group2Spin->buttonApply->setFocus();
      }
    }
    else if(myConstructorDirId == 1) {  // Tangent
      if(constructorId == 2) {  // Length
	mySketchType = DIR_TAN_LENGTH;
	Group2Spin->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, 5., 3);
	Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_RADIUS2"));
	Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_ANGLE2"));
	myRadius = 100.0;
	Group2Spin->SpinBox_DX->SetValue(myRadius);
	myLength = 30.0;
	Group2Spin->SpinBox_DY->SetValue(myLength);
	Group2Spin->show();
	Group2Spin->buttonApply->setFocus();
      }
    }
    else if(myConstructorDirId == 3) {  // DXDY
      if(constructorId == 2) {  // Length
	mySketchType = DIR_DXDY_LENGTH;
	Group4Spin->TextLabel1->setText(tr("GEOM_SKETCHER_VX2"));
	Group4Spin->TextLabel2->setText(tr("GEOM_SKETCHER_VY2"));
	Group4Spin->TextLabel3->setText(tr("GEOM_SKETCHER_RADIUS2"));
	Group4Spin->TextLabel4->setText(tr("GEOM_SKETCHER_ANGLE2"));
	myDX = 0.0;
	Group4Spin->SpinBox_DX->SetValue(myDX);
	myDY = 0.0;
	Group4Spin->SpinBox_DY->SetValue(myDY);
	myRadius = 100.0;
	Group4Spin->SpinBox_DZ->SetValue(myRadius);
	myLength = 30.0;
	Group4Spin->SpinBox_DS->SetValue(myLength);
	Group4Spin->show();
	Group4Spin->buttonApply->setFocus();
      }
    }
  }

  this->MakeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnCancel()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  myEntityGUI->EraseSimulationShape();
  mySimulationTopoDs1.Nullify();
  mySimulationTopoDs2.Nullify();

  mySelection->ClearFilters();
  disconnect(mySelection, 0, this, 0);

  reject();
  return;
}


//=================================================================================
// function : ClickOnEnd()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnEnd()
{
  if(mySketchState != FIRST_POINT && mySketchState != SECOND_POINT)
    myEntityGUI->OnSketchEnd(myCommand.ascii());

  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnClose()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnClose()
{
  if(mySketchState != FIRST_POINT && mySketchState != SECOND_POINT) {
    myCommand = myCommand + ":WW";
    myEntityGUI->OnSketchEnd(myCommand.ascii());
  }

  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnApply()
{
  if(mySketchState == FIRST_POINT) {
    mySketchState = SECOND_POINT;
    myLastX2 = myLastX1;
    myLastY2 = myLastY1;
  }
  else {
    if(mySketchState != SECOND_POINT) {
      Group1Sel->buttonUndo->setEnabled(true);
      Group1Spin->buttonUndo->setEnabled(true);
      Group2Spin->buttonUndo->setEnabled(true);
      Group3Spin->buttonUndo->setEnabled(true);
      Group4Spin->buttonUndo->setEnabled(true);
    }

    QString myNewCommand = this->GetNewCommand();
    QString myCommandTemp = myCommand + myNewCommand;

    Sketcher_Profile aProfile (myCommandTemp.ascii());

    if(aProfile.IsDone()) {
      myLastCommand = myCommand;
      myCommand = myCommandTemp;
      myLastX2 = myLastX1;
      myLastY2 = myLastY1;
      mySketchState = NEXT_POINT;
    }
  }

  myLastOp = myConstructorId;
  this->MakeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : ClickOnUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnUndo()
{
  Group1Sel->buttonUndo->setEnabled(false);
  Group1Spin->buttonUndo->setEnabled(false);
  Group2Spin->buttonUndo->setEnabled(false);
  Group3Spin->buttonUndo->setEnabled(false);
  Group4Spin->buttonUndo->setEnabled(false);

  myCommand = myLastCommand;
  this->MakeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void EntityGUI_SketcherDlg::SelectionIntoArgument()
{
  myEntityGUI->EraseSimulationShape(1); //Juste Shape2!!

  Group1Sel->buttonApply->setEnabled(false);
  Group1Sel->buttonApply->setFocus();

  myEditCurrentArgument->setText("");
  QString aString = "";

  myX = 0.0;
  myY = 0.0;

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  gp_Pnt myPoint1;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();

  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(myEditCurrentArgument == Group1Sel->LineEdit1 && myGeomBase->VertexToPoint(S, myPoint1)) {
    myX = myPoint1.X();
    myY = myPoint1.Y();
    Group1Sel->LineEdit1->setText(aString);

    Group1Sel->buttonApply->setEnabled(true);
    Group1Sel->buttonApply->setFocus();
    this->MakeSimulationAndDisplay();
  }

  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  mySelection->ClearFilters();
  if(send == Group1Sel->PushButton1) {
    //Group1Sel->LineEdit1->setFocus();
    myEditCurrentArgument = Group1Sel->LineEdit1;
  }
  mySelection->AddFilter(myVertexFilter);
  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::LineEditReturnPressed()
{
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text();
  QWidget* thisWidget = (QWidget*)this;
  if(myGeomBase->SelectionByNameInDialogs(thisWidget, objectUserName, mySelection))
    myEditCurrentArgument->setText(objectUserName);

  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::DeactivateActiveDialog()
{
  this->setEnabled(false);
  mySelection->ClearFilters();
  disconnect(mySelection, 0, this, 0);
  myEntityGUI->EraseSimulationShape();
  myGeomGUI->SetActiveDialogBox(0);
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog();
  this->setEnabled(true);
  myGeomGUI->SetActiveDialogBox((QDialog*)this);
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myVertexFilter);
  myEntityGUI->DisplaySimulationShape(mySimulationTopoDs1, mySimulationTopoDs2);

  return;
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();
  Standard_Real vx, vy, vz, vs;
  vx = vy = vz = vs = 0.0;

  if(send == Group1Spin->SpinBox_DX) {
    vx = newValue;
    Group1Spin->buttonApply->setFocus();
  }
  else if(send == Group2Spin->SpinBox_DX) {
    vx = newValue;
    vy = Group2Spin->SpinBox_DY->GetValue();
    Group2Spin->buttonApply->setFocus();
  }
  else if(send == Group2Spin->SpinBox_DY) {
    vx = Group2Spin->SpinBox_DX->GetValue();
    vy = newValue;
    Group2Spin->buttonApply->setFocus();
  }
  else if(send == Group3Spin->SpinBox_DX) {
    vx = newValue;
    vy = Group3Spin->SpinBox_DY->GetValue();
    vz = Group3Spin->SpinBox_DZ->GetValue();
    Group3Spin->buttonApply->setFocus();
  }
  else if(send == Group3Spin->SpinBox_DY) {
    vx = Group3Spin->SpinBox_DX->GetValue();
    vy = newValue;
    vz = Group3Spin->SpinBox_DZ->GetValue();
    Group3Spin->buttonApply->setFocus();
  }
  else if(send == Group3Spin->SpinBox_DZ) {
    vx = Group3Spin->SpinBox_DX->GetValue();
    vy = Group3Spin->SpinBox_DY->GetValue();
    vz = newValue;
    Group3Spin->buttonApply->setFocus();
  }
  else if(send == Group4Spin->SpinBox_DX) {
    vx = newValue;
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = Group4Spin->SpinBox_DS->GetValue();
    Group4Spin->buttonApply->setFocus();
  }
  else if(send == Group4Spin->SpinBox_DY) {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = newValue;
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = Group4Spin->SpinBox_DS->GetValue();
    Group4Spin->buttonApply->setFocus();
  }
  else if(send == Group4Spin->SpinBox_DZ) {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = newValue;
    vs = Group4Spin->SpinBox_DS->GetValue();
    Group4Spin->buttonApply->setFocus();
  }
  else if(send == Group4Spin->SpinBox_DS) {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = newValue;
    Group4Spin->buttonApply->setFocus();
  }

  if(myConstructorId == 0) {  // SEGMENT
    if(mySketchType == PT_ABS) {
      myX = vx;
      myY = vy;
    }
    else if(mySketchType == PT_RELATIVE) {
      myDX = vx;
      myDY = vy;
    }
    else if(mySketchType == DIR_ANGLE_LENGTH) {
      myAngle = vx;
      myLength = vy;
    }
    else if(mySketchType == DIR_ANGLE_X) {
      myAngle = vx;
      myX = vy;
    }
    else if(mySketchType == DIR_ANGLE_Y) {
      myAngle = vx;
      myY = vy;
    }
    else if(mySketchType == DIR_PER_LENGTH) {
      myLength = vx;
    }
    else if(mySketchType == DIR_PER_X) {
      myX = vx;
    }
    else if(mySketchType == DIR_PER_Y) {
      myY = vx;
    }
    else if(mySketchType == DIR_TAN_LENGTH) {
      myLength = vx;
    }
    else if(mySketchType == DIR_TAN_X) {
      myX = vx;
    }
    else if(mySketchType == DIR_TAN_Y) {
      myY = vx;
    }
    else if(mySketchType == DIR_DXDY_LENGTH) {
      myDX = vx;
      myDY = vy;
      myLength = vz;
    }
    else if(mySketchType == DIR_DXDY_X) {
      myDX = vx;
      myDY = vy;
      myX = vz;
    }
    else if(mySketchType == DIR_DXDY_Y) {
      myDX = vx;
      myDY = vy;
      myY = vz;
    }
  }
  else if(myConstructorId == 1) {  // ARC
    if(mySketchType == DIR_ANGLE_LENGTH) {
      myAngle = vx;
      myRadius = vy;
      myLength = vz;
    }
    else if(mySketchType == DIR_PER_LENGTH) {
      myRadius = vx;
      myLength = vy;
    }
    else if(mySketchType == DIR_TAN_LENGTH) {
      myRadius = vx;
      myLength = vy;
    }
    else if(mySketchType == DIR_DXDY_LENGTH) {
      myDX = vx;
      myDY = vy;
      myRadius = vz;
      myLength = vs;
    }
  }

  this->MakeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : GetNewCommand()
// purpose  :
//=================================================================================
QString EntityGUI_SketcherDlg::GetNewCommand()
{
  QString myNewCommand = "";
  if(mySketchState == FIRST_POINT)
    return myNewCommand;

  myNewCommand = ":";
  if(myConstructorId == 0) {  // SEGMENT
    if(mySketchType == PT_ABS || mySketchType == PT_SEL)
      myNewCommand = myNewCommand + "TT " + QString::number(myX) + " " + QString::number(myY);
    if(mySketchType == PT_RELATIVE)
      myNewCommand = myNewCommand + "T " + QString::number(myDX) + " " + QString::number(myDY);
    if(mySketchType == DIR_ANGLE_LENGTH) {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if(mySketchType == DIR_ANGLE_X) {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if(mySketchType == DIR_ANGLE_Y) {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
    if(mySketchType == DIR_PER_LENGTH) {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if(mySketchType == DIR_PER_X) {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if(mySketchType == DIR_PER_Y) {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
    if(mySketchType == DIR_TAN_LENGTH)
      myNewCommand = myNewCommand + "L " + QString::number(myLength);
    if(mySketchType == DIR_TAN_X)
      myNewCommand = myNewCommand + "IX " + QString::number(myX);
    if(mySketchType == DIR_TAN_Y)
      myNewCommand = myNewCommand + "IY " + QString::number(myY);
    if(mySketchType == DIR_DXDY_LENGTH) {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if(mySketchType == DIR_DXDY_X) {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if(mySketchType == DIR_DXDY_Y) {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
  }
  else if(myConstructorId == 1) {  // ARC
    if(mySketchType == DIR_ANGLE_LENGTH) {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if(mySketchType == DIR_PER_LENGTH) {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if(mySketchType == DIR_TAN_LENGTH) {
      myNewCommand = myNewCommand + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if(mySketchType == DIR_DXDY_LENGTH) {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
  }
  return myNewCommand;
}


//=================================================================================
// function : MakeSimulationAndDisplay()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::MakeSimulationAndDisplay()
{
  myEntityGUI->EraseSimulationShape();
  mySimulationTopoDs1.Nullify();
  mySimulationTopoDs2.Nullify();
  
  try {
    if(mySketchState == FIRST_POINT) {
      myCommand = "Sketch ";
      myCommand = myCommand + ":" + "F " + QString::number(myX) + " " + QString::number(myY);

      myLastX1 = myX;
      myLastY1 = myY;

    }

    Sketcher_Profile aProfile1 (myCommand.ascii());

    if(aProfile1.IsDone()) {
      myShape = aProfile1.GetShape();
      mySimulationTopoDs1 = myShape;
    }

    if(mySketchState != FIRST_POINT) {
      if(mySketchState == SECOND_POINT) {
	gp_Pnt pt = BRep_Tool::Pnt(TopoDS::Vertex(myShape));
	myLastX1 = pt.X();
	myLastY1 = pt.Y();
      }
      else {
	TopoDS_Vertex V1, V2;
	TopExp::Vertices(TopoDS::Wire(myShape), V1, V2);
	gp_Pnt pt = BRep_Tool::Pnt(V2);
	myLastX1 = pt.X();
	myLastY1 = pt.Y();
	if(myLastOp == 1) {   // ARC
	  myLastDir = aProfile1.GetLastDir();
	  myLastX2 = myLastX1 - myLastDir.X();
	  myLastY2 = myLastY1 - myLastDir.Y();
	}
      }
      QString myNewCommand = "Sketch ";
      myNewCommand = myNewCommand + ":" + "F " + QString::number(myLastX1) + " " + QString::number(myLastY1);
      if(mySketchState != SECOND_POINT && !((myLastX1 - myLastX2) == 0 && (myLastY1 - myLastY2) == 0))
	myNewCommand = myNewCommand + ":" + "D " + QString::number(myLastX1 - myLastX2) + " " + QString::number(myLastY1 - myLastY2);
      myNewCommand = myNewCommand + this->GetNewCommand();
      Sketcher_Profile aProfile2 (myNewCommand.ascii());
      if(aProfile2.IsDone())
	mySimulationTopoDs2 = aProfile2.GetShape();

      if ( mySimulationTopoDs2.IsNull() ) {
	if ( Group1Sel->isVisible() ) {
	  Group1Sel->buttonApply->setEnabled(false);
	  Group1Sel->buttonApply->setFocus();
	}
	if ( Group1Spin->isVisible() ) {
	  Group1Spin->buttonApply->setEnabled(false);
	  Group1Spin->buttonApply->setFocus();
	}
	if ( Group2Spin->isVisible() ) {
	  Group2Spin->buttonApply->setEnabled(false);
	  Group2Spin->buttonApply->setFocus();
	}
	if ( Group3Spin->isVisible() ) {
	  Group3Spin->buttonApply->setEnabled(false);
	  Group3Spin->buttonApply->setFocus();
	}
	if ( Group4Spin->isVisible() ) {
	  Group4Spin->buttonApply->setEnabled(false);
	  Group4Spin->buttonApply->setFocus();
	}
      } else {
	if ( Group1Sel->isVisible() ) {
	  Group1Sel->buttonApply->setEnabled(true);
	  Group1Sel->buttonApply->setFocus();
	}
	if ( Group1Spin->isVisible() ) {
	  Group1Spin->buttonApply->setEnabled(true);
	  Group1Spin->buttonApply->setFocus();
	}
	if ( Group2Spin->isVisible() ) {
	  Group2Spin->buttonApply->setEnabled(true);
	  Group2Spin->buttonApply->setFocus();
	}
	if ( Group3Spin->isVisible() ) {
	  Group3Spin->buttonApply->setEnabled(true);
	  Group3Spin->buttonApply->setFocus();
	}
	if ( Group4Spin->isVisible() ) {
	  Group4Spin->buttonApply->setEnabled(true);
	  Group4Spin->buttonApply->setFocus();
	}
      }
    }

    myEntityGUI->DisplaySimulationShape(mySimulationTopoDs1, mySimulationTopoDs2);
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeSimulationAndDisplay");
    return;
  }
  return;
}
