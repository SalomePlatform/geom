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
//  File   : GEOM_InteractiveObject.ixx
//  Module : GEOM

#include "GEOM_InteractiveObject.jxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

GEOM_InteractiveObject::~GEOM_InteractiveObject() {}
 


Standard_EXPORT Handle_Standard_Type& GEOM_InteractiveObject_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(SALOME_InteractiveObject);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(SALOME_InteractiveObject);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOM_InteractiveObject",
			                                 sizeof(GEOM_InteractiveObject),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(GEOM_InteractiveObject) Handle(GEOM_InteractiveObject)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(GEOM_InteractiveObject) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOM_InteractiveObject))) {
       _anOtherObject = Handle(GEOM_InteractiveObject)((Handle(GEOM_InteractiveObject)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& GEOM_InteractiveObject::DynamicType() const 
{ 
  return STANDARD_TYPE(GEOM_InteractiveObject) ; 
}
Standard_Boolean GEOM_InteractiveObject::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(GEOM_InteractiveObject) == AType || SALOME_InteractiveObject::IsKind(AType)); 
}
Handle_GEOM_InteractiveObject::~Handle_GEOM_InteractiveObject() {}

