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
//  File   : GeometryGUI_MultiTranslationDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_MultiTranslationDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRep_Builder.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Compound.hxx>

#include <qbuttongroup.h>
#include <qcheckbox.h>
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
#include <qspinbox.h>


//=================================================================================
// class    : GeometryGUI_MultiTranslationDlg()
// purpose  : Constructs a GeometryGUI_MultiTranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_MultiTranslationDlg::GeometryGUI_MultiTranslationDlg( QWidget* parent,  const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_MULTITRANSLATION_SIMPLE")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_MULTITRANSLATION_DOUBLE")));

    if ( !name )
	setName( "GeometryGUI_MultiTranlationDlg" );
    resize( 303, 251 ); 
    setCaption( tr( "GEOM_MULTITRANSLATION_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_MultiTranslationDlgLayout = new QGridLayout( this ); 
    GeometryGUI_MultiTranslationDlgLayout->setSpacing( 6 );
    GeometryGUI_MultiTranslationDlgLayout->setMargin( 11 );

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_MULTITRANSLATION"  ) );
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

    GeometryGUI_MultiTranslationDlgLayout->addWidget( GroupConstructors, 0, 0 );

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
    GeometryGUI_MultiTranslationDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_MULTITRANSLATION_SIMPLE"  ) );
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
    TextLabelC1A2->setText( tr( "GEOM_VECTOR_U"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );

    TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
    TextLabelC1A3->setText( tr( "GEOM_STEP_U"  ) );
    TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A3->setFrameShape( QLabel::NoFrame );
    TextLabelC1A3->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A3, 2, 0 );

    TextLabelC1A4 = new QLabel( GroupC1, "TextLabelC1A4" );
    TextLabelC1A4->setText( tr( "GEOM_NB_TIMES_U"  ) );
    TextLabelC1A4->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A4->setFrameShape( QLabel::NoFrame );
    TextLabelC1A4->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A4, 3, 0 );

    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );

    LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
    GroupC1Layout->addWidget( LineEditC1A2, 1, 2 );

    /* a GeometryGUI_SpinBox */
    SpinBox_C1A3 = new GeometryGUI_SpinBox( GroupC1, "GeomSpinBox_C1A3" ) ;
    SpinBox_C1A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C1A3->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SpinBox_C1A3, 2, 2 );
    /* a QSpinBox            */
    SpinBox_C1A4 = new QSpinBox( GroupC1, "SpinBox_C1A4" ) ;
    SpinBox_C1A4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C1A4->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SpinBox_C1A4, 3, 2 );

    CheckBoxReverse0 = new QCheckBox( GroupC1, "CheckBoxReverse0" );
    CheckBoxReverse0->setText( tr( "GEOM_REVERSE_U"  ) );
    GroupC1Layout->addWidget( CheckBoxReverse0, 5, 0  );

    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    SelectButtonC1A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );

    SelectButtonC1A2 = new QPushButton( GroupC1, "SelectButtonC1A2" );
    SelectButtonC1A2->setText( tr( ""  ) );
    SelectButtonC1A2->setPixmap( image1 );
    SelectButtonC1A2->setToggleButton( FALSE );
    SelectButtonC1A2->setMaximumSize( QSize( 28, 32767 ) );
    GroupC1Layout->addWidget( SelectButtonC1A2, 1, 1 );

    GeometryGUI_MultiTranslationDlgLayout->addWidget( GroupC1, 1, 0 );

    /***************************************************************/

    GroupC2 = new QGroupBox( this, "GroupC2" );
    GroupC2->setTitle( tr( "GEOM_MULTITRANSLATION_DOUBLE"  ) );
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
    TextLabelC2A2->setText( tr( "GEOM_VECTOR_U"  ) );
    TextLabelC2A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A2->setFrameShape( QLabel::NoFrame );
    TextLabelC2A2->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A2, 1, 0 );

    TextLabelC2A3 = new QLabel( GroupC2, "TextLabelC2A3" );
    TextLabelC2A3->setText( tr( "GEOM_VECTOR_V"  ) );
    TextLabelC2A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A3->setFrameShape( QLabel::NoFrame );
    TextLabelC2A3->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A3, 2, 0 );

    TextLabelC2A4 = new QLabel( GroupC2, "TextLabelC2A4" );
    TextLabelC2A4->setText( tr( "GEOM_STEP_U"  ) );
    TextLabelC2A4->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A4->setFrameShape( QLabel::NoFrame );
    TextLabelC2A4->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A4, 3, 0 );

    TextLabelC2A5 = new QLabel( GroupC2, "TextLabelC2A5" );
    TextLabelC2A5->setText( tr( "GEOM_NB_TIMES_U"  ) );
    TextLabelC2A5->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A5->setFrameShape( QLabel::NoFrame );
    TextLabelC2A5->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A5, 4, 0 );

    TextLabelC2A6 = new QLabel( GroupC2, "TextLabelC2A6" );
    TextLabelC2A6->setText( tr( "GEOM_STEP_V"  ) );
    TextLabelC2A6->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A6->setFrameShape( QLabel::NoFrame );
    TextLabelC2A6->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A6, 6, 0 );

    TextLabelC2A7 = new QLabel( GroupC2, "TextLabelC2A7" );
    TextLabelC2A7->setText( tr( "GEOM_NB_TIMES_V"  ) );
    TextLabelC2A7->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A7->setFrameShape( QLabel::NoFrame );
    TextLabelC2A7->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A7, 7, 0 );

    LineEditC2A1 = new QLineEdit( GroupC2, "LineEditC2A1" );
    GroupC2Layout->addWidget( LineEditC2A1, 0, 2 );

    LineEditC2A2 = new QLineEdit( GroupC2, "LineEditC2A2" );
    GroupC2Layout->addWidget( LineEditC2A2, 1, 2 );

    LineEditC2A3 = new QLineEdit( GroupC2, "LineEditC2A3" );
    GroupC2Layout->addWidget( LineEditC2A3, 2, 2 );

    /* a GeometryGUI_SpinBox */
    SpinBox_C2A4 = new GeometryGUI_SpinBox( GroupC2, "GeomSpinBox_C2A4" ) ;
    SpinBox_C2A4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A4->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A4, 3, 2 );
    /* a QSpinBox            */
    SpinBox_C2A5 = new QSpinBox( GroupC2, "SpinBox_C2A5" ) ;
    SpinBox_C2A5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A5->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A5, 4, 2 );

    /* a GeometryGUI_SpinBox */
    SpinBox_C2A6 = new GeometryGUI_SpinBox( GroupC2, "GeomSpinBox_C2A6" ) ;
    SpinBox_C2A6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A6->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A6, 6, 2 );
    /* a QSpinBox            */
    SpinBox_C2A7 = new QSpinBox( GroupC2, "SpinBox_C2A7" ) ;
    SpinBox_C2A7->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A7->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A7, 7, 2 );


    CheckBoxReverse1 = new QCheckBox( GroupC2, "CheckBoxReverse1" );
    CheckBoxReverse1->setText( tr( "GEOM_REVERSE_U"  ) );
    GroupC2Layout->addWidget( CheckBoxReverse1, 5, 0  );

    CheckBoxReverse2 = new QCheckBox( GroupC2, "CheckBoxReverse2" );
    CheckBoxReverse2->setText( tr( "GEOM_REVERSE_V"  ) );
    GroupC2Layout->addWidget( CheckBoxReverse2, 8, 0  );

    SelectButtonC2A1 = new QPushButton( GroupC2, "SelectButtonC2A1" );
    SelectButtonC2A1->setText( tr( ""  ) );
    SelectButtonC2A1->setPixmap( image1 );
    SelectButtonC2A1->setToggleButton( FALSE );
    SelectButtonC2A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC2Layout->addWidget( SelectButtonC2A1, 0, 1 );

    SelectButtonC2A2 = new QPushButton( GroupC2, "SelectButtonC2A2" );
    SelectButtonC2A2->setText( tr( ""  ) );
    SelectButtonC2A2->setPixmap( image1 );
    SelectButtonC2A2->setToggleButton( FALSE );
    SelectButtonC2A2->setMaximumSize( QSize( 28, 32767 ) );
    GroupC2Layout->addWidget( SelectButtonC2A2, 1, 1 );

    SelectButtonC2A3 = new QPushButton( GroupC2, "SelectButtonC2A3" );
    SelectButtonC2A3->setText( tr( ""  ) );
    SelectButtonC2A3->setPixmap( image1 );
    SelectButtonC2A3->setToggleButton( FALSE );
    SelectButtonC2A3->setMaximumSize( QSize( 28, 32767 ) );
    GroupC2Layout->addWidget( SelectButtonC2A3, 2, 1 );

    GeometryGUI_MultiTranslationDlgLayout->addWidget( GroupC2, 1, 0 );

    /***************************************************************/
   

    Init(Sel) ; /* Initialisations */
}

//=================================================================================
// function : ~GeometryGUI_MultiTranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_MultiTranslationDlg::~GeometryGUI_MultiTranslationDlg()
{
    // no need to delete child widgets, Qt does it all for us
  this->destroy(TRUE, TRUE) ;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::Init( SALOME_Selection* Sel )
{
 
  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for geom spin boxes */
  SpinBox_C1A3->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ; /* step U : constructor 1 */
  SpinBox_C1A3->SetValue( 50 ) ;
  SpinBox_C2A4->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ; /* step U : constructor 2 */
  SpinBox_C2A4->SetValue( 50 ) ;
  SpinBox_C2A6->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ; /* step V : constructor 2 */
  SpinBox_C2A6->SetValue( 50 ) ;
  
  /* min, max, step for QT spin boxes */
  SpinBox_C1A4->setMinValue( 1 );    /* myNbTimes1 : constructor 1 */
  SpinBox_C1A4->setMaxValue( 10000 );
  SpinBox_C1A4->setWrapping( TRUE );
  SpinBox_C1A4->setValue(2) ;
  
  SpinBox_C2A5->setMinValue( 1 );    /* myNbTimes1 : constructor 2 */
  SpinBox_C2A5->setMaxValue( 10000 );
  SpinBox_C2A5->setWrapping( TRUE );
  SpinBox_C2A5->setValue(2) ;
  
  SpinBox_C2A7->setMinValue( 1 );    /* myNbTimes2 : constructor 2 */
  SpinBox_C2A7->setMaxValue( 10000 );
  SpinBox_C2A7->setWrapping( TRUE );
  SpinBox_C2A7->setValue(2) ;

  myStep1 = 50.0 ;
  myStep2 = 50.0 ;
  myNbTimes1 = 2 ;
  myNbTimes2 = 2 ;

  GroupC1->show();
  GroupC2->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  myOkBase = myOkDir1 = myOkDir2  = false ;
  mySimulationTopoDs.Nullify() ;
  myBase.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  // TODO : set previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myEdgeFilter = new GEOM_EdgeFilter( StdSelect_Line, myGeom );

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),  this, SLOT(ClickOnApply() ) );

  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonC1A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC1A2, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC2A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC2A2, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC2A3, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  
  /* GeometryGUI_SpinBox */
  connect( SpinBox_C1A3, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C2A4, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C2A6, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  
  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC1A2, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A2, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A3, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  /* QSpinBox */
  connect( SpinBox_C1A4, SIGNAL ( valueChanged(int) ), this, SLOT( ValueChangedInt(int) ) ) ; /* Not const ! */
  connect( SpinBox_C2A5, SIGNAL ( valueChanged(int) ), this, SLOT( ValueChangedInt(int) ) ) ;
  connect( SpinBox_C2A7, SIGNAL ( valueChanged(int) ), this, SLOT( ValueChangedInt(int) ) ) ;
  
  connect( CheckBoxReverse0, SIGNAL (stateChanged(int) ), this, SLOT( ReverseAngle1(int) ) ) ;
  connect( CheckBoxReverse1, SIGNAL (stateChanged(int) ), this, SLOT( ReverseAngle1(int) ) ) ;
  connect( CheckBoxReverse2, SIGNAL (stateChanged(int) ), this, SLOT( ReverseAngle2(int) ) ) ;
  
  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;  
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* displays Dialog */

  return ;
}

//=================================================================================
// function : ReverseAngle1()
// purpose  : 'state' not used here
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ReverseAngle1(int state)
{
  myStep1 = -myStep1 ;
  SpinBox_C1A3->SetValue( myStep1 ) ;
  SpinBox_C2A4->SetValue( myStep1 ) ;
  if( myOkBase && myOkDir1 ) {
    MakeMultiTranslationSimulationAndDisplay() ;
  }
  else {
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
  }
   return ;
}


//=================================================================================
// function : ReverseAngle2()
// purpose  : 'state' not used here
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ReverseAngle2(int state)
{
  myStep2 = -myStep2 ;
  SpinBox_C2A6->SetValue( myStep2 ) ;
  if( myOkBase && myOkDir1 && myOkDir2 ) {
    MakeMultiTranslationSimulationAndDisplay() ;
  }
  else {
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
  }
   return ;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ConstructorsClicked(int constructorId)
{
  myEditCurrentArgument->setText(tr("")) ;
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  myStep1 = 50.0 ;
  myStep2 = 50.0 ;
  myNbTimes1 = 2;
  myNbTimes2 = 2;

  myOkBase = myOkDir1 = myOkDir2  = false ;
  myConstructorId = constructorId ;

  switch (constructorId)
    {
    case 0: /* Translate simple */
      {
	GroupC1->show();
	GroupC2->hide() ;
	myEditCurrentArgument = LineEditC1A1 ;
	SpinBox_C1A3->SetValue(50) ;
	SpinBox_C1A4->setValue(2) ;
	LineEditC1A1->setText(tr("")) ;
	LineEditC1A2->setText(tr("")) ;
	break;
      }

    case 1: /* Translate double */
      {
	GroupC1->hide();
	GroupC2->show() ;
	myEditCurrentArgument = LineEditC2A1 ;
	SpinBox_C2A4->SetValue(50) ;
	SpinBox_C2A5->setValue(2) ;
	SpinBox_C2A6->SetValue(50) ;
	SpinBox_C2A7->setValue(2) ;
	LineEditC2A1->setText(tr("")) ;
	LineEditC2A2->setText(tr("")) ;
	LineEditC2A3->setText(tr("")) ;
	break ;
      }
      
    }

  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  switch(myConstructorId)
    { 
    case 0 :
      { 
	if(myOkBase && myOkDir1 ) {
	  myGeomGUI->MakeMultiTranslation1DAndDisplay( myGeomShape, myDir1, myStep1, myNbTimes1 ) ;
	}
	break ;
      }
    case 1 :
      { 
	if(myOkBase && myOkDir1 && myOkDir2 ) {
	  myGeomGUI->MakeMultiTranslation2DAndDisplay( myGeomShape, myDir1, myStep1, myNbTimes1, myDir2, myStep2, myNbTimes2 ) ;
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
void GeometryGUI_MultiTranslationDlg::ClickOnCancel()
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
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender() ;
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else if ( send == LineEditC1A2 )
    myEditCurrentArgument = LineEditC1A2 ; 
  else if ( send == LineEditC2A1 )
    myEditCurrentArgument = LineEditC2A1;
  else if ( send == LineEditC2A2 )
    myEditCurrentArgument = LineEditC2A2 ; 
  else if ( send == LineEditC2A3 )
    myEditCurrentArgument = LineEditC2A3 ; 
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
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_MultiTranslationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future name of selection */
  QString aString = "";
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;

  TopoDS_Shape S;
  Standard_Boolean testResult ;
  
  switch (myConstructorId)
    {
    case 0 :
      {	
	if ( nbSel != 1 ) {
	  if ( myEditCurrentArgument == LineEditC1A1 ) {
	    myEditCurrentArgument->setText("") ;
	    myOkBase = false ;
	  }
	  else if ( myEditCurrentArgument == LineEditC1A2 ) { 
	    myEditCurrentArgument->setText("") ;
	    myOkDir1 = false ;
	  }
	  return ;
	}

	Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
	if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
	  return ;

	if ( myEditCurrentArgument == LineEditC1A1 ) {
	  myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
	  if( !testResult )
	    return ;
	  myEditCurrentArgument->setText(aString) ;
	  myBase = S ;
	  myOkBase = true ;
	}    
	else if ( myEditCurrentArgument == LineEditC1A2 ) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir1 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString) ;
	  myOkDir1 = true ;
	}
	
	if (myOkBase && myOkDir1 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    case 1 :
      {	
	if ( nbSel != 1 ) {
	  if ( myEditCurrentArgument == LineEditC2A1 ) {
	    myEditCurrentArgument->setText("") ;
	    myOkBase = false ;
	  }
	  else if ( myEditCurrentArgument == LineEditC2A2 ) { 
	    myEditCurrentArgument->setText("") ;
	    myOkDir1 = false ;
	  }
	  else if ( myEditCurrentArgument == LineEditC2A3 ) { 
	    myEditCurrentArgument->setText("") ;
	    myOkDir2 = false ;
	  }
	  return ;
	}

	Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
	if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
	  return ; 

	if ( myEditCurrentArgument == LineEditC2A1 ) {
	  myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
	  if( !testResult )
	    return ;
	  myEditCurrentArgument->setText(aString) ;
	  myBase = S ;
	  myOkBase = true ;
	}    
	else if ( myEditCurrentArgument == LineEditC2A2 ) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir1 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString) ;
	  myOkDir1 = true ;
	}
	else if ( myEditCurrentArgument == LineEditC2A3 ) {
	  BRepAdaptor_Curve curv(TopoDS::Edge(S));
	  myDir2 = curv.Line().Direction();
	  myEditCurrentArgument->setText(aString) ;
	  myOkDir2 = true ;
	}

	if (myOkBase && myOkDir1 && myOkDir2 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    }

  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0 :
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	  mySelection->ClearFilters() ;
	}
	else if(send == SelectButtonC1A2) {
	  LineEditC1A2->setFocus() ;	  
	  myEditCurrentArgument = LineEditC1A2;
	  mySelection->AddFilter(myEdgeFilter) ;
	}	
	SelectionIntoArgument() ;
	break;
      }
    case 1 :
      {	
	if(send == SelectButtonC2A1) {
	  LineEditC2A1->setFocus() ;
	  myEditCurrentArgument = LineEditC2A1;
	  mySelection->ClearFilters() ;
	}
	else if(send == SelectButtonC2A2) {
	  LineEditC2A2->setFocus() ;	  
	  myEditCurrentArgument = LineEditC2A2;
	  mySelection->AddFilter(myEdgeFilter) ;
	}
	else if(send == SelectButtonC2A3) {
	  LineEditC2A3->setFocus() ;	  
	  myEditCurrentArgument = LineEditC2A3;
	  mySelection->AddFilter(myEdgeFilter) ;
	}
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}


//=================================================================================
// function : ValueChangedInt()
// purpose  : (General QT SpinBox)
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ValueChangedInt( int newIntValue )
{
  QSpinBox* send = (QSpinBox*)sender();

  /* First constructor */
  if( send == SpinBox_C1A4 ) {
    myNbTimes1 = newIntValue ;
  }
  /* Second constructor */
  else if( send == SpinBox_C2A5 ) {
    myNbTimes1 = newIntValue ;
  }
  else if( send == SpinBox_C2A7 ) {
    myNbTimes2 = newIntValue ;
  }

  switch (myConstructorId)
    {
    case 0 :
      {	
	if (myOkBase && myOkDir1 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    case 1 :
      {	
	if (myOkBase && myOkDir1 && myOkDir2 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    }
  return ;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  : (specifig for GeometryGUI_SpinBox)
//=================================================================================
void GeometryGUI_MultiTranslationDlg::ValueChangedInSpinBox( double newValue )
{    
  GeometryGUI_SpinBox* send = (GeometryGUI_SpinBox*)sender();

  /* First constructor */
  if( send == SpinBox_C1A3 ) {
    myStep1 = newValue ;
  }
  /* Second constructor */
  else if( send == SpinBox_C2A4 ) {
    myStep1 = newValue ;
  }
  else if( send == SpinBox_C2A6 ) {
    myStep2 = newValue ;
  }
  
  switch (myConstructorId)
    {
    case 0 :
      {	
	if (myOkBase && myOkDir1 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    case 1 :
      {	
	if (myOkBase && myOkDir1 && myOkDir2 ) {
	  MakeMultiTranslationSimulationAndDisplay() ;
	}
	else {
	  myGeomGUI->EraseSimulationShape() ; 
	  mySimulationTopoDs.Nullify() ;
	}
	break;
      }
    }
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupC2->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->EraseSimulationShape() ;
    mySelection->ClearFilters() ;
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
void GeometryGUI_MultiTranslationDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupC2->setEnabled(true) ;
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
void GeometryGUI_MultiTranslationDlg::enterEvent(QEvent* e)
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
void GeometryGUI_MultiTranslationDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
  return ;
}


//=================================================================================
// function : MakeMultiTranslationSimulationAndDisplay()
// purpose  :
//=================================================================================
void GeometryGUI_MultiTranslationDlg::MakeMultiTranslationSimulationAndDisplay() 
{
  myGeomGUI->EraseSimulationShape() ;
  gp_Trsf theTransformation ;
  mySimulationTopoDs.Nullify() ;

  int i ;
  int j ;
  Standard_Real DX ;
  Standard_Real DY ;
  Standard_Real DZ ;
  GProp_GProps System ;
  gp_Pnt myPoint ;
  TopoDS_Compound compound;
  BRep_Builder B;

  B.MakeCompound( compound );  
  TopoDS_Shape S ; 
    
  try {

    BRepGProp::LinearProperties(myBase, System);
    myPoint = System.CentreOfMass() ;
    S = BRepBuilderAPI_MakeVertex(myPoint).Shape();

    switch (myConstructorId)
      {
      case 0 :
	{ 
	  gp_Vec Vec(myDir1) ;
	  Vec.Normalize();

	  for ( i = 0; i < myNbTimes1; i++ ) {
	    DX = i * myStep1 * Vec.X() ;
	    DY = i * myStep1 * Vec.Y() ;
	    DZ = i * myStep1 * Vec.Z() ;
	    myVec.SetCoord( DX, DY, DZ ) ;
	    
	    theTransformation.SetTranslation(myVec) ;
	    BRepBuilderAPI_Transform myBRepTransformation(S, theTransformation, Standard_False) ;
	    B.Add( compound, myBRepTransformation.Shape() );
	  }
	  mySimulationTopoDs = compound;
	  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
	  break;
	}
      case 1 :
	{	
	  gp_Vec Vec1(myDir1) ;
	  Vec1.Normalize();
	  gp_Vec Vec2(myDir2) ;
	  Vec2.Normalize();

	  for ( i = 0; i < myNbTimes1; i++ ) {
	    for ( j = 0; j < myNbTimes2; j++ ) {
	      DX = i * myStep1 * Vec1.X() + j * myStep2 * Vec2.X() ;
	      DY = i * myStep1 * Vec1.Y() + j * myStep2 * Vec2.Y() ;
	      DZ = i * myStep1 * Vec1.Z() + j * myStep2 * Vec2.Z() ;
	      myVec.SetCoord( DX, DY, DZ ) ;
	      
	      theTransformation.SetTranslation(myVec) ;
	      BRepBuilderAPI_Transform myBRepTransformation(S, theTransformation, Standard_False) ;
	      B.Add( compound, myBRepTransformation.Shape() );
	    }
	  }
	  mySimulationTopoDs = compound ;
	  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
	  break;
	}
      }
  }
  catch(Standard_Failure) {
    MESSAGE( "Exception catched in MakeMultitranslationSimulationAndDisplay" ) ;
    return ;
  }
  return ;
}
