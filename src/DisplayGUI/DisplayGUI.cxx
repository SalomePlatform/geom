//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : DisplayGUI.cxx
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#include "DisplayGUI.h"
#include "GeometryGUI.h"
#include "GEOM_Displayer.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_OverrideCursor.h>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>

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

#include <qmenubar.h>


DisplayGUI* DisplayGUI::myGUIObject = 0;

//=======================================================================
// function : DisplayGUI::GetDisplayGUI()
// purpose  : Get the only DisplayGUI object [ static ]
//=======================================================================
DisplayGUI* DisplayGUI::GetDisplayGUI( GeometryGUI* parent )
{
  if ( myGUIObject == 0 ) {
    // init DisplayGUI only once
    myGUIObject = new DisplayGUI( parent );
  }
  return myGUIObject;
}

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
  DisplayGUI* myDisplayGUI = GetDisplayGUI( getGeometryGUI() );
  LightApp_SelectionMgr *Sel = getGeometryGUI()->getApp()->selectionMgr();
  SALOME_ListIO selected;
  Sel->selectedObjects( selected );

  switch (theCommandID) {
  case 211: // MENU VIEW - WIREFRAME/SHADING
    {
      myDisplayGUI->InvertDisplayMode();
      int newMode = myDisplayGUI->GetDisplayMode();
      getGeometryGUI()->action( 211 )->setMenuText( newMode == 1 ? tr( "GEOM_MEN_WIREFRAME" ) : tr("GEOM_MEN_SHADING") );
      getGeometryGUI()->menuMgr()->update();
//      SUIT_Session::session()->activeApplication()->desktop()->menuBar()->
//	changeItem( 211, newMode == 1 ? tr( "GEOM_MEN_WIREFRAME" ) : tr("GEOM_MEN_SHADING") );
      break;
    }
  case 212: // MENU VIEW - DISPLAY ALL
    {
      getGeometryGUI()->EmitSignalDeactivateDialog();
      myDisplayGUI->DisplayAll();
      break;
    }
  case 213: // MENU VIEW - DISPLAY ONLY
    {
      getGeometryGUI()->EmitSignalDeactivateDialog();
      myDisplayGUI->DisplayOnly();
      break;
    }
  case 214: // MENU VIEW - ERASE ALL
    {
      myDisplayGUI->EraseAll();
      break;
    }
  case 215: // MENU VIEW - ERASE
    {
      myDisplayGUI->Erase();
      break;
    }
  case 216: // MENU VIEW - DISPLAY
    {
      getGeometryGUI()->EmitSignalDeactivateDialog();
      myDisplayGUI->Display();
      break;
    }
  case 80311: // POPUP VIEWER - WIREFRAME
    {
      myDisplayGUI->ChangeDisplayMode( 0 );
      break;
    }
  case 80312: // POPUP VIEWER - SHADING
    {
      myDisplayGUI->ChangeDisplayMode( 1 );
      break;
    }
  default:
    {
      SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
      break;
    }
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
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
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
      listIO.Append( new SALOME_InteractiveObject( valSO->GetID().c_str(), SC->ComponentDataType().c_str() ,valSO->GetName().c_str() ) );
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

  SUIT_Application* app = SUIT_Session::session()->activeApplication();
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
  
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
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
      if ( SO && QString( SO->GetID().c_str() ) == QString( SO->GetFatherComponent()->GetID().c_str() ) ) {
	_PTR(SComponent) SC ( SO->GetFatherComponent() );
	// if component is selected
	listIO.Clear();
	_PTR(ChildIterator) anIter ( anActiveStudy->studyDS()->NewChildIterator( SO ) );
	anIter->InitEx( true );
	while( anIter->More() ) {
	  _PTR(SObject) valSO ( anIter->Value() );
	  _PTR(SObject) refSO;
	  if ( !valSO->ReferencedObject( refSO ) ) {
	    listIO.Append( new SALOME_InteractiveObject( valSO->GetID().c_str(), SC->ComponentDataType().c_str() ,valSO->GetName().c_str() ) );
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

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
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
      if ( SO && QString( SO->GetID().c_str() ) == QString( SO->GetFatherComponent()->GetID().c_str() ) ) {
	_PTR(SComponent) SC ( SO->GetFatherComponent() );
	// if component is selected
	listIO.Clear();
	_PTR(ChildIterator) anIter ( anActiveStudy->studyDS()->NewChildIterator( SO ) );
	anIter->InitEx( true );
	while( anIter->More() ) {
	  _PTR(SObject) valSO ( anIter->Value() );
	  _PTR(SObject) refSO;
	  if ( !valSO->ReferencedObject( refSO ) ) {
	    listIO.Append( new SALOME_InteractiveObject( valSO->GetID().c_str(), SC->ComponentDataType().c_str() ,valSO->GetName().c_str() ) );
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
  GEOM_Displayer(anActiveStudy).Erase( listIO, true );
  ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr()->clearSelected();
}

//=====================================================================================
// function : DisplayGUI::SetDisplayMode()
// purpose  : Set display mode for the viewer (current viewer if <viewWindow> - 0 )
//=====================================================================================
void DisplayGUI::SetDisplayMode( const int mode, SUIT_ViewWindow* viewWindow )
{
  SUIT_OverrideCursor();

  if ( !viewWindow ) 
    viewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( viewWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_View* aView = ((SVTK_ViewWindow*)viewWindow)->getView();
    aView->SetDisplayMode( mode );
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
    viewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
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
  if ( !viewWindow ) 
    viewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;

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
      SVTK_Prs* vtkPrs = stvkViewer ? dynamic_cast<SVTK_Prs*>( stvkViewer->CreatePrs( It.Value()->getEntry() ) ) : 0;
      if ( vtkPrs && !vtkPrs->IsNull() ) {
	if ( mode == 0 )
	  aView->ChangeRepresentationToWireframe( vtkPrs->GetObjects() );
	else if ( mode == 1 )
	  aView->ChangeRepresentationToSurface( vtkPrs->GetObjects() );
      }
    }
    aView->Repaint();
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
	}
      }
    }
    ic->UpdateCurrentViewer();
  }
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WNT
	__declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return DisplayGUI::GetDisplayGUI( parent );
  }
}
