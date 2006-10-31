// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef _NMTDS_PassKey_HeaderFile
#define _NMTDS_PassKey_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Address_HeaderFile
#include <Standard_Address.hxx>
#endif


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif


class NMTDS_PassKey  {

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


Standard_EXPORT NMTDS_PassKey();


Standard_EXPORT   NMTDS_PassKey& Assign(const NMTDS_PassKey& Other) ;
  NMTDS_PassKey& operator =(const NMTDS_PassKey& Other) 
{
  return Assign(Other);
}



Standard_EXPORT   void SetIds(const Standard_Integer aI1,const Standard_Integer aI2) ;


Standard_EXPORT   Standard_Integer NbMax() const;


Standard_EXPORT   void Clear() ;


Standard_EXPORT   void Compute() ;


Standard_EXPORT   Standard_Boolean IsEqual(const NMTDS_PassKey& aOther) const;


Standard_EXPORT   Standard_Address Key() const;


Standard_EXPORT   Standard_Integer HashCode(const Standard_Integer Upper) const;


Standard_EXPORT   void Ids(Standard_Integer& aI1,Standard_Integer& aI2) const;


Standard_EXPORT   void Dump() const;





protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //
Standard_Integer myNbIds;
Standard_Integer myNbMax;
Standard_Integer mySum;
Standard_Integer myIds[2];


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
