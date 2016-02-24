// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_TopWireframeShape.cxx
//  Author : 
//  Module : GEOM

/*!
  \class GEOM_TopWireframeShape GEOM_TopWireframeShape.hxx
  \brief This class designed for drawing special OCC wireframe
         presentation on the top level of the viewer.         
*/

//Local includes
#include "GEOM_TopWireframeShape.hxx"
#include "GEOM_AISShape.hxx"

//GUI includes
#include <SALOME_InteractiveObject.hxx>

//Open CASCADE includes
#include <TopoDS_Shape.hxx>
#include <Prs3d_IsoAspect.hxx>

OCCT_IMPLEMENT_STANDARD_RTTIEXT(GEOM_TopWireframeShape, SALOME_AISShape)

GEOM_TopWireframeShape::GEOM_TopWireframeShape( const TopoDS_Shape& shape )
  :SALOME_AISShape(shape)
{
  SetDisplayMode( AIS_WireFrame );
  Handle(Prs3d_IsoAspect) anAspect = Attributes()->UIsoAspect();
  anAspect->SetNumber( 0 );
  Attributes()->SetUIsoAspect( anAspect );
  anAspect = Attributes()->VIsoAspect();
  anAspect->SetNumber( 0 );
  Attributes()->SetVIsoAspect( anAspect );
  SetColor( GEOM_AISShape::topLevelColor() );
}

GEOM_TopWireframeShape::~GEOM_TopWireframeShape()
{
}

Handle(SALOME_InteractiveObject) GEOM_TopWireframeShape::getIO()
{
  Handle(SALOME_InteractiveObject) IO;
  if ( !GetOwner().IsNull() )
    IO = Handle(SALOME_InteractiveObject)::DownCast( GetOwner() );
  return IO;
}

Standard_Boolean GEOM_TopWireframeShape::hasIO()
{
  return !getIO().IsNull();
}

void GEOM_TopWireframeShape::setName( const Standard_CString /*aName*/ )
{
}

Standard_CString GEOM_TopWireframeShape::getName()
{
  return "";
}


void GEOM_TopWireframeShape::highlightSubShapes( const TColStd_IndexedMapOfInteger& /*aIndexMap*/,
                                                 const Standard_Boolean /*aHighlight*/ )
{
}

Standard_Boolean GEOM_TopWireframeShape::isTopLevel()
{
  return Standard_True;
}

void GEOM_TopWireframeShape::setTopLevel( Standard_Boolean /*f*/ )
{
}

Standard_Boolean GEOM_TopWireframeShape::toActivate()
{
  return Standard_True;
}

Standard_Boolean GEOM_TopWireframeShape::switchTopLevel()
{
  return Standard_True;
}

void GEOM_TopWireframeShape::setIO( const Handle(SALOME_InteractiveObject)& io )
{
  SetOwner( io );
}
