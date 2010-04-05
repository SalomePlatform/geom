//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_SketcherDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "EntityGUI_SketcherDlg.h"
#include "EntityGUI_Widgets.h"
#include <QtxDoubleSpinBox.h>

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <QKeyEvent>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Sketcher_Profile.hxx>

#include <SalomeApp_Study.h>

#include <gp_Pln.hxx>

//=================================================================================
// class    : EntityGUI_SketcherDlg()
// purpose  : Constructs a EntityGUI_SketcherDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_SketcherDlg::EntityGUI_SketcherDlg( GeometryGUI* GUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl,
                                              const double lineWidth )
  : QDialog( parent, fl ),
    myIsAllAdded( false ),
    myIsApply( false ),
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
    myGeometryGUI( GUI ),
    myLineWidth( lineWidth )
{
  setModal( modal );
  setAttribute( Qt::WA_DeleteOnClose );

  myGeometryGUI->SetActiveDialogBox(this);

  MainWidget = new EntityGUI_Skeleton( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 9 ); topLayout->setSpacing( 6 );

  MainWidget->buttonCancel->setText( tr( "GEOM_BUT_CANCEL" ) );
  MainWidget->buttonEnd->setText( tr( "GEOM_BUT_END_SKETCH" ) );
  MainWidget->buttonClose->setText( tr( "GEOM_BUT_CLOSE_SKETCH" ) );
  MainWidget->buttonHelp->setText( tr( "GEOM_BUT_HELP" ) );

  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_UNDO" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_REDO" ) ) );

  setWindowTitle( tr( "GEOM_SKETCHER_TITLE" ) );

  MainWidget->GroupConstructors->setTitle( tr( "GEOM_SKETCHER_EL" ) );
  MainWidget->RadioButton1->setText( tr( "GEOM_SKETCHER_SEGMENT" ) );
  MainWidget->RadioButton2->setText( tr( "GEOM_SKETCHER_ARC" ) );
  MainWidget->GroupDest->setTitle( tr( "GEOM_SKETCHER_DEST" ) );
  MainWidget->GroupDest1->setTitle( tr( "GEOM_SKETCHER_TYPE" ) );
  MainWidget->RB_Dest1->setText( tr( "GEOM_SKETCHER_POINT" ) );
  MainWidget->RB_Dest2->setText( tr( "GEOM_SKETCHER_DIR" ) );

  /***************************************************************/

  GroupBox1 = new QGroupBox(tr("GEOM_CS"), this);
  QGridLayout* OwnLayout = new QGridLayout(GroupBox1);
  OwnLayout->setSpacing(6);
  OwnLayout->setMargin(11);

  ComboBox1 = new QComboBox(GroupBox1);
  OwnLayout->addWidget(ComboBox1);

  topLayout->addWidget(GroupBox1);
  topLayout->addWidget( MainWidget );

  GroupPt = new EntityGUI_Point( MainWidget->DestCnt );

  GroupPt->GroupPoint->setTitle( tr( "GEOM_SKETCHER_POINT" ) );
  GroupPt->RB_Point1->setText( tr( "GEOM_SKETCHER_ABS" ) );
  GroupPt->RB_Point2->setText( tr( "GEOM_SKETCHER_REL" ) );
  GroupPt->RB_Point3->setText( tr( "GEOM_SKETCHER_SEL" ) );

  GroupD1 = new EntityGUI_Dir1( MainWidget->DestCnt );

  GroupD1->GroupDir1->setTitle( tr( "GEOM_SKETCHER_DIR" ) );
  GroupD1->RB_Dir11->setText( tr( "GEOM_SKETCHER_ANGLE" ) );
  GroupD1->RB_Dir12->setText( tr( "GEOM_SKETCHER_PER" ) );
  GroupD1->RB_Dir13->setText( tr( "GEOM_SKETCHER_TAN" ) );
  GroupD1->RB_Dir14->setText( tr( "GEOM_SKETCHER_VXVY" ) );

  GroupD2 = new EntityGUI_Dir2( MainWidget->DestCnt );

  GroupD2->GroupDir2->setTitle( tr( "GEOM_SKETCHER_DIR" ) );
  GroupD2->RB_Dir21->setText( tr( "GEOM_SKETCHER_LENGTH" ) );
  GroupD2->RB_Dir22->setText( tr( "GEOM_SKETCHER_X" ) );
  GroupD2->RB_Dir23->setText( tr( "GEOM_SKETCHER_Y" ) );

  Group1Sel = new EntityGUI_1Sel( MainWidget->SkeletonCnt );

  Group1Sel->TextLabel1->setText( tr( "GEOM_SKETCHER_POINT2" ) );
  Group1Sel->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group1Sel->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group1Sel->PushButton1->setIcon( image0 );
  Group1Sel->buttonUndo->setIcon( image1 );
  Group1Sel->buttonRedo->setIcon( image2 );
  Group1Sel->LineEdit1->setReadOnly( true );

  Group1Spin = new EntityGUI_1Spin( MainWidget->SkeletonCnt );
  Group1Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group1Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group1Spin->buttonUndo->setIcon( image1 );
  Group1Spin->buttonRedo->setIcon( image2 );

  Group2Spin = new EntityGUI_2Spin( MainWidget->SkeletonCnt );
  Group2Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group2Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group2Spin->buttonUndo->setIcon( image1 );
  Group2Spin->buttonRedo->setIcon( image2 );

  Group3Spin = new EntityGUI_3Spin( MainWidget->SkeletonCnt );
  Group3Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group3Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group3Spin->buttonUndo->setIcon( image1 );
  Group3Spin->buttonRedo->setIcon( image2 );

  Group4Spin = new EntityGUI_4Spin( MainWidget->SkeletonCnt );
  Group4Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group4Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group4Spin->buttonUndo->setIcon( image1 );
  Group4Spin->buttonRedo->setIcon( image2 );

  QGridLayout* DestCntLayout = new QGridLayout( MainWidget->DestCnt );
  DestCntLayout->setMargin( 0 ); DestCntLayout->setSpacing( 6 );
  DestCntLayout->addWidget( GroupPt, 0, 0, 1, 2 );
  DestCntLayout->addWidget( GroupD1, 1, 0 );
  DestCntLayout->addWidget( GroupD2, 1, 1 );

  QVBoxLayout* SkeletonCntlayout = new QVBoxLayout( MainWidget->SkeletonCnt );
  SkeletonCntlayout->setMargin( 0 ); SkeletonCntlayout->setSpacing( 6 );
  SkeletonCntlayout->addWidget( Group1Sel,  0, 0 );
  SkeletonCntlayout->addWidget( Group1Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group2Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group3Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group4Spin, 0, 0 );
  /***************************************************************/

  QButtonGroup* ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( MainWidget->RB_Dest1, 1 );
  ButtonGroup->addButton( MainWidget->RB_Dest2, 0 );

  /* signals and slots connections */
  connect( MainWidget->buttonEnd,    SIGNAL( clicked() ), this, SLOT( ClickOnEnd() ) );
  connect( MainWidget->buttonClose,  SIGNAL( clicked() ), this, SLOT( ClickOnEnd() ) );
  connect( MainWidget->buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) );
  connect( MainWidget->buttonHelp,   SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  connect( Group1Sel->buttonApply,   SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( Group1Sel->buttonUndo,    SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group1Sel->buttonRedo,    SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) );
  connect( Group1Spin->buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( Group1Spin->buttonUndo,   SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group1Spin->buttonRedo,   SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) );
  connect( Group2Spin->buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( Group2Spin->buttonUndo,   SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group2Spin->buttonRedo,   SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) );
  connect( Group3Spin->buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( Group3Spin->buttonUndo,   SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group3Spin->buttonRedo,   SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) ) ;
  connect( Group4Spin->buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( Group4Spin->buttonUndo,   SIGNAL( clicked() ), this, SLOT( ClickOnUndo() ) );
  connect( Group4Spin->buttonRedo,   SIGNAL( clicked() ), this, SLOT( ClickOnRedo() ) );

  connect( MainWidget->ButtonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( TypeClicked( int ) ) );
  connect( ButtonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( DestClicked( int ) ) );
  connect( GroupPt->ButtonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( PointClicked( int ) ) );
  connect( GroupD1->ButtonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( Dir1Clicked( int ) ) );
  connect( GroupD2->ButtonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( Dir2Clicked( int ) ));

  connect( Group1Sel->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group1Sel->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( Group1Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group2Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group2Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DS, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( ComboBox1, SIGNAL( activated( int ) ), this, SLOT( SelectionIntoArgument() ) );

  connect( myGeometryGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeometryGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeometryGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  
  // install event filter on spin-boxes to provide Apply action on Return pressed
  Group1Spin->SpinBox_DX->installEventFilter(this);
  Group2Spin->SpinBox_DX->installEventFilter(this);
  Group2Spin->SpinBox_DY->installEventFilter(this);
  Group3Spin->SpinBox_DX->installEventFilter(this);
  Group3Spin->SpinBox_DY->installEventFilter(this);
  Group3Spin->SpinBox_DZ->installEventFilter(this);
  Group4Spin->SpinBox_DX->installEventFilter(this);
  Group4Spin->SpinBox_DY->installEventFilter(this);
  Group4Spin->SpinBox_DZ->installEventFilter(this);
  Group4Spin->SpinBox_DS->installEventFilter(this);

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
// function : eventFilter()
// purpose  : event filter for spin-boxes to provide Apply action on Return pressed
//=================================================================================
bool EntityGUI_SketcherDlg::eventFilter (QObject* object, QEvent* event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* ke = (QKeyEvent*)event;
    if (ke->key() == Qt::Key_Return) {
      if (object == Group1Spin->SpinBox_DX) {
        Group1Spin->buttonApply->animateClick();
        return true;
      } else if (object == Group2Spin->SpinBox_DX ||
                 object == Group2Spin->SpinBox_DY) {
        Group2Spin->buttonApply->animateClick();
        return true;
      } else if (object == Group3Spin->SpinBox_DX ||
                 object == Group3Spin->SpinBox_DY ||
                 object == Group3Spin->SpinBox_DZ) {
        Group3Spin->buttonApply->animateClick();
        return true;
      } else if (object == Group4Spin->SpinBox_DX ||
                 object == Group4Spin->SpinBox_DY ||
                 object == Group4Spin->SpinBox_DZ ||
                 object == Group4Spin->SpinBox_DS) {
        Group4Spin->buttonApply->animateClick();
        return true;
      }
    }
  }

  // Fix of the NPAL16010 bug is removed, because it's not actual with the Qt-4.x

  return QDialog::eventFilter(object, event);
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

  myHelpFileName = "create_sketcher_page.html";

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  /* min, max, step and decimals for spin boxes */
  initSpinBox( Group1Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group2Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( Group4Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
  // san: Note specific step values below!
  initSpinBox( Group4Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
  initSpinBox( Group4Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
  initSpinBox( Group4Spin->SpinBox_DS, COORD_MIN, COORD_MAX, 5., "length_precision" );

  /* displays Dialog */
  MainWidget->GroupConstructors->setEnabled( false );
  MainWidget->GroupDest1->setEnabled( false );
  setEnabledUndo( false );
  setEnabledRedo( false );

  MainWidget->RadioButton1->setChecked( true );

  TypeClicked( 0 );

  FindLocalCS();
  resize(100,100);


  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}


//=================================================================================
// function : InitClick()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::InitClick()
{
  disconnect( myGeometryGUI->getApp()->selectionMgr(), 0, this, 0 );

  Group1Sel->hide();
  Group1Spin->hide();
  Group2Spin->hide();
  Group3Spin->hide();
  Group4Spin->hide();
}


//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::TypeClicked( int constructorId )
{
  myConstructorId = constructorId;
  if ( myConstructorId == 0 ) {    // SEGMENT
    GroupD2->setEnabled( true );
    MainWidget->RB_Dest1->setEnabled( true );
    MainWidget->RB_Dest1->setChecked( true );
    DestClicked( 1 );
  }
  else if (  myConstructorId == 1 ) { // ARC
    GroupD2->setEnabled( false );
    MainWidget->RB_Dest1->setEnabled( false );
    MainWidget->RB_Dest2->setChecked( true );
    DestClicked( 0 );
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

  if ( constructorId == 1 ) {
    // Point
    GroupPt->RB_Point1->setChecked( true );
    GroupPt->show();
    PointClicked( 1 );  // XY
  }
  else if (  constructorId == 0 ) {
    // Direction
    GroupD1->RB_Dir11->setChecked( true );
    GroupD1->show();
    GroupD2->show();
    Dir1Clicked( 2 );  // Angle
  }
}


//=================================================================================
// function : PointClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::PointClicked( int constructorId )
{
  InitClick();

  // Get setting of step value from file configuration
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  if ( myConstructorId == 0 ) {  // SEGMENT
    if ( constructorId == 1 ) {  // XY
      mySketchType = PT_ABS;
      initSpinBox( Group2Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group2Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
      Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y2" ) );
      myX = 0.0;
      Group2Spin->SpinBox_DX->setValue( myX );
      myY = 0.0;
      Group2Spin->SpinBox_DY->setValue( myY );
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
    }
    else if ( constructorId == 0 ) {  // DXDY
      mySketchType = PT_RELATIVE;
      initSpinBox( Group2Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group2Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_DX2" ) );
      Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_DY2" ) );
      myDX = 0.0;
      Group2Spin->SpinBox_DX->setValue( myDX );
      myDY = 0.0;
      Group2Spin->SpinBox_DY->setValue( myDY );
      Group2Spin->show();
      Group2Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
    }
    else if ( constructorId == 2 ) {  // Selection
      mySketchType = PT_SEL;
      myEditCurrentArgument = Group1Sel->LineEdit1;
      connect( myGeometryGUI->getApp()->selectionMgr(),
               SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
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
void EntityGUI_SketcherDlg::Dir1Clicked( int constructorId )
{
  myConstructorDirId = constructorId;
  GroupD2->RB_Dir21->setChecked( true );
  Dir2Clicked( 2 );
}


//=================================================================================
// function : Dir2Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir2Clicked( int constructorId )
{
  InitClick();
  myAngle = 0.0;

  // Get setting of step value from file configuration
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  if ( myConstructorId == 0 ) {  // SEGMENT
    myX = 0.0;
    myY = 0.0;
    myLength = 100.0;
    if ( myConstructorDirId == 2 ) {  // Angle
      initSpinBox( Group2Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 5., "length_precision" );
      Group2Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_ANGLE2" ) );
      Group2Spin->SpinBox_DX->setValue( myAngle );
      Group2Spin->buttonApply->setFocus();
      Group2Spin->show();

      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_ANGLE_LENGTH;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_LENGTH2" ) );
        Group2Spin->SpinBox_DY->setValue( myLength );
      }
      else if ( constructorId == 0 ) {  // X
        mySketchType = DIR_ANGLE_X;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "angle_precision" );
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_X3" ) );
        Group2Spin->SpinBox_DY->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_ANGLE_Y;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "angle_precision" );        
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y3" ) );
        Group2Spin->SpinBox_DY->setValue( myY );
      }
    }
    else if ( myConstructorDirId == 0 ) {  // Perpendicular
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_PER_LENGTH;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_LENGTH2" ) );
        Group1Spin->SpinBox_DX->setValue( myLength );
      }
      else if ( constructorId == 0 ) {  // X
        mySketchType = DIR_PER_X;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X3" ) );
        Group1Spin->SpinBox_DX->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_PER_Y;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_Y3" ) );
        Group1Spin->SpinBox_DX->setValue( myY );
      }
    }
    else if ( myConstructorDirId == 1 ) {  // Tangent
      Group1Spin->show();
      Group1Spin->buttonApply->setFocus();

      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_TAN_LENGTH;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_LENGTH2" ) );
        Group1Spin->SpinBox_DX->setValue( myLength );
      }
      else if ( constructorId == 0 ) {  // X
        mySketchType = DIR_TAN_X;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X3" ) );
        Group1Spin->SpinBox_DX->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_TAN_Y;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_Y3" ) );
        Group1Spin->SpinBox_DX->setValue( myY );
      }
    }
    else if ( myConstructorDirId == 3 ) {  // DXDY
      initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_VX2" ) );
      Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_VY2" ) );
      myDX = 0.0;
      Group3Spin->SpinBox_DX->setValue( myDX );
      myDY = 0.0;
      Group3Spin->SpinBox_DY->setValue( myDY );
      Group3Spin->show();
      Group3Spin->buttonApply->setFocus();

      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_DXDY_LENGTH;
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_LENGTH2" ) );
        Group3Spin->SpinBox_DZ->setValue( myLength );
      }
      else if ( constructorId == 0 ) { // X
        mySketchType = DIR_DXDY_X;
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_X3" ) );
        Group3Spin->SpinBox_DZ->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_DXDY_Y;
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_Y3" ) );
        Group3Spin->SpinBox_DZ->setValue( myY );
      }
    }
  }
  else if ( myConstructorId == 1 ) {  // ARC
    if ( myConstructorDirId == 2 ) {  // Angle
      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_ANGLE_LENGTH;
        initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 5., "angle_precision" );
        initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
        initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, 5., "angle_precision" );
        Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_ANGLE2" ) );
        Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_RADIUS2" ) );
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_ANGLE2" ));
        Group3Spin->SpinBox_DX->setValue( myAngle );
        myRadius = 100.0;
        Group3Spin->SpinBox_DY->setValue( myRadius );
        myLength = 30.0;
        Group3Spin->SpinBox_DZ->setValue( myLength );
        Group3Spin->show();
        Group3Spin->buttonApply->setFocus();
      }
    }
    else if ( myConstructorDirId == 0 ) {  // Perpendicular
      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_PER_LENGTH;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 5., "angle_precision" );
        Group2Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_RADIUS2" ) );
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_ANGLE2" ) );
        myRadius = 100.0;
        Group2Spin->SpinBox_DX->setValue( myRadius );
        myLength = 30.0;
        Group2Spin->SpinBox_DY->setValue( myLength );
        Group2Spin->show();
        Group2Spin->buttonApply->setFocus();
      }
    }
    else if ( myConstructorDirId == 1 ) {  // Tangent
      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_TAN_LENGTH;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 5., "angle_precision" );
        Group2Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_RADIUS2" ) );
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_ANGLE2" ) );
        myRadius = 100.0;
        Group2Spin->SpinBox_DX->setValue( myRadius );
        myLength = 30.0;
        Group2Spin->SpinBox_DY->setValue( myLength );
        Group2Spin->show();
        Group2Spin->buttonApply->setFocus();
      }
    }
    else if ( myConstructorDirId == 3 ) {  // DXDY
      if ( constructorId == 2 ) {  // Length
        mySketchType = DIR_DXDY_LENGTH;
        Group4Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_VX2" ) );
        Group4Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_VY2" ) );
        Group4Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_RADIUS2" ) );
        Group4Spin->TextLabel4->setText( tr( "GEOM_SKETCHER_ANGLE2" ) );
        myDX = 0.0;
        Group4Spin->SpinBox_DX->setValue( myDX );
        myDY = 0.0;
        Group4Spin->SpinBox_DY->setValue( myDY );
        myRadius = 100.0;
        Group4Spin->SpinBox_DZ->setValue( myRadius );
        myLength = 30.0;
        Group4Spin->SpinBox_DS->setValue( myLength );
        Group4Spin->show();
        Group4Spin->buttonApply->setFocus();
      }
    }
  }

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
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
  if ( sender() == MainWidget->buttonClose ) {
    // Verify validity of commands
    if ( myCommand.count() <= 2 ) {
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr( "GEOM_ERROR_STATUS" ), tr( "CANNOT_CLOSE" ), tr( "BUT_OK" ) );
      return;
    }

    QString Parameters;
    QString Command = myCommand.join( "" ) + GetNewCommand( Parameters );
    Sketcher_Profile aProfile( Command.toAscii() );

    Command = myCommand.join( "" );
    aProfile = Sketcher_Profile( Command.toAscii() );
    TopoDS_Shape myShape;
    if ( aProfile.IsDone() )
      myShape = aProfile.GetShape();

    if ( myShape.ShapeType() != TopAbs_VERTEX )
      myCommand.append( ":WW" );
  }
  else {
    // PAL16008 (Sketcher Validation should be equal to Apply&Close)
    if ( Group1Spin->buttonApply->isEnabled() && Group1Spin->isVisible() ||
         Group2Spin->buttonApply->isEnabled() && Group2Spin->isVisible() ||
         Group3Spin->buttonApply->isEnabled() && Group3Spin->isVisible() ||
         Group4Spin->buttonApply->isEnabled() && Group4Spin->isVisible() ||
         Group1Sel ->buttonApply->isEnabled() && Group1Sel->isVisible() ) {
      ClickOnApply();
    }
    myIsAllAdded = true;
  }

  if ( myCommand.size() > 2 )
    if ( !onAccept() )
      return;

  close();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::ClickOnApply()
{
  if ( sender() && sender()->inherits( "QPushButton" ) )
    ( (QPushButton*)sender() )->setFocus(); // to update value of currently edited spin-box (PAL11948)

  QString aParameters;
  myCommand.append( GetNewCommand( aParameters ) );
  mySketchState = NEXT_POINT;

  myUndoCommand.clear();
  myUndoCommand.append( "Sketcher" );

  myParameters.append( aParameters );
  myUndoParameters.clear();

  MainWidget->GroupConstructors->setEnabled( true );
  MainWidget->GroupDest1->setEnabled( true );
  setEnabledUndo( true );
  setEnabledRedo( false );

  myIsApply = true;
  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
  myIsApply = false;

  // Set focus to SpinBox_DX
  if ( sender() == Group1Spin->buttonApply ) {
    (Group1Spin->SpinBox_DX)->setFocus();
    (Group1Spin->SpinBox_DX)->selectAll();
  }
  else if ( sender() == Group2Spin->buttonApply ) {
    (Group2Spin->SpinBox_DX)->setFocus();
    (Group2Spin->SpinBox_DX)->selectAll();
  }
  else if ( sender() == Group3Spin->buttonApply ) {
    (Group3Spin->SpinBox_DX)->setFocus();
    (Group3Spin->SpinBox_DX)->selectAll();
  }
  else if ( sender() == Group4Spin->buttonApply ) {
    (Group4Spin->SpinBox_DX)->setFocus();
    (Group4Spin->SpinBox_DX)->selectAll();
  }

  return true;
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( app )
    app->onHelpContextModule( myGeometryGUI ? app->moduleName( myGeometryGUI->moduleName() ) : QString( "" ), myHelpFileName );
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif

    SUIT_MessageBox::warning( 0, QObject::tr( "WRN_WARNING" ),
                              QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", platform ) ).arg( myHelpFileName ),
                              QObject::tr( "BUT_OK" ) );
  }
}

//=================================================================================
// function : ClickOnUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnUndo()
{
  myUndoCommand.append( myCommand.last() );
  myCommand.pop_back();

  myUndoParameters.append( myParameters.last() );
  myParameters.pop_back();

  if ( myCommand.count() == 1 ) {
    mySketchState = FIRST_POINT;

    MainWidget->RadioButton1->setChecked( true );
    TypeClicked( 0 );

    MainWidget->GroupConstructors->setEnabled( false );
    MainWidget->GroupDest1->setEnabled( false );
    setEnabledUndo( false );
  }

  setEnabledRedo( true );

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : ClickOnRedo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ClickOnRedo()
{
  myCommand.append( myUndoCommand.last() );
  myUndoCommand.pop_back();

  myParameters.append( myUndoParameters.last() );
  myUndoParameters.pop_back();

  mySketchState = NEXT_POINT;

  MainWidget->GroupConstructors->setEnabled( true );
  MainWidget->GroupDest1->setEnabled( true );
  setEnabledUndo( true );

  if ( myUndoCommand.count() == 1 )
    setEnabledRedo( false );

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}

//=================================================================================
// function : setEnabledUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::setEnabledUndo( bool value )
{
  Group1Sel->buttonUndo->setEnabled( value );
  Group1Spin->buttonUndo->setEnabled( value );
  Group2Spin->buttonUndo->setEnabled( value );
  Group3Spin->buttonUndo->setEnabled( value );
  Group4Spin->buttonUndo->setEnabled( value );
}

//=================================================================================
// function : setEnabledRedo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::setEnabledRedo( bool value )
{
  Group1Sel->buttonRedo->setEnabled( value );
  Group1Spin->buttonRedo->setEnabled( value );
  Group2Spin->buttonRedo->setEnabled( value );
  Group3Spin->buttonRedo->setEnabled( value );
  Group4Spin->buttonRedo->setEnabled( value );
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void EntityGUI_SketcherDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  myX = myLastX1;
  myY = myLastY1;

  LightApp_SelectionMgr* aSelMgr = myGeometryGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = aSelList.Extent();
  if (nbSel == 1 && myEditCurrentArgument == Group1Sel->LineEdit1) {
    Standard_Boolean aRes = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aRes);
    if (!CORBA::is_nil(aSelectedObject) && aRes) {
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_VERTEX)) {
        gp_Trsf aTrans;
        gp_Ax3 aWPlane = GetActiveLocalCS();

        aTrans.SetTransformation(aWPlane);
        BRepBuilderAPI_Transform aTransformation (aShape, aTrans, Standard_False);
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

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::SetEditCurrentArgument()
{
  if ( sender() == Group1Sel->PushButton1 ) {
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
  if ( sender() == Group1Sel->LineEdit1 ) {
    myEditCurrentArgument = Group1Sel->LineEdit1;

    /* User name of object input management                          */
    /* If successfull the selection is changed and signal emitted... */
    /* so SelectionIntoArgument() is automatically called.           */
    const QString objectUserName = myEditCurrentArgument->text();
    QWidget* thisWidget = (QWidget*)this;

    LightApp_SelectionMgr* aSelMgr = myGeometryGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    if (GEOMBase::SelectionByNameInDialogs(thisWidget, objectUserName, aSelList))
      myEditCurrentArgument->setText( objectUserName );
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
  disconnect( myGeometryGUI->getApp()->selectionMgr(), 0, this, 0 );
  myGeometryGUI->SetActiveDialogBox( 0 );
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ActivateThisDialog()
{
  myGeometryGUI->EmitSignalDeactivateDialog();
  setEnabled( true );
  myGeometryGUI->SetActiveDialogBox( this );

  connect( myGeometryGUI->getApp()->selectionMgr(),
          SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  //myGeometryGUI->SetState( 0 );
  globalSelection( GEOM_POINT );

  myEditCurrentArgument = Group1Sel->LineEdit1;
  myEditCurrentArgument->setFocus();

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::enterEvent( QEvent* )
{
  if ( !MainWidget->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::closeEvent( QCloseEvent* e )
{
  //myGeometryGUI->SetState( -1 );
  disconnect( myGeometryGUI->getApp()->selectionMgr(), 0, this, 0 );
  myGeometryGUI->getApp()->updateActions();
  QDialog::closeEvent( e );
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();
  Standard_Real vx, vy, vz, vs;
  vx = vy = vz = vs = 0.0;

  QString vxStr, vyStr, vzStr, vsStr;

  QString newValueStr;
  if( SalomeApp_DoubleSpinBox* aDoubleSpinBox = (SalomeApp_DoubleSpinBox*)send )
    newValueStr = aDoubleSpinBox->text();

  if ( send == Group1Spin->SpinBox_DX ) {
    vx = newValue;                        vxStr = newValueStr;
  }
  else if ( send == Group2Spin->SpinBox_DX ) {
    vx = newValue;                        vxStr = newValueStr;
    vy = Group2Spin->SpinBox_DY->value(); vyStr = Group2Spin->SpinBox_DY->text();
  }
  else if ( send == Group2Spin->SpinBox_DY ) {
    vx = Group2Spin->SpinBox_DX->value(); vxStr = Group2Spin->SpinBox_DX->text();
    vy = newValue;                        vyStr = newValueStr;
  }
  else if ( send == Group3Spin->SpinBox_DX ) {
    vx = newValue;                        vxStr = newValueStr;
    vy = Group3Spin->SpinBox_DY->value(); vyStr = Group3Spin->SpinBox_DY->text();
    vz = Group3Spin->SpinBox_DZ->value(); vzStr = Group3Spin->SpinBox_DZ->text();
  }
  else if ( send == Group3Spin->SpinBox_DY ) {
    vx = Group3Spin->SpinBox_DX->value(); vxStr = Group3Spin->SpinBox_DX->text();
    vy = newValue;                        vyStr = newValueStr;
    vz = Group3Spin->SpinBox_DZ->value(); vzStr = Group3Spin->SpinBox_DZ->text();
  }
  else if ( send == Group3Spin->SpinBox_DZ ) {
    vx = Group3Spin->SpinBox_DX->value(); vxStr = Group3Spin->SpinBox_DX->text();
    vy = Group3Spin->SpinBox_DY->value(); vyStr = Group3Spin->SpinBox_DY->text();
    vz = newValue;                        vzStr = newValueStr;
  }
  else if ( send == Group4Spin->SpinBox_DX ) {
    vx = newValue;                        vxStr = newValueStr;
    vy = Group4Spin->SpinBox_DY->value(); vyStr = Group4Spin->SpinBox_DY->text();
    vz = Group4Spin->SpinBox_DZ->value(); vzStr = Group4Spin->SpinBox_DZ->text();
    vs = Group4Spin->SpinBox_DS->value(); vsStr = Group4Spin->SpinBox_DS->text();
  }
  else if ( send == Group4Spin->SpinBox_DY ) {
    vx = Group4Spin->SpinBox_DX->value(); vxStr = Group4Spin->SpinBox_DX->text();
    vy = newValue;                        vyStr = newValueStr;
    vz = Group4Spin->SpinBox_DZ->value(); vzStr = Group4Spin->SpinBox_DZ->text();
    vs = Group4Spin->SpinBox_DS->value(); vsStr = Group4Spin->SpinBox_DS->text();
  }
  else if ( send == Group4Spin->SpinBox_DZ ) {
    vx = Group4Spin->SpinBox_DX->value(); vxStr = Group4Spin->SpinBox_DX->text();
    vy = Group4Spin->SpinBox_DY->value(); vyStr = Group4Spin->SpinBox_DY->text();
    vz = newValue;                        vzStr = newValueStr;
    vs = Group4Spin->SpinBox_DS->value(); vsStr = Group4Spin->SpinBox_DS->text();
  }
  else if ( send == Group4Spin->SpinBox_DS ) {
    vx = Group4Spin->SpinBox_DX->value(); vxStr = Group4Spin->SpinBox_DX->text();
    vy = Group4Spin->SpinBox_DY->value(); vyStr = Group4Spin->SpinBox_DY->text();
    vz = Group4Spin->SpinBox_DZ->value(); vzStr = Group4Spin->SpinBox_DZ->text();
    vs = newValue;                        vsStr = newValueStr;
  }
  // Fix of the NPAL16010 bug is removed, because it's not actual with the Qt-4.x

  if ( myConstructorId == 0 ) {  // SEGMENT
    if ( mySketchType == PT_ABS ) {
      myX = vx;
      myY = vy;
      myXStr = vxStr;
      myYStr = vyStr;
    }
    else if ( mySketchType == PT_RELATIVE ) {
      myDX = vx;
      myDY = vy;
      myDXStr = vxStr;
      myDYStr = vyStr;
    }
    else if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myAngle = vx;
      myLength = vy;
      myAngleStr = vxStr;
      myLengthStr = vyStr;
    }
    else if ( mySketchType == DIR_ANGLE_X ) {
      myAngle = vx;
      myX = vy;
      myAngleStr = vxStr;
      myXStr = vyStr;
    }
    else if ( mySketchType == DIR_ANGLE_Y ) {
      myAngle = vx;
      myY = vy;
      myAngleStr = vxStr;
      myYStr = vyStr;
    }
    else if ( mySketchType == DIR_PER_LENGTH ) {
      myLength = vx;
      myLengthStr = vxStr;
    }
    else if ( mySketchType == DIR_PER_X ) {
      myX = vx;
      myXStr = vxStr;
    }
    else if ( mySketchType == DIR_PER_Y ) {
      myY = vx;
      myYStr = vxStr;
    }
    else if ( mySketchType == DIR_TAN_LENGTH ) {
      myLength = vx;
      myLengthStr = vxStr;
    }
    else if ( mySketchType == DIR_TAN_X ) {
      myX = vx;
      myXStr = vxStr;
    }
    else if ( mySketchType == DIR_TAN_Y ) {
      myY = vx;
      myYStr = vxStr;
    }
    else if ( mySketchType == DIR_DXDY_LENGTH ) {
      myDX = vx;
      myDY = vy;
      myLength = vz;
      myDXStr = vxStr;
      myDYStr = vyStr;
      myLengthStr = vzStr;
    }
    else if ( mySketchType == DIR_DXDY_X ) {
      myDX = vx;
      myDY = vy;
      myX = vz;
      myDXStr = vxStr;
      myDYStr = vyStr;
      myXStr = vzStr;
    }
    else if ( mySketchType == DIR_DXDY_Y ) {
      myDX = vx;
      myDY = vy;
      myY = vz;
      myDXStr = vxStr;
      myDYStr = vyStr;
      myYStr = vzStr;
    }
  }
  else if ( myConstructorId == 1 ) {  // ARC
    if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myAngle = vx;
      myRadius = vy;
      myLength = vz;
      myAngleStr = vxStr;
      myRadiusStr = vyStr;
      myLengthStr = vzStr;
    }
    else if ( mySketchType == DIR_PER_LENGTH ) {
      myRadius = vx;
      myLength = vy;
      myRadiusStr = vxStr;
      myLengthStr = vyStr;
    }
    else if ( mySketchType == DIR_TAN_LENGTH ) {
      myRadius = vx;
      myLength = vy;
      myRadiusStr = vxStr;
      myLengthStr = vyStr;
    }
    else if ( mySketchType == DIR_DXDY_LENGTH ) {
      myDX = vx;
      myDY = vy;
      myRadius = vz;
      myLength = vs;
      myDXStr = vxStr;
      myDYStr = vyStr;
      myRadiusStr = vzStr;
      myLengthStr = vsStr;
    }
  }

  GEOMBase_Helper::displayPreview( false, true, true, myLineWidth );
}


//=================================================================================
// function : GetNewCommand()
// purpose  : Build the new command with context
//=================================================================================
QString EntityGUI_SketcherDlg::GetNewCommand( QString& theParameters )
{
  theParameters.clear();
  QString myNewCommand = ":";
  if ( mySketchState == FIRST_POINT ) {
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL ) {
      myNewCommand = myNewCommand + "F " + QString::number( myX ) + " " + QString::number( myY );
      theParameters = myXStr + ":" + myYStr;
    }
    if ( mySketchType == PT_RELATIVE) {
      myNewCommand = myNewCommand + "F " + QString::number( myDX ) + " " + QString::number( myDY );
      theParameters = myDXStr + ":" + myDYStr;
    }
    return myNewCommand;
  }

  if ( myConstructorId == 0  ) {  // SEGMENT
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL ) {
      myNewCommand = myNewCommand + "TT " + QString::number( myX ) + " " + QString::number( myY );
      theParameters = myXStr + ":" + myYStr;
    }
    if ( mySketchType == PT_RELATIVE) {
      myNewCommand = myNewCommand + "T " + QString::number( myDX ) + " " + QString::number( myDY );
      theParameters = myDXStr + ":" + myDYStr;
    }
    if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength );
      theParameters = myAngleStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_ANGLE_X ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX );
      theParameters = myAngleStr + ":" + myXStr;
    }
    if ( mySketchType == DIR_ANGLE_Y ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY );
      theParameters = myAngleStr + ":" + myYStr;
    }
    if ( mySketchType == DIR_PER_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength );
      theParameters = QString::number( 90.0 ) + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_PER_X ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX );
      theParameters = QString::number( 90.0 ) + ":" + myXStr;
    }
    if ( mySketchType == DIR_PER_Y ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY );
      theParameters = QString::number( 90.0 ) + ":" + myYStr;
    }
    if ( mySketchType == DIR_TAN_LENGTH ) {
      myNewCommand = myNewCommand + "L " + QString::number( myLength );
      theParameters = myLengthStr;
    }
    if ( mySketchType == DIR_TAN_X ) {
      myNewCommand = myNewCommand + "IX " + QString::number( myX );
      theParameters = myXStr;
    }
    if ( mySketchType == DIR_TAN_Y) {
      myNewCommand = myNewCommand + "IY " + QString::number(myY);
      theParameters = myYStr;
    }
    if ( mySketchType == DIR_DXDY_LENGTH ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX ) + " " + QString::number( myDY );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength );
      theParameters = myDXStr + ":" + myDYStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_DXDY_X ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX ) + " " + QString::number( myDY );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX );
      theParameters = myDXStr + ":" + myDYStr + ":" + myXStr;
    }
    if ( mySketchType == DIR_DXDY_Y ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX ) + " " + QString::number( myDY );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY );
      theParameters = myDXStr + ":" + myDYStr + ":" + myYStr;
    }
  }
  else if ( myConstructorId == 1 ) {  // ARC
    if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius ) + " " + QString::number( myLength );
      theParameters = myAngleStr + ":" + myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_PER_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius ) + " " + QString::number( myLength );
      theParameters = QString::number( 90.0 ) + ":" + myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_TAN_LENGTH ) {
      myNewCommand = myNewCommand + "C " + QString::number( myRadius ) + " " + QString::number( myLength );
      theParameters = myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_DXDY_LENGTH ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX ) + " " + QString::number( myDY );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius ) + " " + QString::number( myLength );
      theParameters = myDXStr + ":" + myDYStr + ":" + myRadiusStr + ":" + myLengthStr;
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
  bool ok = true;
  bool toCorrect = !IsPreview() || myIsApply;

  if( Group1Spin->isVisible() ) {
    ok = Group1Spin->SpinBox_DX->isValid( msg, toCorrect ) && ok;
  }
  else if( Group2Spin->isVisible() ) {
    ok = Group2Spin->SpinBox_DX->isValid( msg, toCorrect ) && ok;
    ok = Group2Spin->SpinBox_DY->isValid( msg, toCorrect ) && ok;
  }
  else if( Group3Spin->isVisible() ) {
    ok = Group3Spin->SpinBox_DX->isValid( msg, toCorrect ) && ok;
    ok = Group3Spin->SpinBox_DY->isValid( msg, toCorrect ) && ok;
    ok = Group3Spin->SpinBox_DZ->isValid( msg, toCorrect ) && ok;
  }
  else if( Group4Spin->isVisible() ) {
    ok = Group4Spin->SpinBox_DX->isValid( msg, toCorrect ) && ok;
    ok = Group4Spin->SpinBox_DY->isValid( msg, toCorrect ) && ok;
    ok = Group4Spin->SpinBox_DZ->isValid( msg, toCorrect ) && ok;
    ok = Group4Spin->SpinBox_DS->isValid( msg, toCorrect ) && ok;
  }

  if( myIsApply && !ok )
    showError( msg );

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::execute( ObjectList& objects )
{
  QString aParameters;

  if ( mySketchState == FIRST_POINT ) {
    myLastX2 = myX;
    myLastY2 = myY;
  }
  else {
    //Test if the current point is the same as the last one
    TopoDS_Shape myShape1, myShape2;

    //Last Shape
    QString Command1 = myCommand.join( "" );
    Sketcher_Profile aProfile1( Command1.toAscii() );
    if ( aProfile1.IsDone() )
      myShape1 = aProfile1.GetShape();

    //Current Shape
    QString Command2 = Command1 + GetNewCommand( aParameters );
    Sketcher_Profile aProfile2( Command2.toAscii() );
    if ( aProfile2.IsDone() )
      myShape2 = aProfile2.GetShape();

    if ( myShape2.IsNull() ) {
      //the current point is the same as the last one
      myLastX2 = myLastX1;
      myLastY2 = myLastY1;
    }
    else {
      TopoDS_Vertex V1, V2;
      gp_Pnt pt;
      if ( myShape1.ShapeType() == TopAbs_VERTEX ) {
        //the last shape is the first point
        pt = BRep_Tool::Pnt( TopoDS::Vertex( myShape1 ) );
        myLastX1 = pt.X();
        myLastY1 = pt.Y();
      }
      else {
        TopExp::Vertices( TopoDS::Wire( myShape1 ), V1, V2 );
        pt = BRep_Tool::Pnt( V2 );
        myLastX1 = pt.X();
        myLastY1 = pt.Y();
      }
      TopExp::Vertices( TopoDS::Wire( myShape2 ), V1, V2 );
      pt = BRep_Tool::Pnt( V2 );
      myLastX2 = pt.X();
      myLastY2 = pt.Y();
    }
  }

  QString cmd;
  if ( ( mySketchState != FIRST_POINT &&
         myLastX1 == myLastX2 && myLastY1 == myLastY2 ) || myIsAllAdded ) {
    cmd = myCommand.join( "" );

    if ( Group1Sel->isVisible() ) {
      Group1Sel->buttonApply->setEnabled( false );
      //Group1Sel->buttonApply->setFocus();
    }
    if ( Group1Spin->isVisible() ) {
      Group1Spin->buttonApply->setEnabled( false );
      //Group1Spin->buttonApply->setFocus();
    }
    if ( Group2Spin->isVisible() ) {
      Group2Spin->buttonApply->setEnabled( false );
      //Group2Spin->buttonApply->setFocus();
    }
    if ( Group3Spin->isVisible() ) {
      Group3Spin->buttonApply->setEnabled( false );
      //Group3Spin->buttonApply->setFocus();
    }
    if ( Group4Spin->isVisible() ) {
      Group4Spin->buttonApply->setEnabled( false );
      //Group4Spin->buttonApply->setFocus();
    }
  }
  else {
    cmd = myCommand.join( "" ) + GetNewCommand( aParameters );

    if ( Group1Sel->isVisible() ) {
      Group1Sel->buttonApply->setEnabled( true );
      //Group1Sel->buttonApply->setFocus();
    }
    if ( Group1Spin->isVisible() ) {
      Group1Spin->buttonApply->setEnabled( true );
      //Group1Spin->buttonApply->setFocus();
    }
    if ( Group2Spin->isVisible() ) {
      Group2Spin->buttonApply->setEnabled( true );
      //Group2Spin->buttonApply->setFocus();
    }
    if ( Group3Spin->isVisible() ) {
      Group3Spin->buttonApply->setEnabled( true );
      //Group3Spin->buttonApply->setFocus();
    }
    if ( Group4Spin->isVisible() ) {
      Group4Spin->buttonApply->setEnabled( true );
      //Group4Spin->buttonApply->setFocus();
    }
  }

  gp_Ax3 myWPlane = GetActiveLocalCS();
  GEOM::ListOfDouble_var WPlane = new GEOM::ListOfDouble;
  WPlane->length( 9 );
  WPlane[0] = myWPlane.Location().X();
  WPlane[1] = myWPlane.Location().Y();
  WPlane[2] = myWPlane.Location().Z();

  WPlane[3] = myWPlane.Direction().X();
  WPlane[4] = myWPlane.Direction().Y();
  WPlane[5] = myWPlane.Direction().Z();

  WPlane[6] = myWPlane.XDirection().X();
  WPlane[7] = myWPlane.XDirection().Y();
  WPlane[8] = myWPlane.XDirection().Z();

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->MakeSketcher( cmd.toLatin1().constData(), WPlane );

  if ( !anObj->_is_nil() )
  {
    if( !IsPreview() ) {
      QStringList aCurrentParameters = myParameters;
      aCurrentParameters << aParameters;
      anObj->SetParameters(aCurrentParameters.join(":").toLatin1().constData());
    }

    objects.push_back( anObj._retn() );
  }

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
                                            const double          lineWidth,
                                            const int             displayMode,
                                            const int             color )
{
  // Set color for preview shape
  getDisplayer()->SetColor( Quantity_NOC_RED );

  // set width of displayed shape
  getDisplayer()->SetWidth( (lineWidth == -1)?myLineWidth:lineWidth );

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  CORBA::String_var objStr = myGeometryGUI->getApp()->orb()->object_to_string( object );
  getDisplayer()->SetName( objStr.in() );

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
       Group4Spin->isVisible() && !Group4Spin->buttonApply->isEnabled() ) {
     theApplyedWire = aShape;
     return true;
  }

  BRepBuilderAPI_MakeWire aBuilder;
  TopExp_Explorer anExp( aShape, TopAbs_EDGE );
  while ( 1 ) {
    TopoDS_Shape anEdge = anExp.Current();
    anExp.Next();
    if ( anExp.More() ) // i.e. non-last edge
      aBuilder.Add( TopoDS::Edge( anEdge ) );
    else {
      theLastSegment = anEdge;
      break;
    }
  }

  if ( aBuilder.IsDone() )
    theApplyedWire = aBuilder.Shape();

  return true;
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void  EntityGUI_SketcherDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    ClickOnHelp();
  }
}

//=================================================================================
// function : initSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::initSpinBox( SalomeApp_DoubleSpinBox* spinBox,
                                         double min,  double max,
                                         double step, const char* quantity )
{
  // The same stuff as in GEOMBase_Skeleton::initSpinBox()!
  // TODO: Think how to keep the single piece of code...

  // Obtain precision from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", quantity, 6 );
  
  spinBox->setPrecision( aPrecision );
  spinBox->setDecimals( aPrecision ); // it's necessary to set decimals before the range setting,
                                    // by default Qt rounds boundaries to 2 decimals at setRange
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
  
  // Add a hint for the user saying how to tune precision
  QString userPropName = QObject::tr( QString( "PREF_%1" ).arg( quantity ).toLatin1().constData() );
  spinBox->setProperty( "validity_tune_hint", 
                        QVariant( QObject::tr( "PRECISION_HINT" ).arg( userPropName ) ) );  
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_SketcherDlg::SetDoubleSpinBoxStep( double step )
{
  Group1Spin->SpinBox_DX->setSingleStep(step);
  Group2Spin->SpinBox_DX->setSingleStep(step);
  Group2Spin->SpinBox_DY->setSingleStep(step);
  Group3Spin->SpinBox_DX->setSingleStep(step);
  Group3Spin->SpinBox_DY->setSingleStep(step);
  Group3Spin->SpinBox_DZ->setSingleStep(step);
  Group4Spin->SpinBox_DZ->setSingleStep(step);

  // san: Do NOT override the step when a speicifc step value is used
  // in some input fields!
  //Group4Spin->SpinBox_DX->setSingleStep(step);
  //Group4Spin->SpinBox_DY->setSingleStep(step);
  //Group4Spin->SpinBox_DS->setSingleStep(step);
}

//=================================================================================
// function : FindLocalCS()
// purpose  : Find All Coordinates systems in study
//=================================================================================
void EntityGUI_SketcherDlg::FindLocalCS()
{
  ComboBox1->clear();
  myLCSList.clear();
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy )
    return;

  _PTR(Study) aStudy = appStudy->studyDS();

  //add Global CS
  ComboBox1->addItem(tr("GEOM_GCS"));
  gp_Pnt aOrigin = gp_Pnt(0, 0, 0);
  gp_Dir aDirZ = gp_Dir(0, 0, 1);
  gp_Dir aDirX = gp_Dir(1, 0, 0);
  gp_Ax3 globalCS = gp_Ax3(aOrigin, aDirZ, aDirX);
  myLCSList.push_back(globalCS);

  // get GEOM component
  CORBA::String_var geomIOR = app->orb()->object_to_string( GeometryGUI::GetGeomGen() );
  _PTR(SObject) obj = aStudy->FindObjectIOR( geomIOR.in() );
  if (!obj)
    return;
  _PTR(SComponent) fc = obj->GetFatherComponent();
  QString geomComp = fc->GetID().c_str();
  _PTR(SObject) comp = aStudy->FindObjectID( geomComp.toLatin1().data() );
  if ( !comp )
    return;

  // browse through all GEOM data tree
  _PTR(ChildIterator) it ( aStudy->NewChildIterator( comp ) );
  for ( it->InitEx( true ); it->More(); it->Next() ) {
    _PTR(SObject) child( it->Value() );
    CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject( child );
    GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
    if( CORBA::is_nil( geomObj ) ) 
      continue;
    if (geomObj->GetType() == GEOM_MARKER) {
      ComboBox1->addItem(geomObj->GetName());
      TopoDS_Shape aShape = GEOM_Client().GetShape(GeometryGUI::GetGeomGen(), geomObj);
      
      gp_Ax3 aLCS;
      aLCS.Transform(aShape.Location().Transformation());
      if (aShape.ShapeType() == TopAbs_FACE) {
        Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(aShape));
        if (!aGS.IsNull() && aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
          Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast(aGS);
          gp_Pln aPln = aGPlane->Pln();
          aLCS = aPln.Position();
        }
      }
      myLCSList.push_back(aLCS);
    }
  }
}

//=================================================================================
// function : GetActiveLocalCS()
// purpose  : Find All Coordinates systems in study
//=================================================================================
gp_Ax3 EntityGUI_SketcherDlg::GetActiveLocalCS()
{
  int ind = ComboBox1->currentIndex();
  if (ind == -1)
    return myGeometryGUI->GetWorkingPlane();

  gp_Ax3 aLCS = myLCSList.at(ind);

  myGeometryGUI->SetWorkingPlane( aLCS );
  myGeometryGUI->ActiveWorkingPlane();
  return aLCS;
}
