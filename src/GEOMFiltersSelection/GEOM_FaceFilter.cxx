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

#include "GEOM_FaceFilter.h"

#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <StdSelect_TypeOfFace.hxx>

//=======================================================================
// function : GEOM_FaceFilter
// purpose  : 
//=======================================================================
GEOM_FaceFilter::GEOM_FaceFilter( SalomeApp_Study* study, const int kind )
: GEOM_SelectionFilter( study ),
myKind( kind )
{
  add( TopAbs_FACE );
}

//=======================================================================
// function : ~GEOM_SelectionFilter
// purpose  : 
//=======================================================================
GEOM_FaceFilter::~GEOM_FaceFilter()
{
}

//=======================================================================
// function : isShapeOk
// purpose  : 
//=======================================================================
bool GEOM_FaceFilter::isShapeOk( const TopoDS_Shape& theShape ) const
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
  return false;
}

