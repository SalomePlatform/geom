// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "GEOMToolsGUI_ReduceStudyDlg.h"

// GEOM includes
#include <GEOMBase.h>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>

#include <LightApp_SelectionMgr.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>

// Qt includes
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QHeaderView>
#include <QMessageBox>

GEOMToolsGUI_ReduceStudyDlg::GEOMToolsGUI_ReduceStudyDlg( QWidget* parent )
:QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
myDisplayer(NULL)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myVisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) );
  myInvisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) );

  myApp = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !myApp ) return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
  myStudy = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() )->studyDS();
  myDisplayer = GEOM_Displayer( study );

  setWindowTitle( tr( "GEOM_REDUCE_STUDY_TITLE" ) );
  setAttribute(Qt::WA_DeleteOnClose);

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

  // Intermediate objects
  QGroupBox* groupIntermediates = createButtonGroup( myGroupIntermediates = new QButtonGroup() );
  groupIntermediates->setTitle( tr( "GEOM_REDUCE_STUDY_INTERMEDIATES" ) );

  // Sub-objects
  QGroupBox* groupSubObjects = createButtonGroup( myGroupSubObjects = new QButtonGroup() );
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

  connect( myGroupIntermediates, SIGNAL( buttonClicked( int ) ), this, SLOT( update() ) );
  connect( myGroupSubObjects, SIGNAL( buttonClicked( int ) ), this, SLOT( update() ) );

  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( clickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( clickOnHelp() ) );

  connect( myApp->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( selectionChanged() ) );

  init( getSelectedObjects() );
}

GEOMToolsGUI_ReduceStudyDlg::~GEOMToolsGUI_ReduceStudyDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : init()
// purpose  : initialize dialog data
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::init( const std::set<std::string>& theObjectEntries )
{
  myMainEntries.clear();

  myKeptObjects.clear();
  myListParents.clear();
  myListSubObjects.clear();
  myRemovedObjects.clear();

  myMainEntries = theObjectEntries;

  GEOM::string_array_var keptObjects = new GEOM::string_array();
  int It = 0;
  keptObjects->length( theObjectEntries.size() );
  std::set<std::string>::iterator iter;
  for( iter=theObjectEntries.begin(); iter!=theObjectEntries.end(); ++iter, It++ )
    keptObjects[ It ] = (*iter).c_str();

  GEOM::string_array_var parentsObjects = new GEOM::string_array();
  GEOM::string_array_var subObjects = new GEOM::string_array();
  GEOM::string_array_var otherObjects = new GEOM::string_array();

  GeometryGUI::GetGeomGen()->GetEntriesToReduceStudy( GeometryGUI::ClientStudyToStudy( myStudy ),
		                                             keptObjects, parentsObjects,
		                                             subObjects, otherObjects );

  for ( int i = 0; i < keptObjects->length(); i++ )
    myKeptObjects.insert( keptObjects[i].in() );
  for( int i = 0; i< otherObjects->length(); i++ )
    myRemovedObjects.insert( otherObjects[i].in() );
  for( int i = 0; i< parentsObjects->length(); i++ )
    myListParents.insert( parentsObjects[i].in() );
  for( int i = 0; i< subObjects->length(); i++ )
    myListSubObjects.insert( subObjects[i].in() );

  update();

  checkVisibleIcon( myTreeKeptObjects );
  checkVisibleIcon( myTreeRemoveObjects );
}

//=================================================================================
// function : getSelectedObjects()
// purpose  : get selected objects in object browser
//=================================================================================
std::set<std::string> GEOMToolsGUI_ReduceStudyDlg::getSelectedObjects() const
{
  std::set<std::string> objects;

  SALOME_ListIO selected;
  myApp->selectionMgr()->selectedObjects( selected );

  for( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    if( !io->hasEntry() )
      continue;
    GEOM::GEOM_Object_var geomObject = GEOM::GEOM_Object::_nil();
    geomObject = GEOMBase::ConvertIOinGEOMObject( io );
    if( geomObject->_is_nil() )
      continue;
    QString entry = geomObject->GetEntry();
    objects.insert( entry.toStdString().c_str() );
  }
  return objects;
}

//=================================================================================
// function : createTreeWidget()
// purpose  : create tree widget for unpublished or removed objects
//=================================================================================
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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  theTreeWidget->header()->setClickable( true );
  theTreeWidget->header()->setMovable( false );
  theTreeWidget->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
#else
  theTreeWidget->header()->setSectionsClickable( true );
  theTreeWidget->header()->setSectionsMovable( false );
  theTreeWidget->header()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );
#endif
  theTreeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );
}

//=================================================================================
// function : createButtonGroup()
// purpose  : create button group for intermediate objects or sub-objects
//=================================================================================
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

//=================================================================================
// function : addObjectsToTree()
// purpose  : add the list of objects to tree
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::addObjectsToTree( QTreeWidget* theWidget, std::set<std::string>& theObjects )
{
  std::set<std::string>::iterator it;
  for( it = theObjects.begin(); it != theObjects.end(); ++it ) {
    std::string objectEntry = *it;
    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudy->StudyId(), objectEntry.c_str() );
    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
    QString studyEntry = GeomBaseObject->GetStudyEntry();
    if( GeomObject->_is_nil() || studyEntry.isEmpty() || !isObjectDrawable( studyEntry.toStdString() ) )
      continue;
    addSubObject( theWidget, theObjects, GeomObject );
  }
}

//=================================================================================
// function : addSubObject()
// purpose  : add sub-object to parent object in the tree
//=================================================================================
GEOMToolsGUI_TreeWidgetItem* GEOMToolsGUI_ReduceStudyDlg::addSubObject( QTreeWidget* theWidget,
                                                                        std::set<std::string>& theObjects,
                                                                        GEOM::GEOM_Object_var theObject )
{
  GEOMToolsGUI_TreeWidgetItem* item;
  if( !theObject->IsMainShape() ) {
    GEOM::GEOM_Object_var aMainShape = theObject->GetMainShape();
    if ( CORBA::is_nil( aMainShape ) )
      return NULL;
    GEOMToolsGUI_TreeWidgetItem* parentItem = addSubObject( theWidget, theObjects, aMainShape );
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( parentItem, QStringList() << theObject->GetName(), theObject->GetStudyEntry() );
  }
  else {
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( theWidget, QStringList() << theObject->GetName(), theObject->GetStudyEntry() );
  }

  bool isDisplayed = false;
  if( theObjects.find( theObject->GetEntry() ) != theObjects.end() ) {
    isDisplayed = myDisplayer.IsDisplayed( theObject->GetStudyEntry() );
    if ( isDisplayed )
      item->setVisible( true, myVisible );
    else
      item->setVisible( false, myInvisible );

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

//=================================================================================
// function : findObjectInTree()
// purpose  : find object in the tree
//=================================================================================
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

//=================================================================================
// function : checkVisibleIcon()
// purpose  : set visible or invisible icon in the header of tree
//=================================================================================
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

//=================================================================================
// function : isObjectDrawable()
// purpose  : return true if object is drawable, and false if object is hidden in the study
//=================================================================================
bool GEOMToolsGUI_ReduceStudyDlg::isObjectDrawable( std::string theStudyEntry )
{
  _PTR(StudyBuilder) aStudyBuilder = myStudy->NewBuilder();
  //If object hasn't "AttributeDrawable" => it visible
  bool isDrawable = true;
  _PTR(SObject) SO ( myStudy->FindObjectID( theStudyEntry ) );
  _PTR(GenericAttribute) anAttr;
  if ( SO && SO->FindAttribute( anAttr, "AttributeDrawable" ) ) {
    _PTR(AttributeDrawable) aDrw (anAttr);
    isDrawable = aDrw->IsDrawable();
  }
  return isDrawable;
}

//=================================================================================
// function : unpublishObjects()
// purpose  : unpublish(hide) objects in the study
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::unpublishObjects( std::set<std::string>& theObjects )
{
  _PTR(StudyBuilder) aStudyBuilder = myStudy->NewBuilder();
  std::set<std::string>::iterator it;
  for( it = theObjects.begin(); it != theObjects.end(); ++it ) {
    std::string objectEntry = *it;
    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudy->StudyId(),
                                                                                     objectEntry.c_str() );
    std::string studyEntry = GeomBaseObject->GetStudyEntry();
    if ( studyEntry == "" || !isObjectDrawable( studyEntry ) )
      continue;
    _PTR(SObject) obj ( myStudy->FindObjectID( studyEntry.c_str() ) );
    _PTR(GenericAttribute) anAttr;
    if ( obj ) {
      _PTR(AttributeDrawable) aDrw = aStudyBuilder->FindOrCreateAttribute( obj, "AttributeDrawable" );
      aDrw->SetDrawable( false );
      myDisplayer.EraseWithChildren( new SALOME_InteractiveObject( studyEntry.c_str(), "GEOM", "TEMP_IO" ) );
      // hide references if any
      std::vector< _PTR(SObject) > vso = myStudy->FindDependances(obj);
      for ( int i = 0; i < vso.size(); i++ ) {
        _PTR(SObject) refObj = vso[i];
        aDrw = aStudyBuilder->FindOrCreateAttribute( refObj, "AttributeDrawable" );
        aDrw->SetDrawable( false );
      }
    }
  }
  myApp->updateObjectBrowser( false );
  myApp->updateActions();
}

//=================================================================================
// function : removeObjects()
// purpose  : remove objects from the study
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::removeObjects( std::set<std::string>& theObjects )
{
  std::set<std::string>::iterator it;
  for( it = theObjects.begin(); it != theObjects.end(); ++it ) {
    std::string objectEntry = *it;
    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( myStudy->StudyId(),
                                                                                     objectEntry.c_str() );
    std::string studyEntry = GeomBaseObject->GetStudyEntry();
    if ( studyEntry == "" )
      GeometryGUI::GetGeomGen()->RemoveObject( GeomBaseObject );
    else {
      if( !isObjectDrawable( studyEntry ) )
        continue;
      removeObject( studyEntry );
    }
  }
  myApp->updateObjectBrowser( false );
  myApp->updateActions();
}

//=================================================================================
// function : removeObject()
// purpose  : remove object with given study entry
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::removeObject( std::string& theStudyEntry )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( myApp->activeStudy() );
  _PTR(StudyBuilder) aStudyBuilder = myStudy->NewBuilder();
  _PTR(UseCaseBuilder) aUseCaseBuilder = myStudy->GetUseCaseBuilder();

  _PTR(SObject) obj ( myStudy->FindObjectID( theStudyEntry.c_str() ) );
  if ( obj ) {
    // remove visual properties of the object
    appStudy->removeObjectProperties(obj->GetID().c_str());
    // remove references to this object
    appStudy->deleteReferencesTo( obj );
    // remove objects from study
    aStudyBuilder->RemoveObjectWithChildren( obj );
    // remove object from use case tree
    aUseCaseBuilder->Remove( obj );
    myDisplayer.EraseWithChildren( new SALOME_InteractiveObject( theStudyEntry.c_str(), "GEOM", "TEMP_IO" ) );
  }
}

//=================================================================================
// function : removeEmptyFolders()
// purpose  : remove empty folders from the study
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::removeEmptyFolders()
{
  std::set<std::string> emptyFolders;

  _PTR(SComponent) SC ( myStudy->FindComponent( "GEOM" ) );
  if ( !SC )
    return;
  _PTR(ChildIterator) anIter ( myStudy->NewChildIterator( SC ) );
  anIter->InitEx( true );
  while( anIter->More() ) {
    _PTR(SObject) valSO ( anIter->Value() );
    _PTR(SObject) refSO;
    if ( !valSO->ReferencedObject( refSO ) )
      getEmptyFolders( valSO, emptyFolders );
    anIter->Next();
  }

  std::set<std::string>::iterator iter;
  for( iter = emptyFolders.begin(); iter != emptyFolders.end(); ++iter ) {
    std::string studyEntry = *iter;
    removeObject( studyEntry );
  }
  myApp->updateObjectBrowser( false );
  myApp->updateActions();
}

//=================================================================================
// function : removeEmptyFolders()
// purpose  : remove empty folders from the study
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::getEmptyFolders( _PTR(SObject) theSO, std::set<std::string>& theFolders )
{
  _PTR(UseCaseBuilder) aUseCaseBuilder = myStudy->GetUseCaseBuilder();

  bool isFolder = false;
  _PTR(GenericAttribute) anAttr;
  if ( theSO->FindAttribute(anAttr, "AttributeLocalID") ) {
    _PTR(AttributeLocalID) aLocalID( anAttr );
    isFolder = aLocalID->Value() == 999;
  }
  QString studyEntry = theSO->GetID().c_str();
  if ( isFolder ) {
    if( !aUseCaseBuilder->HasChildren( theSO ) )
      theFolders.insert( studyEntry.toStdString() );
    else {
      _PTR(UseCaseIterator) ucit ( aUseCaseBuilder->GetUseCaseIterator( theSO ) );
      for ( ucit->Init( false ); ucit->More(); ucit->Next() )
        getEmptyFolders( ucit->Value(), theFolders );
    }
  }
}

//=================================================================================
// function : onItemClicked()
// purpose  : called when tree item was clicked
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

//=================================================================================
// function : onHeaderClicked()
// purpose  : called when header item of tree was clicked
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::onHeaderClicked( int theColumn )
{
  if( theColumn != 1 )
    return;

  QTreeWidget* treeWidget = dynamic_cast<QTreeWidget*>(sender()->parent());
  if( myMapTreeSelectAll[ treeWidget ] ) {
    myMapTreeSelectAll[ treeWidget ] = false;
    treeWidget->headerItem()->setIcon( 1, myInvisible );
    QTreeWidgetItemIterator it( treeWidget );
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
    myMapTreeSelectAll[ treeWidget ] = true;
    treeWidget->headerItem()->setIcon( 1, myVisible );
    QTreeWidgetItemIterator it( treeWidget );
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

//=================================================================================
// function : selectionChanged()
// purpose  : called when selection of object browser was changed
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::selectionChanged()
{
  init( getSelectedObjects() );
}

//=================================================================================
// function : update()
// purpose  : update tree data
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::update()
{
  myTreeKeptObjects->clear();
  myTreeRemoveObjects->clear();

  std::set<std::string> keptObjects( myKeptObjects );
  std::set<std::string> removeObjects( myRemovedObjects );

  // Intermediate objects
  if( myGroupIntermediates->checkedId() == 2 ) { // remove
    std::set<std::string>::iterator iter;
    for( iter=myListParents.begin(); iter!=myListParents.end(); ++iter)
      removeObjects.insert( *iter );
  }
  else { // keep or unpublish
    std::set<std::string>::iterator iter;
    for( iter=myListParents.begin(); iter!=myListParents.end(); ++iter)
      keptObjects.insert( *iter );
  }

  // Sub-objects
  if( myGroupSubObjects->checkedId() == 2 ) {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      removeObjects.insert( *iter );
  }
  else {
    std::set<std::string>::iterator iter;
    for( iter=myListSubObjects.begin(); iter!=myListSubObjects.end(); ++iter)
      keptObjects.insert( *iter );
  }

  addObjectsToTree( myTreeKeptObjects, keptObjects );
  addObjectsToTree( myTreeRemoveObjects, removeObjects );

  myTreeKeptObjects->collapseAll();
  myTreeRemoveObjects->collapseAll();

}

//=================================================================================
// function : clickOnOk()
// purpose  : called when OK button was clicked
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::clickOnOk()
{
  std::set<std::string> objectsToBeRemoved = myRemovedObjects;
  std::set<std::string> objectsToBeUnpublished;

  // Create lists of intermediate objects to be removed or to be unpublished
  std::set<std::string>::iterator iter;
  if( myGroupIntermediates->checkedId() == 1 ) { // unpublish
    for( iter = myListParents.begin(); iter != myListParents.end(); ++iter )
    objectsToBeUnpublished.insert( *iter );
  }
  if( myGroupIntermediates->checkedId() == 2 ) { // remove
    if( !myCBSoftRemoval->isChecked() && 
	SUIT_MessageBox::question( this,
				   tr( "GEOM_WRN_WARNING" ),
				   tr( "GEOM_REDUCE_STUDY_WARNING_DELETE" ),
				   QMessageBox::Yes | QMessageBox::No,
				   QMessageBox::Yes ) == QMessageBox::No ) {
      return;
    }
    for( iter = myListParents.begin(); iter != myListParents.end(); ++iter )
      objectsToBeRemoved.insert( *iter );
  }

  // Create lists of sub-objects to be removed or to be unpublished
  if( myGroupSubObjects->checkedId() == 1 ) { // unpublish
    for( iter = myListSubObjects.begin(); iter != myListSubObjects.end(); ++iter )
      objectsToBeUnpublished.insert( *iter );
  }
  else if( myGroupSubObjects->checkedId() == 2 ) { // remove
    for( iter = myListSubObjects.begin(); iter != myListSubObjects.end(); ++iter )
      objectsToBeRemoved.insert( *iter );
  }

  // if user chosen the soft removal
  if( myCBSoftRemoval->isChecked() ) {
    for( iter = objectsToBeRemoved.begin(); iter != objectsToBeRemoved.end(); ++iter )
      objectsToBeUnpublished.insert( *iter );
    unpublishObjects( objectsToBeUnpublished );
  }
  else {
    unpublishObjects( objectsToBeUnpublished );
    removeObjects( objectsToBeRemoved );
  }

  // if user want to delete the empty folders
  if( myCBRemoveEmptyFolder->isChecked() )
    removeEmptyFolders();

  accept();
}

//=================================================================================
// function : clickOnHelp()
// purpose  : called when Help button was clicked to open a help page
//=================================================================================
void GEOMToolsGUI_ReduceStudyDlg::clickOnHelp()
{
  myApp->onHelpContextModule( "GEOM", "reduce_study_page.html" );
}

GEOMToolsGUI_TreeWidgetItem::GEOMToolsGUI_TreeWidgetItem( QTreeWidget* view, const QStringList &strings,
                                                          char* studyEntry, int type )
:QTreeWidgetItem( view, strings, type ),
 myStudyEntry( studyEntry ),
 myVisible( false )
{
}

GEOMToolsGUI_TreeWidgetItem::GEOMToolsGUI_TreeWidgetItem( QTreeWidgetItem* parent, const QStringList &strings,
                                                          char* studyEntry, int type )
:QTreeWidgetItem( parent, strings, type ),
 myStudyEntry( studyEntry ),
 myVisible( false )
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
