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

#include "DependencyTree_View.h"
#include "DependencyTree_Object.h"
#include "DependencyTree_Arrow.h"

#include <GEOMBase.h>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Study.h>
#include <QtxActionToolMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>

// Qt includes
#include <QCloseEvent>
#include <QApplication>
#include <QGraphicsScene>

#define DRAW_EVENT  ( QEvent::User + 1 )

#include <iostream>

DependencyTree_View::DependencyTree_View( QWidget* theParent )
:GraphicsView_ViewPort(theParent),
myMaxDownwardLevelsNumber(0),
myMaxUpwardLevelsNumber(0),
myLevelsNumber(0),
myIsCompute(false),
myIsUpdate( true ),
myTotalCost(0),
myComputedCost(0)
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  myStudy = GeometryGUI::ClientStudyToStudy( study->studyDS());

  mySelectionMgr = app->selectionMgr();
  if ( !mySelectionMgr ) return;

  myMainEntries = new GEOM::string_array();

  getNewTreeModel();
}

DependencyTree_View::~DependencyTree_View()
{
}

void DependencyTree_View::init( GraphicsView_ViewFrame* theViewFrame )
{
  qthread = new DependencyTree_ComputeDlg_QThread( this );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  myNodesMovable = new QCheckBox( tr( "MOVE_NODES" ) );
  QWidgetAction* nodesMovableAction = new QWidgetAction( theViewFrame );
  nodesMovableAction->setDefaultWidget( myNodesMovable );

  myDisplayAscendants = new QCheckBox( tr( "DISPLAY_ASCENDANTS" ) );
  QWidgetAction* ShowParentsAction = new QWidgetAction( theViewFrame );
  ShowParentsAction->setDefaultWidget( myDisplayAscendants  );

  myDisplayDescendants = new QCheckBox(tr("DISPLAY_DESCENDANTS"));
  QWidgetAction* ShowChildrenAction = new QWidgetAction(theViewFrame);
  ShowChildrenAction->setDefaultWidget( myDisplayDescendants );

  QLabel* hierarchyDepthLabel = new QLabel( tr( "HIERARCHY_DEPTH" ) );
  QWidgetAction* hierarchyDepthLabelAction = new QWidgetAction(theViewFrame);
  hierarchyDepthLabelAction->setDefaultWidget( hierarchyDepthLabel );

  myLevelsNumber = checkMaxLevelsNumber();

  myHierarchyDepth = new QSpinBox();
  myHierarchyDepth->setRange( 0, checkMaxLevelsNumber() );
  myHierarchyDepth->setSpecialValueText( tr( "SHOW_ALL" ) );
  myHierarchyDepth->setValue( 0 );
  QWidgetAction* hierarchyDepthAction = new QWidgetAction( theViewFrame );
  hierarchyDepthAction->setDefaultWidget( myHierarchyDepth );

  updateButton = new QPushButton( tr( "UPDATE" ) );
  QWidgetAction* updateAction = new QWidgetAction( theViewFrame );
  updateAction->setDefaultWidget( updateButton );

  QPushButton* cancelButton = new QPushButton( tr( "CANCEL" ) );
  cancelButton->setCheckable( true );
  cancelAction = new QWidgetAction( theViewFrame );
  cancelAction->setDefaultWidget( cancelButton );
  cancelAction->setVisible( false );

  QProgressBar* progressBar = new QProgressBar( this );
  progressBar->setMinimum( 0 );
  progressBar->setMaximum( 100 );
  progressBar->setFixedWidth( 100 );
  progressAction = new QWidgetAction( theViewFrame );
  progressAction->setDefaultWidget( progressBar );
  progressAction->setVisible( false );

  QAction* separator1 = theViewFrame->toolMgr()->separator( false );
  QAction* separator2 = theViewFrame->toolMgr()->separator( false );

  theViewFrame->toolMgr()->append( separator1, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( nodesMovableAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( hierarchyDepthLabelAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( hierarchyDepthAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( ShowParentsAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( ShowChildrenAction, theViewFrame->getToolBarId() );

  theViewFrame->toolMgr()->append( separator2, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( updateAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( progressAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( cancelAction, theViewFrame->getToolBarId() );

  connect( myNodesMovable, SIGNAL( toggled( bool ) ), this, SLOT( onMoveNodes( bool ) ) );
  connect( myHierarchyDepth, SIGNAL( valueChanged ( int ) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayAscendants , SIGNAL( toggled( bool ) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayDescendants, SIGNAL( toggled( bool ) ), this, SLOT( onHierarchyType() ) );
  connect( updateButton, SIGNAL( clicked() ), this, SLOT( onUpdateModel() ) );
  connect( cancelButton, SIGNAL( clicked() ), this, SLOT( onCancel() ) );

  setPrefBackgroundColor( resMgr->colorValue( "Geometry", "dependency_tree_background_color", QColor( 255, 255, 255 ) ) );
  setNodesMovable( resMgr->booleanValue( "Geometry", "dependency_tree_move_nodes", true ) );
  setHierarchyType( resMgr->integerValue( "Geometry", "dependency_tree_hierarchy_type", 0 ) );
}

void DependencyTree_View::updateModel( bool theUseSelectedObject, bool theUseOB )
{
  getNewTreeModel( theUseSelectedObject, theUseOB );
  onHierarchyType();
}

void DependencyTree_View::drawTree()
{
  myComputedCost = 0;
  calcTotalCost();
  std::cout << "\n\n\n TOTAL COST = " << myTotalCost << std::endl;

	if( !myIsCompute )
    return;

  clearView( false );
  clearSelected();

  // draw nodes on scene
  std::map< std::string, int > entryLevelMap;
  std::map< int, std::vector< std::string > > levelObjects;
  int currentLevel;
  int horDistance, verDistance;
  GEOMUtils::TreeModel::const_reverse_iterator i;
  for( i = myTreeModel.rbegin(); i != myTreeModel.rend(); i++ ) {
	if( !myIsCompute )
      return;
    currentLevel = 0;
    myComputedCost++;
    std::string objectEntry = i->first;
    DependencyTree_Object* objectItem = myTreeMap[ objectEntry ];
    horDistance = 100 + int( objectItem->boundingRect().width() );
    verDistance = 3 * int( objectItem->boundingRect().height() );
    if( isItemAdded( objectItem ) )
      currentLevel = entryLevelMap[ objectEntry ];
    else {
      addNewItem( objectItem );
      objectItem->unselect();
      entryLevelMap[ objectEntry ] = currentLevel;
      levelObjects[ currentLevel ].push_back( objectEntry );
    }
    objectItem->setIsMainObject( true );

    if( myDisplayAscendants->isChecked() )
      drawWard( i->second.first, entryLevelMap, levelObjects, currentLevel, -1 );
    if( myDisplayDescendants->isChecked() )
      drawWard( i->second.second, entryLevelMap, levelObjects, currentLevel, 1 );
  }

  std::map< int, std::vector< std::string > >::const_iterator level;
  for( level = levelObjects.begin(); level != levelObjects.end(); level++ ) {
    int step = -horDistance * ( level->second.size() - 1 ) / 2;
    std::cout<<"\n\n LEVEL = " << level->first << std::endl;
    for( int objIter = 0; objIter < level->second.size(); objIter++ ) {
        std::cout << level->second.at( objIter ) << ", ";
        DependencyTree_Object* anObject = myTreeMap[ level->second.at( objIter ) ];
        anObject->setPos( step, verDistance * level->first );
        step += horDistance;
    }
  }

  // draw arrows on scene
  GEOMUtils::TreeModel::const_iterator j;
  for( j = myTreeModel.begin(); j != myTreeModel.end(); j++ ) {
    DependencyTree_Object* Main_object = myTreeMap[ j->first ];
    if( j->second.first.size() > 0 ) {
      GEOMUtils::LevelInfo Levelup = j->second.first.at(0);
      if( myDisplayAscendants ->isChecked() ) {
        GEOMUtils::LevelInfo::const_iterator node;
        for (node = Levelup.begin(); node != Levelup.end(); node++ ) {
          DependencyTree_Object* object = myTreeMap[node->first];
          DependencyTree_Arrow* arrow = myArrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>(Main_object, object)];
          if( arrow && !isItemAdded( arrow) )
            addNewItem( arrow );
        }
      }
    }
    if( myDisplayAscendants->isChecked() )
      drawWardArrows( j->second.first );
    if( myDisplayDescendants->isChecked() )
      drawWardArrows( j->second.second );
  }
  std::cout << "\n ComputedCost = " << myComputedCost << std::endl;

}

void DependencyTree_View::customEvent( QEvent * event )
{
  if( event->type() == DRAW_EVENT ) {

    QPushButton* cancelButton = dynamic_cast<QPushButton*>( cancelAction->defaultWidget() );
    QProgressBar* progressBar = dynamic_cast<QProgressBar*>( progressAction->defaultWidget() );

    if ( !cancelButton->isChecked() )
      progressBar->setValue( progressBar->maximum() * getComputeProgress() );

    std::cout << "\n\n *** qthread->isFinished() = " << qthread->isFinished() << std::endl;
    if( !myIsCompute || qthread->isFinished() ) {
      changeWidgetState( false );
      cancelButton->setChecked( false );
      progressBar->setValue(0);
      fitAll();
      QApplication::removePostedEvents( this, ( QEvent::Type )DRAW_EVENT );
    }
  }
  event->accept();
}

void DependencyTree_View::addNewItem( QGraphicsItem* theObject )
{
  qthread->sleepDraw();
  if( theObject )
    addItem( theObject );
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  QApplication::postEvent( this, new QEvent( ( QEvent::Type )DRAW_EVENT ) );
}

void DependencyTree_View::mouseMoveEvent(QMouseEvent *event)
{
  QGraphicsView::mouseMoveEvent( event );
  ArrowsInfo::const_iterator j;
  for (j = myArrows.begin(); j != myArrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = myArrows[ j->first ];
    arrow->update();
  }
}

DependencyTree_Object* DependencyTree_View::getObjectByEntry( QString theEntry )
{
  return myTreeMap[theEntry.toStdString()];
}

void DependencyTree_View::setHierarchyType( const int theType )
{
  myIsUpdate = false;
  switch( theType ) {
  case 0:
    myDisplayAscendants->setChecked( true );
    myDisplayDescendants->setChecked( true );
    break;
  case 1:
    myDisplayAscendants->setChecked( true );
    myDisplayDescendants->setChecked( false );
    break;
  case 2:
    myDisplayAscendants->setChecked( false );
    myDisplayDescendants->setChecked( true );
    break;
  }
  myIsUpdate = true;

  myLevelsNumber = checkMaxLevelsNumber();

  onHierarchyType();
}

void DependencyTree_View::setNodesMovable( const bool theIsMovable )
{
  myNodesMovable->setChecked( theIsMovable );
}

void DependencyTree_View::setPrefBackgroundColor( const QColor& theColor )
{
  if( isForegroundEnabled() )
  {
    setForegroundColor( theColor );
    updateForeground();
  }
  else
    setBackgroundColor( theColor );
}

void DependencyTree_View::setNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setColor( theColor );
  }
}

void DependencyTree_View::setMainNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setMainObjectColor( theColor );
  }
}

void DependencyTree_View::setSelectNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setSelectColor( theColor );
  }
}

void DependencyTree_View::setArrowColor( const QColor& theColor )
{
  ArrowsInfo::const_iterator j;
  for (j = myArrows.begin(); j != myArrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = myArrows[ j->first ];
    arrow->setColor( theColor );
  }
}

void DependencyTree_View::setHighlightArrowColor( const QColor& theColor )
{
	ArrowsInfo::const_iterator j;
  for (j = myArrows.begin(); j != myArrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = myArrows[ j->first ];
    arrow->setHighlightColor( theColor );
  }
}

void DependencyTree_View::setSelectArrowColor( const QColor& theColor )
{
	ArrowsInfo::const_iterator j;
  for (j = myArrows.begin(); j != myArrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = myArrows[ j->first ];
    arrow->setSelectColor( theColor );
  }
}

void DependencyTree_View::setIsCompute( bool theIsCompute )
{
  myIsCompute = theIsCompute;
}

bool DependencyTree_View::getIsCompute()
{
  return myIsCompute;
}

//void DependencyTree_View::timerEvent(QTimerEvent *event)
//{
//  QPushButton* cancelButton = dynamic_cast<QPushButton*>( cancelAction->defaultWidget() );
//  QProgressBar* progressBar = dynamic_cast<QProgressBar*>( progressAction->defaultWidget() );
//
//  std::cout << "TIMER! " << std::endl;
//  if ( !cancelButton->isChecked() )
//    progressBar->setValue( progressBar->maximum() * getComputeProgress() );
//
//  if( !myIsCompute || qthread->isFinished() ) {
//    changeWidgetState( false );
//    killTimer( myTimer );
//    cancelButton->setChecked( false );
//    progressBar->setValue(0);
//  }
//  event->accept();
//}

void DependencyTree_View::closeEvent( QCloseEvent* event )
{
  if(qthread->isRunning())
  {
    event->ignore();
    return;
  }
  event->accept();
}

void DependencyTree_View::onUpdateModel()
{
  updateModel( false );
}

void DependencyTree_View::onRebuildModel()
{
  updateModel( true, false );
}

void DependencyTree_View::updateView()
{
  if( !myIsUpdate )
    return;

  changeWidgetState( true );

  qthread->start();




}

void DependencyTree_View::onMoveNodes( bool theIsMoveNodes )
{
  EntryObjectMap::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    if( object )
      object->setMovable( theIsMoveNodes );
  }
}

void DependencyTree_View::onHierarchyType()
{
  myHierarchyDepth->setRange( 0, checkMaxLevelsNumber() );
  if( myHierarchyDepth->value() > checkMaxLevelsNumber() )
    myHierarchyDepth->setValue( checkMaxLevelsNumber() );

  if( myHierarchyDepth->value() == 0 )
    myLevelsNumber = myHierarchyDepth->maximum();
  else
    myLevelsNumber = myHierarchyDepth->value();

  updateView();
}

void DependencyTree_View::onCancel()
{
  qthread->cancel();
  //qthread->deleteLater();
}

void DependencyTree_View::addNode( const std::string& theEntry )
{
  if( !myTreeMap[theEntry] )
    myTreeMap[theEntry] = new DependencyTree_Object( theEntry );
}

void DependencyTree_View::addArrow( DependencyTree_Object *startItem, DependencyTree_Object *endItem )
{
  bool isFind = false;

  std::cout << "   " << startItem->getEntry() << "   " << endItem->getEntry() << std::endl;
  ArrowsInfo::const_iterator i;
  for (i = myArrows.begin(); i != myArrows.end(); i++ ) {
	  DependencyTree_Arrow* arrow = i->second;
    if( arrow->getStartItem() == startItem && arrow->getEndItem() == endItem ) {
      isFind = true;
      std::cout<<" theSame " << std::endl;
    }
    else if( arrow->getStartItem() == endItem && arrow->getEndItem() == startItem ) {
      arrow->setIsBiLink( true );
      std::cout<<" Bilink " << std::endl;
      isFind = true;
    }
  }

  if( !isFind ) {
	  DependencyTree_Arrow *arrow = new DependencyTree_Arrow(startItem, endItem);
	    myArrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>( startItem, endItem )] = arrow;
	    std::cout<<" addArrow " << std::endl;
  }
}

void DependencyTree_View::parseTree()
{

  GEOMUtils::TreeModel::const_iterator i;
  for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
    std::string objectEntry = i->first;
    addNode( objectEntry );
    parseTreeWard( i->second.first );
    if( i->second.first.size() > myMaxUpwardLevelsNumber )
      myMaxUpwardLevelsNumber = i->second.first.size();
    parseTreeWard( i->second.second );
    if( i->second.second.size() > myMaxDownwardLevelsNumber )
      myMaxDownwardLevelsNumber = i->second.second.size();
  }

  for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
    DependencyTree_Object* Main_object = myTreeMap[i->first];
    if( i->second.first.size() > 0 ) {
      GEOMUtils::LevelInfo Levelup = i->second.first.at(0);
      GEOMUtils::LevelInfo::const_iterator node;
      for (node = Levelup.begin(); node != Levelup.end(); node++ ) {
        DependencyTree_Object* object = myTreeMap[node->first];
        addArrow( Main_object, object );
      }
    }
    parseTreeWardArrow( i->second.first );
    parseTreeWardArrow( i->second.second );
  }


}
void DependencyTree_View::parseTreeWard(const GEOMUtils::LevelsList theWard)
{
  int levelsNumber = theWard.size();
  for( int level = 0; level < levelsNumber; level++ ) {
    GEOMUtils::LevelInfo levelInfo = theWard[ level ];
    GEOMUtils::LevelInfo::const_iterator node;
    for (node = levelInfo.begin(); node != levelInfo.end(); node++ ) {
      addNode( node->first );
    }
  }
}
void DependencyTree_View::parseTreeWardArrow(const GEOMUtils::LevelsList theWard)
{
  for(int j = 0; j < theWard.size(); j++ ) {
	  GEOMUtils::LevelInfo Level = theWard.at(j);
	  GEOMUtils::LevelInfo::const_iterator node;
    for (node = Level.begin(); node != Level.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[node->first];
      std::vector<std::string> Links = node->second;
      for( int link = 0; link < Links.size(); link++ ) {
        DependencyTree_Object* LinkObject = myTreeMap[Links[link]];
        if( object && LinkObject )
          addArrow( object, LinkObject );
      }
    }
  }
}

void DependencyTree_View::drawWard( const GEOMUtils::LevelsList& theWard,
                                    std::map< std::string, int >& theEntryLevelMap,
                                    std::map< int, std::vector< std::string > >& theLevelObjects,
                                    int theCurrentLevel, const int theLevelStep )
{
  for( int level = 0; level < theWard.size(); level++ ) {
    if( level >= myLevelsNumber || !myIsCompute )
      return;
    myComputedCost++;
    theCurrentLevel += theLevelStep;
    GEOMUtils::LevelInfo levelInfo = theWard.at( level );
    GEOMUtils::LevelInfo::const_iterator node;
    for (node = levelInfo.begin(); node != levelInfo.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[ node->first ];
      if( !isItemAdded( object ) ) {
        addNewItem( object );
        object->unselect();
        theEntryLevelMap[ node->first ] = theCurrentLevel;
        theLevelObjects[ theCurrentLevel ].push_back( node->first );
      }
    }
  }
}

void DependencyTree_View::drawWardArrows( GEOMUtils::LevelsList theWard )
{
  for(int j = 0; j < theWard.size(); j++ ) {
    if( j >= myLevelsNumber || !myIsCompute )
      break;
    myComputedCost++;
    GEOMUtils::LevelInfo Level = theWard.at(j);
    GEOMUtils::LevelInfo::const_iterator node;
    for (node = Level.begin(); node != Level.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[node->first];
      GEOMUtils::NodeLinks Links = node->second;
      for( int link = 0; link < Links.size(); link++ ) {
        DependencyTree_Object* LinkObject = myTreeMap[Links[link]];
        if( isItemAdded( object ) && isItemAdded( LinkObject ) ) {
          DependencyTree_Arrow* arrow = myArrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>(object, LinkObject)];
          if( arrow && !isItemAdded( arrow) )
            addNewItem( arrow );
        }
      }
    }
  }
}

void DependencyTree_View::getNewTreeModel( bool theUseSelectedObject, bool theUseOB )
{

  GEOM::string_array_var objectsEntry = new GEOM::string_array();
  int iter = 0;

  if( theUseSelectedObject ) {
    if( theUseOB ) {
      SALOME_ListIO mainObjects;
      mySelectionMgr->selectedObjects( mainObjects );
      // create a list of selected object entry
      objectsEntry->length( mainObjects.Extent());
      for ( SALOME_ListIteratorOfListIO It( mainObjects ); It.More(); It.Next(), iter++ ) {
        Handle( SALOME_InteractiveObject ) io = It.Value();
        GEOM::GEOM_Object_var geomObject = GEOM::GEOM_Object::_nil();
        geomObject = GEOMBase::ConvertIOinGEOMObject( io );
        QString entry = geomObject->GetEntry();
        objectsEntry[ iter ] = entry.toLatin1().constData();
      }
    }
    else {
      objectsEntry->length( nbSelected() );
      for( initSelected(); moreSelected(); nextSelected(), iter++ )
        if( DependencyTree_Object* treeObject = dynamic_cast<DependencyTree_Object*>( selectedObject() ) ) {
          objectsEntry[ iter ] = treeObject->getEntry().c_str();
          std::cout << "\n\n\n ----------- entry = " << treeObject->getEntry() << std::endl;
        }
    }

    myMainEntries = objectsEntry;
  }

  // get string which describes dependency tree structure
  SALOMEDS::TMPFile_var SeqFile =
    GeometryGUI::GetGeomGen()->GetDependencyTree( myStudy, myMainEntries );
  char* buf = (char*) &SeqFile[0];

  std::cout << "\n\n\n\n\n TREE = " << buf << std::endl;

  clearView( true );
  // get dependency tree structure
  GEOMUtils::ConvertStringToTree( buf, myTreeModel );

  mySelectionMgr->clearSelected();

  parseTree();

}

void DependencyTree_View::clearView( bool isClearModel )
{
  EntryObjectMap::const_iterator objectIter;
  for( objectIter = myTreeMap.begin(); objectIter != myTreeMap.end(); objectIter++ ) {
    DependencyTree_Object* object = objectIter->second;
    if( object )
      if( isItemAdded( object ) )
      removeItem( object );
  }

  ArrowsInfo::const_iterator arrowIter;
  for( arrowIter = myArrows.begin(); arrowIter != myArrows.end(); arrowIter++ ) {
    DependencyTree_Arrow* object = arrowIter->second;
    if( object )
      if( isItemAdded( object ) )
      removeItem( object );
  }
  if( isClearModel ) {
    myTreeMap.clear();
    myArrows.clear();
    myTreeModel.clear();
    myMaxDownwardLevelsNumber = 0;
    myMaxUpwardLevelsNumber = 0;
    myLevelsNumber = 0;
    myIsCompute = false;
    myIsUpdate = true;
  }
}

int DependencyTree_View::checkMaxLevelsNumber()
{
  if( myDisplayAscendants->isChecked() && myDisplayDescendants->isChecked() )
    return myMaxUpwardLevelsNumber>myMaxDownwardLevelsNumber?myMaxUpwardLevelsNumber:myMaxDownwardLevelsNumber;
  else if( myDisplayAscendants ->isChecked() )
    return myMaxUpwardLevelsNumber;
  else if( myDisplayDescendants->isChecked() )
    return  myMaxDownwardLevelsNumber;
}

void DependencyTree_View::calcTotalCost()
{
  myTotalCost = myTreeModel.size();
  GEOMUtils::TreeModel::const_iterator i;
  for( i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
    if( myDisplayAscendants->isChecked() )
      myTotalCost += 2*( myLevelsNumber < i->second.first.size() ? myLevelsNumber : i->second.first.size() );
    if( myDisplayDescendants->isChecked() )
      myTotalCost += 2*( myLevelsNumber < i->second.second.size() ? myLevelsNumber : i->second.second.size() );
  }
}

double DependencyTree_View::getComputeProgress()
{
  return double( myComputedCost ) / double( myTotalCost );
}

void DependencyTree_View::changeWidgetState( bool theIsCompute )
{
  cancelAction->setVisible( theIsCompute );
  progressAction->setVisible( theIsCompute );

  myHierarchyDepth->setEnabled( !theIsCompute );
  myDisplayAscendants->setEnabled( !theIsCompute );
  myDisplayDescendants->setEnabled( !theIsCompute );
  updateButton->setEnabled( !theIsCompute );
}

bool DependencyTree_View::updateObjectName( const std::string &theEntry )
{
  bool res = false;
  for( initSelected(); moreSelected(); nextSelected() ) {
    if( DependencyTree_Object* aDepObject = dynamic_cast<DependencyTree_Object*>( selectedObject() ) ) {
      aDepObject->updateName();
      res = true;
    }
  }
  return res;
}

DependencyTree_ComputeDlg_QThread::DependencyTree_ComputeDlg_QThread( DependencyTree_View* theView )
{
  myView = theView;
}

void DependencyTree_ComputeDlg_QThread::run()
{
  myView->myMutex.lock();
 // QMutexLocker lock( &myView->myMutex );
  myView->setIsCompute( true );
  myView->drawTree();

  QApplication::postEvent( myView, new QEvent( ( QEvent::Type )DRAW_EVENT ) );
  myView->myMutex.unlock();
  //exec();
}

void DependencyTree_ComputeDlg_QThread::sleepDraw()
{
  msleep(1);
}

void DependencyTree_ComputeDlg_QThread::cancel()
{
  myView->setIsCompute( false );
}
