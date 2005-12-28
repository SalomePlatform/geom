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
//  File   : GEOM_ShapeTypeFilter.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include <Standard_Stream.hxx>

#include "GEOM_ShapeTypeFilter.ixx"
#include "SALOME_InteractiveObject.hxx"
#include "GEOM_Client.hxx"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

#include <TopoDS_Shape.hxx>

//=======================================================================
// function : getShape()
// purpose  : returns a TopoDS_Shape stored in GEOM_Object
//=======================================================================
static bool getShape( const GEOM::GEOM_Object_ptr& theObject, TopoDS_Shape& theShape )
{
  if ( !CORBA::is_nil( theObject ) )
  {
    static GEOM::GEOM_Gen_var myGeom;
    if(CORBA::is_nil(myGeom) {
      Engines::Component_var comp = QAD_Application::getDesktop()->getEngine( "FactoryServer", "GEOM" );
      myGeom = GEOM::GEOM_Gen::_narrow( comp );
    }
    TopoDS_Shape aTopoDSShape = GEOM_Client().GetShape( myGeom, theObject );
        
    if ( !aTopoDSShape.IsNull() )
    {
      theShape = aTopoDSShape;
       return true;
    }
  }
  return false;
}

//=======================================================================
// function : ConvertIOinGEOMObject()
// purpose  :
//=======================================================================
static GEOM::GEOM_Object_ptr convertIOinGEOMObject(
  const Handle(SALOME_InteractiveObject)& theIO, Standard_Boolean& theResult )
{
  theResult = Standard_False;
  GEOM::GEOM_Object_var aReturnObject;
  if ( !theIO.IsNull() )
  {
    const char* anEntry = theIO->getEntry();
    SALOMEDS::SObject_var aSObj =
      QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument()->FindObjectID( anEntry );
    if ( !CORBA::is_nil( aSObj ) )
    {
      aReturnObject = GEOM::GEOM_Object::_narrow( aSObj->GetObject() );
      theResult = !CORBA::is_nil( aReturnObject );
    }
  }
  return aReturnObject._retn();
}

//=======================================================================
// function : ShapeTypeFilter
// purpose  : 
//=======================================================================
GEOM_ShapeTypeFilter::GEOM_ShapeTypeFilter( const TopAbs_ShapeEnum theShapeType,
                                            const bool theIsAll ) 
{
  myIsAll = theIsAll;
  myShapeTypes.Add( theShapeType );
  myTypeFilter = new SALOME_TypeFilter( "GEOM" );
}

//=======================================================================
// function : ShapeTypeFilter
// purpose  : 
//=======================================================================
GEOM_ShapeTypeFilter::GEOM_ShapeTypeFilter( const TColStd_MapOfInteger& theShapeTypes,
                                            const bool theIsAll ) 
{
  myIsAll = theIsAll;
  myShapeTypes = theShapeTypes;
  myTypeFilter = new SALOME_TypeFilter( "GEOM" );
}

//=======================================================================
// function : IsOk
// purpose  : 
//=======================================================================
Standard_Boolean GEOM_ShapeTypeFilter::IsOk(
  const Handle(SALOME_InteractiveObject)& anObj ) const 
{
  if ( !myTypeFilter->IsOk(anObj) ) 
    return Standard_False;

  Standard_Boolean aResult = Standard_False;
  GEOM::GEOM_Object_ptr aGeomObj = convertIOinGEOMObject( anObj, aResult );
  if ( !CORBA::is_nil( aGeomObj ) && aResult && aGeomObj->IsShape() )
  {
    if ( myIsAll )
      return true;
    TopoDS_Shape aShape;
    if ( getShape( aGeomObj, aShape ) )
    {
      if ( myShapeTypes.Contains( aShape.ShapeType() ) )
        return IsShapeOk( aShape );
    }
  }
  return Standard_False;
}

//=======================================================================
// function : IsShapeOk
// purpose  : 
//=======================================================================
Standard_Boolean GEOM_ShapeTypeFilter::IsShapeOk( const TopoDS_Shape& ) const
{
  return Standard_True;
}
























