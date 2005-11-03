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

#include "EntityGUI_SketcherDlg.h"
#include "Sketcher_Profile.hxx"
#include "GEOM_Displayer.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_ResourceMgr.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qpushbutton.h>
#include <qlabel.h>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : EntityGUI_SketcherDlg()
// purpose  : Constructs a EntityGUI_SketcherDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_SketcherDlg::EntityGUI_SketcherDlg(GeometryGUI* GUI, QWidget* parent, const char* name, bool modal, WFlags fl)
  :EntityGUI_Skeleton_QTD(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose), myIsAllAdded( false ),
   GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
   myGeometryGUI( GUI )
{
  myGeometryGUI->SetActiveDialogBox(this);

  if ( !name ) setName("EntityGUI_SketcherDlg");

  buttonCancel->setText(tr("GEOM_BUT_CANCEL"));
  buttonEnd->setText(tr("GEOM_BUT_END_SKETCH"));
  buttonClose->setText(tr("GEOM_BUT_CLOSE_SKETCH"));

  GroupVal->close(TRUE);        
  GroupDest2->close(TRUE);
  GroupDest3->close(TRUE);

  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_UNDO")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_REDO")));

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
  Group1Sel->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group1Sel->PushButton1->setPixmap(image0);
  Group1Sel->buttonUndo->setPixmap(image1);
  Group1Sel->buttonRedo->setPixmap(image2);
  Group1Sel->LineEdit1->setReadOnly( true );

  Group1Spin = new EntityGUI_1Spin(this, "Group1Spin");
  Group1Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group1Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group1Spin->buttonUndo->setPixmap(image1);
  Group1Spin->buttonRedo->setPixmap(image2);

  Group2Spin = new EntityGUI_2Spin(this, "Group2Spin");
  Group2Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group2Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group2Spin->buttonUndo->setPixmap(image1);
  Group2Spin->buttonRedo->setPixmap(image2);

  Group3Spin = new EntityGUI_3Spin(this, "Group3Spin");
  Group3Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group3Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group3Spin->buttonUndo->setPixmap(image1);
  Group3Spin->buttonRedo->setPixmap(image2);

  Group4Spin = new EntityGUI_4Spin(this, "Group4Spin");
  Group4Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group4Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group4Spin->buttonUndo->setPixmap(image1);
  Group4Spin->buttonRedo->setPixmap(image2);

  Layout5->addMultiCellWidget(GroupPt, 1, 1, 0, 1);
  Layout5->addWidget(GroupD1, 1, 0);
  Layout5->addWidget(GroupD2, 1, 1);

  Layout1->addWidget(Group1Sel, 2, 0);
  Layout1->addWidget(Group1Spin, 2, 0);
  Layout1->addWidget(Group2Spin, 2, 0);
  Layout1->addWidget(Group3Spin, 2, 0);
  Layout1->addWidget(Group4Spin, 2, 0);
  /***************************************************************/

  /* signals and slots connections */
  connect(buttonEnd, SIGNAL(clicked()), this, SLOT(ClickOnEnd()));
  connect(buttonClose, SIGNAL(clicked()), this, SLOT(ClickOnEnd()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));

  connect(Group1Sel->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group1Sel->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group1Sel->buttonRedo, SIGNAL(clicked()), this, SLOT(ClickOnRedo()));
  connect(Group1Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group1Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group1Spin->buttonRedo, SIGNAL(clicked()), this, SLOT(ClickOnRedo()));
  connect(Group2Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group2Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group2Spin->buttonRedo, SIGNAL(clicked()), this, SLOT(ClickOnRedo()));
  connect(Group3Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group3Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group3Spin->buttonRedo, SIGNAL(clicked()), this, SLOT(ClickOnRedo()));
  connect(Group4Spin->buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(Group4Spin->buttonUndo, SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group4Spin->buttonRedo, SIGNAL(clicked()), this, SLOT(ClickOnRedo()));

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

  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group1Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group2Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group2Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Spin->SpinBox_DZ, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DZ, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group4Spin->SpinBox_DS, SLOT(SetStep(double)));

  connect(myGeometryGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeometryGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  Init();
}


//=================================================================================
// function : ~EntityGUI_SketcherDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_SketcherDlg::~EntityGUI_SketcherDlg()
{
	myGeometryGUI->SetActiveDialogBox( 0 );
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = Group1Sel->LineEdit1;
  myCommand.append( "Sketcher" );
  myUndoCommand.append( "Sketcher" );

  mySketchState = FIRST_POINT;
  globalSelection( GEOM_POINT );

  myLastX1 = 0.0;
  myLastY1 = 0.0;
  myLastX2 = 0.0;
  myLastY2 = 0.0;

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  /* min, max, step and decimals for spin boxes */
  Group1Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group2Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group3Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group3Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group4Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 0.1, 3);
  Group4Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 0.1, 3);
  Group4Spin->SpinBox_DZ->RangeStepAndValidator(-999999.999, 999999.999, step, 3);
  Group4Spin->SpinBox_DS->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);

  /* displays Dialog */
  GroupConstructors->setEnabled(false);
  GroupDest1->setEnabled(false);
  setEnabledUndo(false);
  setEnabledRedo(false);

  RadioButton1->setChecked(true);

  resize( 0, 0 );
  TypeClicked(0);

  GEOMBase_Helper::displayPreview();
}


//=================================================================================
// function : InitClick()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::InitClick()
{
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);

  Group1Sel->hide();
  Group1Spin->hide();
  Group2Spin->hide();
  Group3Spin->hide();
  Group4Spin->hide();

  resize(0, 0);
}


//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::TypeClicked(int constructorId)
{
  myConstructorId = constructorId;
  if ( myConstructorId == 0 )     // SEGMENT
  {
    GroupD2->setEnabled(true);
    RB_Dest1->setEnabled(true);
    RB_Dest1->setChecked(true);
    DestClicked(1);
  }
  else if (  myConstructorId == 1 ) // ARC
  {
    GroupD2->setEnabled(false);
    RB_Dest1->setEnabled(false);
    RB_Dest2->setChecked(true);
    DestClicked(0);
  }
}


//=================================================================================
// function : DestClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::DestClicked( int constructorId )
{
  GroupPt->hide();
  GroupD1->hide();
  GroupD2->hide();

  if ( constructorId == 1 )
  {  // Point
    GroupPt->RB_Point1->setChecked(true);
    GroupPt->show();
    PointClicked(1);  // XY
  }
  else if (  constructorId == 0 )
  {  // Direction
    GroupD1->RB_Dir11->setChecked(true);
    GroupD1->show();
    GroupD2->show();
    Dir1Clicked(2);  // Angle
  }
}


//=================================================================================
// function : PointClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::PointClicked(int constructorId)
{
  InitClick();

  if ( myConstructorId == 0 )
  {  // SEGMENT
    if ( constructorId == 1 )
    {  // XY
      mySketchType = PT_ABS;
      Group2Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X2"));
      Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y2"));
      myX = 0.0;
      Group2Spin->SpinBox_DX->SetValue(myX);
      myY = 0.0;
      Group2Spin->SpinBox_DY->SetValue(myY);
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview();
    }
    else if ( constructorId == 0 )
    {  // DXDY
      mySketchType = PT_RELATIVE;
      Group2Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_DX2"));
      Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_DY2"));
      myDX = 0.0;
      Group2Spin->SpinBox_DX->SetValue(myDX);
      myDY = 0.0;
      Group2Spin->SpinBox_DY->SetValue(myDY);
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview();
    }
    else if ( constructorId == 2 )
    {  // Selection
      mySketchType = PT_SEL;
      myEditCurrentArgument = Group1Sel->LineEdit1;
      connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	      SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
      Group1Sel->show();
      Group1Sel->buttonApply->setFocus();
      SelectionIntoArgument();
    }
  }
}


//=================================================================================
// function : Dir1Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir1Clicked(int constructorId)
{
  myConstructorDirId = constructorId;
  GroupD2->RB_Dir21->setChecked(true);
  Dir2Clicked(2);
}


//=================================================================================
// function : Dir2Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir2Clicked(int constructorId)
{
  InitClick();
  myAngle = 0.0;

  if ( myConstructorId == 0 )
  {  // SEGMENT
    myX = 0.0;
    myY = 0.0;
    myLength = 100.0;
    if ( myConstructorDirId == 2 )
    {  // Angle
      Group2Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);
      Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group2Spin->TextLabel1->setText(tr("GEOM_SKETCHER_ANGLE2"));
      Group2Spin->SpinBox_DX->SetValue(myAngle);
      Group2Spin->buttonApply->setFocus();
      Group2Spin->show();

      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_ANGLE_LENGTH;
				Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_LENGTH2"));
				Group2Spin->SpinBox_DY->SetValue(myLength);
      }
      else if ( constructorId == 0 )
      {  // X
				mySketchType = DIR_ANGLE_X;
				Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_X3"));
				Group2Spin->SpinBox_DY->SetValue(myX);
      }
      else if ( constructorId == 1 )
      {  // Y
				mySketchType = DIR_ANGLE_Y;
				Group2Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y3"));
				Group2Spin->SpinBox_DY->SetValue(myY);
      }
    }                   
    else if ( myConstructorDirId == 0 )
    {  // Perpendicular
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_PER_LENGTH;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_LENGTH2"));
				Group1Spin->SpinBox_DX->SetValue(myLength);
      }
      else if ( constructorId == 0 )
      {  // X
				mySketchType = DIR_PER_X;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X3"));
				Group1Spin->SpinBox_DX->SetValue(myX);
      }
      else if ( constructorId == 1 )
      {  // Y
				mySketchType = DIR_PER_Y;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_Y3"));
				Group1Spin->SpinBox_DX->SetValue(myY);
      }
    }
    else if ( myConstructorDirId == 1 )
    {  // Tangent
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_TAN_LENGTH;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_LENGTH2"));
				Group1Spin->SpinBox_DX->SetValue(myLength);
      }
      else if ( constructorId == 0 )
      {  // X
				mySketchType = DIR_TAN_X;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X3"));
				Group1Spin->SpinBox_DX->SetValue(myX);
      }
      else if ( constructorId == 1 )
      {  // Y
				mySketchType = DIR_TAN_Y;
				Group1Spin->TextLabel1->setText(tr("GEOM_SKETCHER_Y3"));
				Group1Spin->SpinBox_DX->SetValue(myY);
      }
    }
    else if ( myConstructorDirId == 3 )
    {  // DXDY
      Group3Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 0.1, 3);
      Group3Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 0.1, 3);
      Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
      Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_VX2"));
      Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_VY2"));
      myDX = 0.0;
      Group3Spin->SpinBox_DX->SetValue(myDX);
      myDY = 0.0;
      Group3Spin->SpinBox_DY->SetValue(myDY);
      Group3Spin->show();
      Group3Spin->buttonApply->setFocus();

      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_DXDY_LENGTH;
				Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_LENGTH2"));
				Group3Spin->SpinBox_DZ->SetValue(myLength);
      }
      else if ( constructorId == 0 )
      {  // X
				mySketchType = DIR_DXDY_X;
				Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_X3"));
				Group3Spin->SpinBox_DZ->SetValue(myX);
      }
      else if ( constructorId == 1 )
      {  // Y
				mySketchType = DIR_DXDY_Y;
				Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_Y3"));
				Group3Spin->SpinBox_DZ->SetValue(myY);
      }
    }
  }
  else if ( myConstructorId == 1 )
  {  // ARC
    if ( myConstructorDirId == 2 )
    {  // Angle
      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_ANGLE_LENGTH;
				Group3Spin->SpinBox_DX->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);
				Group3Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
				Group3Spin->SpinBox_DZ->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);
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
    else if ( myConstructorDirId == 0 )
    {  // Perpendicular
      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_PER_LENGTH;
				Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
				Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);
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
    else if ( myConstructorDirId == 1 )
    {  // Tangent
      if ( constructorId == 2 )
      {  // Length
				mySketchType = DIR_TAN_LENGTH;
				Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 100., 3);
				Group2Spin->SpinBox_DY->RangeStepAndValidator(-999999.999, 999999.999, 5., 3);
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
    else if ( myConstructorDirId == 3 )
    {  // DXDY
      if ( constructorId == 2 )
      {  // Length
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

  GEOMBase_Helper::displayPreview();
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnCancel()
{
  close();
}


//=================================================================================
// function : ClickOnEnd()
// purpose  : connected to buttonEnd AND buttonClose
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnEnd()
{
  if ( sender() == buttonClose )
  {
    // Verify validity of commands
    if ( myCommand.count() <= 2 )
    {
      SUIT_MessageBox::error1( SUIT_Session::session()->activeApplication()->desktop(),
			       tr( "GEOM_ERROR_STATUS" ), tr( "CANNOT_CLOSE" ), tr( "BUT_OK" ) );
      return;
    }
    
    QString Command = myCommand.join( "" ) + GetNewCommand();
    Sketcher_Profile aProfile (Command.ascii());

    Command = myCommand.join( "" );
    aProfile = Sketcher_Profile(Command.ascii());
    TopoDS_Shape myShape;
    if ( aProfile.IsDone() )
      myShape = aProfile.GetShape();
      
    if(myShape.ShapeType() != TopAbs_VERTEX)
      myCommand.append( ":WW" );
  }
  else 
    myIsAllAdded = true;

  if( myCommand.size() > 2 )
    if( !onAccept() )
      return;

  close();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::ClickOnApply()
{
  myCommand.append( GetNewCommand() );
  mySketchState = NEXT_POINT;

  myUndoCommand.clear();
  myUndoCommand.append( "Sketcher" );

  GroupConstructors->setEnabled(true);
  GroupDest1->setEnabled(true);
  setEnabledUndo(true);
  setEnabledRedo(false);

  GEOMBase_Helper::displayPreview();

  return true;
}

//=================================================================================
// function : ClickOnUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnUndo()
{
  myUndoCommand.append( myCommand.last() );
  myCommand.pop_back();

  if(myCommand.count() == 1) {
    mySketchState = FIRST_POINT;
    
    RadioButton1->setChecked(true);
    TypeClicked(0);

    GroupConstructors->setEnabled(false);
    GroupDest1->setEnabled(false);
    setEnabledUndo(false);
  }

  setEnabledRedo(true);

  GEOMBase_Helper::displayPreview();
}

//=================================================================================
// function : ClickOnRedo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnRedo()
{
  myCommand.append( myUndoCommand.last() );
  myUndoCommand.pop_back();

  mySketchState = NEXT_POINT;

  GroupConstructors->setEnabled(true);
  GroupDest1->setEnabled(true);
  setEnabledUndo(true);

  if(myUndoCommand.count() == 1)
    setEnabledRedo(false);

  GEOMBase_Helper::displayPreview();
}

//=================================================================================
// function : setEnabledUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::setEnabledUndo(bool value)
{
  Group1Sel->buttonUndo->setEnabled(value);
  Group1Spin->buttonUndo->setEnabled(value);
  Group2Spin->buttonUndo->setEnabled(value);
  Group3Spin->buttonUndo->setEnabled(value);
  Group4Spin->buttonUndo->setEnabled(value);
}

//=================================================================================
// function : setEnabledRedo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::setEnabledRedo(bool value)
{
  Group1Sel->buttonRedo->setEnabled(value);
  Group1Spin->buttonRedo->setEnabled(value);
  Group2Spin->buttonRedo->setEnabled(value);
  Group3Spin->buttonRedo->setEnabled(value);
  Group4Spin->buttonRedo->setEnabled(value);
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void EntityGUI_SketcherDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  myX = myLastX1;
  myY = myLastY1;

  int nbSel = IObjectCount();
  if ( nbSel == 1 && myEditCurrentArgument == Group1Sel->LineEdit1 )
  {
    Standard_Boolean aRes = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_VERTEX ) ) {
	gp_Trsf aTrans;
	gp_Ax3 aWPlane = myGeometryGUI->GetWorkingPlane();

	aTrans.SetTransformation(aWPlane);
	BRepBuilderAPI_Transform aTransformation(aShape, aTrans, Standard_False);
	aShape = aTransformation.Shape();

	gp_Pnt aPnt;
        if ( GEOMBase::VertexToPoint( aShape, aPnt ) ) {
	  myX = aPnt.X();
	  myY = aPnt.Y();
	  Group1Sel->LineEdit1->setText( GEOMBase::GetName( aSelectedObject ) );
	}
      }
    }
  }

  GEOMBase_Helper::displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::SetEditCurrentArgument()
{
  if ( sender() == Group1Sel->PushButton1 )
  {
    myEditCurrentArgument = Group1Sel->LineEdit1;
    myEditCurrentArgument->setFocus();
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::LineEditReturnPressed()
{
  if ( sender() == Group1Sel->LineEdit1 )
  {
    myEditCurrentArgument = Group1Sel->LineEdit1;

  	/* User name of object input management                          */
  	/* If successfull the selection is changed and signal emitted... */
  	/* so SelectionIntoArgument() is automatically called.           */
  	const QString objectUserName = myEditCurrentArgument->text();
  	QWidget* thisWidget = (QWidget*)this;
  	if(GEOMBase::SelectionByNameInDialogs(thisWidget, objectUserName, selectedIO()))
    	myEditCurrentArgument->setText(objectUserName);
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::DeactivateActiveDialog()
{
  //myGeometryGUI->SetState( -1 );

  setEnabled( false );
  globalSelection();
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  myGeometryGUI->SetActiveDialogBox(0);
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ActivateThisDialog()
{
  myGeometryGUI->EmitSignalDeactivateDialog();
  setEnabled(true);
  myGeometryGUI->SetActiveDialogBox((QDialog*)this);

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  //myGeometryGUI->SetState( 0 );
  globalSelection( GEOM_POINT );

  myEditCurrentArgument = Group1Sel->LineEdit1;
  myEditCurrentArgument->setFocus();

  GEOMBase_Helper::displayPreview();
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::closeEvent(QCloseEvent* e)
{
  //myGeometryGUI->SetState( -1 );
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  QDialog::closeEvent( e );
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

  if ( send == Group1Spin->SpinBox_DX)
  {
    vx = newValue;
  }
  else if ( send == Group2Spin->SpinBox_DX )
  {
    vx = newValue;
    vy = Group2Spin->SpinBox_DY->GetValue();
  }
  else if ( send == Group2Spin->SpinBox_DY)
  {
    vx = Group2Spin->SpinBox_DX->GetValue();
    vy = newValue;
  }
  else if ( send == Group3Spin->SpinBox_DX)
  {
    vx = newValue;
    vy = Group3Spin->SpinBox_DY->GetValue();
    vz = Group3Spin->SpinBox_DZ->GetValue();
  }
  else if ( send == Group3Spin->SpinBox_DY)
  {
    vx = Group3Spin->SpinBox_DX->GetValue();
    vy = newValue;
    vz = Group3Spin->SpinBox_DZ->GetValue();
  }
  else if ( send == Group3Spin->SpinBox_DZ)
  {
    vx = Group3Spin->SpinBox_DX->GetValue();
    vy = Group3Spin->SpinBox_DY->GetValue();
    vz = newValue;
  }
  else if ( send == Group4Spin->SpinBox_DX)
  {
    vx = newValue;
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = Group4Spin->SpinBox_DS->GetValue();
  }
  else if ( send == Group4Spin->SpinBox_DY)
  {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = newValue;
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = Group4Spin->SpinBox_DS->GetValue();
  }
  else if ( send == Group4Spin->SpinBox_DZ)
  {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = newValue;
    vs = Group4Spin->SpinBox_DS->GetValue();
  }
  else if ( send == Group4Spin->SpinBox_DS)
  {
    vx = Group4Spin->SpinBox_DX->GetValue();
    vy = Group4Spin->SpinBox_DY->GetValue();
    vz = Group4Spin->SpinBox_DZ->GetValue();
    vs = newValue;
  }

  if ( myConstructorId == 0 )
  {  // SEGMENT
    if ( mySketchType == PT_ABS)
    {
      myX = vx;
      myY = vy;
    }
    else if ( mySketchType == PT_RELATIVE)
    {
      myDX = vx;
      myDY = vy;
    }
    else if ( mySketchType == DIR_ANGLE_LENGTH)
    {
      myAngle = vx;
      myLength = vy;
    }
    else if ( mySketchType == DIR_ANGLE_X)
    {
      myAngle = vx;
      myX = vy;
    }
    else if ( mySketchType == DIR_ANGLE_Y)
    {
      myAngle = vx;
      myY = vy;
    }
    else if ( mySketchType == DIR_PER_LENGTH)
    {
      myLength = vx;
    }
    else if ( mySketchType == DIR_PER_X)
    {
      myX = vx;
    }
    else if ( mySketchType == DIR_PER_Y)
    {
      myY = vx;
    }
    else if ( mySketchType == DIR_TAN_LENGTH)
    {
      myLength = vx;
    }
    else if ( mySketchType == DIR_TAN_X)
    {
      myX = vx;
    }
    else if ( mySketchType == DIR_TAN_Y)
    {
      myY = vx;
    }
    else if ( mySketchType == DIR_DXDY_LENGTH)
    {
      myDX = vx;
      myDY = vy;
      myLength = vz;
    }
    else if ( mySketchType == DIR_DXDY_X)
    {
      myDX = vx;
      myDY = vy;
      myX = vz;
    }
    else if ( mySketchType == DIR_DXDY_Y)
    {
      myDX = vx;
      myDY = vy;
      myY = vz;
    }
  }
  else if ( myConstructorId == 1 )
  {  // ARC
    if ( mySketchType == DIR_ANGLE_LENGTH)
    {
      myAngle = vx;
      myRadius = vy;
      myLength = vz;
    }
    else if ( mySketchType == DIR_PER_LENGTH)
    {
      myRadius = vx;
      myLength = vy;
    }
    else if ( mySketchType == DIR_TAN_LENGTH)
    {
      myRadius = vx;
      myLength = vy;
    }
    else if ( mySketchType == DIR_DXDY_LENGTH)
    {
      myDX = vx;
      myDY = vy;
      myRadius = vz;
      myLength = vs;
    }
  }

  GEOMBase_Helper::displayPreview();
}


//=================================================================================
// function : GetNewCommand()
// purpose  : Build the new command with context
//=================================================================================
QString EntityGUI_SketcherDlg::GetNewCommand()
{
  QString myNewCommand = ":";
  if ( mySketchState == FIRST_POINT ) {
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL)
      myNewCommand = myNewCommand + "F " + QString::number(myX) + " " + QString::number(myY);
    if ( mySketchType == PT_RELATIVE)
      myNewCommand = myNewCommand + "F " + QString::number(myDX) + " " + QString::number(myDY);
    return myNewCommand;
  }

  if ( myConstructorId == 0  )
  {  // SEGMENT
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL)
      myNewCommand = myNewCommand + "TT " + QString::number(myX) + " " + QString::number(myY);
    if ( mySketchType == PT_RELATIVE)
      myNewCommand = myNewCommand + "T " + QString::number(myDX) + " " + QString::number(myDY);
    if ( mySketchType == DIR_ANGLE_LENGTH)
    {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if ( mySketchType == DIR_ANGLE_X)
    {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if ( mySketchType == DIR_ANGLE_Y)
    {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
    if ( mySketchType == DIR_PER_LENGTH)
    {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if ( mySketchType == DIR_PER_X)
    {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if ( mySketchType == DIR_PER_Y)
    {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
    if ( mySketchType == DIR_TAN_LENGTH)
      myNewCommand = myNewCommand + "L " + QString::number(myLength);
    if ( mySketchType == DIR_TAN_X)
      myNewCommand = myNewCommand + "IX " + QString::number(myX);
    if ( mySketchType == DIR_TAN_Y)
      myNewCommand = myNewCommand + "IY " + QString::number(myY);
    if ( mySketchType == DIR_DXDY_LENGTH)
    {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "L " + QString::number(myLength);
    }
    if ( mySketchType == DIR_DXDY_X)
    {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "IX " + QString::number(myX);
    }
    if ( mySketchType == DIR_DXDY_Y)
    {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "IY " + QString::number(myY);
    }
  }
  else if ( myConstructorId == 1 )
  {  // ARC
    if ( mySketchType == DIR_ANGLE_LENGTH)
    {
      myNewCommand = myNewCommand + "R " + QString::number(myAngle);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if ( mySketchType == DIR_PER_LENGTH)
    {
      myNewCommand = myNewCommand + "R " + QString::number(90.0);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if ( mySketchType == DIR_TAN_LENGTH)
    {
      myNewCommand = myNewCommand + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
    if ( mySketchType == DIR_DXDY_LENGTH)
    {
      myNewCommand = myNewCommand + "D " + QString::number(myDX) + " " + QString::number(myDY);
      myNewCommand = myNewCommand + ":" + "C " + QString::number(myRadius) + " " + QString::number(myLength);
    }
  }
  return myNewCommand;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_SketcherDlg::createOperation()
{
  return getGeomEngine()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::isValid( QString& msg )
{
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::execute( ObjectList& objects )
{
  if(mySketchState == FIRST_POINT) {
    myLastX2 = myX;
    myLastY2 = myY;
  }
  else {
    //Test if the current point is the same as the last one
    TopoDS_Shape myShape1, myShape2;

    //Last Shape
    QString Command1 = myCommand.join( "" );
    Sketcher_Profile aProfile1 (Command1.ascii());
    if(aProfile1.IsDone())
      myShape1 = aProfile1.GetShape();

    //Current Shape
    QString Command2 = Command1 + GetNewCommand();
    Sketcher_Profile aProfile2 (Command2.ascii());
    if(aProfile2.IsDone())
      myShape2 = aProfile2.GetShape();

    if(myShape2.IsNull()) {
      //the current point is the same as the last one
      myLastX2 = myLastX1;
      myLastY2 = myLastY1;
    }
    else {
      TopoDS_Vertex V1, V2;
      gp_Pnt pt;
      if(myShape1.ShapeType() == TopAbs_VERTEX) {
	//the last shape is the first point
	pt = BRep_Tool::Pnt(TopoDS::Vertex(myShape1));
	myLastX1 = pt.X();
	myLastY1 = pt.Y();
      }
      else {
	TopExp::Vertices(TopoDS::Wire(myShape1), V1, V2);
	pt = BRep_Tool::Pnt(V2);
	myLastX1 = pt.X();
	myLastY1 = pt.Y();
      }
      TopExp::Vertices(TopoDS::Wire(myShape2), V1, V2);
      pt = BRep_Tool::Pnt(V2);
      myLastX2 = pt.X();
      myLastY2 = pt.Y();
    }
  }

  QString cmd;
  if( ( mySketchState != FIRST_POINT && myLastX1 == myLastX2 && myLastY1 == myLastY2 ) || myIsAllAdded ) {
    cmd = myCommand.join( "" );

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
  }
  else {
    cmd = myCommand.join( "" ) + GetNewCommand();

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
    
  gp_Ax3 myWPlane = myGeometryGUI->GetWorkingPlane();
  GEOM::ListOfDouble_var WPlane = new GEOM::ListOfDouble;
  WPlane->length(9);
  WPlane[0] = myWPlane.Location().X();
  WPlane[1] = myWPlane.Location().Y();
  WPlane[2] = myWPlane.Location().Z();

  WPlane[3] = myWPlane.Direction().X();
  WPlane[4] = myWPlane.Direction().Y();
  WPlane[5] = myWPlane.Direction().Z();

  WPlane[6] = myWPlane.XDirection().X();
  WPlane[7] = myWPlane.XDirection().Y();
  WPlane[8] = myWPlane.XDirection().Z();

  GEOM::GEOM_Object_var anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeSketcher( cmd.latin1(), WPlane );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying preview of resulting shape
//            Redefined from GEOMBase_Helper.
//================================================================
void EntityGUI_SketcherDlg::displayPreview( GEOM::GEOM_Object_ptr object,
                                            const bool            append,
                                            const bool            activate,
                                            const bool            update,
                                            const double          lineWidth )
{
  // Set color for preview shape
  getDisplayer()->SetColor( Quantity_NOC_RED );

  // set width of displayed shape
  getDisplayer()->SetWidth( lineWidth );

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  getDisplayer()->SetName( myGeometryGUI->getApp()->orb()->object_to_string( object ) );

  // Create wire from applayed object
  TopoDS_Shape anApplyedWire, aLastSegment;
  if ( !createShapes( object, anApplyedWire, aLastSegment ) )
    return;

  // Build prs
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( anApplyedWire );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->SetColor( Quantity_NOC_VIOLET );
  aPrs = getDisplayer()->BuildPrs( aLastSegment );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->UnsetName();

  // Enable activation of displayed objects
  getDisplayer()->SetToActivate( true );
}

//================================================================
// Function : createShapes
// Purpose  : Create applyed wire, and last segment from entry object
//================================================================
bool EntityGUI_SketcherDlg::createShapes( GEOM::GEOM_Object_ptr theObject,
                                          TopoDS_Shape&         theApplyedWire,
                                          TopoDS_Shape&         theLastSegment )
{
  TopoDS_Shape aShape;
  if ( !GEOMBase::GetShape( theObject, aShape ) ||
       aShape.ShapeType() != TopAbs_WIRE && aShape.ShapeType() != TopAbs_VERTEX )
    return false;

  if ( Group1Sel->isVisible()  && !Group1Sel->buttonApply->isEnabled()  ||
       Group1Spin->isVisible() && !Group1Spin->buttonApply->isEnabled() ||
       Group2Spin->isVisible() && !Group2Spin->buttonApply->isEnabled() ||
       Group3Spin->isVisible() && !Group3Spin->buttonApply->isEnabled() ||
       Group4Spin->isVisible() && !Group4Spin->buttonApply->isEnabled() )
  {
     theApplyedWire = aShape;
     return true;
  }

  BRepBuilderAPI_MakeWire aBuilder;
  TopExp_Explorer anExp( aShape, TopAbs_EDGE );
  while( 1 )
  {
    TopoDS_Shape anEdge = anExp.Current();
    anExp.Next();
    if ( anExp.More() ) // i.e. non-last edge
      aBuilder.Add( TopoDS::Edge( anEdge ) );
    else
    {
      theLastSegment = anEdge;
      break;
    }
  }

  if ( aBuilder.IsDone() )
    theApplyedWire = aBuilder.Shape();

  return true;
}








