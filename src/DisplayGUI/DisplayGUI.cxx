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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
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

#include "QAD_RightFrame.h"
#include "QAD_WaitCursor.h"
#include "QAD_Desktop.h"
#include "VTKViewer_ViewFrame.h"
#include "VTKViewer_RenderWindowInteractor.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_Viewer3d.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "VTKViewer_Prs.h"
#include "OCCViewer_Prs.h"

#include <AIS_ListIteratorOfListOfInteractive.hxx>

using namespace std;

DisplayGUI* DisplayGUI::myGUIObject = 0;

//=======================================================================
// function : DisplayGUI::GetDisplayGUI()
// purpose  : Get the only DisplayGUI object [ static ]
//=======================================================================
DisplayGUI* DisplayGUI::GetDisplayGUI()
{
  if ( myGUIObject == 0 ) {
    // init DisplayGUI only once
    myGUIObject = new DisplayGUI();
  }
  return myGUIObject;
}

//=======================================================================
// function : DisplayGUI::DisplayGUI()
// purpose  : Constructor
//=======================================================================
DisplayGUI::DisplayGUI() : GEOMGUI()
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
bool DisplayGUI::OnGUIEvent(int theCommandID, QAD_Desktop* parent)
{
  DisplayGUI* myDisplayGUI = GetDisplayGUI();

  switch (theCommandID) {
  case 211: // MENU VIEW - WIREFRAME/SHADING
    {
      myDisplayGUI->InvertDisplayMode();
      int newMode = myDisplayGUI->GetDisplayMode();
      QAD_Application::getDesktop()->menuBar()->changeItem( 211, newMode == 1 ? tr( "GEOM_MEN_WIREFRAME" ) : tr("GEOM_MEN_SHADING") );
      break;
    }
  case 212: // MENU VIEW - DISPLAY ALL
    {
      myDisplayGUI->DisplayAll();
      break;
    }
  case 213: // MENU VIEW - DISPLAY ONLY
    {
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
      parent->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
      break;
    }
  }
  return true;
}

//=====================================================================================
// function : DisplayGUI::DisplayAll()
// purpose  : Display all GEOM objects
//=====================================================================================
void DisplayGUI::DisplayAll()
{
  QAD_WaitCursor wc;
  SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
  SALOMEDS::SComponent_var SC = aStudy->FindComponent( "GEOM" );
  if ( SC->_is_nil() )
    return;

  SALOME_ListIO listIO;
  SALOMEDS::ChildIterator_var anIter = aStudy->NewChildIterator( SC );
  anIter->InitEx( true );
  while( anIter->More() ) {
    SALOMEDS::SObject_var valSO = anIter->Value();
    SALOMEDS::SObject_var refSO;
    if ( !valSO->ReferencedObject( refSO ) ) {
      listIO.Append( new SALOME_InteractiveObject( valSO->GetID(), SC->ComponentDataType() ,valSO->GetName()) );
    } 
    anIter->Next();
  }
  GEOM_Displayer().Display( listIO, true );
}

//=====================================================================================
// function : DisplayGUI::EraseAll()
// purpose  : Erase all GEOM objects
//=====================================================================================
void DisplayGUI::EraseAll()
{
  QAD_WaitCursor wc;
  QAD_ViewFrame* vf = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame();
  if ( vf->getTypeView() == VIEW_OCC || vf->getTypeView() == VIEW_VTK )
    vf->EraseAll();
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
  QAD_WaitCursor wc;
  SALOME_ListIO listIO;

  QAD_Study* anActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
  SALOME_Selection* Sel = SALOME_Selection::Selection( anActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
  for( ;It.More();It.Next() ) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if ( anIObject->hasEntry() ) {
      SALOMEDS::SObject_var SO = anActiveStudy->getStudyDocument()->FindObjectID( anIObject->getEntry() );
      if ( !SO->_is_nil() && QString( SO->GetID() ) == QString( SO->GetFatherComponent()->GetID() ) ) {
	SALOMEDS::SComponent_var SC = SO->GetFatherComponent();
	// if component is selected
	listIO.Clear();
	SALOMEDS::ChildIterator_var anIter = anActiveStudy->getStudyDocument()->NewChildIterator( SO );
	anIter->InitEx( true );
	while( anIter->More() ) {
	  SALOMEDS::SObject_var valSO = anIter->Value();
	  SALOMEDS::SObject_var refSO;
	  if ( !valSO->ReferencedObject( refSO ) ) {
	    listIO.Append( new SALOME_InteractiveObject( valSO->GetID(), SC->ComponentDataType() ,valSO->GetName()) );
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
  GEOM_Displayer().Display( listIO, true );
}


//=====================================================================================
// function : DisplayGUI::Erase()
// purpose  : Erase selected GEOM objects
//=====================================================================================
void DisplayGUI::Erase()
{
  QAD_WaitCursor wc;
  SALOME_ListIO listIO;

  QAD_Study* anActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
  SALOME_Selection* Sel = SALOME_Selection::Selection( anActiveStudy->getSelection() );
  SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
  for( ;It.More();It.Next() ) {
    Handle(SALOME_InteractiveObject) anIObject = It.Value();
    if ( anIObject->hasEntry() ) {
      SALOMEDS::SObject_var SO = anActiveStudy->getStudyDocument()->FindObjectID( anIObject->getEntry() );
      if ( !SO->_is_nil() && QString( SO->GetID() ) == QString( SO->GetFatherComponent()->GetID() ) ) {
	SALOMEDS::SComponent_var SC = SO->GetFatherComponent();
	// if component is selected
	listIO.Clear();
	SALOMEDS::ChildIterator_var anIter = anActiveStudy->getStudyDocument()->NewChildIterator( SO );
	anIter->InitEx( true );
	while( anIter->More() ) {
	  SALOMEDS::SObject_var valSO = anIter->Value();
	  SALOMEDS::SObject_var refSO;
	  if ( !valSO->ReferencedObject( refSO ) ) {
	    listIO.Append( new SALOME_InteractiveObject( valSO->GetID(), SC->ComponentDataType() ,valSO->GetName()) );
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
  GEOM_Displayer().Erase( listIO, true );
  Sel->ClearIObjects();
}

//=====================================================================================
// function : DisplayGUI::SetDisplayMode()
// purpose  : Set display mode for the viewer (current viewer if <viewFrame> - 0 )
//=====================================================================================
void DisplayGUI::SetDisplayMode( const int mode, QAD_ViewFrame* viewFrame )
{
  QAD_WaitCursor wc;
  if ( !viewFrame ) 
    viewFrame = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame();
  if( viewFrame->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)viewFrame)->getRWInteractor();
    myRenderInter->SetDisplayMode( mode );
  } 
  else if( viewFrame->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)viewFrame)->getViewer();
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
// purpose  : Get display mode of the viewer (current viewer if <viewFrame> - 0 )
//=====================================================================================
int DisplayGUI::GetDisplayMode( QAD_ViewFrame* viewFrame )
{
  int dispMode = 0;
  if ( !viewFrame ) 
    viewFrame = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame();
  if( viewFrame->getTypeView() == VIEW_VTK) {
    VTKViewer_RenderWindowInteractor* myRenderInter= ((VTKViewer_ViewFrame*)viewFrame)->getRWInteractor();
    dispMode = myRenderInter->GetDisplayMode();
  } 
  else if( viewFrame->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)viewFrame)->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    AIS_DisplayMode mode = (AIS_DisplayMode)ic->DisplayMode();
    dispMode = (mode == AIS_WireFrame ? 0 : 1 );
  }
  return dispMode;
}

//=====================================================================================
// function : DisplayGUI::InvertDisplayMode()
// purpose  : Invert display mode ( shadin <-> wireframe ) for the viewer 
//            (current viewer if <viewFrame> = 0 )
//=====================================================================================
void DisplayGUI::InvertDisplayMode( QAD_ViewFrame* viewFrame )
{
  SetDisplayMode( 1 - GetDisplayMode( viewFrame ) );
}

//=====================================================================================
// function : DisplayGUI::ChangeDisplayMode()
// purpose  : Set display mode for selected objects in the viewer given
//            (current viewer if <viewFrame> = 0 )
//=====================================================================================
void DisplayGUI::ChangeDisplayMode( const int mode, QAD_ViewFrame* viewFrame )
{
  QAD_WaitCursor wc;
  if ( !viewFrame ) 
    viewFrame = QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame();
  SALOME_Selection* Sel = SALOME_Selection::Selection(QAD_Application::getDesktop()->getActiveStudy()->getSelection());
  if ( viewFrame->getTypeView() == VIEW_VTK ) {
    VTKViewer_RenderWindowInteractor* myRenderInter = ((VTKViewer_ViewFrame*)viewFrame)->getRWInteractor();
    SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
    for( ;It.More(); It.Next() ) {
      VTKViewer_Prs* vtkPrs = dynamic_cast<VTKViewer_Prs*>( viewFrame->CreatePrs( It.Value()->getEntry() ) );
      if ( vtkPrs && !vtkPrs->IsNull() ) {
	if ( mode == 0 )
	  myRenderInter->ChangeRepresentationToWireframe( vtkPrs->GetObjects() );
	else if ( mode == 1 )
	  myRenderInter->ChangeRepresentationToSurface( vtkPrs->GetObjects() );
      }
    }
    myRenderInter->Render();
  }
  else if ( viewFrame->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)viewFrame)->getViewer();
    Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
    SALOME_ListIteratorOfListIO It( Sel->StoredIObjects() );
    for( ;It.More(); It.Next() ) {
      OCCViewer_Prs* occPrs = dynamic_cast<OCCViewer_Prs*>( viewFrame->CreatePrs( It.Value()->getEntry() ) );
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
  GEOMGUI* GetLibGUI()
  {
    return DisplayGUI::GetDisplayGUI();
  }
}
