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

