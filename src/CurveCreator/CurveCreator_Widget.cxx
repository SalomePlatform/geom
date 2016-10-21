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

#include "CurveCreator_Widget.h"
#include "CurveCreator_TreeView.h"
#include "CurveCreator_NewSectionDlg.h"
#include "CurveCreator_Utils.hxx"
#include "CurveCreator_UtilsICurve.hxx"
#include "CurveCreator_TableView.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_Utilities.h>

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
#include <QTableWidget>
#include <QTime>
#include <QSplitter>

#include <utilities.h>

//#define MEASURE_TIME

#ifdef MEASURE_TIME

  #define START_MEASURE_TIME \
    QTime aTimer;            \
    aTimer.start();          \

  #define END_MEASURE_TIME( theMsg )                      \
    double aTime = aTimer.elapsed() * 0.001;              \
    FILE* aFile = fopen( "performance", "a" );            \
    fprintf( aFile, "%s = %.3lf sec\n", theMsg, aTime );  \
    fclose( aFile );                                      \

#else

  #define START_MEASURE_TIME
  #define END_MEASURE_TIME( theMsg )

#endif






CurveCreator_Widget::CurveCreator_Widget(QWidget* parent,
                                         CurveCreator_ICurve *theCurve,
                                         const int theActionFlags,
                                         const QStringList& theCoordTitles,
                                         Qt::WindowFlags fl,
                                         int theLocalPointRowLimit )
: QWidget(parent), myNewSectionEditor(NULL), myCurve(theCurve), mySection(0),
  myDragStarted( false ), myDragInteractionStyle( SUIT_ViewModel::STANDARD ),
  myOCCViewer( 0 ), myLocalPointRowLimit( theLocalPointRowLimit ),
  myOld2DMode(OCCViewer_ViewWindow::No2dMode)
{
  bool isToEnableClosed = !( theActionFlags & DisableClosedSection );
  myNewSectionEditor = new CurveCreator_NewSectionDlg( this, isToEnableClosed );
  myNewSectionEditor->hide();
  connect( myNewSectionEditor, SIGNAL(addSection()), this, SLOT(onAddNewSection()) );
  connect( myNewSectionEditor, SIGNAL(modifySection()), this, SLOT(onModifySection()) );
  connect( myNewSectionEditor, SIGNAL(cancelSection()), this, SLOT(onCancelSection()) );

  QGroupBox* aSectionGroup = new QGroupBox(tr("SECTION_GROUP_TITLE"),this);

  mySectionView = new CurveCreator_TreeView(myCurve, aSectionGroup);
  mySectionView->setSelectionMode( QTreeView::ExtendedSelection );
  connect( mySectionView, SIGNAL(selectionChanged()), this, SLOT( onSelectionChanged() ) );
  connect( mySectionView, SIGNAL(sectionEntered(int)), this, SLOT(onEditSection(int)) );
  connect( mySectionView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)) );

  myLocalPointView = new CurveCreator_TableView( myCurve, this, theCoordTitles );
  connect( myLocalPointView, SIGNAL( cellChanged( int, int ) ),
           this, SLOT( onCellChanged( int, int ) ) );

  QToolBar* aTB = new QToolBar(tr("SECTION_GROUP_TITLE"), aSectionGroup);
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
  QPixmap aBringTogetherPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_BRING_TOGETHER")));
  QPixmap aStepUpPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_ARROW_UP")));
  QPixmap aStepDownPixmap(aResMgr->loadPixmap("GEOM", tr("ICON_CC_ARROW_DOWN")));

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
  if ( !(theActionFlags & DisableNewSection) ) {
    aTB->addAction(anAct);
    aTB->addSeparator();
  }

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
  connect(anAct, SIGNAL(triggered(bool)), this, SLOT(onDetectionMode(bool)) );
  connect(anAct, SIGNAL(toggled(bool)), this, SLOT(onModeChanged(bool)) );
//  if ( !(theActionFlags & DisableDetectionMode) ) {
//    aTB->addAction(anAct);
//  }
  
  anAct = createAction( CLOSE_SECTIONS_ID, tr("CLOSE_SECTIONS"), QPixmap(), tr("CLOSE_SECTIONS_TLT"), 
                        QKeySequence(Qt::ControlModifier|Qt::Key_W) );
  connect(anAct, SIGNAL(triggered()), this, SLOT(onCloseSections()) );

  anAct = createAction( UNCLOSE_SECTIONS_ID, tr("UNCLOSE_SECTIONS"), QPixmap(), 
                        tr("UNCLOSE_SECTIONS_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_S) );
  connect(anAct, SIGNAL(triggered()), this, SLOT(onUncloseSections()) );

  anAct = createAction( SET_SECTIONS_POLYLINE_ID, tr("SET_SECTIONS_POLYLINE"), 
                        aPolylinePixmap, tr("SET_SECTIONS_POLYLINE_TLT"), 
                        QKeySequence(Qt::ControlModifier|Qt::Key_E) );
  connect(anAct, SIGNAL(triggered()), this, SLOT(onSetPolyline()) );

  anAct = createAction( SET_SECTIONS_SPLINE_ID, tr("SET_SECTIONS_SPLINE"), aSplinePixmap, 
                        tr("SET_SECTIONS_SPLINE_TLT"), QKeySequence(Qt::ControlModifier|Qt::Key_R) );
  connect(anAct, SIGNAL(triggered()), this, SLOT(onSetSpline()) );

  aTB->addSeparator();

  anAct = createAction( REMOVE_ID, tr("REMOVE"), aRemovePixmap, tr("REMOVE_TLT"), 
                        QKeySequence(Qt::ControlModifier|Qt::Key_Delete ) );
  connect(anAct, SIGNAL(triggered()), this, SLOT(onRemove()) );
  aTB->addAction(anAct);
  
  anAct = createAction( JOIN_ID, tr("JOIN"), aJoinPixmap, tr("JOIN_TLT"), 
                        QKeySequence(Qt::ControlModifier|Qt::Key_Plus ) );
  connect( anAct, SIGNAL(triggered()), this, SLOT(onJoin()) );
  aTB->addAction(anAct);

  anAct = createAction( BRING_TOGETHER_ID, tr("BRING_TOGETHER"), aBringTogetherPixmap, tr("BRING_TOGETHER_TLT"),
                        QKeySequence(Qt::ControlModifier|Qt::Key_Equal ) );
  connect( anAct, SIGNAL(triggered()), this, SLOT(onBringTogether()) );
  aTB->addAction(anAct);

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

  QSplitter* aSplitter = new QSplitter(aSectionGroup);
  aSplitter->setOrientation(Qt::Vertical);

  aSplitter->addWidget(mySectionView);
  aSplitter->addWidget( myLocalPointView );

  aSectLayout->addWidget(aSplitter);
  aSectionGroup->setLayout(aSectLayout);
  QVBoxLayout* aLay = new QVBoxLayout();
  aLay->setMargin( 0 );
  aLay->setSpacing( 5 );
//    aLay->addLayout(aNameLayout);
  aLay->addWidget(aSectionGroup);
  setLayout(aLay);

  updateActionsStates();
  updateUndoRedo();
}

/**
 * Set an OCC viewer
 */
void CurveCreator_Widget::setOCCViewer( OCCViewer_Viewer* theViewer )
{
  if ( myOCCViewer == theViewer )
    return;

  if ( myOCCViewer ) {
    OCCViewer_ViewManager* aViewManager = dynamic_cast<OCCViewer_ViewManager*>
                                                    ( myOCCViewer->getViewManager() );
    disconnect( aViewManager, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMousePress( SUIT_ViewWindow*, QMouseEvent* ) ) );
    disconnect( aViewManager, SIGNAL( mouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ) );
    disconnect( aViewManager, SIGNAL( mouseMove( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMouseMove( SUIT_ViewWindow*, QMouseEvent* ) ) );
    disconnect( aViewManager, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onLastViewClosed( SUIT_ViewManager* ) ) );
    // restore normal mode in the viewer
    SetViewer2DMode(false);
    // all local contexts should be closed if the viewer is not more used
    setLocalPointContext( false, true );
  }

  myOCCViewer = theViewer;
  if ( myOCCViewer ) {
    OCCViewer_ViewManager* aViewManager = dynamic_cast<OCCViewer_ViewManager*>
                                                    ( myOCCViewer->getViewManager() );
    connect( aViewManager, SIGNAL( mousePress( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMousePress( SUIT_ViewWindow*, QMouseEvent* ) ) );
    connect( aViewManager, SIGNAL( mouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMouseRelease( SUIT_ViewWindow*, QMouseEvent* ) ) );
    connect( aViewManager, SIGNAL( mouseMove( SUIT_ViewWindow*, QMouseEvent* ) ),
           this, SLOT( onMouseMove( SUIT_ViewWindow*, QMouseEvent* ) ) );
    connect( aViewManager, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onLastViewClosed( SUIT_ViewManager* ) ) );
    SetViewer2DMode(true);
  }
}

/**
 * Returns current OCC viewer
 */
OCCViewer_Viewer* CurveCreator_Widget::getOCCViewer()
{
  return myOCCViewer;
}

/**
 * Returns OCC viewer context
 */
Handle(AIS_InteractiveContext) CurveCreator_Widget::getAISContext()
{
  Handle(AIS_InteractiveContext) aContext;
  OCCViewer_Viewer* aViewer = getOCCViewer();
  if ( aViewer )
    aContext = aViewer->getAISContext();

  return aContext;
}

/**
 * Returns OCC viewer view port
 */
OCCViewer_ViewPort3d* CurveCreator_Widget::getViewPort()
{
  OCCViewer_ViewPort3d* aViewPort = 0;
  OCCViewer_Viewer* aViewer = getOCCViewer();
  if ( aViewer )
    aViewPort = ((OCCViewer_ViewWindow*)aViewer->getViewManager()->getActiveView())->getViewPort();
    
  return aViewPort;
}

/**
 * Set interaction style in the OCC viewer
 * \param theStyle a new style
 * \return the previous style
 */
int CurveCreator_Widget::changeInteractionStyle( int theStyle )
{
  OCCViewer_Viewer* aViewer = getOCCViewer();
  if ( !aViewer )
    return -1;

  int aPrevStyle = aViewer->interactionStyle();
  aViewer->setInteractionStyle( theStyle );

  return aPrevStyle;
}

//=======================================================================
// function: reset
// purpose: reset the widget viewer, close local context, clear selection
//=======================================================================
void CurveCreator_Widget::reset()
{
  stopActionMode();
}

void CurveCreator_Widget::setCurve( CurveCreator_ICurve* theCurve )
{
  myCurve = theCurve;
  mySectionView->setCurve( myCurve );
  myLocalPointView->setCurve( myCurve );
  updateActionsStates();
  updateUndoRedo();
}

void CurveCreator_Widget::onSelectionChanged()
{
  updateActionsStates();
  updateUndoRedo();
  emit selectionChanged();
}

void CurveCreator_Widget::updateActionsStates()
{
  QList<ActionId> anEnabledAct;
  if( myCurve ){
    anEnabledAct << NEW_SECTION_ID << MODIFICATION_MODE_ID;
    if ( removeEnabled() )
      anEnabledAct << REMOVE_ID;
    QList<int> aSelSections = mySectionView->getSelectedSections();
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
        anEnabledAct << ADDITION_MODE_ID << DETECTION_MODE_ID;
      }
      switch ( getActionMode() ) {
        case AdditionMode: {
          mySection = -1;
          myPointNum = -1;
          QList<int> aSelSection = mySectionView->getSelectedSections();
          if( aSelSection.size() > 0 ){
            mySection = aSelSection[0];
            myPointNum = myCurve->getNbPoints(mySection);
          }
        }
        break;
        case ModificationMode: {
          if ( myNewSectionEditor->isEnableClosed() )
            anEnabledAct << CLOSE_SECTIONS_ID << UNCLOSE_SECTIONS_ID;
          anEnabledAct << SET_SECTIONS_POLYLINE_ID << SET_SECTIONS_SPLINE_ID;
          int aSectCnt = myCurve->getNbSections();
          if( aSectCnt > 0 )
            anEnabledAct << CLEAR_ALL_ID;
          if( aSectCnt > 1 )
            anEnabledAct << JOIN_ALL_ID;
          if( aSelSections.size() > 1 ){
            anEnabledAct << JOIN_ID << BRING_TOGETHER_ID;
          }
        }
        break;
        case DetectionMode: {
        }
        break;
        case NoneMode:
          {
            int aSectCnt = myCurve->getNbSections();
            if( aSectCnt > 1 )
              anEnabledAct << JOIN_ALL_ID;
            if( aSelSections.size() > 1 )
              anEnabledAct << JOIN_ID;
          }
          break;
        default:
        break;
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
}

void CurveCreator_Widget::onAdditionMode(bool checked)
{
  if (!checked)
    return;

  Handle(AIS_InteractiveContext) aContext = getAISContext();
  if( !myCurve || aContext.IsNull() )
    return;

  mySection= -1;
  myPointNum = -1;
  QList<int> aSelSection = mySectionView->getSelectedSections();
  if( aSelSection.size() > 0 ){
    mySection = aSelSection[0];
  }
//  emit subOperationStarted( myNewPointEditor );
}

void CurveCreator_Widget::onModificationMode(bool checked)
{
  myLocalPointView->setVisible( checked );
}

void CurveCreator_Widget::onDetectionMode(bool checked)
{
}

void CurveCreator_Widget::onModeChanged(bool checked)
{
  ActionMode aMode = NoneMode;
  if (checked) {
    QAction* anAction = (QAction*)sender();
    switch(myActionMap.key(anAction)) {
      case ADDITION_MODE_ID:
        aMode = AdditionMode;
        if (myActionMap[MODIFICATION_MODE_ID]->isChecked())
          myActionMap[MODIFICATION_MODE_ID]->trigger();
        else if (myActionMap[DETECTION_MODE_ID]->isChecked())
          myActionMap[DETECTION_MODE_ID]->trigger();
        break;
      case MODIFICATION_MODE_ID:
        aMode = ModificationMode;
        if (myActionMap[ADDITION_MODE_ID]->isChecked())
          myActionMap[ADDITION_MODE_ID]->trigger();
        else if (myActionMap[DETECTION_MODE_ID]->isChecked())
          myActionMap[DETECTION_MODE_ID]->trigger();
        break;
      case DETECTION_MODE_ID:
        aMode = DetectionMode;
        if (myActionMap[ADDITION_MODE_ID]->isChecked())
          myActionMap[ADDITION_MODE_ID]->trigger();
        else if (myActionMap[MODIFICATION_MODE_ID]->isChecked())
          myActionMap[MODIFICATION_MODE_ID]->trigger();
        break;
    }
  }
  updateActionsStates();
  updateUndoRedo();
  setLocalPointContext( aMode == ModificationMode, true );
}

void CurveCreator_Widget::onNewSection()
{
  if( !myCurve )
    return;

  stopActionMode();
  myNewSectionEditor->clear();
  myNewSectionEditor->setEditMode(false);
  QString aSectName = QString( CurveCreator_UtilsICurve::getUniqSectionName( myCurve ).c_str() );
  myNewSectionEditor->setSectionParameters(aSectName, true, CurveCreator::Polyline );
  emit subOperationStarted( myNewSectionEditor, false );
}

void CurveCreator_Widget::onAddNewSection()
{
  if( !myCurve )
    return;
  myCurve->addSection( myNewSectionEditor->getName().toStdString(),
                       myNewSectionEditor->getSectionType(),
                       myNewSectionEditor->isClosed() );
  mySectionView->sectionAdded( -1 ); // add a new section to the end of list
  QString aNewName = QString( CurveCreator_UtilsICurve::getUniqSectionName( myCurve ).c_str() );
  myNewSectionEditor->setSectionName(aNewName);
  updateActionsStates();
  updateUndoRedo();
  onCancelSection();
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

QAction* CurveCreator_Widget::getAction( ActionId theId )
{
  if( myActionMap.contains(theId) )
    return myActionMap[theId];
  return NULL;
}

QAction* CurveCreator_Widget::getAction( ActionMode theMode )
{
  ActionId anActionId = NONE_ID;
  switch ( theMode ) {
    case AdditionMode:
      anActionId = ADDITION_MODE_ID;
      break;
    case ModificationMode:
      anActionId = MODIFICATION_MODE_ID;
      break;
    case DetectionMode:
      anActionId = DETECTION_MODE_ID;
      break;
    default:
      break;
  }
  QAction* anAction = 0;
  if ( anActionId != NONE_ID && myActionMap.contains( anActionId ) )
    anAction = myActionMap[anActionId];
  return anAction;
}

void CurveCreator_Widget::onEditSection( int theSection )
{
  if( !myCurve )
    return;
  
  stopActionMode();
  mySection = theSection;
  QString aSectName = QString::fromStdString( myCurve->getSectionName(theSection));
  bool isClosed = myCurve->isClosed(theSection);
  CurveCreator::SectionType aType = myCurve->getSectionType(theSection);
  myNewSectionEditor->setEditMode(true);
  myNewSectionEditor->setSectionParameters( aSectName, isClosed, aType );

  emit subOperationStarted( myNewSectionEditor, true );
}

void CurveCreator_Widget::onModifySection()
{
  if( !myCurve )
    return;
  QString aName = myNewSectionEditor->getName();
  bool isClosed = myNewSectionEditor->isClosed();
  CurveCreator::SectionType aSectType = myNewSectionEditor->getSectionType();
  if( myCurve->getSectionName(mySection) != aName.toStdString() )
    myCurve->setSectionName( mySection , aName.toStdString() );

  bool isGeomModified = false;

  if( myCurve->getSectionType(mySection) != aSectType ) {
    myCurve->setSectionType( mySection, aSectType );
    isGeomModified = true;
  }

  if( myCurve->isClosed(mySection) != isClosed ) {
    myCurve->setClosed( mySection, isClosed );
    isGeomModified = true;
  }
  mySectionView->sectionChanged(mySection);
  updateUndoRedo();
  onCancelSection();
}

void CurveCreator_Widget::onJoin()
{
  if( !myCurve )
    return;
  QList<int> aSections = mySectionView->getSelectedSections();
  if( aSections.size() == 0 ){
    return;
  }
  stopActionMode();

  std::list<int> aSectionsToJoin;
  for( int i = 0; i < aSections.size() ; i++ ){
    aSectionsToJoin.push_back( aSections[i] );
  }
  //int aMainSect = aSectionsToJoin.front();
  //int aMainSectSize = myCurve->getNbPoints(aMainSect);
  if ( myCurve->join( aSectionsToJoin ) )
  {
    std::list<int>::const_iterator anIt = aSectionsToJoin.begin(),
                                   aLast = aSectionsToJoin.end();
    // the first section should be skipped. It is not removed, but is modified
    anIt++;
    for ( ; anIt != aLast; anIt++ )
      mySectionView->sectionsRemoved( *anIt );
  }

  /* The update for the points of the main section
  int aNewSectSize = myCurve->getNbPoints(aMainSect);
  if( aNewSectSize != aMainSectSize )
    mySectionView->pointsAdded( aMainSect, aMainSectSize, aNewSectSize-aMainSectSize );*/
  updateUndoRedo();
}

void CurveCreator_Widget::onRemove()
{
  if( !myCurve )
    return;

  switch( getActionMode() ) {
    case NoneMode:
      removeSection();
    break;
    case ModificationMode:
      removePoint();
    break;
    default:
      break;
  }
}

void CurveCreator_Widget::onClearAll()
{
  if( !myCurve )
    return;
  stopActionMode();
  myCurve->clear();
  mySectionView->reset();
  updateActionsStates();
  updateUndoRedo();
}

void CurveCreator_Widget::onJoinAll()
{
  if( !myCurve )
    return;
  stopActionMode();

  std::list<int> aSectionsToJoin;
  for( int i = 0, aNb = myCurve->getNbSections(); i < aNb ; i++ ){
    aSectionsToJoin.push_back( i );
  }
  bool aRes = myCurve->join( aSectionsToJoin );

  mySectionView->reset();
  updateActionsStates();
  updateUndoRedo();
}

void CurveCreator_Widget::onUndoSettings()
{

}

void CurveCreator_Widget::onSetSpline()
{
  if( !myCurve )
    return;
  stopActionMode();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myCurve->setSectionType(aSelSections[i], CurveCreator::Spline );
    mySectionView->sectionChanged(aSelSections[i]);
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onSetPolyline()
{
  if( !myCurve )
    return;
  stopActionMode();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myCurve->setSectionType( aSelSections[i], CurveCreator::Polyline );
    mySectionView->sectionChanged( aSelSections[i] );
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onCloseSections()
{
  if( !myCurve )
    return;
  stopActionMode();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myCurve->setClosed(aSelSections[i], true);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onUncloseSections()
{
  if( !myCurve )
    return;
  stopActionMode();
  QList<int> aSelSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSelSections.size() ; i++ ){
    myCurve->setClosed(aSelSections[i], false);
    mySectionView->sectionChanged(aSelSections[i]);
  }
  updateUndoRedo();
}

void CurveCreator_Widget::onUndo()
{
  if( !myCurve )
    return;

  CurveCreator_ICurve::SectionToPointList aPoints;
  startCurveModification( aPoints, false );
  myCurve->undo();
  finishCurveModification();
  mySectionView->reset();
}

void CurveCreator_Widget::onRedo()
{
  if( !myCurve )
    return;
  CurveCreator_ICurve::SectionToPointList aPoints;
  startCurveModification( aPoints, false );
  myCurve->redo();
  finishCurveModification();
  mySectionView->reset();
}

void CurveCreator_Widget::updateUndoRedo()
{
  if( !myCurve )
    return;
  QAction* anAct = myActionMap[UNDO_ID];
  if( anAct != 0 ){
    if( myCurve->getNbUndo() != 0 ){
      anAct->setEnabled(true);
    }
    else{
      anAct->setDisabled(true);
    }
  }
  anAct = myActionMap[REDO_ID];
  if( anAct != 0 ){
    if( myCurve->getNbRedo() != 0 ){
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
  aContextActions << CLEAR_ALL_ID << JOIN_ID << JOIN_ALL_ID << SEPARATOR_ID <<
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

void CurveCreator_Widget::setSelectedSections( const QList<int>& theSections )
{
  mySectionView->setSelectedSections( theSections );
  updateActionsStates();
  updateUndoRedo();
}

/**
 * According to the widget state, performs the remove action
 */
void CurveCreator_Widget::removeSelected()
{
  onRemove();
}

/**
 * Checks whether there are some selection to be removed
 */
bool CurveCreator_Widget::removeEnabled()
{
  bool isEnabled = getActionMode() == ModificationMode;
  if ( !isEnabled ) {
    QList<int> aSelSections = mySectionView->getSelectedSections();
    CurveCreator_TreeView::SelectionType aSelType = mySectionView->getSelectionType();
    isEnabled = aSelType == CurveCreator_TreeView::ST_SECTIONS &&
                aSelSections.size() == 1;
  }
  return isEnabled;
}

void CurveCreator_Widget::setActionMode( const ActionMode& theMode )
{
  ActionMode aPrevMode = getActionMode();
  QAction* aPrevAction = getAction( aPrevMode );
  QAction* anAction = getAction( theMode );
  switch ( theMode ) {
    case NoneMode:
    case AdditionMode: {
      if ( aPrevAction ) {
        if ( aPrevAction->isChecked() ) {
          aPrevAction->setChecked( false );
        }
      }
      if ( aPrevMode == ModificationMode )
        onModificationMode( false );
      if ( aPrevMode == AdditionMode )
        onAdditionMode( false );

      if ( theMode == AdditionMode )
      {
        anAction->setChecked( true );
        onModeChanged( true );
      }
    }
    break;
    break;
    case ModificationMode:
    {
      //TODO
    }
    break;
    case DetectionMode:
      break;
  }
}

CurveCreator_Widget::ActionMode CurveCreator_Widget::getActionMode() const
{
  ActionMode aMode = NoneMode;

  if ( myActionMap[ADDITION_MODE_ID]->isChecked() )
    aMode = AdditionMode;
  else if ( myActionMap[MODIFICATION_MODE_ID]->isChecked() )
    aMode = ModificationMode;
  else if ( myActionMap[DETECTION_MODE_ID]->isChecked() )
    aMode = DetectionMode;

  return aMode;
}

void CurveCreator_Widget::SetViewer2DMode(const bool To2D)
{
  if (myOCCViewer) {
    if (To2D) {
      myOld2DMode = OCCViewer_Utilities::setViewer2DMode
                    (myOCCViewer, OCCViewer_ViewWindow::XYPlane);
    } else {
      OCCViewer_Utilities::setViewer2DMode(myOCCViewer, myOld2DMode);
    }
  }
}

//=================================================================================
// function : GeometryGUI::addCoordsByClick()
// purpose  : Manage mouse press events in Additon mode
//=================================================================================
void CurveCreator_Widget::addCoordsByClick( QMouseEvent* pe )
{
  if (pe->button() != Qt::LeftButton)
    return;

  if ( pe->modifiers() != Qt::ControlModifier ) {
    Handle(AIS_InteractiveContext) ic = getAISContext();
    if ( ic.IsNull() )
      return;

    gp_Pnt aPnt;    
    OCCViewer_ViewPort3d* vp = getViewPort();

    aPnt = CurveCreator_Utils::ConvertClickToPoint( pe->x(), pe->y(), vp->getView() );

    // set the coordinates into dialog
    CurveCreator::Coordinates aCoords;
    aCoords.push_back( aPnt.X() );
    aCoords.push_back( aPnt.Y() );
    if ( myCurve->getDimension() == 3 ) {
      aCoords.push_back( aPnt.Z() );
    }
    addNewPoint(aCoords);
  }
}

/**
 * Set the same coordinates on two section extremities.
 * Usage: build a closed contour with several sections.
 * Works with two points selected, at the extremities of different sections.
 * The second point gets the coordinates of the first.
 */
void CurveCreator_Widget::onBringTogether()
{
  MESSAGE("onBringTogether");
  ActionMode aMode = getActionMode();
  if ( aMode != ModificationMode )
    return;
  int nbPoints = myLocalPoints.size();
  MESSAGE("number of selected points: " << nbPoints);
  if (nbPoints != 2)
    {
      MESSAGE("works only with two points taken at the extremities of different sections, nothing done");
      return;
    }

  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = myLocalPoints.begin(),
                                                          aLast = myLocalPoints.end();
  std::vector<int> sections(nbPoints);
  std::vector<int> iPoints(nbPoints);
  std::vector<int> nbPtsSection(nbPoints);
  std::vector<double> x(nbPoints);
  std::vector<double> y(nbPoints);
  int i = 0;
  for ( ; anIt != aLast; anIt++, i++ )
    {
      CurveCreator_ICurve::SectionToPoint aSPoint = *anIt;
      sections[i] = aSPoint.first;
      iPoints[i] = aSPoint.second;
      nbPtsSection[i] = myCurve->getNbPoints(sections[i]);
      if ((iPoints[i] != 0) && (iPoints[i] != nbPtsSection[i]-1))
        {
          MESSAGE("a point is not on a section extremity, nothing done");
          return;
        }
      gp_Pnt aPoint;
      CurveCreator_UtilsICurve::getPoint( myCurve, sections[i], iPoints[i], aPoint );
      x[i] = aPoint.X();
      y[i] = aPoint.Y();
      MESSAGE("point: " << sections[i] << " " << iPoints[i] << " " << x[i] << " " << y[i]);
    }
  if (sections[1] == sections[0])
    {
      MESSAGE("the two points must be at the extremities of different sections, nothing done");
      return;
    }

  CurveCreator_ICurve::SectionToPointList aSelPoints;
  startCurveModification( aSelPoints );

  CurveCreator::Coordinates aChangedPos;
  aChangedPos.push_back( x[0] ); // apply the first point coordinates to the second point.
  aChangedPos.push_back( y[0] );
  myCurve->setPoint( sections[1], iPoints[1], aChangedPos );

  finishCurveModification( aSelPoints );
}

/**
 * Manage mouse press events
 * \param theWindow an owner of the signal
 * \param theEvent a mouse event
 */
void CurveCreator_Widget::onMousePress( SUIT_ViewWindow*, QMouseEvent* theEvent )
{
  if ( theEvent->button() != Qt::LeftButton )
    return;

  // Initialize the starting point
  myStartPoint.setX( theEvent->x() );
  myStartPoint.setY( theEvent->y() );

  switch( getActionMode() ) {
    case ModificationMode: {
      //store initial cursor position for Drag&Drop
      setDragStarted( true, theEvent->pos() );
      break;
    }
    case AdditionMode: {
      addCoordsByClick( theEvent );
      break;
    }
    default:
      break;
  }
}

/**
 * Manage mouse release events in Modification mode
 * \param theWindow an owner of the signal
 * \param theEvent a mouse event
 */
void CurveCreator_Widget::onMouseRelease( SUIT_ViewWindow* theWindow, QMouseEvent* theEvent )
{
  ActionMode aMode = getActionMode();
  if ( aMode != ModificationMode )
  {
    // Emit selectionChanged() signal
    getOCCViewer()->performSelectionChanged();

    if ( aMode == AdditionMode )
    {
      Handle(AIS_InteractiveContext) aCtx = getAISContext();
      if ( !aCtx.IsNull() )
        aCtx->ClearSelected();
    }
    return;
  } 
  if (theEvent->button() != Qt::LeftButton) return;
  if (!theWindow->inherits("OCCViewer_ViewWindow")) return;

  // Initialize the ending point
  myEndPoint.setX( theEvent->x() );
  myEndPoint.setY( theEvent->y() );

  bool aHasShift = ( theEvent->modifiers() & Qt::ShiftModifier );

  // Highlight detected objects
  Handle(AIS_InteractiveContext) aCtx = getAISContext();
  if ( !aCtx.IsNull() )
  {
    OCCViewer_ViewWindow* aView = (OCCViewer_ViewWindow*) theWindow;
    if (!aView)
      return;

    if (!aHasShift)
      aCtx->ClearCurrents( false );

    Handle(V3d_View) aView3d = aView->getViewPort()->getView();
    if ( !aView3d.IsNull() )
    {
      // Initialize the single selection if start and end points are equal,
      // otherwise a rectangular selection.
      if ( myStartPoint == myEndPoint )
      {
        aCtx->MoveTo( myEndPoint.x(), myEndPoint.y(), aView3d );
        if ( aHasShift )
          aCtx->ShiftSelect();
        else
          aCtx->Select();
      }
      else
      {
        if ( aHasShift )
          aCtx->ShiftSelect( myStartPoint.x(), myStartPoint.y(), myEndPoint.x(), myEndPoint.y(),
                             aView3d, Standard_False );
        else
          aCtx->Select( myStartPoint.x(), myStartPoint.y(), myEndPoint.x(), myEndPoint.y(),
                        aView3d, Standard_False );
      }
    }
  }

  if ( myDragStarted ) {
    bool isDragged = myDragged;
    CurveCreator_ICurve::SectionToPointList aDraggedPoints;
    QMap<CurveCreator_ICurve::SectionToPoint, CurveCreator::Coordinates > anInitialDragPointsCoords;
    if ( myDragged ) {
      aDraggedPoints = myDragPoints;
      anInitialDragPointsCoords = myInitialDragPointsCoords;
    }

    setDragStarted( false );

    if ( aDraggedPoints.size() > 0 ) {
      // Collect old coordinates of the dragged points
      CurveCreator_ICurve::SectionToPointCoordsList anOldPoints;
      foreach ( const CurveCreator_ICurve::SectionToPoint aSectionToPoint, anInitialDragPointsCoords.keys() ) {
        CurveCreator::Coordinates aCoords = anInitialDragPointsCoords.value( aSectionToPoint );
        anOldPoints.push_back( std::make_pair( aSectionToPoint, aCoords ) );
      }

      if ( myCurve->canPointsBeSorted() ) {
        // Add old coordinates of the curve points (except the dragged points) to the list
        for( int aSectionId = 0 ; aSectionId < myCurve->getNbSections() ; aSectionId++ ) {
          CurveCreator::Coordinates aCoords;
          for ( int aPointId = 0, aNb = myCurve->getNbPoints( aSectionId ); aPointId < aNb; aPointId++ ) {
            aCoords = myCurve->getPoint( aSectionId, aPointId );
            if ( aCoords.size() < 2 ) {
              continue;
            }
            
            CurveCreator_ICurve::SectionToPoint aSectionToPoint = std::make_pair( aSectionId, aPointId );

            if ( !anInitialDragPointsCoords.contains( aSectionToPoint ) ) {
              anOldPoints.push_back( std::make_pair( aSectionToPoint, aCoords ) );
            }
          }
        }
        
        // Apply points sorting
        CurveCreator_ICurve::SectionToPointList aPoints;
        startCurveModification( aPoints, false );

        myCurve->setSkipSorting( false );

        CurveCreator_ICurve::SectionToPointCoordsList aCoordList;
        CurveCreator_ICurve::SectionToPointList::const_iterator anIt = aDraggedPoints.begin(),
                                                                aLast = aDraggedPoints.end();
        for ( ; anIt != aLast; anIt++ ) {
          int aSectionId = anIt->first;
          int aPointId = anIt->second;
          CurveCreator::Coordinates aPos = myCurve->getPoint( aSectionId, aPointId );

          aCoordList.push_back(
            std::make_pair( std::make_pair( aSectionId, aPointId ), aPos ) );
        }

        myCurve->setSeveralPoints( aCoordList, false );
    
        finishCurveModification( aDraggedPoints );
      } else {
        // if the drag of some points has happened, restore the drag selection
        START_MEASURE_TIME;
        setSelectedPoints( aDraggedPoints );
        END_MEASURE_TIME( "drop" );
      }

      // Save drag difference
      myCurve->saveCoordDiff( anOldPoints );
    }
  }
  else // check whether the segment is clicked an a new point should be added to the segment
  {
    if ( myStartPoint.x() == myEndPoint.x() && myStartPoint.y() == myEndPoint.y() )
      insertPointToSelectedSegment( myEndPoint.x(), myStartPoint.y() );
  }

  // updates the input panel table to show the selected point coordinates
  updateLocalPointView();
  updateUndoRedo();
}

/**
 * Manage mouse move events in Modification mode
 * \param theWindow an owner of the signal
 * \param theEvent a mouse event
 */
void CurveCreator_Widget::onMouseMove( SUIT_ViewWindow*, QMouseEvent* theEvent )
{
  if ( getActionMode() != ModificationMode || !myDragStarted )
    return;

  QPoint aPos = theEvent->pos();
  if ( (aPos - myDragStartPosition).manhattanLength() < QApplication::startDragDistance() )
    return;

  START_MEASURE_TIME;

  moveSelectedPoints( aPos.x(), aPos.y() );
  myDragStartPosition = aPos;

  END_MEASURE_TIME( "drag" );
}

/**
 * Set zero viewer by the last view closed in
 * \param theManager a viewer manager
 */
void CurveCreator_Widget::onLastViewClosed( SUIT_ViewManager* theManager )
{
  myOCCViewer = 0;
}

void CurveCreator_Widget::onMousePress( QMouseEvent* theEvent )
{
  onMousePress( 0, theEvent );
}

void CurveCreator_Widget::onMouseRelease( QMouseEvent* theEvent )
{
  onMouseRelease( 0, theEvent );
}

void CurveCreator_Widget::onMouseMove( QMouseEvent* theEvent )
{
  onMouseMove( 0, theEvent );
}

void CurveCreator_Widget::onCellChanged( int theRow, int theColumn )
{
  int aCurrSect = myLocalPointView->getSectionId( theRow );
  int aPntIndex = myLocalPointView->getPointId( theRow );

  if ( aPntIndex < 0 )
    return;

  CurveCreator_ICurve::SectionToPointList aSelPoints;
  startCurveModification( aSelPoints );

  double aX  = myLocalPointView->item( theRow, 2 )->data( Qt::UserRole ).toDouble();
  double anY = myLocalPointView->item( theRow, 3 )->data( Qt::UserRole ).toDouble();
  CurveCreator::Coordinates aChangedPos;
  aChangedPos.push_back( aX );
  aChangedPos.push_back( anY );
  myCurve->setPoint( aCurrSect, aPntIndex, aChangedPos );

  finishCurveModification( aSelPoints );
}

/**
 * Removes a selected section from the curve. Updates undo/redo status
 */
void CurveCreator_Widget::removeSection()
{
  stopActionMode();

  QList<int> aSections = mySectionView->getSelectedSections();
  for( int i = 0 ; i < aSections.size() ; i++ ){
    int aSectNum = aSections[i] - (i);
    myCurve->removeSection( aSectNum );
    mySectionView->sectionsRemoved( aSectNum );
  }
  mySectionView->clearSelection();
  updateUndoRedo();
}

/**
 * Removes a selected points from the curve. Updates undo/redo status
 */
void CurveCreator_Widget::removePoint()
{
  CurveCreator_ICurve::SectionToPointList aPoints;
  getSelectedPoints( aPoints );
  if ( aPoints.size() == 0 )
    return;

  CurveCreator_ICurve::SectionToPointList aSelPoints;
  startCurveModification( aSelPoints, false );

  myCurve->removeSeveralPoints( aPoints );
  finishCurveModification( CurveCreator_ICurve::SectionToPointList() );
  mySectionView->reset();
}

void CurveCreator_Widget::addNewPoint(const CurveCreator::Coordinates& theCoords)
{
  if( !myCurve )
    return;
  QList<int> aSections = mySectionView->getSelectedSections();
  if( aSections.size() == 0 ){
    return;
  }
  int aSection = aSections[0];
  myCurve->addPoints(theCoords, aSection); // add to the end of section
  mySectionView->pointsAdded( aSection, myCurve->getNbPoints( aSection ) );
  updateActionsStates();
  updateUndoRedo();
}

void CurveCreator_Widget::insertPointToSelectedSegment( const int theX,
                                                        const int theY )
{
  Handle(AIS_InteractiveContext) aContext = getAISContext();

  OCCViewer_ViewPort3d* aViewPort = getViewPort();
  Handle(V3d_View) aView;
  if ( aViewPort )
    aView = aViewPort->getView();

  if ( aContext.IsNull() || aView.IsNull() )
    return;
  gp_Pnt aPoint;
  gp_Pnt aPoint1, aPoint2;
  Handle(AIS_InteractiveObject) anAISObject = myCurve->getAISObject();
  bool isFoundPoint = CurveCreator_Utils::pointOnObject( aView, anAISObject, theX, theY,
                                                         aPoint, aPoint1, aPoint2 );
  if ( !isFoundPoint )
    return;

  // insert the point to the model curve
  CurveCreator_ICurve::SectionToPointList aSelPoints;
  startCurveModification( aSelPoints );

  CurveCreator::Coordinates aCoords;
  aCoords.push_back( aPoint.X() );
  aCoords.push_back( aPoint.Y() );

  CurveCreator_ICurve::SectionToPointList aPoints1, aPoints2;
  findSectionsToPoints( aPoint1.X(), aPoint1.Y(), aPoints1 );
  findSectionsToPoints( aPoint2.X(), aPoint2.Y(), aPoints2 );
  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = aPoints1.begin(),
                                                          aLast = aPoints1.end();
  int aSectionId = -1;
  // find the indices of the neighbour point
  // there can be a case when a new point is added into two sections
  int aPoint1Id = -1, aPoint2Id = -1;
  for ( ; anIt != aLast && aSectionId < 0; anIt++ ) {
    int aSectionCur = anIt->first;
    CurveCreator_ICurve::SectionToPointList::const_iterator anIt2 = aPoints2.begin(),
                                                            aLast2 = aPoints2.end();
    for ( ; anIt2 != aLast2 && aSectionId < 0; anIt2++ ) {
      if ( anIt2->first == aSectionCur ) {
        aSectionId = aSectionCur;
        aPoint1Id = anIt->second;
        aPoint2Id = anIt2->second;
      }
    }
  }

  int anInsertPos = -1;
  int aLastPoint = myCurve->getNbPoints( aSectionId )-1; 
  if ( ( aPoint1Id == aLastPoint && aPoint2Id == 0 ) ||
       ( aPoint2Id == aLastPoint && aPoint1Id == 0 ) )
    anInsertPos = -1; // if the section happens between first and last points
  else
    anInsertPos = aPoint1Id < aPoint2Id ? aPoint1Id + 1 : aPoint2Id + 1;

  myCurve->addPoints( aCoords, aSectionId, anInsertPos );
  mySectionView->pointsAdded( aSectionId, myCurve->getNbPoints( aSectionId ) );

  finishCurveModification( aSelPoints );

  setSelectedPoints();
}

void CurveCreator_Widget::moveSelectedPoints( const int theXPosition,
                                              const int theYPosition )
{
  OCCViewer_ViewPort3d* aViewPort = getViewPort();
  if ( !aViewPort )
    return;

  CurveCreator_ICurve::SectionToPointList aPoints;
  startCurveModification( aPoints, false );

  gp_Pnt aStartPnt = CurveCreator_Utils::ConvertClickToPoint( myDragStartPosition.x(),
                                                              myDragStartPosition.y(),
                                                              aViewPort->getView() );
  gp_Pnt anEndPnt = CurveCreator_Utils::ConvertClickToPoint( theXPosition, theYPosition,
                                                             aViewPort->getView() );
  double aXDelta = aStartPnt.X() - anEndPnt.X();
  double anYDelta = aStartPnt.Y() - anEndPnt.Y();

  CurveCreator_ICurve::SectionToPointCoordsList aCoordList;
  CurveCreator::Coordinates aChangedPos;
  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = myDragPoints.begin(),
                                                          aLast = myDragPoints.end();
  for ( ; anIt != aLast; anIt++ ) {
    int aSectionId = anIt->first;
    int aPointId = anIt->second;
    aChangedPos = myCurve->getPoint( aSectionId, aPointId );
    if ( aChangedPos.size() < 2 )
      continue;

    // Remember drag points coordinates
    if ( !myDragged ) {
      myInitialDragPointsCoords.insert( std::make_pair( aSectionId, aPointId ), aChangedPos );
    }

    aChangedPos[0] = aChangedPos[0] - aXDelta;
    aChangedPos[1] = aChangedPos[1] - anYDelta;
    
    aCoordList.push_back(
      std::make_pair(std::make_pair( aSectionId, aPointId ), 
                     aChangedPos ));
  }
  myCurve->setSeveralPoints( aCoordList, false );

  myDragged = true;
  finishCurveModification( myDragPoints );
}

void CurveCreator_Widget::updateLocalPointView()
{
  if ( myDragStarted )
    return;
  Handle(AIS_InteractiveContext) aContext = getAISContext();
  if ( aContext.IsNull() )
    return;

  CurveCreator_Utils::getSelectedPoints( aContext, myCurve, myLocalPoints );
  int aNbPoints = myLocalPoints.size();

  bool isRowLimit = aNbPoints > myLocalPointRowLimit;
  myLocalPointView->setVisible( getActionMode() == ModificationMode && !isRowLimit );

  if ( !isRowLimit ) {
    bool isBlocked = myLocalPointView->blockSignals(true);

    myLocalPointView->setLocalPointsToTable( myLocalPoints );

    myLocalPointView->blockSignals( isBlocked );
  }
}

/**
 * 
 */
void CurveCreator_Widget::setLocalPointContext( const bool theOpen, const bool isUpdateTable )
{
  CurveCreator_Utils::setLocalPointContext( myCurve, getAISContext(), theOpen );
  if ( !theOpen && isUpdateTable )
    updateLocalPointView();
}

/**
 * Set drag operation started. Save the position and a list of dragged points
 * \param theState the drag operation state: started/finished
 * \param thePoint the start drag position
 */
void CurveCreator_Widget::setDragStarted( const bool theState, const QPoint& thePoint )
{
  if ( theState ) {
    getSelectedPoints( myDragPoints );

    myDragStarted = myDragPoints.size();
    myDragStartPosition = thePoint;
    if ( myDragStarted ) {
      // change a viewer interaction style in order to avoid a select rectangle build
      myDragInteractionStyle = changeInteractionStyle( SUIT_ViewModel::KEY_FREE );
      myCurve->setSkipSorting( true );
    }
  }
  else {
    if ( myDragStarted )
      changeInteractionStyle( myDragInteractionStyle );
    myDragStarted = false;
    myDragPoints.clear();
    myInitialDragPointsCoords.clear();
  }
  myDragged = false;
}

void CurveCreator_Widget::getSelectedPoints( CurveCreator_ICurve::SectionToPointList& thePoints )
{
  thePoints.clear();
  thePoints = myLocalPoints;
}

void CurveCreator_Widget::setSelectedPoints( const CurveCreator_ICurve::SectionToPointList& thePoints )
{
  if ( myDragStarted )
    return;
  Handle(AIS_InteractiveContext) aContext = getAISContext();
  if ( aContext.IsNull() || !aContext->HasOpenedContext() )
    return;

  CurveCreator_Utils::setSelectedPoints( aContext, myCurve, thePoints );

  updateLocalPointView();
}

void CurveCreator_Widget::stopActionMode()
{
  setActionMode( NoneMode );
}

/**
 * Get viewer information before perform the curve modification.
 * Take a list of selected cuve points an close local context.
 * The context should be closed because the curve presentation is
 * redisplayed and if it is not closed, when we close the local context
 * later, the presentation shown in the local context is disappeared.
 * \param thePoints an output list of curve selected points
 * \param theFillPoints a flag whether the selection list should be filled
 */
void CurveCreator_Widget::startCurveModification(
                           CurveCreator_ICurve::SectionToPointList& thePoints,
                           const bool theFillPoints )
{
  if ( theFillPoints ) {
    thePoints.clear();
    getSelectedPoints( thePoints );
  }
  setLocalPointContext( false );
}

/**
 * Restore the viewer state after the curve modification is done.
 * Open local context and select given points inside it.
 * \param thePoints a list of curve selected points
 */
void CurveCreator_Widget::finishCurveModification(
                           const CurveCreator_ICurve::SectionToPointList& thePoints )
{
  if ( getActionMode() == ModificationMode )
    setLocalPointContext( true );
  setSelectedPoints( thePoints );
  updateUndoRedo();
}

/**
 * Returns a point index in the model curve by the point coordinates in the viewer
 * \param theX the X coordinate of the point
 * \param theY the Y coordinate of the point
 */
int CurveCreator_Widget::findLocalPointIndex( int theSectionId, double theX, double theY )
{
  return CurveCreator_UtilsICurve::findLocalPointIndex( myCurve, theSectionId, theX, theY );
}

void CurveCreator_Widget::findSectionsToPoints( const double theX, const double theY,
                                 CurveCreator_ICurve::SectionToPointList& thePoints )
{
  return CurveCreator_UtilsICurve::findSectionsToPoints( myCurve, theX, theY, thePoints );
}

void CurveCreator_Widget::convert( const CurveCreator_ICurve::SectionToPointList& thePoints,
                                   QMap<int, QList<int> >& theConvPoints )
{
  return CurveCreator_UtilsICurve::convert( thePoints, theConvPoints );
}

/**
 * Returns whethe the container has the value
 * \param theList a container of values
 * \param theValue a value
 */
bool CurveCreator_Widget::contains( const CurveCreator_ICurve::SectionToPointList& theList,
                                    const CurveCreator_ICurve::SectionToPoint& theValue ) const
{
  return CurveCreator_UtilsICurve::contains( theList, theValue );
}
