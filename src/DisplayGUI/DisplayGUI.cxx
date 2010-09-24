//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : DisplayGUI.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "DisplayGUI.h"
#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"
#include <GEOM_Displayer.h>
#include <GEOM_AISShape.hxx>
#include <GEOM_Actor.h>

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_OverrideCursor.h>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

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

  LightApp_SelectionMgr *Sel = app->selectionMgr();
  SALOME_ListIO selected;
  Sel->selectedObjects( selected );

  switch ( theCommandID ) {
  case GEOMOp::OpDisplayMode:    // MENU VIEW - DISPLAY MODE - WIREFRAME/SHADING
    InvertDisplayMode();
    getGeometryGUI()->action( GEOMOp::OpDisplayMode )->setText
      ( GetDisplayMode() == 1 ? tr( "GEOM_MEN_WIREFRAME" ) : tr("GEOM_MEN_SHADING") );
    getGeometryGUI()->menuMgr()->update();
    break;
  case GEOMOp::OpShowAll:        // MENU VIEW - SHOW ALL
    getGeometryGUI()->EmitSignalDeactivateDialog();
    DisplayAll();
    break;
  case GEOMOp::OpShowOnly:       // POPUP MENU - SHOW ONLY
    getGeometryGUI()->EmitSignalDeactivateDialog();
    DisplayOnly();
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
  case GEOMOp::OpWireframe:      // POPUP MENU - DISPLAY MODE - WIREFRAME
    ChangeDisplayMode( 0 );
    break;
  case GEOMOp::OpShading:        // POPUP MENU - DISPLAY MODE - SHADING
    ChangeDisplayMode( 1 );
    break;
  case GEOMOp::OpVectors:        // POPUP MENU - DISPLAY MODE - SHOW EDGE DIRECTION
    ChangeDisplayMode( 2 );
    break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }
  Sel->setSelectedObjects( selected );
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

  SUIT_OverrideCursor();

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
  SUIT_OverrideCursor();

  SUIT_Application* app = getGeometryGUI()->getApp();
  if ( app ) {
    SUIT_ViewWindow* vw = app->desktop()->activeWindow();
    if ( vw ) {
      SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
      SUIT_ViewManager* vman = vw->getViewManager();
      if ( vman->getType() == OCCViewer_Viewer::Type() || 
           vman->getType() == SVTK_Viewer::Type() ) {
        GEOM_Displayer( appStudy ).EraseAll();
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

  SUIT_OverrideCursor();

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

  SUIT_OverrideCursor();

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
  SUIT_OverrideCursor();

  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  if ( viewWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_View* aView = ((SVTK_ViewWindow*)viewWindow)->getView();
    aView->SetDisplayMode( mode );
    GeometryGUI::Modified();
  } 
  else if ( viewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)(viewWindow->getViewManager()))->getOCCViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    AIS_DisplayMode newmode = (mode == 1 ? AIS_Shaded : AIS_WireFrame);
    AIS_ListOfInteractive List;
    ic->DisplayedObjects( List );
    AIS_ListOfInteractive List1;
    ic->ObjectsInCollector( List1 );
    List.Append( List1 );

    AIS_ListIteratorOfListOfInteractive ite( List );
    while( ite.More() ) {
      if( ite.Value()->IsInstance( STANDARD_TYPE(GEOM_AISShape) ) ) {
        Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast( ite.Value() );
        ic->SetDisplayMode( aSh, Standard_Integer( newmode ),true );
      }
      ite.Next();
    }

    ic->SetDisplayMode( newmode, Standard_False );
    GeometryGUI::Modified();
  }
}

//=====================================================================================
// function : DisplayGUI::GetDisplayMode()
// purpose  : Get display mode of the viewer (current viewer if <viewWindow> - 0 )
//=====================================================================================
int DisplayGUI::GetDisplayMode( SUIT_ViewWindow* viewWindow )
{
  int dispMode = 0;
  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  if ( viewWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_View* aView = ((SVTK_ViewWindow*)viewWindow)->getView();
    dispMode = aView->GetDisplayMode();
  } 
  else if ( viewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)(viewWindow->getViewManager()))->getOCCViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    AIS_DisplayMode mode = (AIS_DisplayMode)ic->DisplayMode();
    dispMode = (mode == AIS_WireFrame ? 0 : 1 );
  }
  return dispMode;
}

//=====================================================================================
// function : DisplayGUI::SetVectorsMode()
// purpose  : Set vector mode for the viewer
//=====================================================================================
void DisplayGUI::SetVectorMode( const bool mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor();

  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  if ( viewWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    viewWindow->setCustomData( "VectorsMode", QVariant( mode ) );
    SVTK_ViewWindow* vw = dynamic_cast<SVTK_ViewWindow*>( viewWindow );
    vtkActorCollection* allActors = vw->getRenderer()->GetActors();
    allActors->InitTraversal();
    while (vtkActor* actor = allActors->GetNextActor()) {
      if (actor->GetVisibility()) { // only for visible actors
        GEOM_Actor* aGeomActor = 0;
        if ( actor->IsA( "GEOM_Actor" ) ) {
          aGeomActor = GEOM_Actor::SafeDownCast( actor );
          if ( aGeomActor )
            aGeomActor->SetVectorMode( mode );
        }
      }
    }
    GeometryGUI::Modified();
  }
  else if ( viewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    viewWindow->setCustomData( "VectorsMode", QVariant( mode ) );
    OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)(viewWindow->getViewManager()))->getOCCViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    AIS_ListOfInteractive List;
    ic->DisplayedObjects( List );
    AIS_ListOfInteractive List1;
    ic->ObjectsInCollector( List1 );
    List.Append( List1 );

    AIS_ListIteratorOfListOfInteractive ite( List );
    while( ite.More() ) {
      if( ite.Value()->IsInstance( STANDARD_TYPE(GEOM_AISShape) ) ) {
        Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast( ite.Value() );
        aSh->SetDisplayVectors(mode);
        ic->RecomputePrsOnly(ite.Value());
      }
      ite.Next();
    }
    GeometryGUI::Modified();
  }
}

//=====================================================================================
// function : DisplayGUI::GetVectorMode()
// purpose  : Get the "show edge direction" mode of the viewer
//=====================================================================================
int DisplayGUI::GetVectorMode( SUIT_ViewWindow* viewWindow )
{
  if ( !viewWindow ) 
    viewWindow = getGeometryGUI()->getApp()->desktop()->activeWindow();
  return viewWindow->getCustomData( "VectorsMode" ).toBool();
}

//=====================================================================================
// function : DisplayGUI::InvertDisplayMode()
// purpose  : Invert display mode ( shadin <-> wireframe ) for the viewer 
//            (current viewer if <viewWindow> = 0 )
//=====================================================================================
void DisplayGUI::InvertDisplayMode( SUIT_ViewWindow* viewWindow )
{
  SetDisplayMode( 1 - GetDisplayMode( viewWindow ) );
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

  SUIT_OverrideCursor();

  SALOME_ListIO aList;

  if ( viewWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_ViewWindow* vw = dynamic_cast<SVTK_ViewWindow*>( viewWindow );
    SVTK_View* aView = vw->getView();

    aSelMgr->selectedObjects( aList );
    SALOME_ListIteratorOfListIO It( aList );

    for( ;It.More(); It.Next() ) {
      SVTK_Viewer* stvkViewer = dynamic_cast<SVTK_Viewer*>(vw->getViewManager()->getViewModel());
      SVTK_Prs* vtkPrs =
        stvkViewer ? dynamic_cast<SVTK_Prs*>( stvkViewer->CreatePrs( It.Value()->getEntry() ) ) : 0;
      if ( vtkPrs && !vtkPrs->IsNull() ) {
        if ( mode == 0 )
          aView->ChangeRepresentationToWireframe( vtkPrs->GetObjects() );
        else if ( mode == 1 )
          aView->ChangeRepresentationToSurface( vtkPrs->GetObjects() );
        else if ( mode == 2 ) {
          vtkActorCollection* anActors = vtkPrs->GetObjects();
          anActors->InitTraversal();
          while (vtkActor* anAct = anActors->GetNextActor()) {
            GEOM_Actor* aGeomActor = GEOM_Actor::SafeDownCast(anAct);
            aGeomActor->SetVectorMode(!aGeomActor->GetVectorMode());
          }
        }
      }
    }
    aView->Repaint();
    GeometryGUI::Modified();
  }
  else if ( viewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)(viewWindow->getViewManager()))->getOCCViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();

    aSelMgr->selectedObjects( aList );
    SALOME_ListIteratorOfListIO It( aList );

    for( ;It.More(); It.Next() ) {
      SOCC_Viewer* soccViewer = (SOCC_Viewer*)(viewWindow->getViewManager()->getViewModel());
      SOCC_Prs* occPrs = dynamic_cast<SOCC_Prs*>( soccViewer->CreatePrs( It.Value()->getEntry() ) );
      if ( occPrs && !occPrs->IsNull() ) {
        AIS_ListOfInteractive shapes; occPrs->GetObjects( shapes );
        AIS_ListIteratorOfListOfInteractive interIter( shapes );
        for ( ; interIter.More(); interIter.Next() ) {
          if ( mode == 0 )
            ic->SetDisplayMode( interIter.Value(), AIS_WireFrame, false );
          else if ( mode == 1 )
            ic->SetDisplayMode( interIter.Value(), AIS_Shaded, false );
          if (mode == 2 ) {
            Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast( interIter.Value() );
            if ( !aSh.IsNull() ) {
              aSh->SetDisplayVectors(!aSh->isShowVectors());
              ic->RecomputePrsOnly(interIter.Value());
            }
          }
        }
      }
    }
    ic->UpdateCurrentViewer();
    GeometryGUI::Modified();
  }
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
