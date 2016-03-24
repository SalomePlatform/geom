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

// internal includes
#include "DependencyTree_Arrow.h"
#include "DependencyTree_Object.h"

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

// Qt includes
#include <QPainter>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <math.h>

const qreal arrowSize = 20;

DependencyTree_Arrow::DependencyTree_Arrow( DependencyTree_Object* theStartItem,
                                            DependencyTree_Object* theEndItem,
                                            QGraphicsItem* parent, QGraphicsScene* scene )
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
:QGraphicsLineItem( parent, scene ),
#else
:QGraphicsLineItem( parent ),
#endif
myIsBiLink( false ),
myStartItem( theStartItem ),
myEndItem( theEndItem )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  myColor = resMgr->colorValue( "Geometry", "dependency_tree_arrow_color", QColor( 0, 0, 130 ) );
  myHighlightColor = resMgr->colorValue( "Geometry", "dependency_tree_highlight_arrow_color", QColor( 0, 0, 255 ) );
  mySelectColor = resMgr->colorValue( "Geometry", "dependency_tree_select_arrow_color", QColor( 255, 0, 0 ) );

  myStartItem = theStartItem;
  myEndItem = theEndItem;

  myLine = QLineF( myStartItem->pos(), myEndItem->pos() );
  myArrowHead  = createArrowHead( myStartItem->pos(), myEndItem->pos() );
  myReverseArrowHead = createArrowHead( myEndItem->pos(), myStartItem->pos() );

  mySelfDependencyArrow = QRectF();

  setPen( QPen( myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  setColor( myColor );

  setFlag( QGraphicsItem::ItemIsSelectable, true );
  setZValue( -1000.0 );
}

DependencyTree_Arrow::~DependencyTree_Arrow()
{
}

//=================================================================================
// function : boundingRect()
// purpose  : return the outer bounds of the item as a rectangle.
//            QGraphicsView uses this to determine whether the item requires redrawing
//=================================================================================
QRectF DependencyTree_Arrow::boundingRect() const
{
  qreal extra;
  QRectF boundingRect;
  if( myStartItem == myEndItem ) {
    extra = arrowSize / 2.0 + 2.0;
    boundingRect = mySelfDependencyArrow;
  }
  else {
    extra = ( pen().width() + 20 ) / 2.0;
    boundingRect = QRectF( myLine.p1(), QSizeF( myLine.p2().x() - myLine.p1().x(),
                                                myLine.p2().y() - myLine.p1().y() ) );
  }
  return boundingRect.normalized().adjusted( -extra, -extra, extra, extra );
}

//=================================================================================
// function : shape()
// purpose  : return the shape of this item to define an area of preselection
//=================================================================================
QPainterPath DependencyTree_Arrow::shape() const
{
  QPainterPath path;

  if( myStartItem == myEndItem ) {
    qreal extra = 5;
    QPolygonF aPolygonBigger( mySelfDependencyArrow.normalized()
              .adjusted( -extra, -extra, extra, extra ) );
    QPolygonF aPolygonSmaller( mySelfDependencyArrow.normalized()
              .adjusted( extra, extra, -extra, -extra ) );
    path.addPolygon( aPolygonBigger.subtracted( aPolygonSmaller ) );
    path.addPolygon(myArrowHead);
  }
  else {
    QPolygonF aShapePolygon;
    QPolygon anArrowHead = myArrowHead.toPolygon();
    QPolygon anReverseArrowHead = myReverseArrowHead.toPolygon();
    aShapePolygon << anArrowHead.point(1) << anArrowHead.point(0) << anArrowHead.point(2) <<
      anReverseArrowHead.point(1) << anReverseArrowHead.point(0) << anReverseArrowHead.point(2);
    path.addPolygon( aShapePolygon );
  }
  return path;
 }

//=================================================================================
// function : setColor()
// purpose  : set default color for arrow
//=================================================================================
void DependencyTree_Arrow::setColor( const QColor& theColor )
{
  myColor = theColor;
}

//=================================================================================
// function : setHighlightColor()
// purpose  : set color for highlighted arrow
//=================================================================================
void DependencyTree_Arrow::setHighlightColor( const QColor& theColor )
{
  myHighlightColor = theColor;
}

//=================================================================================
// function : setSelectColor()
// purpose  : set color for selected arrow
//=================================================================================
void DependencyTree_Arrow::setSelectColor( const QColor& theColor )
{
  mySelectColor = theColor;
}

//=================================================================================
// function : getStartItem()
// purpose  : get start item of arrow
//=================================================================================
DependencyTree_Object* DependencyTree_Arrow::getStartItem() const
{
  return myStartItem;
}

//=================================================================================
// function : getEndItem()
// purpose  : get end item of arrow
//=================================================================================
DependencyTree_Object* DependencyTree_Arrow::getEndItem() const
{
  return myEndItem;
}

//=================================================================================
// function : setIsBiLink()
// purpose  : set true if current arrow is bi-directional link, else set false
//=================================================================================
void DependencyTree_Arrow::setIsBiLink( bool theIsBiLink )
{
  myIsBiLink = theIsBiLink;
}

//=================================================================================
// function : paint()
// purpose  : paint the contents of an item in local coordinates (called by QGraphicsView)
//=================================================================================
void DependencyTree_Arrow::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
  if( isSelected() ) {
    painter->setBrush( mySelectColor );
    painter->setPen( QPen( mySelectColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  }
  else if( isUnderMouse() ) {
    painter->setBrush( myHighlightColor );
    painter->setPen( QPen( myHighlightColor, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  }
  else {
    painter->setBrush( myColor );
    painter->setPen( QPen( myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  }

  if( myStartItem == myEndItem ) {
    int lineSize = 60;
    QPointF p1( myStartItem->pos().x() - myStartItem->boundingRect().width()/2,
                myStartItem->pos().y() );
    QPointF p2( p1.x() - lineSize + myStartItem->boundingRect().height()/2, p1.y() );
    QPointF p3( p2.x(), p2.y() - lineSize );
    QPointF p4( p3.x() + lineSize, p3.y() );
    QPointF p5( p4.x(), p4.y() + lineSize - myStartItem->boundingRect().height()/2 );
    mySelfDependencyArrow = QRectF( p3.x(), p3.y(), lineSize, lineSize );
    myArrowHead = createArrowHead( p4, p5, false );
    QVector<QPointF> pointVector;
    pointVector << p1 << p2 << p2 << p3 << p3 << p4 << p4 << p5;

    painter->drawLines( pointVector);
    painter->drawPolygon(myArrowHead);
  }
  else {
    if (myStartItem->collidesWithItem(myEndItem))
      return;

    myArrowHead  = createArrowHead( myStartItem->pos(), myEndItem->pos() );
    myReverseArrowHead = createArrowHead( myEndItem->pos(), myStartItem->pos() );

    painter->drawLine( myLine );
    painter->drawPolygon( myArrowHead );
    if( myIsBiLink )
      painter->drawPolygon( myReverseArrowHead );
  }
}

//=================================================================================
// function : createArrowHead()
// purpose  : create a head of arrow from start point to end point
//=================================================================================
QPolygonF DependencyTree_Arrow::createArrowHead( QPointF theStartPoint, QPointF theEndPoint,
		                                         bool theIsDynamic )
{
  if( theIsDynamic ) {
    QLineF centerLine( theStartPoint, theEndPoint );
    QPolygonF endPolygon = QPolygonF( myEndItem->boundingRect() );
    QPointF p1 = endPolygon.first() + theEndPoint;
    QPointF p2;
    QPointF intersectPoint;
    QLineF polyLine;
    for( int i = 1; i < endPolygon.count(); ++i ) {
      p2 = endPolygon.at(i) + theEndPoint;
      polyLine = QLineF( p1, p2 );
      QLineF::IntersectType intersectType = polyLine.intersect( centerLine, &intersectPoint );
      if( intersectType == QLineF::BoundedIntersection )
        break;
      p1 = p2;
    }
    myLine = QLineF( intersectPoint, theStartPoint );
  }
  else
    myLine = QLineF( theEndPoint, theStartPoint );

  double angle = acos(myLine.dx() / myLine.length());
  if( myLine.dy() >= 0 )
    angle = ( M_PI * 2 ) - angle;

  QPointF arrowP1 = myLine.p1() + QPointF( sin( angle + M_PI / 3 ) * arrowSize,
                                           cos( angle + M_PI / 3 ) * arrowSize );
  QPointF arrowP2 = myLine.p1() + QPointF( sin( angle + M_PI - M_PI / 3 ) * arrowSize,
                                           cos( angle + M_PI - M_PI / 3 ) * arrowSize );

  QPolygonF anArrowHead;
  anArrowHead << myLine.p1() << arrowP1 << arrowP2;
  return anArrowHead;
}
