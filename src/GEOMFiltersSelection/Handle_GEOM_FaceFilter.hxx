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
//  File   : Handle_GEOM_FaceFilter.hxx
//  Module : GEOM

#ifndef _Handle_GEOM_FaceFilter_HeaderFile
#define _Handle_GEOM_FaceFilter_HeaderFile

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
class GEOM_FaceFilter;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_FaceFilter);

class Handle(GEOM_FaceFilter) : public Handle(GEOM_ShapeTypeFilter) {
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
    Handle(GEOM_FaceFilter)():Handle(GEOM_ShapeTypeFilter)() {} 
    Handle(GEOM_FaceFilter)(const Handle(GEOM_FaceFilter)& aHandle) : Handle(GEOM_ShapeTypeFilter)(aHandle) 
     {
     }

    Handle(GEOM_FaceFilter)(const GEOM_FaceFilter* anItem) : Handle(GEOM_ShapeTypeFilter)((GEOM_ShapeTypeFilter *)anItem) 
     {
     }

    Handle(GEOM_FaceFilter)& operator=(const Handle(GEOM_FaceFilter)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_FaceFilter)& operator=(const GEOM_FaceFilter* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_FaceFilter* operator->() 
     {
      return (GEOM_FaceFilter *)ControlAccess();
     }

    GEOM_FaceFilter* operator->() const 
     {
      return (GEOM_FaceFilter *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_FaceFilter)();
 
   Standard_EXPORT static const Handle(GEOM_FaceFilter) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
