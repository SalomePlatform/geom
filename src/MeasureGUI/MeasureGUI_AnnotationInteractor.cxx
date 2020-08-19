// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : MeasureGUI_AnnotationInteractor.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_AnnotationInteractor.h"

// SALOME includes
#include <GeometryGUI.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Desktop.h>
#include <SalomeApp_Application.h>

// Qt includes
#include <QMouseEvent>

// OCCT includes
#include <V3d_View.hxx>

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
MeasureGUI_AnnotationInteractor::MeasureGUI_AnnotationInteractor( GeometryGUI* theGUI,
                                                                  QObject* theParent )
: QObject( theParent ),
  myGeomGUI( theGUI ),
  myIsEnabled( false ),
  myVM( NULL ),
  myViewer( NULL ),
  myActiveViewPort( NULL )
{
}

//=================================================================================
// function : Deactivate
// purpose  : 
//=================================================================================
MeasureGUI_AnnotationInteractor::~MeasureGUI_AnnotationInteractor()
{
  if ( myActiveViewPort )
  {
    myActiveViewPort->releaseMouse();
    myActiveViewPort = NULL;
  }

  Disable();
}

//=================================================================================
// function : Enable
// purpose  : Enables event processing and interaction handlers.
//=================================================================================
void MeasureGUI_AnnotationInteractor::Enable()
{
  if ( myIsEnabled )
  {
    return;
  }

  myIsEnabled = true;

  // install event filtering on viewer windows
  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  myVM     = (OCCViewer_ViewManager*) anApp->getViewManager( OCCViewer_Viewer::Type(), false );
  myViewer = (OCCViewer_Viewer*) myVM->getViewModel();
  if ( !myVM || !myViewer )
  {
    return;
  }

  connect( myVM, SIGNAL( viewCreated( SUIT_ViewWindow* ) ), SLOT( OnViewCreated( SUIT_ViewWindow* ) ) );
  connect( myVM, SIGNAL( deleteView ( SUIT_ViewWindow* ) ), SLOT( OnViewRemoved( SUIT_ViewWindow* ) ) );

  QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
  QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
  for ( ; aViewIt != aViews.end(); ++aViewIt )
  {
    ConnectView( *aViewIt );
  }
}

//=================================================================================
// function : Disable
// purpose  : Disables event processing and interaction handlers.
//=================================================================================
void MeasureGUI_AnnotationInteractor::Disable()
{
  if ( !myIsEnabled )
  {
    return;
  }

  myIsEnabled = false;

  // remove event filtering from viewer windows
  QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
  QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
  for ( ; aViewIt != aViews.end(); ++aViewIt )
  {
    DisconnectView( *aViewIt );
  }

  if ( myActiveViewPort )
  {
    myActiveViewPort->releaseMouse();
    myActiveViewPort = NULL;
  }

  if ( !myActiveIO.IsNull() )
  {
    emit SignalInteractionFinished( myActiveIO );
    myActiveIO.Nullify();
  }
}

//=================================================================================
// function : ConnectView
// purpose  : Connect interactor's event handler to the view window given.
//=================================================================================
void MeasureGUI_AnnotationInteractor::ConnectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->installEventFilter( this );
}

//=================================================================================
// function : DisconnectView
// purpose  : Disconnect interactor's event handler from the view window given.
//=================================================================================
void MeasureGUI_AnnotationInteractor::DisconnectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->removeEventFilter( this );
}

//=================================================================================
// function : OnViewCreated
// purpose  : Handler for signal coming from GUI layer.
//=================================================================================
void MeasureGUI_AnnotationInteractor::OnViewCreated( SUIT_ViewWindow* theView )
{
  ConnectView( theView );
}

//=================================================================================
// function : OnViewRemoved
// purpose  : Handler for signal coming from GUI layer.
//=================================================================================
void MeasureGUI_AnnotationInteractor::OnViewRemoved( SUIT_ViewWindow* theView )
{
  DisconnectView( theView );
}

//=================================================================================
// function : eventFilter
// purpose  : Hooks and process events from OCCT viewer prior to their coming into the base viewer class.
//=================================================================================
bool MeasureGUI_AnnotationInteractor::eventFilter( QObject* theObject, QEvent* theEvent )
{
  OCCViewer_ViewPort3d* aViewPort = (OCCViewer_ViewPort3d*)theObject;

  const Handle(V3d_View) aView3d = aViewPort->getView();

  switch( theEvent->type() )
  {
    // ------------------------------------------------------------------------
    // Start dragging ("grab") event
    // ------------------------------------------------------------------------
    case QEvent::MouseButtonPress :
    {
      QMouseEvent* aMouseEv = dynamic_cast<QMouseEvent*>( theEvent );

      if ( myEditEntry.isEmpty() )
      {
        return false;
      }

      if ( !( aMouseEv->buttons() & Qt::LeftButton ) )
      {
        return false;
      }

      const Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

      anAISContext->MoveTo( aMouseEv->x(), aMouseEv->y(), aView3d, Standard_True );

      if ( !anAISContext->HasDetected() )
      {
        return false;
      }

      const Handle(SelectMgr_EntityOwner) aDetected = anAISContext->DetectedOwner();

      if( aDetected.IsNull() )
      {
        return false;
      }

      const Handle(GEOM_Annotation) aAnnotation =
        Handle(GEOM_Annotation)::DownCast( aDetected->Selectable() );

      if ( aAnnotation.IsNull() )
      {
        return false;
      }

      const Handle(SALOME_InteractiveObject) anIO = 
        Handle(SALOME_InteractiveObject)::DownCast( aAnnotation->GetOwner() );

      if ( anIO.IsNull() || anIO->getEntry() != myEditEntry )
      {
        return false;
      }

      myStartPoint = aMouseEv->pos();
      mySelection.Clear();

      for ( anAISContext->InitSelected(); anAISContext->MoreSelected(); anAISContext->NextSelected() )
      {
        mySelection.Append( anAISContext->SelectedOwner() );
      }

      anAISContext->ClearSelected( Standard_False );
      anAISContext->Unhilight( myActiveIO, Standard_True );

      myActiveViewPort = aViewPort;
      myActiveViewPort->grabMouse();
      myActiveIO = aAnnotation;
      myActiveIO->BeginDrag();

      emit SignalInteractionStarted( myActiveIO );

      return true;
    }

    // ------------------------------------------------------------------------
    // Perform dragging operation
    // ------------------------------------------------------------------------
    case QEvent::MouseMove :
    {
      QMouseEvent* aMouseEv = (QMouseEvent*) theEvent;

      if ( !myActiveIO.IsNull() )
      {
        const Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

        if ( anAISContext->IsHilighted( myActiveIO ) )
        {
          anAISContext->Unhilight( myActiveIO, Standard_False );
        }

        const QPoint aDelta = aMouseEv->pos() - myStartPoint;
        myActiveIO->Drag( aDelta.x(), (-aDelta.y()), aView3d );
        anAISContext->Update( myActiveIO, Standard_False );
        anAISContext->UpdateCurrentViewer();
        return true;
      }

      return false;
    }

    // ------------------------------------------------------------------------
    // Stop dragging operation
    // ------------------------------------------------------------------------
    case QEvent::FocusOut :
    case QEvent::MouseButtonRelease :
    {
      QMouseEvent* aMouseEv = (QMouseEvent*) theEvent;

      if ( myActiveViewPort )
      {
        myActiveViewPort->releaseMouse();
        myActiveViewPort = NULL;
      }
      if ( !myActiveIO.IsNull() )
      {
        myActiveIO->EndDrag();

        const Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

        anAISContext->ClearSelected( Standard_False );
        SelectMgr_SequenceOfOwner::Iterator anIt( mySelection );
        for( ; anIt.More(); anIt.Next() )
        {
          anAISContext->AddOrRemoveSelected( anIt.Value(), Standard_False );
        }

        anAISContext->Update( myActiveIO, Standard_False );
        anAISContext->UpdateCurrentViewer();
        anAISContext->MoveTo( aMouseEv->pos().x(), aMouseEv->pos().y(), aView3d, Standard_True );

        emit SignalInteractionFinished( myActiveIO );

        mySelection.Clear();
        myActiveIO.Nullify();
        return (theEvent->type() == QEvent::MouseButtonRelease);
      }

      return false;
    }

    default: return false;
  }
}
