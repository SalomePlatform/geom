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
#include <BRepAdaptor_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>


static GEOM_Client  ShapeReader;


/*!
  enumeration TypeOfEdge is AnyEdge,Line,Circle;
*/
GEOM_EdgeFilter::GEOM_EdgeFilter(const StdSelect_TypeOfEdge Edge,
				 GEOM::GEOM_Gen_ptr geom) 
{
  myKind = Edge;
  myComponentGeom = GEOM::GEOM_Gen::_narrow(geom);
}

Standard_Boolean GEOM_EdgeFilter::IsOk(const Handle(SALOME_InteractiveObject)& anObj) const 
{
  Handle(SALOME_TypeFilter) GeomFilter = new SALOME_TypeFilter( "GEOM" );
  if ( !GeomFilter->IsOk(anObj) ) 
    return false;

  Handle(GEOM_ShapeTypeFilter) GeomShapeTypeFilter = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myComponentGeom );
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
	 case StdSelect_AnyEdge:
	   return Standard_True;
	 case StdSelect_Line:
	   {
	     BRepAdaptor_Curve curv(TopoDS::Edge(Shape));
	     return (curv.GetType() == GeomAbs_Line);
	   }
	   break;
	 case StdSelect_Circle:
	   BRepAdaptor_Curve curv(TopoDS::Edge(Shape));
	   return (curv.GetType() == GeomAbs_Circle);
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
    case StdSelect_AnyEdge:
      return Standard_True;
    case StdSelect_Line:
      {
	BRepAdaptor_Curve curv(TopoDS::Edge(Shape));
	return (curv.GetType() == GeomAbs_Line);
      }
      break;
    case StdSelect_Circle:
      BRepAdaptor_Curve curv(TopoDS::Edge(Shape));
      return (curv.GetType() == GeomAbs_Circle);
    }
  }
  return false;
}
