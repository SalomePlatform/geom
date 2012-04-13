// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
//

#ifndef _GEOMAlgo_DataMapNodeOfDataMapOfShapePnt_HeaderFile
#define _GEOMAlgo_DataMapNodeOfDataMapOfShapePnt_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapePnt_HeaderFile
#include <Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapePnt.hxx>
#endif

#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _gp_Pnt_HeaderFile
#include <gp_Pnt.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif

class TopoDS_Shape;
class gp_Pnt;
class TopTools_ShapeMapHasher;
class GEOMAlgo_DataMapOfShapePnt;
class GEOMAlgo_DataMapIteratorOfDataMapOfShapePnt;

class GEOMAlgo_DataMapNodeOfDataMapOfShapePnt : public TCollection_MapNode
{
public:
  GEOMAlgo_DataMapNodeOfDataMapOfShapePnt
  (const TopoDS_Shape& K, const gp_Pnt& I, const TCollection_MapNodePtr& n);

  TopoDS_Shape& Key() const;
  gp_Pnt& Value() const;

  DEFINE_STANDARD_RTTI(GEOMAlgo_DataMapNodeOfDataMapOfShapePnt)

private: 
  TopoDS_Shape myKey;
  gp_Pnt myValue;
};

#define TheKey TopoDS_Shape
#define TheKey_hxx <TopoDS_Shape.hxx>
#define TheItem gp_Pnt
#define TheItem_hxx <gp_Pnt.hxx>
#define Hasher TopTools_ShapeMapHasher
#define Hasher_hxx <TopTools_ShapeMapHasher.hxx>
#define TCollection_DataMapNode GEOMAlgo_DataMapNodeOfDataMapOfShapePnt
#define TCollection_DataMapNode_hxx <GEOMAlgo_DataMapNodeOfDataMapOfShapePnt.hxx>
#define TCollection_DataMapIterator GEOMAlgo_DataMapIteratorOfDataMapOfShapePnt
#define TCollection_DataMapIterator_hxx <GEOMAlgo_DataMapIteratorOfDataMapOfShapePnt.hxx>
#define Handle_TCollection_DataMapNode Handle_GEOMAlgo_DataMapNodeOfDataMapOfShapePnt
#define TCollection_DataMapNode_Type_() GEOMAlgo_DataMapNodeOfDataMapOfShapePnt_Type_()
#define TCollection_DataMap GEOMAlgo_DataMapOfShapePnt
#define TCollection_DataMap_hxx <GEOMAlgo_DataMapOfShapePnt.hxx>

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
