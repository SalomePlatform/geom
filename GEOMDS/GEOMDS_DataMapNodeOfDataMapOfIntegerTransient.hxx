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
//  File   : GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx
//  Module : GEOM

#ifndef _GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_HeaderFile
#define _GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_HeaderFile
#include "Handle_GEOMDS_DataMapNodeOfDataMapOfIntegerTransient.hxx"
#endif

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Handle_Standard_Transient_HeaderFile
#include <Handle_Standard_Transient.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif
class Standard_Transient;
class TColStd_MapIntegerHasher;
class GEOMDS_DataMapOfIntegerTransient;
class GEOMDS_DataMapIteratorOfDataMapOfIntegerTransient;


class GEOMDS_DataMapNodeOfDataMapOfIntegerTransient : public TCollection_MapNode {

public:

    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
 // Methods PUBLIC
 // 
Standard_EXPORT inline GEOMDS_DataMapNodeOfDataMapOfIntegerTransient(const Standard_Integer& K,const Handle(Standard_Transient)& I,const TCollection_MapNodePtr& n);
Standard_EXPORT inline   Standard_Integer& Key() const;
Standard_EXPORT inline   Handle_Standard_Transient& Value() const;
Standard_EXPORT ~GEOMDS_DataMapNodeOfDataMapOfIntegerTransient();




 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOMDS_DataMapNodeOfDataMapOfIntegerTransient_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //
Standard_Integer myKey;
Handle_Standard_Transient myValue;


};

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

#include <TCollection_DataMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef TheItem
#undef TheItem_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_DataMapNode
#undef TCollection_DataMapNode_hxx
#undef TCollection_DataMapIterator
#undef TCollection_DataMapIterator_hxx
#undef Handle_TCollection_DataMapNode
#undef TCollection_DataMapNode_Type_
#undef TCollection_DataMap
#undef TCollection_DataMap_hxx


// other inline functions and methods (like "C++: function call" methods)
//


#endif
