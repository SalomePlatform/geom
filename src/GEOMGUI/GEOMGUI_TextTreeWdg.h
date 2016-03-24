// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef GEOMGUI_TEXTTREEWDG_H
#define GEOMGUI_TEXTTREEWDG_H

#include "GEOM_GEOMGUI.hxx"
#include "GEOM_Displayer.h"

#include <QTreeWidget>
#include <QHash>

#include <SALOMEDSClient.hxx>

class QLabel;
class QLineEdit;
class QString;
class QTreeWidgetItem;
class SalomeApp_Application;
class SalomeApp_Study;

/*!
 * \brief Tree view contains Dimension and Annotation text items:
 *  - text visibility in OCC viewer
 *  - text object name
 */
class GEOMGUI_EXPORT GEOMGUI_TextTreeWdg : public QTreeWidget
{
  Q_OBJECT

 public:
  GEOMGUI_TextTreeWdg( SalomeApp_Application* app );
  ~GEOMGUI_TextTreeWdg();

  int getWinID() { return myWindowID; }

  void                          removeBranch( const QString& theEntry, 
					      bool force = true );
  int                           idFromItem( QTreeWidgetItem* theItem );
  QString                       entryFromItem( QTreeWidgetItem* theShapeItem );
  QTreeWidgetItem*              itemFromEntry( QString theEntry );
  void                          setShapeDimensionsVisibility( QString theEntry, bool theVisibility );
  void                          setDimensionVisibility( QString theEntry, QTreeWidgetItem* theDimItem, bool theVisibility );

protected:
  void                          createActions();
  void                          redisplay( QString theEntry );

 public slots:
  void                          updateTree();
  void                          updateBranch( const QString& theEntry );

private slots:
  void                          onItemClicked(QTreeWidgetItem*, int );
  void                          updateVisibilityColumn( QString theEntry, Qtx::VisibilityState theState );
  void                          setVisibility( bool visibility );
  void                          showContextMenu( const QPoint& pos );

 private:

  int                           myWindowID;

  QIcon                         myVisibleIcon;
  QIcon                         myInvisibleIcon;
  QHash<QString, QTreeWidgetItem*> myObjects;
  SalomeApp_Study*              myStudy;
  QTreeWidgetItem*              myDimensionsItem;
  GEOM_Displayer                myDisplayer;

  QMap<int, QAction*>           myActions;   //!< menu actions list

};
#endif
