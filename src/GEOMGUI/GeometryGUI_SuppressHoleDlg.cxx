using namespace std;
//  File      : GeometryGUI_SuppressHoleDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_SuppressHoleDlg.h"
#include "GeometryGUI.h"

#include "TopExp_Explorer.hxx"

#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "utilities.h"

#include <qapplication.h>
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
// class    : GeometryGUI_SuppressHoleDlg()
// purpose  : Constructs a GeometryGUI_SuppressHoleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_SuppressHoleDlg::GeometryGUI_SuppressHoleDlg( QWidget* parent, 
							  const char* name,
							  SALOME_Selection* Sel,
							  Handle (AIS_InteractiveContext) ic,					  
							  bool modal,
							  WFlags fl )
  : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_SUPRESS_HOLE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_SUPRESS_HOLE_FACE_SHELL")));

  if ( !name )
    setName( "GeometryGUI_SuppressHoleDlg" );
  resize( 303, 204 ); 
  setCaption( tr( "GEOM_SUPPRESSHOLE_TITLE"  ) );
  setSizeGripEnabled( TRUE );
  GeometryGUI_SuppressHoleLayout = new QGridLayout( this ); 
  GeometryGUI_SuppressHoleLayout->setSpacing( 6 );
  GeometryGUI_SuppressHoleLayout->setMargin( 11 );
  

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

  Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
  Constructor2->setText( tr( ""  ) );
  Constructor2->setPixmap( image2 );
  Constructor2->setChecked( TRUE );
  Constructor2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, Constructor2->sizePolicy().hasHeightForWidth() ) );
  Constructor2->setMinimumSize( QSize( 50, 0 ) );
  GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );  
  QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  GroupConstructorsLayout->addItem( spacer_4, 0, 3 );  
  GeometryGUI_SuppressHoleLayout->addWidget( GroupConstructors, 0, 0 );
  

  /***************************************************************/
  GroupButtons = new QGroupBox( this, "GroupButtons" );
  GroupButtons->setTitle( tr( ""  ) );
  GroupButtons->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupButtons->sizePolicy().hasHeightForWidth() ) );
  GroupButtons->setColumnLayout(0, Qt::Vertical );
  GroupButtons->layout()->setSpacing( 0 );
  GroupButtons->layout()->setMargin( 0 );
  GroupButtonsLayout = new QHBoxLayout( GroupButtons->layout() );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  
  buttonOk = new QPushButton( GroupButtons, "buttonOk" );
  buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );
  buttonOk->setAccel( 276824143 );
  GroupButtonsLayout->addWidget( buttonOk );
  QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  GroupButtonsLayout->addItem( spacer_2 );  
  buttonApply = new QPushButton( GroupButtons, "buttonApply" );
  buttonApply->setText( tr( "GEOM_BUT_APPLY"  ) );
  buttonApply->setAutoDefault( TRUE );
  buttonApply->setDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonApply );  
  buttonClose = new QPushButton( GroupButtons, "buttonClose" );
  buttonClose->setText( tr( "GEOM_BUT_CLOSE"  ) );
  buttonClose->setAutoDefault( TRUE );
  GroupButtonsLayout->addWidget( buttonClose );  
  GeometryGUI_SuppressHoleLayout->addWidget( GroupButtons, 2, 0 );
  
  /* First constructor */
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
  TextLabelC1A1->setText( tr( "GEOM_MAIN_OBJECT"  ) );
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
  CheckBox1->setText( tr( "GEOM_SUPPRESSHOLE_SELECTFACE" ) );
  CheckBox1->setChecked( FALSE );
  GroupC1Layout->addWidget( CheckBox1, 1, 0 );

  CheckBox2 = new QCheckBox( GroupC1, "CheckBox2" );
  CheckBox2->setText( tr( "GEOM_SUPPRESSHOLE_SELECTWIRE" ) );
  CheckBox2->setChecked( FALSE );
  GroupC1Layout->addWidget( CheckBox2, 2, 0 );
  
  CheckBox3 = new QCheckBox( GroupC1, "CheckBox3" );
  CheckBox3->setText( tr( "GEOM_SUPPRESSHOLE_SELECTFACE_END" ) );
  CheckBox3->setChecked( FALSE );
  GroupC1Layout->addWidget( CheckBox3, 3, 0 );
  GeometryGUI_SuppressHoleLayout->addWidget( GroupC1, 1, 0 );

  /* Second constructor */
  GroupC2 = new QGroupBox( this, "GroupC2" );
  GroupC2->setTitle( tr( ""  ) );
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
  
  Layout3 = new QHBoxLayout; 
  Layout3->setSpacing( 6 );
  Layout3->setMargin( 0 );
  
  TextLabelC2A1 = new QLabel( GroupC2, "TextLabelC2A1" );
  TextLabelC2A1->setText( tr( "GEOM_SUPPRESSHOLE_FACE_SHELL"  ) );
  TextLabelC2A1->setMinimumSize( QSize( 50, 0 ) );
  TextLabelC2A1->setFrameShape( QLabel::NoFrame );
  TextLabelC2A1->setFrameShadow( QLabel::Plain );
  Layout3->addWidget( TextLabelC2A1 );
  
  SelectButtonC2A1 = new QPushButton( GroupC2, "SelectButtonC2A1" );
  SelectButtonC2A1->setText( tr( ""  ) );
  SelectButtonC2A1->setPixmap( image1 );
  SelectButtonC2A1->setToggleButton( FALSE );
  SelectButtonC2A1->setMaximumSize( QSize( 28, 32767 ) );
  Layout3->addWidget( SelectButtonC2A1 );
  
  LineEditC2A1 = new QLineEdit( GroupC2, "LineEditC2A1" );
  LineEditC2A1->setAlignment( int( QLineEdit::AlignLeft ) );
  Layout3->addWidget( LineEditC2A1 );
  
  GroupC2Layout->addLayout( Layout3, 0, 0 );
  QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  GroupC2Layout->addItem( spacer_3, 2, 0 );
  
  CheckBoxC2_1 = new QCheckBox( GroupC2, "CheckBoxC2_1" );
  CheckBoxC2_1->setText( tr( "GEOM_SUPPRESSHOLE_SELECT_HOLES_ON_FACE"  ) );
  CheckBoxC2_1->setChecked( FALSE );
  
  GroupC2Layout->addWidget( CheckBoxC2_1, 1, 0 );
  GeometryGUI_SuppressHoleLayout->addWidget( GroupC2, 1, 0 );


  /* Initialisations */
  Init(Sel, ic) ;
}


//=================================================================================
// function : ~GeometryGUI_SuppressHoleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_SuppressHoleDlg::~GeometryGUI_SuppressHoleDlg()
{
    // no need to delete child widgets, Qt does it all for us
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_SuppressHoleDlg::Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic )
{
  GroupC1->show();
  GroupC2->hide();
  
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;
  mySelection = Sel;
  myShape.Nullify() ;
  
  myIC = ic ;
  myUseLocalContext = false ;
  myOkShape         = false ;
  myOkSelectFace    = false ;

  myListOfIdFace    = new GEOM::GEOM_Shape::ListOfSubShapeID;
  myListOfIdWire    = new GEOM::GEOM_Shape::ListOfSubShapeID;
  myListOfIdEndFace = new GEOM::GEOM_Shape::ListOfSubShapeID;
  
  myListOfIdFace->length(0) ;
  myListOfIdWire->length(0) ;
  myListOfIdEndFace->length(0) ; 
  
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  
  /* Select sub modes not checked */
  CheckBox1->setChecked( FALSE );    /* sub mode GEOM::FACE     */
  CheckBox2->setChecked( FALSE );    /* sub mode GEOM::WIRE     */
  CheckBox3->setChecked( FALSE );    /* sub mode END GEOM::FACE */

  CheckBoxC2_1->setChecked( FALSE ); /* sub mode GEOM::WIRE(S)  */

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk,          SIGNAL( clicked() ),    this, SLOT( ClickOnOk() ) );
  connect( buttonApply,       SIGNAL( clicked() ),    this, SLOT( ClickOnApply() ) );
  connect( buttonClose,       SIGNAL( clicked() ),    this, SLOT( ClickOnClose() ) );
  connect( GroupConstructors, SIGNAL( clicked(int) ), this, SLOT( ConstructorsClicked(int) ));

  connect( SelectButtonC1A1,  SIGNAL( clicked() ),         this, SLOT( SetEditCurrentArgument() ));
  connect( SelectButtonC2A1,  SIGNAL( clicked() ),         this, SLOT( SetEditCurrentArgument() ));
  
  connect( CheckBox1,         SIGNAL( stateChanged(int) ), this, SLOT( ActivateUserFaceSelection() ));
  connect( CheckBox2,         SIGNAL( stateChanged(int) ), this, SLOT( ActivateUserWireSelection() ));
  connect( CheckBox3,         SIGNAL( stateChanged(int) ), this, SLOT( ActivateUserEndFaceSelection() ));

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  /* for the second constructor */
  connect( CheckBoxC2_1,      SIGNAL( stateChanged(int) ), this, SLOT( ActivateUserWiresOnFaceShellSelection() ));

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
void GeometryGUI_SuppressHoleDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId)
    {
    case 0:
      {
	GroupC1->show();
	GroupC2->hide();
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditC1A1 ;	
	LineEditC1A1->setText(tr("")) ;
	myOkShape         = false ;
	myOkSelectFace    = false ;
	this->ResetPartial() ;
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	break;
      }
    case 1:
      {
	GroupC1->hide();
	GroupC2->show();
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditC2A1 ;
	LineEditC2A1->setText(tr("")) ;
	myOkShape = false ;
	this->ResetPartial() ;
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	this->ResetPartial() ;
	break;
      }
    }
 return ;
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  accept();

  return ;
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ClickOnApply()
{
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  bool testResult = false ;
  
  if( !myOkShape )
    return ;
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {
	if(  !myOkSelectFace )
	  return ;
  
	if( CheckBox2->isChecked() ) {
	  
	  if( !CheckBox3->isChecked() ) {
	    
	    /* Call method to get sub shape selection of GEOM::WIRE */
	    bool aTest = myGeomGUI->GetIndexSubShapeSelected(myFace, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext) ;
	    
	    myGeomGUI->OnDisplayAll(true) ; /* Display all objects so that next method using ic can memorize them */
	    if( !aTest || myListOfIdWire->length() != 1 ) {
	      CheckBox2->setChecked(FALSE) ;
	      myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ; 
	    }
	    else {
	      myListOfIdEndFace->length(0) ; /* no end face */	
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      testResult = myGeomGUI->OnSuppressHole( myShapeIOR, myListOfIdFace, myListOfIdWire, myListOfIdEndFace ) ;
	      QApplication::restoreOverrideCursor();
	    }
	  }
	  else { /* CheckBox3->isChecked() */
	    
	    /* Call method to get sub shape selection of END GEOM::FACE */
	    bool aTest = myGeomGUI->GetIndexSubShapeSelected(myShape, int(TopAbs_FACE), myListOfIdEndFace, myLocalContextId, myUseLocalContext) ;
	    
	    myGeomGUI->OnDisplayAll(true) ; /* Display all objects so that next method using ic can memorize them */
	    if( !aTest || myListOfIdEndFace->length() != 1 ) {
	      CheckBox3->setChecked(FALSE) ;
	      myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ; 
	    }
	    else {	
	      QApplication::setOverrideCursor( Qt::waitCursor );
	      testResult = myGeomGUI->OnSuppressHole( myShapeIOR, myListOfIdFace, myListOfIdWire, myListOfIdEndFace ) ; 
	      QApplication::restoreOverrideCursor();
	    }     
	  }
	}
	break ;
      }

    case 1:  /* second constructor */
      {
	if( CheckBoxC2_1->isChecked() ) {
	  
	  /* Call method to get sub shape selection of one or more GEOM::WIRE(s) on a face or a shell */
	  bool aTest = myGeomGUI->GetIndexSubShapeSelected(myShape, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext) ;
	  
	  myGeomGUI->OnDisplayAll(true) ; /* Display all objects so that next method using ic can memorize them */
	  
	  if( !aTest || myListOfIdWire->length() < 1 ) {
	    CheckBoxC2_1->setChecked(FALSE) ;
	    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ;
	  }
	  else {
	    QApplication::setOverrideCursor( Qt::waitCursor );
	    testResult = myGeomGUI->OnSuppressHolesInFaceOrShell( myShapeIOR, myListOfIdWire ) ;
	    QApplication::restoreOverrideCursor();
	  }
	}
	break ;
      }
    }
  

  if( !testResult )
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ;
  else
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_DONE") ) ;
  
  /* Reset arguments to allow a new selection */
  this->ResetStateOfDialog() ;
  return ;
}



//=================================================================================
// function : ClickOnClose()
// purpose  :
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ClickOnClose()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); //    myIC = myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getViewerOCC()->getAISContext();
    if(myUseLocalContext) {
      myIC->CloseLocalContext(myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }

  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 and SelectButtonC2A1
//=================================================================================
void GeometryGUI_SuppressHoleDlg::SelectionIntoArgument()
{
  
  /* Reset argument and local context when selection as changed */
  this->ResetStateOfDialog() ;
  
  QString aString = ""; /* Name of selection */
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 )
    return ;
  
  /* nbSel == 1 */
  TopoDS_Shape S ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;

  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  if ( S.IsNull() || S.ShapeType() == TopAbs_VERTEX || S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE ) {
    return ;
  }
  
  /* Test the exact type of topology to suppress faces into.         */
  /* For the second constructor a face or shell selection is needed  */
  if (  myConstructorId == 0 || ( myConstructorId == 1 && ( S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL ) ) ) {
    
    if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );

      /* The Geom IOR string of selection */
      myShapeIOR = GIObject->getIOR();
      myEditCurrentArgument->setText(aString) ;
      myShape = S ;
      myOkShape = true ;
      return;
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
void GeometryGUI_SuppressHoleDlg::SetEditCurrentArgument()
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
   case 1:
      {
	if(send == SelectButtonC2A1) {
	  LineEditC2A1->setFocus() ;
	  myEditCurrentArgument = LineEditC2A1;	  
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
void GeometryGUI_SuppressHoleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else if ( send == LineEditC2A1)
    myEditCurrentArgument = LineEditC2A1; 
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
void GeometryGUI_SuppressHoleDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    
    this->ResetStateOfDialog() ;
    
    disconnect( mySelection, 0, this, 0 );
    GroupConstructors->setEnabled(false) ;    
    GroupC1->setEnabled(false) ;
    GroupC2->setEnabled(false) ;
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
void GeometryGUI_SuppressHoleDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate other active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;

  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupC2->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;

  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void GeometryGUI_SuppressHoleDlg::enterEvent(QEvent* e)
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
void GeometryGUI_SuppressHoleDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnClose() ;
  return ;
}


//=================================================================================
// function : ActivateUserFaceSelection()
// purpose  : Called when CheckBox1 state has changed. (Face selection is ckecked)
//          : Be careful user must first select a face then a wire !
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ActivateUserFaceSelection()
{
  if( !this->myOkShape ) {
    this->ResetStateOfDialog() ;
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_MAIN_OBJECT") ) ;
    return ;
  }
  
  /* Test the viewer type VTK */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    this->ResetStateOfDialog() ;  
    return;
  }
  
  if( CheckBox1->isChecked() ) {
    
    /* local context is opened into the method : Prepare GEOM::FACE sub selection */
    myGeomGUI->PrepareSubShapeSelection( int(TopAbs_FACE), this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_SUPPRESSHOLE_SELECTFACE") ) ;
  } 
  else {
    this->ResetPartial() ;
  }

  return ;
}


//=================================================================================
// function : ActivateUserWireSelection()
// purpose  : Called when CheckBox2 state has changed. (Wire selection is ckecked)
//          : Be careful user must first select a face then a wire !
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ActivateUserWireSelection()
{
  
  if( !this->myOkShape ) {
    this->ResetStateOfDialog() ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_MAIN_OBJECT") ) ;
    return ;
  } 
  
  /* Test the type of viewer VTK */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    this->ResetStateOfDialog() ;
    return;
  }

  if( CheckBox1->isChecked() ) {
    
    /* Get sub shape selection GEOM::FACE : local context is closed */    
    bool aTest = myGeomGUI->GetIndexSubShapeSelected(myShape, int(TopAbs_FACE), myListOfIdFace, myLocalContextId, myUseLocalContext) ;
    
    myGeomGUI->OnDisplayAll(true) ; /* Display all objects so that next method using ic can memorize them */
    if( !aTest || myListOfIdFace->length() != 1 ) {
      CheckBox1->setChecked(FALSE) ;
      myOkSelectFace = false ;
      myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ; 
    }
    else {
      myOkSelectFace = true ;
    }
  }
  else {
    this->ResetPartial() ;
    return ;
  }
  

  if( CheckBox2->isChecked() ) {
    
    /* Get the face selection */
    this->myFace = FaceFromList(myShape, myListOfIdFace) ;    
    /* Local context is opened into the method : Prepare GEOM::WIRE sub selection into a face */
    myGeomGUI->PrepareSubShapeSelectionArgumentShape( this->myFace, int(TopAbs_WIRE), this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_SUPPRESSHOLE_SELECTWIRE") ) ;
  }
  else {
    this->ResetPartial() ;
  }
  
  return ;
}



//=================================================================================
// function : ActivateUserlEndFaceSelection()
// purpose  : Called when CheckBox3 state has changed. ( Optional End Face selection )
//          : Be careful user must first select a face then a wire then this optional end face !
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ActivateUserEndFaceSelection()
{
  
  if( !this->myOkShape ) {
    this->ResetStateOfDialog() ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_MAIN_OBJECT") ) ;
    return ;
  }
  
  /* Test the type of viewer VTK */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    this->ResetStateOfDialog() ;
    return;
  }
 
   
  if( CheckBox2->isChecked() ) {
    /* Call method to get sub shape selection for the GEOM::WIRE into myFace : local context is closed */
    bool aTest = myGeomGUI->GetIndexSubShapeSelected(this->myFace, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext) ;
    
    myGeomGUI->OnDisplayAll(true) ; /* Display all objects so that next method using ic can memorize them */
    
    if( !aTest || myListOfIdWire->length() != 1 ) {
      CheckBox2->setChecked(FALSE) ;
      CheckBox3->setChecked(FALSE) ;
      myGeomGUI->GetDesktop()->putInfo( tr("GEOM_PRP_ABORT") ) ;     
      return ;
    }
  }
  else {
    this->ResetPartial() ;
    return ;
  }
  
  
  if( CheckBox3->isChecked() ) {    
    /* Local context is opened into the method : prepare GEOM::FACE(end) into myShape sub selection */
    myGeomGUI->PrepareSubShapeSelectionArgumentShape( this->myShape, int(TopAbs_FACE), this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_SUPPRESSHOLE_SELECTFACE_END") ) ;
  }
  else {
    this->ResetPartial() ;
  }

  return ;
}



//=================================================================================
// function : ActivateUserWiresOnFaceShellSelection()
// purpose  : Called when CheckBoxC2_1 state has changed.
//          : Only for second constructor !
//          : Prepare selection for wire(s) on main object that is a face or a shell
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ActivateUserWiresOnFaceShellSelection()
{
  
  if( !this->myOkShape ) {
    this->ResetStateOfDialog() ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_MAIN_OBJECT") ) ;
    return ;
  }
  
  /* Test the type of viewer VTK */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC ) {
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    this->ResetStateOfDialog() ;
    return;
  }
  
  if( CheckBoxC2_1->isChecked() ) {    
    /* Local context is opened to prepare GEOM::WIRE(S) selection into 'myShape' that is a (main) face */
    myGeomGUI->PrepareSubShapeSelectionArgumentShape( this->myShape, int(TopAbs_WIRE), this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo( tr("GEOM_SUPPRESSHOLE_SELECT_HOLES_ON_FACE") ) ;
  }
  else {
    this->ResetPartial() ;
  }
  return ;
}



//=================================================================================
// function : FaceFromList()
// purpose  : Return the face (selected by user) that is a sub shape of 'aShape'
//          : and which unique index is in 'ListOfSub'.
//          : This allows opening a local context with this face loaded.
//          : See : myGeomGUI->PrepareSubShapeSelectionArgumentShape(...)
//=================================================================================
TopoDS_Shape GeometryGUI_SuppressHoleDlg::FaceFromList( const TopoDS_Shape& aShape,
							const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfSub )
{
  TopoDS_Shape tds ;
  tds.Nullify() ;
  if( ListOfSub.length() != 1 || aShape.IsNull() )
    return tds ;
  
  int i = ListOfSub[0] ;
  TopExp_Explorer exp ;
  int j = 1 ; 
  for( exp.Init(aShape, TopAbs_FACE); exp.More(); exp.Next() ) {
    if(j == i)
      return exp.Current() ;    
    j++ ;
  }
  return tds ;
}




//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ResetStateOfDialog()
{
  this->myOkShape = false ;
  this->myEditCurrentArgument->setText("") ;
  
  /* Partial reset and more ...*/
  this->ResetPartial() ;

  return ;
}


//=================================================================================
// function : ResetPartial()
// purpose  : Partially reset to keep only main selection
//=================================================================================
void GeometryGUI_SuppressHoleDlg::ResetPartial()
{
  /* Select sub shape modes not checked */
  this->myOkSelectFace = false ;
  this->CheckBox1->setChecked( FALSE );
  this->CheckBox2->setChecked( FALSE );
  this->CheckBox3->setChecked( FALSE );
  this->CheckBoxC2_1->setChecked( FALSE );
  
  myListOfIdFace->length(0) ;
  myListOfIdWire->length(0) ;
  myListOfIdEndFace->length(0) ;
  
  /* Close its local contact if opened */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); //    myIC = myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getViewerOCC()->getAISContext();
    if( this->myUseLocalContext ) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }

  return ;
}
