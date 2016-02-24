// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_Fillet1dDriver.hxx>

#include <GEOMImpl_Fillet1d.hxx>
#include <GEOMImpl_IFillet1d.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_HealingDriver.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <ShapeFix_Wire.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_ConstructionError.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>

#include <Precision.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_Fillet1dDriver::GetID()
{
  static Standard_GUID aFillet1dDriver("FF60908B-AB2E-4b71-B098-5C256C37D961");
  return aFillet1dDriver;
}

//=======================================================================
//function : GEOMImpl_Fillet1dDriver
//purpose  :
//=======================================================================
GEOMImpl_Fillet1dDriver::GEOMImpl_Fillet1dDriver()
{
}

//=======================================================================
//function : anotherVertex
//purpose  : local function to get vertex from edge
//=======================================================================
static TopoDS_Vertex anotherVertex( const TopoDS_Edge& theE,
                                    const TopoDS_Vertex& theV )
{
  // here is an assumption that edge has different vertices
  TopoDS_Vertex aV;
  TopExp_Explorer anExp( theE, TopAbs_VERTEX );
  for ( ; anExp.More(); anExp.Next() )
  {
    if ( BRepTools::Compare(theV,TopoDS::Vertex(anExp.Current())) /*theV.IsSame(anExp.Current())*/ )
      continue;
    aV = TopoDS::Vertex( anExp.Current() );
    break;
  }
  return aV;
}

//=======================================================================
//function : takePlane
//purpose  : local function returns plane of given edges
//=======================================================================
static Standard_Boolean takePlane( const TopoDS_Edge& theE1,
                                   const TopoDS_Edge& theE2,
                                   const TopoDS_Vertex& theV,
                                   gp_Pln& thePlane )
{
  TopoDS_Vertex aV12 = anotherVertex( theE1, theV );
  TopoDS_Vertex aV22 = anotherVertex( theE2, theV );
  // check can closed wire be created by two initial edges
  if ( aV12.IsNull()  || aV22.IsNull() || aV12.IsSame( aV22 ) )
    return false;

  // create plane by 3 points
  gp_XYZ aXYZ = BRep_Tool::Pnt( theV ).XYZ();
  gp_XYZ aXYZ1 = BRep_Tool::Pnt( aV12 ).XYZ();
  gp_XYZ aXYZ2 = BRep_Tool::Pnt( aV22 ).XYZ();
  try {
    gp_Dir aDir1( aXYZ - aXYZ1 );
    gp_Dir aDir2( aXYZ2 - aXYZ );
    Standard_Real anAngle = aDir1.Angle(aDir2);
    if ( fabs(anAngle) <= gp::Resolution() ||
         fabs(anAngle - M_PI) <= gp::Resolution() )
      return false;
    thePlane = gp_Pln( gp_Pnt(aXYZ), aDir1^ aDir2);
  }
  catch (Standard_Failure) {
    return false;
  }
  return true;
}

//=======================================================================
//function : addEdgeRelation
//purpose  : local function to remember relation between initial and modified edge
//=======================================================================
static void addEdgeRelation(TopTools_DataMapOfShapeShape& theMap,
                            const TopoDS_Edge& theInitE,
                            const TopoDS_Edge& theResE)
{
  if ( theMap.IsBound( theInitE ) )
    theMap.ChangeFind( theInitE ) = theResE;
  else
    theMap.Bind( theInitE, theResE );
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_Fillet1dDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet1d aCI (aFunction);

  Handle(GEOM_Function) aRefShape = aCI.GetShape();
  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull())
    return 0;
  if (aShape.ShapeType() != TopAbs_WIRE)
    Standard_ConstructionError::Raise("Wrong arguments: polyline as wire must be given");

  TopoDS_Wire aWire = TopoDS::Wire(aShape);

  bool doIgnoreSecantPoints = aCI.GetFlag();

  double rad = aCI.GetR();
  if (rad < Precision::Confusion())
    return 0;

  // collect vertices for make fillet
  TopTools_ListOfShape aVertexList;
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aWire, anIndices);
  int aLen = aCI.GetLength();
  if (aLen > 0) {
    for (int ii = 1; ii <= aLen; ii++) {
      int ind = aCI.GetVertex(ii);
      if (1 <= ind && ind <= anIndices.Extent()) {
        TopoDS_Shape aShapeVertex = anIndices.FindKey(ind);
        if (aShapeVertex.ShapeType() == TopAbs_VERTEX)
          aVertexList.Append(aShapeVertex);
      }
    }
  }
  else { // get all vertices from wire
    TopTools_MapOfShape mapShape;
    TopExp_Explorer anExp (aWire, TopAbs_VERTEX);
    for (; anExp.More(); anExp.Next()) {
      if (mapShape.Add(anExp.Current()))
        aVertexList.Append(anExp.Current());
    }
  }
  if (aVertexList.IsEmpty())
    Standard_ConstructionError::Raise("Invalid input: no vertices to make fillet");

  // at first we try to make fillet on the initial wire (without edges fusing)
  bool isFinalPass = !doIgnoreSecantPoints;
  TopoDS_Wire aResult;
  bool isAllStepsOk = MakeFillet(aWire, aVertexList, rad, isFinalPass, aResult);

  // try to fuse collinear edges to allow bigger radius
  if (!isFinalPass && !isAllStepsOk) {
    // 1. Fuse
    TopoDS_Shape aShapeNew;
    Handle(TColStd_HSequenceOfTransient) aVerts;
    GEOMImpl_HealingDriver::FuseCollinearEdges(aWire, aVerts, aShapeNew);
    TopoDS_Wire aWireNew = TopoDS::Wire(aShapeNew);

    // 2. Rebuild the list of vertices (by coincidence)
    Standard_Real tol, tolMax = Precision::Confusion();
    for (TopExp_Explorer ExV (aWireNew, TopAbs_VERTEX); ExV.More(); ExV.Next()) {
      TopoDS_Vertex Vertex = TopoDS::Vertex(ExV.Current());
      tol = BRep_Tool::Tolerance(Vertex);
      if (tol > tolMax)
        tolMax = tol;
    }

    TopTools_ListOfShape aVertexListNew;
    TopTools_IndexedMapOfShape anIndicesNew;
    TopExp::MapShapes(aWireNew, anIndicesNew);
    TopTools_ListIteratorOfListOfShape anIt (aVertexList);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Vertex aV = TopoDS::Vertex(anIt.Value());
      if (anIndicesNew.Contains(aV))
        aVertexListNew.Append(aV);
      else {
        // try to find by coords in the new wire
        gp_Pnt aP = BRep_Tool::Pnt(aV);

        bool isFound = false;
        TopTools_MapOfShape mapShape;
        TopExp_Explorer exp (aWireNew, TopAbs_VERTEX);
        for (; exp.More() && !isFound; exp.Next()) {
          if (mapShape.Add(exp.Current())) {
            TopoDS_Vertex aVi = TopoDS::Vertex(exp.Current());
            gp_Pnt aPi = BRep_Tool::Pnt(aVi);
            if (aPi.Distance(aP) < tolMax) {
              aVertexListNew.Append(aVi);
              isFound = true;
            }
          }
        }
      }
    }

    // 3. Repeat the fillet algorithm
    isFinalPass = true;
    MakeFillet(aWireNew, aVertexListNew, rad, isFinalPass, aResult);
  }

  if (!GEOMUtils::CheckShape(aResult, true) &&
      !GEOMUtils::FixShapeTolerance(aResult)) {
    Standard_ConstructionError::Raise("Non valid shape result");
  }

  aFunction->SetValue(aResult);
#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//=======================================================================
//function : MakeFillet
//purpose  :
//=======================================================================
bool GEOMImpl_Fillet1dDriver::MakeFillet(const TopoDS_Wire& aWire,
                                         const TopTools_ListOfShape& aVertexList,
                                         const Standard_Real rad,
                                         bool isFinalPass,
                                         TopoDS_Wire& aResult) const
{
  // this variable is needed to break execution
  // in case of fillet failure and try to fuse edges
  bool isAllStepsOk = true;

  //INFO: this algorithm implemented in assumption that user can select both
  //  vertices of some edges to make fillet. In this case we should remember
  //  already modified initial edges to take care in next fillet step
  TopTools_DataMapOfShapeShape anEdgeToEdgeMap;

  //iterates on vertices, and make fillet on each couple of edges
  //collect result fillet edges in list
  TopTools_ListOfShape aListOfNewEdge;
  // remember relation between initial and modified map
  TopTools_IndexedDataMapOfShapeListOfShape aMapVToEdges;
  TopExp::MapShapesAndAncestors( aWire, TopAbs_VERTEX, TopAbs_EDGE, aMapVToEdges );
  TopTools_ListIteratorOfListOfShape anIt( aVertexList );
  for ( ; anIt.More(); anIt.Next() ) {
    TopoDS_Vertex aV = TopoDS::Vertex( anIt.Value() );
    if ( aV.IsNull() || !aMapVToEdges.Contains( aV ) )
      continue;
    const TopTools_ListOfShape& aVertexEdges = aMapVToEdges.FindFromKey( aV );
    if ( aVertexEdges.Extent() != 2 )
      continue; // no input data to make fillet
    TopoDS_Edge anEdge1 = TopoDS::Edge( aVertexEdges.First() );
    TopoDS_Edge anEdge2 = TopoDS::Edge( aVertexEdges.Last() );
    // check if initial edges already modified in previous fillet operation
    if ( anEdgeToEdgeMap.IsBound( anEdge1 ) ) anEdge1 = TopoDS::Edge(anEdgeToEdgeMap.Find( anEdge1 ));
    if ( anEdgeToEdgeMap.IsBound( anEdge2 ) ) anEdge2 = TopoDS::Edge(anEdgeToEdgeMap.Find( anEdge2 ));
    if ( anEdge1.IsNull() || anEdge2.IsNull() || anEdge1.IsSame( anEdge2 ) )
      continue; //no input data to make fillet

    // create plane on 2 edges
    gp_Pln aPlane;
    if ( !takePlane(anEdge1, anEdge2, aV, aPlane) )
      continue; // seems edges does not belong to same plane or parallel (fillet can not be build)

    GEOMImpl_Fillet1d aFilletAlgo (anEdge1, anEdge2, aPlane);
    if (!aFilletAlgo.Perform(rad)) {
      if (isFinalPass)
        continue; // can not create fillet with given radius
      else {
        isAllStepsOk = false;
        break; // can not create fillet with given radius
      }
    }

    // take fillet result in given vertex
    TopoDS_Edge aModifE1, aModifE2;
    TopoDS_Edge aNewE = aFilletAlgo.Result(BRep_Tool::Pnt(aV), aModifE1, aModifE2);
    if (aNewE.IsNull()) {
      if (isFinalPass)
        continue; // no result found
      else {
        isAllStepsOk = false;
        break; // no result found
      }
    }

    // add  new created edges and take modified edges
    aListOfNewEdge.Append(aNewE);

    // check if wire edges modified,
    // if yes, then map to original edges (from vertex-edges list), because edges can be modified before
    if (aModifE1.IsNull() || !anEdge1.IsSame( aModifE1 ))
      addEdgeRelation( anEdgeToEdgeMap, TopoDS::Edge(aVertexEdges.First()), aModifE1 );
    if (aModifE2.IsNull() || !anEdge2.IsSame( aModifE2 ))
      addEdgeRelation( anEdgeToEdgeMap, TopoDS::Edge(aVertexEdges.Last()), aModifE2 );
  }

  if (anEdgeToEdgeMap.IsEmpty() && aListOfNewEdge.IsEmpty()) {
    if (isFinalPass)
      StdFail_NotDone::Raise("1D Fillet can't be computed on the given shape with the given radius");
    else
      isAllStepsOk = false;
  }

  if (!isAllStepsOk)
    return false;

  // create new wire instead of original
  for (TopExp_Explorer anExp (aWire, TopAbs_EDGE); anExp.More(); anExp.Next()) {
    TopoDS_Shape anEdge = anExp.Current();
    if (!anEdgeToEdgeMap.IsBound(anEdge))
      aListOfNewEdge.Append(anEdge);
    else if (!anEdgeToEdgeMap.Find(anEdge).IsNull())
      aListOfNewEdge.Append(anEdgeToEdgeMap.Find(anEdge));
  }

  GEOMUtils::SortShapes(aListOfNewEdge);

  BRepBuilderAPI_MakeWire aWireTool;
  aWireTool.Add(aListOfNewEdge);
  aWireTool.Build();
  if (!aWireTool.IsDone())
    return 0;

  aResult = aWireTool.Wire();

  return isAllStepsOk;
}
//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_Fillet1dDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet1d aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "FILLET_1D";

  switch ( aType ) {
  case GEOM_FILLET_1D:
    AddParam( theParams, "Wire", aCI.GetShape() );
    AddParam( theParams, "Vertexes");
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " vertexes: ";
    for (int i = 1; i <= aCI.GetLength(); ++i )
      theParams[1] << aCI.GetVertex( i ) << " ";
    AddParam( theParams, "Radius", aCI.GetR() );
    AddParam( theParams, "Fuse collinear edges", aCI.GetFlag() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_Fillet1dDriver,GEOM_BaseDriver);
