//  GEOM GEOMFiltersSelection : filter selector for the viewer
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
//  File   : Handle_GEOM_EdgeFilter.hxx
//  Module : GEOM

#ifndef _Handle_GEOM_EdgeFilter_HeaderFile
#define _Handle_GEOM_EdgeFilter_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_GEOM_ShapeTypeFilter_HeaderFile
#include "Handle_GEOM_ShapeTypeFilter.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(GEOM_ShapeTypeFilter);
class GEOM_EdgeFilter;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_EdgeFilter);

class Handle(GEOM_EdgeFilter) : public Handle(GEOM_ShapeTypeFilter) {
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
    Handle(GEOM_EdgeFilter)():Handle(GEOM_ShapeTypeFilter)() {} 
    Handle(GEOM_EdgeFilter)(const Handle(GEOM_EdgeFilter)& aHandle) : Handle(GEOM_ShapeTypeFilter)(aHandle) 
     {
     }

    Handle(GEOM_EdgeFilter)(const GEOM_EdgeFilter* anItem) : Handle(GEOM_ShapeTypeFilter)((GEOM_ShapeTypeFilter *)anItem) 
     {
     }

    Handle(GEOM_EdgeFilter)& operator=(const Handle(GEOM_EdgeFilter)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_EdgeFilter)& operator=(const GEOM_EdgeFilter* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_EdgeFilter* operator->() 
     {
      return (GEOM_EdgeFilter *)ControlAccess();
     }

    GEOM_EdgeFilter* operator->() const 
     {
      return (GEOM_EdgeFilter *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_EdgeFilter)();
 
   Standard_EXPORT static const Handle(GEOM_EdgeFilter) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
