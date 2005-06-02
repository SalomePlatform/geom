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

