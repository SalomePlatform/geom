using namespace std;
//  File      : GeometryGUI_ArchimedeDlg.cxx
//  Created   : Mon Feb 25 11:13:13 CET 2002
//  Author    : Nicolas REJNERI 
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_ArchimedeDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Tools.h"
#include "utilities.h"

#include "SALOME_InteractiveObject.hxx"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

//=================================================================================
// class    : GeometryGUI_ArchimedeDlg()
// purpose  : Constructs a GeometryGUI_ArchimedeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_ArchimedeDlg::GeometryGUI_ArchimedeDlg( QWidget* parent,  const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
  : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_ARCHIMEDE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
  
  if ( !name )
    setName( "GeometryGUI_ArchimedeDlg" );
  resize( 303, 219 ); 
  setCaption( tr( "GEOM_ARCHIMEDE_TITLE"  ) );
  setSizeGripEnabled( TRUE );
  GeometryGUI_ArchimedeDlgLayout = new QGridLayout( this ); 
  GeometryGUI_ArchimedeDlgLayout->setSpacing( 6 );
  GeometryGUI_ArchimedeDlgLayout->setMargin( 11 );
  
  /***************************************************************/
  GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
  GroupConstructors->setTitle( tr( "GEOM_ARCHIMEDE"  ) );
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
  GeometryGUI_ArchimedeDlgLayout->addWidget( GroupConstructors, 0, 0 );

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
  QSpacerItem* spacer_1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  GroupButtonsLayout->addItem( spacer_1, 0, 2 );
  buttonOk = new QPushButton( GroupButtons, "buttonOk" );
  buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
  GeometryGUI_ArchimedeDlgLayout->addWidget( GroupButtons, 2, 0 );
  
  GroupC1 = new QGroupBox( this, "GroupC1" );
  GroupC1->setTitle( tr( "GEOM_ARGUMENTS"  ) );
  GroupC1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupC1->sizePolicy().hasHeightForWidth() ) );
  GroupC1->setColumnLayout(0, Qt::Vertical );
  GroupC1->layout()->setSpacing( 0 );
  GroupC1->layout()->setMargin( 0 );
  GroupC1Layout = new QGridLayout( GroupC1->layout() );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
  GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );
  
  LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
  LineEditC1A2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A2->sizePolicy().hasHeightForWidth() ) );
  LineEditC1A2->setMinimumSize( QSize( 40, 0 ) );
  LineEditC1A2->setMaximumSize( QSize( 32767, 32767 ) );
  GroupC1Layout->addWidget( LineEditC1A2, 1,  2 );
  
  LineEditC1A3 = new QLineEdit( GroupC1, "LineEditC1A3" );
  LineEditC1A3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A3->sizePolicy().hasHeightForWidth() ) );
  LineEditC1A3->setMinimumSize( QSize( 40, 0 ) );
  LineEditC1A3->setMaximumSize( QSize( 32767, 32767 ) );
  GroupC1Layout->addWidget( LineEditC1A3, 2, 2 );
  
  LineEditC1A4 = new QLineEdit( GroupC1, "LineEditC1A4" );
  LineEditC1A4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A4->sizePolicy().hasHeightForWidth() ) );
  LineEditC1A4->setMinimumSize( QSize( 40, 0 ) );
  LineEditC1A4->setMaximumSize( QSize( 32767, 32767 ) );
  GroupC1Layout->addWidget( LineEditC1A4, 3, 2 );
  
  SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
  SelectButtonC1A1->setText( tr( ""  ) );
  SelectButtonC1A1->setPixmap( image1 );
  GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
  TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
  TextLabelC1A1->setText( tr( "GEOM_OBJECTS"  ) );
  TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC1A1->setFrameShape( QLabel::NoFrame );
  TextLabelC1A1->setFrameShadow( QLabel::Plain );
  GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );
  
  TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
  TextLabelC1A2->setText( tr( "GEOM_WEIGHT"  ) );
  TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC1A2->setFrameShape( QLabel::NoFrame );
  TextLabelC1A2->setFrameShadow( QLabel::Plain );
  GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );
  
  TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
  TextLabelC1A3->setText( tr( "GEOM_WATER_DENSITY"  ) );
  TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC1A3->setFrameShape( QLabel::NoFrame );
  TextLabelC1A3->setFrameShadow( QLabel::Plain );
  GroupC1Layout->addWidget( TextLabelC1A3, 2, 0 );
  
  TextLabelC1A4 = new QLabel( GroupC1, "TextLabelC1A4" );
  TextLabelC1A4->setText( tr( "GEOM_MESHING_DEFLECTION"  ) );
  TextLabelC1A4->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC1A4->setFrameShape( QLabel::NoFrame );
  TextLabelC1A4->setFrameShadow( QLabel::Plain );
  GroupC1Layout->addWidget( TextLabelC1A4, 3, 0 );
  
  GeometryGUI_ArchimedeDlgLayout->addWidget( GroupC1, 1, 0 );
  /***************************************************************/
  
  Init(Sel) ; /* Initialisations */
  
/* Move widget on the botton right corner of main widget */
  QAD_Tools::alignWidget(this, parent, AlignBottom | AlignRight);
  /* Display  Dialog */
  this->show() ; 
}


//=================================================================================
// function : ~GeometryGUI_ArchimedeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_ArchimedeDlg::~GeometryGUI_ArchimedeDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::Init( SALOME_Selection* Sel )
{
  LineEditC1A2->setMaxLength( 10 );
  LineEditC1A3->setMaxLength( 10 );
  LineEditC1A4->setMaxLength( 10 );
  this->myVa = new QDoubleValidator( 0, +999999.999, 3, LineEditC1A2 ) ;
  this->myVb = new QDoubleValidator( 0, +999999.999, 3, LineEditC1A3 ) ;
  this->myVc = new QDoubleValidator( 0, +999999.999, 3, LineEditC1A4 ) ;
  LineEditC1A2->setValidator( myVa ) ;
  LineEditC1A3->setValidator( myVb ) ;
  LineEditC1A4->setValidator( myVc ) ;

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;  

  this->myWeight = 100.0 ;
  LineEditC1A2->setText("100.0") ;
  this->myWaterDensity = 1.0 ;
  LineEditC1A3->setText("1.0") ;
  this->myMeshingDeflection = 0.01 ;
  LineEditC1A4->setText("0.01") ;

  myOkWeight = myOkWaterDensity = myOkMeshingDeflection  = true ;
  myOkIO = false ;

  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),     this, SLOT( ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );

  connect( SelectButtonC1A1, SIGNAL (clicked() ),   this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A2, SIGNAL (textChanged(const QString&) ), this, SLOT( TextChangedInLineEdit(const QString&) ) ) ;
  connect( LineEditC1A3, SIGNAL (textChanged(const QString&) ), this, SLOT( TextChangedInLineEdit(const QString&) ) ) ;
  connect( LineEditC1A4, SIGNAL (textChanged(const QString&) ), this, SLOT( TextChangedInLineEdit(const QString&) ) ) ;

  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ; 
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  return ;
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_ArchimedeDlg::ConstructorsClicked(int constructorId)
{
  return ;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::ClickOnApply()
{
  switch(myConstructorId)
    { 
    case 0 :
      {  
	if(myOkIO && myOkWeight && myOkWaterDensity && myOkMeshingDeflection ) {
	  myGeomGUI->Archimede( myIO, myWeight, myWaterDensity, myMeshingDeflection );
	}
      }
      break ;
    }
  // accept();
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::ClickOnCancel()
{
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_ArchimedeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  QString aString = "";

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    myEditCurrentArgument->setText("") ;
    myOkIO = false ;
  }
  else {
    myEditCurrentArgument->setText(aString) ;
    myIO = mySelection->firstIObject();
    myOkIO = true ;
  }
  return ; 
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	}
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}


//=================================================================================
// function : TextChangedInLineEdit()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::TextChangedInLineEdit(const QString& newText)
{    
  QLineEdit* send = (QLineEdit*)sender();
  QString newT = strdup(newText) ;
  int i ;
  
  if(send == LineEditC1A2) {
    if( myVa->validate(newT, i) == myVa->Acceptable ) { 
      this->myWeight = newText.toFloat() ;
      myOkWeight = true ;
    }
    else {
      myOkWeight = false ;
    }
  }
  else if(send == LineEditC1A3) {
    if( myVb->validate(newT, i) == myVb->Acceptable ) { 
      this->myWaterDensity = newText.toFloat() ;
      myOkWaterDensity = true ;
    }
    else {
      myOkWaterDensity = false ;
    }
  }
  else if(send == LineEditC1A4) {
    if( myVc->validate(newT, i) == myVc->Acceptable ) { 
      this->myMeshingDeflection = newText.toFloat() ;
      myOkMeshingDeflection = true ;
    }
    else {
      myOkMeshingDeflection = false ;
    }
  }
  
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_ArchimedeDlg::enterEvent(QEvent* e)
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
void GeometryGUI_ArchimedeDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


