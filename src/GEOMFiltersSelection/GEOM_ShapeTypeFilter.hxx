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
//  File   : GEOM_ShapeTypeFilter.hxx
//  Module : GEOM

#ifndef _GEOM_ShapeTypeFilter_HeaderFile
#define _GEOM_ShapeTypeFilter_HeaderFile

#ifndef _Handle_GEOM_ShapeTypeFilter_HeaderFile
#include "Handle_GEOM_ShapeTypeFilter.hxx"
#endif

#include "SALOME_InteractiveObject.hxx"
#include "SALOME_Filter.hxx"
#include "SALOME_TypeFilter.hxx"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

// Open CASCADE Includes
#include <Standard.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_MapOfInteger.hxx>

class GEOM_ShapeTypeFilter : public SALOME_Filter
{

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
Standard_EXPORT GEOM_ShapeTypeFilter( const TopAbs_ShapeEnum theShapeType,
                                      const bool theIsAll = false );
Standard_EXPORT GEOM_ShapeTypeFilter( const TColStd_MapOfInteger& theShapeTypes,
                                      const bool theIsAll = false );
Standard_EXPORT ~GEOM_ShapeTypeFilter();

Standard_EXPORT virtual  Standard_Boolean IsOk(const Handle(SALOME_InteractiveObject)& anobj) const;


 // Type management
 //
 Standard_EXPORT friend Handle_Standard_Type& GEOM_ShapeTypeFilter_Type_();
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)&) const;

protected:

 // Methods PROTECTED
 //
 Standard_EXPORT virtual Standard_Boolean IsShapeOk(const TopoDS_Shape& theShape ) const;


 // Fields PROTECTED
 //
TColStd_MapOfInteger myShapeTypes;
Handle(SALOME_TypeFilter) myTypeFilter;

private: 

 bool myIsAll;


};





// other inline functions and methods (like "C++: function call" methods)
//


#endif
