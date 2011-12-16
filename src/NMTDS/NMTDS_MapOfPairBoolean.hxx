// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com

#ifndef _NMTDS_MapOfPairBoolean_HeaderFile
#define _NMTDS_MapOfPairBoolean_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TCollection_BasicMap_HeaderFile
#include <TCollection_BasicMap.hxx>
#endif
#ifndef _Handle_NMTDS_StdMapNodeOfMapOfPairBoolean_HeaderFile
#include <Handle_NMTDS_StdMapNodeOfMapOfPairBoolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class Standard_DomainError;
class NMTDS_PairBoolean;
class NMTDS_PairMapHasher;
class NMTDS_StdMapNodeOfMapOfPairBoolean;
class NMTDS_MapIteratorOfMapOfPairBoolean;



class NMTDS_MapOfPairBoolean  : public TCollection_BasicMap {
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

  
  Standard_EXPORT   NMTDS_MapOfPairBoolean(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT     NMTDS_MapOfPairBoolean& Assign(const NMTDS_MapOfPairBoolean& Other) ;
    NMTDS_MapOfPairBoolean& operator =(const NMTDS_MapOfPairBoolean& Other) 
{
  return Assign(Other);
}
  
  Standard_EXPORT     void ReSize(const Standard_Integer NbBuckets) ;
  
  Standard_EXPORT     void Clear() ;
~NMTDS_MapOfPairBoolean()
{
  Clear();
}
  
  Standard_EXPORT     Standard_Boolean Add(const NMTDS_PairBoolean& aKey) ;
  
  Standard_EXPORT     Standard_Boolean Contains(const NMTDS_PairBoolean& aKey) const;
  
  Standard_EXPORT     Standard_Boolean Remove(const NMTDS_PairBoolean& aKey) ;





protected:





private:

  
  Standard_EXPORT   NMTDS_MapOfPairBoolean(const NMTDS_MapOfPairBoolean& Other);




};





// other Inline functions and methods (like "C++: function call" methods)


#endif
