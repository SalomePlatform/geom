// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// GEOM includes
#include <GEOMUtils.hxx>
#include <GeometryGUI.h>

// GUI includes
#include <GraphicsView_ViewPort.h>
#include <GraphicsView_ViewFrame.h>

#include <SalomeApp_Application.h>

// QT includes
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>

#include "DependencyTree.h"

class DependencyTree_Object;
class DependencyTree_Arrow;
class DependencyTree_View;

typedef std::map<std::string,DependencyTree_Object*> EntryObjectMap;
typedef std::map<std::pair<DependencyTree_Object*,DependencyTree_Object*>,DependencyTree_Arrow*> ArrowsInfo;

class DEPENDENCYTREE_EXPORT DependencyTree_View: public GraphicsView_ViewPort
{
  Q_OBJECT

public:

  DependencyTree_View( QWidget* = 0 );
  ~DependencyTree_View();

  void                    init( GraphicsView_ViewFrame* );
  void                    updateModel( bool = true, bool = true );

  void                    mouseMoveEvent(QMouseEvent* event);
  void                    wheelEvent( QWheelEvent* event );

  QString                 getViewName() const;
  int                     getStudyId() const;

  DependencyTree_Object*  getObjectByEntry( const std::string& );

public slots:

  void                    onRebuildModel();

protected:

  void                    resizeEvent( QResizeEvent *event );

private slots:

  void                    onUpdateModel();
  void                    onMoveNodes( bool );
  void                    onHierarchyType();
  void                    onPreferenceChanged( const QString&, const QString& );
  void                    onRenameObject( const QString& theEntry );

private:

  void                    parseTree();
  void                    parseTreeWard( const GEOMUtils::LevelsList& );
  void                    parseTreeWardArrow( const GEOMUtils::LevelsList& );

  void                    addNode( const std::string& );
  void                    addArrow( DependencyTree_Object*, DependencyTree_Object* );

  void                    drawTree();
  void                    drawWard( const GEOMUtils::LevelsList&, std::map< std::string, int >&,
                                    std::map< int, std::vector< std::string > >&, int, const int );
  void                    drawWardArrows( const GEOMUtils::LevelsList& );

  void                    updateView();
  void                    clearView( bool );

  void                    fitWindow();

  int                     checkMaxLevelsNumber();

  void                    getNewTreeModel( bool = true, bool = true );

  void                    setHierarchyType( const int );
  void                    setNodesMovable( const bool );
  void                    setPrefBackgroundColor( const QColor& );
  void                    setNodeColor( const QColor& );
  void                    setMainNodeColor( const QColor& );
  void                    setUnpublishNodeColor( const QColor& );
  void                    setSelectNodeColor( const QColor& );
  void                    setArrowColor( const QColor& );
  void                    setHighlightArrowColor( const QColor& );
  void                    setSelectArrowColor( const QColor& );

  GEOMUtils::TreeModel    myTreeModel;

  EntryObjectMap          myTreeMap;
  ArrowsInfo              myArrows;

  int                     myLevelsNumber;
  int                     myMaxDownwardLevelsNumber;
  int                     myMaxUpwardLevelsNumber;

  QCheckBox*              myNodesMovable;
  QSpinBox*               myHierarchyDepth;
  QCheckBox*              myDisplayAscendants;
  QCheckBox*              myDisplayDescendants;
  QPushButton*            updateButton;

  SALOMEDS::Study_var     myStudy;
  LightApp_SelectionMgr*  mySelectionMgr;
  GEOM::string_array_var  myMainEntries;

  bool                    myIsUpdate;

};

#endif
