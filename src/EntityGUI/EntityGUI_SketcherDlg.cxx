// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_SketcherDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "EntityGUI_SketcherDlg.h"
#include "EntityGUI_Widgets.h"
#include <SalomeApp_DoubleSpinBox.h>

#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewSketcher.h>

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>

#include <GeomLib_IsPlanarSurface.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include "utilities.h"

#include <DlgRef.h>

#include <QKeyEvent>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Sketcher_Profile.hxx>

#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include "utilities.h"

#include <gp_Pln.hxx>

#include <Basics_Utils.hxx>

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
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  setModal( modal );
  setAttribute( Qt::WA_DeleteOnClose );
  
  // Disable rectangular selection
//   SUIT_ViewWindow*      theViewWindow  = getDesktop()->activeWindow();
//   ((OCCViewer_ViewWindow*)theViewWindow)->setEnabledDrawMode( false );
//   MESSAGE("((OCCViewer_ViewWindow*)theViewWindow)->isEnabledDrawMode();"<<((OCCViewer_ViewWindow*)theViewWindow)->isEnabledDrawMode())
//   OCCViewer_ViewPort3d* vp             = ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
//   vp->setSketchingEnabled( false );
//   MESSAGE("vp->isSketchingEnabled()"<<vp->isSketchingEnabled())
//   ((OCCViewer_ViewWindow*)theViewWindow)->setSketcherStyle(true);
//   OCCViewer_ViewSketcher* aViewSketcher = ((OCCViewer_ViewWindow*)theViewWindow)->getSketcher( OCCViewer_ViewWindow::Rect );
//   aViewSketcher->deactivate()
//   aViewSketcher->setVisible(false);
  
//   this->setMouseTracking(false);
  
  myGeometryGUI->SetActiveDialogBox(this);

  MainWidget = new EntityGUI_Skeleton( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 9 ); topLayout->setSpacing( 6 );

  MainWidget->buttonCancel->setText( tr( "GEOM_BUT_CANCEL" ) );
  MainWidget->buttonEnd->setText( tr( "GEOM_BUT_CLOSE" ) );
  MainWidget->buttonClose->setText( tr( "GEOM_BUT_CLOSE_SKETCH" ) );
  MainWidget->buttonHelp->setText( tr( "GEOM_BUT_HELP" ) );

  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT"        ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_UNDO"      ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_REDO"      ) ) );
  QPixmap image3( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_LINE_2P"   ) ) );
  QPixmap image4( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_ARC"       ) ) );
  QPixmap image5 (aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT"        ) ) );
  QPixmap image6 (aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_RECTANGLE" ) ) );

  setWindowTitle( tr( "GEOM_SKETCHER_TITLE" ) );

  MainWidget->GroupConstructors->setTitle( tr( "GEOM_SKETCHER_EL" ) );
  MainWidget->RadioButton1->setText( "" );
  MainWidget->RadioButton1->setIcon( image3 );
  MainWidget->RadioButton2->setText( "" );
  MainWidget->RadioButton2->setIcon( image4 );
  MainWidget->RadioButton3->setText( "" );
  MainWidget->RadioButton3->setIcon( image6 );
  MainWidget->GroupDest->setTitle( tr( "GEOM_SKETCHER_DEST" ) );
  MainWidget->GroupDest1->setTitle( tr( "GEOM_SKETCHER_TYPE" ) );
  MainWidget->RB_Dest1->setText( tr( "GEOM_SKETCHER_POINT" ) );
  MainWidget->RB_Dest2->setText( tr( "GEOM_SKETCHER_DIR" ) );

  /***************************************************************/

  GroupBox1 = new QGroupBox(tr("GEOM_CS"), this);
  QGridLayout* planeLayout = new QGridLayout(GroupBox1);
  planeLayout->setSpacing(6);
  planeLayout->setMargin(11);

  ComboBox1 = new QComboBox(GroupBox1);
  planeLayout->addWidget(ComboBox1,0,0,1,2);

  planeButton = new QPushButton (GroupBox1);
  planeButton->setText( tr( "GEOM_SKETCHER_RESTORE" ) );
  planeLayout->addWidget(planeButton,0,2);
  
  selButton = new QPushButton (GroupBox1);
  selButton->setIcon(image5);
  planeLayout->addWidget(selButton,1,0);
  
  WPlaneLineEdit = new QLineEdit (GroupBox1);
  WPlaneLineEdit->setReadOnly(true);
  planeLayout->addWidget(WPlaneLineEdit,1,1,1,2);
  
  planeLayout->setColumnStretch(1,1);
  
  topLayout->addWidget(GroupBox1);
  topLayout->addWidget( MainWidget );
  topLayout->setStretch( 1, 1);

  GroupPt = new EntityGUI_Point( MainWidget->DestCnt );

  GroupPt->GroupPoint->setTitle( tr( "GEOM_SKETCHER_POINT" ) );
  GroupPt->RB_Point1->setText( tr( "GEOM_SKETCHER_ABS" ) );
  GroupPt->RB_Point2->setText( tr( "GEOM_SKETCHER_REL" ) );
  GroupPt->RB_Point3->setText( tr( "GEOM_SKETCHER_SEL" ) );

  GroupPt2 = new EntityGUI_Point( MainWidget->DestCnt );

  GroupPt2->GroupPoint->setTitle( tr( "GEOM_SKETCHER_ADD_PARAMS" ) ); 
  GroupPt2->RB_Point1->setText( tr( "GEOM_SKETCHER_NONE" ) );
  GroupPt2->RB_Point2->setText( tr( "GEOM_SKETCHER_RADIUS" ) );
  GroupPt2->RB_Point3->setText( tr( "GEOM_SKETCHER_CENTER" ) ); 

  GroupD1 = new EntityGUI_Dir1( MainWidget->DestCnt );

  GroupD1->GroupDir1->setTitle( tr( "GEOM_SKETCHER_DIR" ) );
  GroupD1->RB_Dir11->setText( tr( "GEOM_SKETCHER_ANGLE" ) );
  GroupD1->RB_Dir12->setText( tr( "GEOM_SKETCHER_PER" ) );
  GroupD1->RB_Dir13->setText( tr( "GEOM_SKETCHER_TAN" ) );
  GroupD1->RB_Dir14->setText( tr( "GEOM_SKETCHER_VXVY" ) );

  GroupD2 = new EntityGUI_Dir2( MainWidget->DestCnt );

  GroupD2->GroupDir2->setTitle( tr( "GEOM_SKETCHER_DIST" ) );
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

  Group2Sel = new EntityGUI_2Sel1Check( MainWidget->SkeletonCnt );
  Group2Sel->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group2Sel->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group2Sel->checkBox->setText( tr( "GEOM_REVERSE" ) );
  Group2Sel->PushButton1->setIcon( image0 );
  Group2Sel->PushButton2->setIcon( image0 );
  Group2Sel->buttonUndo->setIcon( image1 );
  Group2Sel->buttonRedo->setIcon( image2 );
  Group2Sel->LineEdit1->setReadOnly( true );
  Group2Sel->LineEdit2->setReadOnly( true );

  Group1Sel1Spin = new EntityGUI_1Sel1Spin1Check( MainWidget->SkeletonCnt );
  Group1Sel1Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group1Sel1Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group1Sel1Spin->checkBox->setText( tr( "GEOM_REVERSE" ) );
  Group1Sel1Spin->PushButton1->setIcon( image0 );
  Group1Sel1Spin->buttonUndo->setIcon( image1 );
  Group1Sel1Spin->buttonRedo->setIcon( image2 );
  Group1Sel1Spin->LineEdit1->setReadOnly( true );

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

  Group3Spin = new EntityGUI_3Spin1Check( MainWidget->SkeletonCnt );
  Group3Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group3Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group3Spin->checkBox->setText( tr( "GEOM_REVERSE" ) );
  Group3Spin->buttonUndo->setIcon( image1 );
  Group3Spin->buttonRedo->setIcon( image2 );

  Group4Spin = new EntityGUI_4Spin1Check( MainWidget->SkeletonCnt );
  Group4Spin->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  Group4Spin->buttonApply->setText( tr( "GEOM_SKETCHER_APPLY" ) );
  Group4Spin->checkBox->setText( tr( "GEOM_REVERSE" ) );
  Group4Spin->buttonUndo->setIcon( image1 );
  Group4Spin->buttonRedo->setIcon( image2 );
  
  GroupRect = new DlgRef_4Spin( MainWidget->SkeletonCnt );
  GroupRect->GroupBox1->setTitle( tr( "GEOM_SKETCHER_VALUES" ) );
  GroupRect->TextLabel1->setText("X1:");
  GroupRect->TextLabel2->setText("Y1:");
  GroupRect->TextLabel3->setText("X2:");
  GroupRect->TextLabel4->setText("Y2:");

  // Defines a palette for the error message on Group4Spin and Group2Sel 
  QPalette palette;
  QColor color("red");
  palette.setColor(Group4Spin->label->foregroundRole(), color);
  Group4Spin->label->setPalette(palette);
  palette.setColor(Group2Sel->label->foregroundRole(), color);
  Group2Sel->label->setPalette(palette);

  QGridLayout* DestCntLayout = new QGridLayout( MainWidget->DestCnt );
  DestCntLayout->setMargin( 0 ); DestCntLayout->setSpacing( 6 );
  DestCntLayout->addWidget( GroupPt, 0, 0 );
  DestCntLayout->addWidget( GroupPt2, 0, 1 );
  DestCntLayout->addWidget( GroupD1, 0, 0 );
  DestCntLayout->addWidget( GroupD2, 0, 1 );

  QVBoxLayout* SkeletonCntlayout = new QVBoxLayout( MainWidget->SkeletonCnt );
  SkeletonCntlayout->setMargin( 0 ); SkeletonCntlayout->setSpacing( 6 );
  SkeletonCntlayout->addWidget( Group1Sel,  0, 0 );
  SkeletonCntlayout->addWidget( Group2Sel,  0, 0 );
  SkeletonCntlayout->addWidget( Group1Sel1Spin,  0, 0 );
  SkeletonCntlayout->addWidget( Group1Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group2Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group3Spin, 0, 0 );
  SkeletonCntlayout->addWidget( Group4Spin, 0, 0 );
  SkeletonCntlayout->addWidget( GroupRect, 0, 0 );
  //SkeletonCntlayout->setStretch( 0, 1);
  /***************************************************************/

  QButtonGroup* ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( MainWidget->RB_Dest1, 1 );
  ButtonGroup->addButton( MainWidget->RB_Dest2, 0 );

  /* signals and slots connections */
  connect( MainWidget->buttonEnd,    SIGNAL( clicked() ),              this, SLOT( ClickOnEnd() ) );
  connect( MainWidget->buttonClose,  SIGNAL( clicked() ),              this, SLOT( ClickOnEnd() ) );
  connect( MainWidget->buttonCancel, SIGNAL( clicked() ),              this, SLOT( ClickOnCancel() ) );
  connect( MainWidget->buttonHelp,   SIGNAL( clicked() ),              this, SLOT( ClickOnHelp() ) );

  connect( Group1Sel->buttonApply,   SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group1Sel->buttonUndo,    SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group1Sel->buttonRedo,    SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) );
  connect( Group2Sel->buttonApply,   SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group2Sel->buttonUndo,    SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group2Sel->buttonRedo,    SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) );
  connect( Group1Sel1Spin->buttonApply,SIGNAL( clicked() ),            this, SLOT( ClickOnApply() ) );
  connect( Group1Sel1Spin->buttonUndo, SIGNAL( clicked() ),            this, SLOT( ClickOnUndo() ) );
  connect( Group1Sel1Spin->buttonRedo, SIGNAL( clicked() ),            this, SLOT( ClickOnRedo() ) );
  connect( Group1Spin->buttonApply,  SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group1Spin->buttonUndo,   SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group1Spin->buttonRedo,   SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) );
  connect( Group2Spin->buttonApply,  SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group2Spin->buttonUndo,   SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group2Spin->buttonRedo,   SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) );
  connect( Group3Spin->buttonApply,  SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group3Spin->buttonUndo,   SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group3Spin->buttonRedo,   SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) ) ;
  connect( Group4Spin->buttonApply,  SIGNAL( clicked() ),              this, SLOT( ClickOnApply() ) );
  connect( Group4Spin->buttonUndo,   SIGNAL( clicked() ),              this, SLOT( ClickOnUndo() ) );
  connect( Group4Spin->buttonRedo,   SIGNAL( clicked() ),              this, SLOT( ClickOnRedo() ) );

  connect( MainWidget->ButtonGroup,  SIGNAL( buttonClicked( int ) ),   this, SLOT( TypeClicked( int ) ) );
  connect( ButtonGroup,              SIGNAL( buttonClicked( int ) ),   this, SLOT( DestClicked( int ) ) );
  connect( GroupPt->ButtonGroup,     SIGNAL( buttonClicked( int ) ),   this, SLOT( PointClicked( int ) ) );
  connect( GroupPt2->ButtonGroup,    SIGNAL( buttonClicked( int ) ),   this, SLOT( Point2Clicked( int ) ) );
  connect( GroupD1->ButtonGroup,     SIGNAL( buttonClicked( int ) ),   this, SLOT( Dir1Clicked( int ) ) );
  connect( GroupD2->ButtonGroup,     SIGNAL( buttonClicked( int ) ),   this, SLOT( Dir2Clicked( int ) ));

  connect( Group1Sel->LineEdit1,     SIGNAL( returnPressed() ),        this, SLOT( LineEditReturnPressed() ) );
  connect( Group1Sel->PushButton1,   SIGNAL( clicked() ),              this, SLOT( SetEditCurrentArgument() ) );

  connect( Group2Sel->LineEdit1,     SIGNAL( returnPressed() ),        this, SLOT( LineEditReturnPressed() ) );
  connect( Group2Sel->LineEdit2,     SIGNAL( returnPressed() ),        this, SLOT( LineEditReturnPressed() ) );
  connect( Group2Sel->PushButton1,   SIGNAL( clicked() ),              this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2Sel->PushButton2,   SIGNAL( clicked() ),              this, SLOT( SetEditCurrentArgument() ) );

  connect( Group1Sel1Spin->LineEdit1,SIGNAL( returnPressed() ),        this, SLOT( LineEditReturnPressed() ) );
  connect( Group1Sel1Spin->PushButton1,SIGNAL( clicked() ),            this, SLOT( SetEditCurrentArgument() ) );
  connect( Group1Sel1Spin->SpinBox_DX,SIGNAL( valueChanged( double ) ),this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( Group1Spin->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group2Spin->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group2Spin->SpinBox_DY,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DY,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Spin->SpinBox_DZ,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DY,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DZ,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group4Spin->SpinBox_DS,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRect->SpinBox_DX1,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRect->SpinBox_DX2,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRect->SpinBox_DY1,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRect->SpinBox_DY2,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( Group3Spin->checkBox,     SIGNAL( stateChanged( int ) ),    this, SLOT( CheckBoxClicked( int ) ) );
  connect( Group4Spin->checkBox,     SIGNAL( stateChanged( int ) ),    this, SLOT( CheckBoxClicked( int ) ) );
  connect( Group2Sel->checkBox,      SIGNAL( stateChanged( int ) ),    this, SLOT( CheckBoxClicked( int ) ) );
  connect( Group1Sel1Spin->checkBox, SIGNAL( stateChanged( int ) ),    this, SLOT( CheckBoxClicked( int ) ) );

  connect( ComboBox1,                SIGNAL( activated( int ) ),       this, SLOT( ActivateLocalCS() ) );
  connect( planeButton,              SIGNAL( clicked() ),              this, SLOT( ActivateLocalCS() ) );
  connect( selButton,                SIGNAL( clicked() ),              this, SLOT( SetEditCurrentArgument() ) );

  connect( myGeometryGUI,            SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeometryGUI,            SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeometryGUI,            SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  
  // install event filter on spin-boxes to provide Apply action on Return pressed
  Group1Spin->SpinBox_DX->installEventFilter(this);
  Group1Sel1Spin->SpinBox_DX->installEventFilter(this);
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
//   SUIT_ViewWindow*      theViewWindow  = getDesktop()->activeWindow();
  
  myGeometryGUI->SetActiveDialogBox( 0 );
//   ((OCCViewer_ViewWindow*)theViewWindow)->setSketcherStyle(false);
}


//=================================================================================
// function : eventFilter()
// purpose  : event filter for spin-boxes to provide Apply action on Return pressed
//=================================================================================
bool EntityGUI_SketcherDlg::eventFilter (QObject* object, QEvent* event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* ke = (QKeyEvent*)event;
    if ( ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter ) {
      if (object == Group1Spin->SpinBox_DX) {
        Group1Spin->buttonApply->click();
        return true;
      } else if (object == Group1Sel1Spin->SpinBox_DX) {
        Group1Sel1Spin->buttonApply->click();
        return true;
      } else if (object == Group2Spin->SpinBox_DX ||
                 object == Group2Spin->SpinBox_DY) {
        Group2Spin->buttonApply->click();
        return true;
      } else if (object == Group3Spin->SpinBox_DX ||
                 object == Group3Spin->SpinBox_DY ||
                 object == Group3Spin->SpinBox_DZ) {
        Group3Spin->buttonApply->click();
        return true;
      } else if (object == Group4Spin->SpinBox_DX ||
                 object == Group4Spin->SpinBox_DY ||
                 object == Group4Spin->SpinBox_DZ ||
                 object == Group4Spin->SpinBox_DS) {
        Group4Spin->buttonApply->click();
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
  autoApply = false;
  
  myEditCurrentArgument = WPlaneLineEdit; // Initiate the parameters selection with the first WPlaneLineEdit
  
  myCommand.append( "Sketcher" );
  myUndoCommand.append( "Sketcher" );

  mySketchState = FIRST_POINT;
  globalSelection( GEOM_FACE );

  myCheckFlag = 0;

  myLastX1 = 0.0;
  myLastY1 = 0.0;
  myLastX2 = 0.0;
  myLastY2 = 0.0;

  myHelpFileName = "create_sketcher_page.html";
  
  GEOM::GEOM_IBasicOperations_var aBasicOp = getGeomEngine()->GetIBasicOperations( getStudyId() );
  myGlobalCS = aBasicOp->MakeMarker( 0,0,0,
                                     1,0,0,
                                     0,1,0 ); 
  myWPlane = myGlobalCS;
  myLCSList.push_back( WPlaneToLCS(myGlobalCS) );

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  /* min, max, step and decimals for spin boxes */
  initSpinBox( Group1Sel1Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
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
  
  initSpinBox( GroupRect->SpinBox_DX1, COORD_MIN, COORD_MAX, step, "length_precision" ); 
  initSpinBox( GroupRect->SpinBox_DY1, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupRect->SpinBox_DX2, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupRect->SpinBox_DY2, COORD_MIN, COORD_MAX, step, "length_precision" );
    
  /* displays Dialog */

//   MainWidget->RadioButton1->setEnabled( false );
//   MainWidget->RadioButton2->setEnabled( false );
//   MainWidget->RadioButton3->setEnabled( true );
//   MainWidget->GroupDest1->setEnabled( false );
  setEnabledUndo( false );
  setEnabledRedo( false );

  MainWidget->RadioButton1->setChecked( true );

  FindLocalCS();
  TypeClicked( 0 );
  // If a face has already been selected use it. Placed after FindLocalCS to avoid clearing the combobox
  // that should be filled with the possibly selected face
  
  OnFirstPoint();
  
  SelectionIntoArgument();     
  
  resize(100,100);
  
  setPrefix(tr("GEOM_SKETCH"));

  ActivateLocalCS();
  
  setIsWaitCursorEnabled( false );
  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
}


//=================================================================================
// function : InitClick()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::InitClick()
{
  MESSAGE("EntityGUI_SketcherDlg::InitClick()")
  disconnect( myGeometryGUI->getApp()->selectionMgr(), 0, this, 0 );
  myCheckFlag = 0;

  Group1Sel->hide();
  Group2Sel->hide();
  Group1Sel1Spin->hide();
  Group1Spin->hide();
  Group2Spin->hide();
  Group3Spin->hide();
  Group4Spin->hide();
  GroupRect->hide();
  globalSelection(); // close local selection to clear it
  localSelection( TopAbs_VERTEX );
}


//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::TypeClicked( int constructorId )
{
  myConstructorId = constructorId;
  MainWidget->buttonEnd->setText(tr("GEOM_BUT_CLOSE"));
  MainWidget->buttonClose->setEnabled(true);
  
  if ( myConstructorId == 0 ) {    // SEGMENT
    GroupPt2->RB_Point1->setChecked( true );
    GroupPt ->setEnabled( true );
    GroupPt2->setEnabled( false );
    GroupD1->setEnabled( true );
    GroupD2->setEnabled( true );
    MainWidget->RB_Dest1->setEnabled( true );
    MainWidget->RB_Dest1->setChecked( true );
    
    MainWidget->GroupDest ->setEnabled( true );
    MainWidget->GroupDest1->setEnabled( true );
    MainWidget->GroupDest ->show();
    MainWidget->GroupDest1->show();
    DestClicked( 1 );
    if (mySketchState == FIRST_POINT)
    {
      OnFirstPoint();
    }
  }
  else if (  myConstructorId == 1 ) { // ARC
    GroupPt ->setEnabled( true );
    GroupPt2->setEnabled( true );
    GroupD2->RB_Dir21->setChecked( true );
    GroupD1->setEnabled( true );
    GroupD2->setEnabled( false );
    MainWidget->RB_Dest1->setEnabled( true ); 
    MainWidget->RB_Dest1->setChecked( true );
    
    MainWidget->GroupDest ->setEnabled( true );
    MainWidget->GroupDest1->setEnabled( true );
    MainWidget->GroupDest ->show();
    MainWidget->GroupDest1->show();
    DestClicked( 1 );
    if (mySketchState == FIRST_POINT)
    {
      OnFirstPoint();
    }
  }
  else if (  myConstructorId == 2 ) { // RECTANGLE
    GroupPt ->setEnabled( false );
    GroupPt2->setEnabled( false );
    GroupD1 ->setEnabled( false );
    GroupD2 ->setEnabled( false );
    MainWidget->GroupDest ->setEnabled( false );
    MainWidget->GroupDest1->setEnabled( false );

    GroupPt ->hide();
    GroupPt2->hide();
    GroupD1 ->hide();
    GroupD2 ->hide();
    MainWidget->GroupDest ->hide();
    MainWidget->GroupDest1->hide();
    
    MainWidget->buttonEnd->setText(tr("Apply and Close"));
    MainWidget->buttonClose->setEnabled(false);
    
    GroupRect->setEnabled( true );
    MainWidget->RadioButton1->setEnabled( true );
    MainWidget->RadioButton2->setEnabled( true );
    
    RectClicked();
  }
}

//=================================================================================
// function : RectClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::RectClicked()
{
  InitClick();
  
  // Connect the selction manager (disconnected in InitClick())
  connect( myGeometryGUI->getApp()->selectionMgr(),
       SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
  GroupRect->show();
 
  myX1=0;
  GroupRect->SpinBox_DX1->setValue(myX1); 
  myY1=0;
  GroupRect->SpinBox_DY1->setValue(myY1);
  myX2=10;
  GroupRect->SpinBox_DX2->setValue(myX2);
  myY2=10;
  GroupRect->SpinBox_DY2->setValue(myY2);
  
  resize( minimumSizeHint() );
  
  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
}


//=================================================================================
// function : DestClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::DestClicked( int constructorId )
{
  GroupPt->hide();
  GroupPt2->hide();
  GroupD1->hide();
  GroupD2->hide();

  if ( constructorId == 1 ) {
    // Point
    GroupPt->RB_Point1->setChecked( true );
    GroupPt->show();
    GroupPt2->show();
    Group3Spin->checkBox->show();
    Group4Spin->checkBox->show();
    PointClicked( 1 );  // XY
  }
  else if (  constructorId == 0 ) {
    // Direction
    GroupD1->RB_Dir11->setChecked( true );
    GroupD1->show();
    GroupD2->show();
    Group3Spin->checkBox->hide();
    Group4Spin->checkBox->hide();
    if (myConstructorId == 1) {
      myRadius = 100.0;
      myLength = 30.0;
      myAngle = 0.0;
      myDX = 0.0;
      myDY = 0.0;
    }
    Dir1Clicked( 2 );  // Angle
  }
  resize( minimumSizeHint() );
}

//=================================================================================
// function : PointClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::PointClicked( int constructorId )
{
  MESSAGE("PointClicked")
  myConstructorPntId = constructorId;  
  GroupPt->RB_Point3->setEnabled( true );
  int buttonId = GroupPt2->ButtonGroup->checkedId();
  if (buttonId >= 0){           // = If a button is checked
    Point2Clicked(buttonId);
  }
  else{ 
    GroupPt2->RB_Point1->setChecked( true );
    Point2Clicked( 1 ); 
  }
}

//=================================================================================
// function : Point2Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Point2Clicked( int constructorId )
{
  MESSAGE("Point2Clicked")
  InitClick();

  // Get setting of step value from file configuration
  double step = SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "SettingsGeomStep", 100.0 );

  if ( myConstructorPntId == 1 ) {  // XY
    if ( constructorId == 1 ){      // No additional parameter
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

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }
    else if ( constructorId == 0 ){   // Point + radius
      mySketchType = PT_ABS_RADIUS;
      initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" ); 
      initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step/10.0, "length_precision" );
      Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
      Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y2" ) );
      Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_RADIUS2" ) ); 
      myX = 0.0;
      Group3Spin->SpinBox_DX->setValue( myX );
      myY = 0.0;
      Group3Spin->SpinBox_DY->setValue( myY );
      myRadius=0.0;
      Group3Spin->SpinBox_DZ->setValue( myRadius ); 
      Group3Spin->show();
      Group3Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }
    else if ( constructorId == 2 ){   // Point + center
      mySketchType = PT_ABS_CENTER;
      initSpinBox( Group4Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" ); 
      initSpinBox( Group4Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group4Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group4Spin->SpinBox_DS, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group4Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_X2" ) );
      Group4Spin->TextLabel4->setText( tr( "GEOM_SKETCHER_Y2" ) );
      Group4Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_CENTER_X" ) );
      Group4Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_CENTER_Y" ) ); 
      myXc = 0.0;
      Group4Spin->SpinBox_DX->setValue( myXc );
      myYc = 0.0;
      Group4Spin->SpinBox_DY->setValue( myYc );
      myX = 0.0;
      Group4Spin->SpinBox_DZ->setValue( myX ); 
      myY = 0.0;
      Group4Spin->SpinBox_DS->setValue( myY );
      Group4Spin->show();
      Group4Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }

  }
  else if ( myConstructorPntId == 0 ) {  // DXDY
    if ( constructorId == 1 ){    // No additional parameter
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

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }
    else if ( constructorId == 0 ){   // Point + radius 
      mySketchType = PT_REL_RADIUS;
      initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" ); 
      initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step/10.0, "length_precision" );
      Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_DX2" ) );
      Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_DY2" ) );
      Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_RADIUS2" ) ); 
      myDX = 10.0;
      Group3Spin->SpinBox_DX->setValue( myDX );
      myDY = 0.0;
      Group3Spin->SpinBox_DY->setValue( myDY );
      myRadius=5.0;
      Group3Spin->SpinBox_DZ->setValue( myRadius );
      Group3Spin->show();
      Group3Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }
    else if ( constructorId == 2 ){   // Point + center
      mySketchType = PT_REL_CENTER;
      initSpinBox( Group4Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" ); 
      initSpinBox( Group4Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group4Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
      initSpinBox( Group4Spin->SpinBox_DS, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group4Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_DX2" ) );
      Group4Spin->TextLabel4->setText( tr( "GEOM_SKETCHER_DY2" ) );
      Group4Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_CENTER_DX" ) );
      Group4Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_CENTER_DY" ) ); 
      myDXc = 0.0;
      Group4Spin->SpinBox_DX->setValue( myDXc );
      myDYc = 0.0;
      Group4Spin->SpinBox_DY->setValue( myDYc );
      myDX = 0.0;
      Group4Spin->SpinBox_DZ->setValue( myDX ); 
      myDY = 0.0;
      Group4Spin->SpinBox_DS->setValue( myDY );
      Group4Spin->show();
      Group4Spin->buttonApply->setFocus();

      GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
    }

  }
  else if ( myConstructorPntId == 2 ) {  // Selection
    if ( constructorId == 1 ){    // No additional parameter
      mySketchType = PT_SEL;
      myEditCurrentArgument = Group1Sel->LineEdit1;
      Group1Sel->TextLabel1->setText( tr( "GEOM_SKETCHER_END_POINT2" ) ); 
      Group1Sel->show();
      Group1Sel->buttonApply->setFocus();
      SelectionIntoArgument();
    }
    else if ( constructorId == 0 ){   // Point + radius 
      mySketchType = PT_SEL_RADIUS;
      myEditCurrentArgument = Group1Sel1Spin->LineEdit1;
      initSpinBox( Group1Sel1Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step/10.0, "length_precision" );
      Group1Sel1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_END_POINT2" ) ); 
      Group1Sel1Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_RADIUS2" ) ); 
      myRadius=0.0;
      Group1Sel1Spin->SpinBox_DX->setValue( myRadius );

      Group1Sel1Spin->show();
      Group1Sel1Spin->buttonApply->setFocus();
      SelectionIntoArgument();
    }
    else if ( constructorId == 2 ){   // Point + center 
      mySketchType = PT_SEL_CENTER;
      myEditCurrentArgument = Group2Sel->LineEdit1;
      Group2Sel->TextLabel2->setText( tr( "GEOM_SKETCHER_END_POINT2" ) );  
      Group2Sel->TextLabel1->setText( tr( "GEOM_SKETCHER_CENTER2" ) );
      Group2Sel->LineEdit1->setEnabled(true);
      Group2Sel->PushButton1->setDown(true);
      Group2Sel->LineEdit2->setEnabled(false);
      Group2Sel->PushButton2->setDown(false);
      Group2Sel->show();
      Group2Sel->buttonApply->setFocus();
      SelectionIntoArgument();
    }

  }
  resize( minimumSizeHint() );
  connect( myGeometryGUI->getApp()->selectionMgr(),
      SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
}


//=================================================================================
// function : Dir1Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir1Clicked( int constructorId )
{
  myConstructorDirId = constructorId;
  int dirButtonId = GroupD2->ButtonGroup->checkedId();
  if (dirButtonId >= 0){           // = If a button is checked
    Dir2Clicked(dirButtonId);
  }
  else{ 
    GroupD2->RB_Dir21->setChecked( true );
    Dir2Clicked( 2 ); 
  }
}


//=================================================================================
// function : Dir2Clicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_SketcherDlg::Dir2Clicked( int constructorId )
{
  InitClick();
  Group3Spin->SpinBox_DX->blockSignals(true); // Block signals in order not to modify
  Group3Spin->SpinBox_DY->blockSignals(true); // defaut values
  Group3Spin->SpinBox_DZ->blockSignals(true);
  
  Group2Spin->SpinBox_DX->blockSignals(true);
  Group2Spin->SpinBox_DY->blockSignals(true);
  
  Group4Spin->SpinBox_DX->blockSignals(true); 
  Group4Spin->SpinBox_DY->blockSignals(true);
  Group4Spin->SpinBox_DZ->blockSignals(true);
  Group4Spin->SpinBox_DS->blockSignals(true); 
  
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
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_X2" ) );
        Group2Spin->SpinBox_DY->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_ANGLE_Y;
        initSpinBox( Group2Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "angle_precision" );        
        Group2Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_Y2" ) );
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
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
        Group1Spin->SpinBox_DX->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_PER_Y;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_Y2" ) );
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
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_X2" ) );
        Group1Spin->SpinBox_DX->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_TAN_Y;
        Group1Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_Y2" ) );
        Group1Spin->SpinBox_DX->setValue( myY );
      }
    }
    else if ( myConstructorDirId == 3 ) {  // DXDY
      initSpinBox( Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
      initSpinBox( Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
      Group3Spin->TextLabel1->setText( tr( "GEOM_SKETCHER_VX2" ) );
      Group3Spin->TextLabel2->setText( tr( "GEOM_SKETCHER_VY2" ) );
      Group3Spin->SpinBox_DX->setValue( myDX );
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
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_X2" ) );
        Group3Spin->SpinBox_DZ->setValue( myX );
      }
      else if ( constructorId == 1 ) {  // Y
        mySketchType = DIR_DXDY_Y;
        Group3Spin->TextLabel3->setText( tr( "GEOM_SKETCHER_Y2" ) );
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
        Group3Spin->SpinBox_DY->setValue( myRadius );
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
//         myRadius = 100.0;
        Group2Spin->SpinBox_DX->setValue( myRadius );
//         myLength = 30.0;
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
//         myRadius = 100.0;
        Group2Spin->SpinBox_DX->setValue( myRadius );
//         myLength = 30.0;
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
        initSpinBox( Group4Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
        initSpinBox( Group4Spin->SpinBox_DX, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
        initSpinBox( Group4Spin->SpinBox_DY, COORD_MIN, COORD_MAX, 0.1, "length_precision" );
        initSpinBox( Group4Spin->SpinBox_DS, COORD_MIN, COORD_MAX, 5., "length_precision" );
        Group4Spin->SpinBox_DX->setValue( myDX );
        Group4Spin->SpinBox_DY->setValue( myDY );
//         myRadius = 100.0;
        Group4Spin->SpinBox_DZ->setValue( myRadius );
//         myLength = 30.0;
        Group4Spin->SpinBox_DS->setValue( myLength );
        Group4Spin->show();
        Group4Spin->buttonApply->setFocus();
      }
    }
  }
  Group3Spin->SpinBox_DX->blockSignals(false); // Restore signals
  Group3Spin->SpinBox_DY->blockSignals(false); 
  Group3Spin->SpinBox_DZ->blockSignals(false);
  
  Group2Spin->SpinBox_DX->blockSignals(false);
  Group2Spin->SpinBox_DY->blockSignals(false);
  
  Group4Spin->SpinBox_DX->blockSignals(false); 
  Group4Spin->SpinBox_DY->blockSignals(false);
  Group4Spin->SpinBox_DZ->blockSignals(false);
  Group4Spin->SpinBox_DS->blockSignals(false); 
  
  resize( minimumSizeHint() );
  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
}

//=================================================================================
// function : CheckBoxClicked()
// purpose  : CheckBox  management
//=================================================================================
void EntityGUI_SketcherDlg::CheckBoxClicked(int State)
{
  myCheckFlag = State;

  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
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
  MESSAGE("EntityGUI_SketcherDlg::ClickOnEnd()")
  if ( sender() == MainWidget->buttonClose ) {
    // Verify validity of commands
    if ( myCommand.count() <= 2 ) {
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr( "GEOM_ERROR_STATUS" ), tr( "CANNOT_CLOSE" ), tr( "BUT_OK" ) );
      return;
    }

    QString Command = myCommand.join( "" );
    Sketcher_Profile aProfile = Sketcher_Profile( Command.toLatin1() );
    bool isDone = false;
    TopoDS_Shape myShape = aProfile.GetShape( &isDone );
    if ( isDone ) {
      if ( myShape.ShapeType() != TopAbs_VERTEX )
        myCommand.append( ":WW" );
    }
  }
  else {
    /*// PAL16008 (Sketcher Validation should be equal to Apply&Close)
    if ( ( Group1Spin->buttonApply->isEnabled() && Group1Spin->isVisible() ) ||
         ( Group2Spin->buttonApply->isEnabled() && Group2Spin->isVisible() ) ||
         ( Group3Spin->buttonApply->isEnabled() && Group3Spin->isVisible() ) ||
         ( Group4Spin->buttonApply->isEnabled() && Group4Spin->isVisible() ) ||
         ( Group1Sel->buttonApply->isEnabled()  && Group1Sel->isVisible()  ) ||
         ( Group2Sel->buttonApply->isEnabled()  && Group2Sel->isVisible()  ) ||
         ( Group1Sel1Spin->buttonApply->isEnabled() && Group1Sel1Spin->isVisible() ) )  {     
      ClickOnApply();
    }*/
//     ClickOnApply(); // TEST remove then
    myIsAllAdded = true;
  }

  if (myConstructorId == 2)
  {
    QString aParameters;
    myCommand.append( GetNewCommand( aParameters ) );
    mySketchState = NEXT_POINT;
    if ( onAccept() )
      ClickOnCancel();
  }
  if ( myCommand.size() > 2 )
  {
    if ( !onAccept() )
      return;
  }

  close();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::ClickOnApply()
{
  MESSAGE("EntityGUI_SketcherDlg::ClickOnApply()")
  if ( sender() && sender()->inherits( "QPushButton" ) )
    ( (QPushButton*)sender() )->setFocus(); // to update value of currently edited spin-box (PAL11948)

//   addSubshapesToStudy();

  QString aParameters;
  myCommand.append( GetNewCommand( aParameters ) );
  if (mySketchState == FIRST_POINT)
  {
    mySketchState = NEXT_POINT;
    OnNextPoint();
  }

  myUndoCommand.clear();
  myUndoCommand.append( "Sketcher" );

  myParameters.append( aParameters );
  myUndoParameters.clear();

  setEnabledUndo( true );
  setEnabledRedo( false );

  myIsApply = true;
  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
  myIsApply = false;

  // Set focus to SpinBox_DX
  if ( sender() == Group1Spin->buttonApply ) {
    (Group1Spin->SpinBox_DX)->setFocus();
    (Group1Spin->SpinBox_DX)->selectAll();
  }
  else if ( sender() == Group1Sel1Spin->buttonApply ) {
    (Group1Sel1Spin->SpinBox_DX)->setFocus();
    (Group1Sel1Spin->SpinBox_DX)->selectAll();
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

    connect( myGeometryGUI->getApp()->selectionMgr(),
             SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

    OnFirstPoint();

    setEnabledUndo( false );
  }

  setEnabledRedo( true );

  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
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
  
  OnNextPoint();
  
  setEnabledUndo( true );

  if ( myUndoCommand.count() == 1 )
    setEnabledRedo( false );

  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
}

//=================================================================================
// function : setEnabledUndo()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::setEnabledUndo( bool value )
{
  Group1Sel->buttonUndo->setEnabled( value );
  Group2Sel->buttonUndo->setEnabled( value );
  Group1Sel1Spin->buttonUndo->setEnabled( value );
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
  Group2Sel->buttonRedo->setEnabled( value );
  Group1Sel1Spin->buttonRedo->setEnabled( value );
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
  MESSAGE("EntityGUI_SketcherDlg::SelectionIntoArgument")
  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeometryGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);
  
  this->activateWindow();
  
  if (aSelList.Extent() == 0)
  {
    selButton->setDown(false);
    WPlaneLineEdit->setEnabled(false);
    WPlaneLineEdit->setText(tr("GEOM_SKETCHER_WPLANE"));
    return;
  }
  else if (aSelList.Extent() != 1)                                
    return;
  
  double tmpX = myX;
  double tmpY = myY;
  myX = myLastX1;
  myY = myLastY1;
  
  TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
  if (myEditCurrentArgument == WPlaneLineEdit)
    aNeedType = TopAbs_FACE;
  
 
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  
  if(aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) 
                     && !aShape.IsNull())
  { 
    QString aName = GEOMBase::GetName( aSelectedObject.get() ); 
    if (myEditCurrentArgument==WPlaneLineEdit)  
    { 
      // Check if the face is planar
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
      GeomLib_IsPlanarSurface aPlanarCheck(aSurf, Precision::Confusion());
      
      if (aPlanarCheck.IsPlanar())
      {
        myEditCurrentArgument->setText(aName);
        AddLocalCS(aSelectedObject.get());
        selButton->setDown(false);
        WPlaneLineEdit->setEnabled(false);
        TypeClicked( myConstructorId );
      }
      else
      {
        myEditCurrentArgument->setText(tr("GEOM_SKETCHER_WPLANE"));
        // The following leads to crash TODO : find a way to return a warning
//         Standard_Failure::Raise(tr("GEOM_SKETCHER_NOT_PLANAR").toStdString().c_str()); 
      }
    }           
    else
    {
      gp_Trsf aTrans;
      gp_Ax3 aWPlane = GetActiveLocalCS();

      aTrans.SetTransformation(aWPlane);
      BRepBuilderAPI_Transform aTransformation (aShape, aTrans, Standard_False);
      aShape = aTransformation.Shape();

      gp_Pnt aPnt;
      if ( GEOMBase::VertexToPoint( aShape, aPnt ) ) 
      {
        myEditCurrentArgument->setText(aName);
        myX = aPnt.X();
        myY = aPnt.Y();       
        double Xcoord = myX;
        double Ycoord = myY;
        
        switch (mySketchType)
        {
          case PT_ABS:
          disconnect( Group2Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
          disconnect( Group2Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
          
          Group2Spin->SpinBox_DX->setValue(Xcoord);
          Group2Spin->SpinBox_DY->setValue(Ycoord);
      
          connect( Group2Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
          connect( Group2Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            break;
            
          case PT_RELATIVE:
            if (myLastX1 && myLastY1)
            {
              Xcoord = myX - myLastX1;
              Ycoord = myY - myLastY1;
            }
            else if (mySketchState != FIRST_POINT)
            {
              Xcoord = myX - tmpX;
              Ycoord = myY - tmpY;
            }          
            Group2Spin->SpinBox_DX->setValue(Xcoord);
            Group2Spin->SpinBox_DY->setValue(Ycoord);      
            break; 
            
          case PT_ABS_RADIUS:
            disconnect( Group3Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            disconnect( Group3Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            
            Group3Spin->SpinBox_DX->setValue(Xcoord);
            Group3Spin->SpinBox_DY->setValue(Ycoord);
      
            connect( Group3Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            connect( Group3Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            break;
            
          case PT_REL_RADIUS:
            if (myLastX1 && myLastY1)
            {
              Xcoord = myX - myLastX1;
              Ycoord = myY - myLastY1;
            }
            else if (mySketchState != FIRST_POINT)
            {
              Xcoord = myX - tmpX;
              Ycoord = myY - tmpY;
            }             
            Group3Spin->SpinBox_DX->setValue(Xcoord);
            Group3Spin->SpinBox_DY->setValue(Ycoord);
            break; 
            
          case PT_ABS_CENTER:
            disconnect( Group4Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            disconnect( Group4Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            
            Group4Spin->SpinBox_DX->setValue(Xcoord);
            Group4Spin->SpinBox_DY->setValue(Ycoord);
      
            connect( Group4Spin->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
            connect( Group4Spin->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );        
            break;
            
          case PT_REL_CENTER:
            if (myLastX1 && myLastY1)
            {
              Xcoord = myXc - myLastX1;
              Ycoord = myYc - myLastY1;
            }
            else if (mySketchState != FIRST_POINT)
            {
              Xcoord = myXc - tmpX;
              Ycoord = myYc - tmpY;
            } 
            else
            {
              Xcoord = myXc;
              Ycoord = myYc;
            }
            
            Group4Spin->SpinBox_DX->setValue(Xcoord);
            Group4Spin->SpinBox_DY->setValue(Ycoord);            
            break; 
            
        }
      }
    }
  }

  if(!autoApply){
    GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
  }
  else {    
    ClickOnApply();
    autoApply = false;
  }
  
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::SetEditCurrentArgument()
{
  TopAbs_ShapeEnum myNeedType = TopAbs_VERTEX;
  if ( sender() == Group1Sel->PushButton1 ) {
    myEditCurrentArgument = Group1Sel->LineEdit1;
    myEditCurrentArgument->setFocus();
  }
  else if ( sender() == Group1Sel1Spin->PushButton1 ) {
    myEditCurrentArgument = Group1Sel1Spin->LineEdit1;
    myEditCurrentArgument->setFocus();
  }
  else if ( sender() == Group2Sel->PushButton1 ) {
    myEditCurrentArgument = Group2Sel->LineEdit1;
    Group2Sel->PushButton2->setDown(false);
    Group2Sel->PushButton1->setDown(true);
    Group2Sel->LineEdit2->setEnabled(false);
    Group2Sel->LineEdit1->setEnabled(true);
   // myEditCurrentArgument->setFocus();
  }
   else if ( sender() == Group2Sel->PushButton2 ) {
    myEditCurrentArgument = Group2Sel->LineEdit2;
    Group2Sel->PushButton1->setDown(false);
    Group2Sel->PushButton2->setDown(true);
    Group2Sel->LineEdit1->setEnabled(false);
    Group2Sel->LineEdit2->setEnabled(true);
   // myEditCurrentArgument->setFocus();
  }
  else if ( sender() == selButton ) {
    myNeedType = TopAbs_FACE;
    myEditCurrentArgument = WPlaneLineEdit;
    WPlaneLineEdit->setEnabled(true);
    selButton->setDown(true);
  }
  globalSelection(); // close local selection to clear it
  localSelection( myNeedType );
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::LineEditReturnPressed()
{
  if ( sender() == Group1Sel->LineEdit1 ) 
    myEditCurrentArgument = Group1Sel->LineEdit1;
  else if ( sender() == Group1Sel1Spin->LineEdit1 ) 
     myEditCurrentArgument = Group1Sel1Spin->LineEdit1;
  else if ( sender() == Group2Sel->LineEdit1 ) 
     myEditCurrentArgument = Group2Sel->LineEdit1;
  else if ( sender() == Group2Sel->LineEdit2 )
     myEditCurrentArgument = Group2Sel->LineEdit2;

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
  MESSAGE("EntityGUI_SketcherDlg::ActivateThisDialog()")
  myGeometryGUI->EmitSignalDeactivateDialog();
  setEnabled( true );
  myGeometryGUI->SetActiveDialogBox( this );

  connect( myGeometryGUI->getApp()->selectionMgr(),
          SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  //myGeometryGUI->SetState( 0 );
//   globalSelection( GEOM_POINT );
  
  myEditCurrentArgument = WPlaneLineEdit;
  myEditCurrentArgument->setFocus();
  
   if ( sender() == Group1Sel->LineEdit1 ) {
     myEditCurrentArgument = Group1Sel->LineEdit1;
     myEditCurrentArgument->setFocus();
   }
   else if ( sender() == Group1Sel1Spin->LineEdit1 ) {
     myEditCurrentArgument = Group1Sel1Spin->LineEdit1;
     myEditCurrentArgument->setFocus();
   }
   else if ( sender() == Group2Sel->LineEdit1 ) {
     myEditCurrentArgument = Group2Sel->LineEdit1;
     myEditCurrentArgument->setFocus();
   }
    else if ( sender() == Group2Sel->LineEdit2 ) {
     myEditCurrentArgument = Group2Sel->LineEdit2;
     myEditCurrentArgument->setFocus();
   }
   
  GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::enterEvent( QEvent* )
{
  if ( !isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::closeEvent( QCloseEvent* e )
{
  MESSAGE("EntityGUI_SketcherDlg::closeEvent")
  //myGeometryGUI->SetState( -1 );
  disconnect( myGeometryGUI->getApp()->selectionMgr(), 0, this, 0 );
  myGeometryGUI->getApp()->updateActions();
  QDialog::closeEvent( e );
}

//=================================================================================
// function : OnPointSelected
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::OnPointSelected(Qt::KeyboardModifiers modifiers, const gp_Pnt& thePnt,
                                            bool isStart )
{
  // NOTE Basing the autoapply functionnality on the background picture has no sense anymore
  // The import picture functionnality is now used for drawing on top of a picture
  
//   SUIT_ViewWindow*      theViewWindow  = getDesktop()->activeWindow();
//   OCCViewer_ViewPort3d* vp             = ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
//   
//   QString                theImgFileName;
//   vp->background().texture( theImgFileName ); ////////////// VSR: temporarily
  
  double x, y;
  x = y = 0;
  
  BRepBuilderAPI_MakeVertex mkVertex (thePnt);
  TopoDS_Shape aShape = mkVertex.Shape();
  
  // Taking into account LocalCS
  gp_Ax3 aWPlane = GetActiveLocalCS();
  gp_Trsf aTrans;

  aTrans.SetTransformation(aWPlane);
  BRepBuilderAPI_Transform aTransformation (aShape, aTrans, Standard_False);
  aShape = aTransformation.Shape();
  
  gp_Pnt aTrsfPnt; 
  GEOMBase::VertexToPoint( aShape, aTrsfPnt );
  
  // NOTE autoapply disabled : doesn't seem useful anymore
//   autoApply = ( (getPnt2ConstructorId() == 1) && (!theImgFileName.isEmpty()) );  // If no additional argument needed after selection and there is a backgroundimage
                                                                                 // -> apply automatically 
  
  if ( getPnt1ConstructorId() == 0 ){                    // Relative selection mode
    x = aTrsfPnt.X() - myLastX1;
    y = aTrsfPnt.Y() - myLastY1;
  }
  else {
    x = aTrsfPnt.X();
    y = aTrsfPnt.Y();
  }
  switch (getPnt2ConstructorId()){
    case 1:
      Group2Spin->SpinBox_DX->setValue( x );
      Group2Spin->SpinBox_DY->setValue( y );
      Group2Spin->buttonApply->setFocus();               // Previous setFocus (during preview) may have been inoperative if it was disabled 
      break;
    case 0:
      Group3Spin->SpinBox_DX->setValue( x );
      Group3Spin->SpinBox_DY->setValue( y );
      Group3Spin->buttonApply->setFocus(); 
      break;
    case 2:
      if (modifiers == Qt::MetaModifier){                // Select center with Meta key
        Group4Spin->SpinBox_DX->setValue( x );
        Group4Spin->SpinBox_DY->setValue( y );
      }
      else{                                              // Select end point
        Group4Spin->SpinBox_DZ->setValue( x );
        Group4Spin->SpinBox_DS->setValue( y );
      }
      Group4Spin->buttonApply->setFocus(); 
      break;
  }
  if ( myConstructorId == 2 ) //RECTANGLE
  {
    if (isStart)
    {
      GroupRect->SpinBox_DX1->setValue( aTrsfPnt.X() );
      GroupRect->SpinBox_DY1->setValue( aTrsfPnt.Y() );
      GroupRect->SpinBox_DX2->setValue( aTrsfPnt.X() );
      GroupRect->SpinBox_DY2->setValue( aTrsfPnt.Y() );
    }
    else
    {
      GroupRect->SpinBox_DX2->setValue( aTrsfPnt.X() );
      GroupRect->SpinBox_DY2->setValue( aTrsfPnt.Y() );
    }
  }
}

//=================================================================================
// function : OnFirstPoint
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::OnFirstPoint()
{
  MainWidget->RadioButton3->setEnabled( true );
  MainWidget->GroupDest1->setEnabled( false );
  GroupPt2->setEnabled( false );
}

//=================================================================================
// function : OnNextPoint
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::OnNextPoint()
{
  MainWidget->RadioButton3->setEnabled( false );
  MainWidget->GroupDest1->setEnabled( true );
  TypeClicked(myConstructorId);
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_SketcherDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();
  Standard_Real vx, vy, vz, vs, minRad, dx, dy;
  vx = vy = vz = vs = minRad = dx = dy = 0.0;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
  double LastDecimal = 0.0;

  QString vxStr, vyStr, vzStr, vsStr;

  QString newValueStr;
  if( SalomeApp_DoubleSpinBox* aDoubleSpinBox = (SalomeApp_DoubleSpinBox*)send )
    newValueStr = aDoubleSpinBox->text();

  if ( send == Group1Spin->SpinBox_DX ) {
    vx = newValue;                        vxStr = newValueStr;
  }
  else if ( send == Group1Sel1Spin->SpinBox_DX ) {
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
    vz = Group3Spin->SpinBox_DZ->value();
    if ( (mySketchType == PT_REL_RADIUS || mySketchType == PT_ABS_RADIUS) && (vx != 0 || vy != 0) ) {  
      if (mySketchType == PT_ABS_RADIUS){
        dx = vx - myLastX1;
        dy = vy - myLastY1;
      }
      else{
        dx = vx;
        dy = vy;
      }
      minRad = 0.5 * Sqrt(dx * dx + dy * dy);                             //Computation of the minimum acceptable radius for the arc calculation
      if (aPrecision >= 0)    // 'f' format in the QString             
        LastDecimal = aPrecision;
      else                    // 'g' format in the Qstring
        LastDecimal = qAbs( aPrecision ) - ceil( log10(minRad) ); 
      minRad = ceil(pow(10,LastDecimal) * minRad) / pow(10,LastDecimal);  // Rounded up at the last allowed decimal place
      if ( Abs(vz) < minRad){
        if (vz < 0.0)
          Group3Spin->SpinBox_DZ->setValue( - minRad );
        else 
          Group3Spin->SpinBox_DZ->setValue( minRad );
      }
    }
    vz = Group3Spin->SpinBox_DZ->value(); vzStr = Group3Spin->SpinBox_DZ->text();
  }
  else if ( send == Group3Spin->SpinBox_DY ) {
    vx = Group3Spin->SpinBox_DX->value(); vxStr = Group3Spin->SpinBox_DX->text();
    vy = newValue;                        vyStr = newValueStr;
    vz = Group3Spin->SpinBox_DZ->value(); vzStr = Group3Spin->SpinBox_DZ->text();
    if ( (mySketchType == PT_REL_RADIUS || mySketchType == PT_ABS_RADIUS) && (vx != 0 || vy != 0)){
      if (mySketchType == PT_ABS_RADIUS){
        dx = vx - myLastX1;
        dy = vy - myLastY1;
      }
      else{
        dx = vx;
        dy = vy;
      }    
      minRad = 0.5 * Sqrt(dx * dx + dy * dy);                            //Computation of the minimum acceptable radius for the arc calculation
      if (aPrecision >= 0)    // 'f' format in the QString 
        LastDecimal = aPrecision;
      else                    // 'g' format in the QString 
        LastDecimal = qAbs( aPrecision ) - ceil( log10(minRad) ); 
        minRad = ceil(pow(10,LastDecimal) * minRad) / pow(10,LastDecimal); // Rounded up at the last allowed decimal place
      if ( Abs(vz) < minRad){
        if (vz < 0.0)
          Group3Spin->SpinBox_DZ->setValue( - minRad );
        else 
          Group3Spin->SpinBox_DZ->setValue( minRad );
      }
    }
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
    if ( mySketchType == PT_ABS_RADIUS ) {  
      myX = vx;
      myY = vy;
      myRadius=vz;
      myXStr = vxStr;
      myYStr = vyStr;
      myRadiusStr = vzStr;
    }
    else if ( mySketchType == PT_REL_RADIUS ) {
      myDX = vx;
      myDY = vy;
      myRadius=vz;
      myDXStr = vxStr;
      myDYStr = vyStr;
      myRadiusStr = vzStr;
    }
    else if ( mySketchType == PT_SEL_RADIUS ) {
      myRadius = vx;
      myRadiusStr = vxStr;
    }
    if ( mySketchType == PT_ABS_CENTER ) {  
      myXc = vx;
      myYc = vy;
      myX = vz;
      myY = vs;
      myXcStr = vxStr;
      myYcStr = vyStr;
      myXStr = vzStr;
      myYStr = vsStr;
    }
    else if ( mySketchType == PT_REL_CENTER ) {
      myDXc = vx;
      myDYc = vy;
      myDX = vz;
      myDY = vs;
      myDXcStr = vxStr;
      myDYcStr = vyStr;
      myDXStr = vzStr;
      myDYStr = vsStr;
    }
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
  else if ( myConstructorId == 2 ) {  // RECTANGLE
    myX1 = GroupRect->SpinBox_DX1->value(); myX1Str = GroupRect->SpinBox_DX1->text();
    myX2 = GroupRect->SpinBox_DX2->value(); myX2Str = GroupRect->SpinBox_DX2->text();
    myY1 = GroupRect->SpinBox_DY1->value(); myY1Str = GroupRect->SpinBox_DY1->text();
    myY2 = GroupRect->SpinBox_DY2->value(); myY2Str = GroupRect->SpinBox_DY2->text();
  }

  
  if (!autoApply) 
    GEOMBase_Helper::displayPreview( true, false, true, true, myLineWidth );
  
  double x, y, xc, yc;
  x = y = xc = yc = 0.0;
  if ( mySketchType == PT_ABS_CENTER || mySketchType == PT_REL_CENTER )
  {
    if ( mySketchType == PT_REL_CENTER ){
      x  = myDX  + myLastX1;
      y  = myDY  + myLastY1;
      xc = myDXc + myLastX1;
      yc = myDYc + myLastY1;
    }
    else {
      x  = myX ;
      y  = myY ;
      xc = myXc;
      yc = myYc;
    }
    displayPntPreview(xc,yc);
    displayPntPreview(x,y);
  }
}


//=================================================================================
// function : GetNewCommand()
// purpose  : Build the new command with context
//=================================================================================
QString EntityGUI_SketcherDlg::GetNewCommand( QString& theParameters )
{
  theParameters.clear();
  QString myNewCommand = ":";
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
  int DigNum = qAbs(aPrecision);                   // options for the format of numbers in  myNewCommand
  char Format = 'f';
  if ( aPrecision < 0 )                            // f --> DigNum is the number of digits after the decimal point
    Format = 'g';                                  // g --> DigNum is the maximum number of significant digits 

  if ( mySketchState == FIRST_POINT ) {
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL ) {
      myNewCommand = myNewCommand + "F " + QString::number( myX, Format, DigNum) + " " + QString::number( myY, Format, DigNum);
      theParameters = myXStr + ":" + myYStr;
    }
    if ( mySketchType == PT_RELATIVE) {
      myNewCommand = myNewCommand + "F " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      theParameters = myDXStr + ":" + myDYStr;
    }
    if (myConstructorId == 2 )
      myNewCommand = ":";
    else
      return myNewCommand;
  }

  if ( myConstructorId == 0  ) {  // SEGMENT
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL ) {
      myNewCommand = myNewCommand + "TT " + QString::number( myX, Format, DigNum) + " " + QString::number( myY, Format, DigNum);
      theParameters = myXStr + ":" + myYStr;
    }
    if ( mySketchType == PT_RELATIVE) {
      myNewCommand = myNewCommand + "T " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      theParameters = myDXStr + ":" + myDYStr;
    }
    if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength, Format, DigNum );
      theParameters = myAngleStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_ANGLE_X ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX, Format, DigNum);
      theParameters = myAngleStr + ":" + myXStr;
    }
    if ( mySketchType == DIR_ANGLE_Y ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY, Format, DigNum);
      theParameters = myAngleStr + ":" + myYStr;
    }
    if ( mySketchType == DIR_PER_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength, Format, DigNum );
      theParameters = QString::number( 90.0 ) + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_PER_X ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX, Format, DigNum);
      theParameters = QString::number( 90.0 ) + ":" + myXStr;
    }
    if ( mySketchType == DIR_PER_Y ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY, Format, DigNum);
      theParameters = QString::number( 90.0 ) + ":" + myYStr;
    }
    if ( mySketchType == DIR_TAN_LENGTH ) {
      myNewCommand = myNewCommand + "L " + QString::number( myLength, Format, DigNum );
      theParameters = myLengthStr;
    }
    if ( mySketchType == DIR_TAN_X ) {
      myNewCommand = myNewCommand + "IX " + QString::number( myX, Format, DigNum);
      theParameters = myXStr;
    }
    if ( mySketchType == DIR_TAN_Y) {
      myNewCommand = myNewCommand + "IY " + QString::number(myY, Format, DigNum);
      theParameters = myYStr;
    }
    if ( mySketchType == DIR_DXDY_LENGTH ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      myNewCommand = myNewCommand + ":" + "L " + QString::number( myLength, Format, DigNum );
      theParameters = myDXStr + ":" + myDYStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_DXDY_X ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      myNewCommand = myNewCommand + ":" + "IX " + QString::number( myX, Format, DigNum);
      theParameters = myDXStr + ":" + myDYStr + ":" + myXStr;
    }
    if ( mySketchType == DIR_DXDY_Y ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      myNewCommand = myNewCommand + ":" + "IY " + QString::number( myY, Format, DigNum);
      theParameters = myDXStr + ":" + myDYStr + ":" + myYStr;
    }
  }
  else if ( myConstructorId == 1 ) {  // ARC
    if ( mySketchType == PT_ABS || mySketchType == PT_SEL ) {
      myNewCommand = myNewCommand + "AA " + QString::number( myX, Format, DigNum) + " " + QString::number( myY, Format, DigNum);
      theParameters = myXStr + ":" + myYStr;
    }
    if ( mySketchType == PT_RELATIVE) {
      myNewCommand = myNewCommand + "A " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      theParameters = myDXStr + ":" + myDYStr;
    }
    if ( mySketchType == PT_ABS_RADIUS || mySketchType == PT_SEL_RADIUS ) {
      myNewCommand = myNewCommand + "UU " + QString::number( myX, Format, DigNum) + " " + QString::number( myY, Format, DigNum ) + " " + QString::number( myRadius , Format,  DigNum)+ " " + QString::number( myCheckFlag );
      theParameters = myXStr + ":" + myYStr + ":" + myRadiusStr; 
    }
    if ( mySketchType == PT_REL_RADIUS) {
      myNewCommand = myNewCommand + "U " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum ) + " " + QString::number( myRadius, Format, DigNum )+ " " + QString::number( myCheckFlag );
      theParameters = myDXStr + ":" + myDYStr + ":" + myRadiusStr;  
    }
    if ( mySketchType == PT_ABS_CENTER || mySketchType == PT_SEL_CENTER ) {
      myNewCommand = myNewCommand + "EE " + QString::number( myX, Format, DigNum) + " " + QString::number( myY, Format, DigNum ) + " " + QString::number( myXc , Format,  DigNum) + " " + QString::number( myYc , Format,  DigNum)+ " " + QString::number( myCheckFlag ) + " " + QString::number( 0 );
      theParameters = myXStr + ":" + myYStr + ":" + myXcStr+ ":" + myYcStr; 
    }
    if ( mySketchType == PT_REL_CENTER) {
      myNewCommand = myNewCommand + "E " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum ) + " " + QString::number( myDXc, Format, DigNum )+ " " + QString::number( myDYc, Format, DigNum )+ " " + QString::number( myCheckFlag )+ " " + QString::number( 0 );
      theParameters = myDXStr + ":" + myDYStr + ":" +  myDXcStr + ":" + myDYcStr ;  
    }
    if ( mySketchType == DIR_ANGLE_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( myAngle );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius, Format, DigNum ) + " " + QString::number( myLength, Format, DigNum );
      theParameters = myAngleStr + ":" + myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_PER_LENGTH ) {
      myNewCommand = myNewCommand + "R " + QString::number( 90.0 );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius, Format, DigNum ) + " " + QString::number( myLength, Format, DigNum );
      theParameters = QString::number( 90.0 ) + ":" + myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_TAN_LENGTH ) {
      myNewCommand = myNewCommand + "C " + QString::number( myRadius, Format, DigNum ) + " " + QString::number( myLength, Format, DigNum );
      theParameters = myRadiusStr + ":" + myLengthStr;
    }
    if ( mySketchType == DIR_DXDY_LENGTH ) {
      myNewCommand = myNewCommand + "D " + QString::number( myDX, Format, DigNum ) + " " + QString::number( myDY, Format, DigNum );
      myNewCommand = myNewCommand + ":" + "C " + QString::number( myRadius, Format, DigNum ) + " " + QString::number( myLength, Format, DigNum );
      theParameters = myDXStr + ":" + myDYStr + ":" + myRadiusStr + ":" + myLengthStr;
    }
  }
  else if ( myConstructorId == 2 ) {  // RECTANGLE
      myNewCommand = myNewCommand + "F " + QString::number( myX1, Format, DigNum) + " " + QString::number( myY1, Format, DigNum)
                                  + ":TT " + QString::number( myX1, Format, DigNum) + " " + QString::number( myY2, Format, DigNum)
                                  + ":TT " + QString::number( myX2, Format, DigNum) + " " + QString::number( myY2, Format, DigNum)
                                  + ":TT " + QString::number( myX2, Format, DigNum) + " " + QString::number( myY1, Format, DigNum)
                                  + ":WW";
                          
      theParameters = myX1Str + ":" + myY1Str + ":" + myX1Str + ":" + myY2Str + ":" + myX2Str + ":" + myY2Str + ":" + myX2Str + ":" + myY1Str;
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
  else if( Group1Sel1Spin->isVisible() ) {
    ok = Group1Sel1Spin->SpinBox_DX->isValid( msg, toCorrect ) && ok;
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
  MESSAGE("EntityGUI_SketcherDlg::execute")
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
  int DigNum = qAbs(aPrecision);                   // options for the format of numbers in  myNewCommand
  char Format = 'f';
  if ( aPrecision < 0 )                            // f --> DigNum is the number of digits after the decimal point
    Format = 'g';                                  // g --> DigNum is the maximum number of significant digits 
    
  QString aParameters;
  
  if ( mySketchState == FIRST_POINT ) {
    myLastX2 = myX;
    myLastY2 = myY;
  }
  else {
    //Test if the current point is the same as the last one
    TopoDS_Shape myShape1, myShape2;
    bool isDone = false;
    double error = 0.;

    // Set "C" numeric locale
    Kernel_Utils::Localizer loc;

    //Last Shape
    QString Command1 = myCommand.join( "" );
    Sketcher_Profile aProfile1( Command1.toLatin1() );
    myShape1 = aProfile1.GetShape();

    //Current Shape
    QString Command2 = Command1 + GetNewCommand( aParameters );
    Sketcher_Profile aProfile2( Command2.toLatin1() );
    myShape2 = aProfile2.GetShape( &isDone, &error );

    //Error Message
    if ( mySketchType == PT_ABS_CENTER || mySketchType == PT_REL_CENTER  ){
      if (error > Precision::Confusion()){
        Group4Spin->label->show();
        Group4Spin->label->setText( tr("GEOM_SKETCHER_WARNING") + QString::number( error, Format, DigNum));
      }
      else{
        Group4Spin->label->hide();
      } 
    }
    else 
      Group4Spin->label->hide();
    if ( mySketchType == PT_SEL_CENTER ){
      if (error > Precision::Confusion()){
        Group2Sel->label->show();
        Group2Sel->label->setText( tr("GEOM_SKETCHER_WARNING") + QString::number( error, Format, DigNum));
      }
      else{
        Group2Sel->label->hide();
      } 
    }
    else 
      Group2Sel->label->hide();

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
    if ( Group2Sel->isVisible() ) {
      Group2Sel->buttonApply->setEnabled( false );
      //Group2Sel->buttonApply->setFocus();
    }
    if ( Group1Sel1Spin->isVisible() ) {
      Group1Sel1Spin->buttonApply->setEnabled( false );
      //Group1Sel1Spin->buttonApply->setFocus();
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
    }
    if ( Group2Sel->isVisible() ) {
      Group2Sel->buttonApply->setEnabled( true );
    }
    if ( Group1Sel1Spin->isVisible() ) {
      Group1Sel1Spin->buttonApply->setEnabled( true );;
    }
    if ( Group1Spin->isVisible() ) {
      Group1Spin->buttonApply->setEnabled( true );
    }
    if ( Group2Spin->isVisible() ) {
      Group2Spin->buttonApply->setEnabled( true );
    }
    if ( Group3Spin->isVisible() ) {
      Group3Spin->buttonApply->setEnabled( true );
    }
    if ( Group4Spin->isVisible() ) {
      Group4Spin->buttonApply->setEnabled( true );
    }
  }

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = NULL;
  
  int index = ComboBox1->currentIndex();
  if(index != -1 && !myWPlane->_is_nil()) // The combobox is not empty
  {
    anObj = anOper->MakeSketcherOnPlane( cmd.toLatin1().constData(), myWPlane );
  }
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
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr(); 
  
  QColor aColor = resMgr->colorValue("Geometry","line_color",QColor(255,0,0));
  Quantity_NameOfColor line_color = SalomeApp_Tools::color( aColor ).Name();
  
  // set width of displayed shape
  int lw = lineWidth;
  if(lw == -1) { 
    lw = resMgr->integerValue("Geometry", "preview_edge_width", -1);
  }
  getDisplayer()->SetWidth( lw );

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  CORBA::String_var objStr = myGeometryGUI->getApp()->orb()->object_to_string( object );
  getDisplayer()->SetName( objStr.in() );

  // Create wire from applied object
  TopoDS_Shape anApplyedWire, aLastSegment;
  if ( !createShapes( object, anApplyedWire, aLastSegment ) )
    return;

  // Build prs
  if( myConstructorId != 2)
    getDisplayer()->SetColor( line_color );
  else
    getDisplayer()->SetColor( Quantity_NOC_VIOLET );
  
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( anApplyedWire );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->SetColor( Quantity_NOC_VIOLET ); 
  aPrs = getDisplayer()->BuildPrs( aLastSegment );
  
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );

  getDisplayer()->SetColor( line_color );
  
  getDisplayer()->UnsetName();

  // Enable activation of displayed objects
  getDisplayer()->SetToActivate( true );
}

//================================================================
// Function : displayPntPreview
// Purpose  : creates a TopoDS_VERTEX and display a preview of it
//================================================================
void EntityGUI_SketcherDlg::displayPntPreview(const double x,
                                              const double y,
                                              bool append,
                                              bool update
                                             )
{
  // Get globalCS and working plane
  gp_Ax3 globalCS = WPlaneToLCS( myGlobalCS );
  gp_Ax3 aWPlane  = GetActiveLocalCS();
  
  // Build point in localCS
  gp_Pnt aPnt = gp_Pnt(x,y,0.0);
  
  // Get transfomation from local to global CS
  gp_Trsf aTrans;
  aTrans.SetTransformation(aWPlane, globalCS);
  
  BRepBuilderAPI_MakeVertex mkVertex (aPnt);
  TopoDS_Shape aLocalVertex = mkVertex.Shape();
  
  // Perform transformation
  BRepBuilderAPI_Transform aTransformation (aLocalVertex, aTrans, Standard_False);
  TopoDS_Shape aGlobalVertex = aTransformation.Shape();
  
  // Build prs with vertex in globalCS
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( aGlobalVertex );
  if ( aPrs != 0 && !aPrs->IsNull() )
    GEOMBase_Helper::displayPreview( aPrs, append, update );
  
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
  if (  !GEOMBase::GetShape( theObject, aShape ) ||
       ( aShape.ShapeType() != TopAbs_WIRE && aShape.ShapeType() != TopAbs_VERTEX ) )
    return false;

  if ( ( Group1Sel->isVisible()  && !Group1Sel->buttonApply->isEnabled() )||
       ( Group2Sel->isVisible()  && !Group2Sel->buttonApply->isEnabled() ) ||
       ( Group1Sel1Spin->isVisible()  && !Group1Sel1Spin->buttonApply->isEnabled() ) ||
       ( Group1Spin->isVisible() && !Group1Spin->buttonApply->isEnabled() ) ||
       ( Group2Spin->isVisible() && !Group2Spin->buttonApply->isEnabled() ) ||
       ( Group3Spin->isVisible() && !Group3Spin->buttonApply->isEnabled() ) ||
       ( Group4Spin->isVisible() && !Group4Spin->buttonApply->isEnabled() ) ) {
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
// function : acceptMouseEvent()
// purpose  :
//=================================================================================
bool EntityGUI_SketcherDlg::acceptMouseEvent() const 
{
  return ( (getPnt1ConstructorId() == 1 
            || getPnt1ConstructorId() == 0
            || myConstructorId == 2)                       //accept mouse event only on absolute and relative selection mode                                                        
            && !WPlaneLineEdit->isEnabled());              //or when the constructor is rectangle
                                                           //called by EntityGUI::OnMousePress()        
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
  spinBox->setDecimals( qAbs( aPrecision ) ); // it's necessary to set decimals before the range setting,
                                              // by default Qt rounds boundaries to 2 decimals at setRange
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
  
  // Add a hint for the user saying how to tune precision
  QString userPropName = QObject::tr( QString( "GEOM_PREF_%1" ).arg( quantity ).toLatin1().constData() );
  spinBox->setProperty( "validity_tune_hint", 
                        QVariant( QObject::tr( "GEOM_PRECISION_HINT" ).arg( userPropName ) ) );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_SketcherDlg::SetDoubleSpinBoxStep( double step )
{
  Group1Spin->SpinBox_DX->setSingleStep(step);
  Group1Sel1Spin->SpinBox_DX->setSingleStep(step);
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
// function : AddLocalCS()
// purpose  : Add All Coordinates systems in study
//=================================================================================
void EntityGUI_SketcherDlg::AddLocalCS(GEOM::GEOM_Object_var aSelectedObject)
{
  QString aName = GEOMBase::GetName( aSelectedObject );
  
  int index = ComboBox1->findText(aName, Qt::MatchExactly);
  
  if (index==-1)  // If the working plane hasn't been added yet
  {   
    myWPlaneList.push_back(aSelectedObject);
    myWPlane = aSelectedObject;
    addSubshapesToStudy();
    myLCSList.push_back(WPlaneToLCS(aSelectedObject));
    ComboBox1->addItem(aName); 
    index = ComboBox1->count();
    ComboBox1->setCurrentIndex(index-1);    
  }
  else
  {
    ComboBox1->setCurrentIndex(index);
  }
  ActivateLocalCS();   
}

//=================================================================================
// function : FindLocalCS()
// purpose  : Find All Coordinates systems in study
//=================================================================================
void EntityGUI_SketcherDlg::FindLocalCS()
{
  ComboBox1->clear();
  myWPlaneList.clear();
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
  myWPlaneList.push_back(myGlobalCS);

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
  for ( it->InitEx( true ); it->More(); it->Next() ) 
  {
    _PTR(SObject) child( it->Value() );
    CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject( child );
    GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
    if( CORBA::is_nil( geomObj ) ) 
      continue;
    if (geomObj->GetType() == GEOM_MARKER) 
    {
      myWPlaneList.push_back(geomObj);
      myLCSList.push_back(WPlaneToLCS(geomObj));
      ComboBox1->addItem(geomObj->GetName());
    }
  }
}

//=================================================================================
// function : WPlaneToLCS ( aWPlane )
// purpose  : 
//=================================================================================
gp_Ax3 EntityGUI_SketcherDlg::WPlaneToLCS( GEOM::GEOM_Object_var geomObj )
{
  TopoDS_Shape aShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), geomObj);
  
  gp_Ax3 aLCS;
  if (CORBA::is_nil( geomObj ) || aShape.IsNull())
  {
    MESSAGE("CORBA::is_nil( geomObj ) || aShape.IsNull()")
  }
  aLCS.Transform(aShape.Location().Transformation());
  if (aShape.ShapeType() == TopAbs_FACE) 
  {
    GEOM::GEOM_IMeasureOperations_ptr aMeasureOp =
    myGeometryGUI->GetGeomGen()->GetIMeasureOperations( getStudyId() );
    double Ox, Oy, Oz, Zx, Zy, Zz, Xx, Xy, Xz;
    aMeasureOp->GetPosition( geomObj, Ox, Oy, Oz, Zx, Zy, Zz, Xx, Xy, Xz);
    if ( aMeasureOp->IsDone() )
    {  
      gp_Pnt aPnt ( Ox, Oy, Oz );
      gp_Dir aDirN ( Zx, Zy, Zz );
      gp_Dir aDirX ( Xx, Xy, Xz );
      aLCS = gp_Ax3( aPnt, aDirN, aDirX );
    }
  }
  return aLCS;
}


//=================================================================================
// function : getPnt1ConstructorId()
// purpose  :
//=================================================================================
int EntityGUI_SketcherDlg::getPnt1ConstructorId() const
{ 
  int buttonId = GroupPt->ButtonGroup->checkedId(); 
//   MESSAGE("buttonId = "<<buttonId)
  return buttonId;
}

//=================================================================================
// function : getPnt2ConstructorId()
// purpose  :
//=================================================================================
int EntityGUI_SketcherDlg::getPnt2ConstructorId() const
{ 
  int buttonId = GroupPt2->ButtonGroup->checkedId(); 
//   MESSAGE("buttonId = "<<buttonId)
  return buttonId;
}

//=================================================================================
// function : GetActiveLocalCS()
// purpose  : Get Working plane
//=================================================================================
gp_Ax3 EntityGUI_SketcherDlg::GetActiveLocalCS()
{
  int ind = ComboBox1->currentIndex();
  if (ind == -1)
    return myGeometryGUI->GetWorkingPlane();
  
  gp_Ax3 aLCS = myLCSList.at(ind);
  myWPlane = myWPlaneList.at(ind);

  return aLCS;
}

//=================================================================================
// function : ActivateLocalCS()
// purpose  : Activate & Fit Working plane
//=================================================================================
void EntityGUI_SketcherDlg::ActivateLocalCS()
{
  myGeometryGUI->SetWorkingPlane( GetActiveLocalCS() );
  myGeometryGUI->ActiveWorkingPlane();
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void EntityGUI_SketcherDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject(myWPlane);
}
