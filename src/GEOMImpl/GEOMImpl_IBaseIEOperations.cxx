// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// internal includes
#include "GEOMImpl_IBaseIEOperations.hxx"
#include "GEOMImpl_IGroupOperations.hxx"
#include "GEOMImpl_IFieldOperations.hxx"
#include "GEOMImpl_IShapesOperations.hxx"

// OCC includes
#include <NCollection_DataMap.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Comment.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

typedef NCollection_DataMap< TCollection_ExtendedString, NCollection_List<TopoDS_Shape> >
        DataMapOfStringListOfShape;

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
GEOMImpl_IBaseIEOperations::GEOMImpl_IBaseIEOperations(GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  myGroupOperations = new GEOMImpl_IGroupOperations( GetEngine(), GetDocID() );
  myFieldOperations = new GEOMImpl_IFieldOperations( GetEngine(), GetDocID() );
  myShapesOperations = new GEOMImpl_IShapesOperations( GetEngine(), GetDocID() );
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IBaseIEOperations::~GEOMImpl_IBaseIEOperations()
{
  delete myGroupOperations;
  delete myFieldOperations;
  delete myShapesOperations;
}

//=============================================================================
/*!
 *  This method creates material groups for an imported object.
 *  \param theObject the imported object.
 */
//=============================================================================
void GEOMImpl_IBaseIEOperations::MakeMaterialGroups
                        (const Handle(GEOM_Object) &theObject,
                         const Handle(TColStd_HSequenceOfTransient) &theSeq)
{
  TopoDS_Shape aResShape = theObject->GetValue();

  if (aResShape.IsNull() == Standard_False) {
    // Group shapes by material names.
    Handle(GEOM_Function)      aFunction = theObject->GetLastFunction();
    DataMapOfStringListOfShape aMapMaterialShapes;

    // check all named shapes using iterator
    TDF_ChildIDIterator anIt (aFunction->GetNamingEntry(),
        TNaming_NamedShape::GetID(), Standard_True);

    for (; anIt.More(); anIt.Next()) {
      Handle(TNaming_NamedShape) anAttr =
          Handle(TNaming_NamedShape)::DownCast(anIt.Value());

      if (anAttr.IsNull() == Standard_False) {
        TDF_Label                aLabel = anAttr->Label();
        Handle(TDataStd_Comment) aComment;

        if (aLabel.FindAttribute(TDataStd_Comment::GetID(), aComment)) {
          TCollection_ExtendedString aMatName = aComment->Get();
          TopoDS_Shape               aShape   = anAttr->Get();

          if (aMapMaterialShapes.IsBound(aMatName) == Standard_False) {
            NCollection_List<TopoDS_Shape> anEmptyList;

            aMapMaterialShapes.Bind(aMatName, anEmptyList);
          }

          aMapMaterialShapes(aMatName).Append(aShape);
        }
      }
    }

    if (aMapMaterialShapes.IsEmpty() == Standard_False) {
      // Construct groups.
      TopAbs_ShapeEnum aType = aResShape.ShapeType();
      Standard_Integer i;
      DataMapOfStringListOfShape::Iterator aMapIter;

      // Check each shape type.
      for(i = aType; i <= TopAbs_VERTEX; i++) {
        DataMapOfStringListOfShape::Iterator aMapIter(aMapMaterialShapes);

        for (; aMapIter.More(); aMapIter.Next()) {
          NCollection_List<TopoDS_Shape> &aShList = aMapIter.ChangeValue();
          NCollection_List<TopoDS_Shape>::Iterator aShIter(aShList);
          NCollection_List<TopoDS_Shape>  aShListSameType;

          while (aShIter.More()) {
            const TopoDS_Shape &aShape = aShIter.Value();

            if (i == aShape.ShapeType()) {
              // Treat this element.
              aShListSameType.Append(aShape);
              aShList.Remove(aShIter);
            } else {
              // Go to the next element.
              aShIter.Next();
            }
          }

          if (aShListSameType.IsEmpty() == Standard_False) {
            // Construct a group.
            Handle(GEOM_Object) aGroup =
              MakeGroup(theObject, aMapIter.Key(), aShListSameType);

            if (aGroup.IsNull() == Standard_False) {
              theSeq->Append(aGroup);
            }
          }
        }
      }
    }
  }
}


//=============================================================================
/*!
 *  This method creates a group of shapes of certain type.
 *  \param theObject the imported object.
 *  \param theName the material name.
 *  \param theShapes the list of shapes to be added to this group.
 *  \return the created group.
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IBaseIEOperations::MakeGroup
                  (const Handle(GEOM_Object)            &theObject,
                   const TCollection_ExtendedString     &theName,
                   const NCollection_List<TopoDS_Shape> &theShapes)
{
  Handle(GEOM_Object)                aGroup;
  TopTools_IndexedMapOfShape         anIndices;
  Handle(TColStd_HSequenceOfInteger) aSeqIDs = new TColStd_HSequenceOfInteger;
  NCollection_List<TopoDS_Shape>::Iterator anIter(theShapes);

  TopExp::MapShapes(theObject->GetValue(), anIndices);

  // Compose shape IDs.
  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aShape = anIter.Value();
    const Standard_Integer anIndex = anIndices.FindIndex(aShape);

    if (anIndex > 0) {
      aSeqIDs->Append(anIndex);
    }
  }

  if (aSeqIDs->IsEmpty() == Standard_False) {
    // Create a group.
    const TopAbs_ShapeEnum aType  = theShapes.First().ShapeType();

    aGroup = myGroupOperations->CreateGroup(theObject, aType);

    if (aGroup.IsNull() == Standard_False) {
      aGroup->GetLastFunction()->SetDescription("");
      myGroupOperations->UnionIDs(aGroup, aSeqIDs);
      aGroup->GetLastFunction()->SetDescription("");

      // Compose the group name.
      TCollection_AsciiString aGroupName(theName);

      switch(aType) {
        case TopAbs_VERTEX:
          aGroupName += "_VERTEX";
          break;
        case TopAbs_EDGE:
          aGroupName += "_EDGE";
          break;
        case TopAbs_WIRE:
          aGroupName += "_WIRE";
          break;
        case TopAbs_FACE:
          aGroupName += "_FACE";
          break;
        case TopAbs_SHELL:
          aGroupName += "_SHELL";
          break;
        case TopAbs_SOLID:
          aGroupName += "_SOLID";
          break;
        case TopAbs_COMPSOLID:
          aGroupName += "_COMPSOLID";
          break;
        case TopAbs_COMPOUND:
          aGroupName += "_COMPOUND";
          break;
        default:
          aGroupName += "_SHAPE";
          break;
      }

      aGroup->SetName(aGroupName.ToCString());
    }
  }

  return aGroup;
}
