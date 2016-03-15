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
#include "DependencyTree_ViewModel.h"
#include "DependencyTree_View.h"

// GUI includes
#include <SUIT_Session.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>
#include <OCCViewer_ViewManager.h>

// GEOM includes
#include <GEOM_Displayer.h>
#include <GeometryGUI_Operations.h>

// QT includes
#include <QMenu>

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

//=================================================================================
// function : onShowSelected()
// purpose  : slot for showing selected objects in OCC Viewer
//=================================================================================
void DependencyTree_ViewModel::onShowSelected()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/ true );

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  GEOM_Displayer disp( appStudy );

  OCCViewer_ViewManager* anOCCVM = ( OCCViewer_ViewManager* ) app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/ true );

  if ( SUIT_ViewModel* viewModel = anOCCVM->getViewModel() ) {
    if ( SALOME_View* viewFrame = dynamic_cast<SALOME_View*>( viewModel ) ) {
      SALOME_ListIteratorOfListIO Iter( aSelList );
      for ( ; Iter.More(); Iter.Next() )
        disp.Display( Iter.Value(), false, viewFrame );
      viewFrame->Repaint();
    }
  }
}

//=================================================================================
// function : onShowOnlySelected()
// purpose  : slot for showing only selected objects in OCC Viewer
//=================================================================================
void DependencyTree_ViewModel::onShowOnlySelected()
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects( aSelList );

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  GEOM_Displayer disp( appStudy );

  OCCViewer_ViewManager* anOCCVM = (OCCViewer_ViewManager*) app->getViewManager( OCCViewer_Viewer::Type(), /*create=*/ true );

  if ( SUIT_ViewModel* viewModel = anOCCVM->getViewModel() ) {
    if ( SALOME_View* viewFrame = dynamic_cast<SALOME_View*>( viewModel ) ) {
      disp.EraseAll( true, false, viewFrame );
      SALOME_ListIteratorOfListIO Iter( aSelList );
      for ( ; Iter.More(); Iter.Next() )
        disp.Display( Iter.Value(), false, viewFrame );
      viewFrame->Repaint();
    }
  }
}

//=================================================================================
// function : contextMenuPopup()
// purpose  : process calling of context menu popup
//=================================================================================
void DependencyTree_ViewModel::contextMenuPopup( QMenu* theMenu )
{
  GraphicsView_Viewer::contextMenuPopup( theMenu );

  if( DependencyTree_View* viewPort = dynamic_cast<DependencyTree_View*>( getActiveViewPort() ) )
  {
    int aNbSelected = viewPort->nbSelected();
    if( aNbSelected > 0 ) {
      SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
      GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
      theMenu->clear();
      theMenu->addAction( tr( "MEN_DISPLAY" ), this, SLOT( onShowSelected() ) );
      theMenu->addAction( tr( "MEN_DISPLAY_ONLY" ), this, SLOT( onShowOnlySelected() ) );
      theMenu->addAction( tr( "MEN_REBUILD_THE_TREE"), viewPort, SLOT( onRebuildModel() ) );
      theMenu->addSeparator();
      theMenu->addAction( aGeomGUI->action( GEOMOp::OpReduceStudy ) );
    }
  }
}
