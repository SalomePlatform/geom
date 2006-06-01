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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOM_InteractiveObject.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

/*!
  \class GEOM_InteractiveObject GEOM_InteractiveObject.hxx
  \brief ....
*/

#include "GEOM_InteractiveObject.ixx"

using namespace std;

GEOM_InteractiveObject::GEOM_InteractiveObject()
  : SALOME_InteractiveObject()
{
  myIOR       = "";
  myFatherIOR = "";
}

GEOM_InteractiveObject::GEOM_InteractiveObject(const char* anIOR,
					       const char* aFatherIOR,
					       const char* aComponentDataType,
					       const char* anEntry)
  : SALOME_InteractiveObject(anEntry,aComponentDataType)
{
  myIOR       = anIOR;
  myFatherIOR = aFatherIOR;
}

const char* GEOM_InteractiveObject::getIOR(){
  return myIOR.c_str();
}

const char* GEOM_InteractiveObject::getFatherIOR(){
  return myFatherIOR.c_str();
}

Standard_Boolean GEOM_InteractiveObject::isSame(const Handle(SALOME_InteractiveObject)& anIO ){
  if ( anIO->hasEntry() && this->hasEntry() ) {
    if ( myEntry == anIO->getEntry() )
      return Standard_True;
  }
  
  if ( anIO->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
    Handle(GEOM_InteractiveObject) theIO = Handle(GEOM_InteractiveObject)::DownCast( anIO );
    if ( myIOR == theIO->getIOR() )
      return Standard_True;
  }

  return Standard_False;
}
