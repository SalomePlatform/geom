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
// File   : DisplayGUI.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "DisplayGUI.h"
#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <GEOM_Constants.h>
#include <GEOM_Displayer.h>
#include <GEOM_AISShape.hxx>
#include <GEOM_Actor.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_OverrideCursor.h>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>

#include <SALOME_ListIO.hxx>

#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>
#include <SVTK_ViewModel.h>
#include <SOCC_ViewModel.h>
#include <SVTK_Prs.h>
#include <SOCC_Prs.h>

#include <QtxActionMenuMgr.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Study.h>

#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <vtkActorCollection.h>
#include <vtkRenderer.h>

#include <QAction>

//=======================================================================
// function : DisplayGUI::DisplayGUI()
// purpose  : Constructor
//=======================================================================
DisplayGUI::DisplayGUI( GeometryGUI* parent ) : GEOMGUI( parent )
{
}

//=======================================================================
// function : DisplayGUI::~DisplayGUI()
// purpose  : Destructor
//=======================================================================
DisplayGUI::~DisplayGUI()
{
}


//=======================================================================
// function : DisplayGUI::OnGUIEvent()
// purpose  : Dispatch menu command
//=======================================================================
bool DisplayGUI::OnGUIEvent(int theCommandID, SUIT_Desktop* parent)
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return false;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy ) return false;

  LightApp_SelectionMgr *Sel = app->selectionMgr();
  SALOME_ListIO selected;
  Sel->selectedObjects( selected );

  switch ( theCommandID ) {
  case GEOMOp::OpDMWireframe:         // MENU VIEW - DISPLAY MODE - WIREFRAME
    SetDisplayMode( 0 );
    break;
  case GEOMOp::OpDMShading:           // MENU VIEW - DISPLAY MODE - SHADING
    SetDisplayMode( 1 );
    break;
  case GEOMOp::OpDMShadingWithEdges:  // MENU VIEW - DISPLAY MODE - SHADING WITH EDGES
    SetDisplayMode( 2 );
    break;
  case GEOMOp::OpDMTexture:           // MENU VIEW - DISPLAY MODE - TEXTURE
    SetDisplayMode( 3 );
    break;
  case GEOMOp::OpShowAll:        // MENU VIEW - SHOW ALL
    getGeometryGUI()->EmitSignalDeactivateDialog();
    DisplayAll();
    break;
  case GEOMOp::OpShowOnly:       // POPUP MENU - SHOW ONLY
    getGeometryGUI()->EmitSignalDeactivateDialog();
    DisplayOnly();
    break;
  case GEOMOp::OpShowOnlyChildren: // POPUP MENU - SHOW ONLY CHILDREN
    getGeometryGUI()->EmitSignalDeactivateDialog();
    DisplayOnlyChildren();
    break;
  case GEOMOp::OpHideAll:        // MENU VIEW - HIDE ALL
    EraseAll();
    break;
  case GEOMOp::OpHide:           // POPUP MENU - HIDE
    Erase();
    break;
  case GEOMOp::OpShow:           // POPUP MENU - SHOW
    getGeometryGUI()->EmitSignalDeactivateDialog();
    Display();
    break;
  case GEOMOp::OpSwitchVectors:  // MENU VIEW - DISPLAY MODE - SHOW EDGE DIRECTION
    SetVectorMode(!GetVectorMode());
    getGeometryGUI()->action( GEOMOp::OpSwitchVectors )->setText
      ( GetVectorMode() ? tr("MEN_VECTOR_MODE_ON") : tr( "MEN_VECTOR_MODE_OFF" ) );
    getGeometryGUI()->menuMgr()->update();
    break;
  case GEOMOp::OpSwitchVertices:  // MENU VIEW - DISPLAY MODE - SHOW/HIDE VERTICES
    SetVerticesMode(!GetVerticesMode());
    getGeometryGUI()->action( GEOMOp::OpSwitchVertices )->setText
      ( GetVerticesMode() ? tr("MEN_VERTICES_MODE_ON") : tr( "MEN_VERTICES_MODE_OFF" ) );
    getGeometryGUI()->menuMgr()->update();
    break;
  case GEOMOp::OpSwitchName:  // MENU VIEW - DISPLAY MODE - SHOW/HIDE NAME
    SetNameMode(!GetNameMode());
    getGeometryGUI()->action( GEOMOp::OpSwitchName )->setText
      ( GetNameMode() ? tr("MEN_NAME_MODE_ON") : tr( "MEN_NAME_MODE_OFF" ) );
    getGeometryGUI()->menuMgr()->update();
    break;
  case GEOMOp::OpWireframe:      // POPUP MENU - DISPLAY MODE - WIREFRAME
    ChangeDisplayMode( 0 );
    break;
  case GEOMOp::OpShading:        // POPUP MENU - DISPLAY MODE - SHADING
    ChangeDisplayMode( 1 );
    break;
  case GEOMOp::OpShadingWithEdges: // POPUP MENU - DISPLAY MODE - SHADING WITH EDGES
    ChangeDisplayMode( 2 );
    break;
  case GEOMOp::OpTexture:        // POPUP MENU - DISPLAY MODE - TEXTURE
    ChangeDisplayMode( 3 );
    break;
  case GEOMOp::OpVectors:        // POPUP MENU - DISPLAY MODE - SHOW EDGE DIRECTION
    ChangeDisplayMode( 4 );
    break;
  case GEOMOp::OpVertices:       // POPUP MENU - DISPLAY MODE - SHOW VERTICES
    ChangeDisplayMode( 5 );
    break;
  case GEOMOp::OpShowName:       // POPUP MENU - DISPLAY MODE - SHOW NAME
    ChangeDisplayMode( 6 );
    break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }
  Sel->setSelectedObjects( selected );
  GEOM_Displayer( appStudy ).UpdateColorScale();
  return true;
}

//=====================================================================================
// function : DisplayGUI::DisplayAll()
// purpose  : Display all GEOM objects
//=====================================================================================
void DisplayGUI::DisplayAll()
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();
  if ( !aStudy ) return;
  _PTR(SComponent) SC ( aStudy->FindComponent( "GEOM" ) );
  if ( !SC )
    return;

  SALOME_ListIO listIO;
  _PTR(ChildIterator) anIter ( aStudy->NewChildIterator( SC ) );
  anIter->InitEx( true );

  SUIT_OverrideCursor wc;

  while( anIter->More() ) {
    _PTR(SObject) valSO ( anIter->Value() );
    _PTR(SObject) refSO;
    if ( !valSO->ReferencedObject( refSO ) ) {
      listIO.Append( new SALOME_InteractiveObject(valSO->GetID().c_str(),
                                                  SC->ComponentDataType().c_str(),
                                                  valSO->GetName().c_str()) );
    } 
    anIter->Next();
  }
  GEOM_Displayer( appStudy ).Display( listIO, true );
}

//=====================================================================================
// function : DisplayGUI::EraseAll()
// purpose  : Erase all GEOM objects
//=====================================================================================
void DisplayGUI::EraseAll()
{
  SUIT_OverrideCursor wc;

  SUIT_Application* app = getGeometryGUI()->getApp();
  if ( app ) {
    SUIT_ViewWindow* vw = app->desktop()->activeWindow();
    if ( vw ) {
      SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
      SUIT_ViewManager* vman = vw->getViewManager();
      if ( vman->getType() == OCCViewer_Viewer::Type() || 
           vman->getType() == SVTK_Viewer::Type() ) {
        GEOM_Displayer( appStudy ).EraseAll(true);
      }
    }
  }
}

//=====================================================================================
// function : DisplayGUI::DisplayOnly()
// purpose  : Display selected GEOM objects and erase other
//=====================================================================================
void DisplayGUI::DisplayOnly()
{
  EraseAll();
  Display();
}

//=====================================================================================
// function : DisplayGUI::DisplayOnlyChildren()
// purpose  : Display only children of selected GEOM objects and erase other
//=====================================================================================
void DisplayGUI::DisplayOnlyChildren()
{
  EraseAll();

  SALOME_ListIO listIO;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if (!app) return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (!anActiveStudy) return;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if (!aSelMgr) return;

  // get selection
  SALOME_ListIO aList;
  //aSelMgr->selectedObjects(aList);
  aSelMgr->selectedObjects(aList, "ObjectBrowser", false);
  SALOME_ListIteratorOfListIO It (aList);

  SUIT_OverrideCursor wc;

  for (; It.More(); It.Next()) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if (anIObject->hasEntry()) {
      _PTR(SObject) SO (anActiveStudy->studyDS()->FindObjectID(anIObject->getEntry()));
      if (SO) {
        _PTR(SComponent) SC (SO->GetFatherComponent());
        if (QString(SO->GetID().c_str()) == QString(SO->GetFatherComponent()->GetID().c_str())) {
          // if component is selected, pass it
        }
        else {
          _PTR(ChildIterator) anIter (anActiveStudy->studyDS()->NewChildIterator(SO));
          anIter->InitEx(true);
          while (anIter->More()) {
            _PTR(SObject) valSO (anIter->Value());
            _PTR(SObject) refSO;
            if (!valSO->ReferencedObject(refSO)) {
              listIO.Append(new SALOME_InteractiveObject(valSO->GetID().c_str(),
                                                         SC->ComponentDataType().c_str(),
                                                         valSO->GetName().c_str()));
            }
            anIter->Next();
          }
        }
      }
    }
  }
  GEOM_Displayer(anActiveStudy).Display(listIO, true);
}

//=====================================================================================
// function : DisplayGUI::Display()
// purpose  : Display selected GEOM objects
//=====================================================================================
void DisplayGUI::Display()
{
  SALOME_ListIO listIO;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !anActiveStudy ) return;

  //get SalomeApp selection manager
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aList;
  aSelMgr->selectedObjects( aList );
  SALOME_ListIteratorOfListIO It( aList );

  SUIT_OverrideCursor wc;

  for( ;It.More();It.Next() ) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if ( anIObject->hasEntry() ) {
      _PTR(SObject) SO ( anActiveStudy->studyDS()->FindObjectID( anIObject->getEntry() ) );
      if ( SO && QString(SO->GetID().c_str()) == QString(SO->GetFatherComponent()->GetID().c_str()) ) {
        _PTR(SComponent) SC ( SO->GetFatherComponent() );
        // if component is selected
        listIO.Clear();
        _PTR(ChildIterator) anIter ( anActiveStudy->studyDS()->NewChildIterator( SO ) );
        anIter->InitEx( true );
        while( anIter->More() ) {
          _PTR(SObject) valSO ( anIter->Value() );
          _PTR(SObject) refSO;
          if ( !valSO->ReferencedObject( refSO ) ) {
            listIO.Append( new SALOME_InteractiveObject(valSO->GetID().c_str(),
                                                        SC->ComponentDataType().c_str(),
                                                        valSO->GetName().c_str()) );
          }
          anIter->Next();
        }
        break;
      }
      else {
        listIO.Append( anIObject );
      }
    }
    else {
      listIO.Append( anIObject );
    }
  }
  GEOM_Displayer( anActiveStudy ).Display( listIO, true );
}


//=====================================================================================
// function : DisplayGUI::Erase()
// purpose  : Erase selected GEOM objects
//=====================================================================================
void DisplayGUI::Erase()
{
  SALOME_ListIO listIO;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* anActiveStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !anActiveStudy ) return;

  //get SalomeApp selection manager
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SALOME_ListIO aList;
  aSelMgr->selectedObjects( aList );
  SALOME_ListIteratorOfListIO It( aList );

  SUIT_OverrideCursor wc;

  for( ; It.More(); It.Next() ) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if ( anIObject->hasEntry() ) {
      _PTR(SObject) SO ( anActiveStudy->studyDS()->FindObjectID( anIObject->getEntry() ) );
      if ( SO && QString(SO->GetID().c_str()) == QString(SO->GetFatherComponent()->GetID().c_str()) ) {
        _PTR(SComponent) SC ( SO->GetFatherComponent() );
        // if component is selected
        listIO.Clear();
        _PTR(ChildIterator) anIter ( anActiveStudy->studyDS()->NewChildIterator( SO ) );
        anIter->InitEx( true );
        while( anIter->More() ) {
          _PTR(SObject) valSO ( anIter->Value() );
          _PTR(SObject) refSO;
          if ( !valSO->ReferencedObject( refSO ) ) {
            listIO.Append( new SALOME_InteractiveObject(valSO->GetID().c_str(),
                                                        SC->ComponentDataType().c_str(),
                                                        valSO->GetName().c_str()) );
          }
          anIter->Next();
        }
        break;
      }
      else {
        listIO.Append( anIObject );
      }
    }
    else {
      listIO.Append( anIObject );
    }
  }

  SUIT_ViewWindow* viewWindow = app->desktop()->activeWindow();
  bool aIsForced = true;
  if(viewWindow->getViewManager()->getType() == SVTK_Viewer::Type())
    aIsForced = false;

  GEOM_Displayer(anActiveStudy).Erase( listIO, aIsForced);
  getGeometryGUI()->getApp()->selectionMgr()->clearSelected();
}

//=====================================================================================
// function : DisplayGUI::SetDisplayMode()
// purpose  : Set display mode for the viewer (current viewer if <viewWindow> - 0 )
//=====================================================================================
void DisplayGUI::SetDisplayMode( const int mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor wc;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  if ( !viewWindow ) 
    viewWindow = app->desktop()->activeWindow();

  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !aStudy ) return;

  GEOM_Displayer displayer( aStudy );

  int mgrId = viewWindow->getViewManager()->getGlobalId();

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    aStudy->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::DisplayMode ), mode );
    displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

//=====================================================================================
// function : DisplayGUI::SetVectorsMode()
// purpose  : Set vector mode for the viewer
//=====================================================================================
void DisplayGUI::SetVectorMode( const bool mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor wc;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !aStudy ) return;

  if ( !viewWindow ) 
    viewWindow = app->desktop()->activeWindow();

  GEOM_Displayer displayer( aStudy );

  viewWindow->setProperty( "VectorsMode", mode );

  int aMgrId = viewWindow->getViewManager()->getGlobalId();

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    aStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::EdgesDirection ), mode );
    displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

//=====================================================================================
// function : DisplayGUI::GetVectorMode()
// purpose  : Get the "show edge direction" mode of the viewer
//=====================================================================================
int DisplayGUI::GetVectorMode( SUIT_ViewWindow* viewWindow )
{
  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  return viewWindow->property( "VectorsMode" ).toBool();
}

//=====================================================================================
// function : DisplayGUI::SetVerticesMode()
// purpose  : Set vertices mode for the viewer
//=====================================================================================
void DisplayGUI::SetVerticesMode( const bool mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor wc;

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !aStudy ) return;

  if ( !viewWindow ) 
    viewWindow = app->desktop()->activeWindow();

  GEOM_Displayer displayer( aStudy );

  viewWindow->setProperty( "VerticesMode", mode );

  int aMgrId = viewWindow->getViewManager()->getGlobalId();

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    aStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::Vertices ), mode );
    displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

//=====================================================================================
// function : DisplayGUI::GetVerticesMode()
// purpose  : Get the "show vertices" mode of the viewer
//=====================================================================================
int DisplayGUI::GetVerticesMode( SUIT_ViewWindow* viewWindow )
{
  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  return viewWindow->property( "VerticesMode" ).toBool();
}

//=====================================================================================
// function : DisplayGUI::SetNameMode()
// purpose  : Set name mode for the viewer
//=====================================================================================
void DisplayGUI::SetNameMode( const bool mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor();

  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !aStudy ) return;

  if ( !viewWindow )
    viewWindow = app->desktop()->activeWindow();

  viewWindow->setProperty( "NameMode", mode );

  GEOM_Displayer displayer( aStudy );

  int aMgrId = viewWindow->getViewManager()->getGlobalId();

  SALOME_View* window = displayer.GetActiveView();
  if ( !window ) return;

  SALOME_ListIO anIOlst;
  window->GetVisible( anIOlst );

  for ( SALOME_ListIteratorOfListIO It( anIOlst ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    aStudy->setObjectProperty( aMgrId, io->getEntry(), GEOM::propertyName( GEOM::ShowName ), mode );
    displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

//=====================================================================================
// function : DisplayGUI::GetNameMode()
// purpose  : Get the "show name" mode of the viewer
//=====================================================================================
int DisplayGUI::GetNameMode( SUIT_ViewWindow* viewWindow )
{
  if ( !viewWindow )
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  return viewWindow->property( "NameMode" ).toBool();
}

//=====================================================================================
// function : DisplayGUI::ChangeDisplayMode()
// purpose  : Set display mode for selected objects in the viewer given
//            (current viewer if <viewWindow> = 0 )
//=====================================================================================
void DisplayGUI::ChangeDisplayMode( const int mode, SUIT_ViewWindow* viewWindow )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return;

  if ( !viewWindow ) 
    viewWindow = app->desktop()->activeWindow();

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr ) return;

  SalomeApp_Study* aStudy = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  if ( !aStudy ) return;

  SUIT_OverrideCursor wc;

  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  GEOM_Displayer displayer( aStudy );

  int mgrId = viewWindow->getViewManager()->getGlobalId();

  QVariant v = aStudy->getObjectProperty( mgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::EdgesDirection ), QVariant() );
  bool vectorMode =  v.isValid() ? !v.toBool() : false;
  v = aStudy->getObjectProperty( mgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::Vertices ), QVariant() );
  bool verticesMode =  v.isValid() ? !v.toBool() : false;
  v = aStudy->getObjectProperty( mgrId, selected.First()->getEntry(), GEOM::propertyName( GEOM::ShowName ), QVariant() );
  bool nameMode =  v.isValid() ? !v.toBool() : false;

  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
    Handle( SALOME_InteractiveObject ) io = It.Value();
    if ( mode == 0 || mode == 1 || mode == 2 || mode == 3 ) {
      aStudy->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::DisplayMode ), mode );
    }
    else if ( mode == 4 ) {
      aStudy->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::EdgesDirection ), vectorMode );
    }
    else if ( mode == 5 ) {
      aStudy->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::Vertices ), verticesMode );
    }
    else if ( mode == 6 ) {
      aStudy->setObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::ShowName ), nameMode );
    }
    displayer.Redisplay( io, false );
  }
  displayer.UpdateViewer();
  GeometryGUI::Modified();
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new DisplayGUI( parent );
  }
}
