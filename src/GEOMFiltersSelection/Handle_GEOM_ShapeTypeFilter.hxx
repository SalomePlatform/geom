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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : Handle_GEOM_ShapeTypeFilter.hxx
//  Module : GEOM

#ifndef _Handle_GEOM_ShapeTypeFilter_HeaderFile
#define _Handle_GEOM_ShapeTypeFilter_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SALOME_Filter_HeaderFile
#include "Handle_SALOME_Filter.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SALOME_Filter);
class GEOM_ShapeTypeFilter;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_ShapeTypeFilter);

class Handle(GEOM_ShapeTypeFilter) : public Handle(SALOME_Filter) {
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
    Handle(GEOM_ShapeTypeFilter)():Handle(SALOME_Filter)() {} 
    Handle(GEOM_ShapeTypeFilter)(const Handle(GEOM_ShapeTypeFilter)& aHandle) : Handle(SALOME_Filter)(aHandle) 
     {
     }

    Handle(GEOM_ShapeTypeFilter)(const GEOM_ShapeTypeFilter* anItem) : Handle(SALOME_Filter)((SALOME_Filter *)anItem) 
     {
     }

    Handle(GEOM_ShapeTypeFilter)& operator=(const Handle(GEOM_ShapeTypeFilter)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_ShapeTypeFilter)& operator=(const GEOM_ShapeTypeFilter* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_ShapeTypeFilter* operator->() 
     {
      return (GEOM_ShapeTypeFilter *)ControlAccess();
     }

    GEOM_ShapeTypeFilter* operator->() const 
     {
      return (GEOM_ShapeTypeFilter *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_ShapeTypeFilter)();
 
   Standard_EXPORT static const Handle(GEOM_ShapeTypeFilter) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
