using namespace std;
//  File      : GeometryGUI_FilletDlg.cxx
//  Created   : 
//  Author    : Damien COQUERET
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : OPEN CASCADE 2002
//  $Header$

#include "GeometryGUI_FilletDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

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

#include <TopExp_Explorer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>

#include <Standard_ErrorHandler.hxx> 
#include <Standard_Failure.hxx>


//=================================================================================
// class    : GeometryGUI_FilletDlg()
// purpose  : Constructs a GeometryGUI_FilletDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_FilletDlg::GeometryGUI_FilletDlg( QWidget* parent,
					      const char* name,
					      SALOME_Selection* Sel,
					      Handle (AIS_InteractiveContext) ic, 
					      bool modal,
					      WFlags fl )
  : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    /***************************************************************/
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_FILLET_ALL")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_FILLET_EDGE")));
    QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_FILLET_FACE")));

    if ( !name )
	setName( "GeometryGUI_FilletDlg" );
    resize( 365, 220 ); 
    setCaption( tr( "GEOM_FILLET_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_FilletDlgLayout = new QGridLayout( this ); 
    GeometryGUI_FilletDlgLayout->setSpacing( 6 );
    GeometryGUI_FilletDlgLayout->setMargin( 11 );

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
    GeometryGUI_FilletDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_FILLET"  ) );
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
    GeometryGUI_FilletDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_FILLET_ALL"  ) );
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
    TextLabelC1A2->setText( tr( "GEOM_RADIUS"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );
   
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );

    SpinBox_C1A2 = new GeometryGUI_SpinBox( GroupC1, "GeomSpinBox_C1A2" ) ;
    SpinBox_C1A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C1A2->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SpinBox_C1A2, 1, 2 );
    
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    SelectButtonC1A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    GeometryGUI_FilletDlgLayout->addWidget( GroupC1, 1, 0 );

    /***************************************************************/
    GroupC2 = new QGroupBox( this, "GroupC2" );
    GroupC2->setTitle( tr( "GEOM_FILLET_EDGES"  ) );
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
    TextLabelC2A2->setText( tr( "GEOM_RADIUS"  ) );
    TextLabelC2A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC2A2->setFrameShape( QLabel::NoFrame );
    TextLabelC2A2->setFrameShadow( QLabel::Plain );
    GroupC2Layout->addWidget( TextLabelC2A2, 1, 0 );
   
    LineEditC2A1 = new QLineEdit( GroupC2, "LineEditC2A1" );
    GroupC2Layout->addWidget( LineEditC2A1, 0, 2 );

    SpinBox_C2A2 = new GeometryGUI_SpinBox( GroupC2, "GeomSpinBox_C2A2" ) ;
    SpinBox_C2A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C2A2->sizePolicy().hasHeightForWidth() ) );
    GroupC2Layout->addWidget( SpinBox_C2A2, 1, 2 );

    SelectButtonC2A1 = new QPushButton( GroupC2, "SelectButtonC2A1" );
    SelectButtonC2A1->setText( tr( ""  ) );
    SelectButtonC2A1->setPixmap( image1 );
    SelectButtonC2A1->setToggleButton( FALSE );
    SelectButtonC2A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC2Layout->addWidget( SelectButtonC2A1, 0, 1 );
    GeometryGUI_FilletDlgLayout->addWidget( GroupC2, 1, 0 );
    
    /***************************************************************/
    GroupC3 = new QGroupBox( this, "GroupC3" );
    GroupC3->setTitle( tr( "GEOM_FILLET_FACES"  ) );
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
    TextLabelC3A2->setText( tr( "GEOM_RADIUS"  ) );
    TextLabelC3A2->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC3A2->setFrameShape( QLabel::NoFrame );
    TextLabelC3A2->setFrameShadow( QLabel::Plain );
    GroupC3Layout->addWidget( TextLabelC3A2, 1, 0 );
   
    LineEditC3A1 = new QLineEdit( GroupC3, "LineEditC3A1" );
    GroupC3Layout->addWidget( LineEditC3A1, 0, 2 );

    SpinBox_C3A2 = new GeometryGUI_SpinBox( GroupC3, "GeomSpinBox_C3A2" ) ;
    SpinBox_C3A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, SpinBox_C3A2->sizePolicy().hasHeightForWidth() ) );
    GroupC3Layout->addWidget( SpinBox_C3A2, 1, 2 );

    SelectButtonC3A1 = new QPushButton( GroupC3, "SelectButtonC3A1" );
    SelectButtonC3A1->setText( tr( ""  ) );
    SelectButtonC3A1->setPixmap( image1 );
    SelectButtonC3A1->setToggleButton( FALSE );
    SelectButtonC3A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC3Layout->addWidget( SelectButtonC3A1, 0, 1 );
    GeometryGUI_FilletDlgLayout->addWidget( GroupC3, 1, 0 );

    /* Initialisation */
    Init( Sel, ic ) ;
}


//=================================================================================
// function : ~GeometryGUI_FilletDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_FilletDlg::~GeometryGUI_FilletDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
  this->destroy(TRUE, TRUE) ;
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_FilletDlg::Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic )
{

  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_C1A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C1A2->SetValue( 50 ) ;
  SpinBox_C2A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C2A2->SetValue( 50 ) ;
  SpinBox_C3A2->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C3A2->SetValue( 50 ) ;

  GroupC1->show();
  GroupC2->hide() ;
  GroupC3->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );

  mySelection = Sel ;
  myEditCurrentArgument = LineEditC1A1 ;	
  myShape.Nullify() ;
  myRadius = 50.0 ;
  myOkRadius = true ;
  myIC = ic ;
  myLocalContextId = -1 ;
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
void GeometryGUI_FilletDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  myEditCurrentArgument->setText(tr("")) ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); //    myIC = myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getViewerOCC()->getAISContext();
    if(this->myUseLocalContext) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      myGeomGUI->OnDisplayAll(true) ;
      this->myUseLocalContext = false ;
    }
  }

  myOkShape = false ;
  myRadius = 50.0 ;
  myOkRadius = true ;
  myConstructorId = constructorId ;

  //  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  switch (constructorId)
    {
    case 0: /* Fillet All */
      {
	myShapeType = -1;
	GroupC1->show();
	GroupC2->hide() ;
	GroupC3->hide() ;
	myEditCurrentArgument = LineEditC1A1 ;	
	SpinBox_C1A2->SetValue( 50 ) ;;
	LineEditC1A1->setText(tr("")) ;
	break;
      }

    case 1: /* Fillet edges */
      {
	myShapeType = 6;
	GroupC1->hide();
	GroupC2->show() ;
	GroupC3->hide() ;
	myEditCurrentArgument = LineEditC2A1 ;
	SpinBox_C2A2->SetValue( 50 ) ;;
	LineEditC2A1->setText(tr("")) ;
	break ;
      }
      
    case 2: /* Fillet Faces */
      {
	myShapeType = 4;
	GroupC1->hide();
	GroupC2->hide() ;
	GroupC3->show() ;
	myEditCurrentArgument = LineEditC3A1 ;
	SpinBox_C3A2->SetValue( 50 ) ;;
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
void GeometryGUI_FilletDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  bool testResult = false ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  switch(myConstructorId)
    { 
    case 0 : /* Fillet All */
      {	
	if(myOkRadius) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnFilletGetAll( myShape, myRadius, myShapeType, myShapeIOR ) ;
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	} else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
      
    case 1 : /* Fillet Edge */
      {	
	if(myOkRadius) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnFilletGetSelected( myShape, myShapeIOR, myRadius, myShapeType, myLocalContextId, myUseLocalContext );
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	} else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
      
    case 2 :  /* Fillet Face */
      {
	if(myOkRadius) {
	  if( myOkShape ) {
	    testResult = myGeomGUI->OnFilletGetSelected( myShape, myShapeIOR, myRadius, myShapeType, myLocalContextId, myUseLocalContext ) ;
	  }
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	} else {
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
void GeometryGUI_FilletDlg::ClickOnCancel()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); //    myIC = myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getViewerOCC()->getAISContext();
    if(this->myUseLocalContext) {
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
void GeometryGUI_FilletDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_FilletDlg::LineEditReturnPressed()
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
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GeometryGUI_FilletDlg::SelectionIntoArgument()
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
	
    if ( !S.IsNull() &&  S.ShapeType() <= 2 ) {
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
    }

    MakePreview();
	
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_FilletDlg::SetEditCurrentArgument()
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
void GeometryGUI_FilletDlg::ValueChangedInSpinBox( double newValue )
{
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  myRadius = newValue ;
  myOkRadius = true ;

  MakePreview();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_FilletDlg::DeactivateActiveDialog()
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
void GeometryGUI_FilletDlg::ActivateThisDialog()
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
void GeometryGUI_FilletDlg::enterEvent( QEvent* e)
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
void GeometryGUI_FilletDlg::closeEvent( QCloseEvent* e )
{ 
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  :
//=================================================================================
void GeometryGUI_FilletDlg::ResetStateOfDialog()
{
  this->myOkShape = false ;
  this->myEditCurrentArgument->setText("") ;

  /* Close its local contact if opened */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); //    myIC = myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getViewerOCC()->getAISContext();
    if(this->myUseLocalContext) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }
  return ;
}


void GeometryGUI_FilletDlg::MakePreview()
{
  TopoDS_Shape tds ;
  try
  {
  BRepFilletAPI_MakeFillet fill(myShape);
  switch (myConstructorId)
    {
    case 0: /* Fillet All */
      {
	TopExp_Explorer Exp ( myShape, TopAbs_EDGE );
	for (Exp; Exp.More(); Exp.Next()) 
	  {
	    TopoDS_Edge E =TopoDS::Edge(Exp.Current());
	    fill.Add(E);
	  }
	for (int i = 1;i<=fill.NbContours();i++) 
	  fill.SetRadius(myRadius,i);
	
	tds = fill.Shape();
      break;
      }
//    case 1: /* Fillet edges */
//    case 2: /* Fillet Faces */
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
