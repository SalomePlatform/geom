using namespace std;
//  File      : GeometryGUI_SubShapeDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_SubShapeDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "utilities.h"

#include <TopExp_Explorer.hxx>

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


#include <qmessagebox.h> 

//=================================================================================
// class    : GeometryGUI_SubShapeDlg()
// purpose  : Constructs a GeometryGUI_SubShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_SubShapeDlg::GeometryGUI_SubShapeDlg( QWidget* parent,
						  const char* name,
						  SALOME_Selection* Sel,
						  Handle (AIS_InteractiveContext) ic,
						  bool modal,
						  WFlags fl )
  
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_SUBSHAPE")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_SUBSHAPE" );
    resize( 303, 239 ); 
    setCaption( tr( "GEOM_SUBSHAPE_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_SubShapeDlgLayout = new QGridLayout( this ); 
    GeometryGUI_SubShapeDlgLayout->setSpacing( 6 );
    GeometryGUI_SubShapeDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_SUB_SHAPE"  ) );
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
    GeometryGUI_SubShapeDlgLayout->addWidget( GroupConstructors, 0, 0 );

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
    GeometryGUI_SubShapeDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_ARGUMENTS"  ) );
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
    TextLabelComboBox1 = new QLabel( GroupC1, "TextLabelComboBox1" );
    TextLabelComboBox1->setText( tr( "GEOM_SUBSHAPE_TYPE"  ) );
    GroupC1Layout->addMultiCellWidget( TextLabelComboBox1, 1, 1, 0, 1 );
    ComboBox1 = new QComboBox( FALSE, GroupC1, "ComboBox1" );
    ComboBox1->setMaxCount( 100 );
    GroupC1Layout->addMultiCellWidget( ComboBox1, 1, 1, 2, 3 );
    CheckBox1 = new QCheckBox( GroupC1, "CheckBox1" );
    CheckBox1->setText( tr( "GEOM_SUBSHAPE_SELECT"  ) );
    CheckBox1->setChecked( FALSE );
    GroupC1Layout->addMultiCellWidget( CheckBox1, 2, 2, 0, 1 );
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addMultiCellWidget( LineEditC1A1, 0, 0, 2, 3 );
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    SelectButtonC1A1->setMaximumSize( QSize( 28, 32767 ) );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    GeometryGUI_SubShapeDlgLayout->addWidget( GroupC1, 1, 0 );
    /***************************************************************/
   
    /* Initialisations */
    Init(Sel, ic) ;
}


//=================================================================================
// function : ~GeometryGUI_SubShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_SubShapeDlg::~GeometryGUI_SubShapeDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic )
{

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;
  mySelection = Sel;
  myShape.Nullify() ;

  myIC = ic ;
  myUseLocalContext = false ;
  myLocalContextId = -1;
  myAbort = false ;
  myOkShape = false ;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;

  /* type for sub shape selection */
  ComboBox1->insertItem("Compound");
  ComboBox1->insertItem("Compsolid");
  ComboBox1->insertItem("Solid");
  ComboBox1->insertItem("Shell");
  ComboBox1->insertItem("Face");
  ComboBox1->insertItem("Wire");
  ComboBox1->insertItem("Edge");
  ComboBox1->insertItem("Vertex");
  ComboBox1->insertItem("Shape");

  myWithShape = true;
  myShapeType = ComboBox1->currentItem();

  /* Select sub shapes mode not checked */
  CheckBox1->setChecked( FALSE );
  myOkSelectSubMode = CheckBox1->isChecked();

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),            this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ),        this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), this, SLOT( ConstructorsClicked(int) ) );

  connect( SelectButtonC1A1, SIGNAL (clicked() ),  this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( CheckBox1, SIGNAL (stateChanged(int) ), this, SLOT( AllOrNotAll() ) ) ;

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( ComboBox1, SIGNAL( activated(int) ), this, SLOT( ComboTextChanged() ) );

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
void GeometryGUI_SubShapeDlg::ConstructorsClicked(int constructorId)
{
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::ClickOnOk()
{
  this->ClickOnApply() ;

  /* User has aborted or not operation of explode all with many sub shapes */
  if( this->myAbort == false )
    this->ClickOnCancel() ;
  else
    this->myAbort = false ;
    
  return ;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::ClickOnApply()
{
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  bool testResult = false ;

  this->myAbort = false ; /* Not aborted by default */
    
  switch(myConstructorId)
    { 
    case 0 :
      { 
	/* Explode all sub shapes */
	if( myOkShape && !myOkSelectSubMode ) {
	  
	  /* More than 30 subshapes : ask confirmation */
	  unsigned int nb = NumberOfSubShapes( myShape, myShapeType ) ;
	  if( nb > 30 ) {
	    const QString caption  = tr("GEOM_CONFIRM") ;
	    const QString text = tr("GEOM_CONFIRM_INFO").arg(nb) ;
	    const QString button0  = tr("GEOM_BUT_EXPLODE") ;
	    const QString button1  = tr("GEOM_BUT_CANCEL") ;
	    
	    if( QMessageBox::warning( this, caption, text, button0, button1 ) == 0 )
	      testResult = myGeomGUI->OnSubShapeGetAll( myShape, myShapeIOR, myShapeType ) ;
	    else
	      this->myAbort = true ;  /* aborted */
	  }
	  else {
	    testResult = myGeomGUI->OnSubShapeGetAll( myShape, myShapeIOR, myShapeType ) ;
	  }
	}
	/* explode only selected sub shapes */
	else if( myOkShape && myOkSelectSubMode ) {
	  testResult = myGeomGUI->OnSubShapeGetSelected( myShape, myShapeIOR, myShapeType, myLocalContextId, myUseLocalContext ) ; 
	}
	if( !testResult ) {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_ABORT")) ;
	  this->myAbort = true;
	}
	else {
	  myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_DONE")) ;
	}  
	/* Reset all arguments and local context to allow user a new selection ...*/
	this->ResetStateOfDialog() ;
	break ;
      }
    }
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::ClickOnCancel()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); 
    if(myUseLocalContext) {
      myIC->CloseLocalContext(myLocalContextId) ;
      myGeomGUI->OnDisplayAll(true) ;
      this->myUseLocalContext = false ;
    }
  }
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void GeometryGUI_SubShapeDlg::SelectionIntoArgument()
{
  
  /* Reset all arguments and local context when selection as changed */
  this->ResetStateOfDialog() ;
  
  QString aString = ""; /* future name of selection */
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;

  if ( nbSel != 1 ) {
    LineEditC1A1->setText("") ;
    myOkShape = false;
    return ;
  }
  
  /* nbSel == 1 */
  TopoDS_Shape S ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;

  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  if( !IO->hasEntry() ) {
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY")) ;
    return ;
  }

  if ( !S.IsNull() && S.ShapeType() != TopAbs_VERTEX ) 
  {
    if ( IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) 
      {
	Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
	myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
	LineEditC1A1->setText(aString) ;
	myShape = S ;
	myOkShape = true ;
      } 
    else
      {
	SALOMEDS::Study_var aStudy = myGeomGUI->GetActiveStudy()->getStudyDocument();
	  SALOMEDS::SObject_var obj = aStudy->FindObjectID( IO->getEntry() );
	  SALOMEDS::GenericAttribute_var anAttr;
	  SALOMEDS::AttributeIOR_var     anIOR;
	  if ( !obj->_is_nil() ) 
	    {
	      if (obj->FindAttribute(anAttr, "AttributeIOR")) 
		{
		  anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
		  myShapeIOR = anIOR->Value();
		  myOkShape = true ;
		  myShape = S ;
		  LineEditC1A1->setText(aString) ;
		}
	    }
	}
    
    int SelectedShapeType = ComboBox1->currentItem();
    int count = ComboBox1->count();
    if ( myWithShape ) count = count - 1;

    int i = 0;
    while ( i <= myShape.ShapeType() ) {
      ComboBox1->removeItem(0);
      i++;
    }

    if (myShape.ShapeType()==TopAbs_COMPOUND) 
      {
	if (myWithShape == false) {
	  ComboBox1->insertItem("Shape");
	  myWithShape = true;
	}
      }
    else
      {
	if (myWithShape == true) {
	  ComboBox1->removeItem( ComboBox1->count() -1 );
	  myWithShape = false;
	}
      }
    
    int count1 = ComboBox1->count();
    if ( myWithShape ) count1 = count1 - 1;
    
    if ( SelectedShapeType > myShape.ShapeType() ) {
      if ( SelectedShapeType == 8 ) {
	if ( myShape.ShapeType() != TopAbs_COMPOUND ) {
	  ComboBox1->setCurrentItem(0);
	  myShapeType = 8 - count1;
	}
      } else {
	ComboBox1->setCurrentItem( count1 - count + SelectedShapeType );
	myShapeType = 8 - count1 + ComboBox1->currentItem();
      }
    } else {
      ComboBox1->setCurrentItem(0);
      myShapeType = 8 - count1;
    }      
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::SetEditCurrentArgument()
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
void GeometryGUI_SubShapeDlg::LineEditReturnPressed()
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
void GeometryGUI_SubShapeDlg::DeactivateActiveDialog()
{
  /* Check if active */
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
void GeometryGUI_SubShapeDlg::ActivateThisDialog()
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
// purpose  :
//=================================================================================
void GeometryGUI_SubShapeDlg::enterEvent(QEvent* e)
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
void GeometryGUI_SubShapeDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : AllOrNotAll()
// purpose  : Allow user selection of all or only selected sub shapes
//          : Called when 'CheckBox1' state change
//=================================================================================
void GeometryGUI_SubShapeDlg::AllOrNotAll()
{
  
  /* No sub shape selection if main shape not selected */
  if( !this->myOkShape ) {
    ResetStateOfDialog() ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SELECT_FIRST")) ;
    return ;
  }
  
  if (myShapeType ==TopAbs_SHAPE && myShape.ShapeType()==TopAbs_COMPOUND) 
    {
      /* Select sub shapes mode not checked */
      myOkSelectSubMode = false ;    
      CheckBox1->setChecked( FALSE );
      //no meaning to allow user selection for type = shape
      //TODO - add another message
      //myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SELECT_FIRST")) ;
      return ;
    }

  myOkSelectSubMode = CheckBox1->isChecked() ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); 
    if( this->myUseLocalContext ) {
      myIC->CloseLocalContext(myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  } else {
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER") ) ;
    return;
  }

  if( myOkShape && myOkSelectSubMode ) {
    /* local context is defined into the method */
    myGeomGUI->PrepareSubShapeSelection( this->myShapeType, this->myLocalContextId ) ;    
    myUseLocalContext = true ;
    myGeomGUI->GetDesktop()->putInfo(tr("GEOM_PRP_SELECT_FACE")) ;
  }
  return ;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void GeometryGUI_SubShapeDlg::ResetStateOfDialog()
{
  /* To leave current selection if explode all as been aborted by user */
  if( this->myAbort == true ) {
    this->myOkShape = false ;
    this->myEditCurrentArgument->setText("") ;
  }
  else {
    ; /* nothing to do : keep selection argument */
  }

  int SelectedShapeType = ComboBox1->currentItem();
  int count = ComboBox1->count();
  if ( myWithShape ) count = count - 1;
  /* type for sub shape selection */
  ComboBox1->clear();
  ComboBox1->insertItem("Compound");
  ComboBox1->insertItem("Compsolid");
  ComboBox1->insertItem("Solid");
  ComboBox1->insertItem("Shell");
  ComboBox1->insertItem("Face");
  ComboBox1->insertItem("Wire");
  ComboBox1->insertItem("Edge");
  ComboBox1->insertItem("Vertex");
  ComboBox1->insertItem("Shape");
  this->myWithShape=true;
  ComboBox1->setCurrentItem( 8 - count + SelectedShapeType );

  
  /* unpress buttons : due to abort box*/
  this->buttonApply->setDown(FALSE) ;
  this->buttonOk->setDown(FALSE) ;

  /* Select sub shapes mode not checked */
  this->myOkSelectSubMode = false ;    
  this->CheckBox1->setChecked( FALSE );

  /* Close its local contact if opened */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext(); 
    if( this->myUseLocalContext ) {
      myIC->CloseLocalContext(this->myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }
  return ;
}



//=================================================================================
// function : ComboTextChanged()
// purpose  : 
//=================================================================================
void GeometryGUI_SubShapeDlg::ComboTextChanged()
{
  if ( myOkShape )
    this->myShapeType = ComboBox1->currentItem() + myShape.ShapeType() + 1;
  else
    this->myShapeType = ComboBox1->currentItem();

  /* Select sub shapes mode not checked */
  CheckBox1->setChecked( FALSE );
  myOkSelectSubMode = FALSE ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if( this->myUseLocalContext ) {
      myIC->CloseLocalContext(myLocalContextId) ;
      this->myUseLocalContext = false ;
      myGeomGUI->OnDisplayAll(true) ;
    }
  }
  return ;
}


//=================================================================================
// function : NumberOfSubShapes()
// purpose  :
//=================================================================================
unsigned int GeometryGUI_SubShapeDlg::NumberOfSubShapes( const TopoDS_Shape& S, const int shapeType )
{
  if( S.IsNull() )
    return 0 ;
  
  unsigned int index = 0 ;
  TopExp_Explorer Exp( S, TopAbs_ShapeEnum(shapeType) );
  TopTools_MapOfShape M;
  while ( Exp.More() ) {
    if ( M.Add(Exp.Current()) )
      index++;
    Exp.Next();
  }
  M.Clear() ;
  return index ;
}


