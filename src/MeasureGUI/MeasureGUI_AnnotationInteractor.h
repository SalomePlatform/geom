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
// File   : MeasureGUI_AnnotationInteractor.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_ANNOTATIONINTERACTOR_H
#define MEASUREGUI_ANNOTATIONINTERACTOR_H

#include <GEOM_Annotation.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <QObject>
#include <QPoint>

class GeometryGUI;
class OCCViewer_Viewer;
class OCCViewer_ViewManager;
class OCCViewer_ViewPort3d;
class SUIT_ViewWindow;

/*!
 * \class MeasureGUI_AnnotationInteractor
 * \brief Class implementing logical layer for interactive dragging of annotation
 *        labels. It includes two components: listener for GUI events occuring
 *        inside of OCCT 3D viewer and processor for hooked events to perform
 *        interactive modification of the selected annotation within current
 *        AIS context.
 */
class MeasureGUI_AnnotationInteractor : public QObject
{
  Q_OBJECT

public:

  //! Constructor.
  //! Connects to existing viewer/view windows to process events.
  //! \param theGUI [in] the geometry module's GUI interface.
  //! \param theOwner [in] the owner of the instance.
  MeasureGUI_AnnotationInteractor( GeometryGUI* theGUI, QObject* theOwner );

  //! Destructor.
  virtual ~MeasureGUI_AnnotationInteractor();

public:

  //! Enables event processing and interaction handlers.
  void Enable();

  //! Disables event processing and interaction handlers.
  void Disable();

  //! Set entry of allowed annotation presentations.
  void SetEditEntry( const QString& theEntry ) { myEditEntry = theEntry; }

protected:

  //! Connect interactor's event handler to the view window given.
  void ConnectView( SUIT_ViewWindow* theWindow );

  //! Disconnect interactor's event handler from the view window given.
  void DisconnectView( SUIT_ViewWindow* theWindow );

signals:

  //! Emitted when interactor begins modification of the interactive object.
  void SignalInteractionStarted( Handle_GEOM_Annotation theIO );

  //! Emitted when interactor finished modification of the interactive object.
  void SignalInteractionFinished( Handle_GEOM_Annotation theIO );

protected slots:

  //! Handler for signal coming from GUI layer.
  void OnViewCreated( SUIT_ViewWindow* );

  //! Handler for signal coming from GUI layer.
  void OnViewRemoved( SUIT_ViewWindow* );

protected:

  //! Hooks and process events from OCCT viewer prior to their coming into the base viewer class.
  //! It handles the events coming to viewport and identifies whether the events can
  //! invoke an interaction operation or not. If yes, the operation is performed within the
  //! interactor class and events are "accepted". Otherwise, the events are passed to
  //! viewer's subroutines.
  virtual bool eventFilter( QObject*, QEvent* );

private:

  GeometryGUI*              myGeomGUI;
  bool                      myIsEnabled;
  OCCViewer_ViewManager*    myVM;
  OCCViewer_Viewer*         myViewer;
  OCCViewer_ViewPort3d*     myActiveViewPort;
  Handle(GEOM_Annotation)   myActiveIO;
  SelectMgr_SequenceOfOwner mySelection;
  QPoint                    myStartPoint;
  QString                   myEditEntry;
};

#endif
