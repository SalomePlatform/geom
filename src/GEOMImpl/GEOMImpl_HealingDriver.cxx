// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_HealingDriver.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_IHealing.hxx>
#include <GEOM_Function.hxx>

#include <GEOMImpl_GlueDriver.hxx>
#include <GEOMImpl_ShapeDriver.hxx>

#include <GEOMUtils.hxx>
#include <GEOMAlgo_RemoverWebs.hxx>

#include <ShHealOper_ShapeProcess.hxx>
#include <ShHealOper_RemoveFace.hxx>
#include <ShHealOper_CloseContour.hxx>
#include <ShHealOper_RemoveInternalWires.hxx>
#include <ShHealOper_FillHoles.hxx>
#include <ShHealOper_Sewing.hxx>
#include <ShHealOper_EdgeDivide.hxx>
#include <ShHealOper_ChangeOrientation.hxx>

#include <TNaming_CopyShape.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

#include <Precision.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_NullObject.hxx>

//=======================================================================
//function :  raiseNotDoneExeption
//purpose  :  global function: forms error message and raises exeption
//=======================================================================
void raiseNotDoneExeption( const int theErrorStatus )
{
  switch ( theErrorStatus )
  {
  case ShHealOper_NotError:           StdFail_NotDone::Raise( "ShHealOper_NotError_msg" );
  case ShHealOper_InvalidParameters:  StdFail_NotDone::Raise( "ShHealOper_InvalidParameters_msg" );
  case ShHealOper_ErrorExecution:
  default:                            StdFail_NotDone::Raise( "ShHealOper_ErrorExecution_msg" );
  }
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_HealingDriver::GetID()
{
  static Standard_GUID aHealingDriver("FF1BBB61-5D14-4df2-980B-3A668264EA16");
  return aHealingDriver;
}

//=======================================================================
//function : GEOMImpl_HealingDriver
//purpose  :
//=======================================================================
GEOMImpl_HealingDriver::GEOMImpl_HealingDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_HealingDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if (aFunction.IsNull()) return 0;

  GEOMImpl_IHealing HI (aFunction);
  Standard_Integer aType = aFunction->GetType();
  Handle(GEOM_Function) anOriginalFunction = HI.GetOriginal();
  if (anOriginalFunction.IsNull()) return 0;
  TopoDS_Shape aShape, anOriginalShape = anOriginalFunction->GetValue();
  if (anOriginalShape.IsNull()) return 0;

  switch (aType)
  {
  case SHAPE_PROCESS:
    ShapeProcess(&HI, anOriginalShape, aShape);
    break;
  case SUPPRESS_FACES:
    SuppressFaces(&HI, anOriginalShape, aShape);
    break;
  case CLOSE_CONTOUR:
    CloseContour(&HI, anOriginalShape, aShape);
    break;
  case REMOVE_INT_WIRES:
    RemoveIntWires(&HI, anOriginalShape, aShape);
    break;
  case FILL_HOLES:
    RemoveHoles(&HI, anOriginalShape, aShape);
    break;
  case SEWING:
    Sew(&HI, anOriginalShape, aShape, false);
    break;
  case SEWING_NON_MANIFOLD:
    Sew(&HI, anOriginalShape, aShape, true);
    break;
  case REMOVE_INTERNAL_FACES:
    RemoveInternalFaces(anOriginalShape, aShape);
    break;
  case DIVIDE_EDGE:
    AddPointOnEdge(&HI, anOriginalShape, aShape);
    break;
  case CHANGE_ORIENTATION:
    ChangeOrientation(&HI, anOriginalShape, aShape);
    break;
  case LIMIT_TOLERANCE:
    LimitTolerance(&HI, anOriginalShape, aShape);
    break;
  case FUSE_COLLINEAR_EDGES:
    {
      Handle(TColStd_HSequenceOfTransient) aVerts = HI.GetShapes();
      FuseCollinearEdges(anOriginalShape, aVerts, aShape);
    }
    break;
  default:
    return 0;
  }

  if (aShape.IsNull())
    raiseNotDoneExeption( ShHealOper_ErrorExecution );

  aFunction->SetValue(aShape);

  log.SetTouched(Label());
  return 1;
}

//=======================================================================
//function :  ShapeProcess
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::ShapeProcess (GEOMImpl_IHealing* theHI,
                                                       const TopoDS_Shape& theOriginalShape,
                                                       TopoDS_Shape& theOutShape) const
{
  Handle(TColStd_HArray1OfExtendedString) anOperators = theHI->GetOperators();
  Handle(TColStd_HArray1OfExtendedString) aParams = theHI->GetParameters();
  Handle(TColStd_HArray1OfExtendedString) aValues = theHI->GetValues();

  if (anOperators.IsNull() || anOperators->Length() <= 0)
    return Standard_False;

  Standard_Integer nbParams = 0, nbValues = 0;
  if (!aParams.IsNull()) {
    nbParams = aParams->Length();
  }
  if (!aValues.IsNull()) {
    nbValues = aValues->Length();
  }
  if (nbParams != nbValues)
    return Standard_False;

  ShHealOper_ShapeProcess aHealer;
  TColStd_SequenceOfAsciiString anOperatorsAS, aParamsAS, aValuesAS;
  int i;
  for (i = 1; i <= anOperators->Length(); i++)
    anOperatorsAS.Append(TCollection_AsciiString(anOperators->Value(i)));

  aHealer.SetOperators(anOperatorsAS);

  for (i = 1; i <= nbParams; i++) {
    aHealer.SetParameter(TCollection_AsciiString(aParams->Value(i)),
                         TCollection_AsciiString(aValues->Value(i)));
  }

  aHealer.Perform(theOriginalShape, theOutShape);

  if (!aHealer.isDone())
    raiseNotDoneExeption( ShHealOper_NotError );

  return Standard_True;
}

//=======================================================================
//function :  SupressFaces
//purpose  :
//=======================================================================
void SuppressFacesRec (const TopTools_SequenceOfShape& theShapesFaces,
                       const TopoDS_Shape&             theOriginalShape,
                       TopoDS_Shape&                   theOutShape)
{
  if ((theOriginalShape.ShapeType() != TopAbs_COMPOUND &&
       theOriginalShape.ShapeType() != TopAbs_COMPSOLID))
  {
    ShHealOper_RemoveFace aHealer (theOriginalShape);
    Standard_Boolean aResult = aHealer.Perform(theShapesFaces);

    if (aResult)
      theOutShape = aHealer.GetResultShape();
    else
      raiseNotDoneExeption(aHealer.GetErrorStatus());
  }
  else
  {
    BRep_Builder BB;
    TopoDS_Compound CC;
    BB.MakeCompound(CC);

    TopTools_MapOfShape mapShape;
    TopoDS_Iterator It (theOriginalShape, Standard_True, Standard_True);

    for (; It.More(); It.Next()) {
      TopoDS_Shape aShape_i = It.Value();
      if (mapShape.Add(aShape_i)) {
        // check, if current shape contains at least one of faces to be removed
        bool isFound = false;
        TopTools_IndexedMapOfShape aShapes_i;
        TopExp::MapShapes(aShape_i, aShapes_i);
        for (int i = 1; i <= theShapesFaces.Length() && !isFound; i++) {
          const TopoDS_Shape& aFace_i = theShapesFaces.Value(i);
          if (aShapes_i.Contains(aFace_i)) isFound = true;
        }
        if (isFound) {
          TopoDS_Shape anOutSh_i;
          SuppressFacesRec(theShapesFaces, aShape_i, anOutSh_i);
          if ( !anOutSh_i.IsNull() )
            BB.Add(CC, anOutSh_i);
        }
        else {
          // nothing to do
          BB.Add(CC, aShape_i);
        }
      }
    }
    theOutShape = CC;
  }
}

Standard_Boolean GEOMImpl_HealingDriver::SuppressFaces (GEOMImpl_IHealing* theHI,
                                                        const TopoDS_Shape& theOriginalShape,
                                                        TopoDS_Shape& theOutShape) const
{
  Handle(TColStd_HArray1OfInteger) aFaces = theHI->GetFaces();

  Standard_Boolean aResult = Standard_False;

  if (aFaces.IsNull()) {
    ShHealOper_RemoveFace aHealer (theOriginalShape);
    aResult = aHealer.Perform();

    if (aResult)
      theOutShape = aHealer.GetResultShape();
    else
      raiseNotDoneExeption(aHealer.GetErrorStatus());
  }
  else {
    TopTools_SequenceOfShape aShapesFaces;
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    for (int i = 1; i <= aFaces->Length(); i++) {
      int indexOfFace = aFaces->Value(i);
      TopoDS_Shape aFace = aShapes.FindKey(indexOfFace);
      aShapesFaces.Append(aFace);
    }
    SuppressFacesRec(aShapesFaces, theOriginalShape, theOutShape);
    if ((theOriginalShape.ShapeType() == TopAbs_COMPOUND ||
         theOriginalShape.ShapeType() == TopAbs_COMPSOLID)) {
      TopoDS_Shape aSh = theOutShape;
      theOutShape = GEOMImpl_GlueDriver::GlueFaces(aSh, Precision::Confusion(), Standard_True);
    }
  }

  return Standard_True;
}

//=======================================================================
//function :  CloseContour
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::CloseContour (GEOMImpl_IHealing* theHI,
                                                       const TopoDS_Shape& theOriginalShape,
                                                       TopoDS_Shape& theOutShape) const
{
  Standard_Boolean isByVertex = theHI->GetIsCommonVertex();
  Handle(TColStd_HArray1OfInteger) aWires = theHI->GetWires();

  ShHealOper_CloseContour aHealer (theOriginalShape);

  Standard_Boolean aResult = Standard_False;
  if ( aWires.IsNull() ) {
    if ( theOriginalShape.ShapeType() == TopAbs_WIRE )
      aResult = aHealer.Perform(TopoDS::Wire(theOriginalShape), isByVertex, !isByVertex);
  }
  else {
    TopTools_SequenceOfShape aShapesWires;
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    for (int i = 1; i <= aWires->Length(); i++) {
      int indexOfWire = aWires->Value(i);
      TopoDS_Shape aWire = aShapes.FindKey(indexOfWire);
      aShapesWires.Append(aWire);
    }

    aResult = aHealer.Perform( aShapesWires, isByVertex, !isByVertex );
  }

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}

//=======================================================================
//function :  RemoveIntWires
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::RemoveIntWires (GEOMImpl_IHealing* theHI,
                                                         const TopoDS_Shape& theOriginalShape,
                                                         TopoDS_Shape& theOutShape) const
{
  Handle(TColStd_HArray1OfInteger) aWires = theHI->GetWires();

  ShHealOper_RemoveInternalWires aHealer(theOriginalShape);

  Standard_Boolean aResult = Standard_False;
  if (aWires.IsNull()) { // remove all faces
    aResult = aHealer.Remove();
  } else {
    TopTools_SequenceOfShape aShapesWires;
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    for (int i = 1; i <= aWires->Length(); i++) {
      int indexOfWire = aWires->Value(i);
      TopoDS_Shape aWire = aShapes.FindKey(indexOfWire);
      aShapesWires.Append(aWire);
    }

    aResult = aHealer.Remove(aShapesWires);
  }

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}

//=======================================================================
//function :  RemoveHoles
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::RemoveHoles (GEOMImpl_IHealing* theHI,
                                                      const TopoDS_Shape& theOriginalShape,
                                                      TopoDS_Shape& theOutShape) const
{
  Handle(TColStd_HArray1OfInteger) aWires = theHI->GetWires();

  ShHealOper_FillHoles aHealer (theOriginalShape);

  Standard_Boolean aResult = Standard_False;
  if (aWires.IsNull()) { // remove all faces
    aResult = aHealer.Fill();
  } else {
    TopTools_SequenceOfShape aShapesWires;
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    for (int i = 1; i <= aWires->Length(); i++) {
      int indexOfWire = aWires->Value(i);
      TopoDS_Shape aWire = aShapes.FindKey(indexOfWire);
      aShapesWires.Append(aWire);
    }

    aResult = aHealer.Fill(aShapesWires);
  }

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}

//=======================================================================
//function :  Sew
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::Sew (GEOMImpl_IHealing* theHI,
                                              const TopoDS_Shape& theOriginalShape,
                                              TopoDS_Shape& theOutShape,
                                              Standard_Boolean isAllowNonManifold) const
{
  Standard_Real aTol = theHI->GetTolerance();

  ShHealOper_Sewing aHealer (theOriginalShape, aTol);

  // Set non-manifold mode.
  aHealer.SetNonManifoldMode(isAllowNonManifold);

  Standard_Boolean aResult = aHealer.Perform();

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}

//=======================================================================
//function : RemoveInternalFaces
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::RemoveInternalFaces (const TopoDS_Shape& theOriginalShape,
                                                              TopoDS_Shape& theOutShape) const
{
  GEOMAlgo_RemoverWebs aTool;
  aTool.SetShape(theOriginalShape);
  aTool.Perform();

  if (aTool.ErrorStatus() != 0)
    StdFail_NotDone::Raise("GEOMAlgo_RemoverWebs failed!");

  theOutShape = aTool.Result();

  // as GEOMAlgo_RemoverWebs always produces compound, lets simplify it
  // for the case, if it contains only one sub-shape
  TopTools_ListOfShape listShapeRes;
  GEOMUtils::AddSimpleShapes(theOutShape, listShapeRes);
  if (listShapeRes.Extent() == 1) {
    theOutShape = listShapeRes.First();
  }

  return Standard_True;
}

//=======================================================================
//function :  AddPointOnEdge
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::AddPointOnEdge (GEOMImpl_IHealing* theHI,
                                                         const TopoDS_Shape& theOriginalShape,
                                                         TopoDS_Shape& theOutShape) const
{
  Standard_Boolean isByParameter = theHI->GetIsByParameter();
  Standard_Integer anIndex = theHI->GetIndex();
  Standard_Real aValue = theHI->GetDevideEdgeValue();

  ShHealOper_EdgeDivide aHealer (theOriginalShape);

  Standard_Boolean aResult = Standard_False;
  if (anIndex == -1) { // apply algorythm for the whole shape which is EDGE
    if (theOriginalShape.ShapeType() == TopAbs_EDGE)
      aResult = aHealer.Perform(TopoDS::Edge(theOriginalShape), aValue, isByParameter);
  } else {
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    TopoDS_Shape aEdgeShape = aShapes.FindKey(anIndex);
    if (aEdgeShape.ShapeType() == TopAbs_EDGE)
      aResult = aHealer.Perform(TopoDS::Edge(aEdgeShape), aValue, isByParameter);
  }

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}


//=======================================================================
//function :  ChangeOrientation
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::ChangeOrientation (GEOMImpl_IHealing* theHI,
                                                            const TopoDS_Shape& theOriginalShape,
                                                            TopoDS_Shape& theOutShape) const
{
  ShHealOper_ChangeOrientation aHealer (theOriginalShape);

  Standard_Boolean aResult = aHealer.Perform();

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  return aResult;
}

//=======================================================================
//function : LimitTolerance
//purpose  :
//=======================================================================
void GEOMImpl_HealingDriver::LimitTolerance (GEOMImpl_IHealing* theHI,
                                             const TopoDS_Shape& theOriginalShape,
                                             TopoDS_Shape& theOutShape) const
{
  Standard_Real aTol = theHI->GetTolerance();
  if (aTol < Precision::Confusion())
    aTol = Precision::Confusion();

  // 1. Make a copy to prevent the original shape changes.
  TopoDS_Shape aShapeCopy;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
  TNaming_CopyShape::CopyTool(theOriginalShape, aMapTShapes, aShapeCopy);

  // 2. Limit tolerance.
  ShapeFix_ShapeTolerance aSFT;
  aSFT.LimitTolerance(aShapeCopy, aTol, aTol, TopAbs_SHAPE);

  // 3. Fix obtained shape.
  Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape (aShapeCopy);
  aSfs->Perform();
  theOutShape = aSfs->Shape();

  BRepCheck_Analyzer ana (theOutShape, Standard_True);
  if (!ana.IsValid())
    StdFail_NotDone::Raise("Non valid shape result");
}

//=======================================================================
//function : FuseCollinearEdges
//purpose  :
//=======================================================================
void GEOMImpl_HealingDriver::FuseCollinearEdges (const TopoDS_Shape& theOriginalShape,
                                                 const Handle(TColStd_HSequenceOfTransient)& aVerts,
                                                 TopoDS_Shape& theOutShape)
{
  if (theOriginalShape.ShapeType() != TopAbs_WIRE)
    Standard_TypeMismatch::Raise("Not a wire is given");

  // Tolerances
  Standard_Real AngTol = Precision::Angular();
  Standard_Real LinTol = Precision::Confusion();
  Standard_Real tol;
  for (TopExp_Explorer ExV (theOriginalShape, TopAbs_VERTEX); ExV.More(); ExV.Next()) {
    TopoDS_Vertex Vertex = TopoDS::Vertex(ExV.Current());
    tol = BRep_Tool::Tolerance(Vertex);
    if (tol > LinTol)
      LinTol = tol;
  }

  // 1. Make a copy to prevent the original shape changes.
  TopoDS_Shape aWire;
  TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
  TNaming_CopyShape::CopyTool(theOriginalShape, aMapTShapes, aWire);
  TopoDS_Wire theWire = TopoDS::Wire(aWire);

  // 2. Sub-shapes of the wire
  TopTools_MapOfShape aMapToRemove;

  TopTools_IndexedMapOfShape anOldIndices;
  TopExp::MapShapes(theOriginalShape, anOldIndices);

  TopTools_IndexedMapOfShape aNewIndices;
  TopExp::MapShapes(theWire, aNewIndices);

  // 3. Collect vertices of the wire, same or equal to the given vertices
  bool removeAll = false;
  if (aVerts.IsNull() || aVerts->Length() < 1)
    removeAll = true;

  if (!removeAll) {
    for (unsigned int ind = 1; ind <= aVerts->Length(); ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aVerts->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull())
        Standard_NullObject::Raise("Null vertex given");
      if (aShape_i.ShapeType() != TopAbs_VERTEX)
        Standard_TypeMismatch::Raise("Shape to suppress is not a vertex");

      // find vertices shared with the initial wire
      if (anOldIndices.Contains(aShape_i)) {
        aMapToRemove.Add(aNewIndices.FindKey(anOldIndices.FindIndex(aShape_i)));
      } else {
        // try to find by coords in the new wire
        TopoDS_Vertex aVert = TopoDS::Vertex(aShape_i);
        gp_Pnt aP = BRep_Tool::Pnt(aVert);

        bool isFound = false;
        TopTools_MapOfShape mapShape;
        TopExp_Explorer exp (theWire, TopAbs_VERTEX);
        for (; exp.More() && !isFound; exp.Next()) {
          if (mapShape.Add(exp.Current())) {
            TopoDS_Vertex aVi = TopoDS::Vertex(exp.Current());
            gp_Pnt aPi = BRep_Tool::Pnt(aVi);
            if (aPi.Distance(aP) < LinTol) {
              aMapToRemove.Add(aVi);
              isFound = true;
            }
          }
        }
      }
    }
  }

  /*
  BRepLib::BuildCurves3d(theWire);
  Handle(ShapeFix_Shape) Fixer = new ShapeFix_Shape(theWire);
  Fixer->SetPrecision(LinTol);
  Fixer->SetMaxTolerance(LinTol);
  Fixer->Perform();
  theWire = TopoDS::Wire(Fixer->Shape());
  */

  TopoDS_Edge prevEdge;
  TopTools_ListOfShape finalList, currChain;

  BRepTools_WireExplorer wexp (theWire);
  if (wexp.More()) {
    prevEdge = wexp.Current();
    currChain.Append(prevEdge);
    wexp.Next();
  }
  else {
    Standard_NullObject::Raise("Empty wire given");
  }

  for (; wexp.More(); wexp.Next()) {
    TopoDS_Edge anEdge = wexp.Current();
    TopoDS_Vertex CurVertex = wexp.CurrentVertex();

    bool continueChain = false;
    if (aMapToRemove.Contains(CurVertex) || removeAll) {
      // if C1 -> continue chain
      if (AreEdgesC1(prevEdge, anEdge)) {
        continueChain = true;
      }
    }

    if (!continueChain) {
      if (currChain.Extent() == 1) {
        // add one edge to the final list
        finalList.Append(currChain.First());
      }
      else {
        // make wire from the list of edges
        BRep_Builder B;
        TopoDS_Wire aCurrWire;
        B.MakeWire(aCurrWire);
        TopTools_ListIteratorOfListOfShape itEdges (currChain);
        for (; itEdges.More(); itEdges.Next()) {
          TopoDS_Shape aValue = itEdges.Value();
          B.Add(aCurrWire, TopoDS::Edge(aValue));
        }

        // make edge from the wire
        TopoDS_Edge anEdge = GEOMImpl_ShapeDriver::MakeEdgeFromWire(aCurrWire, LinTol, AngTol);

        // add this new edge to the final list
        finalList.Append(anEdge);
      }
      currChain.Clear();
    }

    // add one edge to the chain
    currChain.Append(anEdge);
    prevEdge = anEdge;
  }

  if (currChain.Extent() == 1) {
    // add one edge to the final list
    finalList.Append(currChain.First());
  }
  else {
    // make wire from the list of edges
    BRep_Builder B;
    TopoDS_Wire aCurrWire;
    B.MakeWire(aCurrWire);
    TopTools_ListIteratorOfListOfShape itEdges (currChain);
    for (; itEdges.More(); itEdges.Next()) {
      TopoDS_Shape aValue = itEdges.Value();
      B.Add(aCurrWire, TopoDS::Edge(aValue));
    }

    // make edge from the wire
    TopoDS_Edge anEdge = GEOMImpl_ShapeDriver::MakeEdgeFromWire(aCurrWire, LinTol, AngTol);

    // add this new edge to the final list
    finalList.Append(anEdge);
  }

  BRep_Builder B;
  TopoDS_Wire aFinalWire;
  B.MakeWire(aFinalWire);
  TopTools_ListIteratorOfListOfShape itEdges (finalList);
  for (; itEdges.More(); itEdges.Next()) {
    TopoDS_Shape aValue = itEdges.Value();
    B.Add(aFinalWire, TopoDS::Edge(aValue));
  }
  theOutShape = aFinalWire;

  BRepCheck_Analyzer ana (theOutShape, Standard_True);
  if (!ana.IsValid())
    StdFail_NotDone::Raise("Non valid shape result");
}

//=======================================================================
//function : AreEdgesC1
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::AreEdgesC1 (const TopoDS_Edge& E1, const TopoDS_Edge& E2)
{
  BRepAdaptor_Curve aCurve1 (E1);
  BRepAdaptor_Curve aCurve2 (E2);

  if (aCurve1.Continuity() == GeomAbs_C0 || aCurve2.Continuity() == GeomAbs_C0)
    return Standard_False;

  Standard_Real tol, tolMax = Precision::Confusion();
  for (TopExp_Explorer ExV1 (E1, TopAbs_VERTEX); ExV1.More(); ExV1.Next()) {
    TopoDS_Vertex Vertex = TopoDS::Vertex(ExV1.Current());
    tol = BRep_Tool::Tolerance(Vertex);
      if (tol > tolMax)
        tolMax = tol;
  }
  for (TopExp_Explorer ExV2 (E2, TopAbs_VERTEX); ExV2.More(); ExV2.Next()) {
    TopoDS_Vertex Vertex = TopoDS::Vertex(ExV2.Current());
    tol = BRep_Tool::Tolerance(Vertex);
      if (tol > tolMax)
        tolMax = tol;
  }

  Standard_Real f1, l1, f2, l2;
  f1 = aCurve1.FirstParameter();
  l1 = aCurve1.LastParameter();
  f2 = aCurve2.FirstParameter();
  l2 = aCurve2.LastParameter();

  if (f1 > l1) {
    Standard_Real tmp = f1;
    f1 = l1;
    l1 = tmp;
  }

  if (f2 > l2) {
    Standard_Real tmp = f2;
    f2 = l2;
    l2 = tmp;
  }

  gp_Pnt pf1, pl1, pf2, pl2;
  gp_Vec vf1, vl1, vf2, vl2;
  aCurve1.D1(f1, pf1, vf1);
  aCurve1.D1(l1, pl1, vl1);
  aCurve2.D1(f2, pf2, vf2);
  aCurve2.D1(l2, pl2, vl2);

  // pf1--->---pl1.pf2--->---pl2
  if (pl1.SquareDistance(pf2) < tolMax*tolMax) {
    if (vl1.Angle(vf2) < Precision::Angular())
      return Standard_True;
  }
  // pl1---<---pf1.pf2--->---pl2
  else if (pf1.SquareDistance(pf2) < tolMax*tolMax) {
    if (vf1.Angle(-vf2) < Precision::Angular())
      return Standard_True;
  }
  // pf1--->---pl1.pl2---<---pf2
  else if (pl1.SquareDistance(pl2) < tolMax*tolMax) {
    if (vl1.Angle(-vl2) < Precision::Angular())
      return Standard_True;
  }
  // pl1---<---pf1.pl2---<---pf2
  else {
    if (vf1.Angle(vl2) < Precision::Angular())
      return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function :  GEOMImpl_HealingDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_HealingDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_HealingDriver",
                                                         sizeof(GEOMImpl_HealingDriver),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================

const Handle(GEOMImpl_HealingDriver) Handle(GEOMImpl_HealingDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_HealingDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_HealingDriver))) {
       _anOtherObject = Handle(GEOMImpl_HealingDriver)((Handle(GEOMImpl_HealingDriver)&)AnObject);
     }
  }

  return _anOtherObject;
}
