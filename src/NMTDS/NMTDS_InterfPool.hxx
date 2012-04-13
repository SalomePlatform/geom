// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
//

#ifndef _NMTDS_InterfPool_HeaderFile
#define _NMTDS_InterfPool_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _NMTDS_MapOfPairBoolean_HeaderFile
#include <NMTDS_MapOfPairBoolean.hxx>
#endif
#ifndef _NMTDS_ListOfPairBoolean_HeaderFile
#include <NMTDS_ListOfPairBoolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _BOPTools_CArray1OfSSInterference_HeaderFile
#include <BOPTools_CArray1OfSSInterference.hxx>
#endif
#ifndef _BOPTools_CArray1OfESInterference_HeaderFile
#include <BOPTools_CArray1OfESInterference.hxx>
#endif
#ifndef _BOPTools_CArray1OfVSInterference_HeaderFile
#include <BOPTools_CArray1OfVSInterference.hxx>
#endif
#ifndef _BOPTools_CArray1OfEEInterference_HeaderFile
#include <BOPTools_CArray1OfEEInterference.hxx>
#endif
#ifndef _BOPTools_CArray1OfVEInterference_HeaderFile
#include <BOPTools_CArray1OfVEInterference.hxx>
#endif
#ifndef _BOPTools_CArray1OfVVInterference_HeaderFile
#include <BOPTools_CArray1OfVVInterference.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _NMTDS_InterfType_HeaderFile
#include <NMTDS_InterfType.hxx>
#endif
class NMTDS_PairBoolean;
class NMTDS_ListOfPairBoolean;
class BOPTools_CArray1OfSSInterference;
class BOPTools_CArray1OfESInterference;
class BOPTools_CArray1OfVSInterference;
class BOPTools_CArray1OfEEInterference;
class BOPTools_CArray1OfVEInterference;
class BOPTools_CArray1OfVVInterference;



class NMTDS_InterfPool  {
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

  
  Standard_EXPORT   NMTDS_InterfPool();
Standard_EXPORT virtual ~NMTDS_InterfPool();
  
  Standard_EXPORT     Standard_Boolean Add(const NMTDS_PairBoolean& aPKB,const NMTDS_InterfType aType) ;
  
  Standard_EXPORT     Standard_Boolean Add(const Standard_Integer aInd1,const Standard_Integer aInd2,const NMTDS_InterfType aType) ;
  
  Standard_EXPORT     Standard_Boolean Add(const Standard_Integer aInd1,const Standard_Integer aInd2,const Standard_Boolean bFlag,const NMTDS_InterfType aType) ;
  
  Standard_EXPORT     Standard_Boolean Contains(const NMTDS_PairBoolean& aPKB) const;
  
  Standard_EXPORT     Standard_Boolean Contains(const Standard_Integer aInd1,const Standard_Integer aInd2) const;
  
  Standard_EXPORT    const NMTDS_ListOfPairBoolean& Get() const;
  
  Standard_EXPORT    const NMTDS_ListOfPairBoolean& Get(const NMTDS_InterfType aType) const;
  
  Standard_EXPORT    const NMTDS_ListOfPairBoolean& Get(const Standard_Integer aInd) const;
  
  Standard_EXPORT    const NMTDS_ListOfPairBoolean& Get(const Standard_Integer aInd,const NMTDS_InterfType aType) const;
  
//! Returns the reference to array Of F/F interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfSSInterference& SSInterferences() ;
  
//! Returns the reference to array Of E/F interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfESInterference& ESInterferences() ;
  
//! Returns the reference to array Of V/F interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfVSInterference& VSInterferences() ;
  
//! Returns the reference to arrray Of E/E interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfEEInterference& EEInterferences() ;
  
//! Returns the reference to arrray Of  V/E interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfVEInterference& VEInterferences() ;
  
//! Returns the reference to arrray Of  V/V interferences <br>
  Standard_EXPORT     BOPTools_CArray1OfVVInterference& VVInterferences() ;
  
  Standard_EXPORT     void Purge() ;





protected:



NMTDS_MapOfPairBoolean myTable[6];
NMTDS_ListOfPairBoolean myList;
Standard_Integer myMaxInd;
BOPTools_CArray1OfSSInterference mySSInterferences;
BOPTools_CArray1OfESInterference myESInterferences;
BOPTools_CArray1OfVSInterference myVSInterferences;
BOPTools_CArray1OfEEInterference myEEInterferences;
BOPTools_CArray1OfVEInterference myVEInterferences;
BOPTools_CArray1OfVVInterference myVVInterferences;


private:





};





// other Inline functions and methods (like "C++: function call" methods)


#endif
