//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMBase_Helper.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include "GEOMBase_Helper.h"
#include "GEOMBase.h"
#include "GEOM_Operation.h"
#include "GeometryGUI.h"
#include "GEOM_Displayer.h"
#include "GEOMImpl_Types.hxx"
#include "QAD_Study.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_MessageBox.h"
#include "QAD_ViewFrame.h"
#include "QAD_RightFrame.h"
#include "QAD_WaitCursor.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include <TColStd_MapOfInteger.hxx>
#include <TCollection_AsciiString.hxx>

using namespace std;

//================================================================
// Function : getActiveView
// Purpose  : Get active study frame, returns 0 if no open study frame
//================================================================
static QAD_ViewFrame* getActiveView()
{
  QAD_Study* activeStudy = QAD_Application::getDesktop()->getActiveStudy();
  if ( activeStudy ) {
    QAD_StudyFrame* sf = activeStudy->getActiveStudyFrame();
    if ( sf ) {
      return sf->getRightFrame()->getViewFrame();
    }
  }
  return 0;
}


//================================================================
// Function : getGeomEngine
// Purpose  : Static method
//================================================================
GEOM::GEOM_Gen_ptr GEOMBase_Helper::getGeomEngine()
{
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  return GEOM::GEOM_Gen::_narrow(comp);
}

//================================================================
// Function : GEOMBase_Helper
// Purpose  :
//================================================================
GEOMBase_Helper::GEOMBase_Helper() 
  : myViewFrame( 0 ), myDisplayer( 0 ), myCommand( 0 ), isPreview( false )
{
}

//================================================================
// Function : ~GEOMBase_Helper
// Purpose  : 
//================================================================
GEOMBase_Helper::~GEOMBase_Helper()
{
  if ( myPreview.size() )
    erasePreview();
  if ( hasCommand() )
    abortCommand();

  globalSelection( GEOM_ALLOBJECTS, true );

  delete myDisplayer;
}

//================================================================
// Function : display
// Purpose  : 
//================================================================
void GEOMBase_Helper::display( const ObjectList& objList, const bool updateView )
{
  ObjectList::const_iterator it;
  for ( it = objList.begin(); it != objList.end(); it++ ) {
    display( *it, false );
  }
  if ( !objList.empty() && updateView )
    getDisplayer()->UpdateViewer();
}

//================================================================
// Function  : display
// Purpose   : Display object.
// Important : Object must be already in study
//================================================================
void GEOMBase_Helper::display( GEOM::GEOM_Object_ptr object, const bool updateView )
{
  // Unset color of shape ( this color may be set during preview displaying )
  // Default color will be used
  getDisplayer()->UnsetColor();
  getDisplayer()->UnsetWidth();

  // Enable activisation of selection
  getDisplayer()->SetToActivate( true );

  // Display object
  getDisplayer()->Display( object, updateView );
}

//================================================================
// Function : erase
// Purpose  : 
//================================================================
void GEOMBase_Helper::erase( const ObjectList& objList, const bool updateView )
{
  ObjectList::const_iterator it = objList.begin();
  for ( ; it != objList.end(); it++ ) {
    erase( *it, false );
  }
  if ( !objList.empty() && updateView )
    getDisplayer()->UpdateViewer();
}

//================================================================
// Function : erase
// Purpose  : 
//================================================================
void GEOMBase_Helper::erase( GEOM::GEOM_Object_ptr object, const bool updateView )
{
  if ( !object->_is_nil() ) {
    string entry = getEntry( object );
    getDisplayer()->Erase( new SALOME_InteractiveObject(
      entry.c_str(), "GEOM", strdup( GEOMBase::GetName( object ) ) ), true, updateView );
  }
}

//================================================================
// Function : redisplay
// Purpose  : 
//================================================================
void GEOMBase_Helper::redisplay( const ObjectList& objList, 
				 const bool withChildren,
				 const bool updateView )
{
  ObjectList::const_iterator it = objList.begin();
  for ( ; it != objList.end(); it++ ) {
    redisplay( *it, withChildren, false );
  }
  if ( !objList.empty() && updateView )
    getDisplayer()->UpdateViewer();
}

//================================================================
// Function : redisplay
// Purpose  : 
//================================================================
void GEOMBase_Helper::redisplay( GEOM::GEOM_Object_ptr object, 
				 const bool withChildren,
				 const bool updateView )
{
  if ( !object->_is_nil() ) {
    // Unset color of shape ( this color may be set during preview displaying )
    // Default color will be used
    getDisplayer()->UnsetColor();
    getDisplayer()->UnsetWidth();
    
    // Enable activisation of selection
    getDisplayer()->SetToActivate( true );
    
    string entry = getEntry( object );
    getDisplayer()->Redisplay( new SALOME_InteractiveObject( entry.c_str(), "GEOM", strdup( GEOMBase::GetName( object ) ) ), false );
  }
  
  if ( withChildren ) {
    QAD_Study* aDoc = getStudy();
    if ( aDoc && !CORBA::is_nil( aDoc->getStudyDocument() ) ) {
      SALOMEDS::Study_var aStudy = aDoc->getStudyDocument();
      SALOMEDS::SObject_var aSObj = aStudy->FindObjectIOR( GeometryGUI::GetORB()->object_to_string( object ) );
      if ( !CORBA::is_nil( aSObj ) ) {
	SALOMEDS::ChildIterator_var anIt = aStudy->NewChildIterator( aSObj );
	for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
	  GEOM::GEOM_Object_var aChild = GEOM::GEOM_Object::_narrow( anIt->Value()->GetObject() );
	  if ( !CORBA::is_nil( aChild ) ) {
	    if ( !aChild->_is_nil() ) {
	      string entry = getEntry( aChild );
	      getDisplayer()->Redisplay( new SALOME_InteractiveObject(
                entry.c_str(), "GEOM", strdup( GEOMBase::GetName( aChild ) ) ), false );
	    }
	  }
	}
      }    
    }
  }
  
  if ( updateView )
    getDisplayer()->UpdateViewer();
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying preview based on execute() results
//================================================================
void GEOMBase_Helper::displayPreview( const bool   activate,
                                      const bool   update,
                                      const bool   toRemoveFromEngine,
                                      const double lineWidth )
{
  isPreview = true; 
  QString msg;
  if ( !isValid( msg ) )
  {
    erasePreview( update );
    isPreview = false; 
    return;
  }

  erasePreview( false );

  try {
    QAD_WaitCursor wc;
    ObjectList objects;
    if ( !execute( objects ) || !getOperation()->IsDone() ) {
      wc.stop();
    }
    else {
      for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it )
      {
        displayPreview( *it, true, activate, false, lineWidth );
        if ( toRemoveFromEngine )
          getGeomEngine()->RemoveObject( *it );
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    QtCatchCorbaException( e );
  }

  isPreview = false;

  if ( update )
    updateViewer();
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying preview of resulting shape
//================================================================
void GEOMBase_Helper::displayPreview( GEOM::GEOM_Object_ptr object,
                                      const bool            append,
                                      const bool            activate, 
                                      const bool            update,
                                      const double          lineWidth )
{
  // Set color for preview shape
  getDisplayer()->SetColor( Quantity_NOC_VIOLET );

  // set width of displayed shape
  getDisplayer()->SetWidth( lineWidth );

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  getDisplayer()->SetName( GeometryGUI::GetORB()->object_to_string( object ) );
  
  // Build prs
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( object );
  if ( aPrs == 0 || aPrs->IsNull() )
    return;

  // Display prs
  displayPreview( aPrs, append, update );

  getDisplayer()->UnsetName();

  // Enable activation of displayed objects
  getDisplayer()->SetToActivate( true );
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying arbitrary preview objects (not limited to shapes)
//================================================================
void GEOMBase_Helper::displayPreview( const SALOME_Prs* prs,
                                      const bool        append,
                                      const bool        update )
{
  if ( !append )
    erasePreview( false );

  // remember current view frame to make correct erase preview later
  myViewFrame = getActiveView();

  if ( myViewFrame == 0 )
    return;

  // Display prs
  ( (SALOME_View*)myViewFrame )->Display( prs );

  // Add prs to the preview list
  myPreview.push_back( (SALOME_Prs*)prs );

  // Update viewer
  if ( update )
    myViewFrame->Repaint();
}

//================================================================
// Function : erasePreview
// Purpose  : 
//================================================================
void GEOMBase_Helper::erasePreview( const bool update )
{
  // check view frame where the preview was displayed
  bool vfOK = checkViewFrame() && myViewFrame;
  // Iterate through presentations and delete them
  for ( PrsList::iterator anIter = myPreview.begin(); anIter != myPreview.end(); ++anIter ) {
    if ( vfOK )
      ( (SALOME_View*)myViewFrame )->Erase( *anIter, true );
    delete *anIter;
  }
  myPreview.clear();

  // Update viewer
  if ( update )
    updateViewer();
}

//================================================================
// Function  : localSelection
// Purpose   : Activate selection of objects of a given type
// IMPORTANT : Works after localSelection( ... ) method call only
//================================================================
void GEOMBase_Helper::activate( const int theType )
{
  SALOMEDS::Study_var aStudy = getStudy()->getStudyDocument();
  SALOMEDS::SComponent_var aGeom = aStudy->FindComponent( "GEOM" );
  if ( aGeom->_is_nil() )
    return;

  SALOME_ListIO aList;
  SALOMEDS::ChildIterator_var anIter = aStudy->NewChildIterator( aGeom );
  for ( ; anIter->More(); anIter->Next() )
  {
    SALOMEDS::SObject_var aSO = anIter->Value();
    if ( !aSO->_is_nil() )
    {
      SALOMEDS::SObject_var aRefSO;
      if ( !aSO->ReferencedObject( aRefSO ) )
      {
        GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_narrow( aSO->GetObject() );
        if ( !anObj->_is_nil() && anObj->GetType() == theType )
          aList.Append( new SALOME_InteractiveObject( aSO->GetID(), "GEOM", aSO->GetName()) );
      }
    }
  }

  myDisplayer->LocalSelection( aList, 0 );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of subshapes in accordance with mode
//            theMode is from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( const ObjectList& theObjs, const int theMode )
{
  SALOME_ListIO aListOfIO;
  
  ObjectList::const_iterator anIter = theObjs.begin();
  for ( ; anIter != theObjs.end(); ++anIter )
  {
    GEOM::GEOM_Object_ptr anObj = *anIter;
    if ( anObj->_is_nil() )
      continue;
    string aEntry = getEntry( anObj );
    if ( aEntry != "" )
      aListOfIO.Append( new SALOME_InteractiveObject(
        aEntry.c_str(), "GEOM", strdup( GEOMBase::GetName( anObj ) ) ) );
  }

  myDisplayer->LocalSelection( aListOfIO, theMode );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of subshapes in accordance with mode
//            theMode is from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( GEOM::GEOM_Object_ptr obj, const int mode )
{
  // If object is null local selection for all objects is activated
  if ( obj->_is_nil() ) {
    myDisplayer->LocalSelection( Handle(SALOME_InteractiveObject)(), mode );
    return;
  }

  ObjectList objList;
  objList.push_back( obj );
  localSelection( objList, mode );
}


//================================================================
// Function : globalSelection
// Purpose  : Activate selection of subshapes. Set selection filters
//            in accordance with mode. theMode is from GEOMImpl_Types
//================================================================
void GEOMBase_Helper::globalSelection( const int theMode, const bool update )
{
  myDisplayer->GlobalSelection( theMode, update );
}

//================================================================
// Function : globalSelection
// Purpose  : Activate selection of subshapes. Set selection filters
//            in accordance with mode. theMode is from GEOMImpl_Types
//================================================================
void GEOMBase_Helper::globalSelection( const TColStd_MapOfInteger& theModes,
				       const bool update )
{
  myDisplayer->GlobalSelection( theModes, update );
}

//================================================================
// Function : addInStudy
// Purpose  : Add object in study
//================================================================
void GEOMBase_Helper::addInStudy( GEOM::GEOM_Object_ptr theObj, const char* theName )
{
  if ( !hasCommand() )
    return;

  SALOMEDS::Study_var aStudy = getStudy()->getStudyDocument();
  if ( aStudy->_is_nil() || theObj->_is_nil() )
    return;

  GEOM::GEOM_Object_ptr aFatherObj = getFather( theObj );

  GeometryGUI::GetGeomGUI()->GetGeomGen()->AddInStudy( aStudy, theObj, theName, aFatherObj );
}

//================================================================
// Function : updateObjBrowser
// Purpose  : Update object browser
//================================================================
void GEOMBase_Helper::updateObjBrowser() const
{
  getStudy()->updateObjBrowser();
}

//================================================================
// Function : updateViewer
// Purpose  : Update active 3D view
//================================================================
void GEOMBase_Helper::updateViewer()
{
  getDisplayer()->UpdateViewer();
}

//================================================================
// Function : getStudyId
// Purpose  : Get study Id
//================================================================
int GEOMBase_Helper::getStudyId() const
{
  int anId = -1;
  if ( getStudy() )
    anId = getStudy()->getStudyId();
  return anId;
}

//================================================================
// Function : getStudy
// Purpose  : Returns the active study. It is recommended to use 
//            this method instead of direct desktop->getActiveStudy() calls
//================================================================
QAD_Study* GEOMBase_Helper::getStudy() const
{
  // Probably, it would be better to rememeber the active study in helper...
  return QAD_Application::getDesktop()->getActiveStudy();
}

//================================================================
// Function : getEntry
// Purpose  : 
//================================================================
char* GEOMBase_Helper::getEntry( GEOM::GEOM_Object_ptr object ) const
{
  QAD_Study* study = getStudy();
  if ( study )  {
    string IOR = GEOMBase::GetIORFromObject( object);
    if ( IOR != "" ) {
      SALOMEDS::SObject_var SO = study->getStudyDocument()->FindObjectIOR( IOR.c_str() );
      if ( !SO->_is_nil() ) {
	return SO->GetID();
      }
    }
  }
  return "";
}

//================================================================
// Function : getDisplayer
// Purpose  : 
//================================================================
GEOM_Displayer* GEOMBase_Helper::getDisplayer()
{
  if ( !myDisplayer )
    myDisplayer = new GEOM_Displayer();
  return myDisplayer;
}

//================================================================
// Function : clearShapeBuffer
// Purpose  : 
//================================================================
void GEOMBase_Helper::clearShapeBuffer( GEOM::GEOM_Object_ptr theObj )
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
// Function : openCommand
// Purpose  : 
//================================================================
bool GEOMBase_Helper::openCommand() 
{
  bool res = false;
  if ( !getStudy() || hasCommand() )
    return res;
  
  GEOM::GEOM_IOperations_var anOp = GEOM::GEOM_IOperations::_narrow( getOperation() );
  if ( !anOp->_is_nil() ) {
    myCommand = new GEOM_Operation( getStudy(), anOp.in() );
    myCommand->start();
    res = true;
  }
 
  return res;
}

//================================================================
// Function : abortCommand
// Purpose  : 
//================================================================
bool GEOMBase_Helper::abortCommand()
{
  if ( !hasCommand() )
    return false;

  myCommand->abort();
  myCommand = 0;
 
  return true;  
}

//================================================================
// Function : commitCommand
// Purpose  : 
//================================================================
bool GEOMBase_Helper::commitCommand( const char* )
{
  if ( !hasCommand() )
    return false;

  myCommand->finish();
  myCommand = 0;

  return true;  
}

//================================================================
// Function : hasCommand
// Purpose  : 
//================================================================
bool GEOMBase_Helper::hasCommand() const
{
  return (bool)myCommand;
}

//================================================================
// Function : getOperation
// Purpose  : 
//================================================================
GEOM::GEOM_IOperations_ptr GEOMBase_Helper::getOperation()
{
  if ( myOperation->_is_nil() )
    myOperation = createOperation();

  return myOperation;
}



//================================================================
// Function : checkViewFrame
// Purpose  : 
//================================================================
bool GEOMBase_Helper::checkViewFrame()
{
  if ( myViewFrame ){
    QListIterator<QAD_Study> it( QAD_Application::getDesktop()->getActiveApp()->getStudies() );
    for ( ; it.current(); ++it ) {
      QListIterator<QAD_StudyFrame> it1( it.current()->getStudyFrames() );
      for ( ; it1.current(); ++it1 ) {
	if ( myViewFrame == it1.current()->getRightFrame()->getViewFrame() )
	  return true;
      }
    }
  }
  myViewFrame = 0;
  return false;
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it 
//            performs a proper operation and manages transactions, etc.
//================================================================
bool GEOMBase_Helper::onAccept( const bool publish, const bool useTransaction )
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
      if ( !execute( objects ) || !getOperation()->IsDone() ) {
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
	    clearShapeBuffer( *it );
	    withChildren = true;
	    redisplay( *it, withChildren, false );
          }
	}

	if ( nbObjs ) {
	  commitCommand();
	  updateObjBrowser();
	  QAD_Application::getDesktop()->putInfo( QObject::tr("GEOM_PRP_DONE") );
	}
	else
	  abortCommand();
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


//================================================================
// Function : showError
// Purpose  : Shows a message box with infromation about an error taken from getOperation()->GetErrorCode()
//================================================================
void GEOMBase_Helper::showError()
{
  QString msg;
  if ( !getOperation()->_is_nil() )
    msg = QObject::tr( getOperation()->GetErrorCode() );

  if ( msg.isEmpty() )
    msg = QObject::tr( "GEOM_PRP_ABORT" );

  QAD_MessageBox::error1( QAD_Application::getDesktop(), 
			  QObject::tr( "GEOM_ERROR_STATUS" ), 
			  msg, 
			  QObject::tr( "BUT_OK" ) );
}

//================================================================
// Function : showError
// Purpose  : Shows a error message followed by <msg>
//================================================================
void GEOMBase_Helper::showError( const QString& msg )
{
  QString str( QObject::tr( "GEOM_INCORRECT_INPUT" ) );
  if ( !msg.isEmpty() )
    str += "\n" + msg;
  QAD_MessageBox::error1( QAD_Application::getDesktop(), QObject::tr( "GEOM_ERROR" ), str, QObject::tr( "BUT_OK" ) );
}

//////////////////////////////////////////////////////////////////
// Virtual methods to be redefined in dialogs
//////////////////////////////////////////////////////////////////

//================================================================
// Function : createOperation
// Purpose  : Redefine this method to return proper IOperation reference
//================================================================
GEOM::GEOM_IOperations_ptr GEOMBase_Helper::createOperation()
{
  GEOM::GEOM_IOperations_var aNilOp;
  return aNilOp._retn();
}

//================================================================
// Function : isValid
// Purpose  : Called by onAccept(). Redefine this method to check validity of user input in dialog boxes.
//================================================================
bool GEOMBase_Helper::isValid( QString& )
{
  return true;
}

//================================================================
// Function : execute
// Purpose  : This method is called by onAccept(). 
//            It should perform the required operation and put all new or modified objects into 
//            <objects> argument.Should return <false> if some error occurs during its execution.
//================================================================
bool GEOMBase_Helper::execute( ObjectList& objects )
{
  return false;
}

//================================================================
// Function : getFather
// Purpose  : This method is called by addInStudy(). It should return a father object
//            for <theObj> or a nil reference if <theObj> should be published
//            as a top-level object.
//================================================================
GEOM::GEOM_Object_ptr GEOMBase_Helper::getFather( GEOM::GEOM_Object_ptr theObj )
{
  return GEOM::GEOM_Object::_nil();
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
const char* GEOMBase_Helper::getNewObjectName() const
{
  return "";
}

//================================================================
// Function : getPrefix
// Purpose  : Get prefix for name of created object
//================================================================
QString GEOMBase_Helper::getPrefix( GEOM::GEOM_Object_ptr theObj ) const
{
  if ( !myPrefix.isEmpty() || theObj->_is_nil() )
    return myPrefix;

  TopoDS_Shape aShape;
  if ( !GEOMBase::GetShape( theObj, aShape ) )
    return "";
  
  long aType = aShape.ShapeType();
  
  switch ( aType )
  {
    case TopAbs_VERTEX   : return QObject::tr( "GEOM_VERTEX" );
    case TopAbs_EDGE     : return QObject::tr( "GEOM_EDGE" );
    case TopAbs_WIRE     : return QObject::tr( "GEOM_WIRE" );
    case TopAbs_FACE     : return QObject::tr( "GEOM_FACE" );
    case TopAbs_SHELL    : return QObject::tr( "GEOM_SHELL" );
    case TopAbs_SOLID    : return QObject::tr( "GEOM_SOLID" );
    case TopAbs_COMPSOLID: return QObject::tr( "GEOM_COMPOUNDSOLID" );
    case TopAbs_COMPOUND : return QObject::tr( "GEOM_COMPOUND" );
    default : return "";
  }
}


