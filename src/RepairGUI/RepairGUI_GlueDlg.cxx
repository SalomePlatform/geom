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
//  File   : RepairGUI_GlueDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "RepairGUI_GlueDlg.h"

#include "DlgRef_1Sel_Ext.h"

#include "QAD_Desktop.h"
#include "QAD_SpinBoxDbl.h"
#include "QAD_MessageBox.h"
#include "QAD_WaitCursor.h"

#include "OCCViewer_Viewer3d.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "GEOMImpl_Types.hxx"

#define DEFAULT_TOLERANCE_VALUE 1e-07


//=================================================================================
// class    : RepairGUI_GlueDlg()
// purpose  : Constructs a RepairGUI_GlueDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_GlueDlg::RepairGUI_GlueDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_GLUE_FACES")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_GLUE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_GLUE_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_GLUE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  QGridLayout* aLay = new QGridLayout( 0, 2, 2, 0, 6, "aLay" );
  QLabel* aLbl1 = new QLabel( tr( "GEOM_TOLERANCE" ), GroupPoints->GroupBox1 );
  myTolEdt = new QAD_SpinBoxDbl( GroupPoints->GroupBox1, 0, 100, 1e-7, 10, 1e-10 );
  myTolEdt->setValue( DEFAULT_TOLERANCE_VALUE );

  aLay->addWidget( aLbl1, 0, 0 );
  aLay->addWidget( myTolEdt, 0, 1 );

  GroupPoints->getGroupBoxLayout()->addLayout( aLay, 3, 0 );
  /***************************************************************/

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

  myGeomGUI->SetState( 0 );
  globalSelection( GEOM_COMPOUND );

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GLUE_NEW_OBJ_NAME" ) );
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

  GroupPoints->LineEdit1->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  globalSelection( GEOM_COMPOUND );

  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_GlueDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  if ( mySelection->IObjectCount() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = mySelection->firstIObject();
    Standard_Boolean aRes;
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( aRes )
      myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 )  {
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
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::DeactivateActiveDialog()
{
  myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  GroupPoints->LineEdit1->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  myGeomGUI->SetState( 0 );
  globalSelection( GEOM_COMPOUND );
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
  myGeomGUI->SetState( -1 );
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
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::_narrow
    ( getOperation() )->MakeGlueFaces( myObject, myTolEdt->value() );
  aResult = !anObj->_is_nil();
  if ( aResult )
    objects.push_back( anObj._retn() );

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

  string IOR = GeometryGUI::GetORB()->object_to_string( theObj );
  TCollection_AsciiString asciiIOR( strdup( IOR.c_str() ) );
  GeometryGUI::GetGeomGUI()->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );

  if ( !getStudy() || CORBA::is_nil( getStudy()->getStudyDocument() ) )
    return;

  SALOMEDS::Study_var aStudy = getStudy()->getStudyDocument();
  SALOMEDS::SObject_var aSObj = aStudy->FindObjectIOR( IOR.c_str() );
  if ( CORBA::is_nil( aSObj ) )
    return;

  SALOMEDS::ChildIterator_var anIt = aStudy->NewChildIterator( aSObj );
  for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
    SALOMEDS::GenericAttribute_var anAttr;
    if ( anIt->Value()->FindAttribute(anAttr, "AttributeIOR") ) {
      SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
      TCollection_AsciiString asciiIOR( anIOR->Value() );
      GeometryGUI::GetGeomGUI()->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );      
    }
  }
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it 
//            performs a proper operation and manages transactions, etc.
//================================================================
bool RepairGUI_GlueDlg::onAcceptLocal( const bool publish, const bool useTransaction )
{
  QAD_Study* aDoc = QAD_Application::getDesktop()->getActiveStudy();
  SALOMEDS::Study_var aStudy = aDoc->getStudyDocument();

  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    MESSAGE("GEOMBase_Helper::onAccept - ActiveStudy is locked");
    QAD_MessageBox::warn1 ( (QWidget*)QAD_Application::getDesktop(),
			   QObject::tr("WRN_WARNING"), 
			   QObject::tr("WRN_STUDY_LOCKED"),
			   QObject::tr("BUT_OK") );
    return false;
  }

  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return false;
  }

  erasePreview( false );

  try {
    if ( ( !publish && !useTransaction ) || openCommand() ) {
      QAD_WaitCursor wc;
      QAD_Application::getDesktop()->putInfo( "" );
      ObjectList objects;
      // JFA 28.12.2004 if ( !execute( objects ) || !getOperation()->IsDone() ) {
      if ( !execute( objects ) ) { // JFA 28.12.2004 // To enable warnings
	wc.stop();
	abortCommand();
	showError();
      }
      else {
	const int nbObjs = objects.size();
	bool withChildren = false;
	for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it ) {
	  if ( publish ) {
	    QString aName("");
	    if ( nbObjs > 1 )
	      aName = strlen( getNewObjectName() ) ? GEOMBase::GetDefaultName( getNewObjectName() ) : GEOMBase::GetDefaultName( getPrefix( *it ) );
	    else {
	      aName = getNewObjectName();
	      // PAL6521: use a prefix, if some dialog box doesn't reimplement getNewObjectName()
	      if ( aName.isEmpty() )
		aName = GEOMBase::GetDefaultName( getPrefix( *it ) );
	    }
	    addInStudy( *it, aName.latin1() );
	    withChildren = false;
	    display( *it, false );
	  }
	  else { // asv : fix of PAL6454. If publish==false, then the original shape was modified, and need to be re-cached in GEOM_Client 
	         // before redisplay
	    clearShapeBufferLocal( *it );
	    withChildren = true;
	    redisplay( *it, withChildren, false );
          }
	}

	if ( nbObjs ) {
	  commitCommand();
	  updateObjBrowser();
	  QAD_Application::getDesktop()->putInfo( QObject::tr("GEOM_PRP_DONE") );
	}
	else {
	  abortCommand();
        }

        // JFA 28.12.2004 BEGIN // To enable warnings
        if ( !getOperation()->_is_nil() ) {
          if ( !getOperation()->IsDone() ) {
            wc.stop();
            QString msgw = QObject::tr( getOperation()->GetErrorCode() );
            QAD_MessageBox::warn1(QAD_Application::getDesktop(), 
                                  QObject::tr( "WRN_WARNING" ), 
                                  msgw, 
                                  QObject::tr( "BUT_OK" ));
          }
        }
        // JFA 28.12.2004 END
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    QtCatchCorbaException( e );
    abortCommand();
  }

  updateViewer();

  return true;
}
