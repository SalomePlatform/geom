using namespace std;
//  File      : GEOM_InteractiveObject.cxx
//  Created   : Wed Feb 20 17:24:59 2002
//  Author    : Christophe ATTANASIO
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$

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
