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

// Internal includes
#include "DependencyTree_View.h"
#include "DependencyTree_Object.h"
#include "DependencyTree_Arrow.h"

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Study.h>
#include <QtxActionToolMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>

// GEOM includes
#include <GEOMBase.h>

// Qt includes
#include <QApplication>
#include <QWidgetAction>
#include <QWheelEvent>

DependencyTree_View::DependencyTree_View( QWidget* theParent )
:GraphicsView_ViewPort( theParent ),
myLevelsNumber(0),
myMaxDownwardLevelsNumber(0),
myMaxUpwardLevelsNumber(0),
myIsUpdate( true )
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
  clearView( true );
}

//=================================================================================
// function : init()
// purpose  : this method is obligatory for initialize view frame actions
//=================================================================================
void DependencyTree_View::init( GraphicsView_ViewFrame* theViewFrame )
{
  myNodesMovable = new QCheckBox( tr( "MOVE_NODES" ) );
  QWidgetAction* nodesMovableAction = new QWidgetAction( theViewFrame );
  nodesMovableAction->setDefaultWidget( myNodesMovable );

  myDisplayAscendants = new QCheckBox( tr( "DISPLAY_ASCENDANTS" ) );
  QWidgetAction* displayAscendantsAction = new QWidgetAction( theViewFrame );
  displayAscendantsAction->setDefaultWidget( myDisplayAscendants  );

  myDisplayDescendants = new QCheckBox(tr("DISPLAY_DESCENDANTS"));
  QWidgetAction* displayDescendantsAction = new QWidgetAction( theViewFrame );
  displayDescendantsAction->setDefaultWidget( myDisplayDescendants );

  QLabel* hierarchyDepthLabel = new QLabel( tr( "HIERARCHY_DEPTH" ) );
  QWidgetAction* hierarchyDepthLabelAction = new QWidgetAction( theViewFrame );
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

  QAction* separator1 = theViewFrame->toolMgr()->separator( false );
  QAction* separator2 = theViewFrame->toolMgr()->separator( false );

  theViewFrame->toolMgr()->append( separator1, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( hierarchyDepthLabelAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( hierarchyDepthAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( displayAscendantsAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( displayDescendantsAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( nodesMovableAction, theViewFrame->getToolBarId() );

  theViewFrame->toolMgr()->append( separator2, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( updateAction, theViewFrame->getToolBarId() );

  connect( myNodesMovable, SIGNAL( toggled( bool ) ), this, SLOT( onMoveNodes( bool ) ) );
  connect( myHierarchyDepth, SIGNAL( valueChanged ( int ) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayAscendants , SIGNAL( toggled( bool ) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayDescendants, SIGNAL( toggled( bool ) ), this, SLOT( onHierarchyType() ) );
  connect( updateButton, SIGNAL( clicked() ), this, SLOT( onUpdateModel() ) );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
  if ( aGeomGUI ) {
    connect( aGeomGUI, SIGNAL( SignalDependencyTreeParamChanged( const QString&, const QString& ) ),
             this, SLOT( onPreferenceChanged( const QString&, const QString& ) ) );
    connect( aGeomGUI, SIGNAL( SignalDependencyTreeRenameObject( const QString& ) ),
             this, SLOT( onRenameObject( const QString& ) ) );
  }

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  setPrefBackgroundColor( resMgr->colorValue( "Geometry", "dependency_tree_background_color", QColor( 255, 255, 255 ) ) );
  setNodesMovable( resMgr->booleanValue( "Geometry", "dependency_tree_move_nodes", true ) );
  setHierarchyType( resMgr->integerValue( "Geometry", "dependency_tree_hierarchy_type", 0 ) );
}

//=================================================================================
// function : updateModel()
// purpose  : run all stage of dependency tree creation
//=================================================================================
void DependencyTree_View::updateModel( bool theUseSelectedObject, bool theUseOB )
{
  getNewTreeModel( theUseSelectedObject, theUseOB );
  onHierarchyType();
}

//=================================================================================
// function : mouseMoveEvent()
// purpose  : make some actions when mouse was moved
//=================================================================================
void DependencyTree_View::mouseMoveEvent( QMouseEvent *event )
{
  QGraphicsView::mouseMoveEvent( event );
  ArrowsInfo::const_iterator i;
  for( i = myArrows.begin(); i != myArrows.end(); i++ ) {
    if( DependencyTree_Arrow* arrow = myArrows[ i->first ] )
      arrow->update();
  }
}

//=================================================================================
// function : wheelEvent()
// purpose  : add zoom action when wheel is spinning
//=================================================================================
void DependencyTree_View::wheelEvent( QWheelEvent* event )
{
  int inc = 10; // zoom coefficient
  double cx = width() / 2;
  double cy = height() / 2;
  if( event->delta() > 0 )
    zoom( cx, cy, cx + inc, cy + inc );
  else
    zoom( cx, cy, cx - inc, cy - inc );

  QGraphicsView::wheelEvent( event );
}

//=================================================================================
// function : getViewName()
// purpose  : return the name of current view
//=================================================================================
QString DependencyTree_View::getViewName() const
{
  return tr( "DEPENDENCY_TREE" );
}

//=================================================================================
// function : getStudyId()
// purpose  : return Id of current study
//=================================================================================
int DependencyTree_View::getStudyId() const
{
  return myStudy->StudyId();
}

//=================================================================================
// function : getObjectByEntry()
// purpose  : return DependencyTree_Object by entry
//=================================================================================
DependencyTree_Object* DependencyTree_View::getObjectByEntry( const std::string& theEntry )
{
  return myTreeMap[ theEntry ];
}

//=================================================================================
// function : setHierarchyType()
// purpose  : set hierarchy type of dependency tree
//=================================================================================
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

//=================================================================================
// function : setNodesMovable()
// purpose  : set possibility to move nodes or not
//=================================================================================
void DependencyTree_View::setNodesMovable( const bool theIsMovable )
{
  myNodesMovable->setChecked( theIsMovable );
}

//=================================================================================
// function : setPrefBackgroundColor()
// purpose  : set background color from preferences
//=================================================================================
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

//=================================================================================
// function : setNodeColor()
// purpose  : set node color from preferences
//=================================================================================
void DependencyTree_View::setNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for( i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setColor( theColor );
  }
}

//=================================================================================
// function : setMainNodeColor()
// purpose  : set main node color from preferences
//=================================================================================
void DependencyTree_View::setMainNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for( i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setMainObjectColor( theColor );
  }
}

//=================================================================================
// function : setUnpublishNodeColor()
// purpose  : set unpublished node color from preferences
//=================================================================================
void DependencyTree_View::setUnpublishNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for( i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setUnpublishObjectColor( theColor );
  }
}

//=================================================================================
// function : setSelectNodeColor()
// purpose  : set selected node color from preferences
//=================================================================================
void DependencyTree_View::setSelectNodeColor( const QColor& theColor )
{
  EntryObjectMap::const_iterator i;
  for( i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setSelectColor( theColor );
  }
}

//=================================================================================
// function : setArrowColor()
// purpose  : set arrow color from preferences
//=================================================================================
void DependencyTree_View::setArrowColor( const QColor& theColor )
{
  ArrowsInfo::const_iterator i;
  for( i = myArrows.begin(); i != myArrows.end(); i++ ) {
    DependencyTree_Arrow* arrow = myArrows[ i->first ];
    arrow->setColor( theColor );
  }
}

//=================================================================================
// function : setHighlightArrowColor()
// purpose  : set highlighted arrow color from preferences
//=================================================================================
void DependencyTree_View::setHighlightArrowColor( const QColor& theColor )
{
  ArrowsInfo::const_iterator i;
  for( i = myArrows.begin(); i != myArrows.end(); i++ ) {
    DependencyTree_Arrow* arrow = myArrows[ i->first ];
    arrow->setHighlightColor( theColor );
  }
}

//=================================================================================
// function : setSelectArrowColor()
// purpose  : set selected arrow color from preferences
//=================================================================================
void DependencyTree_View::setSelectArrowColor( const QColor& theColor )
{
  ArrowsInfo::const_iterator i;
  for( i = myArrows.begin(); i != myArrows.end(); i++ ) {
    DependencyTree_Arrow* arrow = myArrows[ i->first ];
    arrow->setSelectColor( theColor );
  }
}

//=================================================================================
// function : onRebuildModel()
// purpose  : slot for updating tree model using selected objects in viewer
//=================================================================================
void DependencyTree_View::onRebuildModel()
{
  updateModel( true, false );
}

//=================================================================================
// function : resizeEvent()
// purpose  : reimplemented from QGraphicsView::resizeEvent()
//=================================================================================
void DependencyTree_View::resizeEvent(QResizeEvent *event)
{
  QPointF aCenter = mapToScene( event->oldSize().width()/2,
                                event->oldSize().height()/2 );
  QGraphicsView::resizeEvent( event );
  centerOn( aCenter.x(),aCenter.y() );
}

//=================================================================================
// function : onUpdateModel()
// purpose  : slot for updating tree model for main objects in viewer
//=================================================================================
void DependencyTree_View::onUpdateModel()
{
  updateModel( false );
}

//=================================================================================
// function : onMoveNodes()
// purpose  : slot for setting the possibility to move nodes in viewer
//=================================================================================
void DependencyTree_View::onMoveNodes( bool theIsMoveNodes )
{
  EntryObjectMap::const_iterator i;
  for( i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    if( object )
      object->setMovable( theIsMoveNodes );
  }
}

//=================================================================================
// function : onHierarchyType()
// purpose  : slot for setting the hierarchy type of tree
//=================================================================================
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

//=================================================================================
// function : onPreferencesChanged()
// purpose  : slot for changing tree parameters from preferences
//=================================================================================
void DependencyTree_View::onPreferenceChanged( const QString& section, const QString& param )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  if( param == QString("dependency_tree_hierarchy_type") ) {
    int hierarchyType = resMgr->integerValue( section, param, 0);
    setHierarchyType( hierarchyType );
  }
  else if(  param == QString("dependency_tree_move_nodes") ) {
    bool isNodesMovable = resMgr->booleanValue( section, param, true);
    setNodesMovable( isNodesMovable );
  }
  else if(  param == QString("dependency_tree_background_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 255, 255, 255 ) );
    setPrefBackgroundColor( c );
  }
  else if(  param == QString("dependency_tree_node_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 62, 180, 238 ) );
    setNodeColor( c );
  }
  else if(  param == QString("dependency_tree_main_node_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 238, 90, 125 ) );
    setMainNodeColor( c );
  }
  else if(  param == QString("dependency_tree_unpublish_node_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 255, 255, 255 ) );
    setUnpublishNodeColor( c );
  }
  else if(  param == QString("dependency_tree_select_node_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 237, 243, 58 ) );
    setSelectNodeColor( c );
  }
  else if(  param == QString("dependency_tree_arrow_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 0, 0, 130 ) );
    setArrowColor( c );
  }
  else if(  param == QString("dependency_tree_highlight_arrow_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 0, 0, 255 ) );
    setHighlightArrowColor( c );
  }
  else if(  param == QString("dependency_tree_select_arrow_color") ) {
    QColor c = resMgr->colorValue( section, param, QColor( 255, 0, 0 ) );
    setSelectArrowColor( c );
  }
}

//=================================================================================
// function : onRenameObject()
// purpose  : update object name, having edited it in Object Browser
//=================================================================================
void DependencyTree_View::onRenameObject( const QString& theEntry )
{
  if( DependencyTree_Object* object = getObjectByEntry( theEntry.toStdString() ) )
    object->updateName();
}

//=================================================================================
// function : parseTree()
// purpose  : parse created model to initialize all nodes and arrows
//=================================================================================
void DependencyTree_View::parseTree()
{
  GEOMUtils::TreeModel::const_iterator i;
  for( i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
    std::string objectEntry = i->first;
    addNode( objectEntry );
    parseTreeWard( i->second.first );
    if( i->second.first.size() > myMaxUpwardLevelsNumber )
      myMaxUpwardLevelsNumber = i->second.first.size();
    parseTreeWard( i->second.second );
    if( i->second.second.size() > myMaxDownwardLevelsNumber )
      myMaxDownwardLevelsNumber = i->second.second.size();
  }

  for( i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
    DependencyTree_Object* Main_object = myTreeMap[ i->first ];
    if( i->second.first.size() > 0 ) {
      GEOMUtils::LevelInfo Levelup = i->second.first.at(0);
      GEOMUtils::LevelInfo::const_iterator node;
      for( node = Levelup.begin(); node != Levelup.end(); node++ ) {
        DependencyTree_Object* object = myTreeMap[ node->first ];
        addArrow( object, Main_object );
      }
    }
    parseTreeWardArrow( i->second.first );
    parseTreeWardArrow( i->second.second );
  }
}

//=================================================================================
// function : parseTreeWard()
// purpose  : parse tree ward to initialize all nodes of current ward
//=================================================================================
void DependencyTree_View::parseTreeWard( const GEOMUtils::LevelsList& theWard )
{
  int levelsNumber = theWard.size();
  for( int level = 0; level < levelsNumber; level++ ) {
    GEOMUtils::LevelInfo levelInfo = theWard[ level ];
    GEOMUtils::LevelInfo::const_iterator node;
    for( node = levelInfo.begin(); node != levelInfo.end(); node++ )
      addNode( node->first );
  }
}

//=================================================================================
// function : parseTreeWardArrow()
// purpose  : parse tree ward to initialize all arrows of current ward
//=================================================================================
void DependencyTree_View::parseTreeWardArrow( const GEOMUtils::LevelsList& theWard)
{
  for( int j = 0; j < theWard.size(); j++ ) {
    GEOMUtils::LevelInfo Level = theWard.at(j);
    GEOMUtils::LevelInfo::const_iterator node;
    for( node = Level.begin(); node != Level.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[ node->first ];
      std::vector<std::string> Links = node->second;
      for( int link = 0; link < Links.size(); link++ ) {
        DependencyTree_Object* LinkObject = myTreeMap[ Links[ link ] ];
        if( object && LinkObject )
          addArrow( LinkObject, object );
      }
    }
  }
}

//=================================================================================
// function : addNode()
// purpose  : add node to viewer
//=================================================================================
void DependencyTree_View::addNode( const std::string& theEntry )
{
  if( !myTreeMap[theEntry] )
    myTreeMap[theEntry] = new DependencyTree_Object( theEntry );
}

//=================================================================================
// function : addArrow()
// purpose  : add arrow to viewer
//=================================================================================
void DependencyTree_View::addArrow( DependencyTree_Object* startItem, DependencyTree_Object* endItem )
{
  bool isFind = false;

  ArrowsInfo::const_iterator i;
  for( i = myArrows.begin(); i != myArrows.end(); i++ ) {
    DependencyTree_Arrow* arrow = i->second;
    if( arrow->getStartItem() == startItem && arrow->getEndItem() == endItem )
      isFind = true;
    else if( arrow->getStartItem() == endItem && arrow->getEndItem() == startItem ) {
      arrow->setIsBiLink( true );
      isFind = true;
    }
  }
  if( !isFind ) {
    DependencyTree_Arrow *arrow = new DependencyTree_Arrow( startItem, endItem );
    myArrows[ std::pair<DependencyTree_Object*,DependencyTree_Object*>( startItem, endItem ) ] = arrow;
  }
}

//=================================================================================
// function : drawTree()
// purpose  : redraw dependency tree using existing model
//=================================================================================
void DependencyTree_View::drawTree()
{
  clearView( false );
  clearSelected();

  // draw nodes on scene
  std::map< std::string, int > entryLevelMap;
  std::map< int, std::vector< std::string > > levelObjects;
  int currentLevel;
  int horDistance, verDistance;
  GEOMUtils::TreeModel::const_reverse_iterator i;
  for( i = myTreeModel.rbegin(); i != myTreeModel.rend(); i++ ) {
    currentLevel = 0;
    std::string objectEntry = i->first;
    DependencyTree_Object* objectItem = myTreeMap[ objectEntry ];
    horDistance = 100 + int( objectItem->boundingRect().width() );
    verDistance = 3 * int( objectItem->boundingRect().height() );
    if( isItemAdded( objectItem ) )
      currentLevel = entryLevelMap[ objectEntry ];
    else {
      addItem( objectItem );
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
    int step = -horDistance * ( int(level->second.size()) - 1 ) / 2;
    for( int objIter = 0; objIter < level->second.size(); objIter++ ) {
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
        for( node = Levelup.begin(); node != Levelup.end(); node++ ) {
          DependencyTree_Object* object = myTreeMap[ node->first ];
          DependencyTree_Arrow* arrow =
            myArrows[ std::pair<DependencyTree_Object*,DependencyTree_Object*>( object, Main_object )];
          if( arrow && !isItemAdded( arrow ) )
            addItem( arrow );
        }
      }
    }
    if( myDisplayAscendants->isChecked() )
      drawWardArrows( j->second.first );
    if( myDisplayDescendants->isChecked() )
      drawWardArrows( j->second.second );
  }
}

//=================================================================================
// function : drawWard()
// purpose  : draw nodes of dependency tree ward (ascendant or descendant)
//=================================================================================
void DependencyTree_View::drawWard( const GEOMUtils::LevelsList& theWard,
                                    std::map< std::string, int >& theEntryLevelMap,
                                    std::map< int, std::vector< std::string > >& theLevelObjects,
                                    int theCurrentLevel, const int theLevelStep )
{
  for( int level = 0; level < theWard.size(); level++ ) {
    if( level >= myLevelsNumber )
      return;
    theCurrentLevel += theLevelStep;
    GEOMUtils::LevelInfo levelInfo = theWard.at( level );
    GEOMUtils::LevelInfo::const_iterator node;
    for( node = levelInfo.begin(); node != levelInfo.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[ node->first ];
      if( object && !isItemAdded( object ) ) {
        addItem( object );
        object->unselect();
        theEntryLevelMap[ node->first ] = theCurrentLevel;
        theLevelObjects[ theCurrentLevel ].push_back( node->first );
      }
    }
  }
}

//=================================================================================
// function : drawWardArrows()
// purpose  : draw arrows of dependency tree ward (ascendant or descendant)
//=================================================================================
void DependencyTree_View::drawWardArrows( const GEOMUtils::LevelsList& theWard )
{
  for( int j = 0; j < theWard.size(); j++ ) {
    if( j >= myLevelsNumber )
      break;
    GEOMUtils::LevelInfo Level = theWard.at(j);
    GEOMUtils::LevelInfo::const_iterator node;
    for( node = Level.begin(); node != Level.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[ node->first ];
      GEOMUtils::NodeLinks Links = node->second;
      for( int link = 0; link < Links.size(); link++ ) {
        DependencyTree_Object* LinkObject = myTreeMap[ Links[ link ] ];
        if( isItemAdded( object ) && isItemAdded( LinkObject ) ) {
          DependencyTree_Arrow* arrow = myArrows[ std::pair<DependencyTree_Object*,DependencyTree_Object*>( LinkObject, object ) ];
          if( arrow && !isItemAdded( arrow ) )
            addItem( arrow );
        }
      }
    }
  }
}

//=================================================================================
// function : updateView()
// purpose  : update viewer using created dependency tree model
//=================================================================================
void DependencyTree_View::updateView()
{
  if( !myIsUpdate )
    return;

  drawTree();
  fitWindow();
}

//=================================================================================
// function : clearView()
// purpose  : clear viewer and initialize all variables
//=================================================================================
void DependencyTree_View::clearView( bool isClearModel )
{
  EntryObjectMap::const_iterator objectIter;
  for( objectIter = myTreeMap.begin(); objectIter != myTreeMap.end(); objectIter++ ) {
    if( DependencyTree_Object* object = objectIter->second )
      if( isItemAdded( object ) )
        removeItem( object );
  }

  ArrowsInfo::const_iterator arrowIter;
  for( arrowIter = myArrows.begin(); arrowIter != myArrows.end(); arrowIter++ ) {
    if( DependencyTree_Arrow* arrow = arrowIter->second )
      if( isItemAdded( arrow ) )
        removeItem( arrow );
  }

  if( isClearModel ) {
    EntryObjectMap::const_iterator objectIter;
    for( objectIter = myTreeMap.begin(); objectIter != myTreeMap.end(); objectIter++ ) {
      if( DependencyTree_Object* object = objectIter->second )
        delete object;
    }

    ArrowsInfo::const_iterator arrowIter;
    for( arrowIter = myArrows.begin(); arrowIter != myArrows.end(); arrowIter++ ) {
      if( DependencyTree_Arrow* arrow = arrowIter->second )
        delete arrow;
    }
    myTreeMap.clear();
    myArrows.clear();
    myTreeModel.clear();
    myLevelsNumber = 0;
    myMaxDownwardLevelsNumber = 0;
    myMaxUpwardLevelsNumber = 0;
    myIsUpdate = true;
  }
}

//=================================================================================
// function : fitWindow()
// purpose  : scale the window considering a size of scene
//=================================================================================
void DependencyTree_View::fitWindow()
{
  int sizeFactor = 4;
  if( objectsBoundingRect(true).width() > sizeFactor*size().width() ||
      objectsBoundingRect(true).height() > sizeFactor*size().width() ) {
    QRectF aRect = QRectF( -sizeFactor*size().width()/2, -sizeFactor*size().height()/2,
                           sizeFactor*size().width(), sizeFactor*size().height() );
    fitInView( aRect, Qt::KeepAspectRatio );
  }
  else
    fitAll();
}

//=================================================================================
// function : getNewTreeModel()
// purpose  : get dependency tree model from engine
//=================================================================================
void DependencyTree_View::getNewTreeModel( bool theUseSelectedObject, bool theUseOB )
{
  GEOM::string_array_var objectsEntry = new GEOM::string_array();
  int iter = 0;

  if( theUseSelectedObject ) {
    if( theUseOB ) {
      SALOME_ListIO mainObjects;
      mySelectionMgr->selectedObjects( mainObjects );
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
    }
    else {
      objectsEntry->length( nbSelected() );
      for( initSelected(); moreSelected(); nextSelected(), iter++ )
        if( DependencyTree_Object* treeObject = dynamic_cast<DependencyTree_Object*>( selectedObject() ) )
          objectsEntry[ iter ] = treeObject->getEntry().c_str();
    }
    myMainEntries = objectsEntry;
  }

  // get string which describes dependency tree structure
  SALOMEDS::TMPFile_var SeqFile =
    GeometryGUI::GetGeomGen()->GetDependencyTree( myStudy, myMainEntries );
  char* buf = (char*)&SeqFile[0];

  clearView( true );
  mySelectionMgr->clearSelected();

  // get dependency tree structure
  GEOMUtils::ConvertStringToTree( buf, myTreeModel );

  parseTree();
}

//=================================================================================
// function : checkMaxLevelsNumber()
// purpose  : calculate max levels number
//=================================================================================
int DependencyTree_View::checkMaxLevelsNumber()
{
  if( myDisplayAscendants->isChecked() && myDisplayDescendants->isChecked() )
    return myMaxUpwardLevelsNumber > myMaxDownwardLevelsNumber ?
           myMaxUpwardLevelsNumber : myMaxDownwardLevelsNumber;
  else if( myDisplayAscendants ->isChecked() )
    return myMaxUpwardLevelsNumber;
  else if( myDisplayDescendants->isChecked() )
    return  myMaxDownwardLevelsNumber;
}
