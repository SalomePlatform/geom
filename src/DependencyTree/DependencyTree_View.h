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

#include <QGraphicsView>
#include <QList>
#include <QMap>
#include <QString>
#include <QPair>
#include <QWidgetAction>
#include <QSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QThread>
#include <QDialog>

#include "DependencyTree_Arrow.h"

class DependencyTree_Object;
class DependencyTree_View;


class DependencyTree_ComputeDlg_QThread : public QThread
{
  Q_OBJECT

public:
  DependencyTree_ComputeDlg_QThread(DependencyTree_View*);
  bool                   result();
  void                   cancel();

  DependencyTree_View*   getView() { return myView; };

protected:
  void run();

private:
  DependencyTree_View* myView;
};

class DependencyTree_View: public GraphicsView_ViewPort
{
	Q_OBJECT
public:


  DependencyTree_View( QWidget* theParent=0 );
  ~DependencyTree_View();

  void setHierarchyType( const int );
  void setNodesMovable( const bool );
  void setPrefBackgroundColor( const QColor& );
  void setNodeColor( const QColor& );
  void setMainNodeColor( const QColor& );
  void setSelectNodeColor( const QColor& );
  void setArrowColor( const QColor& );
  void setHighlightArrowColor( const QColor& );
  void setSelectArrowColor( const QColor& );

  typedef QList<QString> NodeLinks;
  typedef QMap<QString, NodeLinks> LevelInfo;
  typedef QList<LevelInfo> LevelsList;
  typedef QMap<QString,QPair<LevelsList,LevelsList> > TreeModel;

  TreeModel myTreeModel;
  QMap<QString,DependencyTree_Object*> myTreeMap;
  QList<DependencyTree_Arrow*> Arrows;

  QMap<QString,int> myLevelMap;

  QMap< int, QList<QString> > myLevelsObject;
  int myCurrentLevel;

  void init( GraphicsView_ViewFrame* );

  void onRedrawTree();

  void setIsCompute( bool theIsCompute );
  bool getIsCompute() { return myIsCompute; };
private slots:
  void onUpdateTree();
  void updateView();
  void onMoveNodes( bool );
  void onHierarchyType();

protected:
  void timerEvent(QTimerEvent *timer);
  void closeEvent(QCloseEvent *event);

private slots:
  void onCancel();

private:
  void parseData( QString& data );
  void addNode( const QString& entry );
  void addArrow( DependencyTree_Object *startItem, DependencyTree_Object *endItem );
  void findArrow( DependencyTree_Object *startItem, DependencyTree_Object *endItem );
  DependencyTree_View::LevelsList parseWard( const QString& data, int& cursor );
  void drawTree();
  void drawWard( DependencyTree_View::LevelsList ward, const int levelStep );
  void drawArrows();
  void drawWardArrows( LevelsList );

  int checkMaxLevelsNumber();
  int myLevelsNumber;
  int myMaxDownwardLevelsNumber;
  int myMaxUpwardLevelsNumber;

  QCheckBox* myNodesMovable;
  QSpinBox* myHierarchyDepth;
  QCheckBox* myDisplayAscendants;
  QCheckBox* myDisplayDescendants;

  QString myData;

  bool myIsUpdate;

  GraphicsView_ViewFrame* myViewFrame;

  bool myIsCompute;
  DependencyTree_ComputeDlg_QThread* qthread;
  QPushButton *               cancelButton;
  QProgressBar*               progressBar;
  QWidgetAction*  cancelAction;
  QWidgetAction*  progressAction;

};



///*!
// * \brief Dialog to display Cancel button
// */
//
//class DependencyTree_ComputeDlg_QThreadQDialog : public QDialog
//{
//  Q_OBJECT
//
//public:
//  DependencyTree_ComputeDlg_QThreadQDialog(QWidget* parent, DependencyTree_View*);
//  bool result();
//
//protected:
//  void timerEvent(QTimerEvent *timer);
//  void closeEvent(QCloseEvent *event);
//
//private slots:
//  void onCancel();
//
//private:
//
//  DependencyTree_ComputeDlg_QThread qthread;
//  QPushButton *               cancelButton;
//  QProgressBar*               progressBar;
//};

#endif
