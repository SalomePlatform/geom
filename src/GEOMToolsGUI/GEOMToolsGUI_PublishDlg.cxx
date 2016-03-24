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
// File   : GEOMToolsGUI_PublishDlg.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.
//

#include "GEOMToolsGUI_PublishDlg.h"
#include "GEOM_Displayer.h"
#include "GeometryGUI.h"

//SALOME GUI includes
#include <SUIT_Session.h>
#include <SUIT_DataObject.h>
#include <SUIT_ResourceMgr.h>

#include <CAM_DataModel.h>

#include <SalomeApp_DataObject.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Study.h>


//QT includes
#include <QHeaderView>
#include <QGridLayout>
#include <QTreeWidget>
#include <QPushButton>

#define  DEFAULT_MARGIN 11
#define  DEFAULT_SPACING 6

#define VISIBILITY_COLUMN_WIDTH 25

GEOMToolsGUI_PublishDlg::GEOMToolsGUI_PublishDlg(QWidget* parent):
  QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
   setObjectName( "GEOMToolsGUI_PublishDlg" );
   setModal( false );
   setWindowTitle( tr( "GEOM_PUBLISH_OBJECTS_TLT" ) );

   setAttribute(Qt::WA_DeleteOnClose);
   
   //List of the objects
   myTreeWidget = new QTreeWidget(this);
   //myTreeWidget->setRootIsDecorated(false);
   //myTreeWidget->setUniformRowHeights(true);
   myTreeWidget->setAllColumnsShowFocus(true);
   myTreeWidget->setColumnCount(3);

   QStringList columnNames;
   columnNames.append(tr("OBJECT_NAME"));
   columnNames.append(tr("OBJECT_ENTRY"));
   columnNames.append("");
   QTreeWidgetItem * headerItem = new QTreeWidgetItem(columnNames);
   SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
   headerItem->setIcon( 2, QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) ) );
   myTreeWidget->setHeaderItem ( headerItem );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
   myTreeWidget->header()->setClickable( true );
#else
   myTreeWidget->header()->setSectionsClickable( true );
#endif
   myTreeWidget->header()->setSortIndicatorShown( true );
   myTreeWidget->header()->moveSection( 2, 0 );
   myTreeWidget->setSortingEnabled( true );
   myTreeWidget->setColumnWidth( 2, VISIBILITY_COLUMN_WIDTH );

   //Select All button
   QPushButton* selectAllButton = new  QPushButton( tr("SELECT_ALL"),this );

   //Unselect Select All button
   QPushButton* unselectAllButton = new  QPushButton( tr("UNSELECT_ALL"),this );

   //Publish button
   QPushButton* publishButton = new QPushButton( tr("GEOM_PUBLISH_BTN"),this );

   //Publish and Close button
   QPushButton* publishCloseButton = new QPushButton( tr("GEOM_PUBLISH_CLOSE_BTN"), this );
   
   //Close button
   QPushButton* closeButton = new QPushButton( tr("GEOM_BUT_CLOSE"), this );
   
   QGridLayout* gridLayout = new QGridLayout(this);
   gridLayout->setMargin(DEFAULT_MARGIN);
   gridLayout->setSpacing(DEFAULT_SPACING);

   gridLayout->addWidget(myTreeWidget, 0, 0, 3, 3);
   gridLayout->addWidget(selectAllButton, 0, 3, 1, 1);
   gridLayout->addWidget(unselectAllButton, 1, 3, 1, 1);
   gridLayout->addItem( new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 2, 3, 1, 1);
   gridLayout->addWidget(publishCloseButton, 3, 0, 1, 1); 
   gridLayout->addWidget(publishButton, 3, 1, 1, 1);
   gridLayout->addItem( new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 2, 1, 1);
   gridLayout->addWidget(closeButton, 3, 3, 1, 1);

   //Connect signals and slots
   connect( myTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)) );

   connect( selectAllButton, SIGNAL(clicked()), this, SLOT(clickOnSelectAll()) );
   connect( unselectAllButton, SIGNAL(clicked()), this, SLOT(clickOnUnSelectAll()) );
   
   connect(publishCloseButton, SIGNAL(clicked()), this, SLOT(clickOnOk()));
   connect(publishButton, SIGNAL(clicked()), this, SLOT(clickOnApply()));
   connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));

   initData();
}

//=================================================================================
// function : ~GEOMToolsGUI_PublishDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GEOMToolsGUI_PublishDlg::~GEOMToolsGUI_PublishDlg()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( appStudy ) {
      _PTR(Study) aStudy = appStudy->studyDS();
      if ( aStudy ) {
	GEOM_Displayer displayer ( appStudy );
	QTreeWidgetItemIterator it( myTreeWidget );
	while ( *it ) {
	  QString entry = myEntryToItem.key( *it );
	  _PTR(SObject) SO ( aStudy->FindObjectID( qPrintable( entry ) ) );
	  if ( SO ) {
	    GEOM::GEOM_Object_var aGeomObject = GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject( SO ) );
	    if ( CORBA::is_nil( aGeomObject ) ) continue;
	    if ( displayer.IsDisplayed( aGeomObject->GetStudyEntry() ) ) {
	      Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( aGeomObject->GetStudyEntry(), "GEOM", "" );
	      displayer.Erase( io );
	    }
	  }
	  ++it;
	}
	displayer.UpdateViewer();
      }
    }
  }
}

//=================================================================================
// function : initData()
// purpose  : Fill dialog after opening
//=================================================================================
void GEOMToolsGUI_PublishDlg::initData() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if(!app)
    return;
  
  SalomeApp_Module* module = dynamic_cast<SalomeApp_Module*>(app->activeModule());
  
  if(!module)
    return;

  CAM_DataModel* dataModel = module->dataModel();
  if(!dataModel)
    return;
  
  myGeomRoot = dynamic_cast<SalomeApp_DataObject*>(dataModel->root());

  if(!myGeomRoot)
    return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if(!appStudy ) 
    return;

  _PTR(Study) aStudy = appStudy->studyDS();
  if(!aStudy)
    return;

  buildTree(aStudy, myGeomRoot);

  myTreeWidget->resizeColumnToContents(0);
  myTreeWidget->resizeColumnToContents(1);
  myTreeWidget->sortByColumn(0, Qt::AscendingOrder);
}

//=================================================================================
// function : createItem()
// purpose  :
//=================================================================================
QTreeWidgetItem* GEOMToolsGUI_PublishDlg::createItem(QTreeWidgetItem* theParent, Pair theAttributes, bool isCheckable) {
  QStringList aList;
  aList<<theAttributes.second<<theAttributes.first<<"";
  QTreeWidgetItem* anItem = new QTreeWidgetItem(aList);
  
  if(isCheckable)
    anItem->setCheckState(0, Qt::Unchecked);
  else {
    anItem->setFlags(anItem->flags() & ~Qt::ItemIsUserCheckable);
  }
  
  if(theParent)
    theParent->addChild(anItem);
  else 
    myTreeWidget->addTopLevelItem(anItem);

  myEntryToItem.insert(theAttributes.first, anItem);
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  anItem->setIcon( 2, QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) ) );
  anItem->setExpanded(true);
  return anItem;
}

//=================================================================================
// function : findParentItem()
// purpose  :
//=================================================================================
QTreeWidgetItem* GEOMToolsGUI_PublishDlg::findParentItem(_PTR(Study) theStudy, SalomeApp_DataObject* theObject, BufferedList& theList ) {
  
  QTreeWidgetItem* aResult = NULL;
  SalomeApp_DataObject* aParrent = dynamic_cast<SalomeApp_DataObject*>(theObject->parent());
  if(aParrent) {
    QString targetEntry = aParrent->entry();
    if( !(aResult = myEntryToItem.value(targetEntry)) ) {
      if( aParrent != myGeomRoot ) {
        QString aName;
        _PTR(SObject) aSO ( theStudy->FindObjectID(qPrintable(aParrent->entry())));
        _PTR(GenericAttribute) anAttr;
        if ( aSO->FindAttribute(anAttr, "AttributeName") ) {
          _PTR(AttributeName) anAttrName (anAttr);
          aName = anAttrName->Value().c_str();
        }
        theList.push_front(qMakePair(targetEntry, aName));
        aResult = findParentItem(theStudy,aParrent,theList);
      } else {
        //Publish List
        for(int i = 0; i < theList.size(); i++ ) {
          aResult = createItem(aResult, theList[i], false);
        }
        theList.clear();
      }
    } else {
      //Publish List
      for(int i = 0; i < theList.size(); i++ ) {
        aResult = createItem(aResult, theList[i], false);
      }
      theList.clear();
    }
  }
  return aResult;
}

//=================================================================================
// function : buildTree()
// purpose  :
//=================================================================================
void GEOMToolsGUI_PublishDlg::buildTree(_PTR(Study) theStudy, SalomeApp_DataObject* theItem) {
  if(!theItem || theItem->isReference())
    return;
  
  if(theItem != myGeomRoot) {

    //If object hasn't "AttributeDrawable" => it visible
    bool isDrawable = true;
    _PTR(SObject) SO ( theStudy->FindObjectID(qPrintable(theItem->entry())));
    _PTR(GenericAttribute) anAttr;
    if ( SO && SO->FindAttribute(anAttr, "AttributeDrawable") ) {
      _PTR(AttributeDrawable) aDrw (anAttr);
      isDrawable = aDrw->IsDrawable();
    }
    
    if(!isDrawable) {
      QString aName;
      if ( SO->FindAttribute(anAttr, "AttributeName") ) {
        _PTR(AttributeName) aAttrName (anAttr);
        aName = aAttrName->Value().c_str();
      }
      BufferedList aList;
      QTreeWidgetItem* parentItem = findParentItem(theStudy, theItem, aList);
      createItem(parentItem,qMakePair(theItem->entry(),aName),true);
    }
  }
  
  DataObjectList listObj = theItem->children( false );
  DataObjectList::iterator itr = listObj.begin();
  while( itr != listObj.end()) {
    buildTree(theStudy, dynamic_cast<SalomeApp_DataObject*>(*itr));
    itr++;
  }
}

//=================================================================================
// function : clickOnApply()
// purpose  : Called then "Publish" button clicked
//=================================================================================
void GEOMToolsGUI_PublishDlg::clickOnApply() {
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if(!app)
    return;
  
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if(!appStudy ) 
    return;

  _PTR(Study) aStudy = appStudy->studyDS();
  if(!aStudy)
    return;


  QList<QTreeWidgetItem*> toProcess;
  getDrawableList(myTreeWidget->invisibleRootItem(), toProcess);

  _PTR(StudyBuilder) aBuilder = aStudy->NewBuilder();
  for( int i = 0; i < toProcess.count(); i++) {
    QTreeWidgetItem* item = toProcess[i];
    if(item) {
      QString entry = myEntryToItem.key(item);
      _PTR(SObject) SO ( aStudy->FindObjectID(qPrintable(entry)) );
      if(!SO) continue;
      _PTR(AttributeDrawable) aDrw = aBuilder->FindOrCreateAttribute( SO, "AttributeDrawable" );
      aDrw->SetDrawable( true );
      //Remove or change item
      if( item != myTreeWidget->invisibleRootItem() ){
        if( item->data(0,Qt::UserRole).toBool() ) {
          delete item;
        } else {
          item->setFlags(item->flags() & ~Qt::ItemIsUserCheckable);
          item->setData(0,Qt::CheckStateRole,QVariant());
        }
      }
      // show references if any
      std::vector< _PTR(SObject) > vso = aStudy->FindDependances(SO);
      for ( int i = 0; i < vso.size(); i++ ) {
	_PTR(SObject) refObj = vso[i];
	aDrw = aBuilder->FindOrCreateAttribute( refObj, "AttributeDrawable" );
	aDrw->SetDrawable( true );
      }
    }
  }
  toProcess.clear();
  getTails(myTreeWidget->invisibleRootItem(), toProcess);
  for( int i = 0; i < toProcess.count(); i++) {
    if( toProcess[i] != myTreeWidget->invisibleRootItem() )
      delete toProcess[i];
  }
      
  app->updateObjectBrowser( false );
  app->updateActions();
}

//=================================================================================
// function : clickOnOk()
// purpose  : Called then "Publish And Close" button clicked
//=================================================================================
void GEOMToolsGUI_PublishDlg::clickOnOk() {
  clickOnApply();
  accept();
}

//=================================================================================
// function : clickOnSelectAll()
// purpose  : Called then "Select All" button clicked
//=================================================================================
void GEOMToolsGUI_PublishDlg::clickOnSelectAll() {
  processAll(myTreeWidget->invisibleRootItem(),Qt::Checked);
}

//=================================================================================
// function : clickOnUnSelectAll()
// purpose  : Called then "Unselect All" button clicked
//=================================================================================
void GEOMToolsGUI_PublishDlg::clickOnUnSelectAll() {
  processAll(myTreeWidget->invisibleRootItem(),Qt::Unchecked);
}

//=================================================================================
// function : processAll()
// purpose  : 
//=================================================================================
void GEOMToolsGUI_PublishDlg::processAll(QTreeWidgetItem* theItem, Qt::CheckState state) {
  
    
  if((theItem->flags() & Qt::ItemIsUserCheckable))
    theItem->setCheckState(0,state);
  
  int i = 0;
  while(i < theItem->childCount()) {
    processAll(theItem->child(i), state);
    i++;
  }
}

//=================================================================================
// function : processAll()
// purpose  : 
//=================================================================================
void GEOMToolsGUI_PublishDlg::getDrawableList(QTreeWidgetItem* theItem, QList<QTreeWidgetItem*>& theList) {
  
  theItem->setData(0, Qt::UserRole, true);

  if((theItem->flags() & Qt::ItemIsUserCheckable) ) {
    theItem->setData(0, Qt::UserRole,theItem->checkState(0) == Qt::Checked);
  }
  
  int i = 0;
  while(i < theItem->childCount()) {
    getDrawableList(theItem->child(i), theList);
    theItem->setData( 0, Qt::UserRole, (theItem->data(0,Qt::UserRole).toBool() && theItem->child(i)->data(0,Qt::UserRole).toBool()) );
    i++;
  } 

  if( (theItem->flags() & Qt::ItemIsUserCheckable) && (theItem->checkState(0) == Qt::Checked))
    theList.push_back(theItem);
}


void GEOMToolsGUI_PublishDlg::getTails(QTreeWidgetItem* theItem, QList<QTreeWidgetItem*>& theList) {
  int i = 0;
  while(i < theItem->childCount()) {
    getTails(theItem->child(i),theList);
    i++;
  }

  if(theItem->data(0,Qt::UserRole).toBool())
    theList.push_back(theItem);
}

//=================================================================================
// function : onItemClicked()
// purpose  : Called then treeItem clicked
//=================================================================================
void GEOMToolsGUI_PublishDlg::onItemClicked(QTreeWidgetItem* theItem, int theColumn) {
  if ( theColumn == 2 ) {
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    if ( !app ) return;
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    if ( !appStudy ) return;
    _PTR(Study) aStudy = appStudy->studyDS();
    if ( !aStudy ) return;
    QString entry = myEntryToItem.key( theItem );
    _PTR(SObject) SO ( aStudy->FindObjectID( qPrintable( entry ) ) );
    if ( !SO ) return;
    GEOM::GEOM_Object_var aGeomObject = GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject( SO ) );
    if ( CORBA::is_nil( aGeomObject ) ) return;
    GEOM_Displayer displayer ( appStudy );
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( aGeomObject->GetStudyEntry(), "GEOM", "" );
    if ( displayer.IsDisplayed( aGeomObject->GetStudyEntry() ) ) {
      theItem->setIcon( 2, QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) ) );
      displayer.Erase( io );
    }
    else {
      theItem->setIcon( 2, QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) ) );
      displayer.Display( io );
    }
    displayer.UpdateViewer();
  }
}

