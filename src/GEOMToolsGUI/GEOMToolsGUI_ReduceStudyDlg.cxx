// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOMToolsGUI_ReduceStudyDlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QRadioButton>


// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataOwner.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <OCCViewer_ViewManager.h>
#include <LightApp_DataOwner.h>
#include <GEOMToolsGUI.h>

// GEOM includes
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include <GEOMBase_Helper.h>

#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>

// GEOM includes
#include <GEOMBase.h>

#include <iostream>

GEOMToolsGUI_ReduceStudyDlg::GEOMToolsGUI_ReduceStudyDlg( const GEOM::string_array& theObjectEntries, QWidget* parent )
:QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
myDisplayer(NULL)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myVisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) );
  myInvisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  myStudyId = GeometryGUI::ClientStudyToStudy( study->studyDS() )->StudyId();
  myDisplayer = GEOM_Displayer( study );

  setWindowTitle( tr( "GEOM_REDUCE_STUDY_TITLE" ) );

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /**********************   Objects to be kept    **********************/
  QGroupBox* groupKeptObjects = new QGroupBox( tr( "GEOM_REDUCE_STUDY_KEPT_OBJECTS" ) );
  QGridLayout* layoutKeptObjects = new QGridLayout( groupKeptObjects );
  createTreeWidget( myTreeKeptObjects = new QTreeWidget() );
  layoutKeptObjects->addWidget( myTreeKeptObjects );

  /**********************  Objects to be removed  **********************/
  QGroupBox* groupRemoveObjects = new QGroupBox( tr( "GEOM_REDUCE_STUDY_REMOVE_OBJECTS" ) );
  QGridLayout* layoutRemoveObjects = new QGridLayout( groupRemoveObjects );
  createTreeWidget( myTreeRemoveObjects = new QTreeWidget() );
  layoutRemoveObjects->addWidget( myTreeRemoveObjects );

  /**********************         Options         **********************/
  QGroupBox* groupOptions = new QGroupBox( tr( "GEOM_REDUCE_STUDY_OPTIONS" ) );
  QVBoxLayout* layoutOptions = new QVBoxLayout( groupOptions );

  // Intermidiate objects
  QGroupBox* groupIntermediates = createButtonGroup( myIntermediates = new QButtonGroup() );
  groupIntermediates->setTitle( tr( "GEOM_REDUCE_STUDY_INTERMEDIATES" ) );

  // Sub-objects
  QGroupBox* groupSubObjects = createButtonGroup( mySubObjects = new QButtonGroup() );
  groupSubObjects->setTitle( tr( "GEOM_REDUCE_STUDY_SUB_OBJECTS" ) );

  // Others
  myCBRemoveEmptyFolder = new QCheckBox( tr( "GEOM_REDUCE_STUDY_REMOVE_EMPTY_FOLDER" ) );
  myCBRemoveEmptyFolder->setChecked( true );
  myCBSoftRemoval = new QCheckBox( tr( "GEOM_REDUCE_STUDY_SOFT_REMOVAL" ) );

  layoutOptions->addWidget( groupIntermediates );
  layoutOptions->addWidget( groupSubObjects );
  layoutOptions->addWidget( myCBRemoveEmptyFolder );
  layoutOptions->addWidget( myCBSoftRemoval );

  /**********************         Buttons         **********************/
  QGroupBox* groupButtons = new QGroupBox();
  QHBoxLayout* layoutButtons = new QHBoxLayout( groupButtons );

  QPushButton* buttonOk = new QPushButton( tr( "GEOM_BUT_OK" ) );
  QPushButton* buttonCancel = new QPushButton( tr( "GEOM_BUT_CANCEL" ) );
  QPushButton* buttonHelp = new QPushButton( tr( "GEOM_BUT_HELP" ) );

  layoutButtons->addWidget( buttonOk );
  layoutButtons->addStretch();
  layoutButtons->addWidget( buttonCancel );
  layoutButtons->addWidget( buttonHelp );

  topLayout->addWidget( groupKeptObjects, 0, 0 );
  topLayout->addWidget( groupRemoveObjects, 0, 1 );
  topLayout->addWidget( groupOptions, 1, 0, 1, 2 );
  topLayout->addWidget( groupButtons, 2, 0, 1, 2 );

  // Signals and slots connections

  connect( myTreeKeptObjects, SIGNAL( itemClicked( QTreeWidgetItem*, int) ), this, SLOT( onItemClicked( QTreeWidgetItem*, int ) ) );
  connect( myTreeRemoveObjects, SIGNAL( itemClicked( QTreeWidgetItem*, int) ), this, SLOT( onItemClicked( QTreeWidgetItem*, int ) ) );
  connect( myTreeKeptObjects->header(), SIGNAL( sectionClicked ( int ) ), this, SLOT( onHeaderClicked( int ) ) );
  connect( myTreeRemoveObjects->header(), SIGNAL( sectionClicked ( int ) ), this, SLOT( onHeaderClicked( int ) ) );

  connect( myIntermediates, SIGNAL( buttonClicked( int ) ), this, SLOT( update() ) );
  connect( mySubObjects, SIGNAL( buttonClicked( int ) ), this, SLOT( update() ) );

  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( clickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( clickOnCancel() ) );
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( clickOnHelp() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( selectionChanged() ) );

  init( theObjectEntries );
}

GEOMToolsGUI_ReduceStudyDlg::~GEOMToolsGUI_ReduceStudyDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void GEOMToolsGUI_ReduceStudyDlg::init( const GEOM::string_array& theObjectEntries )
{
  for( int i = 0; i < theObjectEntries.length(); i++ )
    myMainEntries.insert( theObjectEntries[i].in() );


  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  SALOMEDS::Study_var myStudy = GeometryGUI::ClientStudyToStudy( study->studyDS());


  myKeptObjects.clear();
  myParents.clear();
  myListSubObjects.clear();
  myOthers.clear();

  GEOM::string_array_var selectedObjects = new GEOM::string_array();
  selectedObjects->length( theObjectEntries.length() );
  for( int i = 0; i < theObjectEntries.length(); i++ )
    selectedObjects[ i ] = theObjectEntries[i].in();

  GEOM::string_array_var parentsObjects = new GEOM::string_array();
//  parentsObjects->length( 5 );
//  parentsObjects[0] = "0:1:21";
//  parentsObjects[1] = "0:1:17";
//  parentsObjects[2] = "0:1:20";
//  parentsObjects[3] = "0:1:13";
//  parentsObjects[4] = "0:1:12";

  GEOM::string_array_var subObjects = new GEOM::string_array();
//  subObjects->length( 4 );
//  subObjects[0] = "0:1:24";
//  subObjects[1] = "0:1:28";
//  subObjects[2] = "0:1:29";
//  subObjects[3] = "0:1:34";

  GEOM::string_array_var oters = new GEOM::string_array();
//  oters->length( 5 );
//  oters[0] = "0:1:35";
//  oters[1] = "0:1:36";
//  oters[2] = "0:1:37";
//  oters[3] = "0:1:38";
//  oters[4] = "0:1:39";

////// PUT ALGORITHM HERE
  GeometryGUI::GetGeomGen()->GetEntriesToCleanStudy( myStudy, selectedObjects,
    parentsObjects, subObjects, oters );

  for ( int i = 0; i < selectedObjects->length(); i++ )
    myKeptObjects.insert( selectedObjects[i].in() );

  for( int i = 0; i< parentsObjects->length(); i++ )
    myParents.insert( parentsObjects[i].in() );

  for( int i = 0; i< subObjects->length(); i++ )
    myListSubObjects.insert( subObjects[i].in() );

  for( int i = 0; i< oters->length(); i++ )
	  myOthers.insert( oters[i].in() );

//  myParents.insert( "0:1:21" );
//  myParents.insert( "0:1:17" );
//  myParents.insert( "0:1:20" );
//  myParents.insert( "0:1:13" );
//  myParents.insert( "0:1:12" );
//
//  myListSubObjects.insert( "0:1:24" );
//  myListSubObjects.insert( "0:1:28" );
//  myListSubObjects.insert( "0:1:29" );
//  myListSubObjects.insert( "0:1:34" );
//
//  myOthers.insert( "0:1:35" );
//  myOthers.insert( "0:1:36" );
//  myOthers.insert( "0:1:37" );
//  myOthers.insert( "0:1:38" );
//  myOthers.insert( "0:1:39" );

  update();

  checkVisibleIcon( myTreeKeptObjects );
  checkVisibleIcon( myTreeRemoveObjects );
}

void GEOMToolsGUI_ReduceStudyDlg::createTreeWidget( QTreeWidget* theTreeWidget )
{
  theTreeWidget->setColumnCount( 2 );
  QStringList columnNames;
  columnNames.append(tr( "GEOM_REDUCE_STUDY_NAME" ));
  columnNames.append("");
  theTreeWidget->setHeaderLabels( columnNames );
  QTreeWidgetItem * headerItem = new QTreeWidgetItem( columnNames );
  theTreeWidget->setHeaderItem ( headerItem );
  theTreeWidget->header()->moveSection( 1, 0 );
  theTreeWidget->header()->setClickable( true );
  theTreeWidget->header()->setMovable( false );
  theTreeWidget->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
  theTreeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
}

QGroupBox* GEOMToolsGUI_ReduceStudyDlg::createButtonGroup( QButtonGroup* theButtonGroup )
{
  QGroupBox* groupObjects = new QGroupBox();
  QHBoxLayout* layoutObjects = new QHBoxLayout( groupObjects );

  QRadioButton* buttonKeep = new QRadioButton( tr( "GEOM_REDUCE_STUDY_KEEP") );
  theButtonGroup->addButton( buttonKeep, 0 );
  QRadioButton* buttonUnpublish = new QRadioButton( tr( "GEOM_REDUCE_STUDY_UNPUBLISH") );
  theButtonGroup->addButton( buttonUnpublish, 1 );
  QRadioButton* buttonRemove = new QRadioButton( tr( "GEOM_REDUCE_STUDY_REMOVE") );
  theButtonGroup->addButton( buttonRemove, 2 );

  theButtonGroup->button( 0 )->setChecked( true );
  theButtonGroup->setExclusive( true );

  layoutObjects->addWidget( buttonKeep );
  layoutObjects->addWidget( buttonUnpublish );
  layoutObjects->addWidget( buttonRemove );

  return groupObjects;
}

void GEOMToolsGUI_ReduceStudyDlg::update()
{

  myTreeKeptObjects->clear();
  myTreeRemoveObjects->clear();

  std::set<std::string> keptObjects(myKeptObjects);
  std::set<std::string> removeObjects(myOthers);

  // Intermediate objects
  if( myIntermediates->checkedId() == 2 ) { // remove
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      removeObjects.insert( *iter );
  }
  else { // keep or unpublish
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      keptObjects.insert( *iter );
  }

  // Sub-objects
  if( mySubObjects->checkedId() == 2 ) {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      removeObjects.insert( *iter );
  }
  else {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      keptObjects.insert( *iter );
  }

  std::cout<<"\n\n\n Objects to be kept: ";
  std::set<std::string>::iterator it_kept;
  for ( it_kept=keptObjects.begin(); it_kept!=keptObjects.end(); ++it_kept)
    std::cout << ", " << *it_kept;
  std::cout << std::endl;

  std::cout<<"\n\n\n Objects to be remove: ";
  std::set<std::string>::iterator it_remove;
  for ( it_remove=removeObjects.begin(); it_remove!=removeObjects.end(); ++it_remove)
    std::cout << ", " << *it_remove;
  std::cout << std::endl;

  sortObjects( myTreeKeptObjects, keptObjects );
  sortObjects( myTreeRemoveObjects, removeObjects );

  myTreeKeptObjects->collapseAll();
  myTreeRemoveObjects->collapseAll();

}

void GEOMToolsGUI_ReduceStudyDlg::selectionChanged()
{
  myMainEntries.clear();
  GEOM::string_array_var objectsEntry = new GEOM::string_array();
  SALOME_ListIO mainObjects;
  int iter = 0;

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  app->selectionMgr()->selectedObjects( mainObjects );
  // create a list of selected object entry
  objectsEntry->length( mainObjects.Extent() );
  for ( SALOME_ListIteratorOfListIO It( mainObjects ); It.More(); It.Next(), iter++ ) {
	  Handle( SALOME_InteractiveObject ) io = It.Value();
    if( !io->hasEntry() )
      continue;
    GEOM::GEOM_Object_var geomObject = GEOM::GEOM_Object::_nil();
    geomObject = GEOMBase::ConvertIOinGEOMObject( io );
    QString entry = geomObject->GetEntry();
    objectsEntry[ iter ] = entry.toLatin1().constData();
  }
  init(objectsEntry);
}

void GEOMToolsGUI_ReduceStudyDlg::clickOnOk()
{

  std::set<std::string> ObjectsToBeRemove = myOthers;
  std::set<std::string> ObjectsToBeUnpublish;

  if( myIntermediates->checkedId() == 2 ) {
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      ObjectsToBeRemove.insert( *iter );
  }
  else if( myIntermediates->checkedId() == 1 ) {
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      ObjectsToBeUnpublish.insert( *iter );
  }

  if( mySubObjects->checkedId() == 2 ) {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      ObjectsToBeRemove.insert( *iter );
  }
  else if( mySubObjects->checkedId() == 1 ) {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      ObjectsToBeUnpublish.insert( *iter );
  }

  unpublishObjects( ObjectsToBeUnpublish );
  removeObjects( ObjectsToBeRemove );


//  std::cout<< "\n\n REMOVE:" << std::endl;
//  std::set<std::string>::iterator it;
//  for( it = ObjectsToBeRemove.begin(); it != ObjectsToBeRemove.end(); ++it )
//    std::cout <<"  " << (*it) << std::endl;
//
//  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
//  if ( !app ) return;
//  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
//  // get currently opened views
//  QList<SALOME_View*> views;
//  SALOME_View* view;
//  ViewManagerList vmans = app->viewManagers();
//  SUIT_ViewManager* vman;
//  foreach ( vman, vmans ) {
//    SUIT_ViewModel* vmod = vman->getViewModel();
//    view = dynamic_cast<SALOME_View*> ( vmod ); // must work for OCC and VTK views
//    if ( view )
//      views.append( view );
//  }
//  _PTR(Study) aStudy = study->studyDS();


//  _PTR(StudyBuilder) aStudyBuilder (aStudy->NewBuilder());
//  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();
//
//  for( it = ObjectsToBeRemove.begin(); it != ObjectsToBeRemove.end(); ++it ) {
//    std::string objectEntry = *it;
//    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudyId, objectEntry.c_str() );
//    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
//    _PTR(SObject) obj = aStudy->FindObjectID( objectEntry.c_str() );
//
//    //Remove visual properties of the object
//    study->removeObjectFromAll(objectEntry.c_str());
//
//    // remove object from study
//    aStudyBuilder->RemoveObjectWithChildren( obj );
////    // remove object from use case tree
////    aUseCaseBuilder->Remove( obj );
//
//    // Erase graphical object
//    QListIterator<SALOME_View*> it( views );
//    while ( it.hasNext() )
//      if ( SALOME_View* view = it.next() )
//        myDisplayer.Erase( GeomObject, true, true, view);
//
//    GeometryGUI::GetGeomGen()->RemoveObject( GeomBaseObject );
//  }
//
//  myDisplayer.UpdateViewer();
//  GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
//  aGeomGUI->updateObjBrowser();
//  app->updateActions(); //SRN: To update a Save button in the toolbar


//  _PTR(StudyBuilder) B = aStudy->NewBuilder();
//  for( it = ObjectsToBeUnpublish.begin(); it != ObjectsToBeUnpublish.end(); ++it ) {
//    std::string objectEntry = *it;
//    _PTR(SObject) obj ( aStudy->FindObjectID( objectEntry ) );
//    _PTR(GenericAttribute) anAttr;
//    if ( obj ) {
//      _PTR(AttributeDrawable) aDrw = B->FindOrCreateAttribute( obj, "AttributeDrawable" );
//      aDrw->SetDrawable( false );
//      myDisplayer.EraseWithChildren( new SALOME_InteractiveObject( objectEntry.c_str(), "GEOM", "TEMP_IO" ) );
//    } // if ( obj )
//  } // iterator
//
//  app->updateObjectBrowser( false );
//  app->updateActions();
//

//  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
//  SALOME_ListIO aSelList;
//
//  for( it = ObjectsToBeUnpublish.begin(); it != ObjectsToBeUnpublish.end(); ++it ) {
//    std::string objectEntry = *it;
//    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudyId, objectEntry.c_str() );
//    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
//    QString studyEntry = GeomBaseObject->GetStudyEntry();
//    Handle(SALOME_InteractiveObject) tmpIO =
//      new SALOME_InteractiveObject( studyEntry.toStdString().c_str(), "GEOM", "TEMP_IO");
//    aSelList.Append(tmpIO);
//
//  }
//  aSelMgr->clearSelected();
//  aSelMgr->setSelectedObjects(aSelList);
//  GEOMToolsGUI::OnUnpublishObject();

  accept();



//  if( myCBRemoveEmptyFolder->isChecked() ) {
//
//	  SALOME_ListIO selected;
//	  SalomeApp_Application* app =
//	    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
//	  if ( !app )
//	    return;
//
//	  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
//	  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
//	  if ( !aSelMgr || !appStudy )
//	    return;
//
//	  // get selection
//	  aSelMgr->selectedObjects( selected, "ObjectBrowser", false );
//	  if ( selected.IsEmpty() )
//	    return;
//
//	  _PTR(Study) aStudy = appStudy->studyDS();
//	  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();
//
//	    // ... and then delete all folders
//	    for ( it = toBeDelFolders.begin(); it != toBeDelFolders.end(); ++it ) {
//	      _PTR(SObject) obj ( aStudy->FindObjectID( it.key().toLatin1().data() ) );
//	      // remove object from GEOM engine
//	      removeObjectWithChildren( obj, aStudy, views, disp );
//	      // remove objects from study
//	      aStudyBuilder->RemoveObjectWithChildren( obj );
//	      // remove object from use case tree
//	      aUseCaseBuilder->Remove( obj );
//	    }
//  }
}

////=======================================================================
//// function : getGeomChildrenAndFolders
//// purpose  : Get direct (1-level) GEOM objects under each folder, sub-folder, etc. and these folders itself
////=======================================================================
//static void getGeomChildrenAndFolders( _PTR(SObject) theSO,
//                                       QMap<QString,QString>& geomObjList,
//                                       QMap<QString,QString>& folderList ) {
//  if ( !theSO ) return;
//  _PTR(Study) aStudy = theSO->GetStudy();
//  if ( !aStudy ) return;
//  _PTR(UseCaseBuilder) aUseCaseBuilder = aStudy->GetUseCaseBuilder();
//
//  bool isFolder = false;
//  _PTR(GenericAttribute) anAttr;
//  if ( theSO->FindAttribute(anAttr, "AttributeLocalID") ) {
//    _PTR(AttributeLocalID) aLocalID( anAttr );
//    isFolder = aLocalID->Value() == 999;
//  }
//  QString anEntry = theSO->GetID().c_str();
//  QString aName = theSO->GetName().c_str();
//  if ( isFolder ) {
//    folderList.insert( anEntry, aName );
//    _PTR(UseCaseIterator) ucit ( aUseCaseBuilder->GetUseCaseIterator( theSO ) );
//    for ( ucit->Init( false ); ucit->More(); ucit->Next() ) {
//      getGeomChildrenAndFolders( ucit->Value(), geomObjList, folderList );
//    }
//  } else {
//    geomObjList.insert( anEntry, aName );
//  }
//}


void GEOMToolsGUI_ReduceStudyDlg::unpublishObjects( std::set<std::string>& theObjects )
{

}

void GEOMToolsGUI_ReduceStudyDlg::removeObjects( std::set<std::string>& theObjects )
{

}


void GEOMToolsGUI_ReduceStudyDlg::clickOnCancel()
{
  close();
}
void GEOMToolsGUI_ReduceStudyDlg::clickOnHelp()
{

}

//=================================================================================
// function : onItemClicked()
// purpose  : Called then treeItem clicked
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::onItemClicked( QTreeWidgetItem* theItem, int theColumn )
{
  if( theColumn != 1 || !( theItem->flags() & Qt::ItemIsSelectable ) )
    return;

  GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>( theItem );

  const char* entry = item->getStudyEntry();
  Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( entry, "GEOM", "TEMP_IO" );
  if( myDisplayer.IsDisplayed( entry ) ) {
    item->setVisible( false, myInvisible );
    myDisplayer.Erase( io );
  }
  else {
    item->setVisible( true, myVisible );
    myDisplayer.Display( io );
  }
  myDisplayer.UpdateViewer();
  checkVisibleIcon( item->treeWidget() );
}

void GEOMToolsGUI_ReduceStudyDlg::onHeaderClicked( int theColumn )
{
  if( theColumn != 1 )
    return;

  QTreeWidget* treeData = dynamic_cast<QTreeWidget*>(sender()->parent());
  if( myMapTreeSelectAll[ treeData ] ) {
    myMapTreeSelectAll[ treeData ] = false;
    treeData->headerItem()->setIcon( 1, myInvisible );
    QTreeWidgetItemIterator it( treeData );
    while(*it) {
      GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
      if( ( item->flags() & Qt::ItemIsSelectable ) && item->isVisible() ) {
        const char* entry = item->getStudyEntry();
        item->setVisible( false, myInvisible );
        myDisplayer.Erase( new SALOME_InteractiveObject( entry, "GEOM", "TEMP_IO" ) );
      }
      ++it;
    }
  }
  else {
    myMapTreeSelectAll[ treeData ] = true;
    treeData->headerItem()->setIcon( 1, myVisible );
    QTreeWidgetItemIterator it( treeData );
    while(*it) {
      GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
      if( ( item->flags() & Qt::ItemIsSelectable ) && !item->isVisible() ) {
        const char* entry = item->getStudyEntry();
        item->setVisible( true, myVisible );
        myDisplayer.Display( new SALOME_InteractiveObject( entry, "GEOM", "TEMP_IO" ) );
      }
      ++it;
    }
  }
  myDisplayer.UpdateViewer();
}

void GEOMToolsGUI_ReduceStudyDlg::checkVisibleIcon( QTreeWidget* theWidget )
{
  bool isInvisible = false;
  QTreeWidgetItemIterator it( theWidget );
  while(*it) {
    GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
    const char* entry = item->getStudyEntry();
    if( item->flags() & Qt::ItemIsSelectable )
      if( !item->isVisible() )
        isInvisible = true;
    ++it;
  }

  if( isInvisible ) {
    theWidget->headerItem()->setIcon( 1, myInvisible );
    myMapTreeSelectAll[ theWidget ] = false;
  }
  else {
    theWidget->headerItem()->setIcon( 1, myVisible );
    myMapTreeSelectAll[ theWidget ] = true;
  }
}

void GEOMToolsGUI_ReduceStudyDlg::sortObjects( QTreeWidget* theWidget, std::set<std::string>& theObjects )
{
  std::set<std::string>::iterator it;
  for( it = theObjects.begin(); it != theObjects.end(); ++it ) {
    std::string objectEntry = *it;
    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudyId, objectEntry.c_str() );
    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
    QString studyEntry = GeomBaseObject->GetStudyEntry();
    if( GeomObject->_is_nil() || studyEntry.isEmpty() )
      continue;
    addSubObject( theWidget, theObjects, GeomObject );
  }
}

GEOMToolsGUI_TreeWidgetItem* GEOMToolsGUI_ReduceStudyDlg::addSubObject( QTreeWidget* theWidget, std::set<std::string>& theObjects, GEOM::GEOM_Object_var theObject )
{
  GEOMToolsGUI_TreeWidgetItem* item;
  if( !theObject->IsMainShape() ) {
    GEOMToolsGUI_TreeWidgetItem* parentItem = addSubObject( theWidget, theObjects, theObject->GetMainShape() );
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( parentItem, QStringList() << theObject->GetName(), theObject->GetStudyEntry(), false );
  }
  else {
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( theWidget, QStringList() << theObject->GetName(), theObject->GetStudyEntry(), false );
  }

  bool isDisplayed = false;
  if( theObjects.find( theObject->GetEntry() ) != theObjects.end() ) {
    isDisplayed = myDisplayer.IsDisplayed( theObject->GetStudyEntry() );
    if ( isDisplayed ) {
      item->setVisible( true, myVisible );
    }
    else {
      item->setVisible( false, myInvisible );
    }
    if( myMainEntries.find( theObject->GetEntry() ) != myMainEntries.end() ) {
      QFont Textfont = item->font(0);
      Textfont.setBold( true );
      item->setFont( 0, Textfont );
    }

  }
  else {
    item->setFlags( item->flags() & ~Qt::ItemIsSelectable );
    item->setTextColor( 0, QColor( 150, 150, 150 ) );
  }

  return item;
}

GEOMToolsGUI_TreeWidgetItem* GEOMToolsGUI_ReduceStudyDlg::findObjectInTree( QTreeWidget* theWidget, GEOM::GEOM_Object_var theObject )
{
  QTreeWidgetItemIterator it( theWidget );
  while(*it) {
    GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
    if( QString( item->getStudyEntry() ) == QString( theObject->GetStudyEntry() ) )
      return item;
    ++it;
  }
  return NULL;
}

GEOMToolsGUI_TreeWidgetItem::GEOMToolsGUI_TreeWidgetItem( QTreeWidget* view, const QStringList &strings,
                                                          char* studyEntry, bool visible, int type )
:QTreeWidgetItem( view, strings, type ),
 myStudyEntry( studyEntry ),
 myVisible( visible )
{

}

GEOMToolsGUI_TreeWidgetItem::GEOMToolsGUI_TreeWidgetItem( QTreeWidgetItem* parent, const QStringList &strings,
                                                          char* studyEntry, bool visible, int type )
:QTreeWidgetItem( parent, strings, type ),
 myStudyEntry( studyEntry ),
 myVisible( visible )
{

}

GEOMToolsGUI_TreeWidgetItem::~GEOMToolsGUI_TreeWidgetItem()
{
}

bool GEOMToolsGUI_TreeWidgetItem::isVisible()
{
  return myVisible;
}

void GEOMToolsGUI_TreeWidgetItem::setVisible( bool theIsVisible, QIcon& theIcon )
{
  myVisible = theIsVisible;
  setIcon( 1, theIcon );
}

char* GEOMToolsGUI_TreeWidgetItem::getStudyEntry() const
{
  return myStudyEntry;
}
