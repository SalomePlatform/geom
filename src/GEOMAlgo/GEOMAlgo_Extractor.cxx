// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:        GEOMAlgo_Extractor.cxx
// Created:
// Author:      Sergey KHROMOV
//


#include <GEOMAlgo_Extractor.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>


//=======================================================================
//function : GEOMAlgo_Extractor
//purpose  :
//=======================================================================
GEOMAlgo_Extractor::GEOMAlgo_Extractor()
{
}

//=======================================================================
//function : ~GEOMAlgo_Extractor
//purpose  :
//=======================================================================
GEOMAlgo_Extractor::~GEOMAlgo_Extractor()
{
}

//=======================================================================
//function : SetShape
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::SetShape(const TopoDS_Shape &theShape)
{
  myShape = theShape;
  myMapShapeAnc.Clear();
  clear();
}

//=======================================================================
//function : SetShapesToRemove
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::SetShapesToRemove
                        (const TopTools_ListOfShape &theSubShapes)
{
  mySubShapes.Assign(theSubShapes);
  clear();
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::Perform()
{
  clear();
  myErrorStatus = 0;
  //
  checkData();

  if(myErrorStatus) {
    return;
  }

  if (myWarningStatus == 10) {
    // The result is the same shape. Nothing is modified.
    myResult = myShape;
    return;
  }

  // Mark sub-shapes as removed and modified.
  markShapes();

  // Process Edges.
  processShapes(TopAbs_EDGE);

  // Process Wires.
  processShapes(TopAbs_WIRE);

  // Process Faces.
  processShapes(TopAbs_FACE);

  // Process Shells.
  processShapes(TopAbs_SHELL);

  // Process Solids.
  processShapes(TopAbs_SOLID);

  // Process Comp-Solids.
  processShapes(TopAbs_COMPSOLID);

  // Process Compounds.
  processShapes(TopAbs_COMPOUND);

  // Make the result.
  myResult = makeResult(myShape);

  TopTools_MapOfShape aMapFence;

  makeHistory(myShape, aMapFence);
}

//=======================================================================
//function : GetResult
//purpose  :
//=======================================================================
const TopoDS_Shape &GEOMAlgo_Extractor::GetResult() const
{
  return myResult;
}

//=======================================================================
//function : clear
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::clear()
{
  myErrorStatus   = 1;
  myWarningStatus = 0;
  myResult.Nullify();
  myRemoved.Clear();
  myModified.Clear();
  myNew.Clear();
  myMapRemoved.Clear();
  myMapModified.Clear();
  myMapNewShapeAnc.Clear();
}

//=======================================================================
//function : checkData
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::checkData()
{
  if (myShape.IsNull()) {
    myErrorStatus = 10;
    return;
  }

  if (mySubShapes.IsEmpty()) {
    myWarningStatus = 10;
    return;
  }

  TopTools_ListIteratorOfListOfShape anIter(mySubShapes);
  TopTools_IndexedMapOfShape         anIndices;
  TopTools_MapOfShape                aMapFence;

  TopExp::MapShapes(myShape, anIndices);

  while (anIter.More()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (aMapFence.Add(aSubShape)) {
      // Check if it is a sub-shape of the given shape.
      if (!anIndices.Contains(aSubShape)) {
        myErrorStatus = 11;
        return;
      }

      // Check if it is a main shape.
      if (aSubShape.IsSame(myShape)) {
        myErrorStatus = 12;
        return;
      }

      anIter.Next();
    } else {
      // Remove duplicated index.
      mySubShapes.Remove(anIter);
    }
  }

  if (myMapShapeAnc.IsEmpty()) {
    // Fill the map of shapes - ancestors.
    makeMapShapeAncestors(myShape);
  }

  // Check if there are seam or degenerated edges on faces.
  for (anIter.Initialize(mySubShapes); anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (aSubShape.ShapeType() == TopAbs_EDGE) {
      // Get the list of ancestor wires.
      TopTools_ListOfShape               anAncWires;
      TopTools_ListIteratorOfListOfShape anAncIt;

      if (myMapShapeAnc.IsBound(aSubShape)) {
        anAncIt.Initialize(myMapShapeAnc.Find(aSubShape));

        for (; anAncIt.More(); anAncIt.Next()) {
          const TopoDS_Shape &anAncShape = anAncIt.Value();

          if (anAncShape.ShapeType() == TopAbs_WIRE) {
            anAncWires.Append(anAncShape);
          }
        }
      }

      if (!anAncWires.IsEmpty()) {
        // Check the ancestor faces.
        Standard_Boolean hasFaces = Standard_False;
        TopoDS_Edge      anEdge   = TopoDS::Edge(aSubShape);

        for (anAncIt.Initialize(anAncWires); anAncIt.More(); anAncIt.Next()) {
          const TopoDS_Shape &anAncShape = anAncIt.Value();

          if (anAncShape.ShapeType() == TopAbs_FACE) {
            TopoDS_Face aFace = TopoDS::Face(anAncShape);

            if (BRepTools::IsReallyClosed(anEdge, aFace)) {
              // Deletion of face's seam edge is not allowed
              myErrorStatus = 13;
              return;
            }

            hasFaces = Standard_True;
          }
        }

        if (hasFaces && BRep_Tool::Degenerated(anEdge)) {
          myErrorStatus = 14;
          return;
        }
      }
    }
  }
}

//=======================================================================
//function : makeMapShapeAncestors
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::makeMapShapeAncestors(const TopoDS_Shape &theShape)
{
  if (theShape.ShapeType() == TopAbs_VERTEX) {
    // Vertex is the lowest type. It has no ancestors.
    return;
  }

  TopoDS_Iterator     anIter(theShape);
  TopTools_MapOfShape aMapFence;

  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (aMapFence.Add(aSubShape)) {
      // Add theShape as an ancestor shape.
      if (!myMapShapeAnc.IsBound(aSubShape)) {
        myMapShapeAnc.Bind(aSubShape, TopTools_ListOfShape());
      }

      myMapShapeAnc.ChangeFind(aSubShape).Append(theShape);

      // Recursively call this method for a sub-shape.
      makeMapShapeAncestors(aSubShape);
    }
  }
}

//=======================================================================
//function : markShapes
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::markShapes()
{
  TopTools_ListIteratorOfListOfShape anIter(mySubShapes);

  // Mark sub-shapes as removed.
  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    markRemoved(aSubShape);
  }

  // Mark undestors of sub-shapes as modified.
  for (anIter.Initialize(mySubShapes); anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    markAncestorsModified(aSubShape);
  }
}

//=======================================================================
//function : markRemoved
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::markRemoved(const TopoDS_Shape &theShape)
{
  if (myMapRemoved.Add(theShape)) {
    // Check sub-shapes.
    TopoDS_Iterator     anIter(theShape);
    TopTools_MapOfShape aMapFence;

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aSubShape = anIter.Value();

      if (aMapFence.Add(aSubShape)) {
        TopTools_ListIteratorOfListOfShape anAncIt
                                (myMapShapeAnc.Find(aSubShape));
        Standard_Boolean                   isToRm = Standard_True;

        for (; anAncIt.More(); anAncIt.Next()) {
          const TopoDS_Shape &anAncShape = anAncIt.Value();

          if (!myMapRemoved.Contains(anAncShape)) {
            isToRm = Standard_False;
            break;
          }
        }

        if (isToRm) {
          // Mark sub-shape as removed.
          markRemoved(aSubShape);
        }
      }
    }
  }
}

//=======================================================================
//function : markAncestorsModified
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::markAncestorsModified(const TopoDS_Shape &theShape)
{
  if (myMapShapeAnc.IsBound(theShape)) {
    TopTools_ListIteratorOfListOfShape anAncIt(myMapShapeAnc.Find(theShape));

    for (; anAncIt.More(); anAncIt.Next()) {
      const TopoDS_Shape &anAncShape = anAncIt.Value();

      if (!myMapRemoved.Contains(anAncShape) &&
          !myMapModified.IsBound(anAncShape)) {
        // Mark anAncShape as modified.
        myMapModified.Bind(anAncShape, TopTools_ListOfShape());

        // Mark its ancestors as modified.
        markAncestorsModified(anAncShape);
      }
    }
  }
}

//=======================================================================
//function : processShapes
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processShapes(const TopAbs_ShapeEnum &theType)
{
  TopExp_Explorer     anExp(myShape, theType);
  TopTools_MapOfShape aMapFence;

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Shape aShape = anExp.Current(); // Copy

    if (aMapFence.Add(aShape)) {
      if (myMapRemoved.Contains(aShape) ||
          !myMapModified.IsBound(aShape)) {
        // Skip removed or not modified shape.
        continue;
      }

      aShape.Orientation(TopAbs_FORWARD);

      switch(theType) {
        case TopAbs_EDGE:
          processEdge(aShape);
          break;
        case TopAbs_WIRE:
          processWire(aShape);
          break;
        case TopAbs_FACE:
        case TopAbs_SOLID:
          processFOrSo(aShape);
          break;
        case TopAbs_SHELL:
        case TopAbs_COMPSOLID:
          processShOrCS(aShape);
          break;
        case TopAbs_COMPOUND:
          processCompound(aShape);
          break;
        default:
          break;
      }
    }
  }

  if (theType == TopAbs_FACE || theType == TopAbs_SOLID) {
    // Clear duplicated edges from the faces and faces from solids
    removeBoundsOnFOrSo(theType);
  }
}

//=======================================================================
//function : processEdge
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processEdge(const TopoDS_Shape &theEdge)
{
  TopoDS_Iterator      anIter(theEdge);
  TopTools_MapOfShape  aMapFence;
  TopTools_ListOfShape aVtxList;

  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aShapeVertex = anIter.Value();

    if (aMapFence.Add(aShapeVertex)) {
      if (myMapRemoved.Contains(aShapeVertex)) {
        // This vertex is removed.
        const TopAbs_Orientation anOri = aShapeVertex.Orientation();

        if (anOri == TopAbs_FORWARD || anOri == TopAbs_REVERSED) {
          // This edge will disappear from the result.
          return;
        }
      } else {
        // This vertex is not removed.
        aVtxList.Append(aShapeVertex);
      }
    }
  }

  TopoDS_Shape aNewEdge = makeShape(theEdge, aVtxList);

  myMapModified.ChangeFind(theEdge).Append(aNewEdge);
}

//=======================================================================
//function : processWire
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processWire(const TopoDS_Shape &theWire)
{
  // Get parent face for the wire.
  TopoDS_Face aFace;

  if (myMapShapeAnc.IsBound(theWire)) {
    TopTools_ListIteratorOfListOfShape anIter(myMapShapeAnc.Find(theWire));

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aParent = anIter.Value();

      if (aParent.ShapeType() == TopAbs_FACE) {
        aFace = TopoDS::Face(aParent.Oriented(TopAbs_FORWARD));
        break;
      }
    }
  }

  TopoDS_Wire                            aWire = TopoDS::Wire(theWire);
  BRepTools_WireExplorer                 anExp(aWire, aFace);
  NCollection_List<TopTools_ListOfShape> aListListEdges;
  TopTools_ListOfShape                   aListEdges;

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge &anEdge = anExp.Current();

    if (myMapRemoved.Contains(anEdge)) {
      // This edge is removed.
      if (!aListEdges.IsEmpty()) {
        aListListEdges.Append(aListEdges);
        aListEdges.Clear();
      }
    } else if (myMapModified.IsBound(anEdge)) {
      // This edge is modified.
      TopTools_ListOfShape aModifEdges;

      getModified(anEdge, aModifEdges);

      if (aModifEdges.IsEmpty()) {
        // This edge is not created.
        if (!aListEdges.IsEmpty()) {
          aListListEdges.Append(aListEdges);
          aListEdges.Clear();
        }
      } else {
        const TopoDS_Shape aModifEdge = oriented(aModifEdges.First(), anEdge);

        aListEdges.Append(aModifEdge);
      }
    } else {
      // Get an edge as it is.
      aListEdges.Append(anEdge);
    }
  }

  if (!aListEdges.IsEmpty()) {
    aListListEdges.Append(aListEdges);
  }

  if (!aListListEdges.IsEmpty()) {
    TopTools_ListOfShape aListWires;

    makeWires(theWire, aListListEdges, aListWires);
    myMapModified.ChangeFind(theWire) = aListWires;
  }
}

//=======================================================================
//function : processFOrSo
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processFOrSo(const TopoDS_Shape &theFOrSo)
{
  Standard_Boolean     isToCreate = Standard_True;
  TopTools_ListOfShape aClosedSubShapes;
  TopTools_ListOfShape aNewShapes;
  TopoDS_Shape         anOuterSubShape;
  TopAbs_ShapeEnum     aShapeType;
  TopAbs_ShapeEnum     aSubShapeType;

  if (theFOrSo.ShapeType() == TopAbs_FACE) {
    aShapeType      = TopAbs_FACE;
    aSubShapeType   = TopAbs_WIRE;
    anOuterSubShape = BRepTools::OuterWire(TopoDS::Face(theFOrSo));
  } else {
    aShapeType      = TopAbs_SOLID;
    aSubShapeType   = TopAbs_SHELL;
    anOuterSubShape = BRepClass3d::OuterShell(TopoDS::Solid(theFOrSo));
  }

  // Process an outer sub-shape.
  if (myMapRemoved.Contains(anOuterSubShape)) {
    isToCreate = Standard_False;
  } else if (myMapModified.IsBound(anOuterSubShape)) {
    TopTools_ListOfShape aModifSubShapes;

    getModified(anOuterSubShape, aModifSubShapes);

    // Check if there is a closed direct sub-shape.
    TopTools_ListIteratorOfListOfShape anIter(aModifSubShapes);
    TopoDS_Shape                       aClosedSubShape;

    for (isToCreate = Standard_False; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aSubShape = anIter.Value();

      if (aSubShape.ShapeType() == aSubShapeType && aSubShape.Closed()) {
        if (isToCreate) {
          // There is another closed sub-shape. NEVERREACHED.
          // No need to create a new shape.
          isToCreate = Standard_False;
          break;
        } else {
          // Remember the closed sub-shape.
          isToCreate      = Standard_True;
          aClosedSubShape = aSubShape;
        }
      }
    }

    if (isToCreate) {
      // Add a closed sub-shape.
      const TopoDS_Shape aNewSubShape =
        oriented(aClosedSubShape, anOuterSubShape);

      aClosedSubShapes.Append(aNewSubShape);
    }

    // Copy shapes to the list of other shapes.
    for (anIter.Initialize(aModifSubShapes); anIter.More(); anIter.Next()) {
      const TopoDS_Shape aNewShape = oriented(anIter.Value(), anOuterSubShape);

      if (!isToCreate || !aNewShape.IsSame(aClosedSubShape)) {
        aNewShapes.Append(aNewShape);
      }
    }
  } else {
    aClosedSubShapes.Append(anOuterSubShape);
  }

  // Treat holes.
  TopoDS_Iterator anIter(theFOrSo);

  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (aSubShape.IsSame(anOuterSubShape)) {
      // Skip an outer sub-shape.
      continue;
    }

    if (myMapModified.IsBound(aSubShape)) {
      // This is a modified sub-shape.
      TopTools_ListOfShape aModifSubShapes;

      getModified(aSubShape, aModifSubShapes);

      TopTools_ListIteratorOfListOfShape anIter(aModifSubShapes);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape aNewShape = oriented(anIter.Value(), aSubShape);

        if (isToCreate) {
          if (aNewShape.ShapeType() == aSubShapeType && aNewShape.Closed()) {
            // This is a closed sub-shape.
            aClosedSubShapes.Append(aNewShape);
          } else {
            aNewShapes.Append(aNewShape);
          }
        } else {
          aNewShapes.Append(aNewShape);
        }
      }
    } else if (!myMapRemoved.Contains(aSubShape)) {
      // The shape is not modified.
      if (isToCreate) {
        aClosedSubShapes.Append(aSubShape);
      } else {
        aNewShapes.Append(aSubShape);
      }
    }
  }

  if (isToCreate) {
    // Create a new shape.
    TopoDS_Shape aNewShape = makeShape(theFOrSo, aClosedSubShapes);

    aNewShapes.Prepend(aNewShape);
  }

  if (!aNewShapes.IsEmpty()) {
    // Store modified shapes.
    myMapModified.ChangeFind(theFOrSo) = aNewShapes;
  }
}

//=======================================================================
//function : processShOrCS
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processShOrCS(const TopoDS_Shape &theShOrCS)
{
  // Treat sub-shapes.
  TopoDS_Iterator      anIter(theShOrCS);
  TopTools_ListOfShape aNewSubShapes;
  TopTools_ListOfShape aNewOtherShapes;
  TopAbs_ShapeEnum     aSubShapeType;
  TopAbs_ShapeEnum     aSubSubShapeType;

  if (theShOrCS.ShapeType() == TopAbs_SHELL) {
    aSubShapeType    = TopAbs_FACE;
    aSubSubShapeType = TopAbs_EDGE;
  } else { // comp-solid
    aSubShapeType    = TopAbs_SOLID;
    aSubSubShapeType = TopAbs_FACE;
  }

  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (myMapModified.IsBound(aSubShape)) {
      TopTools_ListOfShape aModifList;

      getModified(aSubShape, aModifList);

      // Copy shapes to the list of other shapes.
      TopTools_ListIteratorOfListOfShape anIter(aModifList);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape aNewShape = oriented(anIter.Value(), aSubShape);

        if (aNewShape.ShapeType() == aSubShapeType) {
          aNewSubShapes.Append(aNewShape);
        } else {
          aNewOtherShapes.Append(aNewShape);
        }
      }
    } else if (!myMapRemoved.Contains(aSubShape)) {
      // Shape is neither removed nor modified. Add it as it is.
      if (aSubShape.ShapeType() == aSubShapeType) {
        aNewSubShapes.Append(aSubShape);
      } else {
        aNewOtherShapes.Append(aSubShape);
      }
    }
  }

  // Group sub-shapes via bounds
  TopTools_ListOfShape aNewShapes;

  groupViaBounds(theShOrCS, aNewSubShapes, aNewShapes);
  aNewOtherShapes.Prepend(aNewShapes);

  if (!aNewOtherShapes.IsEmpty()) {
    // Store modified shapes.
    myMapModified.ChangeFind(theShOrCS) = aNewOtherShapes;
  }
}

//=======================================================================
//function : processCompound
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::processCompound(const TopoDS_Shape &theCompound)
{
  // Treat sub-shapes.
  TopoDS_Iterator      anIter(theCompound);
  TopTools_ListOfShape aNewSubShapes;

  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aSubShape = anIter.Value();

    if (myMapModified.IsBound(aSubShape)) {
      TopTools_ListOfShape aModifList;

      getModified(aSubShape, aModifList);

      // Copy shapes to the list of other shapes.
      TopTools_ListIteratorOfListOfShape anIter(aModifList);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape aNewShape = oriented(anIter.Value(), aSubShape);

        aNewSubShapes.Append(aNewShape);
      }
    } else if (!myMapRemoved.Contains(aSubShape)) {
      // Shape is neither removed nor modified. Add it as it is.
      aNewSubShapes.Append(aSubShape);
    }
  }

  if (!aNewSubShapes.IsEmpty()) {
    if (aNewSubShapes.Extent() == 1) {
      // Avoid creation of new compound for a single sub-shape.
      myMapModified.ChangeFind(theCompound).Append(aNewSubShapes.First());
    } else {
      TopoDS_Shape aNewShape = makeShape(theCompound, aNewSubShapes);

      // Store modified shapes.
      myMapModified.ChangeFind(theCompound).Append(aNewShape);
    }
  }
}

//=======================================================================
//function : removeBoundsOnFOrSo
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::removeBoundsOnFOrSo(const TopAbs_ShapeEnum theType)
{
  // Get bounds on faces or solids.
  TopExp_Explorer            anExp(myShape, theType);
  TopTools_MapOfShape        aMapFence;
  TopAbs_ShapeEnum           aBoundType;
  TopAbs_ShapeEnum           aComplexBndType;
  TopTools_IndexedMapOfShape aMapBounds;

  if (theType == TopAbs_FACE) {
    aBoundType      = TopAbs_EDGE;
    aComplexBndType = TopAbs_WIRE;
  } else { // solid
    aBoundType      = TopAbs_FACE;
    aComplexBndType = TopAbs_SHELL;
  }

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aShape = anExp.Current();

    if (aMapFence.Add(aShape)) {
      if (myMapRemoved.Contains(aShape)) {
        continue;
      }

      if (myMapModified.IsBound(aShape)) {
        TopTools_ListOfShape aNewShapes;

        getModified(aShape, aNewShapes);

        if (!aNewShapes.IsEmpty()) {
          const TopoDS_Shape &aNewShape = aNewShapes.First();

          if (aNewShape.ShapeType() == theType) {
            // Get bounds from the modified shape.
            TopExp::MapShapes(aNewShape, aBoundType, aMapBounds);
          }
        }
      } else {
        // Get bounds from the original shapes.
        TopExp::MapShapes(aShape, aBoundType, aMapBounds);
      }
    }
  }

  // Remove duplicated bounds from the faces or solids
  aMapFence.Clear();

  for (anExp.Init(myShape, theType); anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aShape = anExp.Current();

    if (aMapFence.Add(aShape)) {
      if (myMapModified.IsBound(aShape)) {
        TopTools_ListOfShape               &aNewShapes =
                                       myMapModified.ChangeFind(aShape);
        TopTools_ListIteratorOfListOfShape  anIter(aNewShapes);

        while (anIter.More()) {
          const TopoDS_Shape &aSubShape = anIter.Value();
          Standard_Boolean    isToRm    = Standard_False;

          if (aSubShape.ShapeType() == aBoundType) {
            // edge or face
            isToRm = aMapBounds.Contains(aSubShape);
          } else if (aSubShape.ShapeType() == aComplexBndType) {
            // wire or shell
            TopTools_ListOfShape aNewBounds;
            Standard_Boolean     isModified;

            if (theType == TopAbs_FACE) {
              isModified = removeCommonEdges(aSubShape, aMapBounds, aNewBounds);
            } else {
              isModified = removeCommonFaces(aSubShape, aMapBounds, aNewBounds);
            }

            if (isModified) {
              myMapModified.Bind(aSubShape, aNewBounds);
              aNewShapes.InsertBefore(aNewBounds, anIter);
              isToRm = Standard_True; // To remove unmodified bound.
            }
          }

          if (isToRm) {
            aNewShapes.Remove(anIter);
          } else {
            anIter.Next();
          }
        }
      }
    }
  }
}

//=======================================================================
//function : oriented
//purpose  :
//=======================================================================
TopoDS_Shape GEOMAlgo_Extractor::oriented(const TopoDS_Shape &theShape,
                                          const TopoDS_Shape &theContext)
{
  const TopAbs_Orientation aShapeOri   = theShape.Orientation();
  const TopAbs_Orientation aContextOri = theContext.Orientation();
  TopoDS_Shape             aResult     = theShape;

  aResult.Orientation(TopAbs::Compose(aShapeOri, aContextOri));

  return aResult;
}

//=======================================================================
//function : makeShape
//purpose  :
//=======================================================================
TopoDS_Shape GEOMAlgo_Extractor::makeShape
                        (const TopoDS_Shape         &theShape,
                         const TopTools_ListOfShape &theSubShapes)
{
  TopoDS_Shape aResult = getShapeFromSubShapes(theShape, theSubShapes);

  if (aResult.IsNull()) {
    // Create a new shape.
    BRep_Builder                       aBuilder;
    TopTools_ListIteratorOfListOfShape anIter(theSubShapes);
    TopTools_MapOfShape                aMapFence;

    aResult = theShape.EmptyCopied();
    aMapFence.Clear();

    for (; anIter.More(); anIter.Next()) {
      const TopoDS_Shape &aSubShape = anIter.Value();

      if (aMapFence.Add(aSubShape)) {
        aBuilder.Add(aResult, aSubShape);

        // Fill the map of new shape - ancestors.
        if (!myMapNewShapeAnc.IsBound(aSubShape)) {
          myMapNewShapeAnc.Bind(aSubShape, TopTools_ListOfShape());
        }

        myMapNewShapeAnc.ChangeFind(aSubShape).Append(aResult);
      }
    }
  }

  return aResult;
}

//=======================================================================
//function : getShapeFromSubShapes
//purpose  :
//=======================================================================
TopoDS_Shape GEOMAlgo_Extractor::getShapeFromSubShapes
                              (const TopoDS_Shape         &theShape,
                               const TopTools_ListOfShape &theSubShapes)
{
  // Fill the map of sub-shapes.
  TopTools_ListIteratorOfListOfShape anIter(theSubShapes);
  TopTools_MapOfShape                aMapSubShapes;
  TopoDS_Shape                       aFirstSubShape = theSubShapes.First();
  TopoDS_Shape                       aResult;

  for (; anIter.More(); anIter.Next()) {
    aMapSubShapes.Add(anIter.Value());
  }

  // Check if such a shape is already created.
  if (!aMapSubShapes.IsEmpty()) {
    TopTools_MapIteratorOfMapOfShape aMapIt(aMapSubShapes);
    Standard_Boolean                 isFirst = Standard_True;
    TopTools_MapOfShape              aMapAncs[2];
    Standard_Integer                 iCur    = 0;
    Standard_Integer                 iPrev   = 1;

    for (; aMapIt.More(); aMapIt.Next()) {
      const TopoDS_Shape &aSubShape = aMapIt.Key();

      // Switch iCur and iPrev.
      iCur  = iCur  ? 0 : 1;
      iPrev = iPrev ? 0 : 1;

      if (myMapNewShapeAnc.IsBound(aSubShape)) {
        TopTools_ListIteratorOfListOfShape
                      anAncIt(myMapNewShapeAnc.Find(aSubShape));

        if (isFirst) {
          // This is a first loop. Just fill the map of ancestors.
          for (; anAncIt.More(); anAncIt.Next()) {
            aMapAncs[iCur].Add(anAncIt.Value());
          }
        } else {
          // Add in aMapAnc[iCur] elements that are only in aMapAnc[iPrev].
          for (aMapAncs[iCur].Clear(); anAncIt.More(); anAncIt.Next()) {
            const TopoDS_Shape &anAncestor = anAncIt.Value();

            if (aMapAncs[iPrev].Contains(anAncestor)) {
              aMapAncs[iCur].Add(anAncIt.Value());
            }
          }
        }

        if (aMapAncs[iCur].IsEmpty()) {
          // There is no common shape. It means that
          // the result should be a new shape.
          aMapAncs[iCur].Clear();
          break;
        }
      } else {
        // This is a new sub-shape. So the result shape is new.
        aMapAncs[iCur].Clear();
        break;
      }
    }

    if (!aMapAncs[iCur].IsEmpty()) {
      // Get exactly the same shape.
      const TopAbs_ShapeEnum aType = theShape.ShapeType();

      for (aMapIt.Initialize(aMapAncs[iCur]); aMapIt.More(); aMapIt.Next()) {
        const TopoDS_Shape &aShape = aMapIt.Key();

        if (aShape.ShapeType() == aType) {
          // Check sub-shapes.
          TopoDS_Iterator    aSubShIt(aShape);
          TopAbs_Orientation aNewOri       = TopAbs_FORWARD;
          Standard_Boolean   isComposedOri = Standard_False;

          for (; aSubShIt.More(); aSubShIt.Next()) {
            const TopoDS_Shape &aSubSh = aSubShIt.Value();

            if (!aMapSubShapes.Contains(aSubSh)) {
              // There are another sub-shapes in the ancestor.
              break;
            }

            if (!isComposedOri && aSubSh.IsSame(aFirstSubShape)) {
              // Compose orientaiton.
              isComposedOri = Standard_True;
              aNewOri       = TopAbs::Compose
                (aFirstSubShape.Orientation(), aSubSh.Orientation());
            }
          }

          if (!aSubShIt.More()) {
            // That is the same shape. Compose the orientation.
            aResult = aShape;
            aResult.Orientation(aNewOri);
            break;
          }
        }
      }
    }
  }

  return aResult;
}

//=======================================================================
//function : makeResult
//purpose  :
//=======================================================================
TopoDS_Shape GEOMAlgo_Extractor::makeResult(const TopoDS_Shape &theShape)
{
  TopoDS_Shape aResult;

  if (!myMapRemoved.Contains(theShape)) {
    if (myMapModified.IsBound(theShape)) {
      // The shape is modified.
      TopTools_ListOfShape aListModif;

      getModified(theShape, aListModif);

      const Standard_Integer aNbShapes = aListModif.Extent();

      if (aNbShapes == 1) {
        aResult = oriented(aListModif.First(), theShape);
      } else if (aNbShapes > 1) {
        // Build a result as a compound
        TopTools_ListIteratorOfListOfShape anIter(aListModif);
        BRep_Builder                       aBuilder;
        TopoDS_Compound                    aCompound;
        TopTools_MapOfShape                aMapFence;

        aBuilder.MakeCompound(aCompound);

        for (; anIter.More(); anIter.Next()) {
          const TopoDS_Shape aModifShape = oriented(anIter.Value(), theShape);

          if (aMapFence.Add(aModifShape)) {
            aBuilder.Add(aCompound, aModifShape);
          }
        }

        aResult = aCompound;
      }
    } else {
      // The result is not modified shape.
      aResult = theShape;
    }
  }

  return aResult;
}

//=======================================================================
//function : makeHistory
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::makeHistory(const TopoDS_Shape        &theShape,
                                           TopTools_MapOfShape &theMapFence)
{
  if (theMapFence.Add(theShape)) {
    Standard_Boolean isKept = Standard_True;

    if (myMapRemoved.Contains(theShape)) {
      myRemoved.Append(theShape);
      isKept = Standard_False;
    } else if (myMapModified.IsBound(theShape)) {
      TopTools_ListOfShape aListModif;

      getModified(theShape, aListModif, theShape.ShapeType());

      Standard_Boolean       isModif = !aListModif.IsEmpty();
      const TopAbs_ShapeEnum aType   = theShape.ShapeType();

      if (isModif) {
        // Add the new shapes.
        TopTools_ListIteratorOfListOfShape anIter(aListModif);

        // Skip the first shape.
        for (anIter.Next(); anIter.More(); anIter.Next()) {
          myNew.Append(anIter.Value());
        }
      }

      if (isModif) {
        myModified.Append(theShape);
      } else {
        myRemoved.Append(theShape);
      }

      isKept = Standard_False;
    }

    if (!isKept) {
      // Collect history for children.
      TopoDS_Iterator anIter(theShape);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape &aSubShape = anIter.Value();

        makeHistory(aSubShape, theMapFence);
      }
    }
  }
}

//=======================================================================
//function : removeCommonEdges
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_Extractor::removeCommonEdges
                     (const TopoDS_Shape               &theWire,
                      const TopTools_IndexedMapOfShape &theMapEdgesToRm,
                            TopTools_ListOfShape       &theNewWires)
{
  TopExp_Explorer                        anExp(theWire, TopAbs_EDGE);
  NCollection_List<TopTools_ListOfShape> aListListEdges;
  TopTools_ListOfShape                   aListEdges;
  Standard_Boolean                       isModified = Standard_False;
  TopoDS_Vertex                          aVtx[2];

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &anEdge = anExp.Current();

    if (theMapEdgesToRm.Contains(anEdge)) {
      // This edge is removed.
      TopExp::Vertices(TopoDS::Edge(anEdge), aVtx[0], aVtx[1]);

      // Skip edges that have same first and last vertices.
      if (aVtx[0].IsNull() || !aVtx[0].IsSame(aVtx[1])) {
        if (!aListEdges.IsEmpty()) {
          aListListEdges.Append(aListEdges);
          aListEdges.Clear();
        }
      }

      isModified = Standard_True;
    } else {
      aListEdges.Append(anEdge);
    }
  }

  if (!aListEdges.IsEmpty()) {
    aListListEdges.Append(aListEdges);
  }

  if (isModified && !aListListEdges.IsEmpty()) {
    // Make wires.
    makeWires(theWire, aListListEdges, theNewWires);
  }

  return isModified;
}

//=======================================================================
//function : removeCommonFaces
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_Extractor::removeCommonFaces
                     (const TopoDS_Shape               &theShell,
                      const TopTools_IndexedMapOfShape &theMapFacesToRm,
                            TopTools_ListOfShape       &theNewShells)
{
  TopExp_Explorer      anExp(theShell, TopAbs_FACE);
  TopTools_ListOfShape aListFaces;
  Standard_Boolean     isModified = Standard_False;

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aFace = anExp.Current();

    if (theMapFacesToRm.Contains(aFace)) {
      isModified = Standard_True;
    } else {
      aListFaces.Append(aFace);
    }
  }

  if (isModified && !aListFaces.IsEmpty()) {
    // Create new shells.
    groupViaBounds(theShell, aListFaces, theNewShells);
  }

  return isModified;
}

//=======================================================================
//function : makeWires
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::makeWires
            (const TopoDS_Shape                           &theWire,
                   NCollection_List<TopTools_ListOfShape> &theListListEdges,
                   TopTools_ListOfShape                   &theWires)
{
  if (theListListEdges.Size() > 1) {
    // Check if it is possible to merge first and last lists of edges.
    TopoDS_Edge   anEdgeFirst = TopoDS::Edge(theListListEdges.First().First());
    TopoDS_Edge   anEdgeLast  = TopoDS::Edge(theListListEdges.Last().Last());
    TopoDS_Vertex aCommonVtx;

    if (TopExp::CommonVertex(anEdgeFirst, anEdgeLast, aCommonVtx)) {
      // Merge First and last lists of edges.
      theListListEdges.First().Prepend(theListListEdges.Last());
      // Remove the last list.
      NCollection_List<TopTools_ListOfShape>::Iterator anIter(theListListEdges);

      for (;anIter.More(); anIter.Next()) {
        if (anIter.Value().IsEmpty()) {
          theListListEdges.Remove(anIter);
          break;
        }
      }
    }
  }

  // Create wires.
  NCollection_List<TopTools_ListOfShape>::Iterator anIter(theListListEdges);

  for (;anIter.More(); anIter.Next()) {
    const TopTools_ListOfShape &anEdges       = anIter.Value();
    TopoDS_Shape                aNewWireShape = makeShape(theWire, anEdges);
    TopoDS_Wire                 aNewWire      = TopoDS::Wire(aNewWireShape);
    TopoDS_Vertex               aV[2];

    TopExp::Vertices(aNewWire, aV[0], aV[1]);

    if (!aV[0].IsNull() && !aV[1].IsNull()) {
      aNewWire.Closed(aV[0].IsSame(aV[1]));
    }

    theWires.Append(aNewWire);
  }
}

//=======================================================================
//function : groupViaBounds
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::groupViaBounds
                       (const TopoDS_Shape         &theShape,
                        const TopTools_ListOfShape &theSubShapes,
                              TopTools_ListOfShape &theNewShapes)
{
  const Standard_Boolean isShell = theShape.ShapeType() == TopAbs_SHELL;
  TopAbs_ShapeEnum       aBoundType;

  if (isShell) {
    aBoundType = TopAbs_EDGE;
  } else { // comp-solid
    aBoundType = TopAbs_FACE;
  }

  // Group connected sub-shapes.
  NCollection_Sequence<TopTools_ListOfShape> aGroupedSubShapes;
  NCollection_Sequence<TopTools_MapOfShape>  aBounds;
  TopTools_ListIteratorOfListOfShape         anIt(theSubShapes);
  Standard_Integer                           i;

  for (; anIt.More(); anIt.Next()) {
    // Find a zone a sub-shape is connected to.
    const TopoDS_Shape     &aSubShape = anIt.Value();
    TColStd_MapOfInteger    aMapIndices;
    const Standard_Integer  aNbZones  = aBounds.Size();
    TopExp_Explorer         anExp(aSubShape, aBoundType);
    Standard_Integer        j;

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aSubSubShape = anExp.Current();

      // Check each zone.
      for (i = 1; i <= aNbZones; ++i) {
        if (!aMapIndices.Contains(i)) {
          if (aBounds.Value(i).Contains(aSubSubShape)) {
            // The current sub-shape belongs to this zone.
            aMapIndices.Add(i);
            break;
          }
        }
      }
    }

    if (aMapIndices.IsEmpty()) {
      // Create a new zone.
      aGroupedSubShapes.Append(TopTools_ListOfShape());
      aBounds.Append(TopTools_MapOfShape());
      aGroupedSubShapes.ChangeLast().Append(aSubShape);
      anExp.Init(aSubShape, aBoundType);

      TopTools_MapOfShape &aLastZoneBound = aBounds.ChangeLast();

      for (; anExp.More(); anExp.Next()) {
        aLastZoneBound.Add(anExp.Current());
      }
    } else {
      // Merge zones. Get the first zone.
      for (i = 1; i <= aNbZones; ++i) {
        if (aMapIndices.Contains(i)) {
          break;
        }
      }

      // Merge other zones with the first one.
      TopTools_ListOfShape &aZoneSubShapes = aGroupedSubShapes.ChangeValue(i);
      TopTools_MapOfShape  &aZoneBounds    = aBounds.ChangeValue(i);

      for (j = i + 1; j <= aNbZones; ++j) {
        if (aMapIndices.Contains(j)) {
          aZoneSubShapes.Append(aGroupedSubShapes.ChangeValue(j));

          TopTools_MapIteratorOfMapOfShape aMapIt(aBounds.Value(j));

          for (; aMapIt.More(); aMapIt.Next()) {
            aZoneBounds.Add(aMapIt.Key());
          }
        }
      }

      // Remove merged zones.
      for (j = aNbZones; j > i; --j) {
        aGroupedSubShapes.Remove(j);
        aBounds.Remove(j);
      }

      // Add aSubShape to merged zone.
      aZoneSubShapes.Append(aSubShape);
      anExp.Init(aSubShape, aBoundType);

      for (; anExp.More(); anExp.Next()) {
        const TopoDS_Shape &aSubSubShape = anExp.Current();

        if (!aZoneBounds.Add(aSubSubShape)) {
          aZoneBounds.Remove(aSubSubShape);
        }
      }
    }
  }

  // Construct new shapes from sub-shapes.
  const Standard_Integer aNbGroups = aGroupedSubShapes.Size();
  TopTools_ListOfShape   aNewSubShapes;

  for (i = 1; i <= aNbGroups; ++i) {
    const TopTools_ListOfShape &aListSubShapes = aGroupedSubShapes.Value(i);

    if (!isShell && aListSubShapes.Extent() == 1) {
      // Avoid creation of comp-solid with a single solid.
      aNewSubShapes.Append(aListSubShapes.First());
    } else {
      TopoDS_Shape aNewShape = makeShape(theShape, aListSubShapes);

      if (aBounds.Value(i).IsEmpty()) {
        // This is a closed shape.
        aNewShape.Closed(Standard_True);
      }

      theNewShapes.Append(aNewShape);
    }
  }

  // Append the list of single solids (if it is filled).
  theNewShapes.Append(aNewSubShapes);
}

//=======================================================================
//function : getModified
//purpose  :
//=======================================================================
void GEOMAlgo_Extractor::getModified(const TopoDS_Shape         &theShape,
                                           TopTools_ListOfShape &theModifShapes,
                                     const TopAbs_ShapeEnum      theShapeType)
{
  // This shape is modified.
  TopTools_ListIteratorOfListOfShape anIt(myMapModified.Find(theShape));

  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape &aSubShape = anIt.Value();

    if (theShapeType == TopAbs_SHAPE || aSubShape.ShapeType() == theShapeType) {
      if (myMapModified.IsBound(aSubShape)) {
        getModified(aSubShape, theModifShapes);
      } else {
        theModifShapes.Append(aSubShape);
      }
    }
  }
}


//
// myErrorStatus :
//
// 10 -myShape=NULL
// 11 -mySubShapes contains not only sub-shapes of myShape.
// 12 -Can't remove the main shape.
// 13 -mySubShapes contains seam edges in context of faces.
// 14 -mySubShapes contains degenerated edges in context of faces.
//
// myWarningStatus :
//
// 10 -mySubShapes is empty
//
