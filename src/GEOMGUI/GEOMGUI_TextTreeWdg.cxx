// Copyright (C) 2015-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File      : GEOMGUI_TextTreeWdg.cxx
// Author    : Alexander KOVALEV (akl)

#include "GEOMGUI_TextTreeWdg.h"

#include "GEOMGUI_DimensionProperty.h"
#include "GEOMGUI_AnnotationAttrs.h"
#include "GEOMGUI_AnnotationMgr.h"
#include "GeometryGUI.h"
#include "GeometryGUI_Operations.h"
#include <GEOM_Constants.h>
#include <GEOMUtils.hxx>

// GUI includes
#include <LightApp_Module.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataBrowser.h>

// Qt includes
#include <QAction>
#include <QMenu>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHash>

// ----------------------------------------------------------------------------
// Common style interface for managing dimension and annotation properties
// ----------------------------------------------------------------------------
namespace
{
  //! Access interface implementation for shape dimension attribute/property.
  class DimensionsProperty : public GEOMGUI_TextTreeWdg::VisualProperty
  {
  public:

    DimensionsProperty( const std::string& theEntry ) :
      myEntry( theEntry ) {
      myAttr.LoadFromAttribute( theEntry );
    }
    virtual int GetNumber() Standard_OVERRIDE {
      return myAttr.GetNumber();
    }
    virtual QString GetName( const int theIndex ) Standard_OVERRIDE {
      return myAttr.GetName( theIndex );
    }
    virtual bool GetIsVisible( const int theIndex ) Standard_OVERRIDE {
      return myAttr.IsVisible( theIndex );
    }
    virtual void SetIsVisible( const int theIndex, const bool theIsVisible ) Standard_OVERRIDE {
      myAttr.SetVisible( theIndex, theIsVisible );
    }
    virtual void Save() Standard_OVERRIDE {
      myAttr.SaveToAttribute( myEntry );
    }
    GEOMGUI_DimensionProperty& Attr() { return myAttr; }

  private:
    GEOMGUI_DimensionProperty myAttr;
    std::string myEntry;
  };

  //! Access interface implementation for shape annotation attribute.
  class AnnotationsProperty : public GEOMGUI_TextTreeWdg::VisualProperty
  {
  public:

    AnnotationsProperty( const std::string& theEntry ) {
      myEntry = theEntry.c_str();
      _PTR(SObject) aSObj = SalomeApp_Application::getStudy()->FindObjectID( theEntry );
      if ( aSObj ) {
        myAttr = GEOMGUI_AnnotationAttrs::FindAttributes( aSObj );
      }
    }
    virtual int GetNumber() Standard_OVERRIDE {
      return !myAttr.IsNull() ? myAttr->GetNbAnnotation() : 0;
    }
    virtual QString GetName( const int theIndex ) Standard_OVERRIDE {
      return !myAttr.IsNull() ? myAttr->GetText( theIndex ) : QString();
    }
    virtual bool GetIsVisible( const int theIndex ) Standard_OVERRIDE {
      GEOMGUI_AnnotationMgr* aMgr = annotationMgr();
      if (!aMgr) {
        return false;
      }
      return aMgr->IsDisplayed( myEntry, theIndex );
    }
    virtual void SetIsVisible( const int theIndex, const bool theIsVisible ) Standard_OVERRIDE {
      GEOMGUI_AnnotationMgr* aMgr = annotationMgr();
      if (!aMgr) {
        return;
      }
      if (theIsVisible)
        annotationMgr()->Display(myEntry, theIndex);
      else
        annotationMgr()->Erase(myEntry, theIndex);
    }
    virtual void Save() Standard_OVERRIDE {
      /* every change is automatically saved */
    }
    Handle(GEOMGUI_AnnotationAttrs) Attr() { return myAttr; }

protected:
    GEOMGUI_AnnotationMgr* annotationMgr() const
    {
      CAM_Application* anApp = dynamic_cast<CAM_Application*>(SUIT_Session::session()->activeApplication());
      GeometryGUI* aModule = dynamic_cast<GeometryGUI*>(anApp->activeModule());
      if (!aModule) {
        return NULL;
      }
      return aModule->GetAnnotationMgr();
    }

private:
    QString myEntry;
    Handle(GEOMGUI_AnnotationAttrs) myAttr;
  };
}

// ----------------------------------------------------------------------------
// Text tree widget implementation
// ----------------------------------------------------------------------------

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
GEOMGUI_TextTreeWdg::GEOMGUI_TextTreeWdg( SalomeApp_Application* app )
{
  myStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  myDisplayer = GEOM_Displayer();

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  myVisibleIcon = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) );
  myInvisibleIcon = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) );

  setWindowTitle( tr( "TEXT_TREE_VIEW_TITLE" ) );
  setObjectName( "geomTextTreeWdg" );

  setRootIsDecorated( true );
  setSelectionMode( QAbstractItemView::ExtendedSelection );
  setAllColumnsShowFocus( true );
  setUniformRowHeights( true );

  QStringList columnNames;
  columnNames << tr("TEXT_TREE_VIEW_NAME") << "";
  QTreeWidgetItem * headerItem = new QTreeWidgetItem( columnNames );
  headerItem->setIcon( 1, myVisibleIcon );
  setHeaderItem ( headerItem ); 
  header()->moveSection( 1, 0 );
  header()->setSectionResizeMode( 1, QHeaderView::ResizeToContents );

  QStringList rootNames;
  rootNames << tr("GEOM_DIMENSIONS") << "";
  myDimensionsItem = new QTreeWidgetItem( this, rootNames );
  myDimensionsItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  addTopLevelItem( myDimensionsItem );

  rootNames.clear();
  rootNames << tr("GEOM_ANNOTATIONS") << "";
  myAnnotationsItem = new QTreeWidgetItem( this, rootNames );
  myAnnotationsItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
  addTopLevelItem( myAnnotationsItem );

  // get a free dockable window id
  myWindowID = 11;
  while( app->dockWindow( myWindowID ))
    ++myWindowID;
  ++myWindowID;

  createActions();
  setContextMenuPolicy( Qt::CustomContextMenu );
  connect( this, SIGNAL( customContextMenuRequested(const QPoint&) ),
           this, SLOT( showContextMenu(const QPoint&) ) );

  connect( app->objectBrowser(), SIGNAL( updated() ), this, SLOT( updateTree() ) );

  GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
  connect( aGeomGUI, SIGNAL( DimensionsUpdated( const QString& ) ),
           this, SLOT( updateDimensionBranch( const QString& ) ) );
  connect( aGeomGUI, SIGNAL( SignalAnnotationsUpdated( const QString& ) ),
           this, SLOT( updateAnnotationBranch( const QString& ) ) );
  connect( aGeomGUI, SIGNAL( SignalTextTreeRenameObject( const QString& ) ),
           this, SLOT( updateObjectName( const QString& ) ) );

  connect( this, SIGNAL( itemClicked( QTreeWidgetItem*, int) ), 
           this, SLOT( onItemClicked( QTreeWidgetItem*, int ) ) );
  connect( myStudy, SIGNAL( objVisibilityChanged( QString, Qtx::VisibilityState ) ),
         this, SLOT( onUpdateVisibilityColumn( QString, Qtx::VisibilityState ) ) );
}

//=================================================================================
// function : Destructor
// purpose  :
//=================================================================================
GEOMGUI_TextTreeWdg::~GEOMGUI_TextTreeWdg()
{
  //std::cout<<"~GEOMGUI_TextTreeWdg"<<std::endl;
}

//=================================================================================
// function : createActions
// purpose  : Create context popup menu actions.
//=================================================================================
void GEOMGUI_TextTreeWdg::createActions()
{
  QAction* a = new QAction( tr( "MEN_DISPLAY" ), this );
  a->setIcon( myVisibleIcon );
  myActions.insert( GEOMOp::OpShow, a );
  
  QAction* b = new QAction( tr( "MEN_ERASE" ), this );
  b->setIcon( myInvisibleIcon );
  myActions.insert( GEOMOp::OpHide, b );
}

//=================================================================================
// function : updateTree
// purpose  : Rebuild branches of objects and remove objects if there are no more annotations
//            for it
//=================================================================================
void GEOMGUI_TextTreeWdg::updateTree()
{
  myStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  _PTR(Study) aDSStudy = myStudy->studyDS();
  if ( aDSStudy ) {
    _PTR(SComponent) SC ( aDSStudy->FindComponent( "GEOM" ) );
    if ( SC ) {
      _PTR(ChildIterator) anIter ( aDSStudy->NewChildIterator( SC ) );
      anIter->InitEx( true );
      QList<QString> aDimensionObjEntries = getObjects( DimensionShape ).keys();
      QList<QString> anAnnotationObjEntries = getObjects( AnnotationShape ).keys();
      while( anIter->More() ) {
        _PTR(SObject) valSO ( anIter->Value() );
        _PTR(SObject) refSO;
        if ( !valSO->ReferencedObject( refSO ) ) {
          // update tree of object's dimensions
          QString anEntry = valSO->GetID().c_str();
          updateBranches( anEntry );
          aDimensionObjEntries.removeAll( anEntry );
          anAnnotationObjEntries.removeAll( anEntry );
        }
        anIter->Next();
      }
      foreach ( QString entry, aDimensionObjEntries ) {
        removeBranch( DimensionShape, entry, true );
      }
      foreach ( QString entry, anAnnotationObjEntries ) {
        removeBranch( AnnotationShape, entry, true );
      }
    }
  }
}

//=================================================================================
// function : updateBranches
// purpose  : Rebuild branches for object of the given entry
//=================================================================================
void GEOMGUI_TextTreeWdg::updateBranches( const QString& theEntry )
{
  updateDimensionBranch( theEntry );
  updateAnnotationBranch( theEntry );
}

//=================================================================================
// function : updateDimensionBranch
// purpose  : Rebuild branch of dimension type for object of the given entry
//=================================================================================
void GEOMGUI_TextTreeWdg::updateDimensionBranch( const QString& theEntry )
{
  fillBranch( DimensionShape, theEntry );
}

//=================================================================================
// function : updateAnnotationBranch
// purpose  : Rebuild branch of annotation type for object of the given entry
//=================================================================================
void GEOMGUI_TextTreeWdg::updateAnnotationBranch( const QString& theEntry )
{
  fillBranch( AnnotationShape, theEntry );
}

//=================================================================================
// function : updateObjectName
// purpose  : Find name of the given object and set the name for corresponded tree item
//=================================================================================
void GEOMGUI_TextTreeWdg::updateObjectName( const QString& theEntry )
{
  QTreeWidgetItem* anObjectItem = 0;

  QHash<QString, QTreeWidgetItem*> anObjects = getObjects( DimensionShape );
  if ( anObjects.contains( theEntry ) )
    anObjectItem = anObjects.value( theEntry );
  else {
    anObjects = getObjects( AnnotationShape );
    if ( anObjects.contains( theEntry ) )
      anObjectItem = anObjects.value( theEntry );
  }
  if ( !anObjectItem )
    return;

  myStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
  if ( myStudy ) {
    _PTR(Study) aStudyDS = myStudy->studyDS();
    if ( aStudyDS ) {
      _PTR(SObject) anObject( aStudyDS->FindObjectID( theEntry.toStdString() ) );
      if ( anObject.get() )
        anObjectItem->setText( 0, anObject->GetName().c_str() );
    }
  }
}

//=================================================================================
// function : fillBranch
// purpose  :
//=================================================================================
void GEOMGUI_TextTreeWdg::fillBranch( const BranchType& theBranchType, const QString& theEntry )
{
  myStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );

  if ( myStudy && !theEntry.isEmpty() ) {
    QSharedPointer<VisualProperty> aProp = getVisualProperty( theBranchType, myStudy, theEntry.toStdString() );
    if ( !aProp ) {
      return;
    }

    _PTR(Study) aStudyDS = myStudy->studyDS();
    if ( aStudyDS ) {
      _PTR(SObject) obj( aStudyDS->FindObjectID( theEntry.toStdString() ) );
      const QString aName = obj->GetName().c_str();
      const int nbProps = aProp->GetNumber();

      QTreeWidgetItem* objectItem = itemFromEntry( theBranchType, theEntry );
      if ( objectItem ) {
        removeBranch( theBranchType, theEntry, nbProps > 0 ? false : true );
      }
      QStringList itemName;
      if ( nbProps > 0 ) {
        itemName << aName << "";
        if ( !objectItem ) {
          QTreeWidgetItem* aPropRootItem = getPropertyRootItem( theBranchType );

          objectItem = new QTreeWidgetItem( aPropRootItem, itemName );
          objectItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
          objectItem->setData( 1, Qt::UserRole, theEntry );
          aPropRootItem->addChild( objectItem );
          getObjects( theBranchType ).insert( theEntry, objectItem );
          if ( aPropRootItem->childCount() == 1 )
            aPropRootItem->setExpanded( true );
        }
        for ( int anIt = 0; anIt < nbProps; ++anIt ) {
          const QString aPropName  = aProp->GetName( anIt );
          const bool isVisible = aProp->GetIsVisible( anIt );
          QTreeWidgetItem* anItem = new QTreeWidgetItem;
          anItem->setText( 0, aPropName );
          anItem->setIcon( 1, isVisible ? myVisibleIcon : myInvisibleIcon );
          anItem->setData( 0, Qt::UserRole, anIt );
          anItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
          objectItem->addChild( anItem );
        }
      }
    }
  }
}

//=================================================================================
// function : getVisualProperty
// purpose  :
//=================================================================================
QSharedPointer<GEOMGUI_TextTreeWdg::VisualProperty>
  GEOMGUI_TextTreeWdg::getVisualProperty( const BranchType& theBranchType,
                                          SalomeApp_Study* /*theStudy*/,
                                          const std::string& theEntry )
{
  switch ( theBranchType )
  {
    case DimensionShape  : return QSharedPointer<VisualProperty>( new DimensionsProperty( theEntry ) );
    case AnnotationShape : return QSharedPointer<VisualProperty>( new AnnotationsProperty( theEntry ) );
    default: break;
  }
  return QSharedPointer<VisualProperty>();
}

//=================================================================================
// function : removeBranch
// purpose  :
//=================================================================================
void GEOMGUI_TextTreeWdg::removeBranch( const BranchType& theBranchType, const QString& theEntry,
                                        bool force )
{
  QTreeWidgetItem* objectItem = itemFromEntry( theBranchType, theEntry );
  if ( !objectItem )
    return;
  qDeleteAll( objectItem->takeChildren() );
  if ( force ) {
    QTreeWidgetItem* aPropRootItem = getPropertyRootItem( theBranchType );
    aPropRootItem->removeChild( objectItem );
    getObjects( theBranchType ).remove( theEntry );
  }
}

//=================================================================================
// function : onItemClicked()
// purpose  : called when tree item was clicked
//=================================================================================
void GEOMGUI_TextTreeWdg::onItemClicked( QTreeWidgetItem* theItem, int theColumn )
{
  if( theColumn != 1 || theItem->icon( 1 ).isNull() || theItem->isDisabled() )
    return;
  
  std::string anEntry = entryFromItem( theItem->parent() ).toStdString();
  BranchType aBranchType = branchTypeFromItem( theItem );

  int aDimIndex = idFromItem( theItem );

  QSharedPointer<VisualProperty> aProp = getVisualProperty( aBranchType, myStudy, anEntry );

  SalomeApp_Application* anApp =
    dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  GeometryGUI* aModule = dynamic_cast<GeometryGUI*>(anApp->activeModule());
  if ( aProp->GetIsVisible( aDimIndex ) ) {
    aModule->GetAnnotationMgr()->Erase(anEntry.c_str(), aDimIndex);
    theItem->setIcon( 1, myInvisibleIcon );
  } else {
    aModule->GetAnnotationMgr()->Display(anEntry.c_str(), aDimIndex);

    theItem->setIcon( 1, myVisibleIcon );
  }
  redisplay( anEntry.c_str() );
}

//=================================================================================
// function : idFromItem
// purpose  :
//=================================================================================
int GEOMGUI_TextTreeWdg::idFromItem( QTreeWidgetItem* theItem )
{
  if ( !theItem )
    return -1;

  bool isIdOK = false;
  const int anId = theItem->data( 0, Qt::UserRole ).toInt( &isIdOK );

  return isIdOK ? anId : -1;
}

//=================================================================================
// function : entryFromItem
// purpose  :
//=================================================================================
QString GEOMGUI_TextTreeWdg::entryFromItem( QTreeWidgetItem* theShapeItem ) const
{
  if ( !theShapeItem )
    return "";

  return theShapeItem->data( 1, Qt::UserRole ).toString();
}

//=================================================================================
// function : itemFromEntry
// purpose  :
//=================================================================================
QTreeWidgetItem* GEOMGUI_TextTreeWdg::itemFromEntry( const BranchType& theBranchType, QString theEntry )
{
  if ( theEntry.isEmpty() )
    return 0;

  return getObjects(theBranchType).value( theEntry, 0 );
}

//=================================================================================
// function : onUpdateVisibilityColumn
// purpose  : Update visible state of icons of entry items.
//=================================================================================
void GEOMGUI_TextTreeWdg::onUpdateVisibilityColumn( QString theEntry, Qtx::VisibilityState theState )
{
  // dimension property branch
  updateVisibilityColumn( DimensionShape, theEntry, theState );

  // annotation property branch
  updateVisibilityColumn( AnnotationShape, theEntry, theState );
}

//=================================================================================
// function : updateVisibilityColumn
// purpose  : Update icons of dimension items.
//=================================================================================
void GEOMGUI_TextTreeWdg::updateVisibilityColumn( const BranchType& theBranchType, QString theEntry,
                                                  Qtx::VisibilityState theState )
{
  QTreeWidgetItem* anItem = itemFromEntry( theBranchType, theEntry );
  if ( !anItem )
    return;
  anItem->setDisabled( theState != Qtx::ShownState );
  QTreeWidgetItem* aChildItem;

  QSharedPointer<VisualProperty> aProp = getVisualProperty( theBranchType, myStudy, theEntry.toStdString() );

  for ( int i=0; i < anItem->childCount(); i++ ) {
    aChildItem = anItem->child( i );
    if ( theState == Qtx::ShownState ) {
      if ( aProp->GetNumber() == 0 )
        continue;
      aChildItem->setIcon( 1, aProp->GetIsVisible( idFromItem( aChildItem ) ) ? myVisibleIcon
                                                                              : myInvisibleIcon );
      aChildItem->setDisabled( false );
    } else {
      aChildItem->setIcon( 1, QIcon() );
      aChildItem->setDisabled( true );
    }
  }
}

//=================================================================================
// function : showContextMenu
// purpose  : 
//=================================================================================
void GEOMGUI_TextTreeWdg::showContextMenu( const QPoint& pos )
{
  CAM_Application* anApp = dynamic_cast<CAM_Application*>(myStudy->application());
  GeometryGUI* aModule = dynamic_cast<GeometryGUI*>(anApp->activeModule());

  QMenu aMenu;
  if ( !selectedItems().isEmpty() && selectedItems().count() == 1 ) {
    QTreeWidgetItem* anItem = selectedItems().first();
    QString anEntry = entryFromItem( anItem->parent() );
    if ( !anEntry.isEmpty() ) {
      BranchType aBranchType = branchTypeFromItem( anItem );
      QSharedPointer<VisualProperty>
        aProp = getVisualProperty( aBranchType, myStudy, anEntry.toStdString() );

      if ( aProp->GetNumber() == 0 )
        return;
      // Edit annotation action
      QAction* anEditAction = aModule->action(GEOMOp::OpEditAnnotation);
      if ( anEditAction )
        aMenu.addAction( anEditAction );
      QAction* aDeleteAction = aModule->action(GEOMOp::OpDeleteAnnotation);
      if ( aDeleteAction )
        aMenu.addAction( aDeleteAction );
      // Show/Hide actions
      if ( aProp->GetIsVisible( idFromItem( anItem ) ) )
        aMenu.addAction( myActions[GEOMOp::OpHide] );
      else
        aMenu.addAction( myActions[GEOMOp::OpShow] );
    }
  }

  if (selectedItems().isEmpty() && currentItem()) {
    QTreeWidgetItem* anItem = currentItem();
    bool aShowAll = false;
    if (anItem == getPropertyRootItem(AnnotationShape))
      aShowAll = true;
    else {
      QHash<QString, QTreeWidgetItem*> anObjects = getObjects( AnnotationShape );
      QHash<QString, QTreeWidgetItem*>::const_iterator anIt = anObjects.begin(),
                                                       aLast = anObjects.end();
      for (; anIt != aLast && !aShowAll; anIt++) {
        aShowAll = anIt.value() == anItem;
      }
    }
    if (aShowAll) {
      aMenu.addAction( aModule->action(GEOMOp::OpShowAllAnnotations) );
      aMenu.addAction( aModule->action(GEOMOp::OpHideAllAnnotations) );
    }
  }
  QAction* selPopupItem = aMenu.exec( viewport()->mapToGlobal(pos) );

  if ( selPopupItem == myActions[GEOMOp::OpShow] ||
       selPopupItem == myActions[GEOMOp::OpHide] ) {
    bool isVisible = selPopupItem == myActions[GEOMOp::OpShow];
    foreach ( QTreeWidgetItem* anItem, selectedItems() ) {
      setVisibility( anItem, isVisible );
    }
  }
}

//=================================================================================
// function : setVisibility
// purpose  : set item visible
//=================================================================================
void GEOMGUI_TextTreeWdg::setVisibility( QTreeWidgetItem* theItem, bool theVisibility )
{
  BranchType aBranchType = branchTypeFromItem( theItem );
  if ( theItem == myDimensionsItem ||
       theItem == myAnnotationsItem ) {

    QTreeWidgetItem* anItem;
    foreach ( QString entry, getObjects( aBranchType ).keys() ) {
      anItem = itemFromEntry( aBranchType, entry );
      if ( !anItem->isDisabled() )
        setAllShapeItemsVisibility( aBranchType, entry, theVisibility );
    }
    return;
  }
  else {
    if ( !theItem->isDisabled() && !theItem->parent()->isSelected() ) {
      QString anEntry = entryFromItem( theItem );
      if ( !anEntry.isEmpty() ) {
        // it is a shape item
        setAllShapeItemsVisibility( aBranchType, anEntry, theVisibility );
      } else {
        // it is a dimension item
        anEntry = entryFromItem( theItem->parent() );
        setShapeItemVisibility( aBranchType, anEntry, theItem, theVisibility );
      }
    }
  }
}

//=================================================================================
// function : setAllShapeItemsVisibility
// purpose  : 
//=================================================================================
void GEOMGUI_TextTreeWdg::setAllShapeItemsVisibility( const BranchType& theBranchType,
                                                      const QString& theEntry,
                                                      const bool theVisibility )
{
  QSharedPointer<VisualProperty>
    aProp = getVisualProperty( theBranchType, myStudy, theEntry.toStdString() );

  QTreeWidgetItem* anItem = itemFromEntry( theBranchType, theEntry );
  QTreeWidgetItem* aChildItem;
  for ( int i=0; i < anItem->childCount(); i++ ) {
    aChildItem = anItem->child( i );
    setShapeItemVisibility( aProp, aChildItem, theVisibility );
  }

  aProp->Save();

  redisplay( theEntry );
}

//=================================================================================
// function : setShapeItemVisibility
// purpose  : 
//=================================================================================
void GEOMGUI_TextTreeWdg::setShapeItemVisibility( const BranchType& theBranchType,
                                                  const QString& theEntry,
                                                  QTreeWidgetItem* theWidgetItem,
                                                  const bool theVisibility )
{
  QSharedPointer<VisualProperty>
    aProp = getVisualProperty( theBranchType, myStudy, theEntry.toStdString() );

  if ( setShapeItemVisibility( aProp, theWidgetItem, theVisibility ) ) {
    aProp->Save();
    redisplay( theEntry );
  }
}

//=================================================================================
// function : setShapeItemVisibility
// purpose  : 
//=================================================================================
void GEOMGUI_TextTreeWdg::updateVisibility()
{
  //QList<QString> aDimensionObjEntries = getObjects( DimensionShape ).keys();
  BranchType aBranchType = AnnotationShape;

  QList<QString> anAnnotationObjEntries = getObjects( aBranchType ).keys();

  QTreeWidgetItem* anEntryItem;
  foreach ( QString anEntry, getObjects( aBranchType ).keys() )
  {
    anEntryItem = itemFromEntry( aBranchType, anEntry );

    QTreeWidgetItem* anItem;
    for ( int i = 0; i < anEntryItem->childCount(); i++ ) {
      anItem = anEntryItem->child( i );
      int aDimIndex = idFromItem( anItem );
      QSharedPointer<VisualProperty> aProp = getVisualProperty( aBranchType, myStudy,
                                                                anEntry.toStdString() );
      bool isItemVisible = aProp->GetIsVisible( aDimIndex );
      anItem->setIcon( 1, isItemVisible ? myVisibleIcon : myInvisibleIcon );
    }

    redisplay( anEntry );
  }
}

//=================================================================================
// function : getSelected
// purpose  :
//=================================================================================
void GEOMGUI_TextTreeWdg::getSelected( QMap<QString, QList<int> >& theAnnotations )
{
  theAnnotations.clear();

  QItemSelectionModel* aSelectionModel = selectionModel();
  QModelIndexList aSelectedIndices = aSelectionModel->selectedIndexes();

  for (int i = 0, aNbItems = aSelectedIndices.size(); i < aNbItems; i++) {
    QTreeWidgetItem* anItem = itemFromIndex( aSelectedIndices[i] );
    if ( !anItem )
      continue;
    QString anEntry = entryFromItem( anItem->parent() );
    int aDimIndex = idFromItem( anItem );
    if ( aDimIndex < 0 )
      continue;

    if ( theAnnotations.contains( anEntry ) )
      theAnnotations[anEntry].append( aDimIndex );
    else {
      QList<int> anIndices;
      anIndices.append( aDimIndex );
      theAnnotations[anEntry] = anIndices;
    }
  }
}

//=================================================================================
// function : setSelected
// purpose  :
//=================================================================================
void GEOMGUI_TextTreeWdg::setSelected( const QMap<QString, QList<int> >& theAnnotations )
{
  QItemSelectionModel* aSelectionModel = selectionModel();
  aSelectionModel->clearSelection();

  QMap<QString, QList<int> >::const_iterator anIt = theAnnotations.begin(),
                                             aLast = theAnnotations.end();
  BranchType aBranchType = AnnotationShape;
  for ( ; anIt != aLast; anIt++ ) {
    QString anEntry = anIt.key();
    QTreeWidgetItem* anEntryItem = itemFromEntry( aBranchType, anEntry );
    if ( !anEntryItem )
      continue;

    QList<int> anAnnotationIds = anIt.value();
    QTreeWidgetItem* anItem;
    for ( int i = 0; i < anEntryItem->childCount(); i++ ) {
      anItem = anEntryItem->child( i );
      int aDimIndex = idFromItem( anItem );
      if ( anAnnotationIds.contains( aDimIndex ) ) {
        QModelIndex anIndex = indexFromItem( anItem );
        aSelectionModel->select( anIndex, QItemSelectionModel::Select );
      }
    }
  }
}

//=================================================================================
// function : getAllEntries
// purpose  :
//=================================================================================
QList<QString> GEOMGUI_TextTreeWdg::getAllEntries( const BranchType& theBranchType )
{
  return getObjects( theBranchType ).keys();
}

//=================================================================================
// function : getSingleSelectedObject
// purpose  :
//=================================================================================
QString GEOMGUI_TextTreeWdg::getSingleSelectedObject()
{
  QString anEntry;
  QTreeWidgetItem* anItem = currentItem();
  if (anItem) {
    QHash<QString, QTreeWidgetItem*> anObjects = getObjects( AnnotationShape );
    QHash<QString, QTreeWidgetItem*>::const_iterator anIt = anObjects.begin(),
                                                     aLast = anObjects.end();
    for (; anIt != aLast; anIt++) {
      if ( anIt.value() == anItem ) {
        anEntry = anIt.key();
        break;
      }
    }
  }
  return anEntry;
}

//=================================================================================
// function : setShapeItemVisibility
// purpose  :
//=================================================================================
bool GEOMGUI_TextTreeWdg::setShapeItemVisibility( QSharedPointer<VisualProperty>& theProps,
                                                  QTreeWidgetItem* theWidgetItem,
                                                  const bool theVisibility )
{
  int aDimIndex = idFromItem( theWidgetItem );
  if ( theProps->GetNumber() == 0 
    || theProps->GetIsVisible( aDimIndex ) == theVisibility ) {
    return false;
  }
  theProps->SetIsVisible( aDimIndex, theVisibility );

  theWidgetItem->setIcon( 1, theVisibility ? myVisibleIcon : myInvisibleIcon );

  return true;
}

//=================================================================================
// function : redisplay
// purpose  : 
//=================================================================================
void GEOMGUI_TextTreeWdg::redisplay( QString theEntry )
{
  Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject( theEntry.toUtf8().constData(), "GEOM", "TEMP_IO" );
  myDisplayer.Redisplay( io );
}

//=================================================================================
// function : getPropertyRootItem
// purpose  :
//=================================================================================
QTreeWidgetItem* GEOMGUI_TextTreeWdg::getPropertyRootItem( const BranchType& theBranchType )
{
  return (theBranchType == DimensionShape) ? myDimensionsItem : myAnnotationsItem;
}

//=================================================================================
// function : getObjects
// purpose  :
//=================================================================================
QHash<QString, QTreeWidgetItem*>& GEOMGUI_TextTreeWdg::getObjects( const BranchType& theBranchType )
{
  return (theBranchType == DimensionShape) ? myDimensionObjects : myAnnotationObjects;
}

//=================================================================================
// function : branchTypeFromItem
// purpose  :
//=================================================================================
GEOMGUI_TextTreeWdg::BranchType GEOMGUI_TextTreeWdg::branchTypeFromItem( QTreeWidgetItem* theItem )
{
  BranchType aBranchType = DimensionShape;

  bool aBranchTypeFound = false;
  QTreeWidgetItem* anItem = theItem;
  while( !aBranchTypeFound && anItem ) {
    if ( anItem == myDimensionsItem ||
         anItem == myAnnotationsItem) {
      aBranchTypeFound = true;
      aBranchType = (anItem == myDimensionsItem) ? DimensionShape : AnnotationShape;
    }
    else {
      anItem = anItem->parent();
    }
  }

  return aBranchType;
}
