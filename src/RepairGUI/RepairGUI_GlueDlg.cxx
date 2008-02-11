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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : RepairGUI_GlueDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "OCCViewer_ViewModel.h"

#include "RepairGUI_GlueDlg.h"
#include "DlgRef_1Sel_Ext.h"
#include "GEOMImpl_Types.hxx"

#include "QtxDblSpinBox.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_Tools.h"

#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SUIT_OverrideCursor.h"

#include "SALOME_ListIteratorOfListIO.hxx"

#include <TCollection_AsciiString.hxx>

#include <qapplication.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qgrid.h>

using namespace std;

#define DEFAULT_TOLERANCE_VALUE 1e-07

//=================================================================================
// class    : RepairGUI_GlueDlg()
// purpose  : Constructs a RepairGUI_GlueDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_GlueDlg::RepairGUI_GlueDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                     const char* name, bool modal, WFlags fl)
: GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                    WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu),
  myCurrConstrId( -1 )
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_GLUE_FACES")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_GLUE_FACES2")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_GLUE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_GLUE_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image2);
  //RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_GLUE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );
  
  int aWidth = GroupPoints->TextLabel1->sizeHint().width() + 
               GroupPoints->PushButton1->sizeHint().width() + 
               GroupPoints->getGroupBoxLayout()->spacing();

  Layout1->addWidget(GroupPoints, 2, 0);

  QHBox* aHBox = new QHBox( GroupPoints->GroupBox1 );
  aHBox->setSpacing( 5 );
  
  (new QLabel( tr( "GEOM_TOLERANCE" ), aHBox ))->setFixedWidth( aWidth );
  myTolEdt = new QtxDblSpinBox( 0, 100, 1e-7, aHBox );
  myTolEdt->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myTolEdt->setPrecision( 10 );
  myTolEdt->setValue( DEFAULT_TOLERANCE_VALUE );
  GroupPoints->getGroupBoxLayout()->addWidget( aHBox, 3, 0 );
  /***************************************************************/

  GroupPoints2 = new DlgRef_1Sel_Ext(this, "GroupPoints2");
  GroupPoints2->GroupBox1->setTitle(tr("GEOM_GLUE"));
  GroupPoints2->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints2->PushButton1->setPixmap(image1);
  GroupPoints2->LineEdit1->setReadOnly( true );
  Layout1->addWidget(GroupPoints2, 2, 0);

  QGrid* aGrid = new QGrid( 2, Qt::Horizontal, GroupPoints2->GroupBox1 );
  aGrid->setSpacing( 5 );
  (new QLabel( tr( "GEOM_TOLERANCE" ), aGrid ))->setFixedWidth( aWidth );;
  myTolEdt2 = new QtxDblSpinBox( 0, 100, 1e-7, aGrid );
  myTolEdt2->setPrecision( 10 );
  myTolEdt2->setValue( DEFAULT_TOLERANCE_VALUE );
  myTolEdt2->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  (new QLabel( tr( "GLUE_FACES" ), aGrid ))->setFixedWidth( aWidth );;
  myDetectBtn = new QPushButton( tr( "GEOM_DETECT" ), aGrid );
  myDetectBtn->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  mySubShapesChk = new QCheckBox( tr( "SELECT_FACES" ), aGrid );
  GroupPoints2->getGroupBoxLayout()->addWidget( aGrid, 3, 0 );
  /***************************************************************/

  setHelpFileName("glue_faces_operation_page.html");
  
  // Disable second way of gluing if OCC viewer is not active one
  if (SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
      != OCCViewer_Viewer::Type())
    RadioButton2->setEnabled(false);

  Init();
}


//=================================================================================
// function : ~RepairGUI_GlueDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_GlueDlg::~RepairGUI_GlueDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  //globalSelection( GEOM_COMPOUND );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect( myTolEdt2, SIGNAL( valueChanged( double ) ), this, SLOT( onTolerChanged( double ) ) );
  connect( mySubShapesChk, SIGNAL( stateChanged( int ) ), this, SLOT( onSubShapesChk() ) );

  connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	   SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) );

  connect(myDetectBtn, SIGNAL(clicked()), this, SLOT(onDetect()));

  initName( tr( "GLUE_NEW_OBJ_NAME" ) );

  ConstructorsClicked(0);
  
  activateSelection();
  updateButtonState();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void RepairGUI_GlueDlg::ConstructorsClicked( int constructorId )
{
  if ( myCurrConstrId == constructorId )
    return;
  
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  
  switch ( constructorId )
    {
    case 0:
      {
        GroupPoints2->hide();
        resize( 0, 0 );
        GroupPoints->show();
        GroupPoints->LineEdit1->setText( "" );
        myEditCurrentArgument = GroupPoints->LineEdit1;
        
        if ( myCurrConstrId >= 0 ) // i.e. it is not initialisation
        {
          // copy tolerance from previous tolerance field
          myTolEdt->setValue( myTolEdt2->value() );
        }
          
        break;
      }
    case 1:
      {
        GroupPoints->hide();
        resize(0, 0);
        GroupPoints2->show();
        GroupPoints->LineEdit1->setText("");
        myEditCurrentArgument = GroupPoints2->LineEdit1;
        
        if ( myCurrConstrId >= 0 ) // i.e. it is not initialisation
        {
          // copy tolerance from previous tolerance field
          myTolEdt2->setValue( myTolEdt->value() );
          mySubShapesChk->setChecked( false );
          clearTemporary();
        }
        
        break;
      }
    }
    
    myCurrConstrId = constructorId;

  myEditCurrentArgument->setFocus();

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  qApp->processEvents();
  updateGeometry();
  QSize aSize = minimumSizeHint();
  resize( width(), aSize.height() );
  
  displayPreview();
  updateButtonState();
  activateSelection();
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_GlueDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::ClickOnApply()
{
  if ( !onAcceptLocal() )
    return false;

  initName();

  //GroupPoints->LineEdit1->setText("");
  //myObject = GEOM::GEOM_Object::_nil();

  //globalSelection( GEOM_COMPOUND );

  ConstructorsClicked( getConstructorId() );

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_GlueDlg::SelectionIntoArgument()
{
  if ( mySubShapesChk->isChecked() &&  getConstructorId() == 1 ) {
    updateButtonState();
    return;
  }
  
  erasePreview();
  myEditCurrentArgument->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  if ( IObjectCount() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    Standard_Boolean aRes;
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( aRes )
      myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
  }
  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 || send == GroupPoints2->PushButton1 )  {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if( send == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
  else if( send == GroupPoints2->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints2->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  //GroupPoints->LineEdit1->setText("");
  //GroupPoints2->LineEdit1->setText("");
  //myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  //globalSelection( GEOM_COMPOUND );
  activateSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_GlueDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_GlueDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::isValid( QString& msg )
{
  return !myObject->_is_nil() && ( IsPreview() || myTolEdt->value() > 0. );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  objects.clear();

  switch ( getConstructorId() )
  {
  case 0:
    {
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::_narrow
        ( getOperation() )->MakeGlueFaces( myObject, myTolEdt2->value(), true );
      aResult = !anObj->_is_nil();
      if ( aResult )
        objects.push_back( anObj._retn() );
      break;
    }
  case 1:
    {
      if ( IsPreview() )
      {
        // if this method is used for displaying preview then we must detect glue faces only
        ObjectList::iterator anIter;
        for (anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter)
          objects.push_back( GEOM::GEOM_Object::_duplicate( *anIter ) );
        return myTmpObjs.size() ? true : false;
      } // IsPreview

      // Make glue face by list.
      // Iterate through myTmpObjs and verifies where each object is currently selected or not.
      QMap<QString, char> selected;

      // Get names of selected objects
      SALOME_ListIteratorOfListIO it ( selectedIO() );
      for (; it.More(); it.Next()) 
        selected.insert( it.Value()->getName(), 0 );

      // Iterate through result and select objects with names from selection
      // ObjectList toRemoveFromEnggine;
      ObjectList toGlue;
      ObjectList::iterator anIter;
      for (anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter) 
      {
        if ( selected.contains( myGeomGUI->getApp()->orb()->object_to_string(*anIter) ) )
          toGlue.push_back(*anIter);
      }

      // make glue faces
      GEOM::ListOfGO_var aListForGlue = new GEOM::ListOfGO();
      aListForGlue->length( toGlue.size() );
      ObjectList::iterator anIter3 = toGlue.begin();
      for ( int i = 0; anIter3 != toGlue.end(); ++anIter3, ++i )
        aListForGlue[ i ] = *anIter3;
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::_narrow
        ( getOperation() )->MakeGlueFacesByList( myObject, myTolEdt2->value(), aListForGlue, true );

      aResult = !anObj->_is_nil();

      if ( aResult )
        objects.push_back( anObj._retn() );

      // Remove from engine useless objects
      clearTemporary();

      updateButtonState();

      break;
    } // case 1
  } // switch

  return aResult;
}


//================================================================
// Function : clearShapeBufferLocal
// Purpose  : 
//================================================================
void RepairGUI_GlueDlg::clearShapeBufferLocal( GEOM::GEOM_Object_ptr theObj )
{
  if ( CORBA::is_nil( theObj ) )
    return;

  CORBA::String_var IOR = myGeomGUI->getApp()->orb()->object_to_string( theObj );
  TCollection_AsciiString asciiIOR( (char *)( IOR.in() ) );
  myGeomGUI->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );

  if ( !getStudy() || !( getStudy()->studyDS() ) )
    return;

  _PTR(Study) aStudy = getStudy()->studyDS();
  _PTR(SObject) aSObj ( aStudy->FindObjectIOR( string( IOR.in() ) ) );
  if ( !aSObj )
    return;

  _PTR(ChildIterator) anIt ( aStudy->NewChildIterator( aSObj ) );
  for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
    _PTR(GenericAttribute) anAttr;
    if ( anIt->Value()->FindAttribute(anAttr, "AttributeIOR") ) {
      _PTR(AttributeIOR) anIOR ( anAttr );
      TCollection_AsciiString asciiIOR( (char*)anIOR->Value().c_str() );
      myGeomGUI->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );      
    }
  }
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it 
//            performs a proper operation and manages transactions, etc.
//================================================================
bool RepairGUI_GlueDlg::onAcceptLocal()
{
  if ( !getStudy() || !( getStudy()->studyDS() ) )
    return false;

  _PTR(Study) aStudy = getStudy()->studyDS();

  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) 
  {
    MESSAGE("GEOMBase_Helper::onAccept - ActiveStudy is locked");
    SUIT_MessageBox::warn1 ( (QWidget*)(SUIT_Session::session()->activeApplication()->desktop()),
      QObject::tr("WRN_WARNING"), QObject::tr("WRN_STUDY_LOCKED"), QObject::tr("BUT_OK") );
    return false;
  }

  QString msg;
  if ( !isValid( msg ) ) 
  {
    showError( msg );
    return false;
  }

  erasePreview( false );

  try 
  {
    if ( openCommand() ) 
    {
      SUIT_OverrideCursor wc;

      SUIT_Session::session()->activeApplication()->putInfo( "" );
      ObjectList objects;
      
      if ( !execute( objects ) ) 
      { 
        wc.suspend();
        abortCommand();
        showError();
      }
      else 
      {
        const int nbObjs = objects.size();
        bool withChildren = false;
        for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it ) 
        {
          QString aName("");
          if ( nbObjs > 1 )
          {
            aName = strlen( getNewObjectName() ) ? GEOMBase::GetDefaultName( getNewObjectName() ) : GEOMBase::GetDefaultName(   getPrefix( *it ) );
          }
          else 
          {
            aName = getNewObjectName();
            // PAL6521: use a prefix, if some dialog box doesn't reimplement getNewObjectName()
            if ( aName.isEmpty() )
              aName = GEOMBase::GetDefaultName( getPrefix( *it ) );
          }
          addInStudy( *it, aName.latin1() );
          withChildren = false;
          display( *it, false );
        }

        if ( nbObjs ) 
        {
          commitCommand();
          updateObjBrowser();
          SUIT_Session::session()->activeApplication()->putInfo( QObject::tr("GEOM_PRP_DONE") );
        }
        else 
        {
          abortCommand();
        }

        // JFA 28.12.2004 BEGIN // To enable warnings
        if ( !getOperation()->_is_nil() ) 
        {
          if ( !getOperation()->IsDone() ) 
          {
            wc.suspend();
      	    QString msgw = QObject::tr( getOperation()->GetErrorCode() );
            SUIT_MessageBox::warn1((QWidget*)(SUIT_Session::session()->activeApplication()->desktop()),
				   QObject::tr( "WRN_WARNING" ), 
				   msgw, 
				   QObject::tr( "BUT_OK" ));
          }
        }
        // JFA 28.12.2004 END
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) 
  {
    SalomeApp_Tools::QtCatchCorbaException( e );
    abortCommand();
  }

  updateViewer();
  activateSelection();
  updateButtonState();

  return true;
}


//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::onDetect()
{
  clearTemporary();
  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return;
  }

  GEOM::ListOfGO_var aList = GEOM::GEOM_IShapesOperations::_narrow
    ( getOperation() )->GetGlueFaces( myObject, myTolEdt2->value() );
  
  for (int i = 0, n = aList->length(); i < n; i++) 
    myTmpObjs.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
  
  if ( myTmpObjs.size() > 0  )
  {
    msg = tr( "FACES_FOR_GLUING_ARE_DETECTED" ).arg( myTmpObjs.size() );
    mySubShapesChk->setChecked( true );
  }
  else
  {
    msg = tr( "THERE_ARE_NO_FACES_FOR_GLUING" );
  }
  
  connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	   SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) ) ;
  SUIT_MessageBox::info1( this, tr( "GEOM_FREE_BOUNDS_TLT" ), msg, "Close" );
  updateButtonState();
  activateSelection();
}

//=================================================================================
// function : activateSelection
// purpose  : Redisplay preview and Activate selection
//=================================================================================
void RepairGUI_GlueDlg::activateSelection()
{
  erasePreview(false);
  
  int anId = getConstructorId();
  if ( anId == 0 )  // Case of whole gluing
  {
    disconnect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
		SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) ) ;
    
    globalSelection( GEOM_ALLSHAPES );
    if (myObject->_is_nil()) 
      SelectionIntoArgument();

    connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	     SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) ) ;
  } 
  else // Second case of gluing
  {
    if ( !mySubShapesChk->isChecked() ) 
      globalSelection( GEOM_ALLSHAPES );
    else 
    {
      displayPreview( true, false, false, 2/*line width*/, 1/*display mode*/, Quantity_NOC_RED );
     disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
		SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) ) ;
      globalSelection( GEOM_PREVIEW );
      connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	       SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) ) ;
    } 
  }
  updateViewer();
}

//=================================================================================
// function : updateButtonState
// purpose  : Update button state 
//=================================================================================
void RepairGUI_GlueDlg::updateButtonState()
{
  int anId = getConstructorId();
  bool hasMainObj = !myObject->_is_nil();
  if ( anId == 0 )
  {
    buttonOk->setEnabled( hasMainObj );
    buttonApply->setEnabled( hasMainObj );
  }
  else
  {
    bool wasSelected = false;
    SALOME_ListIteratorOfListIO it ( selectedIO() );
    if (it.More() > 0)
      wasSelected = true;
    bool wasDetected = myTmpObjs.size() ? true : false;
    buttonOk->setEnabled( hasMainObj && wasDetected && wasSelected);
    buttonApply->setEnabled( hasMainObj && wasDetected && wasSelected);
    mySubShapesChk->setEnabled( hasMainObj && wasDetected );
    myDetectBtn->setEnabled( hasMainObj );
    if ( !hasMainObj || !wasDetected )
      mySubShapesChk->setChecked( false );
  }
}

//=================================================================================
// function : clearTemporary
// purpose  : Remove temporary objects from engine
//=================================================================================
void RepairGUI_GlueDlg::clearTemporary()
{
  ObjectList::iterator anIter;
  for (anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter)
    getGeomEngine()->RemoveObject(*anIter);

  myTmpObjs.clear();
}

//=================================================================================
// function : onTolerChanged
// purpose  : Remove temporary objects from engine
//=================================================================================
void RepairGUI_GlueDlg::onTolerChanged( double /*theVal*/ )
{
  clearTemporary();
  activateSelection();
  updateButtonState();
}

//=================================================================================
// function : onSubShapesChk
// purpose  : Update selection mode
//=================================================================================
void RepairGUI_GlueDlg::onSubShapesChk()
{
  if ( !mySubShapesChk->isChecked() )
    clearTemporary();
  activateSelection();
  updateButtonState();
}

//=================================================================================
// function : ClickOnCancel
// purpose  : Remove temporary objects from engine and call method of base class
//=================================================================================
void RepairGUI_GlueDlg::ClickOnCancel()
{
  clearTemporary();
  GEOMBase_Skeleton::ClickOnCancel();
}

//================================================================
// Function : getEntry
// Purpose  :
//================================================================
static string getEntry( GEOM::GEOM_Object_ptr object )
{
  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app )
  {
    string IOR = app->orb()->object_to_string( object );
    if ( IOR != "" )
    {
      SalomeApp_Study* study = ( SalomeApp_Study* )app->activeStudy();
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( IOR ) );
      if ( SO )
        return SO->GetID();
    }
  }
  return "";
}

















