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
//  File   : GEOM_FaceFilter.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GEOM_FaceFilter.ixx"

#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>


//=======================================================================
// function : IsShapeOk
// purpose  : enumeration TypeOfFace is AnyFace,Plane,Cylinder,Sphere,Torus,Revol,Cone;
//=======================================================================
GEOM_FaceFilter::GEOM_FaceFilter( const StdSelect_TypeOfFace theKind ) 
: GEOM_ShapeTypeFilter( TopAbs_FACE )
{
  myKind = theKind;
}

//=======================================================================
// function : IsShapeOk
// purpose  : 
//=======================================================================
Standard_Boolean GEOM_FaceFilter::IsShapeOk( const TopoDS_Shape& theShape ) const
{
  if ( !theShape.IsNull() && theShape.ShapeType() == TopAbs_FACE )
  {
    BRepAdaptor_Surface aSurf( TopoDS::Face( theShape ) );
    GeomAbs_SurfaceType aType = aSurf.GetType();
	
    switch ( myKind ) 
    {
    case StdSelect_AnyFace:   return Standard_True;
    case StdSelect_Plane:     return ( aType == GeomAbs_Plane );
    case StdSelect_Cylinder:  return ( aType == GeomAbs_Cylinder);
    case StdSelect_Sphere:    return ( aType == GeomAbs_Sphere);      
    case StdSelect_Torus:     return ( aType == GeomAbs_Torus);      
    case StdSelect_Revol:     return ( aType == GeomAbs_Cylinder  || 
				       aType == GeomAbs_Cone      ||
		                       aType == GeomAbs_Torus     ||
		                       aType == GeomAbs_Sphere    || 
		                       aType == GeomAbs_SurfaceOfRevolution ); 
    case StdSelect_Cone:      return ( aType == GeomAbs_Cone);      
    }
  }
  return Standard_False;
}
