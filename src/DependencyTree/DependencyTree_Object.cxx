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
#include "DependencyTree_Object.h"

// GEOM includes
#include <GeometryGUI.h>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

// Qt includes
#include <QFont>

const int itemH = 20;
const int itemW = 90;

DependencyTree_Object::DependencyTree_Object( const std::string& theEntry, QGraphicsItem* theParent )
:GraphicsView_Object( theParent ),
myIsMainObject( false )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  myColor = resMgr->colorValue( "Geometry", "dependency_tree_node_color", QColor( 62, 180, 238 ) );
  mySelectColor = resMgr->colorValue( "Geometry", "dependency_tree_select_node_color", QColor( 237, 243, 58 ) );
  myMainObjectColor = resMgr->colorValue( "Geometry", "dependency_tree_main_node_color", QColor( 238, 90, 125 ) );
  myUnpublishObjectColor = resMgr->colorValue( "Geometry", "dependency_tree_unpublish_node_color", QColor( 255, 255, 255 ) );

  myPolygonItem = new QGraphicsPolygonItem();
  QPolygonF myPolygon;
  myPolygon << QPointF( -itemW, -itemH ) << QPointF( itemW, -itemH ) << QPointF( itemW, itemH )
            << QPointF( -itemW, itemH )  << QPointF( -itemW, -itemH );

  myPolygonItem->setPolygon( myPolygon );

  myTextItem = new QGraphicsSimpleTextItem();
  QFont textFont;
  textFont.setPointSize( itemH );
  myTextItem->setFont( textFont );

  myEntry = theEntry;

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app ) return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy( study->studyDS() );
  int studyId = aStudyDS->StudyId();
  myGeomObject = GeometryGUI::GetGeomGen()->GetObject( studyId, myEntry.c_str() );

  updateName();

  addToGroup( myPolygonItem );
  addToGroup( myTextItem );
}

DependencyTree_Object::~DependencyTree_Object()
{
}

//=================================================================================
// function : highlight()
// purpose  : highlight current item
//=================================================================================
bool DependencyTree_Object::highlight( double theX, double theY )
{
  if( !isHighlighted() ) {
    QColor color = myPolygonItem->brush().color();
    int saturation = ( color.saturation() - 100 ) > 10 ? color.saturation() - 100 : 10;
    color.setHsv( color.hsvHue(), saturation, color.value() );

    myPolygonItem->setBrush( color );
    myPolygonItem->setPen( getPen( color ) );

    myPolygonItem->setToolTip( getName() );
  }
  return GraphicsView_Object::highlight( theX, theY );
}

//=================================================================================
// function : unhighlight()
// purpose  : set properties for unhighlighted item
//=================================================================================
void DependencyTree_Object::unhighlight()
{
  if( isSelected() )
    myPolygonItem->setBrush( mySelectColor );
  else if( myIsMainObject )
    myPolygonItem->setBrush( myMainObjectColor );
  else
    myPolygonItem->setBrush( myColor );

  myPolygonItem->setPen( getPen( myPolygonItem->brush().color() ) );

  GraphicsView_Object::unhighlight();
}

//=================================================================================
// function : select()
// purpose  : select current item
//=================================================================================
bool DependencyTree_Object::select( double theX, double theY, const QRectF& theRect )
{
  myPolygonItem->setBrush( mySelectColor );
  myPolygonItem->setPen( getPen( mySelectColor ) );

  return GraphicsView_Object::select( theX, theY, theRect );
}

//=================================================================================
// function : unselect()
// purpose  : set properties for unselected item
//=================================================================================
void DependencyTree_Object::unselect()
{
  if( myIsMainObject )
    myPolygonItem->setBrush( myMainObjectColor );
  else
    myPolygonItem->setBrush( myColor );

  myPolygonItem->setPen( getPen( myPolygonItem->brush().color() ) );

  GraphicsView_Object::unselect();
}

//=================================================================================
// function : getEntry()
// purpose  : get entry of current item
//=================================================================================
std::string DependencyTree_Object::getEntry() const
{
  return myEntry;
}

//=================================================================================
// function : getGeomObject()
// purpose  : get geometry object of current item
//=================================================================================
GEOM::GEOM_BaseObject_var DependencyTree_Object::getGeomObject() const
{
  return myGeomObject;
}

//=================================================================================
// function : updateName()
// purpose  : update name of current item using its entry
//=================================================================================
void DependencyTree_Object::updateName()
{

  QString name = myGeomObject->GetName();
  CORBA::String_var studyEntryVar = myGeomObject->GetStudyEntry();

  if( QString( studyEntryVar.in() ).isEmpty() ) {
	if( name.isEmpty() )
      name = "unpublished";
    myColor = myUnpublishObjectColor;
  }

  myPolygonItem->setBrush( myColor );
  myPolygonItem->setPen( getPen( myColor ) );

  setName( name );

  myTextItem->setText( name );
  double textWidth = myTextItem->sceneBoundingRect().width();
  double textHeight = myTextItem->sceneBoundingRect().height();

  double polygonWidth = myPolygonItem->sceneBoundingRect().width();
  double polygonHeight = myPolygonItem->sceneBoundingRect().height();

  if( ( textWidth - 4 ) > polygonWidth ) {
    int numberSymbol = int( polygonWidth * name.length() / textWidth );
    QString newName = name.left( numberSymbol - 3 ) + "...";
    myTextItem->setText( newName );
    textWidth = myTextItem->sceneBoundingRect().width();
  }
  myTextItem->setPos( ( polygonWidth - textWidth ) / 2 - itemW,
                      ( polygonHeight - textHeight ) / 2 - itemH );
}

//=================================================================================
// function : setColor()
// purpose  : set default color of current item
//=================================================================================
void DependencyTree_Object::setColor( const QColor& theColor )
{
  CORBA::String_var studyEntryVar = myGeomObject->GetStudyEntry();
  if( QString( studyEntryVar.in() ).isEmpty() )
    myColor = myUnpublishObjectColor;
  else
    myColor = theColor;

}

//=================================================================================
// function : setSelectColor()
// purpose  : set color of selected item
//=================================================================================
void DependencyTree_Object::setSelectColor( const QColor& theColor )
{
  mySelectColor = theColor;
}

//=================================================================================
// function : setMainObjectColor()
// purpose  : set color if current item is main object of dependency tree
//=================================================================================
void DependencyTree_Object::setMainObjectColor( const QColor& theColor )
{
  myMainObjectColor = theColor;
}

//=================================================================================
// function : setUnpublishObjectColor()
// purpose  : set color if current item is unpublished object
//=================================================================================
void DependencyTree_Object::setUnpublishObjectColor( const QColor& theColor )
{
  myUnpublishObjectColor = theColor;
  CORBA::String_var studyEntryVar = myGeomObject->GetStudyEntry();
  if( QString( studyEntryVar.in() ).isEmpty() )
    myColor = myUnpublishObjectColor;
}

//=================================================================================
// function : setIsMainObject()
// purpose  : set true if current item is main object of dependency tree
//=================================================================================
void DependencyTree_Object::setIsMainObject( bool theIsMainObject )
{
  myIsMainObject = theIsMainObject;

  if( myIsMainObject )
    myPolygonItem->setBrush( myMainObjectColor );
  else
    myPolygonItem->setBrush( myColor );

  myPolygonItem->setPen( getPen( myPolygonItem->brush().color() ) );
}

//=================================================================================
// function : getPen()
// purpose  : get pen which is dependent of current color
//=================================================================================
QPen DependencyTree_Object::getPen( const QColor& theColor )
{
  int value = ( theColor.value() - 100 ) > 10 ? theColor.value() - 100 : 10;
  QColor penColor;
  penColor.setHsv( theColor.hsvHue(), theColor.saturation(), value );
  return QPen( QBrush( penColor ), 4 );
}
