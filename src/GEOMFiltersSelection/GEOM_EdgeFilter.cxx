//  GEOM GEOMFiltersSelection : filter selector for the viewer
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_EdgeFilter.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GEOM_EdgeFilter.ixx"

#include <BRepAdaptor_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>


//=======================================================================
// function : IsShapeOk
// purpose  : enumeration TypeOfEdge is AnyEdge,Line,Circle;
//=======================================================================
GEOM_EdgeFilter::GEOM_EdgeFilter( const StdSelect_TypeOfEdge theKind )
: GEOM_ShapeTypeFilter( TopAbs_EDGE )
{
  myKind = theKind;
}

//=======================================================================
// function : IsShapeOk
// purpose  : 
//=======================================================================
Standard_Boolean GEOM_EdgeFilter::IsShapeOk( const TopoDS_Shape& theShape ) const
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
  return Standard_False;
}
