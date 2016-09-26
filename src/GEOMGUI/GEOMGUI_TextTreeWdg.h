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

#include <QHash>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QTreeWidget>

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
  enum BranchType { DimensionShape, AnnotationShape };

public:
  GEOMGUI_TextTreeWdg( SalomeApp_Application* app );
  ~GEOMGUI_TextTreeWdg();

  int getWinID() { return myWindowID; }

  void                          removeBranch( const BranchType& theBranchType, const QString& theEntry,
                                              bool force = true );
  int                           idFromItem( QTreeWidgetItem* theItem );
  QString                       entryFromItem( QTreeWidgetItem* theShapeItem ) const;
  QTreeWidgetItem*              itemFromEntry( const BranchType& theBranchType, QString theEntry );
  void                          setAllShapeItemsVisibility( const BranchType& theBranchType,
                                                            const QString& theEntry,
                                                            const bool theVisibility );
  void                          setShapeItemVisibility( const BranchType& theBranchType,
                                                        const QString& theEntry,
                                                        QTreeWidgetItem* theWidgetItem,
                                                        const bool theVisibility );
  void                          updateVisibility();

  void                          getSelected( QMap<QString, QList<int> >& theAnnotations );
  void                          setSelected( const QMap<QString, QList<int> >& theAnnotations );

  QList<QString>                getAllEntries( const BranchType& theBranchType );

  QString                       getSingleSelectedObject();

protected:
  void                          createActions();
  void                          redisplay( QString theEntry );

public slots:
  void                          updateTree();
  void                          updateBranches( const QString& theEntry );
  void                          updateDimensionBranch( const QString& theEntry );
  void                          updateAnnotationBranch( const QString& theEntry );
  void                          updateObjectName( const QString& theEntry );

private slots:
  void                          onUpdateVisibilityColumn( QString theEntry, Qtx::VisibilityState theState );
  void                          onItemClicked(QTreeWidgetItem*, int );
  void                          setVisibility( QTreeWidgetItem* theItem, bool visibility );
  void                          showContextMenu( const QPoint& pos );

public:

  /*!
   * \brief Common interface for working with shape dimension and annotation properties
   *        of object in a unified way irrespectively of the implementation.
   */ 
  class VisualProperty
  {
  public:
    virtual int GetNumber() = 0;
    virtual QString GetName( const int theIndex ) = 0;
    virtual bool GetIsVisible( const int theIndex ) = 0;
    virtual void SetIsVisible( const int theIndex, const bool theIsVisible ) = 0;
    virtual void Save() = 0;
  };

private:
  bool                              setShapeItemVisibility( QSharedPointer<VisualProperty>& theProps,
                                                            QTreeWidgetItem* theWidgetItem,
                                                            const bool theVisibility );
  QSharedPointer<VisualProperty>    getVisualProperty( const BranchType& theBranchType,
                                                       SalomeApp_Study* theStudy,
                                                       const std::string& theEntry );
  void                              fillBranch( const BranchType& theBranchType,
                                                const QString& theEntry );
  void                              updateVisibilityColumn( const BranchType& theBranchType,
                                                            QString theEntry,
                                                            Qtx::VisibilityState theState );
  QTreeWidgetItem*                  getPropertyRootItem( const BranchType& theBranchType );
  QHash<QString, QTreeWidgetItem*>& getObjects( const BranchType& theBranchType );
  BranchType                        branchTypeFromItem( QTreeWidgetItem* theItem );

private:

  int                              myWindowID;
  QIcon                            myVisibleIcon;
  QIcon                            myInvisibleIcon;
  QHash<QString, QTreeWidgetItem*> myDimensionObjects;
  QHash<QString, QTreeWidgetItem*> myAnnotationObjects;
  SalomeApp_Study*                 myStudy;
  QTreeWidgetItem*                 myDimensionsItem;
  QTreeWidgetItem*                 myAnnotationsItem;
  GEOM_Displayer                   myDisplayer;
  QMap<int, QAction*>              myActions; //!< menu actions list
};

#endif
