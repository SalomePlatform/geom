
#ifndef _GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_HeaderFile
#define _GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_HeaderFile
#include <Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx>
#endif

#ifndef _TCollection_AsciiString_HeaderFile
#include <TCollection_AsciiString.hxx>
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
class TCollection_AsciiString;
class GEOM_DataMapOfAsciiStringTransient;
class GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient;


class GEOM_DataMapNodeOfDataMapOfAsciiStringTransient : public TCollection_MapNode {

public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
 // Methods PUBLIC
 // 
GEOM_DataMapNodeOfDataMapOfAsciiStringTransient(const TCollection_AsciiString& K,const Handle(Standard_Transient)& I,const TCollection_MapNodePtr& n);
  TCollection_AsciiString& Key() const;
  Handle_Standard_Transient& Value() const;
Standard_EXPORT ~GEOM_DataMapNodeOfDataMapOfAsciiStringTransient();




 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_Type_();
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
TCollection_AsciiString myKey;
Handle_Standard_Transient myValue;


};

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


// other Inline functions and methods (like "C++: function call" methods)
//


#endif
