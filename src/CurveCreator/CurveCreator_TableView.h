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

#ifndef CURVECREATOR_TABLEVIEW_H
#define CURVECREATOR_TABLEVIEW_H

#include "CurveCreator_ICurve.hxx"

#include <QItemDelegate>
#include <QTableWidget>

class CurveCreator_TableItemDelegate : public QItemDelegate
{
public:
  CurveCreator_TableItemDelegate( QObject* theParent );
  ~CurveCreator_TableItemDelegate() {}

  virtual QWidget* createEditor( QWidget* theParent,
                                 const QStyleOptionViewItem& theOption,
                                 const QModelIndex& theIndex ) const;
  virtual void setEditorData( QWidget* theEditor, const QModelIndex& theIndex ) const;
  virtual void setModelData( QWidget* theEditor, QAbstractItemModel* theModel,
                                 const QModelIndex& theIndex ) const;
};

class CurveCreator_TableView : public QTableWidget
{
  Q_OBJECT

public:
  CurveCreator_TableView( CurveCreator_ICurve* theCurve, QWidget* theParent = 0,
                          const QStringList& theCoordTitles = QStringList() );
  ~CurveCreator_TableView() {};

  void setCurve( CurveCreator_ICurve* theCurve );

  void setLocalPointsToTable( const CurveCreator_ICurve::SectionToPointList& thePoints );

  /**
   * Returns a section index from the table
   * \param theRowId a table row
   */
  int getSectionId( const int theRowId ) const;
  /**
   * Returns a point index from the table
   * \param theRowId a table row
   */
  int getPointId( const int theRowId ) const;

private slots:
  void OnHeaderClick( int );

private:
  CurveCreator_ICurve* myCurve;

};

#endif // CURVECREATOR_TABLEVIEW_H
