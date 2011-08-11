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

#ifndef _GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape_HeaderFile
#define _GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape_HeaderFile
#include <Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape.hxx>
#endif

#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _TopTools_MapOfShape_HeaderFile
#include <TopTools_MapOfShape.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif

class TopoDS_Shape;
class TopTools_MapOfShape;
class TopTools_ShapeMapHasher;
class GEOMAlgo_DataMapOfShapeMapOfShape;
class GEOMAlgo_DataMapIteratorOfDataMapOfShapeMapOfShape;

class GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape : public TCollection_MapNode {

public:
  GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape
  (const TopoDS_Shape& K, const TopTools_MapOfShape& I, const TCollection_MapNodePtr& n);

  TopoDS_Shape& Key() const;
  TopTools_MapOfShape& Value() const;

  DEFINE_STANDARD_RTTI(GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape)

private:
  TopoDS_Shape myKey;
  TopTools_MapOfShape myValue;
};

#define TheKey TopoDS_Shape
#define TheKey_hxx <TopoDS_Shape.hxx>
#define TheItem TopTools_MapOfShape
#define TheItem_hxx <TopTools_MapOfShape.hxx>
#define Hasher TopTools_ShapeMapHasher
#define Hasher_hxx <TopTools_ShapeMapHasher.hxx>
#define TCollection_DataMapNode GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape
#define TCollection_DataMapNode_hxx <GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape.hxx>
#define TCollection_DataMapIterator GEOMAlgo_DataMapIteratorOfDataMapOfShapeMapOfShape
#define TCollection_DataMapIterator_hxx <GEOMAlgo_DataMapIteratorOfDataMapOfShapeMapOfShape.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape
#define TCollection_DataMapNode_Type_() GEOMAlgo_DataMapNodeOfDataMapOfShapeMapOfShape_Type_()
#define TCollection_DataMap GEOMAlgo_DataMapOfShapeMapOfShape
#define TCollection_DataMap_hxx <GEOMAlgo_DataMapOfShapeMapOfShape.hxx>

#include <TCollection_DataMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef TheItem
#undef TheItem_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_DataMapNode
#undef TCollection_DataMapNode_hxx
#undef TCollection_DataMapIterator
#undef TCollection_DataMapIterator_hxx
#undef Handle_TCollection_DataMapNode
#undef TCollection_DataMapNode_Type_
#undef TCollection_DataMap
#undef TCollection_DataMap_hxx

#endif
