//  GEOM OBJECT : interactive object for Geometry entities visualization
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
//  File   : GEOM_InteractiveObject.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

using namespace std;
/*!
  \class GEOM_InteractiveObject GEOM_InteractiveObject.hxx
  \brief ....
*/

#include "GEOM_InteractiveObject.ixx"

GEOM_InteractiveObject::GEOM_InteractiveObject()
  : SALOME_InteractiveObject()
{
  myIOR       = "";
  myFatherIOR = "";
}

GEOM_InteractiveObject::GEOM_InteractiveObject(const Standard_CString anIOR,
					       const Standard_CString aFatherIOR,
					       const Standard_CString aComponentDataType,
					       const Standard_CString anEntry)
  : SALOME_InteractiveObject(anEntry,aComponentDataType)
{
  myIOR       = new char [strlen(anIOR)+1];
  strcpy( myIOR, anIOR);
  myFatherIOR = new char [strlen(aFatherIOR)+1];
  strcpy( myFatherIOR, aFatherIOR);
}

Standard_CString GEOM_InteractiveObject::getIOR(){
  return myIOR;
}
Standard_CString GEOM_InteractiveObject::getFatherIOR(){
  return myFatherIOR;
}

Standard_Boolean GEOM_InteractiveObject::isSame(const Handle(SALOME_InteractiveObject)& anIO ){
  if ( anIO->hasEntry() && this->hasEntry() ) {
    if ( strcmp(myEntry, anIO->getEntry() ) == 0 )
      return Standard_True;
  }
  
  if ( anIO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
    Handle(GEOM_InteractiveObject) theIO = Handle(GEOM_InteractiveObject)::DownCast( anIO );
    if ( strcmp(myIOR, theIO->getIOR() ) == 0 )
      return Standard_True;
  }

  return Standard_False;
}
