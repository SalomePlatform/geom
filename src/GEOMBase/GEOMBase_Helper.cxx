// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GEOMBase_Helper.cxx
//  Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#include "GEOMBase_Helper.h"
#include "GEOMBase.h"
#include "GEOM_Operation.h"

#include <GeometryGUI.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewModel.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>

#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_DataOwner.h>
#include <SalomeApp_Tools.h>
#include <SALOME_ListIO.hxx>
#include "utilities.h"

#include <SALOME_Prs.h>
#include "utilities.h"

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>

#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//To disable automatic genericobj management, the following line should be commented.
//Otherwise, it should be uncommented. Refer to KERNEL_SRC/src/SALOMEDSImpl/SALOMEDSImpl_AttributeIOR.cxx
#define WITHGENERICOBJ

//================================================================
// Function : getActiveView
// Purpose  : Get active view window, returns 0 if no open study frame
//================================================================
SUIT_ViewWindow* GEOMBase_Helper::getActiveView()
{
  SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
  if ( activeStudy )
    return SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  return 0;
}


//================================================================
// Function : getGeomEngine
// Purpose  : Static method
//================================================================
GEOM::GEOM_Gen_ptr GEOMBase_Helper::getGeomEngine()
{
  return GeometryGUI::GetGeomGen();
}

//================================================================
// Function : GEOMBase_Helper
// Purpose  :
//================================================================
GEOMBase_Helper::GEOMBase_Helper( SUIT_Desktop* desktop )
  : myDesktop( desktop ), myViewWindow( 0 ), myDisplayer( 0 ), myCommand( 0 ), isPreview( false ),
    myIsApplyAndClose( false ), myIsOptimizedBrowsing( false ), myIsWaitCursorEnabled( true ),
    myIsDisableBrowsing(false), myIsDisplayResult(true)
{
}

//================================================================
// Function : ~GEOMBase_Helper
// Purpose  :
//================================================================
GEOMBase_Helper::~GEOMBase_Helper()
{
  //rnv: Fix for the "IPAL21922 : WinTC5.1.4: incorrect quit salome"
  if ( !SUIT_Session::session()->activeApplication() || !SUIT_Session::session()->activeApplication()->desktop() )
    return;

  if ( myPreview.size() )
    erasePreview();
  if ( hasCommand() )
    abortCommand();
  SalomeApp_Application* app = (SalomeApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    if(aGeomGUI)
      globalSelection(aGeomGUI->getLocalSelectionMode() , true );
  }

  if (myDisplayer)
    delete myDisplayer;
  if ( !CORBA::is_nil( myOperation ) )
    myOperation->UnRegister();
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
//   getDisplayer()->UnsetColor();
  getDisplayer()->UnsetWidth();
  
  MESSAGE("GEOMBase_Helper::display myTexture = "<<getDisplayer()->GetTexture())

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
    QString entry = getEntry( object );
    getDisplayer()->Erase( new SALOME_InteractiveObject(
      entry.toLatin1().constData(), 
      "GEOM", strdup( GEOMBase::GetName( object ).toLatin1().constData() ) ), true, updateView );
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

    QString entry = getEntry( object );
    getDisplayer()->Redisplay(new SALOME_InteractiveObject
                              (entry.toLatin1().constData(), "GEOM", strdup(GEOMBase::GetName(object).toLatin1().constData())), false);
  }

  if ( withChildren ) {
    SalomeApp_Study* aDoc = getStudy();
    if ( aDoc && aDoc->studyDS() ) {
      _PTR(Study) aStudy = aDoc->studyDS();
      CORBA::String_var objStr = SalomeApp_Application::orb()->object_to_string(object);
      _PTR(SObject) aSObj (aStudy->FindObjectIOR(std::string(objStr.in())));
      if ( aSObj  ) {
        _PTR(ChildIterator) anIt ( aStudy->NewChildIterator( aSObj ) );
        for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
          GEOM::GEOM_Object_var aChild = GEOM::GEOM_Object::_narrow
            (GeometryGUI::ClientSObjectToObject(anIt->Value()));
          if ( !CORBA::is_nil( aChild ) ) {
            if ( !aChild->_is_nil() ) {
              QString entry = getEntry( aChild );
              getDisplayer()->Redisplay( new SALOME_InteractiveObject(
                entry.toLatin1().constData(), "GEOM", strdup( GEOMBase::GetName( aChild ).toLatin1().constData() ) ), false );
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
void GEOMBase_Helper::displayPreview( const bool   display,
                                      const bool   activate,
                                      const bool   update,
                                      const bool   toRemoveFromEngine,
                                      const double lineWidth,
                                      const int    displayMode,
                                      const int    color,
                                      const bool   append )
{
  if(!display) {
    erasePreview( update );
    return;
  }
  
  isPreview = true;
  QString msg;
  if ( !isValid( msg ) )
  {
    erasePreview( update );
    isPreview = false;
    return;
  }

  if( !append )
    erasePreview( false );

  try {
    SUIT_OverrideCursor wc;
    ObjectList objects;
 
    if ( !isWaitCursorEnabled() )
      wc.suspend();
    
    if ( !execute( objects ) || !getOperation()->IsDone() ) {
      wc.suspend();
    }
    else {
      for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it )
      {
            GEOM::GEOM_Object_var obj=*it;
        displayPreview( obj, true, activate, false, lineWidth, displayMode, color );
        if ( toRemoveFromEngine )
              obj->UnRegister();
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
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
                                      const double          lineWidth,
                                      const int             displayMode,
                                      const int             color )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();    

  // Set color for preview shape
  getDisplayer()->SetColor( color == -1 ? Quantity_NOC_VIOLET : color );

  // set width of displayed shape
  getDisplayer()->SetWidth( lineWidth == -1 ? resMgr->integerValue("Geometry", "preview_edge_width", -1) : lineWidth );

  // set display mode of displayed shape
  int aPrevDispMode = getDisplayer()->SetDisplayMode( displayMode == -1 ? resMgr->integerValue( "Geometry", "display_mode", 0 ) : displayMode );

  // Disable activation of selection
  getDisplayer()->SetToActivate( activate );

  // Make a reference to GEOM_Object
  CORBA::String_var objStr = SalomeApp_Application::orb()->object_to_string( object );
  getDisplayer()->SetName( objStr.in() );

  // Build prs
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs( object );
  if ( aPrs == 0 || aPrs->IsNull() )
    return;
    
  // Make preview not clippable
  aPrs->SetClippable (false);

  // Display prs
  displayPreview( aPrs, append, update );

  getDisplayer()->UnsetName();
  getDisplayer()->UnsetColor();
  getDisplayer()->SetDisplayMode( aPrevDispMode );

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
  myViewWindow = getActiveView();

  if ( myViewWindow == 0 )
    return;
  
  // Display prs
  SUIT_ViewManager* aViewManager = myViewWindow->getViewManager();
  if ( aViewManager->getType() == OCCViewer_Viewer::Type() ||
       aViewManager->getType() == SVTK_Viewer::Type() )
    {
      SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
      SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
      if (aView)
        aView->Display( getDisplayer(), prs );
    }

  // Add prs to the preview list
  myPreview.push_back( (SALOME_Prs*)prs );

  // Update viewer
  if ( update )
    getDisplayer()->UpdateViewer();
}

//================================================================
// Function : erasePreview
// Purpose  :
//================================================================
void GEOMBase_Helper::erasePreview( const bool update )
{
  // check view frame where the preview was displayed
  bool vfOK = checkViewWindow() && myViewWindow;
  // Iterate through presentations and delete them
  for ( PrsList::iterator anIter = myPreview.begin(); anIter != myPreview.end(); ++anIter )
  {
    if ( vfOK )
    {
      SUIT_ViewManager* aViewManager = myViewWindow->getViewManager();
      if ( aViewManager->getType() == OCCViewer_Viewer::Type() ||
           aViewManager->getType() == SVTK_Viewer::Type() )
      {
        SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
        SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
        if (aView)
          aView->Erase( getDisplayer(), *anIter, true );
      }
    }
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
  if (!getStudy()) return;
  _PTR(Study) aStudy = getStudy()->studyDS();
  _PTR(SComponent) aGeom ( aStudy->FindComponent( "GEOM" ) );
  if ( !aGeom )
    return;

  SALOME_ListIO aList;
  _PTR(ChildIterator) anIter ( aStudy->NewChildIterator( aGeom ) );
  for ( ; anIter->More(); anIter->Next() )
  {
    _PTR(SObject) aSO ( anIter->Value() );
    if ( aSO )
    {
      _PTR(SObject) aRefSO;
      if ( !aSO->ReferencedObject( aRefSO ) )
      {
        GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_narrow
          (GeometryGUI::ClientSObjectToObject(aSO));
        if ( !anObj->_is_nil() && anObj->GetType() == theType )
          aList.Append( new SALOME_InteractiveObject( aSO->GetID().c_str(), "GEOM", aSO->GetName().c_str()) );
      }
    }
  }

  getDisplayer()->LocalSelection( aList, 0 );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            modes are from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( const ObjectList& theObjs, const std::list<int> modes )
{
  SALOME_ListIO aListOfIO;

  ObjectList::const_iterator anIter = theObjs.begin();
  for ( ; anIter != theObjs.end(); ++anIter )
  {
    GEOM::GEOM_Object_ptr anObj = *anIter;
    if ( anObj->_is_nil() )
      continue;
    QString anEntry = getEntry( anObj );
    if ( anEntry != "" )
      aListOfIO.Append( new SALOME_InteractiveObject(
        anEntry.toLatin1().constData(), "GEOM", strdup( GEOMBase::GetName( anObj ).toLatin1().constData() ) ) );
  }

  getDisplayer()->LocalSelection( aListOfIO, modes );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            theMode is from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( const ObjectList& theObjs, const int theMode )
{
  std::list<int> modes;
  modes.push_back( theMode );
  localSelection( theObjs, modes );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            modes are from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( GEOM::GEOM_Object_ptr obj, const std::list<int> modes )
{
  // If object is null local selection for all objects is activated
  if ( obj->_is_nil() ) {
    getDisplayer()->LocalSelection( Handle(SALOME_InteractiveObject)(), modes );
    return;
  }

  ObjectList objList;
  objList.push_back( obj );
  localSelection( objList, modes );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            mode is from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( GEOM::GEOM_Object_ptr obj, const int mode )
{
  std::list<int> modes;
  modes.push_back( mode );
  localSelection( obj, modes );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            modes are from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( const std::list<int> modes )
{
  localSelection( GEOM::GEOM_Object::_nil(), modes );
}

//================================================================
// Function : localSelection
// Purpose  : Activate selection of sub-shapes in accordance with mode
//            mode is from TopAbs_ShapeEnum
//================================================================
void GEOMBase_Helper::localSelection( const int mode )
{
  std::list<int> modes;
  modes.push_back( mode );
  localSelection( modes );
}

//================================================================
// Function : globalSelection
// Purpose  : Activate selection of sub-shapes. Set selection filters
//            in accordance with mode. theMode is from GEOMImpl_Types
//================================================================
void GEOMBase_Helper::globalSelection( const int theMode, const bool update )
{
  getDisplayer()->GlobalSelection( theMode, update );
}

//================================================================
// Function : globalSelection
// Purpose  : Activate selection of sub-shapes. Set selection filters
//            in accordance with mode. theMode is from GEOMImpl_Types
//================================================================
void GEOMBase_Helper::globalSelection( const TColStd_MapOfInteger& theModes,
                                       const bool update )
{
  getDisplayer()->GlobalSelection( theModes, update );
}

//================================================================
// Function : globalSelection
// Purpose  : Activate selection of sub-shapes. Set selection filters
//            in accordance with mode. theMode is from GEOMImpl_Types
//================================================================
void GEOMBase_Helper::globalSelection( const TColStd_MapOfInteger& theModes,
                                       const QList<int>& subShapes,
                                       const bool update )
{
  getDisplayer()->GlobalSelection( theModes, update, &subShapes );
}

//================================================================
// Function : addInStudy
// Purpose  : Add object in study
//================================================================
QString GEOMBase_Helper::addInStudy( GEOM::GEOM_Object_ptr theObj, const char* theName )
{
  if ( !hasCommand() )
    return QString();

  _PTR(Study) aStudy = getStudy()->studyDS();
  if ( !aStudy || theObj->_is_nil() )
    return QString();

  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy(aStudy);

  GEOM::GEOM_Object_ptr aFatherObj = getFather( theObj );

  SALOMEDS::SObject_var aSO =
    getGeomEngine()->AddInStudy(aStudyDS, theObj, theName, aFatherObj);

  QString anEntry;
  if ( !aSO->_is_nil() ) {
    CORBA::String_var entry = aSO->GetID();
    anEntry = entry.in();
  }
  // Each dialog is responsible for this method implementation,
  // default implementation does nothing
  restoreSubShapes(aStudyDS, aSO);
  aSO->UnRegister();

  return anEntry;
}

//================================================================
// Function : restoreSubShapes
// Purpose  : restore tree of argument's sub-shapes under the resulting shape
//================================================================
void GEOMBase_Helper::restoreSubShapes (SALOMEDS::Study_ptr   /*theStudy*/,
                                        SALOMEDS::SObject_ptr /*theSObject*/)
{
  // do nothing by default

  // example of implementation in particular dialog:
  // GEOM::ListOfGO anArgs;
  // anArgs.length(0); // empty list means that all arguments should be restored
  // getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, anArgs,
  //                                     /*theFindMethod=*/GEOM::FSM_GetInPlace,
  //                                     /*theInheritFirstArg=*/false);
}

//================================================================
// Function : updateObjBrowser
// Purpose  : Update object browser
//================================================================
void GEOMBase_Helper::updateObjBrowser() const
{
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
  if (app) {
    CAM_Module* module = app->module( "Geometry" );
    SalomeApp_Module* appMod = dynamic_cast<SalomeApp_Module*>( module );
    if ( appMod ) {
      appMod->updateObjBrowser( true );
    }
  }
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
    anId = getStudy()->id();
  return anId;
}

//================================================================
// Function : getStudy
// Purpose  : Returns the active study. It is recommended to use
//            this method instead of direct desktop->getActiveStudy() calls
//================================================================
SalomeApp_Study* GEOMBase_Helper::getStudy() const
{
  SUIT_Desktop* aDesktop = getDesktop();
  if (!aDesktop)
    return 0;

  QList<SUIT_Application*> anAppList = SUIT_Session::session()->applications();

  SUIT_Application* anApp = 0;
  QListIterator<SUIT_Application*> it( anAppList );
  while ( it.hasNext() )
    {
      anApp = it.next();
      if ( anApp && anApp->desktop() == aDesktop )
        break;
    }

  return dynamic_cast<SalomeApp_Study*>(anApp->activeStudy());
}

//================================================================
// Function : getEntry
// Purpose  :
//================================================================
QString GEOMBase_Helper::getEntry( GEOM::GEOM_Object_ptr object ) const
{
  SalomeApp_Study* study = getStudy();
  if ( study )  {
    QString objIOR = GEOMBase::GetIORFromObject( object );
    if ( objIOR != "" ) {
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( objIOR.toLatin1().constData() ) );
      if ( SO )
        return QString::fromStdString(SO->GetID());
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
    myDisplayer = new GEOM_Displayer( getStudy() );
  return myDisplayer;
}

//================================================================
// Function : clearShapeBuffer
// Purpose  :
//================================================================
void GEOMBase_Helper::clearShapeBuffer( GEOM::GEOM_Object_ptr theObj )
{
  GeometryGUI::ClearShapeBuffer(theObj);
}

//================================================================
// Function : openCommand
// Purpose  :
//================================================================
bool GEOMBase_Helper::openCommand()
{
  bool res = false;
  if ( !getStudy() || hasCommand() )
  {
    MESSAGE("Getting out from openCommand()")
    return res;
  }

  GEOM::GEOM_IOperations_var anOp = GEOM::GEOM_IOperations::_narrow( getOperation() );
  if ( !anOp->_is_nil() ) {
    myCommand = new GEOM_Operation( SUIT_Session::session()->activeApplication(), anOp.in() );
    myCommand->start();
    res = true;
  }
  else
  {
    MESSAGE("anOp->_is_nil() = true")
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
  delete myCommand; // I don't know where to delete this object here ?
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

  myCommand->commit();
  delete myCommand; // I don't know where to delete this object here ?
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
// Function : checkViewWindow
// Purpose  :
//================================================================
bool GEOMBase_Helper::checkViewWindow()
{
  if ( myViewWindow ){
    QList<SUIT_ViewWindow*> aViewWindowsList = SUIT_Session::session()->activeApplication()->desktop()->windows();
    QListIterator<SUIT_ViewWindow*> it( aViewWindowsList );
    while ( it.hasNext() )
      {
        if ( myViewWindow == it.next() )
          return true;
      }
  }
  myViewWindow = 0;
  return false;
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it
//            performs a proper operation and manages transactions, etc.
//================================================================
bool GEOMBase_Helper::onAccept( const bool publish, const bool useTransaction, bool erasePreviewFlag )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( !appStudy ) 
  {
    MESSAGE("appStudy is empty")
    return false;
  }
  _PTR(Study) aStudy = appStudy->studyDS();

  bool aLocked = (_PTR(AttributeStudyProperties) (aStudy->GetProperties()))->IsLocked();
  if ( aLocked ) {
    MESSAGE("GEOMBase_Helper::onAccept - ActiveStudy is locked");
    SUIT_MessageBox::warning ( (QWidget*)SUIT_Session::session()->activeApplication()->desktop(),
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
  
  if(erasePreviewFlag)
    erasePreview( false );

  bool result = false;

  try {
    if ( ( !publish && !useTransaction ) || openCommand() ) {
      SUIT_OverrideCursor wc;
      SUIT_Session::session()->activeApplication()->putInfo( "" );
      ObjectList objects;
      if ( !execute( objects ) || !getOperation()->IsDone() ) {
        wc.suspend();
        abortCommand();
        showError();
      }
      else {
        QList<GEOM::GeomObjPtr> anObjectList = getSourceObjects( );
        addSubshapesToStudy(); // add Sub-shapes if local selection
        const int nbObjs = objects.size();
        QStringList anEntryList;
        int currObj = 1, aNumber = 1;
        for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it, currObj++ ) {
          GEOM::GEOM_Object_var obj=*it;
          if ( publish ) {
            QString aName = getObjectName(obj);
            if (aName.isEmpty()) {
              aName = getNewObjectName(currObj);
                    if ( nbObjs > 1 ) {
                            if (aName.isEmpty())
                              aName = getPrefix(obj);
                              if (nbObjs <= 30) {
                                // Try to find a unique name
                                aName = GEOMBase::GetDefaultName(aName, extractPrefix());
                              } else {
                                // Don't check name uniqueness in case of numerous objects
                                aName = aName + "_" + QString::number(aNumber++);
                              }
                    } else {
                            // PAL6521: use a prefix, if some dialog box doesn't reimplement getNewObjectName()
                            if ( aName.isEmpty() )
                              aName = GEOMBase::GetDefaultName( getPrefix( obj ) );
                    }
            }
            anEntryList << addInStudy( obj, aName.toLatin1().constData() );
            // updateView=false
            if( isDisplayResult() )
              display( obj, false );
#ifdef WITHGENERICOBJ
            // obj has been published in study. Its refcount has been incremented.
            // It is safe to decrement its refcount
            // so that it will be destroyed when the entry in study will be removed
            obj->UnRegister();
#endif
          }
          else {
            // asv : fix of PAL6454. If publish==false, then the original shape
            // was modified, and need to be re-cached in GEOM_Client before redisplay
            clearShapeBuffer( obj );
            // withChildren=true, updateView=false
            if( isDisplayResult() )
              redisplay( obj, true, false );
          }
        }

        if ( nbObjs ) {
          commitCommand();
          updateObjBrowser();
          if( SUIT_Application* anApp = SUIT_Session::session()->activeApplication() ) {
            LightApp_Application* aLightApp = dynamic_cast<LightApp_Application*>( anApp );
            if(aLightApp) {
              QString anOpName( typeid(*this).name() );
              aLightApp->emitOperationFinished( "Geometry", anOpName, anEntryList );

              if ( !isDisableBrowsing() )
                aLightApp->browseObjects( anEntryList, isApplyAndClose(), isOptimizedBrowsing() );
            }
            anApp->putInfo( QObject::tr("GEOM_PRP_DONE") );
          }
          if ( anObjectList.count() > 0 )
            hideSourceObjects( anObjectList );
          result = true;
        }
        else
          abortCommand();
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
    abortCommand();
    MESSAGE("Exception catched")
  }

  updateViewer();

  MESSAGE("result ="<<result)
  return result;
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

  SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
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
  SUIT_MessageBox::critical(SUIT_Session::session()->activeApplication()->desktop(), QObject::tr( "GEOM_ERROR" ), str, QObject::tr( "BUT_OK" ) );
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
// Function : getObjectName
// Purpose  : Redefine this method to return proper name for the given object
//================================================================
QString GEOMBase_Helper::getObjectName(GEOM::GEOM_Object_ptr object) const
{
  return QString();
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
QString GEOMBase_Helper::getNewObjectName (int) const
{
  return QString();
}

//================================================================
// Function : extractPrefix
// Purpose  : Redefine this method to return \c true if necessary
//            to extract prefix when generating new name for the
//            object(s) being created
//================================================================
bool GEOMBase_Helper::extractPrefix() const
{
  return false;
}

//================================================================
// Function : getPrefix
// Purpose  : Get prefix for name of created object
//================================================================
QString GEOMBase_Helper::getPrefix( GEOM::GEOM_Object_ptr theObj ) const
{
  if ( !myPrefix.isEmpty() || theObj->_is_nil() )
    return myPrefix;

  GEOM::shape_type aType = theObj->GetShapeType();

  switch ( aType )
  {
    case GEOM::VERTEX   : return QObject::tr( "GEOM_VERTEX" );
    case GEOM::EDGE     : return QObject::tr( "GEOM_EDGE" );
    case GEOM::WIRE     : return QObject::tr( "GEOM_WIRE" );
    case GEOM::FACE     : return QObject::tr( "GEOM_FACE" );
    case GEOM::SHELL    : return QObject::tr( "GEOM_SHELL" );
    case GEOM::SOLID    : return QObject::tr( "GEOM_SOLID" );
    case GEOM::COMPSOLID: return QObject::tr( "GEOM_COMPOUNDSOLID" );
    case GEOM::COMPOUND : return QObject::tr( "GEOM_COMPOUND" );
    default : return "";
  }
}

//================================================================
// Function : getDesktop
// Purpose  : Returns myDesktop field. Initialized in constructor,
//            usually as dynamic_cast<SUIT_Desktop*>(parentWidget())
//================================================================
SUIT_Desktop* GEOMBase_Helper::getDesktop() const
{
  return myDesktop;
}

//================================================================
// Function : selectObjects
// Purpose  : Selects list of objects
//================================================================
bool GEOMBase_Helper::selectObjects( ObjectList& objects )
{
  SUIT_DataOwnerPtrList aList;
  ObjectList::iterator anIter;
  for ( anIter = objects.begin(); anIter != objects.end(); ++anIter )
  {
    QString anEntry = getEntry( *anIter );
    LightApp_DataOwner* anOwher = new LightApp_DataOwner( anEntry );
    aList.append( anOwher );
  }

  SUIT_Session* session = SUIT_Session::session();
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( !app )
    return false;

  LightApp_SelectionMgr* aMgr = app->selectionMgr();
  if ( !aMgr )
    return false;

  aMgr->setSelected( aList, false );

  return true;
}

//================================================================
// Function : findObjectInFather
// Purpose  : It should return an object if its founded in study or
//            return Null object if the object is not founded
//================================================================
GEOM::GEOM_Object_ptr GEOMBase_Helper::findObjectInFather (GEOM::GEOM_Object_ptr theFather,
                                                           const QString& theName)
{
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  _PTR(Study) aDStudy = appStudy->studyDS();
  QString IOR = GEOMBase::GetIORFromObject( theFather );
  _PTR(SObject) SObj ( aDStudy->FindObjectIOR( IOR.toLatin1().constData() ) );

  bool inStudy = false;
  GEOM::GEOM_Object_var aReturnObject;
  for (_PTR(ChildIterator) iit (aDStudy->NewChildIterator( SObj )); iit->More() && !inStudy; iit->Next()) {
    _PTR(SObject) child (iit->Value());
    QString aChildName = child->GetName().c_str();
    if (aChildName == theName) {
      inStudy = true;
      CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(iit->Value());
      aReturnObject = GEOM::GEOM_Object::_narrow( corbaObj );
    }
  }
  if (inStudy)
    return aReturnObject._retn();
  
  return GEOM::GEOM_Object::_nil();
}

//================================================================
// Function : findObjectInFather
// Purpose  : It should return an object if its founded in study or
//            return Null object if the object is not founded
//================================================================
GEOM::GEOM_Object_ptr GEOMBase_Helper::findObjectInFather( GEOM::GEOM_Object_ptr theFather,
                                                           int theIndex )
{
  GEOM::GEOM_Object_var object;
  
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( study ) {
    _PTR(Study) studyDS = study->studyDS();
    QString IOR = GEOMBase::GetIORFromObject( theFather );
    _PTR(SObject) sobject( studyDS->FindObjectIOR( IOR.toLatin1().constData() ) );
    if ( sobject ) {
      _PTR(ChildIterator) it( studyDS->NewChildIterator( sobject ) );
      for ( ; it->More(); it->Next() ) {
        GEOM::GEOM_Object_var cobject = GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject( it->Value() ) );
        if ( !CORBA::is_nil( cobject ) ) {
          GEOM::ListOfLong_var indices = cobject->GetSubShapeIndices();
          int length = indices->length();
          // VSR 18/03/2014: we need only sub-shapes with single sub-shape index (to exclude groups, etc)
          if ( length == 1 && indices[0] == theIndex ) {
            object = cobject;
            break;
          }
        }
      }
    }
  }
  
  return object._retn();
}

//================================================================
// Function : addSubshapesToStudy
// Purpose  : Virtual method to add sub-shapes if needs
//================================================================
void GEOMBase_Helper::addSubshapesToStudy()
{
  //Impemented in Dialogs, called from Accept method
}

//================================================================
// Function : getSourceObjects
// Purpose  : Virtual method to get source objects
//================================================================
QList<GEOM::GeomObjPtr> GEOMBase_Helper::getSourceObjects()
{
  //Impemented in Dialogs, called from Accept method
  QList<GEOM::GeomObjPtr> res;
  return res;
}

//================================================================
// Function : getSelected
// Purpose  : Get selected object by specified type
//
// Returns valid object if only one object of the specified type is selected
// (no matter global or local selection is activated). If \a type is TopAbs_SHAPE,
// geometrical object of any valid type is expected.
// 
// \param type type of the object to be obtained from selection
// \return selected geometrical object or nil object if selection is not satisfactory
//================================================================
GEOM::GeomObjPtr GEOMBase_Helper::getSelected( TopAbs_ShapeEnum type )
{
  QList<TopAbs_ShapeEnum> types;
  types << type;
  return getSelected( types );
}

//================================================================
// Function : getSelected
// Purpose  : Get selected object by specified types
//
// Returns valid object if only one object of the specified type is selected
// (no matter global or local selection is activated). The list of allowed
// shape types is passed via \a types. If \a types includes TopAbs_SHAPE,
// geometrical object of any valid type is expected.
// 
// \param types list of allowed shape types for the objects to be obtained from selection
// \return selected geometrical object or nil object if selection is not satisfactory
//================================================================
GEOM::GeomObjPtr GEOMBase_Helper::getSelected( const QList<TopAbs_ShapeEnum>& types )
{
  QList<GEOM::GeomObjPtr> selected = getSelected( types, 1 );
  return selected.count() > 0 ? selected[0] : GEOM::GeomObjPtr();
}

//================================================================
// Function : getSelected
// Purpose  : Get selected object(s) by specified type
//
// Returns list of selected objects if selection satisfies specifies selection options.
// (no matter global or local selection is activated). If \a type is TopAbs_SHAPE,
// geometrical objects of any valid type are expected.
//
// The \a type parameter specifies allowed type of the object(s) being selected.
// The \a count parameter specifies exact number of the objects to be retrieved from selection.
// The \a strict parameter specifies policy being applied to the selection. 
// If \a count < 0, then any number of the selected objects is valid (including 0).
// In this case, if \a strict is \c true (default), all selected objects should satisfy
// the specified \a type.
// If \a count > 0, only specified number of the objects is retrieved from the selection.
// In this case, if \a strict is \c true (default), function returns empty list if total number of selected
// objects does not correspond to the \a count parameter. Otherwise (if \a strict is \c false),
// function returns valid list of objects if at least \a count objects satisfy specified \a type.
// 
// \param type type of the object(s) to be obtained from selection
// \param count number of items to be retrieved from selection
// \param strict selection policy
// \return list of selected geometrical objects or empty list if selection is not satisfactory
//================================================================
QList<GEOM::GeomObjPtr> GEOMBase_Helper::getSelected( TopAbs_ShapeEnum type, int count, bool strict )
{
  QList<TopAbs_ShapeEnum> types;
  types << type;
  return getSelected( types, count, strict );
}

static bool typeInList( TopAbs_ShapeEnum type, const QList<TopAbs_ShapeEnum>& types )
{
  bool ok = false;
  for ( int i = 0; i < types.count() && !ok; i++ )
    ok = types[i] == TopAbs_SHAPE || types[i] == type;
  return ok;
}

//================================================================
// Function : getSelected
// Purpose  : Get selected objects by specified types
//
// Returns list of selected objects if selection satisfies specifies selection options.
// (no matter global or local selection is activated). If \a types includes TopAbs_SHAPE,
// geometrical objects of any valid type are expected.
//
// The \a types parameter specifies allowed types of the object(s) being selected.
// The \a count parameter specifies exact number of the objects to be retrieved from selection.
// The \a strict parameter specifies policy being applied to the selection. 
// If \a count < 0, then any number of the selected objects is valid (including 0).
// In this case, if \a strict is \c true (default), all selected objects should satisfy
// the specified \a type.
// If \a count > 0, only specified number of the objects is retrieved from the selection.
// In this case, if \a strict is \c true (default), function returns empty list if total number of selected
// objects does not correspond to the \a count parameter. Otherwise (if \a strict is \c false),
// function returns valid list of objects if at least \a count objects satisfy specified \a type.
// 
// \param types list of allowed shape types for the objects to be obtained from selection
// \param count number of items to be retrieved from selection
// \param strict selection policy
// \return list of selected geometrical objects or empty list if selection is not satisfactory
//================================================================
QList<GEOM::GeomObjPtr> GEOMBase_Helper::getSelected( const QList<TopAbs_ShapeEnum>& types, int count, bool strict )
{
  SUIT_Session* session = SUIT_Session::session();
  QList<GEOM::GeomObjPtr> result;

  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( session->activeApplication() );
  if ( app ) {
    LightApp_SelectionMgr* selMgr = app->selectionMgr();
    if ( selMgr ) {
      SALOME_ListIO selected;
      selMgr->selectedObjects( selected );
      SALOME_ListIteratorOfListIO it( selected );
      bool stopped = false;
      for ( ; it.More() && !stopped; it.Next() ) {
        Handle(SALOME_InteractiveObject) IO = it.Value();
        GEOM::GeomObjPtr object = GEOMBase::ConvertIOinGEOMObject( IO );
        if ( object ) {
          TColStd_IndexedMapOfInteger subShapes;
          selMgr->GetIndexes( IO, subShapes );
          int nbSubShapes = subShapes.Extent();
          if ( nbSubShapes == 0 ) {
            // global selection
            if ( typeInList( (TopAbs_ShapeEnum)(object->GetShapeType()), types ) ) {
              result << object;
              if ( count > 0 ) {
                if ( strict && result.count() > count ) {
                  result.clear();
                  stopped = true;
                }
                else if ( !strict && result.count() == count )
                  stopped = true;
              }
            }
            else if ( strict ) {
              result.clear();
              stopped = true;
            }
          }
          else {
            // local selection
            for ( int i = 1; i <= nbSubShapes && !stopped; i++ ) {
              int idx = subShapes( i );
              GEOM::GeomObjPtr subShape = findObjectInFather( object.get(), idx );
              if ( !subShape ) {
                // sub-shape is not yet published in the study
                GEOM::ShapesOpPtr shapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
                subShape.take( shapesOp->GetSubShape( object.get(), idx ) ); // take ownership!
              }
              if ( typeInList( (TopAbs_ShapeEnum)(subShape->GetShapeType()), types ) ) {
                result << subShape;
                if ( count > 0 ) {
                  if ( strict && result.count() > count ) {
                    result.clear();
                    stopped = true;
                  }
                  else if ( !strict && result.count() == count )
                    stopped = true;
                }
              }
              else if ( strict ) {
                result.clear();
                stopped = true;
              }
            }
          }
        }
      }
    }
  }
  return result;
}
//================================================================
// Function : hideSourceObject
// Purpose  :
//================================================================
void GEOMBase_Helper::hideSourceObjects( QList<GEOM::GeomObjPtr> theObject )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( resMgr->booleanValue( "Geometry", "hide_input_object", true) ) {
    GEOM_Displayer* aDisplayer = getDisplayer();
    for ( int i = 0; i < theObject.count(); i++ )
      aDisplayer->Erase( theObject[i].get() );
  }
}

//================================================================
// Function : setIsApplyAndClose
// Purpose  : Set value of the flag indicating that the dialog is
//            accepted by Apply & Close button
//================================================================
void GEOMBase_Helper::setIsApplyAndClose( const bool theFlag )
{
  myIsApplyAndClose = theFlag;
}

//================================================================
// Function : isApplyAndClose
// Purpose  : Get value of the flag indicating that the dialog is
//            accepted by Apply & Close button
//================================================================
bool GEOMBase_Helper::isApplyAndClose() const
{
  return myIsApplyAndClose;
}

//================================================================
// Function : setIsOptimizedBrowsing
// Purpose  : Set value of the flag switching to optimized
//            browsing mode (to select the first published
//            object only)
//================================================================
void GEOMBase_Helper::setIsOptimizedBrowsing( const bool theFlag )
{
  myIsOptimizedBrowsing = theFlag;
}

//================================================================
// Function : isOptimizedBrowsing
// Purpose  : Get value of the flag switching to optimized
//            browsing mode (to select the first published
//            object only)
//================================================================
bool GEOMBase_Helper::isOptimizedBrowsing() const
{
  return myIsOptimizedBrowsing;
}
