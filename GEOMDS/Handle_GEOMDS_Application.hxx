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
//  File   : Handle_GEOMDS_Application.hxx
//  Module : GEOM

#ifndef _Handle_GEOMDS_Application_HeaderFile
#define _Handle_GEOMDS_Application_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_TDocStd_Application_HeaderFile
#include <Handle_TDocStd_Application.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TDocStd_Application);
class GEOMDS_Application;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(SimpleOCAF_Application);

class Handle(GEOMDS_Application) : public Handle(TDocStd_Application) {
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
    Handle(GEOMDS_Application)():Handle(TDocStd_Application)() {} 
    Handle(GEOMDS_Application)(const Handle(GEOMDS_Application)& aHandle) : Handle(TDocStd_Application)(aHandle) 
     {
     }

    Handle(GEOMDS_Application)(const GEOMDS_Application* anItem) : Handle(TDocStd_Application)((TDocStd_Application *)anItem) 
     {
     }

    Handle(GEOMDS_Application)& operator=(const Handle(GEOMDS_Application)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOMDS_Application)& operator=(const GEOMDS_Application* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOMDS_Application* operator->() 
     {
      return (GEOMDS_Application *)ControlAccess();
     }

    GEOMDS_Application* operator->() const 
     {
      return (GEOMDS_Application *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOMDS_Application)();
 
   Standard_EXPORT static const Handle(GEOMDS_Application) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
