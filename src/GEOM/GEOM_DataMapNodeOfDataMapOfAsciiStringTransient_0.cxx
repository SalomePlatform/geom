
#include <GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx>

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_Transient_HeaderFile
#include <Standard_Transient.hxx>
#endif
#ifndef _TCollection_AsciiString_HeaderFile
#include <TCollection_AsciiString.hxx>
#endif
#ifndef _GEOM_DataMapOfAsciiStringTransient_HeaderFile
#include <GEOM_DataMapOfAsciiStringTransient.hxx>
#endif
#ifndef _GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_HeaderFile
#include <GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx>
#endif
GEOM_DataMapNodeOfDataMapOfAsciiStringTransient::~GEOM_DataMapNodeOfDataMapOfAsciiStringTransient() {}
 


Standard_EXPORT Handle_Standard_Type& GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOM_DataMapNodeOfDataMapOfAsciiStringTransient",
			                                 sizeof(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient) Handle(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient))) {
       _anOtherObject = Handle(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient)((Handle(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& GEOM_DataMapNodeOfDataMapOfAsciiStringTransient::DynamicType() const 
{ 
  return STANDARD_TYPE(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient) ; 
}
Standard_Boolean GEOM_DataMapNodeOfDataMapOfAsciiStringTransient::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(GEOM_DataMapNodeOfDataMapOfAsciiStringTransient) == AType || TCollection_MapNode::IsKind(AType)); 
}
Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient::~Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient() {}
#define TheKey TCollection_AsciiString
#define TheKey_hxx <TCollection_AsciiString.hxx>
#define TheItem Handle_Standard_Transient
#define TheItem_hxx <Standard_Transient.hxx>
#define Hasher TCollection_AsciiString
#define Hasher_hxx <TCollection_AsciiString.hxx>
#define TCollection_DataMapNode GEOM_DataMapNodeOfDataMapOfAsciiStringTransient
#define TCollection_DataMapNode_hxx <GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx>
#define TCollection_DataMapIterator GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient
#define TCollection_DataMapIterator_hxx <GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient
#define TCollection_DataMapNode_Type_() GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_Type_()
#define TCollection_DataMap GEOM_DataMapOfAsciiStringTransient
#define TCollection_DataMap_hxx <GEOM_DataMapOfAsciiStringTransient.hxx>
#include <TCollection_DataMapNode.gxx>

