using namespace std;
//  File      : GEOM_ShapeTypeFilter.cxx
//  Created   : Fri Dec 07 09:57:24 2001
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : SALOMEGUI
//  Copyright : Open CASCADE
//  $Header$

#include "GEOM_ShapeTypeFilter.ixx"
#include "GEOM_Client.hxx"

#include "SALOME_InteractiveObject.hxx"
#include "GEOM_InteractiveObject.hxx"
#include "SALOME_TypeFilter.hxx"

#include "utilities.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Study.h"

static GEOM_Client  ShapeReader;

GEOM_ShapeTypeFilter::GEOM_ShapeTypeFilter(TopAbs_ShapeEnum ShapeType,
					   GEOM::GEOM_Gen_ptr geom) 
{
  myKind = ShapeType;
  myComponentGeom = GEOM::GEOM_Gen::_narrow(geom);
}

Standard_Boolean GEOM_ShapeTypeFilter::IsOk(const Handle(SALOME_InteractiveObject)& anObj) const 
{
  Handle(SALOME_TypeFilter) GeomFilter = new SALOME_TypeFilter( "GEOM" );
  if ( !GeomFilter->IsOk(anObj) ) 
    return false;
  if ( anObj->hasEntry() ) {
    QAD_Study*                     ActiveStudy = QAD_Application::getDesktop()->getActiveStudy();
    SALOMEDS::Study_var            aStudy      = ActiveStudy->getStudyDocument();
    SALOMEDS::SObject_var          obj         = aStudy->FindObjectID( anObj->getEntry() );
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
	 
	 MESSAGE ( " myKind          = " << myKind );
	 MESSAGE ( " Shape.ShapeType = " << Shape.ShapeType() );
	 if ( myKind == TopAbs_SHAPE )
	   return true;
	 
	 if ( Shape.ShapeType() == myKind )
	   return true;
	 else
	   return false; 
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
     
     MESSAGE ( " myKind          = " << myKind );
     MESSAGE ( " Shape.ShapeType = " << Shape.ShapeType() );
     if ( myKind == TopAbs_SHAPE )
       return true;
     
     if ( Shape.ShapeType() == myKind )
       return true;
     else
       return false;  
  } 
  return false;
}
