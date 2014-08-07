// Copyright (C) 2013-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "CurveCreator_Curve.hxx"
#include "CurveCreator.hxx"

#include <QWidget>
#include <QMap>

#include <SUIT_ViewWindow.h>

class QAction;
class QPixmap;
class CurveCreator_CurveEditor;
class CurveCreator_TreeView;
class CurveCreator_NewPointDlg;
class CurveCreator_NewSectionDlg;

class CURVECREATOR_EXPORT CurveCreator_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit CurveCreator_Widget( QWidget* parent,
                      CurveCreator_Curve *theCurve,
                      Qt::WindowFlags fl=0 );

    void setCurve( CurveCreator_Curve* theCurve );

    QList<int> getSelectedSections();
    QList< QPair< int, int > > getSelectedPoints();

    bool isInstantSketchingEnabled() const;
    void setInstantSketchingEnabled( const bool theState );

signals:
    void selectionChanged();
    void subOperationStarted( QWidget* );
    void subOperationFinished( QWidget* );

public slots:

protected slots:
    void     onAdditionMode(bool checked);
    void     onModificationMode(bool checked);
    void     onDetectPoints(bool checked);
    void     onModeChanged(bool checked);
    void     onNewSection();
    void     onSelectionChanged();
    void     onAddNewPoint(const CurveCreator::Coordinates& theCoords);
    void     onAddNewSection();
    void     onEditSection( int theSection );
    void     onEditPoint( int theSection, int thePoint );
    void     onModifyPoint();
    void     onModifySection();
    void     onCancelPoint();
    void     onCancelSection();
    void     onJoin();
    void     onRemove();
    void     onMoveUp();
    void     onMoveDown();
    void     onClearAll();
    void     onJoinAll();
    void     onInsertSectionBefore();
    void     onInsertSectionAfter();
    void     onSetSpline();
    void     onSetPolyline();
    void     onCloseSections();
    void     onUncloseSections();
    void     onInsertPointBefore();
    void     onInsertPointAfter();
    void     onUndo();
    void     onRedo();
    void     onUndoSettings();
    void     onContextMenu(QPoint thePoint);
    void     onGetPointByClick( SUIT_ViewWindow*, QMouseEvent* );
//    void     onPointSelect( SUIT_ViewWindow*, QMouseEvent* );
    void     onPointDrag( SUIT_ViewWindow*, QMouseEvent* );
protected:
    enum ActionId{ UNDO_ID, REDO_ID, NEW_SECTION_ID, ADDITION_MODE_ID, REMOVE_ID, REMOVE_ALL_ID, JOIN_ID,
                   JOIN_ALL_ID, UP_ID, DOWN_ID, INSERT_SECTION_BEFORE_ID, INSERT_SECTION_AFTER_ID,
                   INSERT_POINT_BEFORE_ID, INSERT_POINT_AFTER_ID, CLOSE_SECTIONS_ID, UNCLOSE_SECTIONS_ID,
                   SET_SECTIONS_POLYLINE_ID, SET_SECTIONS_SPLINE_ID, CLEAR_ALL_ID, SEPARATOR_ID, 
                   MODIFICATION_MODE_ID, DETECTION_MODE_ID };
private:
    QAction* createAction( ActionId theId, const QString& theName, const QPixmap& theImage,
                           const QString& theToolTip, const QKeySequence& theShortcut );
    QAction* getAction(ActionId theId);
    void     updateUndoRedo();
private:
    QMap<ActionId, QAction*>    myActionMap;
    CurveCreator_Curve*         myCurve;
    CurveCreator_CurveEditor*   myEdit;
    CurveCreator_TreeView*      mySectionView;
    CurveCreator_NewPointDlg*   myNewPointEditor;
    CurveCreator_NewSectionDlg* myNewSectionEditor;
    int                         mySection;
    int                         myPointNum;
    QPoint                      myDragStartPosition;
};

#endif // CURVECREATOR_WIDGET_H
