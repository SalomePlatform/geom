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

#include <GEOMImpl_ShapeDriver.hxx>

#include <GEOMImpl_IExtract.hxx>
#include <GEOMImpl_IIsoline.hxx>
#include <GEOMImpl_IShapes.hxx>
#include <GEOMImpl_IShapeExtend.hxx>
#include <GEOMImpl_IVector.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_Block6Explorer.hxx>

#include <GEOM_Function.hxx>
#include <GEOMUtils_Hatcher.hxx>
#include <GEOMAlgo_State.hxx>
#include <GEOMAlgo_Extractor.hxx>

// OCCT Includes
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>

#include <TNaming_CopyShape.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <ElCLib.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>

#include <Precision.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_MakerVolume.hxx>

#include <list>

/**
 * \brief This static function converts the list of shapes into an array
 *  of their IDs. If the input list is empty, null handle will be returned.
 *  this method doesn't check if a shape presents in theIndices map.
 *
 * \param theListOfShapes the list of shapes.
 * \param theIndices the indexed map of shapes.
 * \return the array of shape IDs.
 */
static Handle(TColStd_HArray1OfInteger) GetShapeIDs
                  (const TopTools_ListOfShape       &theListOfShapes,
                   const TopTools_IndexedMapOfShape &theIndices)
{
  Handle(TColStd_HArray1OfInteger) aResult;

  if (!theListOfShapes.IsEmpty()) {
    const Standard_Integer             aNbShapes = theListOfShapes.Extent();
    TopTools_ListIteratorOfListOfShape anIter(theListOfShapes);
    Standard_Integer                   i;

    aResult = new TColStd_HArray1OfInteger(1, aNbShapes);

    for (i = 1; anIter.More(); anIter.Next(), ++i) {
      const TopoDS_Shape     &aShape  = anIter.Value();
      const Standard_Integer  anIndex = theIndices.FindIndex(aShape);

      aResult->SetValue(i, anIndex);
    }
  }

  return aResult;
}

namespace
{
  // check that compound includes only shapes of expected type
  bool checkCompound( TopoDS_Shape& c, TopAbs_ShapeEnum t )
  {
    TopoDS_Iterator it( c, Standard_True, Standard_True );

    // empty compound is OK only if we explicitly create a compound of shapes
    bool result = true;

    // => if expected type is TopAbs_SHAPE, we allow compound consisting of any shapes, this above check is enough
    // => otherwise we have to check compound's content
    // => compound sometimes can contain enclosed compound(s), we process them recursively and rebuild initial compound

    if ( t != TopAbs_SHAPE ) {
      result = it.More();
      std::list<TopoDS_Shape> compounds, shapes;
      compounds.push_back( c );
      while ( !compounds.empty() && result ) {
        // check that compound contains only shapes of expected type
        TopoDS_Shape cc = compounds.front();
        compounds.pop_front();
        it.Initialize( cc, Standard_True, Standard_True );
        for ( ; it.More() && result; it.Next() ) {
          TopAbs_ShapeEnum tt = it.Value().ShapeType();
          if ( tt == TopAbs_COMPOUND || tt == TopAbs_COMPSOLID ) {
            compounds.push_back( it.Value() );
            continue;
          }
          shapes.push_back( it.Value() );
          result = tt == t;
        }
      }
      if ( result ) {
        if ( shapes.empty() ) {
          result = false;
        }
        else if ( shapes.size() == 1 ) {
          c = shapes.front();
        }
        else {
          BRep_Builder b;
          TopoDS_Compound newc;
          b.MakeCompound( newc );
          std::list<TopoDS_Shape> ::const_iterator sit;
          for ( sit = shapes.begin(); sit != shapes.end(); ++sit )
          b.Add( newc, *sit );
          c = newc;
        }
      }
    }

    return result;
  }

  /**
   * This function adds faces from the input shape into the list of faces. If
   * the input shape is a face, it is added itself. If it is a shell, its
   * sub-shapes (faces) are added. If it is a compound, its sub-shapes
   * (faces or shells) are added in the list. For null shapes and for other
   * types of shapes an exception is thrown.
   *
   * @param theShape the shape to be added. Either face or shell or a compound
   *        of faces and/or shells.
   * @param theListFaces the list of faces that is modified on output.
   * @param theMapFence the map that protects from adding the same faces in
   *        the list.
   */
  void addFaces(const TopoDS_Shape         &theShape,
                      TopTools_ListOfShape &theListFaces,
                      TopTools_MapOfShape  &theMapFence)
  {
    if (theShape.IsNull()) {
      Standard_NullObject::Raise("Face for shell construction is null");
    }

    // Append the shape is the mapFence
    if (theMapFence.Add(theShape)) {
      // Shape type
      const TopAbs_ShapeEnum aType = theShape.ShapeType();

      if (aType == TopAbs_FACE) {
        theListFaces.Append(theShape);
      } else if (aType == TopAbs_SHELL || aType == TopAbs_COMPOUND) {
        TopoDS_Iterator anIter(theShape);

        for (; anIter.More(); anIter.Next()) {
          // Add sub-shapes: faces for shell or faces/shells for compound.
          const TopoDS_Shape &aSubShape = anIter.Value();

          addFaces(aSubShape, theListFaces, theMapFence);
        }
      } else {
        Standard_TypeMismatch::Raise
          ("Shape for shell construction is neither a shell nor a face");
      }
    }
  }

  /**
   * This function constructs a shell or a compound of shells
   * from a set of faces and/or shells.
   *
   * @param theShapes is a set of faces, shells and/or
   *        compounds of faces/shells.
   * @return a shell or a compound of shells.
   */
  TopoDS_Shape makeShellFromFaces
        (const Handle(TColStd_HSequenceOfTransient) &theShapes)
  {
    const Standard_Integer aNbShapes = theShapes->Length();
    Standard_Integer       i;
    TopTools_ListOfShape   aListFaces;
    TopTools_MapOfShape    aMapFence;
    BRep_Builder           aBuilder;

    // Fill the list of unique faces
    for (i = 1; i <= aNbShapes; ++i) {
      // Function
      const Handle(GEOM_Function) aRefShape =
        Handle(GEOM_Function)::DownCast(theShapes->Value(i));

      if (aRefShape.IsNull()) {
        Standard_NullObject::Raise("Face for shell construction is null");
      }

      // Shape
      const TopoDS_Shape aShape = aRefShape->GetValue();

      addFaces(aShape, aListFaces, aMapFence);
    }

    // Perform computation of shells.
    TopTools_ListOfShape               aListShells;
    TopTools_ListIteratorOfListOfShape anIter;

    while (!aListFaces.IsEmpty()) {
      // Perform sewing
      BRepBuilderAPI_Sewing aSewing(Precision::Confusion()*10.0);

      for (anIter.Initialize(aListFaces); anIter.More(); anIter.Next()) {
        aSewing.Add(anIter.Value());
      }

      aSewing.Perform();

      // Fill list of shells.
      const TopoDS_Shape &aSewed = aSewing.SewedShape();
      TopExp_Explorer     anExp(aSewed, TopAbs_SHELL);
      Standard_Boolean    isNewShells = Standard_False;

      // Append shells
      for (; anExp.More(); anExp.Next()) {
        aListShells.Append(anExp.Current());
        isNewShells = Standard_True;
      }

      // Append single faces.
      anExp.Init(aSewed, TopAbs_FACE, TopAbs_SHELL);

      for (; anExp.More(); anExp.Next()) {
        TopoDS_Shell aShell;

        aBuilder.MakeShell(aShell);
        aBuilder.Add(aShell, anExp.Current());
        aListShells.Append(aShell);
        isNewShells = Standard_True;
      }

      if (!isNewShells) {
        // There are no more shell can be obtained. Break the loop.
        break;
      }

      // Remove faces that are in the result from the list.
      TopTools_IndexedMapOfShape aMapFaces;

      TopExp::MapShapes(aSewed, TopAbs_FACE, aMapFaces);

      // Add deleted faces to the map
      const Standard_Integer aNbDelFaces = aSewing.NbDeletedFaces();

      for (i = 1; i <= aNbDelFaces; ++i) {
        aMapFaces.Add(aSewing.DeletedFace(i));
      }

      for (anIter.Initialize(aListFaces); anIter.More();) {
        const TopoDS_Shape &aFace      = anIter.Value();
        Standard_Boolean    isFaceUsed = Standard_False;

        if (aMapFaces.Contains(aFace) || aSewing.IsModified(aFace)) {
          // Remove face from the list.
          aListFaces.Remove(anIter);
        } else {
          // Go to the next face.
          anIter.Next();
        }
      }
    }

    // If there are faces not used in shells create a shell for each face.
    for (anIter.Initialize(aListFaces); anIter.More(); anIter.Next()) {
      TopoDS_Shell aShell;

      aBuilder.MakeShell(aShell);
      aBuilder.Add(aShell, anIter.Value());
      aListShells.Append(aShell);
    }

    // Construct the result that can be either a shell or a compound of shells
    TopoDS_Shape aResult;

    if (!aListShells.IsEmpty()) {
      if (aListShells.Extent() == 1) {
        aResult = aListShells.First();
      } else {
        // There are more then one shell.
        TopoDS_Compound aCompound;

        aBuilder.MakeCompound(aCompound);

        for (anIter.Initialize(aListShells); anIter.More(); anIter.Next()) {
          aBuilder.Add(aCompound, anIter.Value());
        }

        aResult = aCompound;
      }
    }

    return aResult;
  }

  // End of namespace
}

//modified by NIZNHY-PKV Wed Dec 28 13:48:20 2011f
//static
//  void KeepEdgesOrder(const Handle(TopTools_HSequenceOfShape)& aEdges,
//                    const Handle(TopTools_HSequenceOfShape)& aWires);
//modified by NIZNHY-PKV Wed Dec 28 13:48:23 2011t

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ShapeDriver::GetID()
{
  static Standard_GUID aShapeDriver("FF1BBB54-5D14-4df2-980B-3A668264EA16");
  return aShapeDriver;
}


//=======================================================================
//function : GEOMImpl_ShapeDriver
//purpose  :
//=======================================================================
GEOMImpl_ShapeDriver::GEOMImpl_ShapeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ShapeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IShapes aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  TCollection_AsciiString aWarning;

  // this is an exact type of expected shape, or shape in a compound if compound is allowed as a result (see below)
  TopAbs_ShapeEnum anExpectedType = TopAbs_SHAPE;
  // this should be true if result can be a compound of shapes of strict type (see above)
  bool allowCompound = false;

  BRep_Builder B;

  if (aType == WIRE_EDGES) {
    // result may be only a single wire
    anExpectedType = TopAbs_WIRE;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();

    Standard_Real aTolerance = aCI.GetTolerance();
    if (aTolerance < Precision::Confusion())
      aTolerance = Precision::Confusion();

    aShape = MakeWireFromEdges(aShapes, aTolerance);
  }
  else if (aType == FACE_WIRE) {
    // result may be a face or a compound of faces
    anExpectedType = TopAbs_FACE;
    allowCompound = true;

    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) Standard_NullObject::Raise("Argument Shape is null");
    TopoDS_Wire W;
    if (aShapeBase.ShapeType() == TopAbs_WIRE) {
      W = TopoDS::Wire(aShapeBase);
      // check the wire is closed
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(W, aV1, aV2);
      if ( !aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2) )
        aShapeBase.Closed(true);
      else
        Standard_NullObject::Raise
          ("Shape for face construction is not closed");
    }
    else if (aShapeBase.ShapeType() == TopAbs_EDGE && BRep_Tool::IsClosed(aShapeBase)) {
      BRepBuilderAPI_MakeWire MW;
      MW.Add(TopoDS::Edge(aShapeBase));
      if (!MW.IsDone()) {
        Standard_ConstructionError::Raise("Wire construction failed");
      }
      W = MW;
    }
    else {
      Standard_NullObject::Raise
        ("Shape for face construction is neither a wire nor a closed edge");
    }
    aWarning = GEOMImpl_Block6Explorer::MakeFace(W, aCI.GetIsPlanar(), aShape);
    if (aShape.IsNull()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }
  }
  else if (aType == FACE_WIRES) {
    // result may be a face or a compound of faces
    anExpectedType = TopAbs_FACE;
    allowCompound = true;

    // Try to build a face from a set of wires and edges
    int ind;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    int nbshapes = aShapes->Length();
    if (nbshapes < 1) {
      Standard_ConstructionError::Raise("No wires or edges given");
    }

    // 1. Extract all edges from the given arguments
    TopTools_MapOfShape aMapEdges;
    Handle(TopTools_HSequenceOfShape) aSeqEdgesIn = new TopTools_HSequenceOfShape;
    TColStd_IndexedDataMapOfTransientTransient aMapTShapes;

    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefSh_i = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aSh_i = aRefSh_i->GetValue();

      TopExp_Explorer anExpE_i (aSh_i, TopAbs_EDGE);
      for (; anExpE_i.More(); anExpE_i.Next()) {
        if (aMapEdges.Add(anExpE_i.Current())) {
          // Copy the original shape.
          TopoDS_Shape aShapeCopy;

          TNaming_CopyShape::CopyTool
            (anExpE_i.Current(), aMapTShapes, aShapeCopy);
          aSeqEdgesIn->Append(aShapeCopy);
        }
      }
    }

    if (aSeqEdgesIn->IsEmpty()) {
      Standard_ConstructionError::Raise("No edges given");
    }

    // 2. Connect edges to wires of maximum length
    Handle(TopTools_HSequenceOfShape) aSeqWiresOut;
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(aSeqEdgesIn, Precision::Confusion(),
                                                  /*shared*/Standard_False, aSeqWiresOut);
    //modified by NIZNHY-PKV Wed Dec 28 13:46:55 2011f
    //KeepEdgesOrder(aSeqEdgesIn, aSeqWiresOut);
    //modified by NIZNHY-PKV Wed Dec 28 13:46:59 2011t

    // 3. Separate closed wires
    Handle(TopTools_HSequenceOfShape) aSeqClosedWires = new TopTools_HSequenceOfShape;
    Handle(TopTools_HSequenceOfShape) aSeqOpenWires = new TopTools_HSequenceOfShape;
    for (ind = 1; ind <= aSeqWiresOut->Length(); ind++) {
      if (aSeqWiresOut->Value(ind).Closed())
        aSeqClosedWires->Append(aSeqWiresOut->Value(ind));
      else
        aSeqOpenWires->Append(aSeqWiresOut->Value(ind));
    }

    if (aSeqClosedWires->Length() < 1) {
      Standard_ConstructionError::Raise
        ("There is no closed contour can be built from the given arguments");
    }

    // 4. Build a face / list of faces from all the obtained closed wires

    // 4.a. Basic face
    TopoDS_Shape aFFace;
    TopoDS_Wire aW1 = TopoDS::Wire(aSeqClosedWires->Value(1));
    aWarning = GEOMImpl_Block6Explorer::MakeFace(aW1, aCI.GetIsPlanar(), aFFace);
    if (aFFace.IsNull()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }

    // 4.b. Add other wires
    if (aSeqClosedWires->Length() == 1) {
      aShape = aFFace;
    }
    else {
      TopoDS_Compound C;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound(C);
      BRepAlgo_FaceRestrictor FR;

      TopAbs_Orientation OriF = aFFace.Orientation();
      TopoDS_Shape aLocalS = aFFace.Oriented(TopAbs_FORWARD);
      FR.Init(TopoDS::Face(aLocalS), Standard_False, Standard_True);

      for (ind = 1; ind <= aSeqClosedWires->Length(); ind++) {
        TopoDS_Wire aW = TopoDS::Wire(aSeqClosedWires->Value(ind));
        FR.Add(aW);
      }

      FR.Perform();

      if (FR.IsDone()) {
        int k = 0;
        TopoDS_Shape aFace;
        for (; FR.More(); FR.Next()) {
          aFace = FR.Current().Oriented(OriF);
          aBuilder.Add(C, aFace);
          k++;
        }
        if (k == 1) {
          aShape = aFace;
        } else {
          aShape = C;
        }
      }
    }

    // 5. Add all open wires to the result
    if (aSeqOpenWires->Length() > 0) {
      //Standard_ConstructionError::Raise("There are some open wires");
      TopoDS_Compound C;
      BRep_Builder aBuilder;
      if (aSeqClosedWires->Length() == 1) {
        aBuilder.MakeCompound(C);
        aBuilder.Add(C, aShape);
      }
      else {
        C = TopoDS::Compound(aShape);
      }

      for (ind = 1; ind <= aSeqOpenWires->Length(); ind++) {
        aBuilder.Add(C, aSeqOpenWires->Value(ind));
      }

      aShape = C;
    }
  }
  else if (aType == FACE_FROM_SURFACE) {
    // result may be only a face
    anExpectedType = TopAbs_FACE;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();

    if (aShapes.IsNull() == Standard_False) {
      Standard_Integer aNbShapes = aShapes->Length();

      if (aNbShapes == 2) {
        Handle(GEOM_Function) aRefFace =
          Handle(GEOM_Function)::DownCast(aShapes->Value(1));
        Handle(GEOM_Function) aRefWire =
          Handle(GEOM_Function)::DownCast(aShapes->Value(2));

        if (aRefFace.IsNull() == Standard_False &&
            aRefWire.IsNull() == Standard_False) {
          TopoDS_Shape aShFace = aRefFace->GetValue();
          TopoDS_Shape aShWire = aRefWire->GetValue();

          if (aShFace.IsNull()    == Standard_False &&
              aShFace.ShapeType() == TopAbs_FACE    &&
              aShWire.IsNull()    == Standard_False &&
              aShWire.ShapeType() == TopAbs_WIRE) {
            TopoDS_Face             aFace = TopoDS::Face(aShFace);
            TopoDS_Wire             aWire = TopoDS::Wire(aShWire);
            Handle(Geom_Surface)    aSurf = BRep_Tool::Surface(aFace);
            BRepBuilderAPI_MakeFace aMkFace(aSurf, aWire);

            if (aMkFace.IsDone()) {
              aShape = aMkFace.Shape();
            }
          }
        }
      }
    }
  }
  else if (aType == SHELL_FACES) {
    // result may be only a shell or a compound of shells
    anExpectedType = TopAbs_SHELL;
    allowCompound = true;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();

    if (aShapes.IsNull()) {
      Standard_NullObject::Raise("Argument Shapes is null");
    }

    // Compute a shell or a compound of shells.
    aShape = makeShellFromFaces(aShapes);
  }
  else if (aType == SOLID_SHELLS) {
    // result may be only a solid or a compound of solids
    anExpectedType = TopAbs_SOLID;
    allowCompound = true;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();
    Standard_Integer ish = 0;
    BRepBuilderAPI_MakeSolid aMkSolid;

    // add shapes
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShapeShell = aRefShape->GetValue();
      if (aShapeShell.IsNull()) {
        Standard_NullObject::Raise("Shell for solid construction is null");
      }
      if (aShapeShell.ShapeType() == TopAbs_COMPOUND) {
        TopoDS_Iterator It (aShapeShell, Standard_True, Standard_True);
        for (; It.More(); It.Next()) {
          TopoDS_Shape aSubShape = It.Value();
          if (aSubShape.ShapeType() == TopAbs_SHELL) {
            aMkSolid.Add(TopoDS::Shell(aSubShape));
            ish++;
          }
          else
            Standard_TypeMismatch::Raise
              ("Shape for solid construction is neither a shell nor a compound of shells");
        }
      }
      else if (aShapeShell.ShapeType() == TopAbs_SHELL) {
        aMkSolid.Add(TopoDS::Shell(aShapeShell));
        ish++;
      }
    }
    if (ish == 0 || !aMkSolid.IsDone()) return 0;

    TopoDS_Solid Sol = aMkSolid.Solid();
    BRepClass3d_SolidClassifier SC (Sol);
    SC.PerformInfinitePoint(Precision::Confusion());
    if (SC.State() == TopAbs_IN)
      aShape = Sol.Reversed();
    else
      aShape = Sol;
  }
  else if (aType == COMPOUND_SHAPES) {
    // result may be only a compound of any shapes
    allowCompound = true;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add shapes
    TopoDS_Compound C;
    B.MakeCompound(C);
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Shape for compound construction is null");
      }
      B.Add(C, aShape_i);
    }

    aShape = C;

  }
  else if (aType == EDGE_WIRE) {
    // result may be only an edge
    anExpectedType = TopAbs_EDGE;

    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aWire = aRefBase->GetValue();
    Standard_Real LinTol = aCI.GetTolerance();
    Standard_Real AngTol = aCI.GetAngularTolerance();
    if (aWire.IsNull()) Standard_NullObject::Raise("Argument Wire is null");

    aShape = MakeEdgeFromWire(aWire, LinTol, AngTol);
  }
  else if (aType == SOLID_FACES) {
    // result may be only a solid or a compound of solids
    anExpectedType = TopAbs_SOLID;
    allowCompound = true;
    
    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();
    
    // add faces
    BOPCol_ListOfShape aLS;
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Shape for solid construction is null");
      }
      if (aShape_i.ShapeType() == TopAbs_COMPOUND) {
        TopoDS_Iterator It (aShape_i, Standard_True, Standard_True);
        for (; It.More(); It.Next()) {
          TopoDS_Shape aSubShape = It.Value();
          if (aSubShape.ShapeType() == TopAbs_FACE || aSubShape.ShapeType() == TopAbs_SHELL)
            aLS.Append(aSubShape);
          else
            Standard_TypeMismatch::Raise
              ("Shape for solid construction is neither a list of faces and/or shells "
               "nor a compound of faces and/or shells");
        }
      }
      aLS.Append(aShape_i);
    }

    BOPAlgo_MakerVolume aMV;
    aMV.SetArguments(aLS);
    aMV.SetIntersect(aCI.GetIsIntersect());
    aMV.Perform();
    if (aMV.ErrorStatus()) return 0;

    aShape = aMV.Shape();
  }
  else if (aType == EDGE_CURVE_LENGTH) {
    // result may be only an edge
    anExpectedType = TopAbs_EDGE;

    GEOMImpl_IVector aVI (aFunction);

    // RefCurve
    Handle(GEOM_Function) aRefCurve = aVI.GetPoint1();
    if (aRefCurve.IsNull()) Standard_NullObject::Raise("Argument Curve is null");
    TopoDS_Shape aRefShape1 = aRefCurve->GetValue();
    if (aRefShape1.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Edge On Curve creation aborted : curve shape is not an edge");
    }
    TopoDS_Edge aRefEdge = TopoDS::Edge(aRefShape1);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(aRefEdge, V1, V2, Standard_True);

    // RefPoint
    TopoDS_Vertex aRefVertex;
    Handle(GEOM_Function) aRefPoint = aVI.GetPoint2();
    if (aRefPoint.IsNull()) {
      aRefVertex = V1;
    }
    else {
      TopoDS_Shape aRefShape2 = aRefPoint->GetValue();
      if (aRefShape2.ShapeType() != TopAbs_VERTEX) {
        Standard_TypeMismatch::Raise
          ("Edge On Curve creation aborted : start point shape is not a vertex");
      }
      aRefVertex = TopoDS::Vertex(aRefShape2);
    }
    gp_Pnt aRefPnt = BRep_Tool::Pnt(aRefVertex);

    // Length
    Standard_Real aLength = aVI.GetParameter();
    //Standard_Real aCurveLength = IntTools::Length(aRefEdge);
    //if (aLength > aCurveLength) {
    //  Standard_ConstructionError::Raise
    //    ("Edge On Curve creation aborted : given length is greater than edges length");
    //}
    if (fabs(aLength) < Precision::Confusion()) {
      Standard_ConstructionError::Raise
        ("Edge On Curve creation aborted : given length is smaller than Precision::Confusion()");
    }

    // Check orientation
    Standard_Real UFirst, ULast;
    Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(aRefEdge, UFirst, ULast);
    Handle(Geom_Curve) ReOrientedCurve = EdgeCurve;

    Standard_Real dU = ULast - UFirst;
    Standard_Real par1 = UFirst + 0.1 * dU;
    Standard_Real par2 = ULast  - 0.1 * dU;

    gp_Pnt P1 = EdgeCurve->Value(par1);
    gp_Pnt P2 = EdgeCurve->Value(par2);

    if (aRefPnt.SquareDistance(P2) < aRefPnt.SquareDistance(P1)) {
      ReOrientedCurve = EdgeCurve->Reversed();
      UFirst = EdgeCurve->ReversedParameter(ULast);
    }

    // Get the point by length
    GeomAdaptor_Curve AdapCurve = GeomAdaptor_Curve(ReOrientedCurve);
    GCPnts_AbscissaPoint anAbsPnt (AdapCurve, aLength, UFirst);
    Standard_Real aParam = anAbsPnt.Parameter();

    if (AdapCurve.IsClosed() && aLength < 0.0) {
      Standard_Real aTmp = aParam;
      aParam = UFirst;
      UFirst = aTmp;
    }

    BRepBuilderAPI_MakeEdge aME (ReOrientedCurve, UFirst, aParam);
    if (aME.IsDone())
      aShape = aME.Shape();
  }
  else if (aType == SHAPE_ISOLINE) {
    // result may be only an edge or compound of edges
    anExpectedType = TopAbs_EDGE;
    allowCompound = true;

    GEOMImpl_IIsoline     aII (aFunction);
    Handle(GEOM_Function) aRefFace = aII.GetFace();
    TopoDS_Shape          aShapeFace = aRefFace->GetValue();

    if (aShapeFace.ShapeType() == TopAbs_FACE) {
      TopoDS_Face   aFace  = TopoDS::Face(aShapeFace);
      bool          isUIso = aII.GetIsUIso();
      Standard_Real aParam = aII.GetParameter();
      Standard_Real U1,U2,V1,V2;

      // Construct a real geometric parameter.
      aFace.Orientation(TopAbs_FORWARD);
      ShapeAnalysis::GetFaceUVBounds(aFace,U1,U2,V1,V2);

      if (isUIso) {
        aParam = U1 + (U2 - U1)*aParam;
      } else {
        aParam = V1 + (V2 - V1)*aParam;
      }

      aShape = MakeIsoline(aFace, isUIso, aParam);
    } else {
      Standard_NullObject::Raise
        ("Shape for isoline construction is not a face");
    }
  }
  else if (aType == EDGE_UV) {
    // result may be only an edge
    anExpectedType = TopAbs_EDGE;

    GEOMImpl_IShapeExtend aSE (aFunction);
    Handle(GEOM_Function) aRefEdge   = aSE.GetShape();
    TopoDS_Shape          aShapeEdge = aRefEdge->GetValue();

    if (aShapeEdge.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShapeEdge);

      aShape = ExtendEdge(anEdge, aSE.GetUMin(), aSE.GetUMax());
    }
  }
  else if (aType == FACE_UV) {
    // result may be only a face
    anExpectedType = TopAbs_FACE;

    GEOMImpl_IShapeExtend aSE (aFunction);
    Handle(GEOM_Function) aRefFace   = aSE.GetShape();
    TopoDS_Shape          aShapeFace = aRefFace->GetValue();

    if (aShapeFace.ShapeType() == TopAbs_FACE) {
      TopoDS_Face aFace = TopoDS::Face(aShapeFace);

      aFace.Orientation(TopAbs_FORWARD);
      aShape = ExtendFace(aFace, aSE.GetUMin(), aSE.GetUMax(),
                          aSE.GetVMin(), aSE.GetVMax()); 
    }
  }
  else if (aType == SURFACE_FROM_FACE) {
    // result may be only a face
    anExpectedType = TopAbs_FACE;

    GEOMImpl_IShapeExtend aSE (aFunction);
    Handle(GEOM_Function) aRefFace   = aSE.GetShape();
    TopoDS_Shape          aShapeFace = aRefFace->GetValue();

    if (aShapeFace.ShapeType() == TopAbs_FACE) {
      TopoDS_Face          aFace    = TopoDS::Face(aShapeFace);
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);

      if (aSurface.IsNull() == Standard_False) {
        Handle(Standard_Type) aType = aSurface->DynamicType();
        Standard_Real         aU1;
        Standard_Real         aU2;
        Standard_Real         aV1;
        Standard_Real         aV2;

         // Get U, V bounds of the face.
        aFace.Orientation(TopAbs_FORWARD);
        ShapeAnalysis::GetFaceUVBounds(aFace, aU1, aU2, aV1, aV2);

        // Get the surface of original type
        while (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
          Handle(Geom_RectangularTrimmedSurface) aTrSurface =
            Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);

          aSurface = aTrSurface->BasisSurface();
          aType    = aSurface->DynamicType();
        }

        const Standard_Real     aTol = BRep_Tool::Tolerance(aFace);
        BRepBuilderAPI_MakeFace aMF(aSurface, aU1, aU2, aV1, aV2, aTol);

        if (aMF.IsDone()) {
          aShape = aMF.Shape();
        }
      }
    }
  } else if (aType == EXTRACTION) {
    allowCompound = true;

    GEOMImpl_IExtract     aCI(aFunction);
    Handle(GEOM_Function) aRefShape  = aCI.GetShape();
    TopoDS_Shape          aShapeBase = aRefShape->GetValue();

    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Argument Shape is null");
      return 0;
    }

    Handle(TColStd_HArray1OfInteger) anIDs = aCI.GetSubShapeIDs();
    TopTools_ListOfShape             aListSubShapes;
    TopTools_IndexedMapOfShape       anIndices;
    int                              i;

    TopExp::MapShapes(aShapeBase, anIndices);

    if (!anIDs.IsNull()) {
      const int anUpperID = anIDs->Upper();
      const int aNbShapes = anIndices.Extent();

      for (i = anIDs->Lower(); i <= anUpperID; ++i) {
        const Standard_Integer anIndex = anIDs->Value(i);

        if (anIndex < 1 || anIndex > aNbShapes) {
          TCollection_AsciiString aMsg(" Invalid index: ");

          aMsg += TCollection_AsciiString(anIndex);
          StdFail_NotDone::Raise(aMsg.ToCString());
          return 0;
        }

        const TopoDS_Shape &aSubShape = anIndices.FindKey(anIndex);

        aListSubShapes.Append(aSubShape);
      }
    }

    // Compute extraction.
    GEOMAlgo_Extractor anExtractor;

    anExtractor.SetShape(aShapeBase);
    anExtractor.SetShapesToRemove(aListSubShapes);

    anExtractor.Perform();

    // Interprete results
    Standard_Integer iErr = anExtractor.ErrorStatus();

    // The detailed description of error codes is in GEOMAlgo_Extractor.cxx
    if (iErr) {
      TCollection_AsciiString aMsg(" iErr : ");

      aMsg += TCollection_AsciiString(iErr);
      StdFail_NotDone::Raise(aMsg.ToCString());
      return 0;
    }

    aShape = anExtractor.GetResult();

    if (aShape.IsNull()) {
      Standard_ConstructionError::Raise("Result of extraction is empty");
    }

    // Get statistics.
    const TopTools_ListOfShape       &aRemoved    = anExtractor.GetRemoved();
    const TopTools_ListOfShape       &aModified   = anExtractor.GetModified();
    const TopTools_ListOfShape       &aNew        = anExtractor.GetNew();
    Handle(TColStd_HArray1OfInteger) aRemovedIDs  =
                          GetShapeIDs(aRemoved, anIndices);
    Handle(TColStd_HArray1OfInteger) aModifiedIDs =
                          GetShapeIDs(aModified, anIndices);
    Handle(TColStd_HArray1OfInteger) aNewIDs;

    if (!aShape.IsNull()) {
      // Get newly created sub-shapes
      TopTools_IndexedMapOfShape aNewIndices;

      TopExp::MapShapes(aShape, aNewIndices);
      aNewIDs = GetShapeIDs(aNew, aNewIndices);
    }

    if (!aRemovedIDs.IsNull()) {
      aCI.SetRemovedIDs(aRemovedIDs);
    }

    if (!aModifiedIDs.IsNull()) {
      aCI.SetModifiedIDs(aModifiedIDs);
    }

    if (!aNewIDs.IsNull()) {
      aCI.SetAddedIDs(aNewIDs);
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  // Check shape validity
  BRepCheck_Analyzer ana (aShape, true);
  if (!ana.IsValid()) {
    //Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
    // For Mantis issue 0021772: EDF 2336 GEOM: Non valid face created from two circles
    Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape (aShape);
    aSfs->Perform();
    aShape = aSfs->Shape();
  }

  // Check if the result shape is of expected type.
  const TopAbs_ShapeEnum aShType = aShape.ShapeType();

  bool ok = false;
  if ( aShType == TopAbs_COMPOUND || aShType == TopAbs_COMPSOLID ) {
    ok = allowCompound && checkCompound( aShape, anExpectedType );
  }
  else {
    ok = ( anExpectedType == TopAbs_SHAPE ) || ( aShType == anExpectedType );
  }
  if (!ok)
    Standard_ConstructionError::Raise("Result type check failed");

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  if (!aWarning.IsEmpty())
    Standard_Failure::Raise(aWarning.ToCString());

  return 1;
}

TopoDS_Wire GEOMImpl_ShapeDriver::MakeWireFromEdges(const Handle(TColStd_HSequenceOfTransient)& theEdgesFuncs,
                                                    const Standard_Real theTolerance)
{
  BRep_Builder B;

  TopoDS_Wire aWire;
  B.MakeWire(aWire);

  // add edges
  for (unsigned int ind = 1; ind <= theEdgesFuncs->Length(); ind++) {
    Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(theEdgesFuncs->Value(ind));
    TopoDS_Shape aShape_i = aRefShape->GetValue();
    if (aShape_i.IsNull()) {
      Standard_NullObject::Raise("Shape for wire construction is null");
    }
    if (aShape_i.ShapeType() == TopAbs_EDGE || aShape_i.ShapeType() == TopAbs_WIRE) {
      TopExp_Explorer exp (aShape_i, TopAbs_EDGE);
      for (; exp.More(); exp.Next())
        B.Add(aWire, TopoDS::Edge(exp.Current()));
    } else {
      Standard_TypeMismatch::Raise
        ("Shape for wire construction is neither an edge nor a wire");
    }
  }

  // fix edges order
  Handle(ShapeFix_Wire) aFW = new ShapeFix_Wire;
  aFW->Load(aWire);
  aFW->FixReorder();

  if (aFW->StatusReorder(ShapeExtend_FAIL1)) {
    Standard_ConstructionError::Raise("Wire construction failed: several loops detected");
  }
  else if (aFW->StatusReorder(ShapeExtend_FAIL)) {
    Standard_ConstructionError::Raise("Wire construction failed");
  }
  else {
  }

  // IMP 0019766: Building a Wire from unconnected edges by introducing a tolerance
  aFW->ClosedWireMode() = Standard_False;
  aFW->FixConnected(theTolerance);
  if (aFW->StatusConnected(ShapeExtend_FAIL)) {
    Standard_ConstructionError::Raise("Wire construction failed: cannot build connected wire");
  }
  // IMP 0019766
  if (aFW->StatusConnected(ShapeExtend_DONE3)) {
    // Confused with <prec> but not Analyzer.Precision(), set the same
    aFW->FixGapsByRangesMode() = Standard_True;
    if (aFW->FixGaps3d()) {
      Handle(ShapeExtend_WireData) sbwd = aFW->WireData();
      Handle(ShapeFix_Edge) aFe = new ShapeFix_Edge;
      for (Standard_Integer iedge = 1; iedge <= sbwd->NbEdges(); iedge++) {
        TopoDS_Edge aEdge = TopoDS::Edge(sbwd->Edge(iedge));
        aFe->FixVertexTolerance(aEdge);
        aFe->FixSameParameter(aEdge);
      }
    }
    else if (aFW->StatusGaps3d(ShapeExtend_FAIL)) {
      Standard_ConstructionError::Raise("Wire construction failed: cannot fix 3d gaps");
    }
  }
  aWire = aFW->WireAPIMake();

  return aWire;
}

TopoDS_Edge GEOMImpl_ShapeDriver::MakeEdgeFromWire(const TopoDS_Shape& aWire,
                                                   const Standard_Real LinTol,
                                                   const Standard_Real AngTol)
{
    TopoDS_Edge ResEdge;

    BRepLib::BuildCurves3d(aWire);
    Handle(ShapeFix_Shape) Fixer = new ShapeFix_Shape(aWire);
    Fixer->SetPrecision(LinTol);
    Fixer->SetMaxTolerance(LinTol);
    Fixer->Perform();
    TopoDS_Wire theWire = TopoDS::Wire(Fixer->Shape());

    TColGeom_SequenceOfCurve CurveSeq;
    TopTools_SequenceOfShape LocSeq;
    TColStd_SequenceOfReal FparSeq;
    TColStd_SequenceOfReal LparSeq;
    TColStd_SequenceOfReal TolSeq;
    GeomAbs_CurveType CurType;
    TopoDS_Vertex FirstVertex, LastVertex;
    Standard_Real aPntShiftDist = 0.;

    BRepTools_WireExplorer wexp(theWire) ;
    for (; wexp.More(); wexp.Next())
    {
      TopoDS_Edge anEdge = wexp.Current();
      Standard_Real fpar, lpar;
      TopLoc_Location aLoc;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aLoc, fpar, lpar);
      if (aCurve.IsNull())
        continue;

      BRepAdaptor_Curve BAcurve(anEdge);
      GeomAbs_CurveType aType = BAcurve.GetType();

      Handle(Geom_Curve) aBasisCurve = BAcurve.Curve().Curve();

      if (aBasisCurve->IsPeriodic())
        ElCLib::AdjustPeriodic(aBasisCurve->FirstParameter(), aBasisCurve->LastParameter(),
                               Precision::PConfusion(), fpar, lpar);

      if (CurveSeq.IsEmpty())
      {
        CurveSeq.Append(aCurve);
        TopoDS_Shape aLocShape;
        aLocShape.Location(aLoc);
        aLocShape.Orientation(wexp.Orientation());
        LocSeq.Append(aLocShape);
        FparSeq.Append(fpar);
        LparSeq.Append(lpar);
        CurType = aType;
        FirstVertex = wexp.CurrentVertex();
      }
      else
      {
        Standard_Boolean Done = Standard_False;
        Standard_Real NewFpar, NewLpar;
        Handle(Geom_Geometry) aTrsfGeom = CurveSeq.Last()->Transformed
                      (LocSeq.Last().Location().Transformation());
        GeomAdaptor_Curve GAprevcurve(Handle(Geom_Curve)::DownCast(aTrsfGeom));
        TopoDS_Vertex CurVertex = wexp.CurrentVertex();
        TopoDS_Vertex CurFirstVer = TopExp::FirstVertex(anEdge);
        TopAbs_Orientation ConnectByOrigin = (CurVertex.IsSame(CurFirstVer))? TopAbs_FORWARD : TopAbs_REVERSED;
        if (aCurve == CurveSeq.Last() && aLoc.IsEqual(LocSeq.Last().Location()))
        {
          NewFpar = fpar;
          NewLpar = lpar;
          if (aBasisCurve->IsPeriodic())
          {
            if (NewLpar < NewFpar)
              NewLpar += aBasisCurve->Period();
            if (ConnectByOrigin == TopAbs_FORWARD)
              ElCLib::AdjustPeriodic(FparSeq.Last(),
                                     FparSeq.Last() + aBasisCurve->Period(),
                                     Precision::PConfusion(), NewFpar, NewLpar);
            else
              ElCLib::AdjustPeriodic(FparSeq.Last() - aBasisCurve->Period(),
                                     FparSeq.Last(),
                                     Precision::PConfusion(), NewFpar, NewLpar);
          }
          Done = Standard_True;
        }
        else if (aType == CurType &&
                 aType != GeomAbs_BezierCurve &&
                 aType != GeomAbs_BSplineCurve &&
                 aType != GeomAbs_OtherCurve)
        {
          switch (aType)
          {
          case GeomAbs_Line:
            {
              gp_Lin aLine    = BAcurve.Line();
              gp_Lin PrevLine = GAprevcurve.Line();
              if (aLine.Contains(PrevLine.Location(), LinTol) &&
                  aLine.Direction().IsParallel(PrevLine.Direction(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aLine);
                gp_Pnt P2 = ElCLib::Value(lpar, aLine);
                NewFpar = ElCLib::Parameter(PrevLine, P1);
                NewLpar = ElCLib::Parameter(PrevLine, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevLine);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevLine);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Circle:
            {
              gp_Circ aCircle    = BAcurve.Circle();
              gp_Circ PrevCircle = GAprevcurve.Circle();
              if (aCircle.Location().Distance(PrevCircle.Location()) <= LinTol &&
                  Abs(aCircle.Radius() - PrevCircle.Radius()) <= LinTol &&
                  aCircle.Axis().IsParallel(PrevCircle.Axis(), AngTol))
              {
                const Standard_Boolean isFwd = ConnectByOrigin == TopAbs_FORWARD;

                if (aCircle.Axis().Direction() * PrevCircle.Axis().Direction() < 0.)
                {
                  Standard_Real memfpar = fpar;
                  fpar = lpar;
                  lpar = memfpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                gp_Pnt P1 = ElCLib::Value(fpar, aCircle);
                gp_Pnt P2 = ElCLib::Value(lpar, aCircle);
                NewFpar = ElCLib::Parameter(PrevCircle, P1);
                NewLpar = ElCLib::Parameter(PrevCircle, P2);

                // Compute shift
                if (isFwd) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevCircle);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevCircle);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                  NewLpar += 2.*M_PI;
                //Standard_Real MemNewFpar = NewFpar, MemNewLpar =  NewLpar;
                if (ConnectByOrigin == TopAbs_FORWARD)
                  ElCLib::AdjustPeriodic(FparSeq.Last(),
                                         FparSeq.Last() + 2.*M_PI,
                                         Precision::PConfusion(), NewFpar, NewLpar);
                else
                  ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
                                         FparSeq.Last(),
                                         Precision::PConfusion(), NewFpar, NewLpar);
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Ellipse:
            {
              gp_Elips anEllipse   = BAcurve.Ellipse();
              gp_Elips PrevEllipse = GAprevcurve.Ellipse();
              if (anEllipse.Focus1().Distance(PrevEllipse.Focus1()) <= LinTol &&
                  anEllipse.Focus2().Distance(PrevEllipse.Focus2()) <= LinTol &&
                  Abs(anEllipse.MajorRadius() - PrevEllipse.MajorRadius()) <= LinTol &&
                  Abs(anEllipse.MinorRadius() - PrevEllipse.MinorRadius()) <= LinTol &&
                  anEllipse.Axis().IsParallel(PrevEllipse.Axis(), AngTol))
              {
                const Standard_Boolean isFwd = ConnectByOrigin == TopAbs_FORWARD;

                if (anEllipse.Axis().Direction() * PrevEllipse.Axis().Direction() < 0.)
                {
                  Standard_Real memfpar = fpar;
                  fpar = lpar;
                  lpar = memfpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                gp_Pnt P1 = ElCLib::Value(fpar, anEllipse);
                gp_Pnt P2 = ElCLib::Value(lpar, anEllipse);
                NewFpar = ElCLib::Parameter(PrevEllipse, P1);
                NewLpar = ElCLib::Parameter(PrevEllipse, P2);

                // Compute shift
                if (isFwd) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevEllipse);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevEllipse);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                  NewLpar += 2.*M_PI;
                if (ConnectByOrigin == TopAbs_FORWARD)
                  ElCLib::AdjustPeriodic(FparSeq.Last(),
                                         FparSeq.Last() + 2.*M_PI,
                                         Precision::PConfusion(), NewFpar, NewLpar);
                else
                  ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
                                         FparSeq.Last(),
                                         Precision::PConfusion(), NewFpar, NewLpar);
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Hyperbola:
            {
              gp_Hypr aHypr    = BAcurve.Hyperbola();
              gp_Hypr PrevHypr = GAprevcurve.Hyperbola();
              if (aHypr.Focus1().Distance(PrevHypr.Focus1()) <= LinTol &&
                  aHypr.Focus2().Distance(PrevHypr.Focus2()) <= LinTol &&
                  Abs(aHypr.MajorRadius() - PrevHypr.MajorRadius()) <= LinTol &&
                  Abs(aHypr.MinorRadius() - PrevHypr.MinorRadius()) <= LinTol &&
                  aHypr.Axis().IsParallel(PrevHypr.Axis(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aHypr);
                gp_Pnt P2 = ElCLib::Value(lpar, aHypr);
                NewFpar = ElCLib::Parameter(PrevHypr, P1);
                NewLpar = ElCLib::Parameter(PrevHypr, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevHypr);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevHypr);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Parabola:
            {
              gp_Parab aParab    = BAcurve.Parabola();
              gp_Parab PrevParab = GAprevcurve.Parabola();
              if (aParab.Location().Distance(PrevParab.Location()) <= LinTol &&
                  aParab.Focus().Distance(PrevParab.Focus()) <= LinTol &&
                  Abs(aParab.Focal() - PrevParab.Focal()) <= LinTol &&
                  aParab.Axis().IsParallel(PrevParab.Axis(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aParab);
                gp_Pnt P2 = ElCLib::Value(lpar, aParab);
                NewFpar = ElCLib::Parameter(PrevParab, P1);
                NewLpar = ElCLib::Parameter(PrevParab, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevParab);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevParab);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          } //end of switch (aType)
        } // end of else if (aType == CurType && ...
        if (Done)
        {
          if (NewFpar < FparSeq.Last())
            FparSeq(FparSeq.Length()) = NewFpar;
          else
            LparSeq(LparSeq.Length()) = NewLpar;
        }
        else
        {
          CurveSeq.Append(aCurve);
          TopoDS_Shape aLocShape;
          aLocShape.Location(aLoc);
          aLocShape.Orientation(wexp.Orientation());
          LocSeq.Append(aLocShape);
          FparSeq.Append(fpar);
          LparSeq.Append(lpar);
          TolSeq.Append(aPntShiftDist + BRep_Tool::Tolerance(CurVertex));
          aPntShiftDist = 0.;
          CurType = aType;
        }
      } // end of else (CurveSeq.IsEmpty()) -> not first time
    } // end for (; wexp.More(); wexp.Next())

    LastVertex = wexp.CurrentVertex();
    TolSeq.Append(aPntShiftDist + BRep_Tool::Tolerance(LastVertex));

    FirstVertex.Orientation(TopAbs_FORWARD);
    LastVertex.Orientation(TopAbs_REVERSED);

    if (!CurveSeq.IsEmpty())
    {
      Standard_Integer nb_curve = CurveSeq.Length();   //number of curves
      TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);                    //array of the curves
      TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances

      Standard_Integer i;

      if (nb_curve > 1)
      {
        for (i = 1; i <= nb_curve; i++)
        {
          if (CurveSeq(i)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
            CurveSeq(i) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(i))))->BasisCurve();

          Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(CurveSeq(i), FparSeq(i), LparSeq(i));
          tab(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
          tab(i-1)->Transform(LocSeq(i).Location().Transformation());
          GeomConvert::C0BSplineToC1BSplineCurve(tab(i-1), Precision::Confusion());
          if (LocSeq(i).Orientation() == TopAbs_REVERSED)
            tab(i-1)->Reverse();

          //Temporary
          //char* name = new char[100];
          //sprintf(name, "c%d", i);
          //DrawTrSurf::Set(name, tab(i-1));

          if (i > 1)
            tabtolvertex(i-2) = TolSeq(i-1);
        } // end for (i = 1; i <= nb_curve; i++)
        tabtolvertex(nb_curve-1) = TolSeq(TolSeq.Length());

        Standard_Boolean closed_flag = Standard_False;
        Standard_Real closed_tolerance = 0.;
        if (FirstVertex.IsSame(LastVertex) &&
            GeomLProp::Continuity(tab(0), tab(nb_curve-1),
                                  tab(0)->FirstParameter(),
                                  tab(nb_curve-1)->LastParameter(),
                                  Standard_False, Standard_False, LinTol, AngTol) >= GeomAbs_G1)
        {
          closed_flag = Standard_True ;
          closed_tolerance = BRep_Tool::Tolerance(FirstVertex);
        }

        Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
        Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
        GeomConvert::ConcatC1(tab,
                              tabtolvertex,
                              ArrayOfIndices,
                              concatcurve,
                              closed_flag,
                              closed_tolerance);   //C1 concatenation

        if (concatcurve->Length() > 1)
        {
          GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));

          for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
            Concat.Add( concatcurve->Value(i), LinTol, Standard_True );

          concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
        }
        // rnc : prevents the driver from building an edge without C1 continuity
        if (concatcurve->Value(concatcurve->Lower())->Continuity()==GeomAbs_C0){
          Standard_ConstructionError::Raise("Construction aborted : The given Wire has sharp bends between some Edges, no valid Edge can be built");
        }

        Standard_Boolean isValidEndVtx = Standard_True;

        if (closed_flag) {
          // Check if closed curve is reordered.
          Handle(Geom_BSplineCurve) aCurve  = concatcurve->Value(concatcurve->Lower());
          Standard_Real      aFPar   = aCurve->FirstParameter();
          gp_Pnt             aPFirst;
          gp_Pnt             aPntVtx = BRep_Tool::Pnt(FirstVertex);
          Standard_Real      aTolVtx = BRep_Tool::Tolerance(FirstVertex);

          aCurve->D0(aFPar, aPFirst);

          if (!aPFirst.IsEqual(aPntVtx, aTolVtx)) {
            // The curve is reordered. Find the new first and last vertices.
            TopTools_IndexedMapOfShape aMapVtx;
            TopExp::MapShapes(theWire, TopAbs_VERTEX, aMapVtx);

            const Standard_Integer aNbVtx = aMapVtx.Extent();
            Standard_Integer       iVtx;

            for (iVtx = 1; iVtx <= aNbVtx; iVtx++) {
              const TopoDS_Vertex aVtx = TopoDS::Vertex(aMapVtx.FindKey(iVtx));
              const gp_Pnt        aPnt = BRep_Tool::Pnt(aVtx);
              const Standard_Real aTol = BRep_Tool::Tolerance(aVtx);

              if (aPFirst.IsEqual(aPnt, aTol)) {
                // The coinsident vertex is found.
                FirstVertex = aVtx;
                LastVertex  = aVtx;
                FirstVertex.Orientation(TopAbs_FORWARD);
                LastVertex.Orientation(TopAbs_REVERSED);
                break;
              }
            }

            if (iVtx > aNbVtx) {
              // It is necessary to create new vertices.
              isValidEndVtx = Standard_False;
            }
          }
        }

        if (isValidEndVtx) {
          ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
                                     FirstVertex, LastVertex,
                                     concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                                     concatcurve->Value(concatcurve->Lower())->LastParameter());
        } else {
          ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
                                     concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                                     concatcurve->Value(concatcurve->Lower())->LastParameter());
        }
      }
      else
      {
        if (CurveSeq(1)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
          CurveSeq(1) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(1))))->BasisCurve();

        Handle(Geom_Curve) aNewCurve =
          Handle(Geom_Curve)::DownCast(CurveSeq(1)->Copy());

        aNewCurve->Transform(LocSeq(1).Location().Transformation());

        if (LocSeq(1).Orientation() == TopAbs_REVERSED) {
          const TopoDS_Vertex aVtxTmp = FirstVertex;

          FirstVertex = LastVertex;
          LastVertex  = aVtxTmp;
          FirstVertex.Orientation(TopAbs_FORWARD);
          LastVertex.Orientation(TopAbs_REVERSED);
        }

        ResEdge = BRepLib_MakeEdge(aNewCurve,
                                   FirstVertex, LastVertex,
                                   FparSeq(1), LparSeq(1));

        if (LocSeq(1).Orientation() == TopAbs_REVERSED) {
          ResEdge.Reverse();
        }
      }
    }

    return ResEdge;
}

//=============================================================================
/*!
 * \brief Returns an isoline for a face.
 */
//=============================================================================

TopoDS_Shape GEOMImpl_ShapeDriver::MakeIsoline
                            (const TopoDS_Face &theFace,
                             const bool         IsUIso,
                             const double       theParameter) const
{
  TopoDS_Shape          aResult;
  GEOMUtils::Hatcher    aHatcher(theFace);
  const GeomAbs_IsoType aType = (IsUIso ? GeomAbs_IsoU : GeomAbs_IsoV);

  aHatcher.Init(aType, theParameter);
  aHatcher.Perform();

  if (!aHatcher.IsDone()) {
    Standard_ConstructionError::Raise("MakeIsoline : Hatcher failure");
  }

  const Handle(TColStd_HArray1OfInteger) &anIndices =
    (IsUIso ? aHatcher.GetUIndices() : aHatcher.GetVIndices());

  if (anIndices.IsNull()) {
    Standard_ConstructionError::Raise("MakeIsoline : Null hatching indices");
  }

  const Standard_Integer anIsoInd = anIndices->Lower();
  const Standard_Integer aHatchingIndex = anIndices->Value(anIsoInd);

  if (aHatchingIndex == 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Invalid hatching index");
  }

  const Standard_Integer aNbDomains =
    aHatcher.GetNbDomains(aHatchingIndex);

  if (aNbDomains < 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Invalid number of domains");
  }

  // The hatching is performed successfully. Create the 3d Curve.
  Handle(Geom_Surface) aSurface   = BRep_Tool::Surface(theFace);
  Handle(Geom_Curve)   anIsoCurve = (IsUIso ?
    aSurface->UIso(theParameter) : aSurface->VIso(theParameter));
  Handle(Geom2d_Curve) aPIsoCurve =
    aHatcher.GetHatching(aHatchingIndex);
  const Standard_Real  aTol = Precision::Confusion();
  Standard_Integer     anIDom = 1;
  Standard_Real        aV1;
  Standard_Real        aV2;
  BRep_Builder         aBuilder;
  Standard_Integer     aNbEdges = 0;

  for (; anIDom <= aNbDomains; anIDom++) {
    if (aHatcher.GetDomain(aHatchingIndex, anIDom, aV1, aV2)) {
      // Check first and last parameters.
      if (!aHatcher.IsDomainInfinite(aHatchingIndex, anIDom)) {
        // Create an edge.
        TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(anIsoCurve, aV1, aV2);

        // Update it with a parametric curve on face.
        aBuilder.UpdateEdge(anEdge, aPIsoCurve, theFace, aTol);
        aNbEdges++;

        if (aNbEdges > 1) {
          // Result is a compond.
          if (aNbEdges == 2) {
            // Create a new compound.
            TopoDS_Compound aCompound;

            aBuilder.MakeCompound(aCompound);
            aBuilder.Add(aCompound, aResult);
            aResult = aCompound;
          }

          // Add an edge to the compound.
          aBuilder.Add(aResult, anEdge);
        } else {
          // Result is the edge.
          aResult = anEdge;
        }
      }
    }
  }

  if (aNbEdges == 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Empty result");
  }

  return aResult;
}

//=============================================================================
/*!
 * \brief Returns an extended edge.
 */
//=============================================================================

TopoDS_Shape GEOMImpl_ShapeDriver::ExtendEdge
                         (const TopoDS_Edge   &theEdge,
                          const Standard_Real  theMin,
                          const Standard_Real  theMax) const
{
  TopoDS_Shape        aResult;
  Standard_Real       aF;
  Standard_Real       aL;
  Handle(Geom_Curve)  aCurve   = BRep_Tool::Curve(theEdge, aF, aL);
  const Standard_Real aTol     = BRep_Tool::Tolerance(theEdge);
  Standard_Real       aRange2d = aL - aF;

  if (aCurve.IsNull() == Standard_False && aRange2d > aTol) {
    Standard_Real aMin = aF + aRange2d*theMin;
    Standard_Real aMax = aF + aRange2d*theMax;

    Handle(Standard_Type) aType = aCurve->DynamicType();

    // Get the curve of original type
    while (aType == STANDARD_TYPE(Geom_TrimmedCurve)) {
      Handle(Geom_TrimmedCurve) aTrCurve =
        Handle(Geom_TrimmedCurve)::DownCast(aCurve);

      aCurve = aTrCurve->BasisCurve();
      aType  = aCurve->DynamicType();
    }

    if (aCurve->IsPeriodic()) {
      // The curve is periodic. Check if a new range is less then a period.
      if (aMax - aMin > aCurve->Period()) {
        aMax = aMin + aCurve->Period();
      }
    } else {
      // The curve is not periodic. Check if aMin and aMax within bounds.
      aMin = Max(aMin, aCurve->FirstParameter());
      aMax = Min(aMax, aCurve->LastParameter());
    }

    if (aMax - aMin > aTol) {
      // Create a new edge.
      BRepBuilderAPI_MakeEdge aME (aCurve, aMin, aMax);

      if (aME.IsDone()) {
        aResult = aME.Shape();
      }
    }
  }

  return aResult;
}

//=============================================================================
/*!
 * \brief Returns an extended face.
 */
//=============================================================================

TopoDS_Shape GEOMImpl_ShapeDriver::ExtendFace
                         (const TopoDS_Face   &theFace,
                          const Standard_Real  theUMin,
                          const Standard_Real  theUMax,
                          const Standard_Real  theVMin,
                          const Standard_Real  theVMax) const
{
  TopoDS_Shape         aResult;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(theFace);
  const Standard_Real  aTol     = BRep_Tool::Tolerance(theFace);
  Standard_Real        aU1;
  Standard_Real        aU2;
  Standard_Real        aV1;
  Standard_Real        aV2;

  // Get U, V bounds of the face.
  ShapeAnalysis::GetFaceUVBounds(theFace, aU1, aU2, aV1, aV2);

  const Standard_Real aURange = aU2 - aU1;
  const Standard_Real aVRange = aV2 - aV1;

  if (aSurface.IsNull() == Standard_False &&
      aURange > aTol && aURange > aTol) {
    Handle(Standard_Type) aType = aSurface->DynamicType();

    // Get the surface of original type
    while (aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      Handle(Geom_RectangularTrimmedSurface) aTrSurface =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);

      aSurface = aTrSurface->BasisSurface();
      aType    = aSurface->DynamicType();
    }

    Standard_Real aUMin = aU1 + aURange*theUMin;
    Standard_Real aUMax = aU1 + aURange*theUMax;
    Standard_Real aVMin = aV1 + aVRange*theVMin;
    Standard_Real aVMax = aV1 + aVRange*theVMax;

    aSurface->Bounds(aU1, aU2, aV1, aV2);

    if (aSurface->IsUPeriodic()) {
      // The surface is U-periodic. Check if a new U range is less
      // then a period.
      if (aUMax - aUMin > aSurface->UPeriod()) {
        aUMax = aUMin + aSurface->UPeriod();
      }
    } else {
      // The surface is not V-periodic. Check if aUMin and aUMax
      // within bounds.
      aUMin = Max(aUMin, aU1);
      aUMax = Min(aUMax, aU2);
    }

    if (aSurface->IsVPeriodic()) {
      // The surface is V-periodic. Check if a new V range is less
      // then a period.
      if (aVMax - aVMin > aSurface->VPeriod()) {
        aVMax = aVMin + aSurface->VPeriod();
      }
    } else {
      // The surface is not V-periodic. Check if aVMin and aVMax
      // within bounds.
      aVMin = Max(aVMin, aV1);
      aVMax = Min(aVMax, aV2);
    }

    if (aUMax - aUMin > aTol && aVMax - aVMin > aTol) {
      // Create a new edge.
      BRepBuilderAPI_MakeFace aMF
        (aSurface, aUMin, aUMax, aVMin, aVMax, aTol);
    
      if (aMF.IsDone()) {
        aResult = aMF.Shape();
      }
    }
  }

  return aResult;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_ShapeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IShapes aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case WIRE_EDGES:
    theOperationName = "WIRE";
    AddParam( theParams, "Wires/edges", aCI.GetShapes() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    break;
  case FACE_WIRE:
    theOperationName = "FACE";
    AddParam( theParams, "Wire/edge", aCI.GetBase() );
    AddParam( theParams, "Is planar wanted", aCI.GetIsPlanar() );
    break;
  case FACE_WIRES:
    theOperationName = "FACE";
    AddParam( theParams, "Wires/edges", aCI.GetShapes() );
    AddParam( theParams, "Is planar wanted", aCI.GetIsPlanar() );
    break;
  case FACE_FROM_SURFACE:
  {
    theOperationName = "FACE";

    Handle(TColStd_HSequenceOfTransient) shapes = aCI.GetShapes();

    if (shapes.IsNull() == Standard_False) {
      Standard_Integer aNbShapes = shapes->Length();

      if (aNbShapes > 0) {
        AddParam(theParams, "Face", shapes->Value(1));

        if (aNbShapes > 1) {
          AddParam(theParams, "Wire", shapes->Value(2));
        }
      }
    }
    break;
  }
  case SHELL_FACES:
    theOperationName = "SHELL";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case SOLID_SHELLS:
    theOperationName = "SOLID";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case SOLID_FACES:
    theOperationName = "SOLID_FROM_FACES";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    AddParam( theParams, "Is intersect", aCI.GetIsIntersect() );
    break;
  case COMPOUND_SHAPES:
    theOperationName = "COMPOUND";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case EDGE_WIRE:
    theOperationName = "EDGE";
    AddParam( theParams, "Wire", aCI.GetBase() );
    AddParam( theParams, "Linear Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "Angular Tolerance", aCI.GetAngularTolerance() );
    break;
  case EDGE_CURVE_LENGTH:
    theOperationName = "EDGE";
    {
      GEOMImpl_IVector aCI( function );
      AddParam( theParams, "Edge", aCI.GetPoint1() );
      AddParam( theParams, "Start point", aCI.GetPoint2() );
      AddParam( theParams, "Length", aCI.GetParameter() );
    }
    break;
  case SHAPES_ON_SHAPE:
  {
    theOperationName = "GetShapesOnShapeAsCompound";
    Handle(TColStd_HSequenceOfTransient) shapes = aCI.GetShapes();
    if ( !shapes.IsNull() && shapes->Length() > 0 )
      AddParam( theParams, "Check shape", shapes->Value(1) );
    if ( !shapes.IsNull() && shapes->Length() > 1 )
      AddParam( theParams, "Shape", shapes->Value(2) );
    AddParam( theParams, "Shape type", TopAbs_ShapeEnum( aCI.GetSubShapeType() ));
    AddParam( theParams, "State" );
    GEOMAlgo_State st = GEOMAlgo_State( (int) ( aCI.GetTolerance()+0.1 ) );
    const char* stName[] = { "UNKNOWN","IN","OUT","ON","ONIN","ONOUT","INOUT" };
    if ( 0 <= st && st <= GEOMAlgo_ST_INOUT )
      theParams.back() << stName[ st ];
    else
      theParams.back() << (int) st;
    break;
  }
  case SHAPE_ISOLINE:
  {
    GEOMImpl_IIsoline aII (function);

    theOperationName = "ISOLINE";
    AddParam(theParams, "Face", aII.GetFace());
    AddParam(theParams, "Isoline type", (aII.GetIsUIso() ? "U" : "V"));
    AddParam(theParams, "Parameter", aII.GetParameter());
    break;
  }
  case EDGE_UV:
  {
    GEOMImpl_IShapeExtend aSE (function);

    theOperationName = "EDGE_EXTEND";
    AddParam(theParams, "Edge", aSE.GetShape());
    AddParam(theParams, "Min", aSE.GetUMin());
    AddParam(theParams, "Max", aSE.GetUMax());
    break;
  }
  case FACE_UV:
  {
    GEOMImpl_IShapeExtend aSE (function);

    theOperationName = "FACE_EXTEND";
    AddParam(theParams, "Face", aSE.GetShape());
    AddParam(theParams, "UMin", aSE.GetUMin());
    AddParam(theParams, "UMax", aSE.GetUMax());
    AddParam(theParams, "VMin", aSE.GetVMin());
    AddParam(theParams, "VMax", aSE.GetVMax());
    break;
  }
  case SURFACE_FROM_FACE:
  {
    GEOMImpl_IShapeExtend aSE (function);

    theOperationName = "SURFACE_FROM_FACE";
    AddParam(theParams, "Face", aSE.GetShape());
    break;
  }
  case EXTRACTION:
  {
    GEOMImpl_IExtract aCI (function);

    theOperationName = "EXTRACTION";
    AddParam(theParams, "Main Shape", aCI.GetShape());
    AddParam(theParams, "Sub-shape IDs", aCI.GetSubShapeIDs());
    break;
  }
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ShapeDriver,GEOM_BaseDriver);

//modified by NIZNHY-PKV Wed Dec 28 13:48:31 2011f
#include <TopoDS_Iterator.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : KeepEdgesOrder
//purpose  : 
//=======================================================================
/*
void KeepEdgesOrder(const Handle(TopTools_HSequenceOfShape)& aEdges,
                    const Handle(TopTools_HSequenceOfShape)& aWires)
{
  Standard_Integer aNbWires, aNbEdges;
  // 
  if (aEdges.IsNull()) {
    return;
  }
  //
  if (aWires.IsNull()) {
    return;
  }
  //
  aNbEdges=aEdges->Length();
  aNbWires=aWires->Length();
  if (!aNbEdges || !aNbWires) {
    return;
  }
  //-----
  Standard_Boolean bClosed;
  Standard_Integer i, j;
  TopoDS_Wire aWy;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  TopTools_MapOfOrientedShape aMEx;
  //
  for (i=1; i<=aNbWires; ++i) {
    const TopoDS_Shape& aWx=aWires->Value(i);
    //
    aMEx.Clear();
    aIt.Initialize (aWx);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aEx=aIt.Value();
      aMEx.Add(aEx);
    }
    // aWy
    aBB.MakeWire (aWy);
    for (j=1; j<=aNbEdges; ++j) {
      const TopoDS_Shape& aE=aEdges->Value(j);
      if (aMEx.Contains(aE)) {
        aBB.Add(aWy, aE);
      }
    }
    //
    bClosed=aWx.Closed();
    aWy.Closed(bClosed);
    //
    aWires->Append(aWy);
  }// for (i=1; i<=aNbWires; ++i) {
  //
  aWires->Remove(1, aNbWires);
}
*/
//modified by NIZNHY-PKV Wed Dec 28 13:48:34 2011t
