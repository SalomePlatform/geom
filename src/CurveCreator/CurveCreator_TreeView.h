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

#ifndef CURVECREATOR_TREEVIEW_H
#define CURVECREATOR_TREEVIEW_H

#include <QTreeView>
#include <QAbstractItemModel>

class CurveCreator_ICurve;

class CurveCreator_TreeViewModel : public QAbstractItemModel
{
public:
  CurveCreator_TreeViewModel( CurveCreator_ICurve* theCurve, QObject* parent );
  virtual int	columnCount(const QModelIndex & parent = QModelIndex()) const;
  virtual int	rowCount(const QModelIndex & parent = QModelIndex()) const;
  virtual QVariant	data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  virtual QModelIndex	index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
  virtual QModelIndex	parent(const QModelIndex & theIndex) const;
//  virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole);

  QModelIndex sectionIndex( int theSection ) const;
  QModelIndex nbPointsIndex( int theSection ) const;
  QModelIndex pointIndex( int theSection, int thePoint ) const;

  bool    isSection( const QModelIndex& theIndx ) const;
  int     getSection( const QModelIndex& theIndx ) const;
  int     getPoint( const QModelIndex& theIndx ) const;

  void    setCurve( CurveCreator_ICurve* theCurve );

private:
  enum IconType{ ICON_POLYLINE, ICON_SPLINE, ICON_CLOSED_SPLINE, ICON_CLOSED_POLYLINE, ICON_POINT };
private:
  CurveCreator_ICurve*          myCurve;
  QMap<IconType, QPixmap>      myCachedIcons;
};

class CurveCreator_TreeView : public QTreeView
{
  Q_OBJECT
public:
  enum SelectionType{ ST_NOSEL, ST_POINTS, ST_POINTS_ONE_SECTION, ST_SECTIONS, ST_MIXED };
public:
  explicit CurveCreator_TreeView( CurveCreator_ICurve* theCurve, QWidget *parent = 0);
  SelectionType getSelectionType() const;
  QList<int> getSelectedSections() const;

  void    pointsAdded( int theSection, int thePoint, int thePointsCnt=1 );
  void    pointDataChanged( int theSection, int thePoint );
  void    pointsRemoved(int theSection, int thePoint, int thePointsCnt=1 );

  void    sectionAdded( int theSection );
  void    sectionChanged(int theSection , int aSectCnt = 1);
  void    sectionsRemoved( int theSection, int theSectionCnt=1 );
  void    sectionsSwapped( int theSection, int theOffset );

  void    setSelectedSections( const QList<int>& theList );

  void    setCurve( CurveCreator_ICurve* theCurve );

  void    reset();

signals:
  void    selectionChanged();
  void    sectionEntered(int);

protected slots:
  void onActivated( QModelIndex theIndx );
protected:
  void setIndexState( const QModelIndex& theIndx, bool& isExpanded, bool& isSelected, bool& isCurrent );
  void swapIndexes( const QModelIndex& theFirst, const QModelIndex& theSecond );
  void getIndexInfo( const QModelIndex& theIndx, bool& isExpanded, bool& isSelected, bool& isCurrent );

};

#endif // CURVECREATOR_TREEVIEW_H
