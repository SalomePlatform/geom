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

#ifndef _GEOMAlgo_DataMapOfShapeMapOfShape_HeaderFile
#define _GEOMAlgo_DataMapOfShapeMapOfShape_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _TCollection_BasicMap_HeaderFile
#include <TCollection_BasicMap.hxx>
#endif
#ifndef _Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape_HeaderFile
#include <Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif

class Standard_DomainError;
class Standard_NoSuchObject;
class TopoDS_Shape;
class TopTools_MapOfShape;
class TopTools_ShapeMapHasher;
class GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape;
class GEOMAlgo_DataMapIteratorOfDataMapOfShapeMapOfShape;

#include <Basics_OCCTVersion.hxx>

class GEOMAlgo_DataMapOfShapeMapOfShape  : public TCollection_BasicMap {
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

  
  Standard_EXPORT   GEOMAlgo_DataMapOfShapeMapOfShape(const Standard_Integer NbBuckets = 1);
  
  Standard_EXPORT     GEOMAlgo_DataMapOfShapeMapOfShape& Assign(const GEOMAlgo_DataMapOfShapeMapOfShape& Other) ;
    GEOMAlgo_DataMapOfShapeMapOfShape& operator =(const GEOMAlgo_DataMapOfShapeMapOfShape& Other) 
{
  return Assign(Other);
}
  
  Standard_EXPORT     void ReSize(const Standard_Integer NbBuckets) ;
  
  Standard_EXPORT     void Clear() ;
~GEOMAlgo_DataMapOfShapeMapOfShape()
{
  Clear();
}
  
  Standard_EXPORT     Standard_Boolean Bind(const TopoDS_Shape& K,const TopTools_MapOfShape& I) ;
  
  Standard_EXPORT     Standard_Boolean IsBound(const TopoDS_Shape& K) const;
  
  Standard_EXPORT     Standard_Boolean UnBind(const TopoDS_Shape& K) ;
  
  Standard_EXPORT    const TopTools_MapOfShape& Find(const TopoDS_Shape& K) const;
   const TopTools_MapOfShape& operator()(const TopoDS_Shape& K) const
{
  return Find(K);
}
  
  Standard_EXPORT     TopTools_MapOfShape& ChangeFind(const TopoDS_Shape& K) ;
    TopTools_MapOfShape& operator()(const TopoDS_Shape& K) 
{
  return ChangeFind(K);
}

#if OCC_VERSION_LARGE > 0x06050100 // for OCC-6.5.2 and higher version
  Standard_EXPORT Standard_Address Find1 (const TopoDS_Shape& K) const;
  Standard_EXPORT Standard_Address ChangeFind1 (const TopoDS_Shape& K);
#endif

private:
  
  Standard_EXPORT   GEOMAlgo_DataMapOfShapeMapOfShape(const GEOMAlgo_DataMapOfShapeMapOfShape& Other);

};

// other Inline functions and methods (like "C++: function call" methods)

#endif
