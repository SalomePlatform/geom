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

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
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
Standard_Integer GEOMImpl_HealingDriver::Execute(LOGBOOK& log) const
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
    RemoveInternalFaces(&HI, anOriginalShape, aShape);
    break;
  case DIVIDE_EDGE:
  case DIVIDE_EDGE_BY_POINT:
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

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif
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

  SaveStatistics( aHealer );

  return Standard_True;
}

//=======================================================================
//function :  SupressFaces
//purpose  :
//=======================================================================
void GEOMImpl_HealingDriver::SuppressFacesRec (const TopTools_SequenceOfShape& theShapesFaces,
                                               const TopoDS_Shape&             theOriginalShape,
                                               TopoDS_Shape&                   theOutShape) const
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
  // count removed faces
  TopTools_IndexedMapOfShape faces;
  TopExp::MapShapes(theOriginalShape, TopAbs_FACE, faces);
  int nbBefore = faces.Extent();
  faces.Clear();
  TopExp::MapShapes(theOutShape, TopAbs_FACE, faces);
  int nbAfter = faces.Extent();

  if ( nbAfter < nbBefore )
  {
    ShHealOper_Tool tool;
    ShHealOper_ModifStats& stats = tool.GetStatistics();
    stats.AddModif( "Face removed", nbBefore - nbAfter );
    SaveStatistics( tool );
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

  SaveStatistics( aHealer );

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

  SaveStatistics( aHealer );

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

  SaveStatistics( aHealer );

  return aResult;
}

//=======================================================================
//function :  Sew
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::Sew (GEOMImpl_IHealing*  theHI,
                                              const TopoDS_Shape& theOriginalShape,
                                              TopoDS_Shape&       theOutShape,
                                              Standard_Boolean    isAllowNonManifold) const
{
  Standard_Real aTol = theHI->GetTolerance();

  TopoDS_Compound aCompound;
  BRep_Builder builder;
  builder.MakeCompound( aCompound );

  builder.Add( aCompound, theOriginalShape );
  Handle(TColStd_HSequenceOfTransient) otherObjs = theHI->GetShapes();
  for ( int ind = 1; ind <= otherObjs->Length(); ind++)
  {
    Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(otherObjs->Value(ind));
    builder.Add( aCompound, aRefShape->GetValue() );
  }

  ShHealOper_Sewing aHealer (aCompound, aTol);

  // Set non-manifold mode.
  aHealer.SetNonManifoldMode(isAllowNonManifold);

  Standard_Boolean aResult = aHealer.Perform();

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  SaveStatistics( aHealer );

  return aResult;
}

//=======================================================================
//function : RemoveInternalFaces
//purpose  :
//=======================================================================
Standard_Boolean
GEOMImpl_HealingDriver::RemoveInternalFaces (GEOMImpl_IHealing*  theHI,
                                             const TopoDS_Shape& theOriginalShape,
                                             TopoDS_Shape&       theOutShape) const
{
  // get all input shapes
  TopTools_SequenceOfShape shapeSeq;
  shapeSeq.Append( theOriginalShape );
  Handle(TColStd_HSequenceOfTransient) otherObjs = theHI->GetShapes();
  if ( !otherObjs.IsNull() )
    for ( int ind = 1; ind <= otherObjs->Length(); ind++)
    {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(otherObjs->Value(ind));
      TopoDS_Shape aShape = aRefShape->GetValue();
      if (aShape.IsNull())
        Standard_NullObject::Raise("Null object given");
      shapeSeq.Append( aShape );
    }

  // pass input shapes to the algorithm
  GEOMAlgo_RemoverWebs aTool;
  if ( shapeSeq.Length() == 1 )
  {
    aTool.SetShape( shapeSeq.First() );
  }
  else
  {
    TopoDS_Compound solidCompound;
    BRep_Builder builder;
    builder.MakeCompound( solidCompound );
    for ( int ind = 1; ind <= shapeSeq.Length(); ++ind )
      for ( TopExp_Explorer so( shapeSeq( ind ), TopAbs_SOLID ); so.More(); so.Next() )
        builder.Add( solidCompound, so.Current() );

    aTool.SetShape( solidCompound );
  }

  // run the algorithm
  aTool.Perform();

  if (aTool.ErrorStatus() == 0) { // OK
    theOutShape = aTool.Result();

    // as GEOMAlgo_RemoverWebs always produces compound, lets simplify it
    // for the case, if it contains only one sub-shape
    TopTools_ListOfShape listShapeRes;
    GEOMUtils::AddSimpleShapes(theOutShape, listShapeRes);
    if (listShapeRes.Extent() == 1) {
      theOutShape = listShapeRes.First();
    }
  }
  else if (aTool.ErrorStatus() == 11) // invalid argument (contains non-solids), do nothing
    theOutShape = theOriginalShape;
  else // error
    StdFail_NotDone::Raise("GEOMAlgo_RemoverWebs failed!");

  return Standard_True;
}

//=======================================================================
//function :  AddPointOnEdge
//purpose  :
//=======================================================================
Standard_Boolean GEOMImpl_HealingDriver::AddPointOnEdge (GEOMImpl_IHealing*  theHI,
                                                         const TopoDS_Shape& theOriginalShape,
                                                         TopoDS_Shape&       theOutShape) const
{
  Standard_Boolean isByParameter = theHI->GetIsByParameter();
  Standard_Integer       anIndex = theHI->GetIndex();
  Standard_Real           aValue = theHI->GetDevideEdgeValue();

  TopoDS_Shape pointToProject;
  {
    Handle(TColStd_HSequenceOfTransient) funs = theHI->GetShapes();
    if ( !funs.IsNull() && funs->Length() > 0 ) {
      TopoDS_Compound vCompound;
      BRep_Builder builder;
      builder.MakeCompound( vCompound );
      pointToProject = vCompound;
      for ( int ind = 1; ind <= funs->Length(); ind++)
      {
        Handle(GEOM_Function) vFun = Handle(GEOM_Function)::DownCast(funs->Value(ind));
        TopoDS_Shape vertex = vFun->GetValue();
        if ( vertex.IsNull() )
          Standard_NullObject::Raise("Null vertex given");
        builder.Add( vCompound, vertex );
      }
    }
  }

  ShHealOper_EdgeDivide aHealer (theOriginalShape);

  Standard_Boolean aResult = Standard_False;
  if (anIndex == -1) { // apply algorithm for the whole shape which is EDGE
    if (theOriginalShape.ShapeType() == TopAbs_EDGE) {
      if ( pointToProject.IsNull() )
        aResult = aHealer.Perform(TopoDS::Edge(theOriginalShape), aValue, isByParameter);
      else
        aResult = aHealer.Perform(TopoDS::Edge(theOriginalShape), pointToProject);
    }
  } else {
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(theOriginalShape, aShapes);
    TopoDS_Shape aEdgeShape = aShapes.FindKey(anIndex);
    if (aEdgeShape.ShapeType() == TopAbs_EDGE) {
      if ( pointToProject.IsNull() )
        aResult = aHealer.Perform(TopoDS::Edge(aEdgeShape), aValue, isByParameter);
      else
        aResult = aHealer.Perform(TopoDS::Edge(aEdgeShape), pointToProject);
    }
  }

  if (aResult)
    theOutShape = aHealer.GetResultShape();
  else
    raiseNotDoneExeption( aHealer.GetErrorStatus() );

  SaveStatistics( aHealer );

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

  SaveStatistics( aHealer );

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
  TopAbs_ShapeEnum aType = theHI->GetType();

  if (aTol < Precision::Confusion())
    aTol = Precision::Confusion();

  // 1. Make a copy to prevent the original shape changes.
  TopoDS_Shape aShapeCopy;
  {
    TColStd_IndexedDataMapOfTransientTransient aMapTShapes;
    TNaming_CopyShape::CopyTool(theOriginalShape, aMapTShapes, aShapeCopy);
  }

  // 2. Limit tolerance.
  if (!GEOMUtils::FixShapeTolerance(aShapeCopy, aType, aTol))
    StdFail_NotDone::Raise("Non valid shape result");

  // 3. Set the result
  theOutShape = aShapeCopy;

  // 4. Collect statistics
  {
    ShHealOper_Tool tool;
    ShHealOper_ModifStats& stats = tool.GetStatistics();

    int nb[3] = { 0,0,0 };
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes( theOutShape, TopAbs_VERTEX, aShapes);
    for ( int i = 1; i <= aShapes.Extent(); ++i )
    {
      const TopoDS_Vertex& v = TopoDS::Vertex( aShapes( i ));
      double tol = BRep_Tool::Tolerance( v );
      if      ( tol < aTol ) nb[0]++;
      else if ( tol > aTol ) nb[2]++;
      else                   nb[1]++;
    }
    if ( nb[0] > 0 )
      stats.AddModif( "Tolerance of vertex decreased for shape validity", nb[0] );
    if ( nb[1] > 0 )
      stats.AddModif( "Tolerance of vertex limited as requested", nb[1] );
    if ( nb[2] > 0 )
      stats.AddModif( "Tolerance of vertex increased for shape validity", nb[2] );

    nb[0] = nb[1] = nb[2] = 0;
    aShapes.Clear();
    TopExp::MapShapes( theOutShape, TopAbs_EDGE, aShapes);
    for ( int i = 1; i <= aShapes.Extent(); ++i )
    {
      const TopoDS_Edge& e = TopoDS::Edge( aShapes( i ));
      double tol = BRep_Tool::Tolerance( e );
      if      ( tol < aTol ) nb[0]++;
      else if ( tol > aTol ) nb[2]++;
      else                   nb[1]++;
    }
    if ( nb[0] > 0 )
      stats.AddModif( "Tolerance of edge decreased for shape validity", nb[0] );
    if ( nb[1] > 0 )
      stats.AddModif( "Tolerance of edge limited as requested", nb[1] );
    if ( nb[2] > 0 )
      stats.AddModif( "Tolerance of edge increased for shape validity", nb[2] );

    nb[0] = nb[1] = nb[2] = 0;
    aShapes.Clear();
    TopExp::MapShapes( theOutShape, TopAbs_FACE, aShapes);
    for ( int i = 1; i <= aShapes.Extent(); ++i )
    {
      const TopoDS_Face& f = TopoDS::Face( aShapes( i ));
      double tol = BRep_Tool::Tolerance( f );
      if      ( tol < aTol ) nb[0]++;
      else if ( tol > aTol ) nb[2]++;
      else                   nb[1]++;
    }
    if ( nb[0] > 0 )
      stats.AddModif( "Tolerance of face decreased for shape validity", nb[0] );
    if ( nb[1] > 0 )
      stats.AddModif( "Tolerance of face limited as requested", nb[1] );
    if ( nb[2] > 0 )
      stats.AddModif( "Tolerance of face increased for shape validity", nb[2] );

    SaveStatistics( tool );
  }
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
    for ( int ind = 1; ind <= aVerts->Length(); ind++) {
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

  // Get the ordered list of edges.
  TopTools_ListOfShape   anEdges;
  TopTools_ListOfShape   aCurVertices;
  BRepTools_WireExplorer aWExp (theWire);

  for (; aWExp.More(); aWExp.Next()) {
    anEdges.Append(aWExp.Current());
    aCurVertices.Append(aWExp.CurrentVertex());
  }

  if (anEdges.IsEmpty()) {
    Standard_NullObject::Raise("Empty wire given");
  }

  // Treat the case if the wire is closed and first and last edges are C1.
  Standard_Boolean isShift = Standard_False;

  if (BRep_Tool::IsClosed(theWire)) {
    // Wire is closed. Check if there are more than 2 edges in the wire.
    if (!anEdges.First().IsSame(anEdges.Last())) {
      isShift = Standard_True;
    }
  }

  if (isShift) {
    // Put first edge to the end of the list while the chain break is reached.
    TopoDS_Shape aFirstEdge = anEdges.First();

    while (isShift) {
      isShift = Standard_False;

      // Check if the first vertex should be kept
      if (aMapToRemove.Contains(aCurVertices.First()) || removeAll) {
        // Check if first and last edges are C1.
        TopoDS_Edge anEdge1 = TopoDS::Edge(anEdges.Last());
        TopoDS_Edge anEdge2 = TopoDS::Edge(anEdges.First());

        if (AreEdgesC1(anEdge1, anEdge2)) {
          // Make the first edge last.
          anEdges.Append(anEdge2);
          anEdges.RemoveFirst();
          aCurVertices.Append(aCurVertices.First());
          aCurVertices.RemoveFirst();

          // Check if we reached the first edge again.
          // Break the loop in this case.
          isShift = !aFirstEdge.IsSame(anEdges.First());
        }
      }
    }
  }

  TopTools_ListOfShape finalList, currChain;
  TopTools_ListIteratorOfListOfShape anEIter(anEdges);
  TopTools_ListIteratorOfListOfShape aVIter(aCurVertices);
  TopoDS_Edge                        prevEdge = TopoDS::Edge(anEIter.Value());

  currChain.Append(prevEdge);
  anEIter.Next();
  aVIter.Next();

  for (; anEIter.More(); anEIter.Next(), aVIter.Next()) {
    TopoDS_Edge         anEdge     = TopoDS::Edge(anEIter.Value());
    const TopoDS_Shape &aCurVertex = aVIter.Value();

    bool continueChain = false;
    if (aMapToRemove.Contains(aCurVertex) || removeAll) {
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

  if (!GEOMUtils::CheckShape(theOutShape, true))
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

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_HealingDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IHealing aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case SHAPE_PROCESS:
  {
    theOperationName = "SHAPE_PROCESS";
    AddParam( theParams, "Object", aCI.GetOriginal() );
    Handle(TColStd_HArray1OfExtendedString) anOperators = aCI.GetOperators();
    Handle(TColStd_HArray1OfExtendedString) aParams     = aCI.GetParameters();
    Handle(TColStd_HArray1OfExtendedString) aValues     = aCI.GetValues();
    for ( int i = anOperators->Lower(), nb = anOperators->Upper(); i <= nb; ++i )
    {
      const TCollection_ExtendedString& op = anOperators->Value(i);
      AddParam( theParams, "Operation", op );
      for ( int iP = aParams->Lower(), nbP = aParams->Upper(); iP <= nbP; ++iP )
      {
        const TCollection_ExtendedString& par = aParams->Value(iP);
        TCollection_AsciiString parAscii( par );
        if ( par.Search( op ) == 1 && parAscii.Value( op.Length() + 1 ) == '.' )
        {
          GEOM_Param& p = AddParam( theParams, parAscii.ToCString() );
          if ( iP <= aValues->Upper() )
            p << aValues->Value( iP );
        }
      }
    }
    break;
  }
  case SUPPRESS_FACES:
    theOperationName = "SUPPRESS_FACES";
    AddParam( theParams, "Selected Shape", aCI.GetOriginal() );
    AddParam( theParams, "Faces to remove", aCI.GetFaces() );
    break;
  case CLOSE_CONTOUR:
    theOperationName = "CLOSE_CONTOUR";
    AddParam( theParams, "Selected Shape", aCI.GetOriginal() );
    AddParam( theParams, "Contour to close", aCI.GetWires() );
    AddParam( theParams, "Close by common vertex", aCI.GetIsCommonVertex() );
    break;
  case REMOVE_INT_WIRES:
    theOperationName = "SUPPRESS_INT_WIRES";
    AddParam( theParams, "Selected face", aCI.GetOriginal() );
    AddParam( theParams, "Wires to remove", aCI.GetWires(), "all" );
    break;
  case FILL_HOLES:
    theOperationName = "SUPPERSS_HOLES";
    AddParam( theParams, "Selected shape", aCI.GetOriginal() );
    AddParam( theParams, "Wires to remove", aCI.GetWires(), "all" );
    break;
  case SEWING:
  case SEWING_NON_MANIFOLD:
    theOperationName = "SEWING";
    AddParam( theParams, "Selected shapes", aCI.GetOriginalAndShapes() );
    AddParam( theParams, "Allow Non Manifold", ( aType == SEWING_NON_MANIFOLD ));
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    break;
  case DIVIDE_EDGE:
    theOperationName = "POINT_ON_EDGE";
    if ( aCI.GetIndex() > 0 )
      AddParam( theParams, "Edge", "#" ) << aCI.GetIndex() << " of " << aCI.GetOriginal();
    else
      AddParam( theParams, "Edge", aCI.GetOriginal() );
    AddParam( theParams, "By parameter", aCI.GetIsByParameter() );
    AddParam( theParams, "Value", aCI.GetDevideEdgeValue() );
    break;
  case DIVIDE_EDGE_BY_POINT:
    theOperationName = "POINT_ON_EDGE";
    if ( aCI.GetIndex() > 0 )
      AddParam( theParams, "Edge", "#" ) << aCI.GetIndex() << " of " << aCI.GetOriginal();
    else
      AddParam( theParams, "Edge", aCI.GetOriginal() );
    AddParam( theParams, "Points", aCI.GetShapes() );
    break;
  case CHANGE_ORIENTATION:
    theOperationName = "CHANGE_ORIENTATION";
    AddParam( theParams, "Selected shape", aCI.GetOriginal() );
    break;
  case LIMIT_TOLERANCE:
    theOperationName = "LIMIT_TOLERANCE";
    AddParam( theParams, "Selected shape", aCI.GetOriginal() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "Type", aCI.GetType() );
    break;
  case FUSE_COLLINEAR_EDGES:
    theOperationName = "FUSE_EDGES";
    AddParam( theParams, "Wire", aCI.GetOriginal() );
    AddParam( theParams, "Vertexes", aCI.GetShapes() );
    break;
  case REMOVE_INTERNAL_FACES:
    theOperationName = "REMOVE_WEBS";
    AddParam( theParams, "Selected shapes", aCI.GetOriginalAndShapes() );
    break;
  default:
    return false;
  }
  
  return true;
}

//================================================================================
/*!
 * \brief Pass a record of what is done to the operation
 */
//================================================================================

void GEOMImpl_HealingDriver::SaveStatistics( const ShHealOper_Tool& healer, bool add ) const
{
  if ( healer.GetStatistics().GetData().empty() )
    return;

  if (Label().IsNull()) return;

  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  if (aFunction.IsNull()) return;

  GEOMImpl_IHealing HI (aFunction);
  ShHealOper_ModifStats * stats = HI.GetStatistics();
  if ( !stats ) return;

  if ( add )
    stats->Add( healer.GetStatistics() );
  else
    *stats = healer.GetStatistics();
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_HealingDriver,GEOM_BaseDriver);
