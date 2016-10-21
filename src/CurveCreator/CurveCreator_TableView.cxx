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

#include "CurveCreator_TableView.h"
#include "CurveCreator_UtilsICurve.hxx"

#include <gp_Pnt.hxx>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include <QtxDoubleSpinBox.h>

const double DBL_MINIMUM = -10000000.;
const double DBL_MAXIMUM = 10000000.;

const int SECTION_NAME_COLUMN_WIDTH = 75;
const int POINT_INDEX_COLUMN_WIDTH = 40;

const double LOCAL_SELECTION_TOLERANCE = 0.0001;

CurveCreator_TableItemDelegate::CurveCreator_TableItemDelegate( QObject* theParent )
: QItemDelegate( theParent )
{
}

/**
 * Creates an editor for the cell
 */
QWidget* CurveCreator_TableItemDelegate::createEditor( QWidget* theParent,
                                                       const QStyleOptionViewItem& theOption,
                                                       const QModelIndex& theIndex ) const
{
  QWidget* anEditor = 0;

  int aColumnId = theIndex.column();
  if ( aColumnId == 2 || aColumnId == 3 ) {
    QDoubleSpinBox* aSpin = new QtxDoubleSpinBox( theParent );
    aSpin->setDecimals( 2 );
    aSpin->setRange( DBL_MINIMUM, DBL_MAXIMUM );
    anEditor = aSpin;
  }
  else
    anEditor = QItemDelegate::createEditor( theParent, theOption, theIndex );

  return anEditor;
}

void CurveCreator_TableItemDelegate::setEditorData( QWidget* theEditor,
                                                    const QModelIndex& theIndex ) const
{
  int aColumnId = theIndex.column();
  if ( aColumnId == 2 || aColumnId == 3 ) {
    QDoubleSpinBox* aDblSpin = dynamic_cast<QDoubleSpinBox*>( theEditor );
    if ( aDblSpin ) {
      double aValue = theIndex.model()->data( theIndex, Qt::EditRole ).toDouble();
      aDblSpin->setValue( aValue );
    }
  }
  else
   QItemDelegate::setEditorData( theEditor, theIndex );
}

void CurveCreator_TableItemDelegate::setModelData( QWidget* theEditor,
                                                   QAbstractItemModel* theModel,
                                                   const QModelIndex& theIndex ) const
{
  int aColumnId = theIndex.column();
  if ( aColumnId == 2 || aColumnId == 3 ) {
    QDoubleSpinBox* aDblSpin = dynamic_cast<QDoubleSpinBox*>( theEditor );
    if ( aDblSpin ) {
      double aValue = aDblSpin->value();
      theModel->setData( theIndex, aValue, Qt::UserRole);
    }
  }
  else
    QItemDelegate::setModelData( theEditor, theModel, theIndex );
}

CurveCreator_TableView::CurveCreator_TableView( CurveCreator_ICurve* theCurve,
                                                QWidget* theParent,
                                                const QStringList& theCoordTitles )
: QTableWidget( theParent ), myCurve( theCurve )
{
  setItemDelegate( new CurveCreator_TableItemDelegate( this ) );
  setVisible( false );
  setColumnCount( 5 );
  setColumnWidth( 0, SECTION_NAME_COLUMN_WIDTH );
  setColumnWidth( 1, POINT_INDEX_COLUMN_WIDTH );
  QStringList aLabels;
  QString aCoord1 = theCoordTitles.size() > 0 ? theCoordTitles[0] : tr( "TABLE_X" ); // tr( "X_POSITION_LBL" )
  QString aCoord2 = theCoordTitles.size() > 1 ? theCoordTitles[1] : tr( "TABLE_Y" ); // tr( "Y_POSITION_LBL" )
  QString aDistance = theCoordTitles.size() > 2 ? theCoordTitles[2] : tr( "DISTANCE_PREV" );
  //aLabels << tr( "SECTION_LABEL" ) << tr( "IDENTIFIER_LABEL" ) << aCoord1 << aCoord2;
  aLabels << tr( "TABLE_SECTION" ) << tr("TABLE_INDEX") << aCoord1 << aCoord2 << aDistance;
  setHorizontalHeaderLabels( aLabels );

  connect( horizontalHeader(), SIGNAL( sectionClicked( int ) ), this, SLOT( OnHeaderClick( int ) ) );
}

void CurveCreator_TableView::setCurve( CurveCreator_ICurve* theCurve )
{
  myCurve = theCurve;
}

void CurveCreator_TableView::setLocalPointsToTable(
  const CurveCreator_ICurve::SectionToPointList& thePoints )
{
  setRowCount( thePoints.size() );

  int aRowId = 0;
  double prevX=0;
  double prevY=0;
  CurveCreator_ICurve::SectionToPointList::const_iterator anIt = thePoints.begin(),
                                                          aLast = thePoints.end();
  for ( ; anIt != aLast; anIt++ ) {
    CurveCreator_ICurve::SectionToPoint aSPoint = *anIt;
    int anISection = aSPoint.first;
    int anIPoint = aSPoint.second;

    QTableWidgetItem* anItem;
    anItem = new QTableWidgetItem( myCurve->getSectionName( anISection ).c_str() );
    anItem->setFlags( anItem->flags() & ~Qt::ItemIsEditable );
    anItem->setData( Qt::UserRole, anISection );
    setItem( aRowId, 0, anItem );

    anItem = new QTableWidgetItem( QString::number( anIPoint + 1 ) );
    anItem->setFlags( anItem->flags() & ~Qt::ItemIsEnabled );
    anItem->setData( Qt::UserRole, anIPoint );
    anItem->setData( Qt::DisplayRole, anIPoint );
    setItem( aRowId, 1, anItem );

    gp_Pnt aPoint;
    CurveCreator_UtilsICurve::getPoint( myCurve, anISection, anIPoint, aPoint );

    anItem = item( aRowId, 2 );
    if ( !anItem ) {
      anItem = new QTableWidgetItem();
      setItem( aRowId, 2, anItem );
    }
    anItem->setData( Qt::UserRole, aPoint.X() );
    anItem->setData( Qt::DisplayRole, QString::number( aPoint.X(), 'f', 3 ).toDouble() );

    anItem = item( aRowId, 3 );
    if ( !anItem ) {
      anItem = new QTableWidgetItem();
      setItem( aRowId, 3, anItem );
    }
    anItem->setData( Qt::UserRole, aPoint.Y() );
    anItem->setData( Qt::DisplayRole, QString::number( aPoint.Y(), 'f', 3 ).toDouble() );

    anItem = item( aRowId, 4 );
    if ( !anItem ) {
      anItem = new QTableWidgetItem();
      setItem( aRowId, 4, anItem );
    }
    double d=0;
    if (aRowId>0)
      d=sqrt((aPoint.X()-prevX)* (aPoint.X()-prevX) + (aPoint.Y()-prevY)* (aPoint.Y()-prevY));
    anItem->setData( Qt::UserRole, d );
    anItem->setData( Qt::DisplayRole, QString::number( d, 'f', 6 ).toDouble() );
    prevX = aPoint.X();
    prevY = aPoint.Y();

    aRowId++;
  }
}

int CurveCreator_TableView::getSectionId( const int theRowId ) const
{
  return item( theRowId, 0 )->data( Qt::UserRole ).toInt();
}

/**
 * Returns a point index from the table
 * \param theRowId a table row
 */
int CurveCreator_TableView::getPointId( const int theRowId ) const
{
  return item( theRowId, 1 )->data( Qt::UserRole ).toInt();
}

void CurveCreator_TableView::OnHeaderClick( int theLogicalId )
{
  sortByColumn( theLogicalId, Qt::AscendingOrder );
}
