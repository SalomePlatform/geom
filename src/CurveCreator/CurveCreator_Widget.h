// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef CURVECREATOR_WIDGET_H
#define CURVECREATOR_WIDGET_H

#include "CurveCreator_Macro.hxx"
#include "CurveCreator.hxx"
#include "CurveCreator_ICurve.hxx"

#include <SUIT_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <Geom_Curve.hxx>
#include <V3d_View.hxx>
#include <gp_Pnt.hxx>

#include <QWidget>
#include <QMap>

class CurveCreator_TableView;
class CurveCreator_TreeView;
class CurveCreator_NewSectionDlg;

class OCCViewer_Viewer;
class OCCViewer_ViewPort3d;

class QAction;
class QPixmap;

class CURVECREATOR_EXPORT CurveCreator_Widget : public QWidget
{
  Q_OBJECT

public:
  enum ActionFlags {
    NoFlags              = 0x00000000,
    DisableDetectionMode = 0x00000001,
    DisableNewSection    = 0x00000002,
    DisableClosedSection = 0x00000004
  };

  enum ActionMode {
    NoneMode,
    AdditionMode,
    ModificationMode,
    DetectionMode
  };

public:
  explicit CurveCreator_Widget( QWidget* parent,
                                CurveCreator_ICurve *theCurve,
                                const int theActionFlags = NoFlags,
                                const QStringList& theCoordTitles = QStringList(),
                                Qt::WindowFlags fl=0,
                                int theLocalPointRowLimit = 20);

  // OCC viewer manipulation
  void setOCCViewer( OCCViewer_Viewer* theViewer );

  Handle(AIS_InteractiveContext) getAISContext();
  OCCViewer_ViewPort3d* getViewPort();
  int changeInteractionStyle( int theStyle );

  void reset();
  void setCurve( CurveCreator_ICurve* theCurve );

  QList<int> getSelectedSections();
  void setSelectedSections( const QList<int>& theSections );

  void  removeSelected();
  bool  removeEnabled();

  void setActionMode( const ActionMode& theMode );
  ActionMode getActionMode() const;

  void SetViewer2DMode(const bool To2D);

signals:
  void selectionChanged();
  void subOperationStarted( QWidget*, bool );
  void subOperationFinished( QWidget* );

public slots:

protected slots:
  void     onAdditionMode(bool checked);
  void     onModificationMode(bool checked);
  void     onDetectionMode(bool checked);
  void     onModeChanged(bool checked);
  void     onNewSection();
  void     onSelectionChanged();
  void     onAddNewSection();
  void     onEditSection( int theSection );
  void     onModifySection();
  void     onCancelSection();
  void     onJoin();
  void     onBringTogether();
  void     onRemove();
  void     onClearAll();
  void     onJoinAll();
  void     onSetSpline();
  void     onSetPolyline();
  void     onCloseSections();
  void     onUncloseSections();
  void     onUndo();
  void     onRedo();
  void     onUndoSettings();
  void     onContextMenu(QPoint thePoint);

  void     onMousePress( SUIT_ViewWindow*, QMouseEvent* theEvent );
  void     onMouseRelease( SUIT_ViewWindow*, QMouseEvent* theEvent );
  void     onMouseMove( SUIT_ViewWindow*, QMouseEvent* theEvent );
  void     onLastViewClosed( SUIT_ViewManager* theManager );

  void     onMousePress( QMouseEvent* theEvent );
  void     onMouseRelease( QMouseEvent* theEvent );
  void     onMouseMove( QMouseEvent* theEvent );

  void     onCellChanged( int theRow, int theColumn );

protected:
  void     addCoordsByClick( QMouseEvent* );

protected:
  enum ActionId{ NONE_ID,
                 UNDO_ID, 
                 REDO_ID, 
                 NEW_SECTION_ID, 
                 ADDITION_MODE_ID, 
                 REMOVE_ID, 
                 REMOVE_ALL_ID, 
                 JOIN_ID,
                 JOIN_ALL_ID, 
                 CLOSE_SECTIONS_ID, 
                 UNCLOSE_SECTIONS_ID,
                 SET_SECTIONS_POLYLINE_ID, 
                 SET_SECTIONS_SPLINE_ID, 
                 CLEAR_ALL_ID, 
                 SEPARATOR_ID, 
                 MODIFICATION_MODE_ID, 
                 DETECTION_MODE_ID,
                 BRING_TOGETHER_ID
  };

private:
  OCCViewer_Viewer* getOCCViewer();

  QAction* createAction( ActionId theId, const QString& theName, const QPixmap& theImage,
                         const QString& theToolTip, const QKeySequence& theShortcut );
  QAction* getAction(ActionId theId);
  QAction* getAction(ActionMode theMode);

  void updateActionsStates();
  void updateUndoRedo();

  void removeSection();
  void removePoint();
  void addNewPoint(const CurveCreator::Coordinates& theCoords);
  void insertPointToSelectedSegment( const int theXPosition,
                                     const int theYPosition );
  void moveSelectedPoints( const int theXPosition, const int theYPosition );
  void updateLocalPointView();
  void setLocalPointContext( const bool theOpen, const bool isUpdateTable = false );

  void setDragStarted( const bool theState, const QPoint& thePoint = QPoint() );

  void getSelectedPoints( CurveCreator_ICurve::SectionToPointList& thePoints );
  void setSelectedPoints( const CurveCreator_ICurve::SectionToPointList& =
                               CurveCreator_ICurve::SectionToPointList() );

  void stopActionMode();

  void startCurveModification( CurveCreator_ICurve::SectionToPointList& thePoints,
                               const bool theFillPoints = true );
  void finishCurveModification( const CurveCreator_ICurve::SectionToPointList& thePoints = 
                                      CurveCreator_ICurve::SectionToPointList() );

  // curve algorithm
  int  findLocalPointIndex( int theSectionId, double theX, double theY );
  void findSectionsToPoints( const double theX, const double theY,
                             CurveCreator_ICurve::SectionToPointList& thePoints );
  void convert( const CurveCreator_ICurve::SectionToPointList& thePoints,
                QMap<int, QList<int> >& theConvPoints );

  bool contains( const CurveCreator_ICurve::SectionToPointList& theList,
                 const CurveCreator_ICurve::SectionToPoint& theValue ) const;

protected:
  // Boundary points of mouse to select the points
  QPoint myStartPoint;
  QPoint myEndPoint;

private:
  QMap<ActionId, QAction*>    myActionMap;
  CurveCreator_ICurve*        myCurve;
  CurveCreator_TreeView*      mySectionView;
  CurveCreator_TableView*     myLocalPointView;
  CurveCreator_ICurve::SectionToPointList myLocalPoints;
  CurveCreator_NewSectionDlg* myNewSectionEditor;
  OCCViewer_Viewer*           myOCCViewer;
  int                         myLocalPointRowLimit;
  int                         mySection;
  int                         myPointNum;
  bool                        myDragStarted;
  QPoint                      myDragStartPosition;
  int                         myDragInteractionStyle;
  CurveCreator_ICurve::SectionToPointList myDragPoints;
  QMap<CurveCreator_ICurve::SectionToPoint, CurveCreator::Coordinates> myInitialDragPointsCoords;
  bool                        myDragged;
  QByteArray                  myGuiState;
  OCCViewer_ViewWindow::Mode2dType myOld2DMode;
};

#endif // CURVECREATOR_WIDGET_H
