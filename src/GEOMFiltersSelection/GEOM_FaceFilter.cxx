using namespace std;
//  File      : GEOM_FaceFilter.cxx
//  Created   : Fri Dec 07 09:57:24 2001
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE
//  $Header$

#include "GEOM_FaceFilter.ixx"
#include "GEOM_Client.hxx"

#include "SALOME_InteractiveObject.hxx"
#include "GEOM_InteractiveObject.hxx"
#include "GEOM_ShapeTypeFilter.hxx"
#include "SALOME_TypeFilter.hxx"

#include "utilities.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Study.h"

// Open CASCADE Includes
#include <BRepAdaptor_Surface.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>


static GEOM_Client  ShapeReader;

/*!
  enumeration TypeOfFace is AnyFace,Plane,Cylinder,Sphere,Torus,Revol,Cone;
*/
GEOM_FaceFilter::GEOM_FaceFilter(const StdSelect_TypeOfFace Face,
				 GEOM::GEOM_Gen_ptr geom) 
{
  myKind = Face;
  myComponentGeom = GEOM::GEOM_Gen::_narrow(geom);
}

Standard_Boolean GEOM_FaceFilter::IsOk(const Handle(SALOME_InteractiveObject)& anObj) const 
{
  Handle(SALOME_TypeFilter) GeomFilter = new SALOME_TypeFilter( "GEOM" );
  if ( !GeomFilter->IsOk(anObj) ) 
    return false;

  Handle(GEOM_ShapeTypeFilter) GeomShapeTypeFilter = new GEOM_ShapeTypeFilter( TopAbs_FACE, myComponentGeom );
  if ( !GeomShapeTypeFilter->IsOk(anObj) ) 
    return false;

  if ( anObj->hasEntry() ) {
    QAD_Study* ActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
    SALOMEDS::Study_var aStudy = ActiveStudy->getStudyDocument();
    SALOMEDS::SObject_var obj = aStudy->FindObjectID( anObj->getEntry() );
    SALOMEDS::GenericAttribute_var anAttr;
    SALOMEDS::AttributeIOR_var     anIOR;
    if ( !obj->_is_nil() ) {
       if (obj->FindAttribute(anAttr, "AttributeIOR")) {
         anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	 GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( anIOR->Value() );  
	 if ( aShape->_is_nil() )
	   return false;
     
	 TopoDS_Shape    Shape = ShapeReader.GetShape( myComponentGeom, aShape );
	 if ( Shape.IsNull() )
	   return false;
	 
	 switch (myKind) {
	 case StdSelect_AnyFace:
	   return Standard_True;
	 case StdSelect_Plane:
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return (surf.GetType() == GeomAbs_Plane);
	   }
	 case StdSelect_Cylinder:
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return (surf.GetType() == GeomAbs_Cylinder);
	   }
	 case StdSelect_Sphere: 
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return (surf.GetType() == GeomAbs_Sphere);      
	   }
	 case StdSelect_Torus: 
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return ( surf.GetType() == GeomAbs_Torus);      
	   }
	 case StdSelect_Revol: 
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return ( surf.GetType() == GeomAbs_Cylinder || 
		      surf.GetType() == GeomAbs_Cone     ||
		      surf.GetType() == GeomAbs_Torus    ||
		      surf.GetType() == GeomAbs_Sphere	  || 
		      surf.GetType() == GeomAbs_SurfaceOfRevolution ); 
	   }
	 case StdSelect_Cone: // en attendant la liberation du cdl, on l'utilise pour Cone
	   {
	     BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	     return (surf.GetType() == GeomAbs_Cone);      
	   }	 
	 }
       }
    }
  }
    
  if ( anObj->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject)) ) {
    Handle(GEOM_InteractiveObject) GObject =
      Handle(GEOM_InteractiveObject)::DownCast(anObj);
    
    GEOM::GEOM_Shape_var aShape = myComponentGeom->GetIORFromString( GObject->getIOR() );  
    if ( aShape->_is_nil() )
      return false;
    
    TopoDS_Shape    Shape = ShapeReader.GetShape( myComponentGeom, aShape );
    if ( Shape.IsNull() )
      return false;
    
    switch (myKind) {
    case StdSelect_AnyFace:
      return Standard_True;
    case StdSelect_Plane:
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return (surf.GetType() == GeomAbs_Plane);
      }
    case StdSelect_Cylinder:
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return (surf.GetType() == GeomAbs_Cylinder);
      }
    case StdSelect_Sphere: 
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return (surf.GetType() == GeomAbs_Sphere);      
      }
    case StdSelect_Torus: 
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return ( surf.GetType() == GeomAbs_Torus);      
      }
    case StdSelect_Revol: 
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return ( surf.GetType() == GeomAbs_Cylinder || 
		 surf.GetType() == GeomAbs_Cone     ||
		 surf.GetType() == GeomAbs_Torus    ||
		 surf.GetType() == GeomAbs_Sphere	  || 
		 surf.GetType() == GeomAbs_SurfaceOfRevolution ); 
      }
    case StdSelect_Cone: // en attendant la liberation du cdl, on l'utilise pour Cone
      {
	BRepAdaptor_Surface surf(TopoDS::Face(Shape));
	return (surf.GetType() == GeomAbs_Cone);      
      }	 
    }
  }
  return false;
}
