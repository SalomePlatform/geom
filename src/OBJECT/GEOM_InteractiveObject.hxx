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
//  File   : GEOM_InteractiveObject.hxx
//  Module : GEOM

#ifndef _GEOM_InteractiveObject_HeaderFile
#define _GEOM_InteractiveObject_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_GEOM_InteractiveObject_HeaderFile
#include "Handle_GEOM_InteractiveObject.hxx"
#endif

#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif
#ifndef _SALOME_InteractiveObject_HeaderFile
#include "SALOME_InteractiveObject.hxx"
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Handle_SALOME_InteractiveObject_HeaderFile
#include "Handle_SALOME_InteractiveObject.hxx"
#endif
class SALOME_InteractiveObject;


class GEOM_InteractiveObject : public SALOME_InteractiveObject {

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
Standard_EXPORT GEOM_InteractiveObject();
Standard_EXPORT GEOM_InteractiveObject(const char* anIOR,
				       const char* aFatherIOR,
				       const char* aComponentDataType,
				       const char* anEntry = "");
Standard_EXPORT   void setIOR(const char* anEntry) ;
Standard_EXPORT   const char* getIOR() ;
Standard_EXPORT   void setFatherIOR(const char* anEntry) ;
Standard_EXPORT   const char* getFatherIOR() ;
Standard_EXPORT   virtual Standard_Boolean isSame(const Handle(SALOME_InteractiveObject)& anIO) ;
Standard_EXPORT   ~GEOM_InteractiveObject();




 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_InteractiveObject_Type_();
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
std::string myIOR;
std::string myFatherIOR;


};





// other inline functions and methods (like "C++: function call" methods)
//


#endif
