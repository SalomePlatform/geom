//  File   : GEOM_Application.hxx
//  Module : GEOM

#ifndef _GEOM_Application_HeaderFile
#define _GEOM_Application_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_GEOM_Application_HeaderFile
#include <Handle_GEOM_Application.hxx>
#endif

#ifndef _TDocStd_Application_HeaderFile
#include <TDocStd_Application.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
class TColStd_SequenceOfExtendedString;


class GEOM_Application : public TDocStd_Application {

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
Standard_EXPORT GEOM_Application();
Standard_EXPORT virtual  void Formats(TColStd_SequenceOfExtendedString& Formats) ;
Standard_EXPORT Standard_CString ResourcesName() ;
Standard_EXPORT ~GEOM_Application();




 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_Application_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)&) const;

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





// other inline functions and methods (like "C++: function call" methods)
//


#endif
