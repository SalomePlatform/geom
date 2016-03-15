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

#include "GEOM_EdgeFilter.h"

#include <BRepAdaptor_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <StdSelect_TypeOfEdge.hxx>

//=======================================================================
// function : GEOM_EdgeFilter
// purpose  : 
//=======================================================================
GEOM_EdgeFilter::GEOM_EdgeFilter( SalomeApp_Study* study, const int kind )
: GEOM_SelectionFilter( study ),
myKind( kind )
{
  add( TopAbs_EDGE );
}

//=======================================================================
// function : ~GEOM_SelectionFilter
// purpose  : 
//=======================================================================
GEOM_EdgeFilter::~GEOM_EdgeFilter()
{
}

//=======================================================================
// function : isShapeOk
// purpose  : 
//=======================================================================
bool GEOM_EdgeFilter::isShapeOk( const TopoDS_Shape& theShape ) const
{
  if ( !theShape.IsNull() && theShape.ShapeType() == TopAbs_EDGE )
  {
    BRepAdaptor_Curve aCurve( TopoDS::Edge( theShape ) );
    GeomAbs_CurveType aType = aCurve.GetType();
        
    switch ( myKind ) 
    {
    case StdSelect_AnyEdge: return Standard_True;
    case StdSelect_Line:    return ( aType == GeomAbs_Line );
    case StdSelect_Circle:  return ( aType == GeomAbs_Circle );
    }
  }
  return false;
}

