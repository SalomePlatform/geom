using namespace std;
//  File      : GeometryGUI_CutDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_CutDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qframe.h>
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
#include <qevent.h>


//=================================================================================
// class    : GeometryGUI_CutDlg()
// purpose  : Constructs a GeometryGUI_CutDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_CutDlg::GeometryGUI_CutDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_CUT")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

    if ( !name )
      setName( "GeometryGUI_CutDlg" );
    resize( 322, 220 ); 
    setCaption( tr( "GEOM_CUT_TITLE" ) );
    setSizeGripEnabled( TRUE );

    GeometryGUI_CutDlgLayout = new QGridLayout( this ); 
    GeometryGUI_CutDlgLayout->setSpacing( 6 );
    GeometryGUI_CutDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_CUT"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    GeometryGUI_CutDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupConstructor1 = new QGroupBox( this, "GroupConstructor1" );
    GroupConstructor1->setTitle( tr( "GEOM_ARGUMENTS"  ) );
    GroupConstructor1->setColumnLayout(0, Qt::Vertical );
    GroupConstructor1->layout()->setSpacing( 0 );
    GroupConstructor1->layout()->setMargin( 0 );
    GroupConstructor1Layout = new QGridLayout( GroupConstructor1->layout() );
    GroupConstructor1Layout->setAlignment( Qt::AlignTop );
    GroupConstructor1Layout->setSpacing( 6 );
    GroupConstructor1Layout->setMargin( 11 );
    LineEditC1A2Shape = new QLineEdit( GroupConstructor1, "LineEditC1A2Shape" );
    LineEditC1A2Shape->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A2Shape->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addWidget( LineEditC1A2Shape, 1, 2 );
    LineEditC1A1Shape = new QLineEdit( GroupConstructor1, "LineEditC1A1Shape" );
    LineEditC1A1Shape->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1Shape->sizePolicy().hasHeightForWidth() ) );
    GroupConstructor1Layout->addWidget( LineEditC1A1Shape, 0, 2 );
    SelectButtonC1A1Shape = new QPushButton( GroupConstructor1, "SelectButtonC1A1Shape" );
    SelectButtonC1A1Shape->setText( tr( ""  ) );
    SelectButtonC1A1Shape->setPixmap( image1 );
    GroupConstructor1Layout->addWidget( SelectButtonC1A1Shape, 0, 1 );
    SelectButtonC1A2Shape = new QPushButton( GroupConstructor1, "SelectButtonC1A2Shape" );
    SelectButtonC1A2Shape->setText( tr( ""  ) );
    SelectButtonC1A2Shape->setPixmap( image1 );
    GroupConstructor1Layout->addWidget( SelectButtonC1A2Shape, 1, 1 );
    TextLabelC1A2Shape = new QLabel( GroupConstructor1, "TextLabelC1A2Shape" );
    TextLabelC1A2Shape->setText( tr( "GEOM_TOOL_OBJECT"  ) );
    TextLabelC1A2Shape->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A2Shape->setFrameShape( QLabel::NoFrame );
    TextLabelC1A2Shape->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelC1A2Shape, 1, 0 );
    TextLabelC1A1Shape = new QLabel( GroupConstructor1, "TextLabelC1A1Shape" );
    TextLabelC1A1Shape->setText( tr( "GEOM_MAIN_OBJECT"  ) );
    TextLabelC1A1Shape->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1Shape->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1Shape->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabelC1A1Shape, 0, 0 );
    GeometryGUI_CutDlgLayout->addWidget( GroupConstructor1, 1, 0 );

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
    GeometryGUI_CutDlgLayout->addWidget( GroupButtons, 2, 0 ); 

    /* Initialisation */
    Init( Sel ) ; 
}


//=================================================================================
// function : ~GeometryGUI_CutDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_CutDlg::~GeometryGUI_CutDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
  this->destroy(TRUE, TRUE) ;
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::Init( SALOME_Selection* Sel )
{  
  mySelection = Sel ;
  myShape1.Nullify() ;
  myShape2.Nullify() ;
  myConstructorId = 0 ;
  
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  GroupConstructor1->show();
  myConstructorId = 0 ;
  myEditCurrentArgument = LineEditC1A1Shape ;	
  Constructor1->setChecked( TRUE );
  myOkShape1 = myOkShape2 = false ;

  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  
  // TODO previous selection into argument ?

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1Shape, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC1A2Shape, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1Shape, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC1A2Shape, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  
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
void GeometryGUI_CutDlg::ConstructorsClicked(int constructorId)
{
  GeometryGUI::GetGeometryGUI()->EraseSimulationShape() ;

  switch (constructorId)
    {
    case 0:
      {
	GroupConstructor1->show();
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditC1A1Shape ;
	LineEditC1A2Shape->setText(tr("")) ;
	Constructor1->setChecked( TRUE );
	myOkShape1 = myOkShape2 = false ;
	break;
      }
    }
 return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::ClickOnApply()
{
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  switch(myConstructorId)
    { 
    case 0 :
      {
	if(myOkShape1 && myOkShape2) {	  
	  myGeomGUI->MakeBooleanAndDisplay(myGeomShape1 ,myGeomShape2, 2 ) ;
	}
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
void GeometryGUI_CutDlg::ClickOnCancel()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}



//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GeometryGUI_CutDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  QString aString = ""; /* name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    switch (myConstructorId) 
      {
      case 0:
	{
	  if ( myEditCurrentArgument == LineEditC1A1Shape ) {
	    myOkShape1 = false ;
	  }
	  else if ( myEditCurrentArgument == LineEditC1A2Shape ) {
	    myOkShape2 = false ;
	  }
	  break ;
	} 
      }
    return ;
  }

  /*  nbSel == 1  */ 
  TopoDS_Shape S;
  Standard_Boolean testResult ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;

  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;

  if ( myEditCurrentArgument == LineEditC1A1Shape ) {
    myGeomShape1 = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
    if( !testResult )
      return ;
    myShape1 = S ;
    LineEditC1A1Shape->setText(aString) ;
    myOkShape1 = true ;
  }    
  else if ( myEditCurrentArgument == LineEditC1A2Shape ) {
    myGeomShape2 = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
    if( !testResult )
      return ;
    myShape2 = S ;
    LineEditC1A2Shape->setText(aString) ;
    myOkShape2 = true ;
  }

  return ; 
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if( send == SelectButtonC1A1Shape ) {
	  LineEditC1A1Shape->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1Shape ;
	}
	else if(send == SelectButtonC1A2Shape) {
	  LineEditC1A2Shape->setFocus() ;
	  myEditCurrentArgument = LineEditC1A2Shape;
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
void GeometryGUI_CutDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1Shape )
    myEditCurrentArgument = LineEditC1A1Shape ;
  else if ( send == LineEditC1A2Shape )
    myEditCurrentArgument = LineEditC1A2Shape ; 
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
void GeometryGUI_CutDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {

    GroupConstructors->setEnabled(false) ;
    GroupConstructor1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;  
    disconnect( mySelection, 0, this, 0 );
  }
  return ;
}



//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GeometryGUI_CutDlg::enterEvent( QEvent *  )
{
  if ( GroupConstructors->isEnabled() )
    return ;
  ActivateThisDialog() ;
}



//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_CutDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate any active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;
  GroupConstructors->setEnabled(true) ;
  GroupConstructor1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;  
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  return ;
}
