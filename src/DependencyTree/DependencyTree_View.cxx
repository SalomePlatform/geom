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

#include <GEOM_InteractiveObject.hxx>
#include <GeometryGUI.h>
#include <GEOMBase.h>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <QtxActionToolMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>

// Qt includes
#include <QCloseEvent>
#include <QTimerEvent>
#include <QGraphicsScene>


#include <iostream>
int iter = 0;

DependencyTree_View::DependencyTree_View( QWidget* theParent )
:GraphicsView_ViewPort(theParent),
myMaxDownwardLevelsNumber(0),
myMaxUpwardLevelsNumber(0),
myLevelsNumber(0),
myIsCompute(true),
myIsUpdate( true )
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  SALOMEDS::Study_var myStudy = GeometryGUI::ClientStudyToStudy( study->studyDS());

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  GEOM::string_array_var ObjectIORs = new GEOM::string_array();
    ObjectIORs->length( aSelList.Extent());
    int aaa=0;

   for ( SALOME_ListIteratorOfListIO It( aSelList ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();


    GEOM::GEOM_Object_var myObject = GEOM::GEOM_Object::_nil();
    myObject = GEOMBase::ConvertIOinGEOMObject( io );
    QString ior = myObject->GetEntry();
    ObjectIORs[aaa] = ior.toLatin1().constData();
    aaa++;

    std::cout << "\n\n IOR = " << ior.toStdString() << std::endl;
  }

  SALOMEDS::TMPFile_var SeqFile =
  GeometryGUI::GetGeomGen()->GetDependencyTree( myStudy, ObjectIORs );
  char* buf;
  buf = (char*) &SeqFile[0];

  std::cout << "\n\n\n\n\n TREE = " << buf << std::endl;

  GEOMUtils::ConvertStringToTree( buf, myTreeModel );

}

DependencyTree_View::~DependencyTree_View()
{

}

void DependencyTree_View::drawArrows()
{
	GEOMUtils::TreeModel::const_iterator i;
	for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
		DependencyTree_Object* Main_object = myTreeMap[i->first];
		GEOMUtils::LevelInfo Levelup = i->second.first.at(0);
		if( myDisplayAscendants ->isChecked() ) {
			GEOMUtils::LevelInfo::const_iterator node;
		for (node = Levelup.begin(); node != Levelup.end(); node++ ) {
		    DependencyTree_Object* object = myTreeMap[node->first];
		    DependencyTree_Arrow* arrow = Arrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>(Main_object, object)];
		    if( arrow && !isItemAdded( arrow) )
		      addItem( arrow );
		  }
		}
		if( myDisplayAscendants ->isChecked() )
       drawWardArrows( i->second.first );
		if( myDisplayDescendants->isChecked() )
       drawWardArrows( i->second.second );

	  }
}

void DependencyTree_View::drawWardArrows( GEOMUtils::LevelsList theWard )
{
	  for(int j = 0; j < theWard.size(); j++ ) {
		  if( j >= myLevelsNumber )
	        break;
		  GEOMUtils::LevelInfo Level = theWard.at(j);
		  GEOMUtils::LevelInfo::const_iterator node;
		  for (node = Level.begin(); node != Level.end(); node++ ) {
		    DependencyTree_Object* object = myTreeMap[node->first];
		    GEOMUtils::NodeLinks Links = node->second;
          for( int link = 0; link < Links.size(); link++ ) {
            DependencyTree_Object* LinkObject = myTreeMap[Links[link]];
            if( isItemAdded( object ) && isItemAdded( LinkObject ) ) {
    		    DependencyTree_Arrow* arrow = Arrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>(object, LinkObject)];
    		    if( arrow && !isItemAdded( arrow) )
    		      addItem( arrow );
               }
          }
		  }
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
    GEOMUtils::LevelInfo Levelup = i->second.first.at(0);
    GEOMUtils::LevelInfo::const_iterator node;
    for (node = Levelup.begin(); node != Levelup.end(); node++ ) {
      DependencyTree_Object* object = myTreeMap[node->first];
      addArrow( Main_object, object );
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
//void DependencyTree_View::parseData( QString& theData )
//{
//	  int cursor = 0;
//
//	  while( theData.indexOf('-',cursor) != -1 ) //find next selected object
//	  {
//	    int objectIndex = theData.indexOf( '-', cursor );
//	    QString objectEntry = theData.mid( cursor, objectIndex - cursor );
//	    //addNode( objectEntry );
//	    std::cout<<"\n\nMainObject = " << objectEntry.toStdString() <<std::endl;
//	    cursor = objectIndex;
//
//	    int upwardIndexBegin = theData.indexOf("{",cursor) + 1;
//	    int upwardIndexFinish = theData.indexOf("}",upwardIndexBegin);
//	    std::cout<<"  Upward:" << std::endl;
//	    LevelsList upwardList = parseWard( theData, cursor );
//	    if( upwardList.size() > myMaxUpwardLevelsNumber )
//	      myMaxUpwardLevelsNumber = upwardList.size();
//	    std::cout<<"  Downward:" << std::endl;
//	    LevelsList downwardList = parseWard( theData, cursor );
//	    if( downwardList.size() > myMaxDownwardLevelsNumber )
//	      myMaxDownwardLevelsNumber = downwardList.size();
//
//	    myTreeModel[objectEntry] = QPair<LevelsList,LevelsList>( upwardList, downwardList );
//	  }
//}

void DependencyTree_View::addNode( const std::string& theEntry )
{
  if( !myTreeMap[theEntry] )
    myTreeMap[theEntry] = new DependencyTree_Object( theEntry );
}

void DependencyTree_View::addArrow( DependencyTree_Object *startItem, DependencyTree_Object *endItem )
{
  bool isFind = false;

  std::cout << "   " << startItem->getEntry() << "   " << endItem->getEntry() << std::endl;
  std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator i;
  for (i = Arrows.begin(); i != Arrows.end(); i++ ) {
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
	    //Arrows.append( arrow );
	    //addItem(arrow);
	    Arrows[std::pair<DependencyTree_Object*,DependencyTree_Object*>( startItem, endItem )] = arrow;
	    std::cout<<" addArrow " << std::endl;
  }
}

//DependencyTree_View::LevelsList DependencyTree_View::parseWard( const QString& theData, int& theCursor )
//{
//	  int indexStart = theData.indexOf( "{", theCursor ) + 1;
//	  int indexEnd = theData.indexOf( "}", indexStart );
//
//	  QString ward = theData.mid( indexStart, indexEnd - indexStart );
//	  QStringList levelsListStr = ward.split( ';' );
//	  LevelsList levelsListData;
//	  for( int level = 0; level < levelsListStr.size(); level++ ) {
//	    std::cout<<"    Level" << level + 1 << ":" << std::endl;
//	    QStringList namesListStr = levelsListStr[level].split( ',' );
//	    LevelInfo levelInfoData;
//	    for( int node = 0; node < namesListStr.size(); node++ ) {
//	      QStringList linksListStr = namesListStr[node].split( '_' );
//	      QString nodeItem = linksListStr[0];
//	      if( !nodeItem.isEmpty() ) {
//	        //addNode( nodeItem );
//	        NodeLinks linksListData;
//	        std::cout<<"      " << nodeItem.toStdString() << " - ";
//	        for( int link = 1; link < linksListStr.size(); link++ ) {
//	          QString linkItem = linksListStr[link];
//	          //addNode( linkItem );
//	          linksListData.append( linkItem );
//	          std::cout << linkItem.toStdString() << ", ";
//	        }// Links
//	        levelInfoData[nodeItem] = linksListData;
//	        std::cout << std::endl;
//	      }
//	    }// Level's objects
//	    levelsListData.append(levelInfoData);
//	  }// Levels
//
//	  theCursor = indexEnd + 1;
//
//	  return levelsListData;
//}

void DependencyTree_View::drawTree()
{

  int horDistance, verDistance;
  myCurrentLevel = 0;
    GEOMUtils::TreeModel::const_iterator i;
    for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
      myCurrentLevel = 0;
      std::string objectEntry = i->first;
      DependencyTree_Object* objectItem = myTreeMap[ objectEntry ];
      horDistance = 100 + int( objectItem->boundingRect().width() );
      verDistance = 3 * int( objectItem->boundingRect().height() );
      if( isItemAdded( objectItem ) ) {
        myCurrentLevel = myLevelMap[ objectEntry ];
      }
      else {
        addItem( objectItem );
        myLevelMap[ objectEntry ] = myCurrentLevel;
        myLevelsObject[ myCurrentLevel ].push_back( objectEntry );
      }
      objectItem->setIsMainObject( true );

      int levelposition = myCurrentLevel;
      if( myDisplayAscendants ->isChecked() ){
        drawWard( i->second.first, -1 );
        myCurrentLevel = levelposition;
      }
      if( myDisplayDescendants->isChecked() )
        drawWard( i->second.second, 1 );
      }

		std::map< int, std::vector<std::string> >::const_iterator j;
		for (j = myLevelsObject.begin(); j != myLevelsObject.end(); j++ ) {
		  int step = -horDistance*( j->second.size() - 1 )/2;
			std::cout<<"\n\n LEVEL = " << j->first << std::endl;
		  for( int object = 0; object < j->second.size(); object++ ) {
			 if( myIsCompute ) {
			  std::cout << j->second.at( object ) << ", ";
			 DependencyTree_Object* anObject = myTreeMap[ j->second.at( object ) ];
			 anObject->setPos( step, verDistance*j->first );
			 step += horDistance;
			 //sleep(1);
			 }
		  }
		}
  centerOn( scene()->sceneRect().center() );

}

void DependencyTree_View::drawWard( const GEOMUtils::LevelsList theWard, const int theLevelStep )
{
	std::cout << "\n\n myLevelsNumber2 = " << myLevelsNumber << std::endl;
	  int levelsNumber = theWard.size();
	std::cout << "\n\n levelsNumber = " << levelsNumber << std::endl;
	  for( int level = 0; level < levelsNumber; level++ ) {
		  if( level >= myLevelsNumber )
	        return;
	    myCurrentLevel += theLevelStep;
	    GEOMUtils::LevelInfo levelInfo = theWard.at( level );
	    GEOMUtils::LevelInfo::const_iterator node;
		for (node = levelInfo.begin(); node != levelInfo.end(); node++ ) {
	      DependencyTree_Object* object = myTreeMap[ node->first ];
	      if( !isItemAdded( object ) ) {
	    	std::cout<< "\n\n\n addItem = " << object->getEntry() << std::endl;
	        addItem( object );
	        myLevelMap[ node->first ] = myCurrentLevel;
	        myLevelsObject[ myCurrentLevel ].push_back( node->first );
	      }
	    }
	 }

}

void DependencyTree_View::onUpdateTree()
{
  myLevelMap.clear();
  myLevelsObject.clear();


  std::map<std::string,DependencyTree_Object*>::const_iterator i;
	for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
		DependencyTree_Object* object = myTreeMap[ i->first ];
		if( isItemAdded( object ) && object )
          removeItem( object );

	}

   std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator j;
   for (j = Arrows.begin(); j != Arrows.end(); j++ ) {
     DependencyTree_Arrow* object = Arrows[ j->first ];
     if( isItemAdded( object ) && object )
       removeItem( object );
   }

  drawTree();
  drawArrows();
}


int DependencyTree_View::checkMaxLevelsNumber()
{
  if( myDisplayAscendants ->isChecked() && myDisplayDescendants->isChecked() )
    return myMaxUpwardLevelsNumber>myMaxDownwardLevelsNumber?myMaxUpwardLevelsNumber:myMaxDownwardLevelsNumber;
  else if( myDisplayAscendants ->isChecked() )
    return myMaxUpwardLevelsNumber;
  else if( myDisplayDescendants->isChecked() )
    return  myMaxDownwardLevelsNumber;
}


void DependencyTree_View::init( GraphicsView_ViewFrame* theViewFrame )
{
	  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

	  std::string Data = "MainObject1-upward{up11_up21_up22,up12_up23,up13_up24;up21_up11,up22_up21_up12,up23_up23,up24}"
			         "downward{down11_MainObject1,down12_MainObject1;down21_down11,down22_down12,down23_down12,down24_down24_down11,down25_down12;down31_down11_down23,down32_down25}"
			         "down23-upward{down12_MainObject1;MainObject1_up11_up12_up13;up11_up21_up22,up12_up23,up13_up24;up21,up22_up21_up12,up23,up24}"
			         "downward{down31_down11_down23}"
			         "MainObject2-upward{up21,newup11_newup31,newup12_newup21;newup21_newup31LongName;newup31LongName}downward{newdown11_MainObject2,newdown12_MainObject2,newdown13_MainObject2;newdown21_newdown13,down21;newdown31_newdown11}"
			         "MainObject3-upward{moUP1_moUP4,moUP2_moUP3;moUP4,moUP3_down23}downward{moDOWN1_MainObject3}";


	  //GEOMUtils::ConvertStringToTree( Data, myTreeModel );



//	  GEOMUtils::TreeModel::const_iterator i;
//	  for ( i = aTreeModel.begin(); i != aTreeModel.end(); i++ ) {
//	    std::string objectEntry = i->first;
//	    std::cout << "\n\n Main object = " << objectEntry << std::endl;
//	  }
//  parseData( Data );
  parseTree();


  myNodesMovable = new QCheckBox( tr( "MOVE_NODES" ) );
  QWidgetAction* moveNodesAction = new QWidgetAction( theViewFrame );
  moveNodesAction->setDefaultWidget( myNodesMovable );

  myDisplayAscendants  = new QCheckBox( tr( "DISPLAY_ASCENDANTS" ));
  myDisplayAscendants ->setChecked( true );
  QWidgetAction* ShowParentsAction = new QWidgetAction(theViewFrame);
  ShowParentsAction->setDefaultWidget( myDisplayAscendants  );

  myDisplayDescendants = new QCheckBox(tr("DISPLAY_DESCENDANTS"));
  QWidgetAction* ShowChildrenAction = new QWidgetAction(theViewFrame);
  ShowChildrenAction->setDefaultWidget( myDisplayDescendants );

  myLevelsNumber = checkMaxLevelsNumber();
  std::cout << "\n\n myLevelsNumber1 = " << myLevelsNumber << std::endl;
  myHierarchyDepth = new QSpinBox();
  myHierarchyDepth->setRange( 0, checkMaxLevelsNumber() );
  myHierarchyDepth->setValue( 0 );
  myHierarchyDepth->setSpecialValueText( "All" );
  QWidgetAction* LevelsAction = new QWidgetAction(theViewFrame);
  LevelsAction->setDefaultWidget( myHierarchyDepth );

  QLabel* LevelsLabel = new QLabel( tr("HIERARCHY_DEPTH") );
  QWidgetAction* LevelsLebelAction = new QWidgetAction(theViewFrame);
  LevelsLebelAction->setDefaultWidget( LevelsLabel );


  QPushButton* UpdateButton = new QPushButton(tr("Update"));
  QWidgetAction* UpdateAction = new QWidgetAction(theViewFrame);
  UpdateAction->setDefaultWidget( UpdateButton );

  cancelButton = new QPushButton(tr("CANCEL"));
  cancelButton->setCheckable(true);
  cancelButton->setVisible( false );
  cancelAction = new QWidgetAction(theViewFrame);
  cancelAction->setDefaultWidget( cancelButton );
  cancelAction->setVisible(false);

  progressBar  = new QProgressBar(this);
  progressBar->setMinimum( 0 );
  progressBar->setMaximum( 1000 );
  progressBar->setFixedWidth(100);
  progressAction = new QWidgetAction(theViewFrame);
  progressAction->setDefaultWidget( progressBar );
  progressAction->setVisible(false);

  QAction* separator = theViewFrame->toolMgr()->separator(false);
  theViewFrame->toolMgr()->append( separator, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( moveNodesAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( LevelsLebelAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( LevelsAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( ShowParentsAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( ShowChildrenAction, theViewFrame->getToolBarId() );
  QAction* separator2 = theViewFrame->toolMgr()->separator(false);
  theViewFrame->toolMgr()->append( separator2, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( UpdateAction, theViewFrame->getToolBarId() );
  std::cout<<"\n\n\n\n ToolBarId = " << theViewFrame->getToolBarId() << std::endl;
  theViewFrame->toolMgr()->append( progressAction, theViewFrame->getToolBarId() );
  theViewFrame->toolMgr()->append( cancelAction, theViewFrame->getToolBarId() );

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

  connect( myHierarchyDepth, SIGNAL( valueChanged ( int ) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayAscendants , SIGNAL( toggled(bool) ), this, SLOT( onHierarchyType() ) );
  connect( myDisplayDescendants, SIGNAL( toggled(bool) ), this, SLOT( onHierarchyType() ) );
  connect( UpdateButton, SIGNAL( clicked() ), this, SLOT( updateView() ) );
  connect( myNodesMovable, SIGNAL( toggled(bool) ), this, SLOT( onMoveNodes( bool ) ) );

  setPrefBackgroundColor( resMgr->colorValue( "Geometry", "dependency_tree_background_color", QColor( 255, 255, 255 ) ) );
  setNodesMovable( resMgr->booleanValue( "Geometry", "dependency_tree_move_nodes", true ) );
  setHierarchyType( resMgr->integerValue( "Geometry", "dependency_tree_hierarchy_type", 0 ) );

}

void DependencyTree_View::onMoveNodes( bool theIsMoveNodes )
{
  std::map<std::string,DependencyTree_Object*>::const_iterator i;
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
void DependencyTree_View::updateView()
{

  if( !myIsUpdate )
    return;

	  myLevelMap.clear();
	  myLevelsObject.clear();


	  std::map<std::string,DependencyTree_Object*>::const_iterator i;
		for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
			DependencyTree_Object* object = myTreeMap[ i->first ];
			if( isItemAdded( object ) && object )
	          removeItem( object );

		}


		   std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator j;
		   for (j = Arrows.begin(); j != Arrows.end(); j++ ) {
		     DependencyTree_Arrow* object = Arrows[ j->first ];
		     if( isItemAdded( object ) && object )
		       removeItem( object );
		   }




	iter = 0;
  qthread = new DependencyTree_ComputeDlg_QThread(this);// = DependencyTree_ComputeDlg_QThread(this);// const_cast<DependencyTree_View*>(this) );
  cancelAction->setVisible( true );
  progressAction->setVisible(true);
  startTimer(100); // millisecs
  qthread->start();
}

void DependencyTree_View::onRedrawTree()
{
  drawTree();
  drawArrows();
}
void DependencyTree_View::onCancel()
{
  qthread->cancel();
  cancelButton->setText( tr("CANCELING"));
  cancelButton->setEnabled(false);
}

void DependencyTree_View::timerEvent(QTimerEvent *event)
{
  if ( !cancelButton->isChecked() ) // not yet cancelled
   // progressBar->setValue( progressBar->maximum() * qthread.getMesh()->GetComputeProgress() );
	  progressBar->setValue( iter++ );

  if(!myIsCompute || qthread->isFinished())
  {
	  cancelAction->setVisible( false );
	  progressAction->setVisible(false);
	  cancelButton->setText( tr("CANCEL"));
	  cancelButton->setEnabled(true);
  }

  event->accept();
}

void DependencyTree_View::closeEvent(QCloseEvent *event)
{
  if(qthread->isRunning())
  {
      event->ignore();
      return;
    }
  event->accept();
}

void DependencyTree_View::setIsCompute( bool theIsCompute )
{
  myIsCompute = theIsCompute;
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
  std::map<std::string,DependencyTree_Object*>::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setColor( theColor );
  }
}

void DependencyTree_View::setMainNodeColor( const QColor& theColor )
{
	  std::map<std::string,DependencyTree_Object*>::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setMainObjectColor( theColor );
  }
}

void DependencyTree_View::setSelectNodeColor( const QColor& theColor )
{
	  std::map<std::string,DependencyTree_Object*>::const_iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i->first ];
    object->setSelectColor( theColor );
  }
}

void DependencyTree_View::setArrowColor( const QColor& theColor )
{
  std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator j;
  for (j = Arrows.begin(); j != Arrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = Arrows[ j->first ];
    arrow->setColor( theColor );
  }
}

void DependencyTree_View::setHighlightArrowColor( const QColor& theColor )
{
	  std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator j;
  for (j = Arrows.begin(); j != Arrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = Arrows[ j->first ];
    arrow->setHighlightColor( theColor );
  }
}

void DependencyTree_View::setSelectArrowColor( const QColor& theColor )
{
	  std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>, DependencyTree_Arrow* >::const_iterator j;
  for (j = Arrows.begin(); j != Arrows.end(); j++ ) {
    DependencyTree_Arrow* arrow = Arrows[ j->first ];
    arrow->setSelectColor( theColor );
  }
}

//================================================================================
//================================================================================

DependencyTree_ComputeDlg_QThread::DependencyTree_ComputeDlg_QThread(DependencyTree_View* theView)
{
  myView = theView;
}

void DependencyTree_ComputeDlg_QThread::run()
{
  myView->setIsCompute( true );
  myView->onRedrawTree();
}

bool DependencyTree_ComputeDlg_QThread::result()
{

}

void DependencyTree_ComputeDlg_QThread::cancel()
{
  myView->setIsCompute( false );
}

////================================================================================
////================================================================================
//
//DependencyTree_ComputeDlg_QThreadQDialog::DependencyTree_ComputeDlg_QThreadQDialog(QWidget* parent,
//		DependencyTree_View* theView)
//  : QDialog(parent,
//            Qt::WindowSystemMenuHint |
//            Qt::WindowCloseButtonHint |
//            Qt::Dialog |
//            Qt::WindowMaximizeButtonHint),
//    qthread( theView )
//{
//  // --
//  setWindowTitle(tr("TITLE"));
//  setMinimumWidth( 200 );
//
//  cancelButton = new QPushButton(tr("CANCEL"));
//  cancelButton->setDefault(true);
//  cancelButton->setCheckable(true);
//
//  progressBar  = new QProgressBar(this);
//  progressBar->setMinimum( 0 );
//  progressBar->setMaximum( 1000 );
//
//  QGridLayout* layout = new QGridLayout(this);
//  layout->setMargin( 11 );
//  layout->setSpacing( 6 );
//  int row = 0;
//  layout->addWidget(progressBar,  row++, 0, 1, 2);
//  layout->addWidget(cancelButton, row++, 0, 1, 2);
//  adjustSize();
//  update();
//
//  connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));
//  // --
//  startTimer(300); // millisecs
//  qthread.start();
//}
//
//bool DependencyTree_ComputeDlg_QThreadQDialog::result()
//{
//  return qthread.result();
//}
//
//void DependencyTree_ComputeDlg_QThreadQDialog::onCancel()
//{
//  qthread.cancel();
//  cancelButton->setText( tr("CANCELING"));
//  cancelButton->setEnabled(false);
//}
//
//void DependencyTree_ComputeDlg_QThreadQDialog::timerEvent(QTimerEvent *event)
//{
//  if ( !cancelButton->isChecked() ) // not yet cancelled
//   // progressBar->setValue( progressBar->maximum() * qthread.getMesh()->GetComputeProgress() );
//	  progressBar->setValue( 10 );
//
//  if(qthread.isFinished())
//  {
//    close();
//  }
//
//  event->accept();
//}
//
//void DependencyTree_ComputeDlg_QThreadQDialog::closeEvent(QCloseEvent *event)
//{
//  if(qthread.isRunning())
//  {
//      event->ignore();
//      return;
//    }
//  event->accept();
//}
//


