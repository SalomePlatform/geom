// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo_HeaderFile
#define _Handle_GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_TCollection_MapNode_HeaderFile
#include <Handle_TCollection_MapNode.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TCollection_MapNode);
class GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo);

class Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo) : public Handle(TCollection_MapNode) {
  public:
    Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)():Handle(TCollection_MapNode)() {} 
    Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)(const Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)& aHandle) : Handle(TCollection_MapNode)(aHandle) 
     {
     }

    Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)(const GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo* anItem) : Handle(TCollection_MapNode)((TCollection_MapNode *)anItem) 
     {
     }

    Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)& operator=(const Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)& operator=(const GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo* operator->() const
     {
      return (GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo *)ControlAccess();
     }

//   Standard_EXPORT ~Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo)();
 
   Standard_EXPORT static const Handle(GEOMAlgo_IndexedDataMapNodeOfIndexedDataMapOfShapeShapeInfo) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
