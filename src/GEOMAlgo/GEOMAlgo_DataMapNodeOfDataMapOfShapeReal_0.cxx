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

#include <GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx>

#include <CASCatch_OCCTVersion.hxx>

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _TopTools_ShapeMapHasher_HeaderFile
#include <TopTools_ShapeMapHasher.hxx>
#endif
#ifndef _GEOMAlgo_DataMapOfShapeReal_HeaderFile
#include <GEOMAlgo_DataMapOfShapeReal.hxx>
#endif
#ifndef _GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal_HeaderFile
#include <GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal.hxx>
#endif

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1

IMPLEMENT_STANDARD_TYPE(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(TCollection_MapNode),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)

IMPLEMENT_DOWNCAST(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)

#else

Standard_EXPORT Handle_Standard_Type& GEOMAlgo_DataMapNodeOfDataMapOfShapeReal_Type_()
{
  static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMAlgo_DataMapNodeOfDataMapOfShapeReal",
                                                         sizeof(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);

  return _aType;
}

// DownCast method
//   allow safe downcasting
//
const Handle(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal) Handle(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal))) {
       _anOtherObject = Handle(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)((Handle(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal)&)AnObject);
     }
  }

  return _anOtherObject ;
}

const Handle(Standard_Type)& GEOMAlgo_DataMapNodeOfDataMapOfShapeReal::DynamicType() const 
{ 
  return STANDARD_TYPE(GEOMAlgo_DataMapNodeOfDataMapOfShapeReal) ; 
}

#endif

#define TheKey TopoDS_Shape
#define TheKey_hxx <TopoDS_Shape.hxx>
#define TheItem Standard_Real
#define TheItem_hxx <Standard_Real.hxx>
#define Hasher TopTools_ShapeMapHasher
#define Hasher_hxx <TopTools_ShapeMapHasher.hxx>
#define TCollection_DataMapNode GEOMAlgo_DataMapNodeOfDataMapOfShapeReal
#define TCollection_DataMapNode_hxx <GEOMAlgo_DataMapNodeOfDataMapOfShapeReal.hxx>
#define TCollection_DataMapIterator GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal
#define TCollection_DataMapIterator_hxx <GEOMAlgo_DataMapIteratorOfDataMapOfShapeReal.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeReal
#define TCollection_DataMapNode_Type_() GEOMAlgo_DataMapNodeOfDataMapOfShapeReal_Type_()
#define TCollection_DataMap GEOMAlgo_DataMapOfShapeReal
#define TCollection_DataMap_hxx <GEOMAlgo_DataMapOfShapeReal.hxx>
#include <TCollection_DataMapNode.gxx>

