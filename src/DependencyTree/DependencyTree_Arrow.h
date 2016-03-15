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

#ifndef DEPENDENCYTREE_ARROW_H
#define DEPENDENCYTREE_ARROW_H

#include <QGraphicsLineItem>

#include "DependencyTree.h"

class DependencyTree_Object;

class DEPENDENCYTREE_EXPORT DependencyTree_Arrow : public QGraphicsLineItem
{

public:

  DependencyTree_Arrow( DependencyTree_Object* startItem, DependencyTree_Object* endItem,
                        QGraphicsItem* parent = 0, QGraphicsScene* scene = 0 );
  ~DependencyTree_Arrow();

  virtual QRectF          boundingRect() const;
  virtual QPainterPath    shape() const;

  void                    setColor(const QColor& );
  void                    setHighlightColor(const QColor& );
  void                    setSelectColor(const QColor& );

  DependencyTree_Object*  getStartItem() const;
  DependencyTree_Object*  getEndItem() const;

  void                    setIsBiLink( bool );

protected:

  void                    paint( QPainter*, const QStyleOptionGraphicsItem*, QWidget* = 0 );

private:

  QPolygonF               createArrowHead( QPointF , QPointF, bool = true );

  DependencyTree_Object*  myStartItem;
  DependencyTree_Object*  myEndItem;

  QColor                  myColor;
  QColor                  mySelectColor;
  QColor                  myHighlightColor;

  QPolygonF               myArrowHead;
  QPolygonF               myReverseArrowHead;

  bool                    myIsBiLink;

  QRectF                  mySelfDependencyArrow;

  QLineF                  myLine;

};

#endif
