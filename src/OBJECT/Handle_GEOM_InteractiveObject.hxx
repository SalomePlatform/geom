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
//  File   : Handle_GEOM_InteractiveObject.hxx
//  Module : GEOM

#ifndef _Handle_GEOM_InteractiveObject_HeaderFile
#define _Handle_GEOM_InteractiveObject_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_SALOME_InteractiveObject_HeaderFile
#include "Handle_SALOME_InteractiveObject.hxx"
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(SALOME_InteractiveObject);
class GEOM_InteractiveObject;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_InteractiveObject);

class Handle(GEOM_InteractiveObject) : public Handle(SALOME_InteractiveObject) {
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
    Handle(GEOM_InteractiveObject)():Handle(SALOME_InteractiveObject)() {} 
    Handle(GEOM_InteractiveObject)(const Handle(GEOM_InteractiveObject)& aHandle) : Handle(SALOME_InteractiveObject)(aHandle) 
     {
     }

    Handle(GEOM_InteractiveObject)(const GEOM_InteractiveObject* anItem) : Handle(SALOME_InteractiveObject)((SALOME_InteractiveObject *)anItem) 
     {
     }

    Handle(GEOM_InteractiveObject)& operator=(const Handle(GEOM_InteractiveObject)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_InteractiveObject)& operator=(const GEOM_InteractiveObject* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_InteractiveObject* operator->() 
     {
      return (GEOM_InteractiveObject *)ControlAccess();
     }

    GEOM_InteractiveObject* operator->() const 
     {
      return (GEOM_InteractiveObject *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_InteractiveObject)();
 
   Standard_EXPORT static const Handle(GEOM_InteractiveObject) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
