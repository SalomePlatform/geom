// Copyright (C) 2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef DEPENDENCYTREE_VIEW_H
#define DEPENDENCYTREE_VIEW_H

#include <GraphicsView_ViewPort.h>
#include <GraphicsView_ViewFrame.h>
#include <GraphicsView_Scene.h>

#include <SalomeApp_Application.h>

#include <SALOME_ListIO.hxx>

#include <GEOMUtils.hxx>
#include <GEOM_InteractiveObject.hxx>
#include <GeometryGUI.h>


#include <QWidgetAction>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QThread>
#include <QMutex>

class DependencyTree_Object;
class DependencyTree_Arrow;
class DependencyTree_View;

class DependencyTree_ComputeDlg_QThread : public QThread
{
  Q_OBJECT

public:

  DependencyTree_ComputeDlg_QThread( DependencyTree_View* );
  void sleepDraw();
  void cancel();

  DependencyTree_View*   getView() { return myView; };

protected:
  void run();

private:
  DependencyTree_View* myView;
};

typedef std::map<std::string,DependencyTree_Object*> EntryObjectMap;
typedef std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>,DependencyTree_Arrow*> ArrowsInfo;

class DependencyTree_View: public GraphicsView_ViewPort
{
  Q_OBJECT

public:

  DependencyTree_View( QWidget* = 0 );
  ~DependencyTree_View();

  void init( GraphicsView_ViewFrame* );
  void updateModel( bool = true, bool = true );
  void drawTree();

  virtual void customEvent ( QEvent* );
  void mouseMoveEvent(QMouseEvent *event);

  DependencyTree_Object* getObjectByEntry( QString );

  void setHierarchyType( const int );
  void setNodesMovable( const bool );
  void setPrefBackgroundColor( const QColor& );
  void setNodeColor( const QColor& );
  void setMainNodeColor( const QColor& );
  void setSelectNodeColor( const QColor& );
  void setArrowColor( const QColor& );
  void setHighlightArrowColor( const QColor& );
  void setSelectArrowColor( const QColor& );

  void setIsCompute( bool );
  bool getIsCompute();

  bool updateObjectName( const std::string &theEntry );

  QMutex myMutex;

public slots:
  void onUpdateModel();
  void onRebuildModel();

protected:
  void closeEvent( QCloseEvent* );

private slots:
  void updateView();
  void onMoveNodes( bool );
  void onHierarchyType();
  void onCancel();

signals:

private:

  void addNode( const std::string& );
  void addArrow( DependencyTree_Object*, DependencyTree_Object* );
  void addNewItem( QGraphicsItem* );

  void parseTree();
  void parseTreeWard(const GEOMUtils::LevelsList);
  void parseTreeWardArrow(const GEOMUtils::LevelsList);

  void drawWard( const GEOMUtils::LevelsList&, std::map< std::string, int >&,
                 std::map< int, std::vector< std::string > >&, int, const int );
  void drawWardArrows( GEOMUtils::LevelsList );

  void getNewTreeModel( bool = true, bool = true );
  void clearView( bool );

  int checkMaxLevelsNumber();
  void calcTotalCost();
  double getComputeProgress();

  void changeWidgetState( bool );

  GEOMUtils::TreeModel myTreeModel;

  EntryObjectMap myTreeMap;
  ArrowsInfo myArrows;

  int myLevelsNumber;
  int myMaxDownwardLevelsNumber;
  int myMaxUpwardLevelsNumber;

  QCheckBox* myNodesMovable;
  QSpinBox* myHierarchyDepth;
  QCheckBox* myDisplayAscendants;
  QCheckBox* myDisplayDescendants;
  QWidgetAction*  cancelAction;
  QWidgetAction*  progressAction;
  QPushButton* updateButton;

  int myTimer;

  bool myIsUpdate;

  bool myIsCompute;

  int myTotalCost;
  int myComputedCost;

  DependencyTree_ComputeDlg_QThread* qthread;

  GEOM::string_array_var myMainEntries;

  SALOMEDS::Study_var myStudy;
  LightApp_SelectionMgr* mySelectionMgr;


};

#endif
