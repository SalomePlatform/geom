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
//  File   : GEOMDS_Explorer.hxx
//  Module : GEOM

#ifndef _GEOMDS_Explorer_HeaderFile
#define _GEOMDS_Explorer_HeaderFile

#ifndef _TDF_ChildIterator_HeaderFile
#include <TDF_ChildIterator.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
class TDF_Label;
class TopoDS_Shape;
class TCollection_ExtendedString;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class GEOMDS_Explorer  {

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
Standard_EXPORT GEOMDS_Explorer(const TDF_Label& Main);
Standard_EXPORT Standard_Boolean More() ;
Standard_EXPORT void Next() ;
Standard_EXPORT TopoDS_Shape Shape() const;
Standard_EXPORT TCollection_ExtendedString Name() const;

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
TDF_ChildIterator myChildIterator;


};


// other inline functions and methods (like "C++: function call" methods)
//

#endif
