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
//  File   : GeometryGUI_ChamferDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_ChamferDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>

#include <Standard_ErrorHandler.hxx> 
#include <Standard_Failure.hxx>

//=================================================================================
// class    : GeometryGUI_ChamferDlg()
// purpose  : Constructs a GeometryGUI_ChamferDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_ChamferDlg::GeometryGUI_ChamferDlg( QWidget* parent,
						const char* name,
						SALOME_Selection* Sel,
						Handle (AIS_InteractiveContext) ic, 
						bool modal,
						WFlags fl )
  : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    /***************************************************************/
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_CHAMFER_ALL")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_CHAMFER_EDGE")));
    QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_CHAMFER_FACE")));

    if ( !name )
	setName( "GeometryGUI_ChamferDlg" );
    resize( 365, 220 ); 
    setCaption( tr( "GEOM_CHAMFER_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_ChamferDlgLayout = new QGridLayout( this ); 
    GeometryGUI_ChamferDlgLayout->setSpacing( 6 );
    GeometryGUI_ChamferDlgLayout->setMargin( 11 );

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
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer, 0, 2 );
    buttonOk = new QPushButton( GroupButtons, "buttonOk" );
    buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    GeometryGUI_ChamferDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_CHAMFER"  ) );
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
    GeometryGUI_ChamferDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_CHAMFER_ALL"  ) );
    GroupC1->setMinimumSize( QSize( 0, 0 ) );
    GroupC1->setFrameShape( QGroupBox::Box );
    GroupC1->setFrameShadow( QGroupBox::Sunken );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );
    
    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_MAIN_OBJECT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );

    TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
    TextLabelC1A2->setText( tr( "GEOM_D1"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );

    TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
    TextLabelC1A3->setText( tr( "GEOM_D2"  ) );
    TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A3->setFrameShape( QLabel::NoFrame );
    TextLabelC1A3->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A3, 2, 0 );
   
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );

//      LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
//      LineEditC1A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A2->sizePolicy().hasHeightForWidth() ) );
//      GroupC1Layout->addWidget( LineEditC1A2, 1, 2 );

//      LineEditC1A3 = new QLineEdit( GroupC1, "LineEditC1A3" );
//      LineEditC1A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A3->sizePolicy().hasHeightForWidth() ) );
//      GroupC1Layout->addWidget( LineEditC1A3, 2, 2 );
    
    SpinBox_C1A2 = new GeometryGUI_SpinBox( GroupC1, "GeomSpinBox_C1A2" ) ;
    SpinBox_C1A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C1A2->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SpinBox_C1A2, 1, 2 );
    
    SpinBox_C1A3 = new GeometryGUI_SpinBox( GroupC1, "GeomSpinBox_C1A3" ) ;
    SpinBox_C1A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C1A3->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SpinBox_C1A3, 2, 2 );

    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    SelectButtonC1A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    GeometryGUI_ChamferDlgLayout->addWidget( GroupC1, 1, 0 );

    /***************************************************************/
    GroupC2 = new QGroupBox( this, "GroupC2" );
    GroupC2->setTitle( tr( "GEOM_CHAMFER_EDGES"  ) );
    GroupC2->setMinimumSize( QSize( 0, 0 ) );
    GroupC2->setFrameShape( QGroupBox::Box );
    GroupC2->setFrameShadow( QGroupBox::Sunken );
    GroupC2->setColumnLayout(0, Qt::Vertical );
    GroupC2->layout()->setSpacing( 0 );
    GroupC2->layout()->setMargin( 0 );
    GroupC2Layout = new QGridLayout( GroupC2->layout() );
    GroupC2Layout->setAlignment( Qt::AlignTop );
    GroupC2Layout->setSpacing( 6 );
    GroupC2Layout->setMargin( 11 );
    
    TextLabelC2A1 = new QLabel( GroupC2, "TextLabelC2A1" );
    TextLabelC2A1->setText( tr( "GEOM_MAIN_OBJECT"  ) );
    TextLabelC2A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A1->setFrameShape( QLabel::NoFrame );
    TextLabelC2A1->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A1, 0, 0 );

    TextLabelC2A2 = new QLabel( GroupC2, "TextLabelC2A2" );
    TextLabelC2A2->setText( tr( "GEOM_D1"  ) );
    TextLabelC2A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A2->setFrameShape( QLabel::NoFrame );
    TextLabelC2A2->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A2, 1, 0 );

    TextLabelC2A3 = new QLabel( GroupC2, "TextLabelC2A3" );
    TextLabelC2A3->setText( tr( "GEOM_D2"  ) );
    TextLabelC2A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A3->setFrameShape( QLabel::NoFrame );
    TextLabelC2A3->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A3, 2, 0 );
   
    LineEditC2A1 = new QLineEdit( GroupC2, "LineEditC2A1" );
    GroupC2Layout->addWidget( LineEditC2A1, 0, 2 );

 //     LineEditC2A2 = new QLineEdit( GroupC2, "LineEditC2A2" );
//      LineEditC2A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC2A2->sizePolicy().hasHeightForWidth() ) );
//      GroupC2Layout->addWidget( LineEditC2A2, 1, 2 );

//      LineEditC2A3 = new QLineEdit( GroupC2, "LineEditC2A3" );
//      LineEditC2A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC2A3->sizePolicy().hasHeightForWidth() ) );
//      GroupC2Layout->addWidget( LineEditC2A3, 2, 2 );

    SpinBox_C2A2 = new GeometryGUI_SpinBox( GroupC2, "GeomSpinBox_C2A2" ) ;
    SpinBox_C2A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A2->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A2, 1, 2 );
    
    SpinBox_C2A3 = new GeometryGUI_SpinBox( GroupC2, "GeomSpinBox_C2A3" ) ;
    SpinBox_C2A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A3->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A3, 2, 2 );

    SelectButtonC2A1 = new QPushButton( GroupC2, "SelectButtonC2A1" );
    SelectButtonC2A1->setText( tr( ""  ) );
    SelectButtonC2A1->setPixmap( image1 );
    SelectButtonC2A1->setToggleButton( FALSE );
    SelectButtonC2A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC2Layout->addWidget( SelectButtonC2A1, 0, 1 );
    GeometryGUI_ChamferDlgLayout->addWidget( GroupC2, 1, 0 );
    
    /***************************************************************/
    GroupC3 = new QGroupBox( this, "GroupC3" );
    GroupC3->setTitle( tr( "GEOM_CHAMFER_FACES"  ) );
    GroupC3->setMinimumSize( QSize( 0, 0 ) );
    GroupC3->setFrameShape( QGroupBox::Box );
    GroupC3->setFrameShadow( QGroupBox::Sunken );
    GroupC3->setColumnLayout(0, Qt::Vertical );
    GroupC3->layout()->setSpacing( 0 );
    GroupC3->layout()->setMargin( 0 );
    GroupC3Layout = new QGridLayout( GroupC3->layout() );
    GroupC3Layout->setAlignment( Qt::AlignTop );
    GroupC3Layout->setSpacing( 6 );
    GroupC3Layout->setMargin( 11 );
    
    TextLabelC3A1 = new QLabel( GroupC3, "TextLabelC3A1" );
    TextLabelC3A1->setText( tr( "GEOM_MAIN_OBJECT"  ) );
    TextLabelC3A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC3A1->setFrameShape( QLabel::NoFrame );
    TextLabelC3A1->setFrameShadow( QLabel::Plain );
    GroupC3Layout->addWidget( TextLabelC3A1, 0, 0 );

    TextLabelC3A2 = new QLabel( GroupC3, "TextLabelC3A2" );
    TextLabelC3A2->setText( tr( "GEOM_D1"  ) );
    TextLabelC3A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC3A2->setFrameShape( QLabel::NoFrame );
    TextLabelC3A2->setFrameShadow( QLabel::Plain );
    GroupC3Layout->addWidget( TextLabelC3A2, 1, 0 );

    TextLabelC3A3 = new QLabel( GroupC3, "TextLabelC3A3" );
    TextLabelC3A3->setText( tr( "GEOM_D2"  ) );
    TextLabelC3A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC3A3->setFrameShape( QLabel::NoFrame );
    TextLabelC3A3->setFrameShadow( QLabel::Plain );
    GroupC3Layout->addWidget( TextLabelC3A3, 2, 0 );
   
    LineEditC3A1 = new QLineEdit( GroupC3, "LineEditC3A1" );
    GroupC3Layout->addWidget( LineEditC3A1, 0, 2 );

 //     LineEditC3A2 = new QLineEdit( GroupC3, "LineEditC3A2" );
//      LineEditC3A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC3A2->sizePolicy().hasHeightForWidth() ) );
//      GroupC3Layout->addWidget( LineEditC3A2, 1, 2 );

//      LineEditC3A3 = new QLineEdit( GroupC3, "LineEditC3A3" );
//      LineEditC3A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC3A3->sizePolicy().hasHeightForWidth() ) );
//      GroupC3Layout->addWidget( LineEditC3A3, 2, 2 );

    SpinBox_C3A2 = new GeometryGUI_SpinBox( GroupC3, "GeomSpinBox_C3A2" ) ;
    SpinBox_C3A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C3A2->sizePolicy().hasHeightForWidth() ) );
    GroupC3Layout->addWidget( SpinBox_C3A2, 1, 2 );
    
    SpinBox_C3A3 = new GeometryGUI_SpinBox( GroupC3, "GeomSpinBox_C3A3" ) ;
    SpinBox_C3A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C3A3->sizePolicy().hasHeightForWidth() ) );
    GroupC3Layout->addWidget( SpinBox_C3A3, 2, 2 );

    SelectButtonC3A1 = new QPushButton( GroupC3, "SelectButtonC3A1" );
    SelectButtonC3A1->setText( tr( ""  ) );
    SelectButtonC3A1->setPixmap( image1 );
    SelectButtonC3A1->setToggleButton( FALSE );
    SelectButtonC3A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC3Layout->addWidget( SelectButtonC3A1, 0, 1 );
    GeometryGUI_ChamferDlgLayout->addWidget( GroupC3, 1, 0 );

    /* Initialisation */
    Init( Sel, ic ) ;
}


//=================================================================================
// function : ~GeometryGUI_ChamferDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_ChamferDlg::~GeometryGUI_ChamferDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
  this->destroy(TRUE, TRUE) ;
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic )
{

  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;
  
  /* min, max, step and decimals for spin boxes */
  SpinBox_C1A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ; /* myD1 */
  SpinBox_C1A2->SetValue( 50 ) ;
  SpinBox_C1A3->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ; /* myD2 */
  SpinBox_C1A3->SetValue( 50 ) ;

  SpinBox_C2A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C2A2->SetValue( 50 ) ;
  SpinBox_C2A3->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C2A3->SetValue( 50 ) ;

  SpinBox_C3A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C3A2->SetValue( 50 ) ;
  SpinBox_C3A3->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C3A3->SetValue( 50 ) ;

  GroupC1->show();
  GroupC2->hide() ;
  GroupC3->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );

  mySelection = Sel ;
  myEditCurrentArgument = LineEditC1A1 ;	
  myShape.Nullify() ;
  myD1 = 50.0 ;
  myOkD1 = true ;
  myD2 = 50.0 ;
  myOkD2 = true ;
  myIC = ic ;
  myUseLocalContext = false ;
  myOkShape = false ;

  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  mySimulationTopoDs.Nullify() ;
  
  /* Filters definition */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonApply, SIGNAL( clicked() ),     this, SLOT( ClickOnApply() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC2A1, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC3A1, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( SpinBox_C1A2, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C2A2, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C3A2, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

  connect( SpinBox_C1A3, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C2A3, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C3A3, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC3A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
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
void GeometryGUI_ChamferDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  myEditCurrentArgument->setText(tr("")) ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(myUseLocalContext ) {
      myIC->CloseLocalContext(this->myLocalContextId);
      myGeomGUI->OnDisplayAll(true) ;
      myUseLocalContext = false ;
    }
  }
    
  myOkShape = false ;
  myD1 = 50.0 ;
  myD2 = 50.0 ;
  myOkD1 = true ;
  myOkD2 = true ;
  myConstructorId = constructorId ;

  switch (constructorId)
    {
    case 0: /* Chamfer All */
      {
	GroupC1->show();
	GroupC2->hide() ;
	GroupC3->hide() ;
	myEditCurrentArgument = LineEditC1A1 ;
	SpinBox_C1A2->SetValue( 50 ) ;
	SpinBox_C1A3->SetValue( 50 ) ;
	LineEditC1A1->setText(tr("")) ;
	myShapeType = -1;
	break;
      }

    case 1: /* Chamfer edges */
      {
	myShapeType = 6;
	GroupC1->hide();
	GroupC2->show() ;
	GroupC3->hide() ;
	myEditCurrentArgument = LineEditC2A1 ;
	SpinBox_C2A2->SetValue( 50 ) ;
	SpinBox_C2A3->SetValue( 50 ) ;
	LineEditC2A1->setText(tr("")) ;
	break ;
      }
      
    case 2: /* Chamfer Faces */
      {
	myShapeType = 4;
	GroupC1->hide();
	GroupC2->hide() ;
	GroupC3->show() ;
	myEditCurrentArgument = LineEditC3A1 ;
	SpinBox_C3A2->SetValue( 50 ) ;
	SpinBox_C3A3->SetValue( 50 ) ;
	LineEditC3A1->setText(tr("")) ;
	break ;
      }
    }
 return ;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  bool testResult = false ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 

  switch(myConstructorId)
    { 
    case 0 : /* Chamfer All */
      {	
	if(myOkD1 && myOkD2) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnChamferGetAll( myShape, myD1, myD2, myShapeType, myShapeIOR ) ;
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	}
	else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
      
    case 1 : /* Chamfer Edge */
      {	
	if(myOkD1 && myOkD2) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnChamferGetSelected( myShape, myShapeIOR, myD1, myD2, myShapeType, 
							 myLocalContextId, myUseLocalContext );
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	}
	else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
      
    case 2 :  /* Chamfer Face */
      {
	if(myOkD1 && myOkD2) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnChamferGetSelected( myShape, myShapeIOR, myD1, myD2, myShapeType, 
							 myLocalContextId, myUseLocalContext ) ;
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	}
	else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
    }

  // accept();
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::ClickOnCancel()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(this->myUseLocalContext ) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }

  reject() ;
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GeometryGUI_ChamferDlg::SelectionIntoArgument()
{
   myGeomGUI->EraseSimulationShape() ; 
   mySimulationTopoDs.Nullify() ;

  /* Reset all arguments and local context when selection as changed */
  this->ResetStateOfDialog() ;

  /* Future name of argument */
  QString aString = "";

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel == 1 ) {

    TopoDS_Shape S ;
    Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
    
    if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
      return ;
    
    if( !IO->hasEntry() ) {
      myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY")) ;
      return ;
    }
	
    if ( !S.IsNull() && S.ShapeType() <= 2 ) {
      if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
	Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
	myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
	myEditCurrentArgument->setText(aString) ;
	myShape = S ;
	myOkShape = true ;
      }
      
      if ( IO->hasEntry() ) {
	SALOMEDS::Study_var aStudy = myGeomGUI->GetActiveStudy()->getStudyDocument();
	SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeIOR_var     anIOR;
	if ( !obj->_is_nil() ) {
	  if (obj->FindAttribute(anAttr, "AttributeIOR")) {
            anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    myShapeIOR = anIOR->Value();
	    myOkShape = true ;
	    myShape = S ;
	    myEditCurrentArgument->setText(aString) ;
	  }
	}
      }
      
      MakePreview();

    }
  } else 
    return;
  
  if( myOkShape && myShapeType!=-1 && myConstructorId != 0 ) {
    /* local context is defined into the method */
    myGeomGUI->PrepareSubShapeSelection( this->myShapeType, this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SELECT_EDGE")) ;
  }  
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else if ( send == LineEditC2A1 )
    myEditCurrentArgument = LineEditC2A1 ; 
  else if ( send == LineEditC3A1 )
    myEditCurrentArgument = LineEditC3A1 ; 
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();  

  switch (myConstructorId)
    {
    case 0:
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	  SelectionIntoArgument() ;
	}
	break;
      }

    case 1:
      {	
	if(send ==SelectButtonC2A1 ) {
	  LineEditC2A1->setFocus() ;
	  myEditCurrentArgument = LineEditC2A1;
	  SelectionIntoArgument() ;
	}
	break;
      }
    
    case 2:
      {
	if(send ==SelectButtonC3A1 ) {
	  LineEditC3A1->setFocus() ;
	  myEditCurrentArgument = LineEditC3A1;
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
void GeometryGUI_ChamferDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();

  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;


  if( send == SpinBox_C1A2 || send == SpinBox_C2A2 || send == SpinBox_C3A2 ) { /* D1 */
    myD1 = newValue ;
    myOkD1 = true ;
    MakePreview();
    return ;
  } 
  if( send == SpinBox_C1A3 || send == SpinBox_C2A3 || send == SpinBox_C3A3 ) { /* D2 */
    myD2 = newValue ;
    myOkD2 = true ;
    MakePreview();
    return ;
  }
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    this->ResetStateOfDialog() ;
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupC2->setEnabled(false) ;
    GroupC3->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->ResetState() ;    
    myGeomGUI->SetActiveDialogBox(0) ;
    myGeomGUI->OnDisplayAll(true) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ; 
  
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupC2->setEnabled(true) ;
  GroupC3->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::enterEvent( QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
  return ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::closeEvent( QCloseEvent* e )
{ 
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  :
//=================================================================================
void GeometryGUI_ChamferDlg::ResetStateOfDialog()
{
  this->myOkShape = false ;
  this->myEditCurrentArgument->setText("") ;

  /* Close its local contact if opened */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(this->myUseLocalContext) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }
  return ;
}

void GeometryGUI_ChamferDlg::MakePreview()
{
  TopoDS_Shape tds ;
  try
  {
  BRepFilletAPI_MakeChamfer MC(myShape);
  switch (myConstructorId)
    {
    case 0: /* Chamfer All */
      {
	TopTools_IndexedDataMapOfShapeListOfShape M;
	TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,M);
	for (int i = 1;i<=M.Extent();i++) 
	  {
	    TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
	    TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
	    if (!BRepTools::IsReallyClosed(E, F) && !BRep_Tool::Degenerated(E))
	      MC.Add(myD1, myD2,E,F);
	  }
	tds = MC.Shape();
	break;
      }
//    case 1: /* Chamfer edges */
//    case 2: /* Chamfer Faces */
    }
   if (!tds.IsNull()) 
    {
      mySimulationTopoDs = tds;
      myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
    }

  }  
  catch(Standard_Failure)
    {
      myGeomGUI->EraseSimulationShape() ; 
      mySimulationTopoDs.Nullify() ;
    }
}
