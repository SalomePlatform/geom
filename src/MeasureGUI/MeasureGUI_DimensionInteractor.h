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
// File   : MeasureGUI_DimensionEditor.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_DIMENSIONINTERACTOR_H
#define MEASUREGUI_DIMENSIONINTERACTOR_H

#include <GeometryGUI.h>
#include <QObject>

#include <SelectMgr_EntityOwner.hxx>
#include <NCollection_Sequence.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Dimension.hxx>
#include <V3d_View.hxx>
#include <gp_Pln.hxx>

class OCCViewer_ViewManager;
class OCCViewer_Viewer;
class SUIT_ViewWindow;

//=================================================================================
// class    : MeasureGUI_DimensionInteractor
// purpose  : Interactive editor of dimension presentations
//=================================================================================
class MeasureGUI_DimensionInteractor : public QObject
{
  Q_OBJECT

public:
  enum Operation
  {
    Operation_MoveFlyoutFree,
    Operation_MoveFlyoutInPlane,
    Operation_MoveText,
    Operation_None
  };

public:
  MeasureGUI_DimensionInteractor( GeometryGUI*, QObject* );
  ~MeasureGUI_DimensionInteractor();

public:
  void                   Enable();
  void                   Disable();

protected:
  /*!
   * \brief Identify interactive operation by picked entity and pressed buttons with modifiers
   *
   * \param theEntity [in] the picked entity.
   * \param theButtons [in] the mouse buttons.
   * \param theKeys [in] the keyboard modifiers.
   */
  Operation              GetOperation( const Handle(SelectMgr_EntityOwner)& theEntity,
                                       const Qt::MouseButtons theButtons,
                                       const Qt::KeyboardModifiers theKeys );

  /*!
   * \brief Prepare for interactive operation.
   * 
   * \param theOp [in] the operation.
   * \param theView [in] the interacted view.
   * \param theX [in] the mouse position x.
   * \param theY [in] the mouse position y.
   * \return TRUE if operation can be started.
   */
  bool                  StartOperation( const Operation theOperation,
                                        const Handle(V3d_View)& theView,
                                        const int theX,
                                        const int theY );

  /*!
   * \brief Perform "Operation_MoveFlyoutFree".
   * Modify flyout of interactive dimension so as the flyout follows
   * the mouse by its length and orientation.
   *
   * \param theView [in] the interacted view.
   * \param theX [in] the mouse position x.
   * \param theY [in] the mouse position y.
   */
  void                   MoveFlyoutFree( const Handle(V3d_View)& theView, const int theX, const int theY );

  /*!
   * \brief Perform "Operation_MoveFlyoutInPlane".
   * Modify flyout of interactive dimension so as the flyout is extended
   * to the mouse point location in the presentation plane.
   *
   * \param theView [in] the interacted view.
   * \param theX [in] the mouse position x.
   * \param theY [in] the mouse position y.
   */
  void                   MoveFlyoutInPlane( const Handle(V3d_View)& theView, const int theX, const int theY );

  /*!
   * \brief Perform "Operation_MoveText". 
   * Modify text of interactive dimension so as the text is moved to its fixed positions.
   *
   * \param theView [in] the interacted view.
   * \param theX [in] the mouse position x.
   * \param theY [in] the mouse position y.
   */
  void                   MoveText( const Handle(V3d_View)& theView, const int theX, const int theY );

signals:
  void                   InteractionStarted( Handle_AIS_InteractiveObject theIO );
  void                   InteractionFinished( Handle_AIS_InteractiveObject theIO );

protected:
  void                   ConnectView( SUIT_ViewWindow* );
  void                   DisconnectView( SUIT_ViewWindow* );
  gp_Lin                 Projection( const Handle(V3d_View)&, const int, const int );
  gp_Pnt                 ProjectPlane( const Handle(V3d_View)&, 
                                       const int,
                                       const int,
                                       const gp_Pln&,
                                       bool& );
  Standard_Real          SensitivityTolerance( const Handle(V3d_View)& );

protected:
  /*!
   * \brief Process events from OCC viewer prior to their coming into the base viewer class.
   *
   * It handles the events coming to viewport and identifies whether the events correspond to
   * interactive operation on dimension. If yes, the operation is performed within the
   * interactor class and events are "accepted". Otherwise, the events are passed to
   * viewer subroutines.
   *
   * The method manages internal workflow related to starting and stopping interactive 
   * operations to modify the presentations which were passed for this interactor class.
   *
   */
  virtual bool           eventFilter( QObject*, QEvent* );

protected slots:
  void                   OnViewCreated( SUIT_ViewWindow* );
  void                   OnViewRemoved( SUIT_ViewWindow* );

private:
  typedef NCollection_Sequence<Handle(SelectMgr_EntityOwner)> SeqOfOwners;

private:
  GeometryGUI*            myGeomGUI;
  bool                    myIsEnabled;
  OCCViewer_ViewManager*  myVM;
  OCCViewer_Viewer*       myViewer;
  Operation               myOperation;
  bool                    myOperationStarted;
  gp_Pln                  myFreeMovePlane;
  Handle(AIS_Dimension)   myInteractedIO;
  SeqOfOwners             mySelection;
};

#endif
