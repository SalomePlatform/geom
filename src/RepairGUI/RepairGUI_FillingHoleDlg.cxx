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
//  File   : GeometryGUI_FillingHoleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_FillingHoleDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

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

//=================================================================================
// class    : GeometryGUI_FillingHoleDlg()
// purpose  : Constructs a  GeometryGUI_FillingHoleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_FillingHoleDlg::GeometryGUI_FillingHoleDlg( QWidget* parent, 
							const char* name,
							SALOME_Selection* Sel,
							Handle (AIS_InteractiveContext) ic,					  
							bool modal,
							WFlags fl )
  : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_SEWING")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
  
  if ( !name )
    setName( "GeometryGUI_FillingHoleDlg" );
  resize( 303, 203 ); 
  setCaption( tr( "Filling hole"  ) );
  setSizeGripEnabled( TRUE );
  GeometryGUI_FillingHoleDlgLayout = new QGridLayout( this ); 
  GeometryGUI_FillingHoleDlgLayout->setSpacing( 6 );
  GeometryGUI_FillingHoleDlgLayout->setMargin( 11 );

  /***************************************************************/
  GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
  GroupConstructors->setTitle( tr( ""  ) );
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
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  GroupConstructorsLayout->addItem( spacer, 0, 1 );
  GeometryGUI_FillingHoleDlgLayout->addWidget( GroupConstructors, 0, 0 );

  /***************************************************************/
  GroupC1 = new QGroupBox( this, "GroupC1" );
  GroupC1->setTitle( tr( ""  ) );
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
  Layout2 = new QHBoxLayout; 
  Layout2->setSpacing( 6 );
  Layout2->setMargin( 0 );
  TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
  TextLabelC1A1->setText( tr( "Main object"  ) );
  TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC1A1->setFrameShape( QLabel::NoFrame );
  TextLabelC1A1->setFrameShadow( QLabel::Plain );
  Layout2->addWidget( TextLabelC1A1 );
  SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
  SelectButtonC1A1->setText( tr( ""  ) );
  SelectButtonC1A1->setPixmap( image1 );
  SelectButtonC1A1->setToggleButton( FALSE );
  SelectButtonC1A1->setMaximumSize( QSize( 28, 32767 ) );
  Layout2->addWidget( SelectButtonC1A1 );
  LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
  LineEditC1A1->setAlignment( int( QLineEdit::AlignLeft ) );
  Layout2->addWidget( LineEditC1A1 );
  GroupC1Layout->addLayout( Layout2, 0, 0 );
  CheckBox1 = new QCheckBox( GroupC1, "CheckBox1" );
  CheckBox1->setText( tr( "Select edges of hole on main object"  ) );
  CheckBox1->setChecked( FALSE );
  GroupC1Layout->addWidget( CheckBox1, 1, 0 );
  GeometryGUI_FillingHoleDlgLayout->addWidget( GroupC1, 1, 0 );

  /***************************************************************/
  GroupButtons = new QGroupBox( this, "GroupButtons" );
  GroupButtons->setTitle( tr( ""  ) );
  GroupButtons->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupButtons->sizePolicy().hasHeightForWidth() ) );
  GroupButtons->setColumnLayout(0, Qt::Vertical );
  GroupButtons->layout()->setSpacing( 0 );
  GroupButtons->layout()->setMargin( 0 );
  GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  buttonClose = new QPushButton( GroupButtons, "buttonClose" );
  buttonClose->setText( tr( "&Close"  ) );
  buttonClose->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonClose, 0, 3 );
  buttonOk = new QPushButton( GroupButtons, "buttonOk" );
  buttonOk->setText( tr( "&Ok"  ) );
  buttonOk->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
  buttonApply = new QPushButton( GroupButtons, "buttonApply" );
  buttonApply->setText( tr( "&Apply"  ) );
  buttonApply->setAutoDefault( TRUE );
  buttonApply->setDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonApply, 0, 1 );
  QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  GroupButtonsLayout->addItem( spacer_2, 0, 2 );
  GeometryGUI_FillingHoleDlgLayout->addWidget( GroupButtons, 2, 0 );
  
  /* Initialisations */
  Init(Sel, ic) ;
  
}


//=================================================================================
// function : ~GeometryGUI_FillingHoleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_FillingHoleDlg::~GeometryGUI_FillingHoleDlg()
{
    // no need to delete child widgets, Qt does it all for us
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_FillingHoleDlg::Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic )
{

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;
  mySelection = Sel;
  myShape.Nullify() ;

  myIC = ic ;
  myUseLocalContext = false ;
  myOkShape = false ;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  /* Select sub shapes mode not checked */
  CheckBox1->setChecked( FALSE );
  myOkSelectSubMode = false ;

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk,          SIGNAL( clicked() ),    this, SLOT( ClickOnOk() ) );
  connect( buttonApply,       SIGNAL( clicked() ),    this, SLOT( ClickOnApply() ) );
  connect( buttonClose,       SIGNAL( clicked() ),    this, SLOT( ClickOnClose() ) );
  connect( GroupConstructors, SIGNAL( clicked(int) ), this, SLOT( ConstructorsClicked(int) ));

  connect( LineEditC1A1,      SIGNAL( returnPressed() ),                this, SLOT( LineEditReturnPressed() ) ) ;
  connect( SelectButtonC1A1,  SIGNAL( clicked() ),                      this, SLOT( SetEditCurrentArgument() ));
  connect( CheckBox1,         SIGNAL( stateChanged(int) ),              this, SLOT( ActivateUserSelection() ));

  connect( mySelection,       SIGNAL( currentSelectionChanged() ),      this, SLOT( SelectionIntoArgument() ));
  connect( myGeomGUI,         SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;  
  connect( myGeomGUI,         SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnClose() ));
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* display Dialog */

  return ;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_FillingHoleDlg::ConstructorsClicked(int constructorId)
{
  return ;
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void GeometryGUI_FillingHoleDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  accept();

  return ;
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_FillingHoleDlg::ClickOnApply()
{
  bool testResult = false ;
  QAD_Application::getDesktop()->putInfo( tr("") ) ; 

  switch(myConstructorId)
    { 
    case 0 :
      {
	if( myOkShape && myOkSelectSubMode ) {
	  testResult = myGeomGUI->OnFillingHole( myShape, myShapeIOR, myLocalContextId, myUseLocalContext ) ; 
	}
	if( !testResult ) {
	  QAD_Application::getDesktop()->putInfo(tr("Operation aborted")) ;
	}
	else {
	  QAD_Application::getDesktop()->putInfo(tr("Operation done"));
	}
	/* Reset arguments to allow a new selection */
	this->ResetStateOfDialog() ;
	break ;
      }
    }
  return ;
}



//=================================================================================
// function : ClickOnClose()
// purpose  :
//=================================================================================
void GeometryGUI_FillingHoleDlg::ClickOnClose()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  
  if(myUseLocalContext) {
    myIC->CloseLocalContext(myLocalContextId) ;
    this->myUseLocalContext = false ;
    myGeomGUI->OnDisplayAll(true) ;
  }
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void GeometryGUI_FillingHoleDlg::SelectionIntoArgument()
{
  
  /* Reset argument and local context when selection as changed */
  this->ResetStateOfDialog() ;
  
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 )
    return ;
  
  /* nbSel == 1 */
  TopoDS_Shape S ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;

  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
//    if( !IO->hasEntry() ) {
//      QAD_Application::getDesktop()->putInfo(tr("Main shape must be in the study before")) ;
//      return ;
//    }
  
  /* Test the exact type of topology to fill an hole */
  if ( !S.IsNull() && ( S.ShapeType() == TopAbs_SOLID || S.ShapeType() == TopAbs_SHELL || S.ShapeType() == TopAbs_COMPOUND ) ) {
    
    if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
      myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
      LineEditC1A1->setText(aString) ;
      myShape = S ;
      myOkShape = true ;
      return;
    } 
    
    if ( IO->hasEntry() ) {
      SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
      SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
      SALOMEDS::GenericAttribute_var anAttr;
      SALOMEDS::AttributeIOR_var     anIOR;
      if ( !obj->_is_nil() ) {
	if (obj->FindAttribute(anAttr, "AttributeIOR")) {
  	  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	  myShapeIOR = anIOR->Value();
	  myOkShape = true ;
	  myShape = S ;
	  LineEditC1A1->setText(aString) ;
	  return;
	}
      }
    }

  }
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_FillingHoleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;	  
	  SelectionIntoArgument() ;
	}
	break;
      }
    }
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_FillingHoleDlg::LineEditReturnPressed()
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
void GeometryGUI_FillingHoleDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
 
   this->ResetStateOfDialog() ;

    disconnect( mySelection, 0, this, 0 );
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
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
void GeometryGUI_FillingHoleDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate other active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void GeometryGUI_FillingHoleDlg::enterEvent(QEvent* e)
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
void GeometryGUI_FillingHoleDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnClose() ;
  return ;
}


//=================================================================================
// function : ActivateUserSelection()
// purpose  : Activate selection of faces when CheckBox1->isChecked()...
//=================================================================================
void GeometryGUI_FillingHoleDlg::ActivateUserSelection()
{
  
  if( !this->myOkShape ) {
    this->ResetStateOfDialog() ;
    QAD_Application::getDesktop()->putInfo(tr("Select main shape first")) ;
    return ;
  }
  
  this->myOkSelectSubMode = CheckBox1->isChecked() ;
  
  if( this->myUseLocalContext ) {
    myIC->CloseLocalContext(myLocalContextId) ;
    this->myUseLocalContext = false ;
    myGeomGUI->OnDisplayAll(true) ;
  }

  if( myOkShape && myOkSelectSubMode ) {
    /* local context is defined into the method : GEOM::EDGE sub selection */
    TopAbs_ShapeEnum aType = TopAbs_EDGE ;
    myGeomGUI->PrepareSubShapeSelection( int(aType), this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    QAD_Application::getDesktop()->putInfo(tr("Select edges to fill an hole and click on Ok/Apply")) ;
  }
  return ;
}



//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void GeometryGUI_FillingHoleDlg::ResetStateOfDialog()
{
  this->myOkShape = false ;
  this->myEditCurrentArgument->setText("") ;

  /* Select sub shapes mode not checked */
  this->myOkSelectSubMode = false ;    
  this->CheckBox1->setChecked( FALSE );

  /* Close its local contact if opened */
  if( this->myUseLocalContext ) {
    myIC->CloseLocalContext(this->myLocalContextId) ;
    this->myUseLocalContext = false ;
    myGeomGUI->OnDisplayAll(true) ;
  }
  return ;
}



