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
//  File   : GeometryGUI_FillingDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_FillingDlg.h"

#include "GeometryGUI.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qvalidator.h>
#include <qimage.h>
#include <qpixmap.h>



//=================================================================================
// class    : GeometryGUI_FillingDlg()
// purpose  : Constructs a GeometryGUI_FillingDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_FillingDlg::GeometryGUI_FillingDlg( QWidget* parent,
						const char* name,
						SALOME_Selection* Sel,
						bool modal,
						WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{

    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_FILLING")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_FillingDlg" );
    resize( 303, 275 ); 
    setCaption( tr( "GEOM_FILLING_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_FillingDlgLayout = new QGridLayout( this ); 
    GeometryGUI_FillingDlgLayout->setSpacing( 6 );
    GeometryGUI_FillingDlgLayout->setMargin( 11 );
    

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_FILLING"  ) );
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
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );

    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );

    GeometryGUI_FillingDlgLayout->addWidget( GroupConstructors, 0, 0 );

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

    GeometryGUI_FillingDlgLayout->addWidget( GroupButtons, 2, 0 );

//      GroupC1 = new QGroupBox( this, "GroupC1" );
//      GroupC1->setTitle( tr( "GEOM_FILLING_ARG"  ) );
//      GroupC1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupC1->sizePolicy().hasHeightForWidth() ) );
//      GroupC1->setColumnLayout(0, Qt::Vertical );
//      GroupC1->layout()->setSpacing( 0 );
//      GroupC1->layout()->setMargin( 0 );
//      GroupC1Layout = new QGridLayout( GroupC1->layout() );
//      GroupC1Layout->setAlignment( Qt::AlignTop );
//      GroupC1Layout->setSpacing( 6 );
//      GroupC1Layout->setMargin( 11 );

//      LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
//      LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );

//      GroupC1Layout->addMultiCellWidget( LineEditC1A1, 0, 0, 3, 5 );

//      TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
//      TextLabelC1A2->setText( tr( "GEOM_FILLING_MIN_DEG"  ) );
//      TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
//      TextLabelC1A2->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A2->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );

//      LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
//      LineEditC1A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A2->sizePolicy().hasHeightForWidth() ) );
//      LineEditC1A2->setMinimumSize( QSize( 40, 0 ) );

//      GroupC1Layout->addMultiCellWidget( LineEditC1A2, 1, 1, 1, 2 );

//      TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
//      TextLabelC1A3->setText( tr( "GEOM_FILLING_MAX_DEG"  ) );
//      TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
//      TextLabelC1A3->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A3->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addWidget( TextLabelC1A3, 1, 4 );

//      TextLabelC1A5 = new QLabel( GroupC1, "TextLabelC1A5" );
//      TextLabelC1A5->setText( tr( "GEOM_FILLING_TOL_2D"  ) );
//      TextLabelC1A5->setMinimumSize( QSize( 50, 0 ) );
//      TextLabelC1A5->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A5->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addWidget( TextLabelC1A5, 2, 4 );

//      LineEditC1A3 = new QLineEdit( GroupC1, "LineEditC1A3" );
//      LineEditC1A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A3->sizePolicy().hasHeightForWidth() ) );
//      LineEditC1A3->setMinimumSize( QSize( 40, 0 ) );

//      GroupC1Layout->addWidget( LineEditC1A3, 1, 5 );

//      LineEditC1A5 = new QLineEdit( GroupC1, "LineEditC1A5" );
//      LineEditC1A5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A5->sizePolicy().hasHeightForWidth() ) );
//      LineEditC1A5->setMinimumSize( QSize( 40, 0 ) );

//      GroupC1Layout->addWidget( LineEditC1A5, 2, 5 );

//      TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
//      TextLabelC1A1->setText( tr( "GEOM_FILLING_COMPOUND"  ) );
//      TextLabelC1A1->setMinimumSize( QSize( 90, 0 ) );
//      TextLabelC1A1->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A1->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addMultiCellWidget( TextLabelC1A1, 0, 0, 0, 1 );

//      SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
//      SelectButtonC1A1->setText( tr( ""  ) );
//      SelectButtonC1A1->setPixmap( image1 );

//      GroupC1Layout->addWidget( SelectButtonC1A1, 0, 2 );

//      TextLabelC1A4 = new QLabel( GroupC1, "TextLabelC1A4" );
//      TextLabelC1A4->setText( tr( "GEOM_FILLING_TOL_3D"  ) );
//      TextLabelC1A4->setMinimumSize( QSize( 50, 0 ) );
//      TextLabelC1A4->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A4->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addWidget( TextLabelC1A4, 2, 0 );

//      TextLabelC1A6 = new QLabel( GroupC1, "TextLabelC1A6" );
//      TextLabelC1A6->setText( tr( "GEOM_FILLING_NB_ITER"  ) );
//      TextLabelC1A6->setMinimumSize( QSize( 50, 0 ) );
//      TextLabelC1A6->setFrameShape( QLabel::NoFrame );
//      TextLabelC1A6->setFrameShadow( QLabel::Plain );

//      GroupC1Layout->addWidget( TextLabelC1A6, 3, 0 );

//      LineEditC1A4 = new QLineEdit( GroupC1, "LineEditC1A4" );
//      LineEditC1A4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A4->sizePolicy().hasHeightForWidth() ) );
//      LineEditC1A4->setMinimumSize( QSize( 40, 0 ) );

//      GroupC1Layout->addMultiCellWidget( LineEditC1A4, 2, 2, 1, 2 );

//      LineEditC1A6 = new QLineEdit( GroupC1, "LineEditC1A6" );
//      LineEditC1A6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A6->sizePolicy().hasHeightForWidth() ) );
//      LineEditC1A6->setMinimumSize( QSize( 40, 0 ) );

//      GroupC1Layout->addMultiCellWidget( LineEditC1A6, 3, 3, 1, 2 );
//      QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//      GroupC1Layout->addItem( spacer_3, 1, 3 );
//      QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//      GroupC1Layout->addItem( spacer_4, 2, 3 );
//      QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
//      GroupC1Layout->addMultiCell( spacer_5, 3, 3, 3, 5 );

//      GeometryGUI_FillingDlgLayout->addWidget( GroupC1, 1, 0 );

    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setMinimumSize( QSize( 0, 0 ) );
    GroupC1->setFrameShape( QGroupBox::Box );
    GroupC1->setFrameShadow( QGroupBox::Sunken );
    GroupC1->setTitle( tr( "GEOM_FILLING_ARG"  ) );
    GroupC1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupC1->sizePolicy().hasHeightForWidth() ) );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );

    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );

    GroupC1Layout->addMultiCellWidget( LineEditC1A1, 0, 0, 3, 6 );

    TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
    TextLabelC1A2->setText( tr( "GEOM_FILLING_MIN_DEG"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );

    TextLabelC1A5 = new QLabel( GroupC1, "TextLabelC1A5" );
    TextLabelC1A5->setText( tr( "GEOM_FILLING_TOL_2D"  ) );
    TextLabelC1A5->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A5->setFrameShape( QLabel::NoFrame );
    TextLabelC1A5->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addWidget( TextLabelC1A5, 2, 5 );

    LineEditC1A5 = new QLineEdit( GroupC1, "LineEditC1A5" );
    LineEditC1A5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A5->sizePolicy().hasHeightForWidth() ) );
    LineEditC1A5->setMinimumSize( QSize( 40, 0 ) );

    GroupC1Layout->addWidget( LineEditC1A5, 2, 6 );

    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_FILLING_COMPOUND"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 90, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addMultiCellWidget( TextLabelC1A1, 0, 0, 0, 1 );

    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, SelectButtonC1A1->sizePolicy().hasHeightForWidth() ) );

    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 2 );

    TextLabelC1A4 = new QLabel( GroupC1, "TextLabelC1A4" );
    TextLabelC1A4->setText( tr( "GEOM_FILLING_TOL_3D" ) );
    TextLabelC1A4->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A4->setFrameShape( QLabel::NoFrame );
    TextLabelC1A4->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addWidget( TextLabelC1A4, 2, 0 );

    TextLabelC1A6 = new QLabel( GroupC1, "TextLabelC1A6" );
    TextLabelC1A6->setText( tr( "GEOM_FILLING_NB_ITER"  ) );
    TextLabelC1A6->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A6->setFrameShape( QLabel::NoFrame );
    TextLabelC1A6->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addWidget( TextLabelC1A6, 3, 0 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupC1Layout->addItem( spacer_3, 1, 3 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupC1Layout->addMultiCell( spacer_4, 2, 2, 3, 4 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupC1Layout->addMultiCell( spacer_5, 3, 3, 3, 6 );

    SpinBox_C1A2 = new QSpinBox( GroupC1, "SpinBox_C1A2" );
    SpinBox_C1A2->setMinValue( 1 );
    SpinBox_C1A2->setMaxValue( 15 );
    SpinBox_C1A2->setWrapping( TRUE );

    GroupC1Layout->addMultiCellWidget( SpinBox_C1A2, 1, 1, 1, 2 );

    LineEditC1A4 = new QLineEdit( GroupC1, "LineEditC1A4" );
    LineEditC1A4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A4->sizePolicy().hasHeightForWidth() ) );
    LineEditC1A4->setMinimumSize( QSize( 40, 0 ) );

    GroupC1Layout->addMultiCellWidget( LineEditC1A4, 2, 2, 1, 2 );

    TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
    TextLabelC1A3->setText( tr( "GEOM_FILLING_MAX_DEG" ) );
    TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A3->setFrameShape( QLabel::NoFrame );
    TextLabelC1A3->setFrameShadow( QLabel::Plain );

    GroupC1Layout->addMultiCellWidget( TextLabelC1A3, 1, 1, 4, 5 );

    SpinBox_C1A3 = new QSpinBox( GroupC1, "SpinBox_C1A3" );
    SpinBox_C1A3->setMinValue( 1 );
    SpinBox_C1A3->setMaxValue( 15 );
    SpinBox_C1A3->setWrapping( TRUE );

    GroupC1Layout->addWidget( SpinBox_C1A3, 1, 6 );

    SpinBox_C1A6 = new QSpinBox( GroupC1, "SpinBox_C1A6" );
    SpinBox_C1A6->setMinValue( 1 );
    SpinBox_C1A6->setMaxValue( 30 );
    SpinBox_C1A6->setWrapping( TRUE );

    GroupC1Layout->addMultiCellWidget( SpinBox_C1A6, 3, 3, 1, 2 );

    GeometryGUI_FillingDlgLayout->addWidget( GroupC1, 1, 0 );

    /* Initialisations */
    Init(Sel) ;
}


//=================================================================================
// function : ~GeometryGUI_FillingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_FillingDlg::~GeometryGUI_FillingDlg()
{
    // no need to delete child widgets, Qt does it all for us
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::Init( SALOME_Selection* Sel )
{

  LineEditC1A4->setMaxLength( 10 ); /* myTol3D  */
  LineEditC1A5->setMaxLength( 10 ); /* myTol2D  */
  QDoubleValidator *Vc = new QDoubleValidator( 0.00001, +10000.0, 3, LineEditC1A4 ) ;
  QDoubleValidator *Vd = new QDoubleValidator( 0.00001, +10000.0, 3, LineEditC1A5 ) ;
  LineEditC1A4->setValidator( Vc) ;
  LineEditC1A5->setValidator( Vd) ;

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  SpinBox_C1A2->setValue(2) ;       /* myMinDeg */
  SpinBox_C1A3->setValue(5) ;       /* myMaxDeg */
  LineEditC1A4->setText("0.0001") ; /* myTol3D  */
  LineEditC1A5->setText("0.0001") ; /* myTol2D  */
  SpinBox_C1A6->setValue(5) ;       /* myNbIter */

  this->myMinDeg = 2 ;
  this->myMaxDeg = 5 ;
  this->myTol3D = 0.0001 ;
  this->myTol2D = 0.0001 ;
  this->myNbIter =  5 ;

  myOkSectionShape = false ;
  mySectionShape.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  // TODO previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myCompoundFilter = new GEOM_ShapeTypeFilter( TopAbs_COMPOUND, myGeom );
  /* first filter used */
  mySelection->AddFilter(myCompoundFilter) ;

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonC1A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1, SIGNAL (returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;  
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  /* Displays Dialog */
  this->show() ;

  return ;
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_FillingDlg::ConstructorsClicked(int constructorId)
{
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::ClickOnApply()
{
  switch(myConstructorId)
    { 
    case 0 :
      { 
	this->myMinDeg = SpinBox_C1A2->value() ;
	this->myMaxDeg = SpinBox_C1A3->value() ;
	this->myTol3D  = LineEditC1A4->text().toDouble(); 
	this->myTol2D  = LineEditC1A5->text().toDouble(); 
	this->myNbIter = SpinBox_C1A6->value() ;
	if(myOkSectionShape) {	  
	  myGeomGUI->MakeFillingAndDisplay( myGeomShape, myMinDeg, myMaxDeg, myTol3D, myTol2D, myNbIter) ;
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
void GeometryGUI_FillingDlg::ClickOnCancel()
{
  mySelection->ClearFilters() ;
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_FillingDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditC1A1 ) {
      myOkSectionShape = false ;
    }
    return ;
  }
  
  // nbSel == 1
  TopoDS_Shape S; 
  Standard_Boolean testResult ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  if ( myEditCurrentArgument == LineEditC1A1 && S.ShapeType() == TopAbs_COMPOUND  ) {
    myEditCurrentArgument->setText(aString) ;
    myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
    if( !testResult )
      return ;
    myOkSectionShape = true ;
  }
  /* no simulation */
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters() ;
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	  mySelection->AddFilter(myCompoundFilter) ;
	}
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}



//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::LineEditReturnPressed()
{  
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
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
void GeometryGUI_FillingDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    mySelection->ClearFilters() ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_FillingDlg::enterEvent(QEvent* e)
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
void GeometryGUI_FillingDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}
