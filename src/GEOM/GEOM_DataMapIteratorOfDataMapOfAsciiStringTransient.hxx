
#ifndef _GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_HeaderFile
#define _GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient_HeaderFile

#ifndef _TCollection_BasicMapIterator_HeaderFile
#include <TCollection_BasicMapIterator.hxx>
#endif
#ifndef _Handle_Standard_Transient_HeaderFile
#include <Handle_Standard_Transient.hxx>
#endif
#ifndef _Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient_HeaderFile
#include <Handle_GEOM_DataMapNodeOfDataMapOfAsciiStringTransient.hxx>
#endif
class Standard_NoSuchObject;
class TCollection_AsciiString;
class Standard_Transient;
class GEOM_DataMapOfAsciiStringTransient;
class GEOM_DataMapNodeOfDataMapOfAsciiStringTransient;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient  : public TCollection_BasicMapIterator {

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
Standard_EXPORT GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient();
Standard_EXPORT GEOM_DataMapIteratorOfDataMapOfAsciiStringTransient(const GEOM_DataMapOfAsciiStringTransient& aMap);
Standard_EXPORT   void Initialize(const GEOM_DataMapOfAsciiStringTransient& aMap) ;
Standard_EXPORT  const TCollection_AsciiString& Key() const;
Standard_EXPORT  const Handle_Standard_Transient& Value() const;





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


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
