//  GEOM GEOMFiltersSelection : filter selector for the viewer
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  File   : GEOM_PreviewFilter.hxx
//  Module : GEOM

#ifndef _GEOM_PreviewFilter_HeaderFile
#define _GEOM_PreviewFilter_HeaderFile

#ifndef _Handle_GEOM_PreviewFilter_HeaderFile
#include "Handle_GEOM_PreviewFilter.hxx"
#endif

#include "SALOME_InteractiveObject.hxx"
#include "SALOME_Filter.hxx"

// IDL Headers


// Open CASCADE Includes
#include <Standard.hxx>

class GEOM_PreviewFilter : public SALOME_Filter {

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
 // Methods PUBLIC
 // 
Standard_EXPORT GEOM_PreviewFilter();
Standard_EXPORT ~GEOM_PreviewFilter();

Standard_EXPORT virtual  Standard_Boolean IsOk(const Handle(SALOME_InteractiveObject)& anobj) const;


 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_PreviewFilter_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)&) const;

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


};





// other inline functions and methods (like "C++: function call" methods)
//


#endif
