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
//  File   : GeometryGUI_SphereDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_SphereDlg.h"
#include "GeometryGUI_SpinBox.h"

#include "GeometryGUI.h"

#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <qbuttongroup.h>
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
#include <qvalidator.h>
#include <qpixmap.h>



//=================================================================================
// class    : GeometryGUI_SphereDlg()
// purpose  : Constructs a GeometryGUI_SphereDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_SphereDlg::GeometryGUI_SphereDlg( QWidget* parent,  const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_SPHERE_P")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_SPHERE_DXYZ")));

    if ( !name )
      setName( "GeometryGUI_SphereDlg" );
    resize( 303, 219 ); 
    setCaption( tr( "GEOM_SPHERE_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_SphereDlgLayout = new QGridLayout( this ); 
    GeometryGUI_SphereDlgLayout->setSpacing( 6 );
    GeometryGUI_SphereDlgLayout->setMargin( 11 );
    
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
    GeometryGUI_SphereDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_SPHERE"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 3 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_3, 0, 1 );
    Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
    Constructor2->setText( tr( ""  ) );
    GroupConstructors->insert( Constructor2, 1 );
    Constructor2->setMinimumSize( QSize( 50, 0 ) );
    Constructor2->setPixmap( image2 );
    GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );
    GeometryGUI_SphereDlgLayout->addWidget( GroupConstructors, 0, 0 );
    GroupConstructor1 = new QGroupBox( this, "GroupConstructor1" );
    GroupConstructor1->setTitle( tr( "GEOM_SPHERE_CR"  ) );
    GroupConstructor1->setColumnLayout(0, Qt::Vertical );
    GroupConstructor1->layout()->setSpacing( 0 );
    GroupConstructor1->layout()->setMargin( 0 );
    GroupConstructor1Layout = new QGridLayout( GroupConstructor1->layout() );
    GroupConstructor1Layout->setAlignment( Qt::AlignTop );
    GroupConstructor1Layout->setSpacing( 6 );
    GroupConstructor1Layout->setMargin( 11 );
    LineEditC1A1 = new QLineEdit( GroupConstructor1, "LineEditC1A1" );
    LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addWidget( LineEditC1A1, 0, 2 );
    SelectButtonC1A1 = new QPushButton( GroupConstructor1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    GroupConstructor1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    TextLabelC1A1 = new QLabel( GroupConstructor1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_CENTER"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelC1A1, 0, 0 );
    TextLabelC1A2 = new QLabel( GroupConstructor1, "TextLabelC1A2" );
    TextLabelC1A2->setText( tr( "GEOM_RADIUS"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelC1A2, 1, 0 );

    SpinBox_C1A2 = new GeometryGUI_SpinBox( GroupConstructor1, "GeomSpinBox_C1A2" ) ;
    GroupConstructor1Layout->addWidget( SpinBox_C1A2, 1, 2 );

    GeometryGUI_SphereDlgLayout->addWidget( GroupConstructor1, 1, 0 );

    /***************************************************************/
    GroupConstructor2 = new QGroupBox( this, "GroupConstructor2" );
    GroupConstructor2->setTitle( tr( "GEOM_SPHERE_RO"  ) );
    GroupConstructor2->setColumnLayout(0, Qt::Vertical );
    GroupConstructor2->layout()->setSpacing( 0 );
    GroupConstructor2->layout()->setMargin( 0 );
    GroupConstructor2Layout = new QGridLayout( GroupConstructor2->layout() );
    GroupConstructor2Layout->setAlignment( Qt::AlignTop );
    GroupConstructor2Layout->setSpacing( 6 );
    GroupConstructor2Layout->setMargin( 11 );

    SpinBox_C2A1 = new GeometryGUI_SpinBox( GroupConstructor2, "GeomSpinBox_C2A1" ) ;
    GroupConstructor2Layout->addWidget( SpinBox_C2A1, 0, 1 );

    TextLabelC2A1 = new QLabel( GroupConstructor2, "TextLabelC2A1" );
    TextLabelC2A1->setText( tr( "GEOM_RADIUS" ) );
    TextLabelC2A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A1->setFrameShape( QLabel::NoFrame );
    TextLabelC2A1->setFrameShadow( QLabel::Plain );
    GroupConstructor2Layout->addWidget( TextLabelC2A1, 0, 0 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupConstructor2Layout->addItem( spacer_5, 1, 1 );
    GeometryGUI_SphereDlgLayout->addWidget( GroupConstructor2, 1, 0 );

    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_SphereDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_SphereDlg::~GeometryGUI_SphereDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::Init( SALOME_Selection* Sel )
{

  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_C1A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C1A2->SetValue( 100.0 ) ;  
  SpinBox_C2A1->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C2A1->SetValue( 100.0 ) ;
  
  GroupConstructor1->show();
  GroupConstructor2->hide();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myPoint1.SetCoord( 0.0, 0.0, 0.0 );
  
  myRadius = 100.0 ;
  myOkRadius = true ;
  myOkPoint1 = false ;
  
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;  
  
  /* Filters definition */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myVertexFilter = new GEOM_ShapeTypeFilter( TopAbs_VERTEX, myGeom );
  mySelection->AddFilter(myVertexFilter) ; /* first filter used */

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),  this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonC1A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  
  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( SpinBox_C1A2, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_C2A1, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

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
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_SphereDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;

  switch (constructorId)
    {
    case 0:
      {
	GroupConstructor1->show();
	GroupConstructor2->hide();
	myConstructorId = constructorId ;
	myEditCurrentArgument = SpinBox_C1A2 ;	
	LineEditC1A1->setText(tr("")) ;
	SpinBox_C1A2->SetValue( 100.0 ) ;
	myRadius = 100.0 ;
	myOkRadius = true ;
	myOkPoint1 = false ;
	/* filter for next selections */
	mySelection->ClearFilters() ;
	mySelection->AddFilter( myVertexFilter );
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	break;
      }
    case 1:
      {
	GroupConstructor1->hide();
	GroupConstructor2->show();
	myConstructorId = constructorId ;
	myEditCurrentArgument = SpinBox_C2A1 ;;
	SpinBox_C2A1->SetValue( 100.0 ) ;
	myRadius = 100.0 ;
	myOkRadius = true ;
	myPoint1.SetCoord( 0.0, 0.0, 0.0 ); /* at origin */
	myOkPoint1 = false ;
	mySimulationTopoDs = BRepPrimAPI_MakeSphere(myPoint1, myRadius).Shape();
	myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
	/* no filters here */
	mySelection->ClearFilters() ;
	break;
      }
    }
 return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  switch(myConstructorId)
    { 
    case 0 :
      { 
	if( myOkPoint1 && myOkRadius ) {
	  myGeomGUI->MakeSphereAndDisplay( myPoint1, myRadius ) ;
	}
	break ;
      }
    case 1 :
      {
	if( myOkRadius )
	  myGeomGUI->MakeSphereAndDisplay( myPoint1, myRadius ) ;
	break ;
      }
    }
  return ;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::ClickOnCancel()
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
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_SphereDlg::SelectionIntoArgument()
{
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;
  
  /* Future name of selection */
  QString aString = "";

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditC1A1 ) {
      LineEditC1A1->setText("") ;
      myOkPoint1 = false ;
    }
    return ;
  }
  
  /* nbSel == 1 ! */
  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
    
  /* Constructor 1 treatment */
  if ( myConstructorId == 0 && myEditCurrentArgument == LineEditC1A1 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    LineEditC1A1->setText(aString) ;
    myOkPoint1 = true ;
  }
  
  if( ( myOkPoint1 || myConstructorId == 1 ) && myOkRadius )  {
    mySimulationTopoDs = BRepPrimAPI_MakeSphere(myPoint1, myRadius).Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else
    return ;
  
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  QLineEdit* LE = (QLineEdit*)myEditCurrentArgument ;
  const QString objectUserName = LE->text() ;
  QWidget* thisWidget = (QWidget*)this ;
  if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
    LE->setText( objectUserName ) ;
  }
  return ;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::ValueChangedInSpinBox( double newValue )
{  
  myRadius = newValue ;
  myOkRadius = true ;
  
  if ( ( myOkPoint1 || myConstructorId == 1 ) && myOkRadius ) {
    mySimulationTopoDs = BRepPrimAPI_MakeSphere(myPoint1, myRadius).Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  else {
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
  }

  return ;
}



//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	  mySelection->AddFilter(myVertexFilter) ;
	  SelectionIntoArgument() ;
	}
	break;
      }
    case 1:
      {
	/* no selection button here */
	break;
      }

    }
  return ;
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {

    GroupConstructors->setEnabled(false) ;
    GroupConstructor1->setEnabled(false) ;
    GroupConstructor2->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->EraseSimulationShape() ;
    mySelection->ClearFilters() ;
  }
  return ;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate other active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;
  GroupConstructors->setEnabled(true) ;
  GroupConstructor1->setEnabled(true) ;
  GroupConstructor2->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;

  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;

  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_SphereDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}



