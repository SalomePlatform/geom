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

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <OCCViewer_ViewManager.h>

// GEOM includes
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include <GEOMBase_Helper.h>

#include <iostream>

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

GEOMToolsGUI_ReduceStudyDlg::GEOMToolsGUI_ReduceStudyDlg( const GEOM::string_array& theObjectEntries, QWidget* parent )
:QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
mySelectAll( false ),
myDisplayer(NULL)
{
  for( int i = 0; i < theObjectEntries.length(); i++ )
    myMainEntries.insert( theObjectEntries[i].in() );

  myParents.insert( "0:1:21" );
  myParents.insert( "0:1:17" );
  myParents.insert( "0:1:20" );
  myParents.insert( "0:1:13" );
  myParents.insert( "0:1:12" );

  mySubObjects.insert( "0:1:24" );
  mySubObjects.insert( "0:1:28" );
  mySubObjects.insert( "0:1:29" );
  mySubObjects.insert( "0:1:34" );

  myOthers.insert( "0:1:35" );
  myOthers.insert( "0:1:36" );
  myOthers.insert( "0:1:37" );
  myOthers.insert( "0:1:38" );
  myOthers.insert( "0:1:39" );

  std::set<std::string>::iterator it;
  for ( it=myParents.begin(); it!=myParents.end(); ++it)
    std::cout << ' ' << *it;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myVisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) );
  myInvisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  int studyId = GeometryGUI::ClientStudyToStudy( study->studyDS())->StudyId();

  myDisplayer = GEOM_Displayer( study );

 // setModal( true );
  setWindowTitle( tr( "GEOM_REDUCE_STUDY_TITLE" ) );
  setSizeGripEnabled( TRUE );

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  QGroupBox* groupKeptObjects = new QGroupBox( tr( "GEOM_REDUCE_STUDY_KEPT_OBJECTS" ) );

  if(true) {
  myTreeKeptObjects = new QTreeWidget();
  myTreeKeptObjects->setColumnCount( 2 );
  myTreeKeptObjects->setAllColumnsShowFocus(true);
  QStringList columnNames;
  columnNames.append(tr( "GEOM_REDUCE_STUDY_NAME" ));
  columnNames.append("");
  myTreeKeptObjects->setHeaderLabels( columnNames );
  QTreeWidgetItem * headerItem = new QTreeWidgetItem( columnNames );
  myTreeKeptObjects->setHeaderItem ( headerItem );
  myTreeKeptObjects->header()->moveSection( 1, 0 );
  myTreeKeptObjects->header()->setClickable( true );
  myTreeKeptObjects->header()->setMovable( false );
  myTreeKeptObjects->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
  myTreeKeptObjects->setSelectionMode( QAbstractItemView::ExtendedSelection );

  }

//  myTreeWidget->header()->setSortIndicatorShown( true );
//  myTreeWidget->header()->moveSection( 2, 0 );
//  myTreeWidget->setSortingEnabled( true );
//  myTreeWidget->setColumnWidth( 2, VISIBILITY_COLUMN_WIDTH );

  QLayout* layoutKeptObjects = new QGridLayout( groupKeptObjects );
  layoutKeptObjects->addWidget( myTreeKeptObjects );

  QGroupBox* groupRemoveObjects = new QGroupBox( tr( "GEOM_REDUCE_STUDY_REMOVE_OBJECTS" ) );

  if(true) {
  myTreeRemoveObjects = new QTreeWidget();
  myTreeRemoveObjects->setColumnCount( 2 );
  myTreeRemoveObjects->setAllColumnsShowFocus(true);
  QStringList columnNames;
  columnNames.append(tr( "GEOM_REDUCE_STUDY_NAME" ));
  columnNames.append("");
  myTreeRemoveObjects->setHeaderLabels( columnNames );
  QTreeWidgetItem* headerItem = new QTreeWidgetItem( columnNames );
  myTreeRemoveObjects->setHeaderItem ( headerItem );
  myTreeRemoveObjects->header()->moveSection( 1, 0 );
  myTreeRemoveObjects->header()->setClickable( true );
  myTreeRemoveObjects->header()->setMovable( false );
  myTreeRemoveObjects->header()->setResizeMode( 1, QHeaderView::ResizeToContents );
  myTreeRemoveObjects->setSelectionMode( QAbstractItemView::ExtendedSelection );
  }

  QGridLayout* layoutRemoveObjects = new QGridLayout( groupRemoveObjects );
  layoutRemoveObjects->addWidget( myTreeRemoveObjects );

  QGroupBox* groupOptions = new QGroupBox( tr( "GEOM_REDUCE_STUDY_OPTIONS" ) );

  QGridLayout* layoutOptions = new QGridLayout( groupOptions );

  myCBUnpublishIntermediate = new QCheckBox( tr( "GEOM_REDUCE_STUDY_UNPUB_INTERMEDIATE" ) );
  myCBRemoveIntermediate = new QCheckBox( tr( "GEOM_REDUCE_STUDY_REMOVE_INTERMEDIATE" ) );
  myCBKeepSubObjects = new QCheckBox( tr( "GEOM_REDUCE_STUDY_KEEP_SUB_OBJECTS" ) );
  myCBRemoveEmptyFolder = new QCheckBox( tr( "GEOM_REDUCE_STUDY_REMOVE_EMPTY_FOLDER" ) );
  myCBRemoveEmptyFolder->setChecked( true );
  myCBSoftRemoval = new QCheckBox( tr( "GEOM_REDUCE_STUDY_SOFT_REMOVAL" ) );

  layoutOptions->addWidget( myCBUnpublishIntermediate, 0, 0 );
  layoutOptions->addWidget( myCBRemoveIntermediate, 0, 1 );
  layoutOptions->addWidget( myCBKeepSubObjects, 1, 0, 1, 2 );
  layoutOptions->addWidget( myCBRemoveEmptyFolder, 2, 0, 1, 2 );
  layoutOptions->addWidget( myCBSoftRemoval, 3, 0, 1, 2 );

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

//  connect( myCBUnpublishIntermediate, SIGNAL( toggled( bool ) ), this, SLOT( onUnpublishIntermediate( bool )  ) );
//  connect( myCBRemoveIntermediate, SIGNAL( toggled ( bool ) ), this, SLOT( onRemoveIntermediate( bool ) ) );
//  connect( myCBKeepSubObjects, SIGNAL( toggled ( bool ) ), this, SLOT( onKeepSubObjects( bool ) ) );
//  connect( myCBRemoveEmptyFolder, SIGNAL( toggled ( bool ) ), this, SLOT( onRemoveEmptyFolder( bool ) ) );
//  connect( myCBSoftRemoval, SIGNAL( toggled ( bool ) ), this, SLOT( onSoftRemoval( bool ) ) );

  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( clickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( clickOnCancel() ) );
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( clickOnHelp() ) );

  connect( myTreeKeptObjects, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)) );
  connect( myTreeRemoveObjects, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)) );

  connect( myTreeKeptObjects->header(), SIGNAL( sectionClicked ( int ) ), this, SLOT( onHeaderClicked( int ) ) );
  connect( myTreeRemoveObjects->header(), SIGNAL( sectionClicked ( int ) ), this, SLOT( onHeaderClicked( int ) ) );

  connect( myCBKeepSubObjects, SIGNAL( toggled(bool)), this, SLOT( update() ) );
  connect( myCBRemoveIntermediate, SIGNAL( toggled(bool)), this, SLOT( update() ) );
  connect( myCBUnpublishIntermediate, SIGNAL( toggled(bool) ), this, SLOT( update() ) );

  update();

  checkVisibleIcon( myTreeKeptObjects );
  checkVisibleIcon( myTreeRemoveObjects );


}

GEOMToolsGUI_ReduceStudyDlg::~GEOMToolsGUI_ReduceStudyDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void GEOMToolsGUI_ReduceStudyDlg::update()
{
  myTreeKeptObjects->clear();
  myTreeRemoveObjects->clear();

  std::set<std::string> keptObjects(myMainEntries);
  std::set<std::string> removeObjects(myOthers);

  if( myCBUnpublishIntermediate->isChecked() || myCBRemoveIntermediate->isChecked() ) {
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      removeObjects.insert( *iter );
  }
  else {
    std::set<std::string>::iterator iter;
    for( iter=myParents.begin(); iter!=myParents.end(); ++iter)
      keptObjects.insert( *iter );
  }

  if( myCBKeepSubObjects->isChecked() ) {
    std::set<std::string>::iterator iter;
    for( iter=mySubObjects.begin(); iter!=mySubObjects.end(); ++iter)
      keptObjects.insert( *iter );
  }
  else {
    std::set<std::string>::iterator iter;
    for( iter=mySubObjects.begin(); iter!=mySubObjects.end(); ++iter)
      removeObjects.insert( *iter );
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

  myTreeKeptObjects->update();
  myTreeKeptObjects->collapseAll();
  myTreeRemoveObjects->update();
  myTreeRemoveObjects->collapseAll();

}

void GEOMToolsGUI_ReduceStudyDlg::onUnpublishIntermediate( bool )
{

}
void GEOMToolsGUI_ReduceStudyDlg::onRemoveIntermediate( bool theIsRemove )
{
  myCBUnpublishIntermediate->setEnabled( !theIsRemove );
}
void GEOMToolsGUI_ReduceStudyDlg::onKeepSubObjects( bool )
{

}
void GEOMToolsGUI_ReduceStudyDlg::onRemoveEmptyFolder( bool )
{

}
void GEOMToolsGUI_ReduceStudyDlg::onSoftRemoval( bool )
{

}

void GEOMToolsGUI_ReduceStudyDlg::clickOnOk()
{
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

void GEOMToolsGUI_ReduceStudyDlg::clickOnCancel()
{
  accept();
}
void GEOMToolsGUI_ReduceStudyDlg::clickOnHelp()
{

}

void GEOMToolsGUI_ReduceStudyDlg::onSelectionChanged()
{
  onShowOnlySelected();
}

void GEOMToolsGUI_ReduceStudyDlg::onShowOnlySelected()
{
//  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
//  if ( !app ) return;
//
//  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
//  GEOM_Displayer* disp = new GEOM_Displayer( appStudy );
//
//  OCCViewer_ViewManager* anOCCVM = (OCCViewer_ViewManager*) app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/ true );
//
//  std::cout << "\n\n\n onShowOnlySelected = " << std::endl;
//  if ( SUIT_ViewModel* viewModel = anOCCVM->getViewModel() )
//    if ( SALOME_View* viewFrame = dynamic_cast<SALOME_View*>( viewModel ) ) {
//      disp->EraseAll( true, false, viewFrame );
//
//      std::cout << "\n\n\n EraseAll = " << std::endl;
//
//      QList<QTreeWidgetItem*> selected = myTreeKeptObjects->selectedItems();
//      for( int i = 0; i< selected.size(); i++ ) {
//          const char* entry = myMap[selected[i]].first.c_str();
//          std::cout << "\n\n\n entry = " << QString( entry ).toStdString() << std::endl;
//          const char* name = "TEMP_IO";
//
//          Handle(SALOME_InteractiveObject) tmpIO =
//            new SALOME_InteractiveObject( entry, "GEOM", name );
//
//          disp->Display( tmpIO, false, viewFrame );
//      }
//      viewFrame->Repaint();
//  }
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
      if( item->flags() & Qt::ItemIsSelectable ) {
      const char* entry = item->getStudyEntry();
      if( item->isVisible() ) {
        item->setVisible( false, myInvisible );
        myDisplayer.Erase( new SALOME_InteractiveObject( entry, "GEOM", "TEMP_IO" ) );
      }
      }
      ++it;

    }
  }
  else {
	  myMapTreeSelectAll[ treeData ] = true;
	  treeData->headerItem()->setIcon( 1, myVisible );
    QTreeWidgetItemIterator it(treeData);
    while(*it) {
    	GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
    	if( item->flags() & Qt::ItemIsSelectable ) {
      const char* entry = item->getStudyEntry();
      if( !item->isVisible() ) {
    	  item->setVisible( true, myVisible );
        myDisplayer.Display( new SALOME_InteractiveObject( entry, "GEOM", "TEMP_IO" ) );
      }
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
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  int studyId = GeometryGUI::ClientStudyToStudy( study->studyDS())->StudyId();

  std::set<std::string>::iterator it;
  for( it = theObjects.begin(); it != theObjects.end(); ++it ) {
    std::string objectEntry = *it;
    GEOM::GEOM_BaseObject_var GeomBaseObject = GeometryGUI::GetGeomGen()->GetObject( studyId, objectEntry.c_str() );
    GEOM::GEOM_Object_var GeomObject = GEOM::GEOM_Object::_narrow( GeomBaseObject );
    if( GeomObject->_is_nil() )
      continue;
    addSubObject( theWidget, theObjects, GeomObject );
  }
}

GEOMToolsGUI_TreeWidgetItem* GEOMToolsGUI_ReduceStudyDlg::addSubObject( QTreeWidget* theWidget, std::set<std::string>& theObjects, GEOM::GEOM_Object_var theObject )
{
  std::cout<<"\n\n ----------------- addSubObject ---------------------" << std::endl;
  std::cout <<"\nObject = " << theObject->GetName() << std::endl;
  GEOMToolsGUI_TreeWidgetItem* item;
  if( !theObject->IsMainShape() ) {
    GEOMToolsGUI_TreeWidgetItem* parentItem = addSubObject( theWidget, theObjects, theObject->GetMainShape() );
    std::cout <<"\nParentItem = " << parentItem->text(0).toStdString() << std::endl;
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( parentItem, QStringList() << theObject->GetName(), theObject->GetStudyEntry(), false );
  }
  else {
    std::cout <<"\nParentItem = ROOT" << std::endl;
    item = findObjectInTree( theWidget, theObject );
    if( !item )
      item = new GEOMToolsGUI_TreeWidgetItem( theWidget, QStringList() << theObject->GetName(), theObject->GetStudyEntry(), false );
  }

  bool isDisplayed = false;
  if( theObjects.find( theObject->GetEntry() ) != theObjects.end() ) {
    std::cout << "\n FOUND " << std::endl;
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
    std::cout << "\n NOT FOUND " << std::endl;
    item->setFlags( item->flags() & ~Qt::ItemIsSelectable );
    item->setTextColor( 0, QColor( 150, 150, 150 ) );
  }

  std::cout<<"\n\n ----------------- FINISH addSubObject ---------------------" << std::endl;
  return item;
}

GEOMToolsGUI_TreeWidgetItem* GEOMToolsGUI_ReduceStudyDlg::findObjectInTree( QTreeWidget* theWidget, GEOM::GEOM_Object_var theObject )
{
  QTreeWidgetItemIterator it( theWidget );
  std::cout <<"\n\n\n theObject->GetStudyEntry() = " << theObject->GetStudyEntry() << std::endl;
  while(*it) {
    GEOMToolsGUI_TreeWidgetItem* item = dynamic_cast<GEOMToolsGUI_TreeWidgetItem*>(*it);
    std::cout <<"\n\n\n item->getStudyEntry() = " << item->getStudyEntry() << std::endl;
    if( QString( item->getStudyEntry() ) == QString( theObject->GetStudyEntry() ) )
      return item;
    ++it;
  }
  std::cout <<"\n\n\n RETURN NULL " << std::endl;
  return NULL;
}
