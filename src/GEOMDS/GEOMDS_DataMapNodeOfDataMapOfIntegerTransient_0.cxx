//  GEOM GEOMDS : implementation of Geometry component data structure and Geometry documents management
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
//  File   : GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_0.cxx
//  Module : GEOM

using namespace std;
#include "GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_Transient_HeaderFile
#include <Standard_Transient.hxx>
#endif
#ifndef _TColStd_MapIntegerHasher_HeaderFile
#include <TColStd_MapIntegerHasher.hxx>
#endif
#ifndef _GEOMDS_DataMapOfIntegerTransient_HeaderFile
#include "GEOMDS_DataMapOfIntegerTransient.hxx"
#endif
#ifndef _GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient_HeaderFile
#include "GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient.hxx"
#endif
GEOMDS_DataMapNodeOfDataMapOfIntegerTransient::~GEOMDS_DataMapNodeOfDataMapOfIntegerTransient() {}
 


Standard_EXPORT Handle_Standard_Type& GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMDS_DataMapNodeOfDataMapOfIntegerTransient",
			                                 sizeof(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient) Handle(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient))) {
       _anOtherObject = Handle(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient)((Handle(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& GEOMDS_DataMapNodeOfDataMapOfIntegerTransient::DynamicType() const 
{ 
  return STANDARD_TYPE(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient) ; 
}
Standard_Boolean GEOMDS_DataMapNodeOfDataMapOfIntegerTransient::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(GEOMDS_DataMapNodeOfDataMapOfIntegerTransient) == AType || TCollection_MapNode::IsKind(AType)); 
}
Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient::~Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient() {}
#define TheKey Standard_Integer
#define TheKey_hxx <Standard_Integer.hxx>
#define TheItem Handle_Standard_Transient
#define TheItem_hxx <Standard_Transient.hxx>
#define Hasher TColStd_MapIntegerHasher
#define Hasher_hxx <TColStd_MapIntegerHasher.hxx>
#define TCollection_DataMapNode GEOMDS_DataMapNodeOfDataMapOfIntegerTransient
#define TCollection_DataMapNode_hxx <GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx>
#define TCollection_DataMapIterator GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient
#define TCollection_DataMapIterator_hxx <GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient
#define TCollection_DataMapNode_Type_() GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_Type_()
#define TCollection_DataMap GEOMDS_DataMapOfIntegerTransient
#define TCollection_DataMap_hxx <GEOMDS_DataMapOfIntegerTransient.hxx>
#include <TCollection_DataMapNode.gxx>

