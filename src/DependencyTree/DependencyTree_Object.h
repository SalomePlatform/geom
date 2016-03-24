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

#ifndef DEPENDENCYTREE_OBJECT_H
#define DEPENDENCYTREE_OBJECT_H

#include <GraphicsView_Object.h>

// GEOM includes
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)

#include <QPen>

#include "DependencyTree.h"

class DEPENDENCYTREE_EXPORT DependencyTree_Object: public GraphicsView_Object
{

public:

  DependencyTree_Object( const std::string&, QGraphicsItem* = 0 );
  ~DependencyTree_Object();

  virtual void                compute() {};

  virtual bool                highlight( double, double );
  virtual void                unhighlight();

  virtual bool                select( double, double, const QRectF& );
  virtual void                unselect();

  std::string                 getEntry() const;

  GEOM::GEOM_BaseObject_var   getGeomObject() const;

  void                        updateName();

  void                        setColor( const QColor& );
  void                        setSelectColor( const QColor& );
  void                        setMainObjectColor( const QColor& );
  void                        setUnpublishObjectColor( const QColor& );

  void                        setIsMainObject( bool );

private:

  QPen                        getPen( const QColor& );

  QColor                      myColor;
  QColor                      mySelectColor;
  QColor                      myMainObjectColor;
  QColor                      myUnpublishObjectColor;

  QGraphicsPolygonItem*       myPolygonItem;
  QGraphicsSimpleTextItem*    myTextItem;

  GEOM::GEOM_BaseObject_var   myGeomObject;
  std::string                 myEntry;

  bool                        myIsMainObject;

};

#endif
