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
//  File   : GEOMBase_Tools.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "GEOMToolsGUI.h"

#include "GeometryGUI.h"
#include "GEOM_Actor.h"
#include "GEOMBase.h"
#include "GEOM_Operation.h"
#include "GEOM_Displayer.h"

#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewModel.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>

#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_Prs.h>

#include "utilities.h"

// QT Includes
#include <qapplication.h>
#include <qmap.h>

// OCCT Includes
#include <TCollection_AsciiString.hxx>

using namespace std;

typedef QMap<QString, QString> FilterMap;

//=======================================================================
// function : getFileName
// purpose  : Selection of a file name for Import/Export. Returns also
//            the selected file type code through <filter> argument.
//=======================================================================
static QString getFileName( QWidget*           parent,
			    const QString&     initial,
			    const FilterMap&   filterMap,
			    const QString&     caption,
			    bool               open,
			    QString&           format )
{
  static QString lastUsedFilter;
  QStringList filters;
  for ( FilterMap::const_iterator it = filterMap.begin(); it != filterMap.end(); ++it )
    filters.push_back( it.key() );

  SUIT_FileDlg* fd = new SUIT_FileDlg( parent, open, true, true );
  if ( !caption.isEmpty() )
    fd->setCaption( caption );

  if ( !initial.isEmpty() )
    fd->setSelection( initial );

  if ( !lastUsedFilter.isEmpty() && filterMap.contains( lastUsedFilter ) )
    fd->setSelectedFilter( lastUsedFilter );

  fd->setFilters( filters );

  fd->exec();
  QString filename = fd->selectedFile();
  format = filterMap[fd->selectedFilter()];
  lastUsedFilter = fd->selectedFilter();
  delete fd;
  qApp->processEvents();
  return filename;
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
    case 413: // SETTINGS - ISOS
      {
	OnSettingsIsos();
	break;
      }
    case 414: // SETTINGS : STEP VALUE FOR SPIN BOXES
      {
	OnSettingsStep();
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
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( aSelMgr && appStudy ) {
      aSelMgr->selectedObjects( selected, QString::null, false );
      if ( !selected.IsEmpty() ) {
	_PTR(Study) aStudy = appStudy->studyDS();

	bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->GetProperties()))->IsLocked();
	if ( aLocked ) {
	  SUIT_MessageBox::warn1 ( app->desktop(),
				   QObject::tr("WRN_WARNING"),
				   QObject::tr("WRN_STUDY_LOCKED"),
				   QObject::tr("BUT_OK") );
	  return;
	}

	// VSR 17/11/04: check if all objects selected belong to GEOM component --> start
	// modifications of ASV 01.06.05
	QString parentComp = getParentComponent( aStudy, selected );
	const char* geomIOR = app->orb()->object_to_string( GeometryGUI::GetGeomGen() );
	QString geomComp = getParentComponent( aStudy->FindObjectIOR( geomIOR ) );

	if ( parentComp != geomComp )  {
	  SUIT_MessageBox::warn1 ( app->desktop(),
				  QObject::tr("ERR_ERROR"),
				  QObject::tr("NON_GEOM_OBJECTS_SELECTED").arg( getGeometryGUI()->moduleName() ),
				  QObject::tr("BUT_OK") );
	  return;
	}
	// VSR 17/11/04: check if all objects selected belong to GEOM component <-- finish

	if ( SUIT_MessageBox::warn2( app->desktop(),
				     QObject::tr( "GEOM_WRN_WARNING" ),
				     QObject::tr( "GEOM_REALLY_DELETE" ),
				     QObject::tr( "GEOM_BUT_YES" ),
				     QObject::tr( "GEOM_BUT_NO" ), 1, 0, 0 ) != 1 )
	  return;

	//	QAD_Operation* op = new SALOMEGUI_ImportOperation(.....);
	//	op->start();

	// prepare list of SALOME_Views
	QPtrList<SALOME_View> views;
	SALOME_View* view;
	// fill the list
	ViewManagerList vmans = app->viewManagers();
	SUIT_ViewManager* vman;
	for ( vman = vmans.first(); vman; vman = vmans.next() ) {
	  SUIT_ViewModel* vmod = vman->getViewModel();
	  view = dynamic_cast<SALOME_View*> ( vmod ); // must work for OCC and VTK views
	  if ( view )
	    views.append( view );
	}

	_PTR(StudyBuilder) aStudyBuilder (aStudy->NewBuilder());
	_PTR(GenericAttribute) anAttr;
	GEOM_Displayer* disp = new GEOM_Displayer( appStudy );

	// MAIN LOOP OF SELECTED OBJECTS
	for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {

	  Handle(SALOME_InteractiveObject) io = It.Value();
	  if ( !io->hasEntry() )
	    continue;

	  _PTR(SObject) obj ( aStudy->FindObjectID( io->getEntry() ) );

	  // disable removal of "Geometry" component object
	  if ( !strcmp( obj->GetIOR().c_str(), geomIOR ) )
	    continue;

	  // iterate through all children of obj, find IOR attributes on children
	  // and remove shapes that correspond to these IORs
	  for (_PTR(ChildIterator) it (aStudy->NewChildIterator(obj)); it->More(); it->Next()) {
	    _PTR(SObject) child (it->Value());
	    if (child->FindAttribute(anAttr, "AttributeIOR")) {
	      _PTR(AttributeIOR) anIOR (anAttr);

	      // Delete child( s ) shape in Client :
	      const TCollection_AsciiString ASCior ((char*)anIOR->Value().c_str());
	      getGeometryGUI()->GetShapeReader().RemoveShapeFromBuffer(ASCior);

              CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(child);
              GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow(corbaObj);
              if (!CORBA::is_nil(geomObj)) {
                for (view = views.first(); view; view = views.next()) {
		  disp->Erase(geomObj, true, view);
                }
              }
	    }
	  } // for ( children of obj )

	  // Erase main graphical object
	  for ( view = views.first(); view; view = views.next() )
	    disp->Erase( io, true, view );

	  // Delete main shape in Client :
	  if ( obj->FindAttribute( anAttr, "AttributeIOR" ) ) {
	    _PTR(AttributeIOR) anIOR( anAttr );
	    const TCollection_AsciiString ASCIor( (char*)anIOR->Value().c_str() );
	    getGeometryGUI()->GetShapeReader().RemoveShapeFromBuffer( ASCIor );
	  }

          CORBA::Object_var corbaObj = GeometryGUI::ClientSObjectToObject(obj);
	  GEOM::GEOM_Object_var geomObj = GEOM::GEOM_Object::_narrow( corbaObj );
	  if ( !CORBA::is_nil( geomObj ) )
	    GeometryGUI::GetGeomGen()->RemoveObject( geomObj );

	  // Remove objects from Study
	  aStudyBuilder->RemoveObject( obj );

	  //deleted = true;
	} // MAIN LOOP of selected

	selected.Clear();
	aSelMgr->setSelectedObjects( selected );
	getGeometryGUI()->updateObjBrowser();
      } // if ( selected not empty )
    } // if ( selMgr && appStudy )

    app->updateActions(); //SRN: To update a Save button in the toolbar

  } // if ( app )


  //  if ( deleted )
  //    op->finish();
  //  else
  //    op->abort();
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
  //SUIT_Application* app = getGeometryGUI()->getApp();
  if (! app) return false;

  SalomeApp_Study* stud = dynamic_cast<SalomeApp_Study*> ( app->activeStudy() );
  if ( !stud ) {
    cout << "FAILED to cast active study to SalomeApp_Study" << endl;
    return false;
  }
  _PTR(Study) aStudy = stud->studyDS();

  bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->GetProperties()))->IsLocked();
  if ( aLocked ) {
    SUIT_MessageBox::warn1 ( app->desktop(),
			    QObject::tr("WRN_WARNING"),
			    QObject::tr("WRN_STUDY_LOCKED"),
			    QObject::tr("BUT_OK") );
    return false;
  }

  GEOM::GEOM_Gen_var eng = GeometryGUI::GetGeomGen();
  if ( CORBA::is_nil( eng ) ) {
    SUIT_MessageBox::error1( app->desktop(),
			    QObject::tr("WRN_WARNING"),
			    QObject::tr( "GEOM Engine is not started" ),
			    QObject::tr("BUT_OK") );
      return false;
    }

  GEOM::GEOM_IInsertOperations_var aInsOp = eng->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;

  GEOM::GEOM_Object_var anObj;

  // Obtain a list of available import formats
  FilterMap aMap;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ImportTranslators( aFormats, aPatterns );

  for ( int i = 0, n = aFormats->length(); i < n; i++ )
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );

  QString fileType;

  QString fileName = getFileName(app->desktop(), "", aMap,
                                 tr("GEOM_MEN_IMPORT"), true, fileType);
  if (fileName.isEmpty() || fileType.isEmpty())
    return false;

  GEOM_Operation* anOp = new GEOM_Operation (app, aInsOp.in());
  try {
    SUIT_OverrideCursor wc;

    app->putInfo(tr("GEOM_PRP_LOADING").arg(SUIT_Tools::file(fileName, /*withExten=*/true)));

    anOp->start();

    CORBA::String_var fileN = fileName.latin1();
    CORBA::String_var fileT = fileType.latin1();
    anObj = aInsOp->Import(fileN, fileT);

    if ( !anObj->_is_nil() && aInsOp->IsDone() ) {
      anObj->SetName(GEOMBase::GetDefaultName(QObject::tr("GEOM_IMPORT")).latin1());
      QString aPublishObjName =
        GEOMBase::GetDefaultName(SUIT_Tools::file(fileName, /*withExten=*/true));

      SALOMEDS::Study_var aDSStudy = GeometryGUI::ClientStudyToStudy(aStudy);
      GeometryGUI::GetGeomGen()->PublishInStudy(aDSStudy,
						SALOMEDS::SObject::_nil(),
						anObj,
						aPublishObjName);

      GEOM_Displayer( stud ).Display( anObj.in() );

      // update data model and object browser
      getGeometryGUI()->updateObjBrowser( true );

      anOp->commit();
    }
    else {
      anOp->abort();
      wc.suspend();
      SUIT_MessageBox::error1( app->desktop(),
			      QObject::tr( "GEOM_ERROR" ),
			      QObject::tr("GEOM_PRP_ABORT") + "\n" + QString( aInsOp->GetErrorCode() ),
			      QObject::tr("BUT_OK") );
    }
  }
  catch( const SALOME::SALOME_Exception& S_ex ) {
    //QtCatchCorbaException(S_ex);
    anOp->abort();
    return false;
  }

  app->updateActions(); //SRN: To update a Save button in the toolbar

  return true;
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
    cout << "FAILED to cast active study to SalomeApp_Study" << endl;
    return false;
  }
  _PTR(Study) aStudy = stud->studyDS();

  GEOM::GEOM_Gen_var eng = GeometryGUI::GetGeomGen();
  if ( CORBA::is_nil( eng ) ) {
    SUIT_MessageBox::error1( app->desktop(),
			     QObject::tr("WRN_WARNING"),
			     QObject::tr( "GEOM Engine is not started" ),
			     QObject::tr("BUT_OK") );
    return false;
  }

  GEOM::GEOM_IInsertOperations_var aInsOp = eng->GetIInsertOperations( aStudy->StudyId() );
  if ( aInsOp->_is_nil() )
    return false;

  // Obtain a list of available export formats
  FilterMap aMap;
  GEOM::string_array_var aFormats, aPatterns;
  aInsOp->ExportTranslators( aFormats, aPatterns );
  for ( int i = 0, n = aFormats->length(); i < n; i++ )
    aMap.insert( (char*)aPatterns[i], (char*)aFormats[i] );

  // Get selected objects
  LightApp_SelectionMgr* sm = app->selectionMgr();
  if ( !sm )
    return false;

  SALOME_ListIO selectedObjects;
  sm->selectedObjects( selectedObjects );

  SALOME_ListIteratorOfListIO It( selectedObjects );
  for(;It.More();It.Next()) {
    Handle(SALOME_InteractiveObject) IObject = It.Value();
    Standard_Boolean found;
    GEOM::GEOM_Object_var anObj = GEOMBase::ConvertIOinGEOMObject(IObject, found);

    if ( !found || anObj->_is_nil() )
      continue;

    QString fileType;
    QString file = getFileName(app->desktop(), QString( IObject->getName() ), aMap,
			       tr("GEOM_MEN_EXPORT"), false, fileType);

    // User has pressed "Cancel" --> stop the operation
    if ( file.isEmpty() || fileType.isEmpty() )
      return false;

    GEOM_Operation* anOp = new GEOM_Operation( app, aInsOp.in() );
    try {
      SUIT_OverrideCursor wc;

      app->putInfo( tr("GEOM_PRP_EXPORT").arg(SUIT_Tools::file( file, /*withExten=*/true )) );

      anOp->start();


      aInsOp->Export( anObj, file, fileType.latin1() );

      if ( aInsOp->IsDone() )
	anOp->commit();
      else
	{
	  anOp->abort();
	  wc.suspend();
	  SUIT_MessageBox::error1( app->desktop(),
				   QObject::tr( "GEOM_ERROR" ),
				   QObject::tr("GEOM_PRP_ABORT") + "\n" + QString( aInsOp->GetErrorCode() ),
				   QObject::tr("BUT_OK") );
	  return false;
	}
    }
    catch (const SALOME::SALOME_Exception& S_ex) {
      //QtCatchCorbaException(S_ex);
      anOp->abort();
      return false;
    }
  }

  return true;
}


QString GEOMToolsGUI::getParentComponent( _PTR( Study ) study, const SALOME_ListIO& iobjs )
{
  QString parentComp;

  for ( SALOME_ListIteratorOfListIO it( iobjs ); it.More(); it.Next() ) {

    Handle(SALOME_InteractiveObject) io = it.Value();
    if ( !io->hasEntry() )
      continue;

    QString compName = getParentComponent( study->FindObjectID( io->getEntry() ) );

    if ( parentComp.isNull() )
      parentComp = compName;
    else if ( parentComp.compare( compName) != 0 ) { // objects belonging to different components are selected
      parentComp = QString::null;
      break;
    }
  }

  return parentComp;
}

QString GEOMToolsGUI::getParentComponent( _PTR( SObject ) obj )
{
  if ( obj ) {
    _PTR(SComponent) comp = obj->GetFatherComponent();
    if ( comp ) {
      _PTR(GenericAttribute) anAttr;
      if ( comp->FindAttribute( anAttr, "AttributeName") ) {
	_PTR(AttributeName) aName( anAttr );
	return QString( aName->Value().c_str() );
      }
    }
  }
  return QString();
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WNT
	__declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new GEOMToolsGUI( parent );
  }
}
