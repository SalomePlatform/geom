//  GEOM GEOMGUI : Implementation of selection filters for GEOM module
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
//  File   : GEOM_TypeFilter.cxx
//  Author : Sergey LITONIN
//  Module : GEOM

#include "GEOM_TypeFilter.hxx"
#include "SALOME_InteractiveObject.hxx"
#include "GEOM_Client.hxx"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

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

/*
  Class       : GEOM_TypeFilter
  Description : Filter for combaining several filters with logical operation (OR or AND)
*/

IMPLEMENT_STANDARD_HANDLE( GEOM_TypeFilter, SALOME_TypeFilter )
IMPLEMENT_STANDARD_RTTIEXT( GEOM_TypeFilter, SALOME_TypeFilter )

//=======================================================================
// name    : GEOM_TypeFilter::GEOM_TypeFilter
// Purpose : Constructor
//=======================================================================
GEOM_TypeFilter::GEOM_TypeFilter( const int theType )
: SALOME_TypeFilter( "GEOM" )
{
  myType = theType;
}

//=======================================================================
// name    : GEOM_TypeFilter::~GEOM_TypeFilter
// Purpose : Destructor
//=======================================================================
GEOM_TypeFilter::~GEOM_TypeFilter()
{
}

//=======================================================================
// name    : GEOM_TypeFilter::IsOk
// Purpose : Verify validity of entry object
//=======================================================================
Standard_Boolean GEOM_TypeFilter::IsOk( const Handle(SALOME_InteractiveObject)& theIO ) const
{
  if ( SALOME_TypeFilter::IsOk( theIO ) )
  {
    Standard_Boolean aTestRes = Standard_False;
    GEOM::GEOM_Object_var anObj = convertIOinGEOMObject( theIO, aTestRes );
    if ( aTestRes && !anObj->_is_nil() )
      return anObj->GetType() == myType;
  }

  return false;
}

//=======================================================================
// name    : GEOM_TypeFilter::SetType
// Purpose : Set type of object
//=======================================================================
void GEOM_TypeFilter::SetType( const int theType )
{
  myType = theType;
}

//=======================================================================
// name    : GEOM_TypeFilter::GetType
// Purpose : Get type of object
//=======================================================================
int GEOM_TypeFilter::GetType() const
{
  return myType;
}
































