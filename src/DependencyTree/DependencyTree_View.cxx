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

#include <GraphicsView_ViewPort.h>
#include <GraphicsView_Scene.h>
#include <GraphicsView_ViewFrame.h>


#include "DependencyTree_View.h"
#include "DependencyTree_Object.h"
#include "DependencyTree_Arrow.h"


#include <QMenu>
#include <QList>
#include <QString>

#include <QGraphicsView>
#include <QSpinBox>

#include <QWidgetAction>

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QCloseEvent>
#include <QTimerEvent>

#include <QtxActionToolMgr.h>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <iostream>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIteratorOfListIO.hxx>
#include <GEOM_InteractiveObject.hxx>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include "GEOMUtils.hxx"




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

  _PTR(Study) aStudy = study->studyDS();
  if ( !aStudy )
    return;

  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy(aStudy);

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;
  
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  GEOM::string_array_var ObjectIORs = new GEOM::string_array();
    ObjectIORs->length( aSelList.Extent());
    int aaa=0;

   for ( SALOME_ListIteratorOfListIO It( aSelList ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
//    if ( io->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
//    Handle(GEOM_InteractiveObject) objectGEOM = Handle(GEOM_InteractiveObject)::DownCast( io );

//    const char* ior = objectGEOM->getIOR();
//    objectGEOM.getEntry();
//    objectGEOM.getName();
//    std::cout << "\n\n\n !!!!! IOR = " << ior << std::endl;
//    }

    GEOM::GEOM_Object_var myObject = GEOM::GEOM_Object::_nil();
    myObject = GEOMBase::ConvertIOinGEOMObject( io );
    //QString ior = GEOMBase::GetIORFromObject(myObject);
    QString ior = myObject->GetEntry();
    ObjectIORs[aaa] = ior.toLatin1().constData();
    aaa++;

    std::cout << "\n\n IOR = " << ior.toStdString() << std::endl;
//    if ( !CORBA::is_nil( myObject ) )
//      myGrp->LineEdit1->setText( GEOMBase::GetName( myObject ) );
  }
//
//  SALOME_ListIO selected;
//  aSelMgr->selectedObjects( selected );
//  if ( selected.IsEmpty() ) return;
//
//  GEOM::string_array ObjectIORs;
//  ObjectIORs.lenght( selected.Extent())
//  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
//    Handle( SALOME_InteractiveObject ) io = It.Value();
//    const char* entry = io->getEntry();
//    ObjectIORs.push_back( entry );
//    //CORBA::String_var IOR = app->orb()->object_to_string( io );
//        //if ( strcmp(IOR.in(), "") != 0 )
//        //{
////    QString objIOR = GEOMBase::GetIORFromObject( io );
//    //char* GetStringFromIOR(GEOM::GEOM_Object_ptr theObject)
//
//  }

  // get dependencies tree as a stream
  SALOMEDS::TMPFile_var SeqFile =
    GeometryGUI::GetGeomGen()->GetDependencyTree( aStudyDS, ObjectIORs );
  // convert stream into string
  char* treeStr;
  treeStr = (char*) &SeqFile[0];

  std::cout << "\n TREE = " << treeStr << std::endl;

  // parse string to deal with 'TreeModel' object
  GEOMUtils::TreeModel tree;
  GEOMUtils::ConvertStringToTree( treeStr, tree );
}

DependencyTree_View::~DependencyTree_View()
{

}

void DependencyTree_View::drawArrows()
{
	QMap<QString,QPair<LevelsList,LevelsList> >::iterator i;
	for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
		DependencyTree_Object* Main_object = myTreeMap[i.key()];
		LevelInfo Levelup = i.value().first.at(0);
		if( myDisplayAscendants ->isChecked() ) {
		QMap<QString, NodeLinks>::iterator node;
		for (node = Levelup.begin(); node != Levelup.end(); node++ ) {
		    DependencyTree_Object* object = myTreeMap[node.key()];
		    addArrow(Main_object, object);
		  }
		}
		if( myDisplayAscendants ->isChecked() )
       drawWardArrows( i.value().first );
		if( myDisplayDescendants->isChecked() )
       drawWardArrows( i.value().second );

	  }
}

void DependencyTree_View::drawWardArrows( LevelsList theWard )
{
	  for(int j = 0; j < theWard.size(); j++ ) {
		  if( j >= myLevelsNumber )
	        break;
		  LevelInfo Level = theWard.at(j);
		  QMap<QString, NodeLinks>::iterator node;
		  for (node = Level.begin(); node != Level.end(); node++ ) {
		    DependencyTree_Object* object = myTreeMap[node.key()];
          QList<QString> Links = node.value();
          for( int link = 0; link < Links.size(); link++ ) {
            DependencyTree_Object* LinkObject = myTreeMap[Links[link]];
            if( isItemAdded( object ) && isItemAdded( LinkObject ) ) {
              addArrow(object, LinkObject);
            }
          }
		  }
	  }
}


void DependencyTree_View::parseData( QString& theData )
{
	  int cursor = 0;

	  while( theData.indexOf('-',cursor) != -1 ) //find next selected object
	  {
	    int objectIndex = theData.indexOf( '-', cursor );
	    QString objectEntry = theData.mid( cursor, objectIndex - cursor );
	    addNode( objectEntry );
	    std::cout<<"\n\nMainObject = " << objectEntry.toStdString() <<std::endl;
	    cursor = objectIndex;

	    int upwardIndexBegin = theData.indexOf("{",cursor) + 1;
	    int upwardIndexFinish = theData.indexOf("}",upwardIndexBegin);
	    std::cout<<"  Upward:" << std::endl;
	    LevelsList upwardList = parseWard( theData, cursor );
	    if( upwardList.size() > myMaxUpwardLevelsNumber )
	      myMaxUpwardLevelsNumber = upwardList.size();
	    std::cout<<"  Downward:" << std::endl;
	    LevelsList downwardList = parseWard( theData, cursor );
	    if( downwardList.size() > myMaxDownwardLevelsNumber )
	      myMaxDownwardLevelsNumber = downwardList.size();

	    myTreeModel[objectEntry] = QPair<LevelsList,LevelsList>( upwardList, downwardList );
	  }
}

void DependencyTree_View::addNode( const QString& theEntry )
{
  if( !myTreeMap[theEntry] )
    myTreeMap[theEntry] = new DependencyTree_Object( theEntry );
}

void DependencyTree_View::addArrow( DependencyTree_Object *startItem, DependencyTree_Object *endItem )
{
  bool isFind = false;

  std::cout << "   " << startItem->getEntry().toStdString() << "   " << endItem->getEntry().toStdString() << std::endl;
  for( int i = 0; i < Arrows.size(); i++ ) {
	  DependencyTree_Arrow* arrow = Arrows.at(i);
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
	    Arrows.append( arrow );
	    addItem(arrow);
	    std::cout<<" addArrow " << std::endl;
  }
}

DependencyTree_View::LevelsList DependencyTree_View::parseWard( const QString& theData, int& theCursor )
{
	  int indexStart = theData.indexOf( "{", theCursor ) + 1;
	  int indexEnd = theData.indexOf( "}", indexStart );

	  QString ward = theData.mid( indexStart, indexEnd - indexStart );
	  QStringList levelsListStr = ward.split( ';' );
	  LevelsList levelsListData;
	  for( int level = 0; level < levelsListStr.size(); level++ ) {
	    std::cout<<"    Level" << level + 1 << ":" << std::endl;
	    QStringList namesListStr = levelsListStr[level].split( ',' );
	    LevelInfo levelInfoData;
	    for( int node = 0; node < namesListStr.size(); node++ ) {
	      QStringList linksListStr = namesListStr[node].split( '_' );
	      QString nodeItem = linksListStr[0];
	      if( !nodeItem.isEmpty() ) {
	        addNode( nodeItem );
	        NodeLinks linksListData;
	        std::cout<<"      " << nodeItem.toStdString() << " - ";
	        for( int link = 1; link < linksListStr.size(); link++ ) {
	          QString linkItem = linksListStr[link];
	          addNode( linkItem );
	          linksListData.append( linkItem );
	          std::cout << linkItem.toStdString() << ", ";
	        }// Links
	        levelInfoData[nodeItem] = linksListData;
	        std::cout << std::endl;
	      }
	    }// Level's objects
	    levelsListData.append(levelInfoData);
	  }// Levels

	  theCursor = indexEnd + 1;

	  return levelsListData;
}

void DependencyTree_View::drawTree()
{

	int horDistance, verDistance;
		myCurrentLevel = 0;
		std::cout<<"\n\n\n\n MY TREE MODEL SIZE = " << myTreeModel.size() << std::endl;
		std::cout<<"\n\n\n\n MY TREE MAP SIZE = " << myTreeMap.size() << std::endl;
		for( int j = 0; j < myTreeModel.keys().size(); j++ )
		  std::cout<< " STRING = " << myTreeModel.keys().at(j).toStdString() << std::endl;
		QMap<QString,QPair<LevelsList,LevelsList> >::iterator i;
		for (i = myTreeModel.begin(); i != myTreeModel.end(); i++ ) {
	      myCurrentLevel = 0;
		  QString objectEntry = i.key();
		  std::cout<< " NAME = " << objectEntry.toStdString() << std::endl;
		  DependencyTree_Object* objectItem = myTreeMap[ objectEntry ];
		  objectItem->setEnabled(true);
		  if( objectItem->isEnabled() )
			  std::cout<<"OK!!! "<< std::endl;
		  horDistance = 100 + int( objectItem->boundingRect().width() );
		  verDistance = 3 * int( objectItem->boundingRect().height() );
		  if( isItemAdded( objectItem ) ) {
		    myCurrentLevel = myLevelMap[ objectEntry ];
		  }
		  else {
		    addItem( objectItem );
		    std::cout<<"\nLevel = " << myCurrentLevel << " Object = " << objectEntry.toStdString() << std::endl;
		    myLevelMap[ objectEntry ] = myCurrentLevel;
		    myLevelsObject[ myCurrentLevel ].append( objectEntry );
		  }
		  objectItem->setIsMainObject( true );

		  int levelposition = myCurrentLevel;
		  if( myDisplayAscendants ->isChecked() ){
		    drawWard( i.value().first, -1 );
		    myCurrentLevel = levelposition;
		  }
		  if( myDisplayDescendants->isChecked() )
		    drawWard( i.value().second, 1 );

		    //centerOn( objectItem );
		  }

		QMap< int, QList<QString> >::iterator j;
		for (j = myLevelsObject.begin(); j != myLevelsObject.end(); j++ ) {
		  int step = -horDistance*( j.value().size() - 1 )/2;
			std::cout<<"\n\n LEVEL = " << j.key() << std::endl;
		  for( int object = 0; object < j.value().size(); object++ ) {
			 if( myIsCompute ) {
			  std::cout << j.value().at( object ).toStdString() << ", ";
			 DependencyTree_Object* anObject = myTreeMap[ j.value().at( object ) ];
			 anObject->setPos( step, verDistance*j.key() );
			 step += horDistance;
			 //sleep(1);
			 }
		  }
		}

}

void DependencyTree_View::drawWard( const DependencyTree_View::LevelsList theWard, const int theLevelStep )
{
	  int levelsNumber = theWard.size();
	  for( int level = 0; level < levelsNumber; level++ ) {
		  if( level >= myLevelsNumber )
	        return;
	    myCurrentLevel += theLevelStep;
	    LevelInfo levelInfo = theWard.at( level );
	    QMap<QString, NodeLinks>::iterator node;
		for (node = levelInfo.begin(); node != levelInfo.end(); node++ ) {
	      DependencyTree_Object* object = myTreeMap[ node.key() ];
	      if( !isItemAdded( object ) ) {
	        addItem( object );
	        myLevelMap[ node.key() ] = myCurrentLevel;
	        myLevelsObject[ myCurrentLevel ].append( node.key() );
	      }
	    }
	 }
}

void DependencyTree_View::onUpdateTree()
{
  myLevelMap.clear();
  myLevelsObject.clear();


  QMap<QString,DependencyTree_Object*>::iterator i;
	for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
		DependencyTree_Object* object = myTreeMap[ i.key() ];
		if( isItemAdded( object ) && object )
          removeItem( object );

	}


	std::cout<<"\n\n\n\n ARROWS = " << Arrows.size() << std::endl;

		for(int j = 0; j<Arrows.size();j++)
			if( isItemAdded( Arrows[j]) )
				removeItem( Arrows[j] );

		Arrows.clear();

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
	  QString Data = "MainObject1-upward{up11_up21_up22,up12_up23,up13_up24;up21_up11,up22_up21_up12,up23_up23,up24}"
			         "downward{down11_MainObject1,down12_MainObject1;down21_down11,down22_down12,down23_down12,down24_down24_down11,down25_down12;down31_down11_down23,down32_down25}"
			         "down23-upward{down12_MainObject1;MainObject1_up11_up12_up13;up11_up21_up22,up12_up23,up13_up24;up21,up22_up21_up12,up23,up24}"
			         "downward{down31_down11_down23}"
			         "MainObject2-upward{up21,newup11_newup31,newup12_newup21;newup21_newup31LongName;newup31LongName}downward{newdown11_MainObject2,newdown12_MainObject2,newdown13_MainObject2;newdown21_newdown13,down21;newdown31_newdown11}";

  parseData( Data );


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
  QMap<QString,DependencyTree_Object*>::iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i.key() ];
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


	  QMap<QString,DependencyTree_Object*>::iterator i;
		for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
			DependencyTree_Object* object = myTreeMap[ i.key() ];
			if( isItemAdded( object ) && object )
	          removeItem( object );

		}


		std::cout<<"\n\n\n\n ARROWS = " << Arrows.size() << std::endl;

			for(int j = 0; j<Arrows.size();j++)
				if( isItemAdded( Arrows[j]) )
					removeItem( Arrows[j] );

			Arrows.clear();




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
  QMap<QString,DependencyTree_Object*>::iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i.key() ];
    object->setColor( theColor );
  }
}

void DependencyTree_View::setMainNodeColor( const QColor& theColor )
{
  QMap<QString,DependencyTree_Object*>::iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i.key() ];
    object->setMainObjectColor( theColor );
  }
}

void DependencyTree_View::setSelectNodeColor( const QColor& theColor )
{
  QMap<QString,DependencyTree_Object*>::iterator i;
  for (i = myTreeMap.begin(); i != myTreeMap.end(); i++ ) {
    DependencyTree_Object* object = myTreeMap[ i.key() ];
    object->setSelectColor( theColor );
  }
}

void DependencyTree_View::setArrowColor( const QColor& theColor )
{
  for( int i = 0; i < Arrows.size(); i++ ) {
    DependencyTree_Arrow* arrow = Arrows[i];
    arrow->setColor( theColor );
  }
}

void DependencyTree_View::setHighlightArrowColor( const QColor& theColor )
{
  for( int i = 0; i < Arrows.size(); i++ ) {
    DependencyTree_Arrow* arrow = Arrows[i];
    arrow->setHighlightColor( theColor );
  }
}

void DependencyTree_View::setSelectArrowColor( const QColor& theColor )
{
  for( int i = 0; i < Arrows.size(); i++ ) {
    DependencyTree_Arrow* arrow = Arrows[i];
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


