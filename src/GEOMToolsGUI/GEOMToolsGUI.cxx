//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMBase_Tools.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "GEOMToolsGUI.h"
#include "GEOMToolsGUI_DeleteDlg.h"

#include <GeometryGUI.h>
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
#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_Prs.h>

// QT Includes
#include <QApplication>
#include <QMap>
#include <QRegExp>

// OCCT Includes
#include <TCollection_AsciiString.hxx>

using namespace std;

typedef QMap<QString, QString> FilterMap;
static QString lastUsedFilter;

//=======================================================================
// function : getFileName
// purpose  : Selection of a file name for Import/Export. Returns also
//            the selected file type code through <filter> argument.
//=======================================================================
static QString getFileName( QWidget*           parent,
			    const QString&     initial,
			    const FilterMap&   filterMap,
                            const QStringList& filters,
			    const QString&     caption,
			    bool               open,
			    QString&           format,
			    bool               showCurrentDirInitially = false )
{
  //QStringList filters;
  QString aBrepFilter;
  for ( FilterMap::const_iterator it = filterMap.begin(); it != filterMap.end(); ++it ) {
    //filters.push_back( it.key() );
    if ( it.key().contains( "BREP", Qt::CaseInsensitive ) )
      aBrepFilter = it.key();
  }

  SUIT_FileDlg* fd = new SUIT_FileDlg( parent, open, true, true );
  if ( !caption.isEmpty() )
    fd->setWindowTitle( caption );

  if ( !initial.isEmpty() )
    fd->selectFile( initial );
  
  if ( showCurrentDirInitially && SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    fd->setDirectory( QDir::currentPath() );

  fd->setFilters( filters );
  
  if ( !lastUsedFilter.isEmpty() && filterMap.contains( lastUsedFilter ) ) {
    fd->selectFilter( lastUsedFilter );
  }
  else if ( !aBrepFilter.isEmpty() ) {
    fd->selectFilter( aBrepFilter );
  }

  QString filename;
  if ( fd->exec() == QDialog::Accepted ) {
    filename = fd->selectedFile();
    format = filterMap[fd->selectedFilter()];
    lastUsedFilter = fd->selectedFilter();
  }

  delete fd;
  qApp->processEvents();
  return filename;
}

//=======================================================================
// function : getFileNames
// purpose  : Select list of files for Import operation. Returns also
//            the selected file type code through <format> argument.
//=======================================================================
static QStringList getFileNames( QWidget*           parent,
				 const QString&     initial,
				 const FilterMap&   filterMap,
				 const QString&     caption,
				 QString&           format,
				 bool               showCurrentDirInitially = false)
{
  QString aBrepFilter;
  QStringList allFilters;
  QStringList filters;
  QRegExp re( "\\((.*)\\)" );
  re.setMinimal( true );
  for ( FilterMap::const_iterator it = filterMap.begin(); it != filterMap.end(); ++it ) {
    if ( it.value().contains( "BREP", Qt::CaseInsensitive ) && aBrepFilter.isEmpty() )
      aBrepFilter = it.key();
    filters.append( it.key() );
    int pos = 0;
    while ( re.indexIn( it.key(), pos ) >= 0 ) {
      QString f = re.cap(1);
      pos = re.pos() + f.length() + 2;
      allFilters.append( f.simplified() );
    }
  }
  filters.append( QObject::tr( "GEOM_ALL_IMPORT_FILES" ).arg( allFilters.join( " " ) ) );
  
  SUIT_FileDlg fd( parent, true, true, true );
  fd.setFileMode( SUIT_FileDlg::ExistingFiles );     
  if ( !caption.isEmpty() )
    fd.setWindowTitle( caption );
  if ( !initial.isEmpty() )
    fd.selectFile( initial );
  
  if ( showCurrentDirInitially && SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    fd.setDirectory( QDir::currentPath() );
  
  fd.setFilters( filters );
  
  if ( !lastUsedFilter.isEmpty() && filterMap.contains( lastUsedFilter ) )
    fd.selectFilter( lastUsedFilter );
  else if ( !aBrepFilter.isEmpty() )
    fd.selectFilter( aBrepFilter );

  QStringList filenames;
  if ( fd.exec() ) {
    filenames = fd.selectedFiles();
    format = filterMap.contains( fd.selectedFilter() ) ? filterMap[ fd.selectedFilter() ] : QString();
    lastUsedFilter = fd.selectedFilter();
  }
  qApp->processEvents();
  return filenames;
}

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
  QMap<QString, GEOM::GEOM_Object_var> gobjects;
  QMap<QString, QString>::ConstIterator oit;
  list<_PTR(SObject)> aSelectedSO;
  for ( oit = objects.begin(); oit != objects.end(); ++oit ) {
    _PTR(SObject) so = study->FindObjectID( oit.key().toLatin1().data() );
    if ( !so )
      continue;
    aSelectedSO.push_back(so);
    CORBA::Object_var corbaObj_rem = GeometryGUI::ClientSObjectToObject( so );
    GEOM::GEOM_Object_var geomObj_rem = GEOM::GEOM_Object::_narrow( corbaObj_rem );
    if( CORBA::is_nil( geomObj_rem ) ) 
      continue;
    gobjects.insert( oit.key(), geomObj_rem );
  }

  // Search References with other Modules
  list< _PTR(SObject) >::iterator itSO = aSelectedSO.begin();
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

    GEOM::ListOfGO_var list = geomObj->GetDependency();
    if( list->length() == 0 ) 
      continue;

    for( int i = 0; i < list->length(); i++ ) {
      bool depends = false;
      bool deleted = false;
      QMap<QString, GEOM::GEOM_Object_var>::Iterator git;
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

  switch (theCommandID)
    {
    case 31: // COPY
      {
	OnEditCopy();
	break;
      }
    case 33: // DELETE
      {
	OnEditDelete();
	break;
      }
    case 111: // IMPORT BREP
    case 112: // IMPORT IGES
    case 113: // IMPORT STEP
      {
	Import();
	break;
      }
    case 121: // EXPORT BREP
    case 122: // EXPORT IGES
    case 123: // EXPORT STEP
      {
	Export();
	break;
      }
    case 2171: // POPUP VIEWER - SELECT ONLY - VERTEX
      {
	OnSelectOnly( GEOM_POINT );
	break;
      }
    case 2172: // POPUP VIEWER - SELECT ONLY - EDGE
      {
	OnSelectOnly( GEOM_EDGE );
	break;
      }
    case 2173: // POPUP VIEWER - SELECT ONLY - WIRE
      {
	OnSelectOnly( GEOM_WIRE );
	break;
      }
    case 2174: // POPUP VIEWER - SELECT ONLY - FACE
      {
	OnSelectOnly( GEOM_FACE );
	break;
      }
    case 2175: // POPUP VIEWER - SELECT ONLY - SHELL
      {
	OnSelectOnly( GEOM_SHELL );
	break;
      }
    case 2176: // POPUP VIEWER - SELECT ONLY - SOLID
      {
	OnSelectOnly( GEOM_SOLID );
	break;
      }
    case 2177: // POPUP VIEWER - SELECT ONLY - COMPOUND
      {
	OnSelectOnly( GEOM_COMPOUND );
	break;
      }
    case 2178: // POPUP VIEWER - SELECT ONLY - SELECT ALL
      {
	OnSelectOnly( GEOM_ALLOBJECTS );
	break;
      }    
    case 411: // SETTINGS - ADD IN STUDY
      {
	// SAN -- TO BE REMOVED !!!
	break;
      }
    case 412: // SETTINGS - SHADING COLOR
      {
	OnSettingsColor();
	break;
      }
    case 804: // ADD IN STUDY - POPUP VIEWER
      {
	// SAN -- TO BE REMOVED !!!!
	break;
      }
    case 901: // RENAME
      {
	OnRename();
	break;
      }
    case 5103: // CHECK GEOMETRY
      {
	OnCheckGeometry();
	break;
      }
    case 8032: // COLOR - POPUP VIEWER
      {
	OnColor();
	break;
      }
    case 8033: // TRANSPARENCY - POPUP VIEWER
      {
	OnTransparency();
	break;
      }
    case 8034: // ISOS - POPUP VIEWER
      {
	OnNbIsos();
	break;
      }
    case 8035: // AUTO COLOR - POPUP VIEWER
      {
	OnAutoColor();
	break;
      }
    case 8036: // DISABLE AUTO COLOR - POPUP VIEWER
      {
	OnDisableAutoColor();
	break;
      }
    case 9024 : // OPEN - OBJBROSER POPUP
      {
	OnOpen();
	break;
      }
    default:
      {
	SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
	break;
      }
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
    if ( obj && obj->ReferencedObject( refobj ) )
      continue; // skip references
    // ...
    QString aName = obj->GetName().c_str();
    if ( entry == geomComp ) {
      // GEOM component is selected, skip other checks
      isComponentSelected = true;
      continue;
    }
    toBeDeleted.insert( entry, aName );
    allDeleted.insert( entry, aName ); // skip GEOM component
    // browse through all children recursively
    _PTR(ChildIterator) it ( aStudy->NewChildIterator( obj ) );
    for ( it->InitEx( true ); it->More(); it->Next() ) {
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
    }
  }
  else {
    // GEOM component is not selected: check if selected objects are in use
    if ( inUse( aStudy, geomComp, allDeleted ) ) {
      SUIT_MessageBox::warning( app->desktop(),
				QObject::tr("WRN_WARNING"),
				QObject::tr("DEP_OBJECT") );
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
    }
  }
  
  selected.Clear();
  aSelMgr->setSelectedObjects( selected );
  getGeometryGUI()->updateObjBrowser();
  app->updateActions(); //SRN: To update a Save button in the toolbar
}


//==============================================================================
// function : OnEditCopy()
// purpose  :
//==============================================================================
void GEOMToolsGUI::OnEditCopy()
{
/*
 SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection() );
  GEOM::string_array_var listIOR = new GEOM::string_array;

  const SALOME_ListIO& List = Sel->StoredIObjects();

  myGeomBase->ConvertListOfIOInListOfIOR(List, listIOR);

  Sel->ClearIObjects();

  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  int aStudyID = aStudy->StudyId();

  for (unsigned int ind = 0; ind < listIOR->length();ind++) {
    GEOM::GEOM_Object_var aShapeInit = myGeom->GetIORFromString(listIOR[ind]);
    try {
      GEOM::GEOM_IInsertOperations_var IOp =  myGeom->GetIInsertOperations(aStudyID);
      GEOM::GEOM_Object_var result = IOp->MakeCopy(aShapeInit);
      myGeomBase->Display(result);
    }
    catch  (const SALOME::SALOME_Exception& S_ex) {
      QtCatchCorbaException(S_ex);
    }
  }

  QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_READY"));
*/
}

//=====================================================================================
// function : Import
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Import()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( getGeometryGUI()->getApp() );
  if (! app) return false;

  SalomeApp_Study* stud = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !stud ) {
    MESSAGE ( "FAILED to cast active study to SalomeApp_Study" );
    return false;
  }
  _PTR(Study) aStudy = stud->studyDS();

  // check if study is locked
  bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->GetProperties()))->IsLocked();
  if ( aLocked ) {
    SUIT_MessageBox::warning( app->desktop(),
			      QObject::tr("WRN_WARNING"),
			      QObject::tr("WRN_STUDY_LOCKED") );
    return false;
  }

  // check if GEOM engine is available
  GEOM::GEOM_Gen_var eng = GeometryGUI::GetGeomGen();
  if ( CORBA::is_nil( eng ) ) {
    SUIT_MessageBox::critical( app->desktop(),
			       QObject::tr("WRN_WARNING"),
			       QObject::tr( "GEOM Engine is not started" ) );
    return false;
  }

  GEOM::GEOM_IInsertOperations_var aInsOp = eng->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;

  // obtain a list of available import formats
  FilterMap aMap;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ImportTranslators( aFormats, aPatterns );

  for ( int i = 0, n = aFormats->length(); i < n; i++ )
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );

  // select files to be imported
  QString fileType;
  QStringList fileNames = getFileNames( app->desktop(), "", aMap,
					tr( "GEOM_MEN_IMPORT" ), fileType, true );

  // set Wait cursor
  SUIT_OverrideCursor wc;

  if ( fileNames.count() == 0 )
    return false; // nothing selected, return

  QStringList errors;

  QList< GEOM::GEOM_Object_var > objsForDisplay;
  
  // iterate through all selected files
  for ( QStringList::ConstIterator it = fileNames.begin(); it != fileNames.end(); ++it ) {
    QString fileName = *it;

    if ( fileName.isEmpty() )
      continue;

    QString aCurrentType;
    if ( fileType.isEmpty() ) {
      // file type is not defined, try to detect
      QString ext = QFileInfo( fileName ).suffix().toUpper();
      QRegExp re( "\\*\\.(\\w+)" );
      for ( FilterMap::const_iterator it = aMap.begin(); 
	    it != aMap.end() && aCurrentType.isEmpty(); ++it ) {
	int pos = 0;
	while ( re.indexIn( it.key(), pos ) >= 0 ) {
	  QString f = re.cap(1).trimmed().toUpper();
	  if ( ext == f ) { aCurrentType = it.value(); break; }
	  pos = re.pos() + re.cap(1).length() + 2;
	}
      }
    }
    else {
      aCurrentType = fileType;
    }

    if ( aCurrentType.isEmpty() ) {
      errors.append( QString( "%1 : %2" ).arg( fileName ).arg( tr( "GEOM_UNSUPPORTED_TYPE" ) ) );
      continue;
    }

    GEOM_Operation* anOp = new GEOM_Operation( app, aInsOp.in() );
    try {
      app->putInfo( tr( "GEOM_PRP_LOADING" ).arg( SUIT_Tools::file( fileName, /*withExten=*/true ) ) );
      anOp->start();

      CORBA::String_var fileN = fileName.toLatin1().constData();
      CORBA::String_var fileT = aCurrentType.toLatin1().constData();
      GEOM::GEOM_Object_var anObj = aInsOp->Import( fileN, fileT );

      if ( !anObj->_is_nil() && aInsOp->IsDone() ) {
	QString aPublishObjName = 
	  GEOMBase::GetDefaultName( SUIT_Tools::file( fileName, /*withExten=*/true ) );
	
	SALOMEDS::Study_var aDSStudy = GeometryGUI::ClientStudyToStudy( aStudy );
	GeometryGUI::GetGeomGen()->PublishInStudy( aDSStudy,
						   SALOMEDS::SObject::_nil(),
						   anObj,
						   aPublishObjName.toLatin1().constData() );

	objsForDisplay.append( anObj );
	
	anOp->commit();
      }
      else {
	anOp->abort();
	errors.append( QString( "%1 : %2" ).arg( fileName ).arg( aInsOp->GetErrorCode() ) );
      }
    }
    catch( const SALOME::SALOME_Exception& S_ex ) {
      anOp->abort();
      errors.append( QString( "%1 : %2" ).arg( fileName ).arg( tr( "GEOM_UNKNOWN_IMPORT_ERROR" ) ) );
    }
  }

  // update object browser
  getGeometryGUI()->updateObjBrowser( true );

  // display imported model (if only one file is selected)
  if ( objsForDisplay.count() == 1 )
    GEOM_Displayer( stud ).Display( objsForDisplay[0].in() );

  if ( errors.count() > 0 ) {
    SUIT_MessageBox::critical( app->desktop(),
			       QObject::tr( "GEOM_ERROR" ),
			       QObject::tr( "GEOM_IMPORT_ERRORS" ) + "\n" + errors.join( "\n" ) );
  }

  app->updateActions(); //SRN: To update a Save button in the toolbar

  return objsForDisplay.count() > 0;
}


//=====================================================================================
// function : Export
// purpose  : BRep, Iges, Step
//=====================================================================================
bool GEOMToolsGUI::Export()
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  SalomeApp_Study* stud = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !stud ) {
    MESSAGE ( "FAILED to cast active study to SalomeApp_Study" );
    return false;
  }
  _PTR(Study) aStudy = stud->studyDS();

  GEOM::GEOM_Gen_var eng = GeometryGUI::GetGeomGen();
  if ( CORBA::is_nil( eng ) ) {
    SUIT_MessageBox::critical( app->desktop(),
			       QObject::tr("WRN_WARNING"),
			       QObject::tr( "GEOM Engine is not started" ) );
    return false;
  }

  GEOM::GEOM_IInsertOperations_var aInsOp = eng->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;

  // Obtain a list of available export formats
  FilterMap aMap;
  QStringList filters;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ExportTranslators( aFormats, aPatterns );
  for ( int i = 0, n = aFormats->length(); i < n; i++ ) {
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );
    filters.push_back( (char*)aPatterns[i] );
  }

  // Get selected objects
  LightApp_SelectionMgr* sm = app->selectionMgr();
  if ( !sm )
    return false;

  SALOME_ListIO selectedObjects;
  sm->selectedObjects( selectedObjects );
  bool appropriateObj = false;

  SALOME_ListIteratorOfListIO It( selectedObjects );
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    Standard_Boolean found;
    GEOM::GEOM_Object_var anObj = GEOMBase::ConvertIOinGEOMObject(IObject, found);

    if ( !found || anObj->_is_nil() )
      continue;

    QString fileType;
    QString file = getFileName(app->desktop(), QString( IObject->getName() ), aMap, filters,
			       tr("GEOM_MEN_EXPORT"), false, fileType, true);

    // User has pressed "Cancel" --> stop the operation
    if ( file.isEmpty() || fileType.isEmpty() )
      return false;

    GEOM_Operation* anOp = new GEOM_Operation( app, aInsOp.in() );
    try {
      SUIT_OverrideCursor wc;

      app->putInfo( tr("GEOM_PRP_EXPORT").arg(SUIT_Tools::file( file, /*withExten=*/true )) );

      anOp->start();


      aInsOp->Export( anObj, file.toStdString().c_str(), fileType.toLatin1().constData() );

      if ( aInsOp->IsDone() )
	anOp->commit();
      else
	{
	  anOp->abort();
	  wc.suspend();
	  SUIT_MessageBox::critical( app->desktop(),
				     QObject::tr( "GEOM_ERROR" ),
				     QObject::tr("GEOM_PRP_ABORT") + "\n" + QString( aInsOp->GetErrorCode() ) );
	  return false;
	}
    }
    catch (const SALOME::SALOME_Exception& S_ex) {
      //QtCatchCorbaException(S_ex);
      anOp->abort();
      return false;
    }
    appropriateObj = true;
  }

  if ( !appropriateObj )
    SUIT_MessageBox::warning( app->desktop(),
			      QObject::tr("WRN_WARNING"),
			      QObject::tr("GEOM_WRN_NO_APPROPRIATE_SELECTION") );
  return true;
}

//=====================================================================================
// function : RemoveObjectWithChildren
// purpose  : to be used by OnEditDelete() method
//=====================================================================================
void GEOMToolsGUI::removeObjectWithChildren(_PTR(SObject) obj,
                                            _PTR(Study) aStudy,
                                            QList<SALOME_View*> views,
                                            GEOM_Displayer* disp)
{
  // iterate through all children of obj
  for (_PTR(ChildIterator) it (aStudy->NewChildIterator(obj)); it->More(); it->Next()) {
    _PTR(SObject) child (it->Value());
    removeObjectWithChildren(child, aStudy, views, disp);
  }

  // erase object and remove it from engine
  _PTR(GenericAttribute) anAttr;
  if (obj->FindAttribute(anAttr, "AttributeIOR")) {
    _PTR(AttributeIOR) anIOR (anAttr);

    // Delete shape in Client
    const TCollection_AsciiString ASCIor ((char*)anIOR->Value().c_str());
    getGeometryGUI()->GetShapeReader().RemoveShapeFromBuffer(ASCIor);

    CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(obj);
    GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
    if (!CORBA::is_nil(geomObj)) {
      // Erase graphical object
      QListIterator<SALOME_View*> it( views );
      while ( it.hasNext() )
	if ( SALOME_View* view = it.next() )
	  disp->Erase(geomObj, true, view);
      
      // Remove object from Engine
      GeometryGUI::GetGeomGen()->RemoveObject( geomObj );
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
