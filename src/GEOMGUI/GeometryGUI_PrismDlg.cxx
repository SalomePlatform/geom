using namespace std;
//  File      : GeometryGUI_PrismDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_PrismDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qlabel.h>



//=================================================================================
// class    : GeometryGUI_PrismDlg()
// purpose  : Constructs a GeometryGUI_PrismDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_PrismDlg::GeometryGUI_PrismDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{

    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_PRISM")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_PrismDlg" );
    resize( 303, 225 ); 
    setCaption( tr( "GEOM_PRISM_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_PrismDlgLayout = new QGridLayout( this ); 
    GeometryGUI_PrismDlgLayout->setSpacing( 6 );
    GeometryGUI_PrismDlgLayout->setMargin( 11 );

    /**************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_PRISM"  ) );
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
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 2 );
    GeometryGUI_PrismDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /**************************************************************/
    GroupConstructor1 = new QGroupBox( this, "GroupConstructor1" );
    GroupConstructor1->setTitle( tr( "GEOM_PRISM_BSV"  ) );
    GroupConstructor1->setColumnLayout(0, Qt::Vertical );
    GroupConstructor1->layout()->setSpacing( 0 );
    GroupConstructor1->layout()->setMargin( 0 );
    GroupConstructor1Layout = new QGridLayout( GroupConstructor1->layout() );
    GroupConstructor1Layout->setAlignment( Qt::AlignTop );
    GroupConstructor1Layout->setSpacing( 6 );
    GroupConstructor1Layout->setMargin( 11 );
    LineEditC1A2Line = new QLineEdit( GroupConstructor1, "LineEditC1A2Line" );
    LineEditC1A2Line->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A2Line->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addMultiCellWidget( LineEditC1A2Line, 1, 1, 2, 3 );
    LineEditC1A1Base = new QLineEdit( GroupConstructor1, "LineEditC1A1Base" );
    LineEditC1A1Base->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1Base->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addMultiCellWidget( LineEditC1A1Base, 0, 0, 2, 3 );
    SelectButtonC1A1Base = new QPushButton( GroupConstructor1, "SelectButtonC1A1Base" );
    SelectButtonC1A1Base->setText( tr( ""  ) );
    SelectButtonC1A1Base->setPixmap( image1 );
    GroupConstructor1Layout->addWidget( SelectButtonC1A1Base, 0, 1 );
    SelectButtonC1A2Line = new QPushButton( GroupConstructor1, "SelectButtonC1A2Line" );
    SelectButtonC1A2Line->setText( tr( ""  ) );
    SelectButtonC1A2Line->setPixmap( image1 );
    GroupConstructor1Layout->addWidget( SelectButtonC1A2Line, 1, 1 );
    TextLabelBase = new QLabel( GroupConstructor1, "TextLabelBase" );
    TextLabelBase->setText( tr( "GEOM_BASE"  ) );
    TextLabelBase->setMinimumSize( QSize( 50, 0 ) );
    TextLabelBase->setFrameShape( QLabel::NoFrame );
    TextLabelBase->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelBase, 0, 0 );
    TextLabelVector = new QLabel( GroupConstructor1, "TextLabelVector" );
    TextLabelVector->setText( tr( "GEOM_VECTOR"  ) );
    TextLabelVector->setMinimumSize( QSize( 50, 0 ) );
    TextLabelVector->setFrameShape( QLabel::NoFrame );
    TextLabelVector->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelVector, 1, 0 );

    SpinBox_C1A3Height = new GeometryGUI_SpinBox( GroupConstructor1, "GeomSpinBox_C1A3Height" ) ;
    SpinBox_C1A3Height->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_C1A3Height->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addWidget( SpinBox_C1A3Height, 2, 3 );

    CheckBoxC3A1Reverse = new QCheckBox( GroupConstructor1, "CheckBoxC3A1Reverse" );
    CheckBoxC3A1Reverse->setText( tr( "GEOM_REVERSE"  ) );
    GroupConstructor1Layout->addMultiCellWidget( CheckBoxC3A1Reverse, 2, 2, 0, 1 );
    TextLabelHeight = new QLabel( GroupConstructor1, "TextLabelHeight" );
    TextLabelHeight->setText( tr( "GEOM_HEIGHT"  ) );
    TextLabelHeight->setMinimumSize( QSize( 50, 0 ) );
    TextLabelHeight->setFrameShape( QLabel::NoFrame );
    TextLabelHeight->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelHeight, 2, 2 );
    GeometryGUI_PrismDlgLayout->addWidget( GroupConstructor1, 1, 0 );
    
    /**************************************************************/
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
    GeometryGUI_PrismDlgLayout->addWidget( GroupButtons, 2, 0 );

    /* Initialisation */
    Init( Sel ) ; 
}


//=================================================================================
// function : ~GeometryGUI_PrismDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_PrismDlg::~GeometryGUI_PrismDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::Init( SALOME_Selection* Sel )
{

  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;
  
  /* min, max, step and decimals for spin boxes */
  SpinBox_C1A3Height->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C1A3Height->SetValue( 100.0 ) ;   /* = myHeight */

  myHeight = 100.000 ;
  mySelection = Sel ;
  mySimulationTopoDs.Nullify() ;
  myBaseTopo.Nullify() ;
  myConstructorId = 0 ;
  myDx = myDy = myDz = 0.0 ;

  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  GroupConstructor1->show();
  myConstructorId = 0 ;
  myEditCurrentArgument = LineEditC1A1Base ;
  Constructor1->setChecked( TRUE );
  myOkBase = myOkLine = false ;

  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  
  /* Retrieves geom component */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  /* Filters definition */
  myEdgeFilter = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myGeom );


  // TODO first selection into selection ?

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),  this, SLOT(ClickOnApply() ) );

  connect( GroupConstructors, SIGNAL(clicked(int) ),   this, SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1Base, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC1A2Line, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1Base, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC1A2Line, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( CheckBoxC3A1Reverse, SIGNAL (stateChanged(int) ), this, SLOT( ReverseVector(int) ) ) ;

  connect( SpinBox_C1A3Height, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

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
void GeometryGUI_PrismDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;

  switch (constructorId)
    {
    case 0: /* base shape + an edge used as a vector */
      {
	GroupConstructor1->show();
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditC1A1Base ;
	LineEditC1A2Line->setText(tr("")) ;

	SpinBox_C1A3Height->SetValue(100) ;
	myHeight = 100.0 ;

	Constructor1->setChecked( TRUE );
	myOkBase = myOkLine = false ;
	SelectionIntoArgument() ;
	break;
      }
    }
 return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::ClickOnApply()
{
  gp_Pnt P1, P2 ;

  if( !myOkBase || !myOkLine )
    return ;
  
  try {
    myGeomGUI->EraseSimulationShape() ;
    mySimulationTopoDs.Nullify() ;
    gp_Vec Vec(myDx, myDy, myDz );
    Vec.Normalize() ;
    Vec *= myHeight ;
    P1.SetCoord( 0.0, 0.0, 0.0 ) ;
    P2.SetCoord( Vec.X(), Vec.Y(), Vec.Z() ) ;
    switch(myConstructorId)
      { 
      case 0 :
	{
	  if( myOkBase && myOkLine ) {
	    myGeomGUI->MakePrismAndDisplay( myGeomShape, P1, P2 ) ;
	  }
	  break ;
	}
      }
  }
  catch(Standard_Failure) {
    MESSAGE("Exception intercepted in GeometryGUI_PrismDlg" << endl ) ;
    return ;
  }
  
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::ClickOnCancel()
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
void GeometryGUI_PrismDlg::SelectionIntoArgument()
{
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future the name of selection */
  QString aString = "";

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    switch (myConstructorId) 
      {
      case 0:
	{
	  if ( myEditCurrentArgument == LineEditC1A1Base ) {
	    LineEditC1A1Base->setText("") ;
	    myOkBase = false ;
	  }
	  else if ( myEditCurrentArgument == LineEditC1A2Line ) {
	    LineEditC1A2Line->setText("") ;
	    myOkLine = false ;
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
  
  gp_Pnt aPoint1, aPoint2 ;
  
  if ( myEditCurrentArgument == LineEditC1A1Base ) {
    Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
    Standard_Boolean testResult ;
    myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
    if( !testResult)
      return ;
    if ( S.ShapeType() <= 2 )
      return;

    LineEditC1A1Base->setText(aString) ;
    myBaseTopo = S ;
    myOkBase = true ;
  }    

  else if ( myEditCurrentArgument == LineEditC1A2Line && myGeomGUI->LinearEdgeExtremities(S, aPoint1, aPoint2) ) {
    myGeomGUI->GetBipointDxDyDz( aPoint1, aPoint2, myDx, myDy, myDz ) ;
    myEditCurrentArgument->setText(aString) ;
    myOkLine = true ;
  }

  if( myOkBase && myOkLine ) {
    MakePrismSimulationAndDisplay( myBaseTopo ) ;
  }  
  return ; 
}



//=================================================================================
// function : MakePrismSimulationAndDisplay()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::MakePrismSimulationAndDisplay( const TopoDS_Shape& S )
{
  try {
    gp_Vec Vec(myDx, myDy, myDz );
    Vec.Normalize() ;
    Vec *= myHeight ;
    mySimulationTopoDs = BRepPrimAPI_MakePrism(S, Vec, Standard_False).Shape() ;
  }
  catch(Standard_Failure) {
    MESSAGE( "Exception catched in MakePrismSimulationAndDisplay" << endl ) ;
    return ;
  }
  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}




//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();  
  mySelection->ClearFilters() ;

  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if( send == SelectButtonC1A1Base ) {
	  LineEditC1A1Base->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1Base ;
	}
	else if(send == SelectButtonC1A2Line) {
	  LineEditC1A2Line->setFocus() ;
	  myEditCurrentArgument = LineEditC1A2Line;
	  mySelection->AddFilter(myEdgeFilter) ;
	}	
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();
  
  if( send == SpinBox_C1A3Height ) {
    myHeight = newValue ;
    if( myOkBase && myOkLine ) {
      myGeomGUI->EraseSimulationShape() ;
      mySimulationTopoDs.Nullify() ;
      MakePrismSimulationAndDisplay( myBaseTopo ) ;
    }
  }
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1Base )
    myEditCurrentArgument = LineEditC1A1Base ;
  else if ( send == LineEditC1A2Line )
    myEditCurrentArgument = LineEditC1A2Line ; 
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
void GeometryGUI_PrismDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupConstructor1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;  
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->EraseSimulationShape() ;
  }
  return ;
}



//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GeometryGUI_PrismDlg::enterEvent( QEvent *  )
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}



//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_PrismDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate any active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;
  GroupConstructors->setEnabled(true) ;
  GroupConstructor1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;

  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : ReverseVector()
// purpose  : 'state' not used here
//=================================================================================
void GeometryGUI_PrismDlg::ReverseVector(int state)
{
  myDx = -myDx ;
  myDy = -myDy ;
  myDz = -myDz ;
  if(myOkBase && myOkLine) {
    myGeomGUI->EraseSimulationShape() ;
    mySimulationTopoDs.Nullify() ;
    MakePrismSimulationAndDisplay( myBaseTopo ) ;
  }
  return ;
} 
