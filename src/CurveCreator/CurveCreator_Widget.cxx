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

#include "CurveCreator_Widget.h"
#include "CurveCreator_TreeView.h"
#include "CurveCreator_Curve.hxx"
#include "CurveCreator_CurveEditor.hxx"
#include "CurveCreator.hxx"
#include "CurveCreator_NewPointDlg.h"
#include "CurveCreator_NewSectionDlg.h"

#include <GEOMUtils.hxx>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewPort3d.h>

#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QToolButton>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>

CurveCreator_Widget::CurveCreator_Widget(QWidget* parent,
                                         CurveCreator_Curve *theCurve,
                                         Qt::WindowFlags fl) :
    QWidget(parent), myNewPointEditor(NULL), myNewSectionEditor(NULL), myEdit(NULL), myCurve(theCurve)
{
    if( myCurve )
        myEdit = new CurveCreator_CurveEditor( myCurve );

    CurveCreator::Dimension aDim = CurveCreator::Dim2d;
    if( myCurve )
        aDim = myCurve->getDimension();
    myNewPointEditor = new CurveCreator_NewPointDlg( aDim, this );
    myNewPointEditor->hide();
//    connect( myNewPointEditor, SIGNAL(addPoint()), this, SLOT(onAddNewPoint()) );
    connect( myNewPointEditor, SIGNAL(modifyPoint()), this, SLOT(onModifyPoint()) );
    connect( myNewPointEditor, SIGNAL(cancelPoint()), this, SLOT(onCancelPoint()) );

    myNewSectionEditor = new CurveCreator_NewSectionDlg( this );
    myNewSectionEditor->hide();
    connect( myNewSectionEditor, SIGNAL(addSection()), this, SLOT(onAddNewSection()) );
    connect( myNewSectionEditor, SIGNAL(modifySection()), this, SLOT(onModifySection()) );
    connect( myNewSectionEditor, SIGNAL(cancelSection()), this, SLOT(onCancelSection()) );

    QGroupBox* aSectionGroup = new QGroupBox(tr("Sections"),this);

    mySectionView = new CurveCreator_TreeView(myCurve, aSectionGroup);
    connect( mySectionView, SIGNAL(selectionChanged()), this, SLOT( onSelectionChanged() ) );
    connect( mySectionView, SIGNAL(pointEntered(int,int)), this, SLOT(onEditPoint(int,int)) );
    connect( mySectionView, SIGNAL(sectionEntered(int)), this, SLOT(onEditSection(int)) );
    connect( mySectionView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)) );
    QToolBar* aTB = new QToolBar(tr("TOOL_BAR_TLT"), aSectionGroup);
//    QToolButton* anUndoBtn = new QToolButton(aTB);

    SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
    QPixmap anUndoPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_UNDO")));
    QPixmap aRedoPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_REDO")));
    QPixmap aNewSectionPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_NEW_SECTION")));
    QPixmap aNewPointPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_NEW_POINT")));
    QPixmap anEditPointsPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_EDIT_POINTS")));
    QPixmap aDetectPointsPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_EDIT_POINTS")));
    QPixmap aPolylinePixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_POLYLINE")));
    QPixmap aSplinePixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_SPLINE")));
    QPixmap aRemovePixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_DELETE")));
    QPixmap aJoinPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_JOIN")));
    QPixmap aStepUpPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_ARROW_UP")));
    QPixmap aStepDownPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_ARROW_DOWN")));

/*    QPixmap anUndoPixmap = QPixmap(tr(":images/ICON_UNDO"));
    QPixmap aRedoPixmap = QPixmap(tr(":images/ICON_REDO"));
    QPixmap aNewSectionPixmap = QPixmap(tr(":images/ICON_NEW_SECTION"));
    QPixmap aNewPointPixmap = QPixmap(tr(":images/ICON_NEW_POINT"));
    QPixmap aPolylinePixmap = QPixmap(tr(":images/ICON_POLYLINE"));
    QPixmap aSplinePixmap = QPixmap(tr(":images/ICON_SPLINE"));
    QPixmap aRemovePixmap = QPixmap(tr(":images/ICON_REMOVE"));
    QPixmap aJoinPixmap = QPixmap(tr(":images/ICON_JOIN"));
    QPixmap aStepUpPixmap = QPixmap(tr(":images/ICON_STEP_UP"));
    QPixmap aStepDownPixmap = QPixmap(tr(":images/ICON_STEP_DOWN"));*/

    QAction* anAct = createAction( UNDO_ID, tr("UNDO"), anUndoPixmap, tr("UNDO_TLT"), 
                                   QKeySequence(Qt::ControlModifier|Qt::Key_Z) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onUndo()) );
    aTB->addAction(anAct);

    anAct = createAction( REDO_ID, tr("REDO"), aRedoPixmap, tr("REDO_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_Y) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onRedo()) );
    aTB->addAction(anAct);

    aTB->addSeparator();

    anAct = createAction( NEW_SECTION_ID, tr("NEW_SECTION"), aNewSectionPixmap, tr("NEW_SECTION_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_N) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onNewSection()) );
    aTB->addAction(anAct);
    aTB->addSeparator();

    anAct = createAction( INSERT_SECTION_BEFORE_ID, tr("INSERT_SECTION_BEFORE"), QPixmap(), 
                          tr("INSERT_SECTION_BEFORE_TLT"),
                          QKeySequence(Qt::ControlModifier | Qt::Key_Insert ) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onInsertSectionBefore()) );

    anAct = createAction( INSERT_SECTION_AFTER_ID, tr("INSERT_SECTION_AFTER"), QPixmap(), 
                          tr("INSERT_SECTION_AFTER_TLT"),
                          QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_Insert ) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onInsertSectionAfter()) );

    anAct = createAction( ADDITION_MODE_ID, tr("ADDITION_MODE"), aNewPointPixmap, tr("ADDITION_MODE_TLT"), 
                          QKeySequence() );
    anAct->setCheckable(true);
    connect(anAct, SIGNAL(triggered(bool)), this, SLOT(onAdditionMode(bool)) );
    connect(anAct, SIGNAL(toggled(bool)), this, SLOT(onModeChanged(bool)) );
    aTB->addAction(anAct);
    
    anAct = createAction( MODIFICATION_MODE_ID, tr("MODIFICATION_MODE"), anEditPointsPixmap, tr("MODIFICATION_MODE_TLT"), 
                          QKeySequence() );
    anAct->setCheckable(true);
    connect(anAct, SIGNAL(triggered(bool)), this, SLOT(onModificationMode(bool)) );
    connect(anAct, SIGNAL(toggled(bool)), this, SLOT(onModeChanged(bool)) );
    aTB->addAction(anAct);

    anAct = createAction( DETECTION_MODE_ID, tr("DETECTION_MODE"), aDetectPointsPixmap, tr("DETECTION_MODE_TLT"), 
                          QKeySequence() );
    anAct->setCheckable(true);
    connect(anAct, SIGNAL(triggered(bool)), this, SLOT(onDetectPoints(bool)) );
    connect(anAct, SIGNAL(toggled(bool)), this, SLOT(onModeChanged(bool)) );
    aTB->addAction(anAct);

    anAct = createAction( INSERT_POINT_BEFORE_ID, tr("INSERT_POINT_BEFORE"), QPixmap(), 
                          tr("INSERT_POINT_BEFORE_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_B) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onInsertPointBefore()) );

    anAct = createAction( INSERT_POINT_AFTER_ID, tr("INSERT_POINT_AFTER"), QPixmap(), 
                           tr("INSERT_POINT_AFTER_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_M) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onInsertPointAfter()) );
                                 
    anAct = createAction( CLOSE_SECTIONS_ID, tr("CLOSE_SECTIONS"), QPixmap(), tr("CLOSE_SECTIONS_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_W) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onCloseSections()) );

    anAct = createAction( UNCLOSE_SECTIONS_ID, tr("UNCLOSE_SECTIONS"), QPixmap(), 
                          tr("UNCLOSE_SECTIONS_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_S) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onUncloseSections()) );

    anAct = createAction( SET_SECTIONS_POLYLINE_ID, tr("SET_SECTIONS_POLYLINE"), 
                          aPolylinePixmap, tr("SET_POLYLINE_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_E) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onSetPolyline()) );

    anAct = createAction( SET_SECTIONS_SPLINE_ID, tr("SET_SECTIONS_SPLINE"), aSplinePixmap, 
                          tr("SET_SPLINE_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_R) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onSetSpline()) );

    anAct = createAction( REMOVE_ID, tr("REMOVE"), aRemovePixmap, tr("REMOVE_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_Delete ) );
    connect(anAct, SIGNAL(triggered()), this, SLOT(onRemove()) );
    aTB->addAction(anAct);
    aTB->addSeparator();

    anAct = createAction( JOIN_ID, tr("JOIN"), aJoinPixmap, tr("JOIN_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_Plus ) );
    connect( anAct, SIGNAL(triggered()), this, SLOT(onJoin()) );
    aTB->addAction(anAct);
    aTB->addSeparator();

    anAct = createAction( UP_ID, tr("STEP_UP"), aStepUpPixmap, tr("STEP_UP_TLT"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_Up ) );
    connect( anAct, SIGNAL(triggered()), this, SLOT(onMoveUp()) );

    anAct = createAction( DOWN_ID, tr("STEP_DOWN"), aStepDownPixmap, tr("STEP_DOWN"), 
                          QKeySequence(Qt::ControlModifier|Qt::Key_Down ) );
    connect( anAct, SIGNAL(triggered()), this, SLOT(onMoveDown()) );

    anAct = createAction( CLEAR_ALL_ID, tr("CLEAR_ALL"), QPixmap(), tr("CLEAR_ALL_TLT"), 
                          QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_Delete ) );
    connect( anAct, SIGNAL(triggered()), this, SLOT( onClearAll()) );

    anAct = createAction( JOIN_ALL_ID, tr("JOIN_ALL"), QPixmap(), tr("JOIN_ALL_TLT"), 
                          QKeySequence(Qt::ControlModifier | Qt::ShiftModifier | Qt::Key_Plus ) );
    connect( anAct, SIGNAL(triggered()), this, SLOT(onJoinAll()) );

    QVBoxLayout* aSectLayout = new QVBoxLayout();
    aSectLayout->setMargin( 5 );
    aSectLayout->setSpacing( 5 );
    aSectLayout->addWidget(aTB);
    aSectLayout->addWidget(mySectionView);
    aSectionGroup->setLayout(aSectLayout);
    QVBoxLayout* aLay = new QVBoxLayout();
    aLay->setMargin( 0 );
    aLay->setSpacing( 5 );
//    aLay->addLayout(aNameLayout);
    aLay->addWidget(aSectionGroup);
    setLayout(aLay);
    onSelectionChanged();
}

void CurveCreator_Widget::setCurve( CurveCreator_Curve* theCurve )
{
  if( myEdit != NULL ){
    delete myEdit;
    myEdit = NULL;
  }
  myCurve = theCurve;
  mySectionView->setCurve(myCurve);
  if( myCurve != NULL ){
    myEdit = new CurveCreator_CurveEditor(myCurve);
  }
  onSelectionChanged();
  updateUndoRedo();
}

void CurveCreator_Widget::onSelectionChanged()
{
  QList<ActionId> anEnabledAct;
  if( myCurve ){
    anEnabledAct << NEW_SECTION_ID;
    QList<int> aSelSections = mySectionView->getSelectedSections();
    QList< QPair< int, int > > aSelPoints = mySectionView->getSelectedPoints();
    CurveCreator_TreeView::SelectionType aSelType = mySectionView->getSelectionType();
    switch( aSelType ){
    case CurveCreator_TreeView::ST_NOSEL:{
      break;
    }
    case CurveCreator_TreeView::ST_SECTIONS:{
      /*if( aSelSections[0] > 0 ){
        anEnabledAct << UP_ID;
      }*/
      if( aSelSections.size() == 1 ){
        anEnabledAct << ADDITION_MODE_ID << MODIFICATION_MODE_ID << DETECTION_MODE_ID;
      }
      if (myActionMap[ADDITION_MODE_ID]->isChecked()) {
        mySection = -1;
        myPointNum = -1;
        QList<int> aSelSection = mySectionView->getSelectedSections();
        if( aSelSection.size() > 0 ){
          mySection = aSelSection[0];
          myPointNum = myCurve->getNbPoints(mySection);
        }
      } else if (myActionMap[MODIFICATION_MODE_ID]->isChecked()) {
        anEnabledAct << REMOVE_ID;
        anEnabledAct << CLOSE_SECTIONS_ID << UNCLOSE_SECTIONS_ID << SET_SECTIONS_POLYLINE_ID << SET_SECTIONS_SPLINE_ID;
        int aSectCnt = myCurve->getNbSections();
        if( aSectCnt > 0 )
          anEnabledAct << CLEAR_ALL_ID;
        if( aSectCnt > 1 )
          anEnabledAct << JOIN_ALL_ID;
        if( aSelSections.size() > 1 ){
          anEnabledAct << JOIN_ID;
        }
      } else if (myActionMap[DETECTION_MODE_ID]->isChecked()) {
      } else { //no active mode
      }
      /*if( aSelSections[ aSelSections.size() - 1 ] < ( myCurve->getNbSections() - 1 ) ){
        anEnabledAct << DOWN_ID;
      }*/
      break;
    }
    /*case CurveCreator_TreeView::ST_POINTS_ONE_SECTION:{
      if( aSelPoints[0].second > 0 ){
        anEnabledAct << UP_ID;
      }
      int aLastIndex = aSelPoints.size()-1;
      int aSect = aSelPoints[0].first;
      if( aSelPoints[aLastIndex].second < (myCurve->getNbPoints(aSect) - 1)){
        anEnabledAct << DOWN_ID;
      }
      if( aSelPoints.size() == 1){
        anEnabledAct << INSERT_POINT_BEFORE_ID << INSERT_POINT_AFTER_ID;
      }
      break;
    }*/

    }
    
    /*int aSelObjsCnt = aSelPoints.size() + aSelSections.size();
    if( aSelObjsCnt > 0 ){
      anEnabledAct << REMOVE_ID;
    }
    if( (myCurve->getNbSections() + myCurve->getNbPoints()) > 0 ){
      anEnabledAct << REMOVE_ALL_ID;
    }*/
    if( myCurve->getNbSections() > 1 ){
      anEnabledAct << JOIN_ALL_ID;
    }
  }
  QList<ActionId> anIds = myActionMap.keys();
  for( int i = 0 ; i < anIds.size() ; i++ ){
    if( myActionMap.contains(anIds[i]) ){
      if( anEnabledAct.contains(anIds[i]) ){
        myActionMap[anIds[i]]->setEnabled(true);
      }
      else{
        myActionMap[anIds[i]]->setEnabled(false);
      }
    }
  }
  emit selectionChanged();
}

void CurveCreator_Widget::onAdditionMode(bool checked)
{
  if( !myEdit )
    return;

  SUIT_ViewWindow* aViewWindow = 0;
  SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
  if ( activeStudy )
    aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( aViewWindow == 0 )
    return;
  SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();

  if ( aViewManager->getType() == OCCViewer_Viewer::Type() ) {
    if (checked) {
      connect( aViewManager, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
             this, SLOT( onGetPointByClick( SUIT_ViewWindow*, QMouseEvent* ) ) );
    } else {
      disconnect( aViewManager, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
             this, SLOT( onGetPointByClick( SUIT_ViewWindow*, QMouseEvent* ) ) );
      return;
    }
  }

  mySection= -1;
  myPointNum = -1;
  QList<int> aSelSection = mySectionView->getSelectedSections();
  if( aSelSection.size() > 0 ){
    mySection = aSelSection[0];
  }
  else{
    QList< QPair<int,int> > aSelPoints = mySectionView->getSelectedPoints();
    if( aSelPoints.size() > 0 ){
      mySection = aSelPoints[0].first;
      myPointNum = aSelPoints[0].second + 1;
    }
  }
/*
  QString aSectName;
  if( mySection < 0 ){
    mySection = myCurve->getNbSections() - 1;
  }
  aSectName = QString::fromStdString( myCurve->getSectionName(mySection));
  if( myPointNum < 0 ){
    myPointNum = myCurve->getNbPoints(mySection);
  }
  myNewPointEditor->clear();
  myNewPointEditor->setEditMode(false);
  myNewPointEditor->setSectionName(aSectName);
  myNewPointEditor->setDimension(myCurve->getDimension());
*/
//  emit subOperationStarted( myNewPointEditor );
}

void CurveCreator_Widget::onModificationMode(bool checked)
{
  SUIT_ViewWindow* aViewWindow = 0;
  SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
  if ( activeStudy )
    aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( aViewWindow == 0 )
    return;
  SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
  if ( aViewManager->getType() == OCCViewer_Viewer::Type() ) {
    if (checked) {
//      connect( aViewManager, SIGNAL( mouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ),
//             this, SLOT( onPointSelect( SUIT_ViewWindow*, QMouseEvent* ) ) );
      connect( aViewManager, SIGNAL( mouseMove( SUIT_ViewWindow*, QMouseEvent* ) ),
             this, SLOT( onPointDrag( SUIT_ViewWindow*, QMouseEvent* ) ) );
    }
    else {
//      disconnect( aViewManager, SIGNAL( mouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ),
//             this, SLOT( onPointSelect( SUIT_ViewWindow*, QMouseEvent* ) ) );
      disconnect( aViewManager, SIGNAL( mouseMove( SUIT_ViewWindow*, QMouseEvent* ) ),
             this, SLOT( onPointDrag( SUIT_ViewWindow*, QMouseEvent* ) ) );
      return;
    }
  }
}

void CurveCreator_Widget::onDetectPoints(bool checked)
{
}

void CurveCreator_Widget::onModeChanged(bool checked)
{
  if (checked) {
    QAction* anAction = (QAction*)sender();
    switch(myActionMap.key(anAction)) {
      case ADDITION_MODE_ID:
        if (myActionMap[MODIFICATION_MODE_ID]->isChecked())
          myActionMap[MODIFICATION_MODE_ID]->trigger();
        else if (myActionMap[DETECTION_MODE_ID]->isChecked())
          myActionMap[DETECTION_MODE_ID]->trigger();
        break;
      case MODIFICATION_MODE_ID:
        if (myActionMap[ADDITION_MODE_ID]->isChecked())
          myActionMap[ADDITION_MODE_ID]->trigger();
        else if (myActionMap[DETECTION_MODE_ID]->isChecked())
          myActionMap[DETECTION_MODE_ID]->trigger();
        break;
      case DETECTION_MODE_ID:
        if (myActionMap[ADDITION_MODE_ID]->isChecked())
          myActionMap[ADDITION_MODE_ID]->trigger();
        else if (myActionMap[MODIFICATION_MODE_ID]->isChecked())
          myActionMap[MODIFICATION_MODE_ID]->trigger();
        break;
    }
  }
  onSelectionChanged();
}

void CurveCreator_Widget::onAddNewPoint(const CurveCreator::Coordinates& theCoords)
{
  if( !myEdit )
    return;
//  CurveCreator::Coordinates aCoords = myNewPointEditor->getCoordinates();
  myEdit->insertPoints(theCoords, mySection, myPointNum );
  mySectionView->pointsAdded( mySection, myPointNum );
//  myNewPointEditor->clear();
  myPointNum++;
  onSelectionChanged();
  updateUndoRedo();
}

void CurveCreator_Widget::onNewSection()
{
  if( !myEdit )
    return;
  myNewSectionEditor->clear();
  myNewSectionEditor->setEditMode(false);
  QString aSectName = QString( myCurve->getUnicSectionName().c_str() );
  myNewSectionEditor->setSectionParameters(aSectName, true, CurveCreator::Polyline );
  emit subOperationStarted( myNewSectionEditor );
}

void CurveCreator_Widget::onAddNewSection()
{
  if( !myEdit )
    return;
  CurveCreator::Coordinates aCoords;
  myEdit->addSection( myNewSectionEditor->getName().toStdString(), myNewSectionEditor->getSectionType(),
                      myNewSectionEditor->isClosed(), aCoords  );
  mySectionView->sectionAdded( mySection );
  QString aNewName = QString(myCurve->getUnicSectionName().c_str());
  myNewSectionEditor->setSectionName(aNewName);
  mySection++;
  onSelectionChanged();
  updateUndoRedo();
  onCancelSection();
}

void CurveCreator_Widget::onCancelPoint()
{
  emit subOperationFinished( myNewPointEditor );
}

void CurveCreator_Widget::onCancelSection()
{
  emit subOperationFinished( myNewSectionEditor );
}

QAction* CurveCreator_Widget::createAction( ActionId theId, const QString& theName, const QPixmap& theImage,
                                            const QString& theToolTip, const QKeySequence& theShortcut )
{
  QAction* anAct = new QAction(theName,this);
  if( !theImage.isNull() ){
    anAct->setIcon(theImage);
  }
  anAct->setShortcut(theShortcut);
  anAct->setToolTip(theToolTip);
  myActionMap[theId] = anAct;
  return anAct;
}

QAction* CurveCreator_Widget::getAction(ActionId theId)
{
  if( myActionMap.contains(theId) )
    return myActionMap[theId];
  return NULL;
}

void CurveCreator_Widget::onEditSection( int theSection )
{
  if( !myEdit )
    return;
  mySection = theSection;
  QString aSectName = QString::fromStdString( myCurve->getSectionName(theSection));
  bool isClosed = myCurve->isClosed(theSection);
  CurveCreator::Type aType = myCurve->getType(theSection);
  myNewSectionEditor->setEditMode(true);
  myNewSectionEditor->setSectionParameters( aSectName, isClosed, aType );

  emit subOperationStarted( myNewSectionEditor );
}

void CurveCreator_Widget::onModifySection()
{
  if( !myEdit )
    return;
  QString aName = myNewSectionEditor->getName();
  bool isClosed = myNewSectionEditor->isClosed();
  CurveCreator::Type aSectType = myNewSectionEditor->getSectionType();
  myEdit->startOperation();
  myEdit->setClosed( isClosed, mySection );
  myEdit->setName( aName.toStdString(), mySection );
  myEdit->setType( aSectType, mySection );
  myEdit->finishOperation();
  mySectionView->sectionChanged(mySection);
  updateUndoRedo();
  onCancelSection();
}

void CurveCreator_Widget::onEditPoint( int theSection, int thePoint )
{
  if( !myNewPointEditor || !myEdit )
    return;
  mySection = theSection;
  myPointNum = thePoint;
  QString aSectName = QString::fromStdString( myCurve->getSectionName(theSection));
  myNewPointEditor->setEditMode(true);
  myNewPointEditor->setSectionName(aSectName);
  myNewPointEditor->setDimension( myCurve->getDimension() );
  CurveCreator::Coordinates aCoords = myCurve->getCoordinates(theSection,thePoint);
  myNewPointEditor->setCoordinates(aCoords);
  emit subOperationStarted( myNewPointEditor );
}

void CurveCreator_Widget::onModifyPoint()
{
  if( !myEdit )
    return;
  CurveCreator::Coordinates aCoords = myNewPointEditor->getCoordinates();
  myEdit->setCoordinates( aCoords, mySection, myPointNum );
  mySectionView->pointDataChanged( mySection, myPointNum );
  updateUndoRedo();
  onCancelPoint();
}

void CurveCreator_Widget::onJoin()
{
  if( !myEdit )
    return;
  QList<int> aSections = mySectionView->getSelectedSections();
  if( aSections.size() == 0 ){
    return;
  }
  int aMainSect = aSections[0];
  int aMainSectSize = myCurve->getNbPoints(aMainSect);
  myEdit->startOperation();
  for( int i = 1 ; i < aSections.size() ; i++ ){
    int aSectNum = aSections[i] - (i-1);
    myEdit->join( aMainSect, aSectNum );
    mySectionView->sectionsRemoved( aSectNum );
  }
  myEdit->finishOperation();
  int aNewSectSize = myCurve->getNbPoints(aMainSect);
  if( aNewSectSize != aMainSectSize )
    mySectionView->pointsAdded( aMainSect, aMainSectSize, aNewSectSize-aMainSectSize );
  updateUndoRedo();
}

void CurveCreator_Widget::onRemove()
{
  if( !myEdit )
    return;
  QList< QPair<int,int> > aSelPoints = mySectionView->getSelectedPoints();
  int aCurrSect=-1;
  int aRemoveCnt = 0;
  myEdit->startOperation();
  for( int i = 0 ; i < aSelPoints.size() ; i++ ){
    if( aCurrSect != aSelPoints[i].first ){
      aRemoveCnt = 0;
      aCurrSect = aSelPoints[i].first;
    }
    int aPntIndx = aSelPoints[i].second - aRemoveCnt;
    myEdit->removePoints(aCurrSect,aPntIndx, 1);
    mySectionView->pointsRemoved( aCurrSect, aPntIndx );
    aRemoveCnt++;
  }
  QList<int> aSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSections.size() ; i++ ){
    int aSectNum = aSections[i] - (i);
    myEdit->removeSection( aSectNum );
    mySectionView->sectionsRemoved( aSectNum );
  }
  myEdit->finishOperation();
  mySectionView->clearSelection();
  updateUndoRedo();
}

void CurveCreator_Widget::onMoveUp()
{
  if( !myEdit )
    return;
  if( mySectionView->getSelectionType() == CurveCreator_TreeView::ST_SECTIONS ){
    //Move sections
    QList<int> aSections = mySectionView->getSelectedSections();
    for( int i = 0 ; i < aSections.size() ; i++ ){
      int anIndx = aSections[i];
      myEdit->moveSection( anIndx, anIndx-1);
      mySectionView->sectionsSwapped( anIndx, -1 );
    }
  }
  else{
    //Move points
    QList< QPair<int,int> > aPoints = mySectionView->getSelectedPoints();
    for( int i = 0 ; i < aPoints.size() ; i++ ){
      int aSection = aPoints[i].first;
      int aPoint = aPoints[i].second;
      myEdit->movePoint(aSection, aPoint, aPoint-2);
      mySectionView->pointsSwapped( aSection, aPoint, -1 );
    }
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onMoveDown()
{
  if( !myEdit )
    return;
  if( mySectionView->getSelectionType() == CurveCreator_TreeView::ST_SECTIONS ){
    //Move sections
    QList<int> aSections = mySectionView->getSelectedSections();
    for( int i = aSections.size()-1 ; i >=0 ; i-- ){
      int anIndx = aSections[i];
      myEdit->moveSection( anIndx, anIndx+1);
      mySectionView->sectionsSwapped( anIndx, 1 );
    }
  }
  else{
    //Move points
    QList< QPair<int,int> > aPoints = mySectionView->getSelectedPoints();
    for( int i = aPoints.size() - 1; i >= 0 ; i--  ){
      int aSection = aPoints[i].first;
      int aPoint = aPoints[i].second;
      myEdit->movePoint(aSection, aPoint, aPoint+1);
      mySectionView->pointsSwapped( aSection, aPoint, 1 );
    }
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onClearAll()
{
  if( !myEdit )
    return;
  myEdit->clear();
  mySectionView->reset();
  onSelectionChanged();
  updateUndoRedo();
}

void CurveCreator_Widget::onJoinAll()
{
  if( !myEdit )
    return;
  myEdit->join();
  mySectionView->reset();
  onSelectionChanged();
  updateUndoRedo();
}

void CurveCreator_Widget::onInsertSectionBefore()
{

}

void CurveCreator_Widget::onInsertSectionAfter()
{

}

void CurveCreator_Widget::onInsertPointBefore()
{

}

void CurveCreator_Widget::onInsertPointAfter()
{

}

void CurveCreator_Widget::onUndoSettings()
{

}

void CurveCreator_Widget::onSetSpline()
{
  if( !myEdit )
    return;
  QList<int> aSelSections = mySectionView->getSelectedSections();
  myEdit->startOperation();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myEdit->setType(CurveCreator::BSpline, aSelSections[i]);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  myEdit->finishOperation();
  updateUndoRedo();
}

void CurveCreator_Widget::onSetPolyline()
{
  if( !myEdit )
    return;
  myEdit->startOperation();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myEdit->setType(CurveCreator::Polyline, aSelSections[i]);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  myEdit->finishOperation();
  updateUndoRedo();
}

void CurveCreator_Widget::onCloseSections()
{
  if( !myEdit )
    return;
  myEdit->startOperation();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myEdit->setClosed(true, aSelSections[i]);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  myEdit->finishOperation();
  updateUndoRedo();
}

void CurveCreator_Widget::onUncloseSections()
{
  if( !myEdit )
    return;
  myEdit->startOperation();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myEdit->setClosed(false, aSelSections[i]);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  myEdit->finishOperation();
  updateUndoRedo();
}

void CurveCreator_Widget::onUndo()
{
    if( !myEdit )
      return;
    myEdit->undo();
    mySectionView->reset();
    updateUndoRedo();
}

void CurveCreator_Widget::onRedo()
{
    if( !myEdit )
      return;
    myEdit->redo();
    mySectionView->reset();
    updateUndoRedo();
}

void CurveCreator_Widget::updateUndoRedo()
{
    QAction* anAct = myActionMap[UNDO_ID];
    if( anAct != 0 ){
        if( myEdit->getNbUndo() != 0 ){
            anAct->setEnabled(true);
        }
        else{
            anAct->setDisabled(true);
        }
    }
    anAct = myActionMap[REDO_ID];
    if( anAct != 0 ){
        if( myEdit->getNbRedo() != 0 ){
            anAct->setEnabled(true);
        }
        else{
            anAct->setDisabled(true);
        }
    }
}

void CurveCreator_Widget::onContextMenu( QPoint thePoint )
{
  QList<ActionId> aContextActions;
  aContextActions << CLEAR_ALL_ID << JOIN_ALL_ID << SEPARATOR_ID <<
                     CLOSE_SECTIONS_ID << UNCLOSE_SECTIONS_ID << SET_SECTIONS_POLYLINE_ID <<
                     SET_SECTIONS_SPLINE_ID;
  QPoint aGlPoint = mySectionView->mapToGlobal(thePoint);
  bool isVis = false;
  QList<ActionId> aResAct;
  for( int i = 0 ; i < aContextActions.size() ; i++ ){
    if( aContextActions[i] != SEPARATOR_ID ){
      if( myActionMap.contains(aContextActions[i]) ){
        QAction* anAct = myActionMap[aContextActions[i]];
        if( anAct->isEnabled() ){
          aResAct << aContextActions[i];
          isVis = true;
        }
      }
    }
    else{
      aResAct << SEPARATOR_ID;
    }
  }
  if( !isVis )
    return;

  QMenu* aMenu = new QMenu(this);
  for( int i = 0 ; i < aResAct.size() ; i++ ){
    if( aResAct[i] == SEPARATOR_ID ){
      aMenu->addSeparator();
    }
    else{
      QAction* anAct = myActionMap[aResAct[i]];
      aMenu->insertAction(NULL, anAct);
    }
  }
  aMenu->exec(aGlPoint);
}

QList<int> CurveCreator_Widget::getSelectedSections()
{
  return mySectionView->getSelectedSections();
}

QList< QPair< int, int > > CurveCreator_Widget::getSelectedPoints()
{
  return mySectionView->getSelectedPoints();
}

bool CurveCreator_Widget::isInstantSketchingEnabled() const
{
  if( myNewPointEditor )
    return myNewPointEditor->isInstantSketchingEnabled();
  return false;
}

void CurveCreator_Widget::setInstantSketchingEnabled( const bool theState )
{
  if( myNewPointEditor )
    myNewPointEditor->setInstantSketchingEnabled( theState );
}

//=================================================================================
// function : GeometryGUI::onGetPointByClick()
// purpose  : Manage mouse press events in Additon mode
//=================================================================================
void CurveCreator_Widget::onGetPointByClick( SUIT_ViewWindow* theViewWindow, QMouseEvent* pe )
{
  if ( myNewPointEditor && theViewWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() &&
       pe->modifiers() != Qt::ControlModifier ) {
    OCCViewer_Viewer* anOCCViewer =
      ( (OCCViewer_ViewManager*)( theViewWindow->getViewManager() ) )->getOCCViewer();
    Handle(AIS_InteractiveContext) ic = anOCCViewer->getAISContext();

    gp_Pnt aPnt;    

    ic->InitSelected();
    if ( pe->modifiers() == Qt::ShiftModifier )
      ic->ShiftSelect();  // Append selection
    else
      ic->Select();       // New selection

    /*TopoDS_Shape aShape;

    ic->InitSelected();
    if ( ic->MoreSelected() )
      aShape = ic->SelectedShape();

    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
      aPnt = BRep_Tool::Pnt( TopoDS::Vertex( ic->SelectedShape() ) );
    else*/
    {
      OCCViewer_ViewPort3d* vp =  ((OCCViewer_ViewWindow*)theViewWindow)->getViewPort();
      aPnt = GEOMUtils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );
    }
    // set the coordinates into dialog
    CurveCreator::Coordinates aCoords;
    aCoords.push_back( aPnt.X() );
    aCoords.push_back( aPnt.Y() );
    if ( myCurve->getDimension() == 3 ) {
      aCoords.push_back( aPnt.Z() );
    }
    onAddNewPoint(aCoords);
//    myNewPointEditor->setCoordinates( aCoords );
  }
}

//=================================================================================
// function : GeometryGUI::onPointDrag()
// purpose  : Manage mouse move events in Modification mode
//=================================================================================
void CurveCreator_Widget::onPointDrag( SUIT_ViewWindow* theViewWindow, QMouseEvent* pe )
{
  if ( !(pe->buttons() & Qt::LeftButton) )
    return;
  if ( (pe->pos() - myDragStartPosition).manhattanLength() < QApplication::startDragDistance() )
    return;
/*  
  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;
  
  mimeData->setData(mimeType, data);
  drag->setMimeData(mimeData);
  
  Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);
  */
}
