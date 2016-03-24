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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMBase_Tools.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

#include "GEOMToolsGUI.h"
#include "GEOMToolsGUI_DeleteDlg.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <GEOMBase.h>
#include <GEOM_Operation.h>
#include <GEOM_Displayer.h>

#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewManager.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <GEOMImpl_Types.hxx>

#include <SALOME_ListIO.hxx>
#include <SALOME_Prs.h>

// QT Includes
#include <QApplication>
#include <QMap>
#include <QRegExp>

// OCCT Includes
#include <TCollection_AsciiString.hxx>

typedef QMap<QString, QString> FilterMap;
static QString lastUsedFilter;

//=======================================================================
// function : getParentComponent
// purpose  : Get object's parent component entry
//=======================================================================
static QString getParentComponent( _PTR( SObject ) obj )
{
  if ( obj ) {
    _PTR(SComponent) comp = obj->GetFatherComponent();
    if ( comp )
      return QString( comp->GetID().c_str() );
  }
  return QString();
}

//=====================================================================================
// function : inUse
// purpose  : check if the object(s) passed as the the second arguments are used
//            by the other objects in the study
//=====================================================================================
static bool inUse( _PTR(Study) study, const QString& component, const QMap<QString,QString>& objects )
{
  _PTR(SObject) comp = study->FindObjectID( component.toLatin1().data() );
  if ( !comp )
    return false;

  // collect all GEOM objects being deleted
  QMap<QString, GEOM::GEOM_BaseObject_var> gobjects;
  QMap<QString, QString>::ConstIterator oit;
  std::list<_PTR(SObject)> aSelectedSO;
  for ( oit = objects.begin(); oit != objects.end(); ++oit ) {
    _PTR(SObject) so = study->FindObjectID( oit.key().toLatin1().data() );
    if ( !so )
      continue;
    aSelectedSO.push_back(so);
    CORBA::Object_var corbaObj_rem = GeometryGUI::ClientSObjectToObject( so );
    GEOM::GEOM_BaseObject_var geomObj_rem = GEOM::GEOM_BaseObject::_narrow( corbaObj_rem );
    if( CORBA::is_nil( geomObj_rem ) )
      continue;
    gobjects.insert( oit.key(), geomObj_rem );
  }

  // Search References with other Modules
  std::list< _PTR(SObject) >::iterator itSO = aSelectedSO.begin();
  for ( ; itSO != aSelectedSO.end(); ++itSO ) {
    std::vector<_PTR(SObject)> aReferences = study->FindDependances( *itSO  );
    int aRefLength = aReferences.size();
    if (aRefLength) {
      for (int i = 0; i < aRefLength; i++) {
        _PTR(SObject) firstSO( aReferences[i] );
        _PTR(SComponent) aComponent = firstSO->GetFatherComponent();
        QString type = aComponent->ComponentDataType().c_str();
        if ( type == "SMESH" )
          return true;
      }
    }
  }

  // browse through all GEOM data tree
  _PTR(ChildIterator) it ( study->NewChildIterator( comp ) );
  for ( it->InitEx( true ); it->More(); it->Next() ) {
    _PTR(SObject) child( it->Value() );
    CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject( child );
    GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
    if( CORBA::is_nil( geomObj ) )
      continue;

    GEOM::ListOfGBO_var list = geomObj->GetDependency();
    if( list->length() == 0 )
      continue;

    for( int i = 0; i < list->length(); i++ ) {
      bool depends = false;
      bool deleted = false;
      QMap<QString, GEOM::GEOM_BaseObject_var>::Iterator git;
      for ( git = gobjects.begin(); git != gobjects.end() && ( !depends || !deleted ); ++git ) {
        depends = depends || list[i]->_is_equivalent( *git );
        deleted = deleted || git.key() == child->GetID().c_str() ;//geomObj->_is_equivalent( *git );
      }
      if ( depends && !deleted )
        return true;
    }
  }
  return false;
}

//=======================================================================
// function : getGeomChildrenAndFolders
// purpose  : Get direct (1-level) GEOM objects under each folder, sub-folder, etc. and these folders itself
//=======================================================================
static void getGeomChildrenAndFolders( _PTR(SObject) theSO, 
                                       QMap<QString,QString>& geomObjList, 
                                       QMap<QString,QString>& folderList ) {
  if ( !theSO ) return;
  _PTR(Study) aStudy = theSO->GetStudy();
  if ( !aStudy ) return;
  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();

  bool isFolder = false;
  _PTR(GenericAttribute) anAttr;
  if ( theSO->FindAttribute(anAttr, "AttributeLocalID") ) {
    _PTR(AttributeLocalID) aLocalID( anAttr );
    isFolder = aLocalID->Value() == 999;
  }
  QString anEntry = theSO->GetID().c_str();
  QString aName = theSO->GetName().c_str();
  if ( isFolder ) {
    folderList.insert( anEntry, aName );
    _PTR(UseCaseIterator) ucit ( aUseCaseBuilder->GetUseCaseIterator( theSO ) );
    for ( ucit->Init( false ); ucit->More(); ucit->Next() ) {
      getGeomChildrenAndFolders( ucit->Value(), geomObjList, folderList );
    }
  } else {
    geomObjList.insert( anEntry, aName );
  }
}

//=======================================================================
// function : GEOMToolsGUI()
// purpose  : Constructor
//=======================================================================
GEOMToolsGUI::GEOMToolsGUI( GeometryGUI* parent )
: GEOMGUI( parent )
{
}

//=======================================================================
// function : ~GEOMToolsGUI()
// purpose  : Destructor
//=======================================================================
GEOMToolsGUI::~GEOMToolsGUI()
{
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool GEOMToolsGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  switch ( theCommandID ) {
  case GEOMOp::OpDelete:         // EDIT - DELETE
    OnEditDelete();
    break;
#ifndef DISABLE_PYCONSOLE
  case GEOMOp::OpCheckGeom:      // TOOLS - CHECK GEOMETRY
    OnCheckGeometry();
    break;
#endif
  case GEOMOp::OpSelectVertex:   // POPUP - SELECT ONLY - VERTEX
    OnSelectOnly( GEOM_POINT );
    break;
  case GEOMOp::OpSelectEdge:     // POPUP - SELECT ONLY - EDGE
    OnSelectOnly( GEOM_EDGE );
    break;
  case GEOMOp::OpSelectWire:     // POPUP - SELECT ONLY - WIRE
    OnSelectOnly( GEOM_WIRE );
    break;
  case GEOMOp::OpSelectFace:     // POPUP - SELECT ONLY - FACE
    OnSelectOnly( GEOM_FACE );
    break;
  case GEOMOp::OpSelectShell:    // POPUP - SELECT ONLY - SHELL
    OnSelectOnly( GEOM_SHELL );
    break;
  case GEOMOp::OpSelectSolid:    // POPUP - SELECT ONLY - SOLID
    OnSelectOnly( GEOM_SOLID );
    break;
  case GEOMOp::OpSelectCompound: // POPUP - SELECT ONLY - COMPOUND
    OnSelectOnly( GEOM_COMPOUND );
    break;
  case GEOMOp::OpSelectAll:      // POPUP - SELECT ONLY - SELECT ALL
    OnSelectOnly( GEOM_ALLOBJECTS );
    break;
  case GEOMOp::OpDeflection:     // POPUP - DEFLECTION ANGLE
    OnDeflection();
    break;
  case GEOMOp::OpColor:          // POPUP - COLOR
    OnColor();
    break;
  case GEOMOp::OpSetTexture:     // POPUP - TEXTURE
    OnTexture();
    break;
  case GEOMOp::OpTransparency:   // POPUP - TRANSPARENCY
    OnTransparency();
    break;
  case GEOMOp::OpIncrTransparency: // SHORTCUT   - INCREASE TRANSPARENCY
    OnChangeTransparency( true );
    break;
  case GEOMOp::OpDecrTransparency: // SHORTCUT   - DECREASE TRANSPARENCY
    OnChangeTransparency( false );
    break;
  case GEOMOp::OpIsos:           // POPUP - ISOS
    OnNbIsos();
    break;
  case GEOMOp::OpIncrNbIsos:     // SHORTCUT   - INCREASE NB ISOLINES
    OnNbIsos( INCR );
    break;
  case GEOMOp::OpDecrNbIsos:     // SHORTCUT   - DECREASE NB ISOLINES
    OnNbIsos( DECR );
    break;
  case GEOMOp::OpMaterialProperties: // POPUP - MATERIAL PROPERTIES
    OnMaterialProperties();
    break;
  case GEOMOp::OpPredefMaterCustom:  // POPUP  - MATERIAL PROPERTIES - CUSTOM...
    OnMaterialProperties();
    break;
  case GEOMOp::OpMaterialsLibrary:    // POPUP MENU - MATERIAL PROPERTIES
    OnMaterialsLibrary();
    break;
  case GEOMOp::OpAutoColor:      // POPUP - AUTO COLOR
    OnAutoColor();
    break;
  case GEOMOp::OpNoAutoColor:    // POPUP - DISABLE AUTO COLOR
    OnDisableAutoColor();
    break;
  case GEOMOp::OpDiscloseChildren:   // POPUP - SHOW CHILDREN
  case GEOMOp::OpConcealChildren:   // POPUP - HIDE CHILDREN
    OnDiscloseConcealChildren( theCommandID == GEOMOp::OpDiscloseChildren );
    break;
  case GEOMOp::OpPointMarker:    // POPUP - POINT MARKER
    OnPointMarker();
    break;
  case GEOMOp::OpUnpublishObject:// POPUP - UNPUBLISH
    OnUnpublishObject();
    break;
  case GEOMOp::OpPublishObject:// GEOM ROOT OBJECT - POPUP - PUBLISH
    OnPublishObject();
    break;
  case GEOMOp::OpEdgeWidth:
    OnEdgeWidth();
    break;
  case GEOMOp::OpIsosWidth:
    OnIsosWidth();
    break;
  case GEOMOp::OpBringToFront:
    OnBringToFront();
    break;
  case GEOMOp::OpClsBringToFront:
    OnClsBringToFront();
     break;
  case GEOMOp::OpCreateFolder:
    OnCreateFolder();
     break;
  case GEOMOp::OpSortChildren:
    OnSortChildren();
    break;
#ifndef DISABLE_GRAPHICSVIEW
  case GEOMOp::OpShowDependencyTree:
    OnShowDependencyTree();
     break;
#endif
  case GEOMOp::OpReduceStudy:
    OnReduceStudy();
    break;
  default:
    SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }
  return true;
}

//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool GEOMToolsGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent, const QVariant& theParam )
{
  getGeometryGUI()->EmitSignalDeactivateDialog();

  switch ( theCommandID ) {
  case GEOMOp::OpPredefMaterial:         // POPUP MENU - MATERIAL PROPERTIES - <SOME MATERIAL>
    OnSetMaterial( theParam );
    break;
  default:
    SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }
  return true;
}

//===============================================================================
// function : OnEditDelete()
// purpose  :
//===============================================================================
void GEOMToolsGUI::OnEditDelete()
{
  SALOME_ListIO selected;
  SalomeApp_Application* app =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !aSelMgr || !appStudy )
    return;

  // get selection
  aSelMgr->selectedObjects( selected, "ObjectBrowser", false );
  if ( selected.IsEmpty() )
    return;

  _PTR(Study) aStudy = appStudy->studyDS();
  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();

  // check if study is locked
  if ( _PTR(AttributeStudyProperties)( aStudy->GetProperties() )->IsLocked() ) {
    SUIT_MessageBox::warning( app->desktop(),
                              tr("WRN_WARNING"),
                              tr("WRN_STUDY_LOCKED") );
    return; // study is locked
  }

  // get GEOM component
  CORBA::String_var geomIOR = app->orb()->object_to_string( GeometryGUI::GetGeomGen() );
  QString geomComp = getParentComponent( aStudy->FindObjectIOR( geomIOR.in() ) );

  // check each selected object: if belongs to GEOM, if not reference...
  QMap<QString,QString> toBeDeleted;
  QMap<QString,QString> allDeleted;
  QMap<QString,QString> toBeDelFolders;
  bool isComponentSelected = false;

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if ( !anIObject->hasEntry() )
      continue; // invalid object
    // ...
    QString entry = anIObject->getEntry();
    _PTR(SObject) obj = aStudy->FindObjectID( entry.toLatin1().data() );
    // check parent component
    QString parentComp = getParentComponent( obj );
    if ( parentComp != geomComp )  {
      SUIT_MessageBox::warning( app->desktop(),
                                QObject::tr("ERR_ERROR"),
                                QObject::tr("NON_GEOM_OBJECTS_SELECTED").arg( getGeometryGUI()->moduleName() ) );
      return; // not GEOM object selected
    }

    ///////////////////////////////////////////////////////
    // if GEOM component is selected, so skip other checks
    if ( isComponentSelected ) continue;
    ///////////////////////////////////////////////////////

    // check if object is reference
    _PTR(SObject) refobj;
    if ( obj && obj->ReferencedObject( refobj ) ) {
      // get the main object by reference IPAL 21354
      obj = refobj;
      entry = obj->GetID().c_str();
    }
    // ...
    QString aName = obj->GetName().c_str();
    if ( entry == geomComp ) {
      // GEOM component is selected, skip other checks
      isComponentSelected = true;
      continue;
    }
    // all sub-objects of folder have to be deleted
    getGeomChildrenAndFolders( obj, toBeDeleted, toBeDelFolders );
    allDeleted.insert( entry, aName ); // skip GEOM component
    // browse through all children recursively
    _PTR(UseCaseIterator) it ( aUseCaseBuilder->GetUseCaseIterator( obj ) );
    for ( it->Init( true ); it->More(); it->Next() ) {
      _PTR(SObject) child( it->Value() );
      if ( child && child->ReferencedObject( refobj ) )
        continue; // skip references
      aName = child->GetName().c_str();
      if ( !aName.isEmpty() )
        allDeleted.insert( child->GetID().c_str(), aName );
    }
  }

  // is there is anything to delete?
  if ( !isComponentSelected && allDeleted.count() <= 0 )
    return; // nothing to delete

  // show confirmation dialog box
  GEOMToolsGUI_DeleteDlg dlg( app->desktop(), allDeleted, isComponentSelected );
  if ( !dlg.exec() )
    return; // operation is cancelled by user

  // get currently opened views
  QList<SALOME_View*> views;
  SALOME_View* view;
  ViewManagerList vmans = app->viewManagers();
  SUIT_ViewManager* vman;
  foreach ( vman, vmans ) {
    SUIT_ViewModel* vmod = vman->getViewModel();
    view = dynamic_cast<SALOME_View*> ( vmod ); // must work for OCC and VTK views
    if ( view )
      views.append( view );
  }

  _PTR(StudyBuilder) aStudyBuilder (aStudy->NewBuilder());
  GEOM_Displayer* disp = new GEOM_Displayer( appStudy );

  if ( isComponentSelected ) {
    // GEOM component is selected: delete all objects recursively
    _PTR(SObject) comp = aStudy->FindObjectID( geomComp.toLatin1().data() );
    if ( !comp )
      return;
    _PTR(ChildIterator) it ( aStudy->NewChildIterator( comp ) );
    // remove top-level objects only
    for ( it->InitEx( false ); it->More(); it->Next() ) {
      _PTR(SObject) child( it->Value() );
      // remove object from GEOM engine
      removeObjectWithChildren( child, aStudy, views, disp );
      // remove object from study
      aStudyBuilder->RemoveObjectWithChildren( child );
      // remove object from use case tree
      aUseCaseBuilder->Remove( child );
    }
  }
  else {
    // GEOM component is not selected: check if selected objects are in use
    if ( inUse( aStudy, geomComp, allDeleted ) && 
         SUIT_MessageBox::question( app->desktop(),
                                    QObject::tr("WRN_WARNING"),
                                    QObject::tr("DEP_OBJECT"),
                                    SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                    SUIT_MessageBox::No ) != SUIT_MessageBox::Yes ) {
      return; // object(s) in use
    }
    // ... and then delete all objects
    QMap<QString, QString>::Iterator it;
    for ( it = toBeDeleted.begin(); it != toBeDeleted.end(); ++it ) {
      _PTR(SObject) obj ( aStudy->FindObjectID( it.key().toLatin1().data() ) );
      // remove object from GEOM engine
      removeObjectWithChildren( obj, aStudy, views, disp );
      // remove objects from study
      aStudyBuilder->RemoveObjectWithChildren( obj );
      // remove object from use case tree
      aUseCaseBuilder->Remove( obj );
    }
    // ... and then delete all folders
    for ( it = toBeDelFolders.begin(); it != toBeDelFolders.end(); ++it ) {
      _PTR(SObject) obj ( aStudy->FindObjectID( it.key().toLatin1().data() ) );
      // remove object from GEOM engine
      removeObjectWithChildren( obj, aStudy, views, disp );
      // remove objects from study
      aStudyBuilder->RemoveObjectWithChildren( obj );
      // remove object from use case tree
      aUseCaseBuilder->Remove( obj );
    }
  }

  selected.Clear();
  aSelMgr->setSelectedObjects( selected );
  getGeometryGUI()->updateObjBrowser();
  app->updateActions(); //SRN: To update a Save button in the toolbar
}

//=====================================================================================
// function : RemoveObjectWithChildren
// purpose  : used by OnEditDelete() method
//=====================================================================================
void GEOMToolsGUI::removeObjectWithChildren(_PTR(SObject) obj,
                                            _PTR(Study) aStudy,
                                            QList<SALOME_View*> views,
                                            GEOM_Displayer* disp)
{
  // iterate through all children of obj
  for (_PTR(ChildIterator) it (aStudy->NewChildIterator(obj)); it->More(); it->Next()) {
  // for (_PTR(UseCaseIterator) it (aStudy->GetUseCaseBuilder()->GetUseCaseIterator(obj)); it->More(); it->Next()) {
    _PTR(SObject) child (it->Value());
    removeObjectWithChildren(child, aStudy, views, disp);
  }

  // erase object and remove it from engine
  _PTR(GenericAttribute) anAttr;
  if (obj->FindAttribute(anAttr, "AttributeIOR")) {
    _PTR(AttributeIOR) anIOR (anAttr);

    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );

    // Delete shape in Client
    const TCollection_AsciiString ASCIor ((char*)anIOR->Value().c_str());
    getGeometryGUI()->GetShapeReader().RemoveShapeFromBuffer(ASCIor);

    CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(obj);
    GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
    if (!CORBA::is_nil(geomObj)) {

      //Remove visual properties of the object
      appStudy->removeObjectProperties(obj->GetID().c_str());

      // Erase graphical object
      QListIterator<SALOME_View*> it( views );
      while ( it.hasNext() )
        if ( SALOME_View* view = it.next() )
          disp->Erase(geomObj, true, true, view);

      // Remove object from Engine
      // We can't directly remove object from engine. All we can do is to unpublish the object
      // from the study. Another client could be using the object.
      // Unpublishing is done just after in aStudyBuilder->RemoveObjectWithChildren( child );
      //GeometryGUI::GetGeomGen()->RemoveObject( geomObj );
    }
  }
}

//=================================================================================
// function : deactivate()
// purpose  : Called when GEOM component is deactivated
//=================================================================================
void GEOMToolsGUI::deactivate()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    GEOM_Displayer aDisp (appStudy);
    aDisp.GlobalSelection();
    getGeometryGUI()->setLocalSelectionMode(GEOM_ALLOBJECTS);
  }
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new GEOMToolsGUI( parent );
  }
}
