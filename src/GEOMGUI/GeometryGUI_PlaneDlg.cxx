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
//  File   : GeometryGUI_PlaneDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_PlaneDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <BRepAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>

#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qevent.h>


//=================================================================================
// class    : GeometryGUI_PlaneDlg()
// purpose  : Constructs a GeometryGUI_PlaneDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_PlaneDlg::GeometryGUI_PlaneDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    /***************************************************************/
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_PLANE_PV")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_PLANE_DXYZ")));
    QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_PLANE_FACE")));

    if ( !name )
	setName( "GeometryGUI_PlaneDlg" );
    resize( 365, 220 ); 
    setCaption( tr( "GEOM_PLANE_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_PlaneDlgLayout = new QGridLayout( this ); 
    GeometryGUI_PlaneDlgLayout->setSpacing( 6 );
    GeometryGUI_PlaneDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setGeometry( QRect( 10, 10, 281, 48 ) ); 
    GroupButtons->setTitle( tr( ""  ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 0 );
    GroupButtons->layout()->setMargin( 0 );
    GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );
    GroupButtonsLayout->setSpacing( 6 );
    GroupButtonsLayout->setMargin( 11 );
    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( tr( "GEOM_BUT_CLOSE"  ) );
    buttonCancel->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonCancel, 0, 3 );
    buttonApply = new QPushButton( GroupButtons, "buttonApply" );
    buttonApply->setText( tr( "GEOM_BUT_APPLY"  ) );
    buttonApply->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonApply, 0, 1 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_9, 0, 2 );
    buttonOk = new QPushButton( GroupButtons, "buttonOk" );
    buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    GeometryGUI_PlaneDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_PLANE"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
    Constructor2->setText( tr( ""  ) );
    Constructor2->setPixmap( image2 );
    Constructor2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, Constructor2->sizePolicy().hasHeightForWidth() ) );
    Constructor2->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_3, 0, 1 );
    Constructor3 = new QRadioButton( GroupConstructors, "Constructor3" );
    Constructor3->setText( tr( ""  ) );
    Constructor3->setPixmap( image3 );
    Constructor3->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor3, 0, 4 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_4, 0, 5 );
    GeometryGUI_PlaneDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupPointDirection = new QGroupBox( this, "GroupPointDirection" );
    GroupPointDirection->setTitle( tr( "GEOM_PLANE_PV"  ) );
    GroupPointDirection->setColumnLayout(0, Qt::Vertical );
    GroupPointDirection->layout()->setSpacing( 0 );
    GroupPointDirection->layout()->setMargin( 0 );
    GroupPointDirectionLayout = new QGridLayout( GroupPointDirection->layout() );
    GroupPointDirectionLayout->setAlignment( Qt::AlignTop );
    GroupPointDirectionLayout->setSpacing( 6 );
    GroupPointDirectionLayout->setMargin( 11 );
    LineEditDirection = new QLineEdit( GroupPointDirection, "LineEditDirection" );
    LineEditDirection->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditDirection->sizePolicy().hasHeightForWidth() ) );
    GroupPointDirectionLayout->addWidget( LineEditDirection, 1, 2 );
    LineEditPt1 = new QLineEdit( GroupPointDirection, "LineEditPt1" );
    LineEditPt1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditPt1->sizePolicy().hasHeightForWidth() ) );
    GroupPointDirectionLayout->addWidget( LineEditPt1, 0, 2 );
    SelectButtonPt1 = new QPushButton( GroupPointDirection, "SelectButtonPt1" );
    SelectButtonPt1->setText( tr( ""  ) );
    SelectButtonPt1->setPixmap( image1 );
    GroupPointDirectionLayout->addWidget( SelectButtonPt1, 0, 1 );
    SelectButtonDirection = new QPushButton( GroupPointDirection, "SelectButtonDirection" );
    SelectButtonDirection->setText( tr( ""  ) );
    SelectButtonDirection->setPixmap( image1 );
    GroupPointDirectionLayout->addWidget( SelectButtonDirection, 1, 1 );
    TextLabelDirection = new QLabel( GroupPointDirection, "TextLabelDirection" );
    TextLabelDirection->setText( tr( "GEOM_VECTOR"  ) );
    TextLabelDirection->setMinimumSize( QSize( 50, 0 ) );
    TextLabelDirection->setFrameShape( QLabel::NoFrame );
    TextLabelDirection->setFrameShadow( QLabel::Plain );
    GroupPointDirectionLayout->addWidget( TextLabelDirection, 1, 0 );
    TextLabelPt1 = new QLabel( GroupPointDirection, "TextLabelPt1" );
    TextLabelPt1->setText( tr( "GEOM_POINT"  ) );
    TextLabelPt1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelPt1->setFrameShape( QLabel::NoFrame );
    TextLabelPt1->setFrameShadow( QLabel::Plain );
    GroupPointDirectionLayout->addWidget( TextLabelPt1, 0, 0 );

    SpinBox_C1Size = new GeometryGUI_SpinBox( GroupPointDirection, "SpinBox_C1Size" );
    GroupPointDirectionLayout->addWidget( SpinBox_C1Size, 2, 2 );
    TextLabelC1Size = new QLabel( GroupPointDirection, "TextLabelC1Size" );
    TextLabelC1Size->setText( tr( "GEOM_PLANE_SIZE"  ) );
    TextLabelC1Size->setMinimumSize( QSize( 60, 0 ) );
    GroupPointDirectionLayout->addWidget( TextLabelC1Size, 2, 0 );
    
    GeometryGUI_PlaneDlgLayout->addWidget( GroupPointDirection, 1, 0 );

    /***************************************************************/
    GroupPointPlusCoordinates = new QGroupBox( this, "GroupPointPlusCoordinates" );
    GroupPointPlusCoordinates->setTitle( tr( "GEOM_PLANE_PVC"  ) );
    GroupPointPlusCoordinates->setColumnLayout(0, Qt::Vertical );
    GroupPointPlusCoordinates->layout()->setSpacing( 0 );
    GroupPointPlusCoordinates->layout()->setMargin( 0 );
    GroupPointPlusCoordinatesLayout = new QGridLayout( GroupPointPlusCoordinates->layout() );
    GroupPointPlusCoordinatesLayout->setAlignment( Qt::AlignTop );
    GroupPointPlusCoordinatesLayout->setSpacing( 6 );
    GroupPointPlusCoordinatesLayout->setMargin( 11 );

    LineEditPt2 = new QLineEdit( GroupPointPlusCoordinates, "LineEditPt2" );
    LineEditPt2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditPt2->sizePolicy().hasHeightForWidth() ) );
    GroupPointPlusCoordinatesLayout->addMultiCellWidget( LineEditPt2, 0, 0, 5, 8 );

    SelectButtonPt2 = new QPushButton( GroupPointPlusCoordinates, "SelectButtonPt2" );
    SelectButtonPt2->setText( tr( ""  ) );
    SelectButtonPt2->setPixmap( image1 );
    SelectButtonPt2->setMaximumSize( QSize( 28, 32767 ) );
    GroupPointPlusCoordinatesLayout->addWidget( SelectButtonPt2, 0, 4 );

    TextLabelPt2 = new QLabel( GroupPointPlusCoordinates, "TextLabelPt2" );
    TextLabelPt2->setText( tr( "GEOM_POINT"  ) );
    TextLabelPt2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelPt2->setFrameShape( QLabel::NoFrame );
    TextLabelPt2->setFrameShadow( QLabel::Plain );
    GroupPointPlusCoordinatesLayout->addMultiCellWidget( TextLabelPt2, 0, 0, 0, 3 );

    SpinBox_DX = new GeometryGUI_SpinBox( GroupPointPlusCoordinates, "SpinBox_DX" );
    SpinBox_DX->setMinimumSize( QSize( 50, 0 ) );
    SpinBox_DX->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DX->sizePolicy().hasHeightForWidth() ) );
    GroupPointPlusCoordinatesLayout->addMultiCellWidget( SpinBox_DX, 1, 1, 3, 4 );
    
    SpinBox_DY = new GeometryGUI_SpinBox( GroupPointPlusCoordinates, "SpinBox_DY" );
    SpinBox_DY->setMinimumSize( QSize( 50, 0 ) );
    SpinBox_DY->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DY->sizePolicy().hasHeightForWidth() ) );
    GroupPointPlusCoordinatesLayout->addWidget( SpinBox_DY, 1, 6 );
    
    SpinBox_DZ = new GeometryGUI_SpinBox( GroupPointPlusCoordinates, "SpinBox_DZ" );
    SpinBox_DZ->setMinimumSize( QSize( 50, 0 ) );
    SpinBox_DZ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DZ->sizePolicy().hasHeightForWidth() ) );
    GroupPointPlusCoordinatesLayout->addWidget( SpinBox_DZ, 1, 8 );
    
    TextLabel_DX = new QLabel( GroupPointPlusCoordinates, "TextLabel_DX" );
    TextLabel_DX->setText( tr( "GEOM_DX"  ) );
    GroupPointPlusCoordinatesLayout->addWidget( TextLabel_DX, 1, 2 );

    TextLabel_DY = new QLabel( GroupPointPlusCoordinates, "TextLabel_DY" );
    TextLabel_DY->setText( tr( "GEOM_DY"  ) );
    GroupPointPlusCoordinatesLayout->addWidget( TextLabel_DY, 1, 5 );

    TextLabel_DZ = new QLabel( GroupPointPlusCoordinates, "TextLabel_DZ" );
    TextLabel_DZ->setText( tr( "GEOM_DZ"  ) );
    GroupPointPlusCoordinatesLayout->addWidget( TextLabel_DZ, 1, 7 );

    TextLabelCoordinates = new QLabel( GroupPointPlusCoordinates, "TextLabelCoordinates" );
    TextLabelCoordinates->setText( tr( "GEOM_COOR"  ) );
    TextLabelCoordinates->setMinimumSize( QSize( 50, 0 ) );
    TextLabelCoordinates->setFrameShape( QLabel::NoFrame );
    TextLabelCoordinates->setFrameShadow( QLabel::Plain );
    GroupPointPlusCoordinatesLayout->addWidget( TextLabelCoordinates, 1, 0 );

    TextLabelC2Size = new QLabel( GroupPointPlusCoordinates, "TextLabelC2Size" );
    TextLabelC2Size->setText( tr( "GEOM_PLANE_SIZE"  ) );
    TextLabelC2Size->setMinimumSize( QSize( 60, 0 ) );
    GroupPointPlusCoordinatesLayout->addMultiCellWidget( TextLabelC2Size, 2, 2, 0, 1 );

    SpinBox_C2Size = new GeometryGUI_SpinBox( GroupPointPlusCoordinates, "SpinBox_C2Size" );
    GroupPointPlusCoordinatesLayout->addMultiCellWidget( SpinBox_C2Size, 2, 2, 5, 8 );

    QSpacerItem* spacer_c = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupPointPlusCoordinatesLayout->addMultiCell( spacer_c, 2, 2, 2, 4 );
    QSpacerItem* spacer_d = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupPointPlusCoordinatesLayout->addItem( spacer_d, 1, 1 );

    GeometryGUI_PlaneDlgLayout->addWidget( GroupPointPlusCoordinates, 1, 0 );

    /***************************************************************/

    GroupFace = new QGroupBox( this, "GroupFace" );
    GroupFace->setTitle( tr( "GEOM_FACE"  ) );
    GroupFace->setColumnLayout(0, Qt::Vertical );
    GroupFace->layout()->setSpacing( 0 );
    GroupFace->layout()->setMargin( 0 );
    GroupFaceLayout = new QGridLayout( GroupFace->layout() );
    GroupFaceLayout->setAlignment( Qt::AlignTop );
    GroupFaceLayout->setSpacing( 6 );
    GroupFaceLayout->setMargin( 11 );

    TextLabelFace = new QLabel( GroupFace, "TextLabelFace" );
    TextLabelFace->setFrameShadow( QLabel::Plain );
    TextLabelFace->setFrameShape( QLabel::NoFrame );
    TextLabelFace->setText( tr( "GEOM_SELECTION"  ) );
    TextLabelFace->setMinimumSize( QSize( 50, 0 ) );
    GroupFaceLayout->addMultiCellWidget( TextLabelFace, 0, 0, 0, 1 );

    SelectButtonFace = new QPushButton( GroupFace, "SelectButtonFace" );
    SelectButtonFace->setText( tr( ""  ) );
    SelectButtonFace->setPixmap( image1 );
    SelectButtonFace->setMaximumSize( QSize( 28, 32767 ) );
    GroupFaceLayout->addWidget( SelectButtonFace, 0, 2 );

    LineEditFace = new QLineEdit( GroupFace, "LineEditFace" );
    LineEditFace->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditFace->sizePolicy().hasHeightForWidth() ) );
    GroupFaceLayout->addWidget( LineEditFace, 0, 3 );

    TextLabelC3Size = new QLabel( GroupFace, "TextLabelC3Size" );
    TextLabelC3Size->setText( tr( "GEOM_PLANE_SIZE"  ) );
    TextLabelC3Size->setMinimumSize( QSize( 60, 0 ) );
    GroupFaceLayout->addWidget( TextLabelC3Size, 1, 0 );

    SpinBox_C3Size = new GeometryGUI_SpinBox( GroupFace, "SpinBox_C3Size" );
    SpinBox_C3Size->setCaption( tr( ""  ) );
    GroupFaceLayout->addWidget( SpinBox_C3Size, 1, 3 );

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupFaceLayout->addMultiCell( spacer, 1, 1, 1, 2 );
    QSpacerItem* spacer_e = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupFaceLayout->addItem( spacer_e, 2, 3 );
    GeometryGUI_PlaneDlgLayout->addWidget( GroupFace, 1, 0 );

    /* Initialisation */
    Init( Sel ) ;
}


//=================================================================================
// function : ~GeometryGUI_PlaneDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_PlaneDlg::~GeometryGUI_PlaneDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
  this->destroy(TRUE, TRUE) ;
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::Init( SALOME_Selection* Sel )
{
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;
  
  /* min, max, step and decimals for spin boxes */
  SpinBox_DX->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DX->SetValue( 1.0 ) ;
  SpinBox_DY->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DY->SetValue( 1.0 ) ;
  SpinBox_DZ->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DZ->SetValue( 1.0 ) ; 
  
  this->myTrimSize = 2000.0 ;
  SpinBox_C1Size->RangeStepAndValidator( +0.001, 10000000.0, step, 5 ) ;
  SpinBox_C1Size->SetValue( myTrimSize ) ;
  SpinBox_C2Size->RangeStepAndValidator( +0.001, 10000000.0, step, 5 ) ;
  SpinBox_C2Size->SetValue( myTrimSize ) ;
  SpinBox_C3Size->RangeStepAndValidator( +0.001, 10000000.0, step, 5 ) ;
  SpinBox_C3Size->SetValue( myTrimSize ) ;

  GroupPointDirection->show();
  GroupPointPlusCoordinates->hide() ;
  GroupFace->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );

  mySelection = Sel ;
  myEditCurrentArgument = LineEditPt1 ;	
  mySimulationTopoDs.Nullify() ;
  myPoint1.SetCoord( 0.0, 0.0, 0.0 );
  myOkPoint1 = myOkDirection = myOkCoordinates = myOkPlanarFace = false ;

  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  // TODO previous selection into argument ?
  
  /* Filters definition */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myVertexFilter = new GEOM_ShapeTypeFilter( TopAbs_VERTEX, myGeom );
  myEdgeFilter   = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myGeom );
  myFaceFilter   = new GEOM_FaceFilter( StdSelect_Plane, myGeom );
  /* Filter for the next selection */
  mySelection->AddFilter( myVertexFilter ) ;

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );

  connect( LineEditPt1,        SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditDirection,  SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( LineEditPt2,  SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditFace, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( SelectButtonPt1, SIGNAL (clicked() ),       this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonPt2, SIGNAL (clicked() ),       this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonFace, SIGNAL (clicked() ),      this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonDirection, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( SpinBox_DX, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DY, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DZ, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

  connect( SpinBox_C1Size, SIGNAL ( ValueChangedSignal( const double) ), this, SLOT( ValueChangedInSpinBox( const double) ) ) ;
  connect( SpinBox_C2Size, SIGNAL ( ValueChangedSignal( const double) ), this, SLOT( ValueChangedInSpinBox( const double) ) ) ;
  connect( SpinBox_C3Size, SIGNAL ( ValueChangedSignal( const double) ), this, SLOT( ValueChangedInSpinBox( const double) ) ) ;

  connect( mySelection, SIGNAL( currentSelectionChanged() ),       this, SLOT( SelectionIntoArgument() ) );
  connect( myGeomGUI,   SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* Displays Dialog */ 

  return ;
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_PlaneDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;
  myOkPoint1 = myOkDirection = myOkCoordinates = myOkPlanarFace = false ;
  mySelection->ClearFilters() ;
  myConstructorId = constructorId ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  switch (constructorId)
    {
    case 0: /* plane from a point and a direction (vector, edge...) */
      {
	GroupPointDirection->show();	
	GroupPointPlusCoordinates->hide() ;
	GroupFace->hide() ;
	myEditCurrentArgument = LineEditPt1 ;
	LineEditPt1->setText(tr("")) ;
	LineEditDirection->setText(tr("")) ;

	/* for the first argument */
	mySelection->AddFilter(myVertexFilter) ;
	break;
      }

    case 1: /* plane from a point and vector coordinates */
      {
	GroupPointDirection->hide() ;
	GroupPointPlusCoordinates->show() ;
	GroupFace->hide() ;
	myEditCurrentArgument = LineEditPt2 ;
	LineEditPt2->setText(tr("")) ;
	SpinBox_DX->SetValue( 1.0 ) ;
	SpinBox_DY->SetValue( 1.0 ) ;
	SpinBox_DZ->SetValue( 1.0 ) ;
	myOkCoordinates = true ;

	/* for the first argument */
	mySelection->AddFilter(myVertexFilter) ;
	break ;
      }
      
    case 2: /* plane from a planar face selection */
      {
	GroupPointDirection->hide() ;
	GroupPointPlusCoordinates->hide() ;
	GroupFace->show() ;
	myEditCurrentArgument = LineEditFace ;
	LineEditFace->setText(tr("")) ;

	/* for the first argument */
	mySelection->AddFilter(myFaceFilter) ;
	break ;
      }

    }
 return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  switch(myConstructorId)
    { 
    case 0 : /* args are myPoint1 and myDx, myDy, myDz from a vector(edge) */
      {	
	if(myOkPoint1 && myOkDirection) {
	  myGeomGUI->MakePlaneAndDisplay( myPoint1, myDx, myDy, myDz, myTrimSize ) ;
	}
	break ;
      }
      
    case 1 : /* args are myPoint1 and myDx, myDy, myDz from a Spin Box */
      {	
	if(myOkPoint1) {
	  myGeomGUI->MakePlaneAndDisplay( myPoint1, myDx, myDy, myDz, myTrimSize ) ;
	}
	break ;
      }
      
    case 2 :  /* arg is a planar face selection */
      {
	if(myOkPlanarFace) {
	  myGeomGUI->MakePlaneAndDisplay( myPoint1, myDx, myDy, myDz, myTrimSize) ;
	}
	break ;
      }
    }
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::ClickOnCancel()
{
  mySelection->ClearFilters() ;
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GeometryGUI_PlaneDlg::SelectionIntoArgument()
{
  
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future name of argument */
  QString aString = "";
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    switch (myConstructorId) 
      {
       case 0:
	 {
	   if ( myEditCurrentArgument == LineEditPt1 ) {
	     LineEditPt1->setText("") ;
	     myOkPoint1 = false ;
	   }
	   else if ( myEditCurrentArgument == LineEditDirection ) {
	     LineEditDirection->setText("") ;
	     myOkDirection = false ;
	   }
	   break ;
	 }
      case 1:
	{
	  if ( myEditCurrentArgument == LineEditPt2 ) {
	    LineEditPt2->setText("") ;
	    myOkPoint1 = false ;
	  }
	  break ;
	}

      case 2:
	{
	  if ( myEditCurrentArgument == LineEditFace ) {
	    LineEditFace->setText("") ;
	    if ( aString.compare("") == 0 )
	      myOkPlanarFace = false ;
	    else
	      myOkPlanarFace = true ;
	  }
	  break ;
	}

      }
    return ;
  }

  /*  nbSel == 1  */ 
  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
 
  
  /* FIRST CONSTRUCTOR */
  if ( myEditCurrentArgument == LineEditPt1 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    LineEditPt1->setText(aString) ;
    myOkPoint1 = true ;
  }
  else if ( myEditCurrentArgument == LineEditDirection ) {
    /* We verify if the selection is a linear edge */
    gp_Pnt Pfirst, Plast ;
    if( myGeomGUI->LinearEdgeExtremities(S, Pfirst, Plast ) ) {    
      myGeomGUI->GetBipointDxDyDz( Pfirst, Plast, myDx, myDy, myDz) ;
      LineEditDirection->setText(aString) ;
      myOkDirection = true ;
      this->myTrimSize = SpinBox_C1Size->GetValue() ;
    }
  }
  
  /* SECOND CONSTRUCTOR */
  else if ( myEditCurrentArgument == LineEditPt2 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    LineEditPt2->setText(aString) ;
    /* Get arguments */
    myDx = SpinBox_DX->GetValue() ;
    myDy = SpinBox_DY->GetValue() ;
    myDz = SpinBox_DZ->GetValue() ;
    this->myTrimSize = SpinBox_C2Size->GetValue() ;
    myOkPoint1 = true ;    
    myOkCoordinates = true ;
  }
 
  /* THIRD CONSTRUCTOR */
  else if ( myEditCurrentArgument == LineEditFace) {
    if( myOkPlanarFace ) {
      LineEditFace->setText(aString) ;
      BRepAdaptor_Surface surf(TopoDS::Face(S));
      gp_Pln Plane = surf.Plane();

      gp_Pnt myPoint1 = Plane.Location();
      gp_Ax1 ax = Plane.Axis();
      myDx = (ax.Direction()).X() ;
      myDy = (ax.Direction()).Y() ;
      myDz = (ax.Direction()).Z() ;
      this->myTrimSize = SpinBox_C3Size->GetValue() ;
    }
  }

  /* Call method simulation */    
  if( ( myOkPoint1 && myOkDirection) || ( myOkPoint1 && myOkCoordinates ) || myOkPlanarFace ) {
    if ( myDx*myDx + myDy*myDy + myDz*myDz > Precision::Confusion()*Precision::Confusion() ) {
      MakePlaneSimulationAndDisplay( myPoint1, myDx, myDy, myDz, myTrimSize ) ;
    }
  }
  return ;
}



//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();  
  mySelection->ClearFilters() ;

  switch (myConstructorId)
    {
    case 0:
      {	
	if(send == SelectButtonPt1) {
	  LineEditPt1->setFocus() ;
	  myEditCurrentArgument = LineEditPt1;
	  mySelection->AddFilter(myVertexFilter) ;
	}
	else if(send == SelectButtonDirection) {
	  LineEditDirection->setFocus() ;
	  myEditCurrentArgument = LineEditDirection;	
	  /* Edge filter here */
	  mySelection->AddFilter(myEdgeFilter) ;
	  SelectionIntoArgument() ;
	}	
	break;
      }

    case 1:
      {	
	if(send == SelectButtonPt2) {
	  LineEditPt2->setFocus() ;
	  myEditCurrentArgument = LineEditPt2;
	  /* Vertex filter here */
	  mySelection->AddFilter(myVertexFilter) ;
	  SelectionIntoArgument() ;
	}
	break;
      }
    
    case 2:
      {
	if(send == SelectButtonFace) {
	  LineEditFace->setFocus() ;
	  myEditCurrentArgument = LineEditFace;
	  /* Face filter here */
	  mySelection->AddFilter(myFaceFilter) ;
	  SelectionIntoArgument() ;
	}
	break;
      }
      
    }
  return ;
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender() ; 
  
  if( send == SpinBox_DX ) {
    myDx = newValue ;
    myDy = SpinBox_DY->GetValue() ;
    myDz = SpinBox_DZ->GetValue() ;
  } else if( send == SpinBox_DY ) {
    myDx = SpinBox_DX->GetValue() ;
    myDy = newValue ;
    myDz = SpinBox_DZ->GetValue() ;
  } else if( send == SpinBox_DZ ) {
    myDx = SpinBox_DX->GetValue() ;
    myDy = SpinBox_DY->GetValue() ;
    myDz = newValue ;
  } else if( send == SpinBox_C1Size || send == SpinBox_C2Size || send == SpinBox_C3Size ) {
    myTrimSize = newValue ;
  } else
    return ;
  
  if ( myDx*myDx + myDy*myDy + myDz*myDz > Precision::Confusion() * Precision::Confusion() ) {
    MakePlaneSimulationAndDisplay( myPoint1, myDx, myDy, myDz, myTrimSize ) ;
  }
  else {
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
  }  
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditPt1 )
    myEditCurrentArgument = LineEditPt1 ;
  else if ( send == LineEditDirection )
    myEditCurrentArgument = LineEditDirection ; 
  else if ( send == LineEditPt2 )
    myEditCurrentArgument = LineEditPt2 ; 
  else if ( send == LineEditFace )
    myEditCurrentArgument = LineEditFace ; 
  else
    return ;
  
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text() ;
  QWidget* thisWidget = (QWidget*)this ;
  if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
    myEditCurrentArgument->setText( objectUserName ) ;
  }
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupPointDirection->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    GroupFace->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->EraseSimulationShape() ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ; 
  
  GroupConstructors->setEnabled(true) ;
  GroupPointDirection->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  GroupFace->setEnabled(true) ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::enterEvent( QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::closeEvent( QCloseEvent* e )
{ 
  /* same than click on cancel button */
  this->ClickOnCancel() ;
}



//=================================================================================
// function : MakePlaneSimulationAndDisplay(()
// purpose  :
//=================================================================================
void GeometryGUI_PlaneDlg::MakePlaneSimulationAndDisplay( const gp_Pnt& P1,
							  const Standard_Real dx,
							  const Standard_Real dy, 
							  const Standard_Real dz, 
							  const Standard_Real trimsize )
{
  try {
    gp_Dir aDirection( dx, dy, dz ) ;
    /* We make a trimmed plane */
    gp_Pln gplane(P1, aDirection) ;
    mySimulationTopoDs = BRepBuilderAPI_MakeFace(gplane, -trimsize, +trimsize, -trimsize, +trimsize) ;
  }
  catch(Standard_Failure) {
    MESSAGE( "Exception catched in MakePlaneSimulation" << endl ) ;
    return ;
  }

  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}
