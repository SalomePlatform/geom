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

#include "DependencyTree_ViewModel.h"

#include "DependencyTree_View.h"
#include "GEOM_Displayer.h"

#include <GraphicsView_Viewer.h>
#include <GraphicsView_ViewPort.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>
#include <SUIT_Session.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>

#include <QWidget>
#include <QMenu>
#include <iostream>

DependencyTree_ViewModel::DependencyTree_ViewModel( const QString& title )
: GraphicsView_Viewer( title )
{
}

DependencyTree_ViewModel::DependencyTree_ViewModel( const QString& title, QWidget* w )
: GraphicsView_Viewer( title,  w )
{

}

DependencyTree_ViewModel::~DependencyTree_ViewModel()
{
}

void activateOCCViewer() {
}

void DependencyTree_ViewModel::onShowSelected()
{
  std::cout<<"\n\n\n\n *****onShowSelected " << std::endl;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  app->getViewManager(OCCViewer_Viewer::Type(), /*create=*/true);

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  GEOM_Displayer* disp = new GEOM_Displayer( appStudy );

  OCCViewer_ViewManager* anOCCVM = (OCCViewer_ViewManager*) app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/true );

  if ( SUIT_ViewModel* vmod = anOCCVM->getViewModel() ) {
    if ( SALOME_View* aViewFrame = dynamic_cast<SALOME_View*>( vmod ) ) {
      SALOME_ListIteratorOfListIO Iter( aSelList );
      for ( ; Iter.More(); Iter.Next() ) {
	disp->Display( Iter.Value(), false, aViewFrame );
      }
      aViewFrame->Repaint();
    }
  }
}

void DependencyTree_ViewModel::onShowOnlySelected()
{
  std::cout<<"\n\n\n\n *****onShowOnlySelected " << std::endl;
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  GEOM_Displayer* disp = new GEOM_Displayer( appStudy );

  OCCViewer_ViewManager* anOCCVM = (OCCViewer_ViewManager*) app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/true );

  if ( SUIT_ViewModel* vmod = anOCCVM->getViewModel() ) {
    if ( SALOME_View* aViewFrame = dynamic_cast<SALOME_View*>( vmod ) ) {
      disp->EraseAll( true, false, aViewFrame ); 
      SALOME_ListIteratorOfListIO Iter( aSelList );
      for ( ; Iter.More(); Iter.Next() ) {
	disp->Display( Iter.Value(), false, aViewFrame );
      }
      aViewFrame->Repaint();
    }
  }
}

void DependencyTree_ViewModel::contextMenuPopup( QMenu* theMenu )
{
  GraphicsView_Viewer::contextMenuPopup( theMenu );
  std::cout<<"\n\n\n\n *****contextMenuPopup " << std::endl;


  if( DependencyTree_View* aViewPort = dynamic_cast<DependencyTree_View*>(getActiveViewPort()) )
  {
    int aNbSelected = aViewPort->nbSelected();
    std::cout<<"\n  aNbSelected " << aNbSelected << std::endl;
    if( aNbSelected > 0 ) {
      theMenu->clear();
      theMenu->addAction( tr( "MEN_DISPLAY" ), this, SLOT( onShowSelected() ) );
      theMenu->addAction( tr( "MEN_DISPLAY_ONLY" ), this, SLOT( onShowOnlySelected() ) );
      theMenu->addAction( tr( "REBUILD_THE_TREE"), aViewPort, SLOT( onRebuildModel() ) );
    }
  }


}

//SUIT_ViewWindow* DependencyTree_ViewModel::createView( SUIT_Desktop* theDesktop )
//{
//	DependencyTree_ViewWindow* aViewFrame;
//if( myWidget )
//  aViewFrame = new DependencyTree_ViewWindow( theDesktop, this, myWidget );
//else
//	aViewFrame = new DependencyTree_ViewWindow( theDesktop, this );
//
//connect( aViewFrame, SIGNAL( keyPressed( QKeyEvent* ) ),
//         this, SLOT( onKeyEvent( QKeyEvent* ) ) );
//
//connect( aViewFrame, SIGNAL( keyReleased( QKeyEvent* ) ),
//         this, SLOT( onKeyEvent( QKeyEvent* ) ) );
//
//connect( aViewFrame, SIGNAL( mousePressed( QGraphicsSceneMouseEvent* ) ),
//         this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );
//
//connect( aViewFrame, SIGNAL( mouseMoving( QGraphicsSceneMouseEvent* ) ),
//         this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );
//
//connect( aViewFrame, SIGNAL( mouseReleased( QGraphicsSceneMouseEvent* ) ),
//         this, SLOT( onMouseEvent( QGraphicsSceneMouseEvent* ) ) );
//
//connect( aViewFrame, SIGNAL( wheeling( QGraphicsSceneWheelEvent* ) ),
//         this, SLOT( onWheelEvent( QGraphicsSceneWheelEvent* ) ) );
//
//return aViewFrame;
//}

