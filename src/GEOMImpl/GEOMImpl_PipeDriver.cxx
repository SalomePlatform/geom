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

#include <GEOMImpl_PipeDriver.hxx>

#include <GEOMImpl_IPipeDiffSect.hxx>
#include <GEOMImpl_IPipeShellSect.hxx>
#include <GEOMImpl_IPipeBiNormal.hxx>
#include <GEOMImpl_IPipe.hxx>
#include <GEOMImpl_IPipePath.hxx>
#include <GEOMImpl_GlueDriver.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp.hxx>
#include <GeomFill_Trihedron.hxx>
#include <GeomFill_CorrectedFrenet.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_DataMapOfShapeSequenceOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <GProp_GProps.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Conic.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert.hxx>

#include <TColgp_SequenceOfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

#include <Precision.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

#include "utilities.h"

#define GROUP_DOWN  0
#define GROUP_UP    1
#define GROUP_SIDE1 2
#define GROUP_SIDE2 3
#define GROUP_OTHER 4

static const Standard_Real TolPipeSurf = 5.e-4;

static bool FillGroups(const TopTools_SequenceOfShape         *theGroups,
                       const TopTools_IndexedMapOfShape       &theIndices,
                             Handle(TColStd_HArray1OfInteger) *theGroupIds);

static void StoreGroups(GEOMImpl_IPipe                   *theCI,
                        Handle(TColStd_HArray1OfInteger) *theGroups);

static bool DoGroups(BRepOffsetAPI_MakePipeShell &theSweep,
                     TopTools_SequenceOfShape    *theGroups);

static bool CreateGroups(BRepOffsetAPI_MakePipeShell &theSweep,
                         GEOMImpl_IPipe              *theCI);

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PipeDriver::GetID()
{
  static Standard_GUID aPipeDriver ("FF1BBB19-5D14-4df2-980B-3A668264EA16");
  return aPipeDriver;
}

//=======================================================================
//function : GEOMImpl_PipeDriver
//purpose  :
//=======================================================================
GEOMImpl_PipeDriver::GEOMImpl_PipeDriver()
{
}

//=======================================================================
//function : EvaluateBestSweepMode
//purpose  : auxilary for right call of MakePipe and MakePipeShell
//=======================================================================
static GeomFill_Trihedron EvaluateBestSweepMode(const TopoDS_Shape& Spine)
{
  GeomFill_Trihedron theMode = GeomFill_IsFrenet;
  
  TopExp_Explorer Explo(Spine, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(Explo.Current());
    Standard_Real fpar, lpar;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
    GeomAdaptor_Curve GAcurve(aCurve, fpar, lpar);
    Handle(GeomAdaptor_HCurve) GAHcurve = new GeomAdaptor_HCurve(GAcurve);

    Handle(GeomFill_CorrectedFrenet) aCorrFrenet = new GeomFill_CorrectedFrenet(Standard_True); //for evaluation
    aCorrFrenet->SetCurve(GAHcurve);
    GeomFill_Trihedron aMode = aCorrFrenet->EvaluateBestMode();
    if (aMode == GeomFill_IsDiscreteTrihedron)
    {
      theMode = aMode;
      break;
    }
    if (aMode == GeomFill_IsCorrectedFrenet)
      theMode = aMode;
  }

  return theMode;
}

//=======================================================================
//function : BuildPipeShell
//purpose  : Builds a pipe shell. If failed, try to build in Descrete Trihedron
//           mode. Returns Standard_True if the building is done successfully.
//=======================================================================
static Standard_Boolean BuildPipeShell(BRepOffsetAPI_MakePipeShell &theBuilder)
{
  theBuilder.SetForceApproxC1(Standard_True);

  theBuilder.Build();

  Standard_Boolean isDone = theBuilder.IsDone();

  if (!isDone ||
      theBuilder.ErrorOnSurface() > TolPipeSurf) {
    // Try to use Descrete Trihedron mode.
    theBuilder.SetDiscreteMode();
    theBuilder.Build();
    isDone = theBuilder.IsDone();
  }

  return isDone;
}

//=======================================================================
//function : FillForOtherEdges
//purpose  : auxilary for CreatePipeForShellSections()
//=======================================================================
static bool FillForOtherEdges(const TopoDS_Shape& F1,
                              const TopoDS_Shape& E1,
                              const TopoDS_Shape& V1,
                              TopTools_IndexedDataMapOfShapeShape& FF)
{
  //cout<<"FillForOtherEdges"<<endl;
  // find other pairs for vertexes and edges
  // creating map of vertex edges for both faces
  TopTools_IndexedDataMapOfShapeListOfShape aMapVertEdge1;
  TopExp::MapShapesAndAncestors(F1, TopAbs_VERTEX, TopAbs_EDGE, aMapVertEdge1);
  if (!FF.Contains(F1))
    MESSAGE("    FillForOtherEdges: map FF not contains key F1");
  if (!FF.Contains(E1))
    MESSAGE("    FillForOtherEdges: map FF not contains key E1");
  if (!FF.Contains(V1))
    MESSAGE("    FillForOtherEdges: map FF not contains key V1");
  const TopoDS_Shape& F2 = FF.FindFromKey(F1);
  const TopoDS_Shape& E2 = FF.FindFromKey(E1);
  const TopoDS_Shape& V2 = FF.FindFromKey(V1);
  TopTools_IndexedDataMapOfShapeListOfShape aMapVertEdge2;
  TopExp::MapShapesAndAncestors(F2, TopAbs_VERTEX, TopAbs_EDGE, aMapVertEdge2);

  TopoDS_Edge ES1 = TopoDS::Edge(E1);
  TopoDS_Edge ES2 = TopoDS::Edge(E2);
  TopoDS_Shape VS1 = V1;
  TopoDS_Shape VS2 = V2;

  ShapeAnalysis_Edge sae;
  while(1) {
    if (!aMapVertEdge1.Contains(VS1))
      MESSAGE ("    FillForOtherEdges: map aMapVertEdge1 not contains key VS1");
    const TopTools_ListOfShape& aList1 = aMapVertEdge1.FindFromKey(VS1);
    //TopoDS_Shape E1next;
    TopTools_ListIteratorOfListOfShape anIter1(aList1);
    if (anIter1.Value().IsSame(ES1)) {
      anIter1.Next();
    }
    //E1next = anIter1.Value();
    if (!aMapVertEdge2.Contains(VS2))
      MESSAGE ("    FillForOtherEdges: map aMapVertEdge2 not contains key VS2");
    const TopTools_ListOfShape& aList2 = aMapVertEdge2.FindFromKey(VS2);
    //TopoDS_Shape E2next;
    TopTools_ListIteratorOfListOfShape anIter2(aList2);
    if (anIter2.Value().IsSame(ES2)) {
      anIter2.Next();
    }
    //E2next = anIter2.Value();
    //ES1 = TopoDS::Edge(E1next);
    //ES2 = TopoDS::Edge(E2next);
    ES1 = TopoDS::Edge(anIter1.Value());
    ES2 = TopoDS::Edge(anIter2.Value());
    if (!FF.Contains(ES1)) {
      FF.Add(ES1,ES2);
    }
    if (VS1.IsSame(sae.FirstVertex(ES1)))
      VS1 = sae.LastVertex(ES1);
    else
      VS1 = sae.FirstVertex(ES1);
    if (VS2.IsSame(sae.FirstVertex(ES2)))
      VS2 = sae.LastVertex(ES2);
    else
      VS2 = sae.FirstVertex(ES2);
    if (VS1.IsSame(V1))
      break;
    if (!FF.Contains(VS1)) {
      FF.Add(VS1,VS2);
    }
  }

  return true;
}

//=======================================================================
//function : FillCorrespondingEdges
//purpose  : auxilary for CreatePipeForShellSections()
//=======================================================================
static bool FillCorrespondingEdges(const TopoDS_Shape& FS1,
                                   const TopoDS_Shape& FS2,
                                   const TopoDS_Vertex& aLoc1,
                                   const TopoDS_Vertex& aLoc2,
                                   const TopoDS_Wire& aWirePath,
                                   TopTools_IndexedDataMapOfShapeShape& FF)
{
  //cout<<"FillCorrespondingEdges"<<endl;
  // find corresponding edges
  TopExp_Explorer expw1(FS1,TopAbs_WIRE);
  TopoDS_Wire aWire1 = TopoDS::Wire(expw1.Current());
  //exp = TopExp_Explorer(FS2,TopAbs_WIRE);
  TopExp_Explorer expw2(FS2,TopAbs_WIRE);
  TopoDS_Wire aWire2 = TopoDS::Wire(expw2.Current());
  BRepOffsetAPI_MakePipeShell aBuilder(aWirePath);
  GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(aWirePath);
  if (theBestMode == GeomFill_IsDiscreteTrihedron)
    aBuilder.SetDiscreteMode();
  aBuilder.Add(aWire1, aLoc1);
  aBuilder.Add(aWire2, aLoc2);
  if (!aBuilder.IsReady()) {
    return false;
  }

  BuildPipeShell(aBuilder);

  TopoDS_Shape aShape = aBuilder.Shape();
  ShapeAnalysis_Edge sae;
  double tol = Max(BRep_Tool::Tolerance(TopoDS::Face(FS1)),
                    BRep_Tool::Tolerance(TopoDS::Face(FS2)));
  TopTools_MapOfShape Vs1,Vs2;
  TopExp_Explorer exp;
  exp.Init(FS1, TopAbs_EDGE);
  TopoDS_Edge E1 = TopoDS::Edge(exp.Current());
  TopoDS_Vertex V11 = sae.FirstVertex(E1);
  TopoDS_Vertex V21 = sae.LastVertex(E1);
  gp_Pnt P11 = BRep_Tool::Pnt(V11);
  gp_Pnt P21 = BRep_Tool::Pnt(V21);
  //cout<<"P11("<<P11.X()<<","<<P11.Y()<<","<<P11.Z()<<")"<<endl;
  //cout<<"P21("<<P21.X()<<","<<P21.Y()<<","<<P21.Z()<<")"<<endl;
  // find corresponding vertexes from created shape
  TopoDS_Vertex VN11,VN21;
  for (exp.Init(aShape, TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    gp_Pnt P = BRep_Tool::Pnt(V);
    if (P.Distance(P11)<tol) {
      VN11 = V;
    }
    if (P.Distance(P21)<tol) {
      VN21 = V;
    }
  }
  // find edge contains VN11 and VN21 and corresponding vertexes
  TopoDS_Vertex VN12,VN22;
  for (exp.Init(aShape, TopAbs_FACE); exp.More(); exp.Next()) {
    TopoDS_Shape F = exp.Current();
    TopExp_Explorer expe;
    bool IsFind = false;
    for (expe.Init(F, TopAbs_EDGE); expe.More(); expe.Next()) {
      TopoDS_Edge E = TopoDS::Edge(expe.Current());
      TopoDS_Vertex VF = sae.FirstVertex(E);
      TopoDS_Vertex VL = sae.LastVertex(E);
      if ((VF.IsSame(VN11) && VL.IsSame(VN21)) || (VF.IsSame(VN21) && VL.IsSame(VN11))) {
        IsFind = true;
        break;
      }
    }
    if (IsFind) {
      for (expe.Init(F, TopAbs_EDGE); expe.More(); expe.Next()) {
        TopoDS_Edge E = TopoDS::Edge(expe.Current());
        TopoDS_Vertex VF = sae.FirstVertex(E);
        TopoDS_Vertex VL = sae.LastVertex(E);
        if (VF.IsSame(VN11) && !VL.IsSame(VN21))
          VN12 = VL;
        if (VL.IsSame(VN11) && !VF.IsSame(VN21))
          VN12 = VF;
        if (VF.IsSame(VN21) && !VL.IsSame(VN11))
          VN22 = VL;
        if (VL.IsSame(VN21) && !VF.IsSame(VN11))
          VN22 = VF;
      }
      break;
    }
  }
  // find vertexes from FS2 corresponded to VN12 and VN22
  // and find edge from FS2 contains V12 and V22,
  // this edge will be corresponded to edge E1
  TopoDS_Vertex V12,V22;
  gp_Pnt PN12 = BRep_Tool::Pnt(VN12);
  gp_Pnt PN22 = BRep_Tool::Pnt(VN22);
  //cout<<"PN12("<<PN12.X()<<","<<PN12.Y()<<","<<PN12.Z()<<")"<<endl;
  //cout<<"PN22("<<PN22.X()<<","<<PN22.Y()<<","<<PN22.Z()<<")"<<endl;
  TopoDS_Edge E2;
  TopExp_Explorer expe;
  for (expe.Init(FS2, TopAbs_EDGE); expe.More(); expe.Next()) {
    TopoDS_Edge E = TopoDS::Edge(expe.Current());
    TopoDS_Vertex VF = sae.FirstVertex(E);
    TopoDS_Vertex VL = sae.LastVertex(E);
    gp_Pnt PF = BRep_Tool::Pnt(VF);
    gp_Pnt PL = BRep_Tool::Pnt(VL);
    if (PF.Distance(PN12)<tol && PL.Distance(PN22)<tol) {
      V12 = VF;
      V22 = VL;
      E2 = E;
      break;
    }
    if (PF.Distance(PN22)<tol && PL.Distance(PN12)<tol) {
      V12 = VL;
      V22 = VF;
      E2 = E;
      break;
    }
  }
  FF.Add(V11,V12);
  FF.Add(V21,V22);
  FF.Add(E1,E2);

  // find other pairs for vertexes and edges
  // creating map of vertex edges for both faces
  return FillForOtherEdges(FS1,E1,V21,FF);

  //return true;
}

//=======================================================================
//function : FillCorrespondingEdges
//purpose  : auxilary for CreatePipeShellsWithoutPath()
//=======================================================================
static bool FillCorrespondingEdges(const TopoDS_Shape& FS1,
                                   const TopoDS_Shape& FS2,
                                   const TopoDS_Vertex& aLoc1,
                                   const TopoDS_Vertex& aLoc2,
                                   TopTools_IndexedDataMapOfShapeShape& FF)
{
  //cout<<"FillCorrespondingEdges"<<endl;

  gp_Pnt P1 = BRep_Tool::Pnt(aLoc1);
  gp_Pnt P2 = BRep_Tool::Pnt(aLoc2);
  gp_Vec aDir(P1,P2);

  ShapeAnalysis_Edge sae;
  double tol = Max(BRep_Tool::Tolerance(TopoDS::Face(FS1)),
                    BRep_Tool::Tolerance(TopoDS::Face(FS2)));
  TopTools_MapOfShape Vs1,Vs2;

  TopoDS_Vertex V11=aLoc1, V12=aLoc2, V21, V22;
  TopoDS_Edge E1,E2;

  TopExp_Explorer exp1;
  for (exp1.Init(FS1,TopAbs_EDGE); exp1.More(); exp1.Next()) {
    E1 = TopoDS::Edge(exp1.Current());
    TopoDS_Vertex V1 = sae.FirstVertex(E1);
    TopoDS_Vertex V2 = sae.LastVertex(E1);
    gp_Pnt Ptmp1 = BRep_Tool::Pnt(V1);
    gp_Pnt Ptmp2 = BRep_Tool::Pnt(V2);
    //cout<<"P11("<<P11.X()<<","<<P11.Y()<<","<<P11.Z()<<")"<<endl;
    //cout<<"P21("<<P21.X()<<","<<P21.Y()<<","<<P21.Z()<<")"<<endl;
    if (P1.Distance(Ptmp1)<tol) {
      V21 = V2;
      break;
    }
    if (P1.Distance(Ptmp2)<tol) {
      V21 = V1;
      break;
    }
  }

  TopoDS_Edge E21,E22;
  TopoDS_Vertex VE21,VE22;
  int nbe=0;
  for (exp1.Init(FS2,TopAbs_EDGE); exp1.More() && nbe<2; exp1.Next()) {
    TopoDS_Edge E = TopoDS::Edge(exp1.Current());
    TopoDS_Vertex V1 = sae.FirstVertex(E);
    TopoDS_Vertex V2 = sae.LastVertex(E);
    gp_Pnt Ptmp1 = BRep_Tool::Pnt(V1);
    gp_Pnt Ptmp2 = BRep_Tool::Pnt(V2);
    if (P2.Distance(Ptmp1)<tol) {
      if (nbe==0) {
        E21 = E;
        VE21 = V2;
        nbe++;
      }
      else if (nbe==1) {
        E22 = E;
        VE22 = V2;
        nbe++;
      }
    }
    if (P2.Distance(Ptmp2)<tol) {
      if (nbe==0) {
        E21 = E;
        VE21 = V1;
        nbe++;
      }
      else if (nbe==1) {
        E22 = E;
        VE22 = V1;
        nbe++;
      }
    }
  }

  gp_Pnt PV21 = BRep_Tool::Pnt(V21);
  gp_Pnt PE21 = BRep_Tool::Pnt(VE21);
  gp_Pnt PE22 = BRep_Tool::Pnt(VE22);
  gp_Vec aDir1(PV21,PE21);
  gp_Vec aDir2(PV21,PE22);
  double ang1 = aDir.Angle(aDir1);
  double ang2 = aDir.Angle(aDir2);
  if (fabs(ang1)<fabs(ang2)) {
    E2 = E21;
    V22 = VE21;
  }
  else {
    E2 = E22;
    V22 = VE22;
  }

  FF.Add(V11,V12);
  FF.Add(V21,V22);
  FF.Add(E1,E2);

  // find other pairs for vertexes and edges
  return FillForOtherEdges(FS1,E1,V21,FF);
}

//=======================================================================
//function : FindNextPairOfFaces
//purpose  : auxilary for CreatePipeForShellSections()
//=======================================================================
static void FindNextPairOfFaces(const TopoDS_Shape& aCurFace,
                                TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgeFaces1,
                                TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgeFaces2,
                                TopTools_IndexedDataMapOfShapeShape& FF,
                                GEOMImpl_IPipe* aCI)
{
  //cout<<"FindNextPairOfFaces"<<endl;
  TopExp_Explorer anExp;
  for (anExp.Init(aCurFace, TopAbs_EDGE); anExp.More(); anExp.Next()) {
    TopoDS_Shape E1 = anExp.Current();
    if (!FF.Contains(E1)) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("FindNextPairOfFaces: Can not find edge in map");
    }
    if (!FF.Contains(E1))
      MESSAGE ("    FindNextPairOfFaces: map FF not contains key E1");
    const TopoDS_Shape& E2 = FF.FindFromKey(E1);
    TopExp_Explorer anExpV;
    anExpV.Init(E1, TopAbs_VERTEX);
    TopoDS_Shape V1 = anExpV.Current();
    if (!FF.Contains(V1)) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("FindNextPairOfFaces: Can not find vertex in map");
    }

    if (!aMapEdgeFaces1.Contains(E1))
      MESSAGE ("    FindNextPairOfFaces: map aMapEdgeFaces1 not contains key E1");
    const TopTools_ListOfShape& aList1 = aMapEdgeFaces1.FindFromKey(E1);
    if (aList1.Extent()<2)
      continue;
    TopTools_ListIteratorOfListOfShape anIter(aList1);
    if (anIter.Value().IsEqual(aCurFace)) {
      anIter.Next();
    }
    TopoDS_Shape F1other = anIter.Value();
    if (FF.Contains(F1other))
      continue;

    if (!FF.Contains(aCurFace))
      MESSAGE ("    FindNextPairOfFaces: map FF not contains key aCurFace");
    const TopoDS_Shape& F2 = FF.FindFromKey(aCurFace);
    if (!aMapEdgeFaces2.Contains(E2))
      MESSAGE ("    FindNextPairOfFaces: map aMapEdgeFaces2 not contains key E2");
    const TopTools_ListOfShape& aList2 = aMapEdgeFaces2.FindFromKey(E2);
    if (aList2.Extent()<2) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("FindNextPairOfFaces: Can not find corresponding face");
    }
    TopTools_ListIteratorOfListOfShape anIter2(aList2);
    if (anIter2.Value().IsEqual(F2)) {
      anIter2.Next();
    }
    TopoDS_Shape F2other = anIter2.Value();
    FF.Add(F1other,F2other);

    // add pairs of edges to FF
    bool stat =  FillForOtherEdges(F1other,E1,V1,FF);
    if (!stat) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("FindNextPairOfFaces: Can not mapping other egdes");
    }

    FindNextPairOfFaces(F1other, aMapEdgeFaces1, aMapEdgeFaces2, FF, aCI);
  }
}

//=======================================================================
//function : FindFirstPairFaces
//purpose  : auxilary for Execute()
//=======================================================================
static void FindFirstPairFaces(const TopoDS_Shape& S1, const TopoDS_Shape& S2,
                               TopoDS_Vertex& V1, TopoDS_Vertex& V2,
                               TopoDS_Shape& FS1, TopoDS_Shape& FS2)
{
  //cout<<"FindFirstPairFaces"<<endl;

  // check if vertexes are sub-shapes of sections
  gp_Pnt P1 = BRep_Tool::Pnt(V1);
  gp_Pnt P2 = BRep_Tool::Pnt(V2);
  TopoDS_Vertex V1new,V2new;
  TopExp_Explorer exp;
  double mindist = 1.e10;
  for (exp.Init(S1, TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    gp_Pnt P = BRep_Tool::Pnt(V);
    double dist = P1.Distance(P);
    if (dist<mindist) {
      mindist = dist;
      V1new = V;
    }
  }
  mindist = 1.e10;
  for (exp.Init(S2, TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    gp_Pnt P = BRep_Tool::Pnt(V);
    double dist = P2.Distance(P);
    if (dist<mindist) {
      mindist = dist;
      V2new = V;
    }
  }

  //gp_Pnt P1new = BRep_Tool::Pnt(V1new);
  //gp_Pnt P2new = BRep_Tool::Pnt(V2new);
  //cout<<"  P1("<<P1.X()<<","<<P1.Y()<<","<<P1.Z()<<")"<<endl;
  //cout<<"  P2("<<P2.X()<<","<<P2.Y()<<","<<P2.Z()<<")"<<endl;
  //cout<<"  P1new("<<P1new.X()<<","<<P1new.Y()<<","<<P1new.Z()<<")"<<endl;
  //cout<<"  P2new("<<P2new.X()<<","<<P2new.Y()<<","<<P2new.Z()<<")"<<endl;

  // replace vertexes if it is needed
  if (!V1.IsSame(V1new)) {
    V1 = V1new;
    P1 = BRep_Tool::Pnt(V1);
    MESSAGE ("  replace V1");
  }
  else
    MESSAGE ("  not replace V1");
  if (!V2.IsSame(V2new)) {
    V2 = V2new;
    P2 = BRep_Tool::Pnt(V2);
    MESSAGE ("  replace V2");
  }
  else
    MESSAGE ("  not replace V2");

  TopTools_IndexedDataMapOfShapeListOfShape aMapVertFaces1;
  TopExp::MapShapesAndAncestors(S1, TopAbs_VERTEX, TopAbs_FACE, aMapVertFaces1);
  TopTools_IndexedDataMapOfShapeListOfShape aMapVertFaces2;
  TopExp::MapShapesAndAncestors(S2, TopAbs_VERTEX, TopAbs_FACE, aMapVertFaces2);

  if (!aMapVertFaces1.Contains(V1))
    MESSAGE ("    FindFirstPairFaces: map aMapVertFaces1 not contains key V1");
  const TopTools_ListOfShape& aList1 = aMapVertFaces1.FindFromKey(V1);
  TopTools_ListIteratorOfListOfShape anIter1(aList1);
  FS1 = anIter1.Value();
  // find middle point
  double x1=0., y1=0., z1=0.;
  int nbv1=0;
  for (exp.Init(FS1, TopAbs_VERTEX); exp.More(); exp.Next()) {
    TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
    gp_Pnt P = BRep_Tool::Pnt(V);
    x1 += P.X();
    y1 += P.Y();
    z1 += P.Z();
    nbv1++;
  }
  gp_Pnt PM1(x1/nbv1, y1/nbv1, z1/nbv1);

  TColgp_SequenceOfPnt Ps;
  TopTools_SequenceOfShape Fs;
  if (!aMapVertFaces2.Contains(V2))
    MESSAGE ("    FindFirstPairFaces: map aMapVertFaces2 not contains key V2");
  const TopTools_ListOfShape& aList2 = aMapVertFaces2.FindFromKey(V2);
  TopTools_ListIteratorOfListOfShape anIter2(aList2);
  for (; anIter2.More(); anIter2.Next()) {
    TopoDS_Shape F = anIter2.Value();
    double x2=0., y2=0., z2=0.;
    int nbv2=0;
    for (exp.Init(F, TopAbs_VERTEX); exp.More(); exp.Next()) {
      TopoDS_Vertex V = TopoDS::Vertex(exp.Current());
      gp_Pnt P = BRep_Tool::Pnt(V);
      x2 += P.X();
      y2 += P.Y();
      z2 += P.Z();
      nbv2++;
    }
    gp_Pnt PM(x2/nbv1, y2/nbv1, z2/nbv1);
    Fs.Append(F);
    Ps.Append(PM);
  }

  gp_Vec aDir(P1,P2);
  int i=1;
  double MinAng = M_PI;
  int numface = 0;
  for (; i<=Fs.Length(); i++) {
    gp_Vec tmpDir(PM1,Ps(i));
    double ang = fabs(aDir.Angle(tmpDir));
    if (ang<MinAng) {
      MinAng = ang;
      numface = i;
    }
  }
  FS2 = Fs(numface);
}

//=======================================================================
//function : RemoveFaces
//purpose  : This function returns theShapeFrom without faces of the shape
//           theFacesToRm. It returns a shell if theShapeFrom is a solid or
//           a compound otherwise. Auxilary for CreatePipeWithDifferentSections
//           method.
//=======================================================================
static TopoDS_Shape RemoveFaces(const TopoDS_Shape &theShapeFrom,
                                const TopoDS_Shape &theFacesToRm)
{
  TopTools_IndexedMapOfShape aMapFaces;
  TopExp_Explorer            anExp(theShapeFrom, TopAbs_FACE);
  BRep_Builder               aBuilder;
  TopoDS_Shape               aResult;

  if (theShapeFrom.ShapeType() == TopAbs_SOLID) {
    // Create shell
    aBuilder.MakeShell(TopoDS::Shell(aResult));
  } else {
    // Create compound
    aBuilder.MakeCompound(TopoDS::Compound(aResult));
  }

  TopExp::MapShapes(theFacesToRm, TopAbs_FACE, aMapFaces);

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aFace = anExp.Current();

    if (!aMapFaces.Contains(aFace)) {
      aBuilder.Add(aResult, aFace);
    }
  }

  return aResult;
}

//=======================================================================
//function : CreatePipeWithDifferentSections
//purpose  :
//=======================================================================
TopoDS_Shape GEOMImpl_PipeDriver::CreatePipeWithDifferentSections
                  (const TopoDS_Wire                       &theWirePath,
                   const Handle(TopTools_HSequenceOfShape)  theHSeqBases,
                   const Handle(TopTools_HSequenceOfShape)  theHSeqLocs,
                   const Standard_Boolean                   theWithContact,
                   const Standard_Boolean                   theWithCorrect,
                   const Standard_Boolean                   IsBySteps,
                         Handle(TColStd_HArray1OfInteger)  *theGroups)
{
  TopoDS_Shape aShape;

  TopoDS_Wire aWirePath = theWirePath;

  Standard_Integer nbBases = theHSeqBases->Length();
  Standard_Integer nbLocs = (theHSeqLocs.IsNull() ? 0 : theHSeqLocs->Length());

  if (nbLocs && nbLocs != nbBases) {
    Standard_ConstructionError::Raise("Number of sections is not equal to number of locations ");
  }

  TopTools_SequenceOfShape aSeqBases;
  TopTools_SequenceOfShape aSeqLocs;
  TopTools_SequenceOfShape aSeqFaces;
  Standard_Boolean NeedCreateSolid = Standard_False;

  Standard_Integer i = 1;
  for (i = 1; i <= nbBases; i++) {
    if (theHSeqBases->Value(i).IsNull())
      continue;

    // Make copy to prevent modifying of base object 0020766 : EDF 1320
    TopoDS_Shape aShapeBase;
    BRepBuilderAPI_Copy Copy (theHSeqBases->Value(i));
    if (Copy.IsDone())
      aShapeBase = Copy.Shape();

    TopAbs_ShapeEnum aTypeBase = aShapeBase.ShapeType();

    //if for section was specified face with a few wires then a few
    //    pipes were build and make solid
    if (aTypeBase == TopAbs_SHELL) {
      // create wire as boundary contour if shell is no closed
      // get free boundary shapes
      ShapeAnalysis_FreeBounds anAnalizer(aShapeBase);
      TopoDS_Compound aClosed = anAnalizer.GetClosedWires();
      TopExp_Explorer anExp;
      TopoDS_Shape aWire;
      Standard_Integer NbWires = 0;
      for (anExp.Init(aClosed, TopAbs_WIRE); anExp.More(); anExp.Next()) {
        NbWires++;
        aWire = anExp.Current();
      }
      if (NbWires != 1) {
        // bad case
        Standard_ConstructionError::Raise("Bad shell is used as section ");
      }
      NeedCreateSolid = Standard_True;
      aSeqFaces.Append(aShapeBase);
      aSeqBases.Append(aWire);
    }
    else if (aTypeBase == TopAbs_FACE) {
      NeedCreateSolid = Standard_True;
      //for case one path should be used other type function
      aSeqFaces.Append(aShapeBase);
      TopExp_Explorer aExpW(aShapeBase,TopAbs_WIRE);
      for (; aExpW.More(); aExpW.Next()) {
        TopoDS_Shape aWireProf = aExpW.Current();
        aSeqBases.Append(aWireProf);
      }
    }
    else if (aTypeBase == TopAbs_WIRE || aTypeBase == TopAbs_VERTEX) {
      aSeqBases.Append(aShapeBase);
    }
    else if (aTypeBase == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShapeBase);
      TopoDS_Shape aWireProf = BRepBuilderAPI_MakeWire(anEdge);
      aSeqBases.Append(aWireProf);
    }
    if (nbLocs) {
      TopoDS_Shape aShapeLoc = theHSeqLocs->Value(i);
      if (aShapeLoc.IsNull() || aShapeLoc.ShapeType() != TopAbs_VERTEX)
        continue;
      aSeqLocs.Append(aShapeLoc);
    }
  }

  nbLocs = aSeqLocs.Length();

  // skl 02.05.2007
  TopTools_SequenceOfShape Edges;
  if (nbLocs > 0) {
    // we have to check that each location shape is a vertex from
    // path and update aSeqLocs if it is needed (and possible)
    TColgp_SequenceOfPnt PLocs;
    for (i=1; i<=nbLocs; i++) {
      TopoDS_Vertex V = TopoDS::Vertex(aSeqLocs.Value(i));
      PLocs.Append(BRep_Tool::Pnt(V));
    }
    //TopTools_SequenceOfShape Edges;
    TopExp_Explorer anExp;
    for (anExp.Init(aWirePath, TopAbs_EDGE); anExp.More(); anExp.Next()) {
      Edges.Append(anExp.Current());
    }
    int nbEdges = Edges.Length();
    ShapeAnalysis_Edge sae;
    TopoDS_Edge edge = TopoDS::Edge(Edges.First());
    double tol = BRep_Tool::Tolerance(edge);
    TopoDS_Vertex VF = sae.FirstVertex(edge);
    gp_Pnt PF = BRep_Tool::Pnt(VF);
    //cout<<"PF("<<PF.X()<<","<<PF.Y()<<","<<PF.Z()<<")"<<endl;
    if (PF.Distance(PLocs.First()) > tol) {
      Standard_ConstructionError::Raise
        ("First location shapes is not coincided with first vertex of aWirePath");
    }
    aSeqLocs.ChangeValue(1) = VF;
    edge = TopoDS::Edge(Edges.Last());
    tol = BRep_Tool::Tolerance(edge);
    TopoDS_Vertex VL = sae.LastVertex(edge);
    gp_Pnt PL = BRep_Tool::Pnt(VL);
    if (PL.Distance(PLocs.Last()) > tol) {
      Standard_ConstructionError::Raise
        ("Last location shapes is not coincided with last vertex of aWirePath");
    }
    aSeqLocs.ChangeValue(nbLocs) = VL;
    int jcurr = 2;
    for (i=1; i<=Edges.Length() && jcurr<nbLocs; i++) {
      TopoDS_Edge E = TopoDS::Edge(Edges.Value(i));
      tol = BRep_Tool::Tolerance(edge);
      TopoDS_Vertex V1 = sae.FirstVertex(E);
      TopoDS_Vertex V2 = sae.LastVertex(E);
      gp_Pnt P1 = BRep_Tool::Pnt(V1);
      gp_Pnt P2 = BRep_Tool::Pnt(V2);
      if (P2.Distance(PLocs.Value(jcurr)) < tol) {
        aSeqLocs.ChangeValue(jcurr) = V2;
        jcurr++;
      }
      else {
        // find distance between E and aLocs(jcurr)
        double fp,lp;
        Handle(Geom_Curve) C = BRep_Tool::Curve(E,fp,lp);
        GeomAPI_ProjectPointOnCurve PPCurve (PLocs.Value(jcurr),C);
        if (PPCurve.NbPoints()>0 &&
            PLocs.Value(jcurr).Distance(PPCurve.Point(1)) < tol) {
          double param = PPCurve.Parameter(1);
          gp_Pnt PC1;
          C->D0(param,PC1);
          // split current edge
          Handle(Geom_TrimmedCurve) tc1 = new Geom_TrimmedCurve(C,fp,param);
          Handle(Geom_TrimmedCurve) tc2 = new Geom_TrimmedCurve(C,param,lp);
          TopoDS_Edge E1,E2;
          BRep_Builder B;
          gp_Pnt Pfp;
          C->D0(fp,Pfp);
          if (Pfp.Distance(P1)<tol) {
            B.MakeEdge(E1,tc1,tol);
            B.Add(E1,V1);
            TopoDS_Shape tmpV = aSeqLocs.Value(jcurr).Oriented(TopAbs_REVERSED);
            B.Add(E1,TopoDS::Vertex(tmpV));
            B.MakeEdge(E2,tc2,tol);
            tmpV = aSeqLocs.Value(jcurr).Oriented(TopAbs_FORWARD);
            B.Add(E2,TopoDS::Vertex(tmpV));
            B.Add(E2,V2);
          }
          else {
            B.MakeEdge(E1,tc2,tol);
            TopoDS_Shape tmpV = aSeqLocs.Value(jcurr).Oriented(TopAbs_FORWARD);
            B.Add(E1,TopoDS::Vertex(tmpV));
            B.Add(E1,V1);
            E1.Reverse();
            B.MakeEdge(E2,tc1,tol);
            B.Add(E2,V2);
            tmpV = aSeqLocs.Value(jcurr).Oriented(TopAbs_REVERSED);
            B.Add(E2,TopoDS::Vertex(tmpV));
            E2.Reverse();
          }
          jcurr++;
          Edges.Remove(i);
          Edges.InsertAfter(i-1,E1);
          Edges.InsertAfter(i,E2);
        }
      }
    }
    if (nbEdges<Edges.Length()) {
        // one of edges was splitted => we have to update WirePath
        BRep_Builder B;
        TopoDS_Wire W;
        B.MakeWire(W);
        for (i=1; i<=Edges.Length(); i++) {
          B.Add(W,TopoDS::Edge(Edges.Value(i)));
        }
        aWirePath = W;
    }
  }

  TColStd_SequenceOfInteger SplitEdgeNums,SplitLocNums;

  if (IsBySteps) {
    // Fill SplitEdgeNums and SplitLocNums with intermediate location indices
    // and corresponding edge indices.
    Standard_Integer i = 1;
    Standard_Integer j;
    TopoDS_Vertex    aVert;
    gp_Pnt           aP;

    for (j = 2; j < aSeqLocs.Length(); j++) {
      SplitLocNums.Append(j);
      aVert = TopoDS::Vertex(aSeqLocs.Value(j));
      aP    = BRep_Tool::Pnt(aVert);

      while (i < Edges.Length()) {
        Standard_Real      aFp;
        Standard_Real      aLp;
        TopoDS_Edge        anEdge = TopoDS::Edge(Edges.Value(i));
        Standard_Real      aTol   = BRep_Tool::Tolerance(anEdge);
        Handle(Geom_Curve) aC     = BRep_Tool::Curve(anEdge, aFp, aLp);
        gp_Pnt             aPLast;

        aC->D0(aLp, aPLast);
        i++;

        if (aP.Distance(aPLast) < aTol) {
          SplitEdgeNums.Append(i - 1);
          break;
        }
      }
    }
  } else {
    // check curvature of wire for condition that
    // max summary angle between directions along
    // wire path must be < 4*PI. If not - split wire
    // and seguences of shapes, perform pipe for each
    // and make sewing after that
    double fp,lp;
    gp_Pnt P1,P2;
    gp_Vec Vec1,Vec2;
    double SumAng = 0;
    if ( Edges.Length() > 0 ) {
      Handle(Geom_Curve) C = BRep_Tool::Curve(TopoDS::Edge(Edges.Value(1)),fp,lp);
      C->D1(fp,P1,Vec1);
      C->D1(lp,P2,Vec2);
      SumAng = fabs(Vec1.Angle(Vec2));
      Vec1 = Vec2;
      P1 = P2;
    }
    int LastLoc = 1;
    //cout<<"Edges.Length()="<<Edges.Length()<<endl;
    for (i=2; i<=Edges.Length(); i++) {
        TopoDS_Edge edge = TopoDS::Edge(Edges.Value(i));
        double tol = BRep_Tool::Tolerance(edge);
        Handle(Geom_Curve) C = BRep_Tool::Curve(edge,fp,lp);
        C->D1(lp,P2,Vec2);
        double ang = fabs(Vec1.Angle(Vec2));
        SumAng += ang;
        if (SumAng>4*M_PI) {
          SumAng = ang;
          SplitEdgeNums.Append(i-1);
          int j;
          for (j=LastLoc+1; j<=aSeqLocs.Length(); j++) {
            TopoDS_Vertex aVert = TopoDS::Vertex(aSeqLocs.Value(j));
            gp_Pnt P = BRep_Tool::Pnt(aVert);
            if (P1.Distance(P) < tol) {
              SplitLocNums.Append(j);
              LastLoc = j;
              break;
            }
          }
        }
        Vec1 = Vec2;
        P1 = P2;
    }
  }

  bool isCreateGroups = (theGroups != NULL);

  if (SplitLocNums.Length()==SplitEdgeNums.Length() && SplitEdgeNums.Length()>0) {
      TopTools_SequenceOfShape               aSeqRes;
      TopTools_DataMapOfShapeSequenceOfShape aMapResGroups[5];
      Standard_Integer                       iGrp;
      int nn, num1 = 1, num2 = 1;
      for (nn=1; nn<=SplitEdgeNums.Length(); nn++) {
        // create wirepath and sequences of shapes
        BRep_Builder B;
        TopoDS_Wire tmpW;
        B.MakeWire(tmpW);
        for (i=num1; i<=SplitEdgeNums.Value(nn); i++) {
          B.Add(tmpW,TopoDS::Edge(Edges.Value(i)));
        }
        num1 = SplitEdgeNums.Value(nn) + 1;
        TopTools_SequenceOfShape aTmpSeqBases;
        TopTools_SequenceOfShape aTmpSeqLocs;
        for (i=num2; i<=SplitLocNums.Value(nn); i++) {
          aTmpSeqBases.Append(aSeqBases.Value(i));
          aTmpSeqLocs.Append(aSeqLocs.Value(i));
        }
        num2 = SplitLocNums.Value(nn);
        // make pipe
        BRepOffsetAPI_MakePipeShell aBuilder(tmpW);
        GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(tmpW);
        if (theBestMode == GeomFill_IsDiscreteTrihedron)
          aBuilder.SetDiscreteMode();
        Standard_Integer nbShapes = aTmpSeqBases.Length();
        for (i=1; i<=nbShapes; i++) {
          TopoDS_Shape aShapeLoc = aTmpSeqLocs.Value(i);
          TopoDS_Vertex aVert = TopoDS::Vertex(aShapeLoc);
          aBuilder.Add(aTmpSeqBases.Value(i), aVert, theWithContact, theWithCorrect);
        }
        if (!aBuilder.IsReady()) {
          Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
        }

        Standard_Boolean isDone = BuildPipeShell(aBuilder);

        if (isDone && NeedCreateSolid && nn == 1) {
          // Make solid for the first step.
          isDone = aBuilder.MakeSolid();
        }

        if (!isDone) {
          Standard_ConstructionError::Raise("Pipe construction failure");
        }

        TopoDS_Shape resShape = aBuilder.Shape();

        if (NeedCreateSolid && nn == 1) {
          // Remove top lid from the result.
          resShape = RemoveFaces(resShape, aBuilder.LastShape());
        }

        aSeqRes.Append(resShape);

        // Create groups.
        if (isCreateGroups) {
          // Make groups.
          TopTools_SequenceOfShape aGroups[5];

          if (!DoGroups(aBuilder, aGroups)) {
            Standard_ConstructionError::Raise("Generate groups failure");
          }

          // Get shapes from all groups.
          for (iGrp = 0; iGrp < 5; ++iGrp) {
            aMapResGroups[iGrp].Bind(resShape, aGroups[iGrp]);
          }
        }
      }
      // create wirepath and sequences of shapes for last part
      BRep_Builder B;
      TopoDS_Wire tmpW;
      B.MakeWire(tmpW);
      for (i=num1; i<=Edges.Length(); i++) {
        B.Add(tmpW,TopoDS::Edge(Edges.Value(i)));
      }
      TopTools_SequenceOfShape aTmpSeqBases;
      TopTools_SequenceOfShape aTmpSeqLocs;
      for (i=num2; i<=aSeqLocs.Length(); i++) {
        aTmpSeqBases.Append(aSeqBases.Value(i));
        aTmpSeqLocs.Append(aSeqLocs.Value(i));
      }
      // make pipe for last part
      BRepOffsetAPI_MakePipeShell aBuilder(tmpW);
      GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(tmpW);
      if (theBestMode == GeomFill_IsDiscreteTrihedron)
        aBuilder.SetDiscreteMode();
      Standard_Integer nbShapes = aTmpSeqBases.Length();
      for (i=1; i<=nbShapes; i++) {
        TopoDS_Shape aShapeLoc = aTmpSeqLocs.Value(i);
        TopoDS_Vertex aVert = TopoDS::Vertex(aShapeLoc);
        aBuilder.Add(aTmpSeqBases.Value(i), aVert, theWithContact, theWithCorrect);
      }
      if (!aBuilder.IsReady()) {
        Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
      }

      Standard_Boolean isDone = BuildPipeShell(aBuilder);

      if (isDone && NeedCreateSolid) {
        isDone = aBuilder.MakeSolid();
      }

      if (!isDone) {
        Standard_ConstructionError::Raise("Pipe construction failure");
      }

      TopoDS_Shape resShape = aBuilder.Shape();

      if (NeedCreateSolid) {
        // Remove bottom lid from the result.
        resShape = RemoveFaces(resShape, aBuilder.FirstShape());
      }

      aSeqRes.Append(resShape);

      // Create groups.
      if (isCreateGroups) {
        // Make groups.
        TopTools_SequenceOfShape aGroups[5];

        if (!DoGroups(aBuilder, aGroups)) {
          Standard_ConstructionError::Raise("Generate groups failure");
        }

        // Get shapes from all groups.
        for (iGrp = 0; iGrp < 5; ++iGrp) {
          aMapResGroups[iGrp].Bind(resShape, aGroups[iGrp]);
        }
      }

      // make sewing for result
      Handle(BRepBuilderAPI_Sewing) aSewing = new BRepBuilderAPI_Sewing;
      aSewing->SetTolerance(Precision::Confusion());
      aSewing->SetFaceMode(Standard_True);
      aSewing->SetFloatingEdgesMode(Standard_False);
      aSewing->SetNonManifoldMode(Standard_False);
      for (i=1; i<=aSeqRes.Length(); i++) {
        aSewing->Add(aSeqRes.Value(i));
      }
      aSewing->Perform();
      aShape = aSewing->SewedShape();

      if (NeedCreateSolid && aShape.ShapeType() == TopAbs_SHELL) {
        // Build a solid.
        BRepBuilderAPI_MakeSolid aMkSolid;

        aMkSolid.Add(TopoDS::Shell(aShape));

        if (!aMkSolid.IsDone()) {
          Standard_ConstructionError::Raise("Can't create solid pipe");
        }

        TopoDS_Solid                aSolid = aMkSolid.Solid();
        BRepClass3d_SolidClassifier aSC(aSolid);

        aSC.PerformInfinitePoint(Precision::Confusion());

        if (aSC.State() == TopAbs_IN) {
          aShape = aSolid.Reversed();
        } else {
          aShape = aSolid;
        }
      }

      if (isCreateGroups) {
        // Replase Group shapes by modified ones.
        TopTools_SequenceOfShape aSeqGroups[5];

        // For each group.
        for (iGrp = 0; iGrp < 5; ++iGrp) {
          // For each pipe
          for (i = 1; i <= aSeqRes.Length(); ++i) {
            if (iGrp == GROUP_DOWN && i > 1) {
              // For DOWN group we use only the first pipe.
              continue;
            }

            if (iGrp == GROUP_UP && i < aSeqRes.Length()) {
              // For UP group we use only the last pipe.
              continue;
            }

            const TopTools_SequenceOfShape &aShapes =
              aMapResGroups[iGrp].Find(aSeqRes.Value(i));
            Standard_Integer                j;

            // For each sub-shape of pipe
            for (j = 1; j <= aShapes.Length(); ++j) {
              const TopoDS_Shape &aGrpShape = aShapes.Value(j);

              if (aSewing->IsModifiedSubShape(aGrpShape)) {
                // Use the shape modified by sewing.
                const TopoDS_Shape &aModifGrpShape =
                  aSewing->ModifiedSubShape(aGrpShape);

                aSeqGroups[iGrp].Append(aModifGrpShape);
              } else {
                // Use the shape as it is.
                aSeqGroups[iGrp].Append(aGrpShape);
              }
            }
          }
        }

        // Fill groups
        TopTools_IndexedMapOfShape anIndices;

        TopExp::MapShapes(aShape, anIndices);

        if (!FillGroups(aSeqGroups, anIndices, theGroups)) {
          Standard_ConstructionError::Raise("Generate groups failure");
        }
      }
  }
  else {
      // old implementation without splitting
      BRepOffsetAPI_MakePipeShell aBuilder(aWirePath);
      GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(aWirePath);
      if (theBestMode == GeomFill_IsDiscreteTrihedron)
        aBuilder.SetDiscreteMode();

      Standard_Integer nbShapes = aSeqBases.Length();
      Standard_Integer step = nbShapes/nbBases;

      if (nbShapes < nbBases || fmod((double)nbShapes, (double)nbBases)) {
        Standard_ConstructionError::Raise("Invalid sections were specified for building pipe");
      }
      Standard_Integer ind =0;
      Standard_Real aTolConf = Precision::Confusion();
      Standard_Real aTolAng  = Precision::Angular();

      for (i = 1; i <= nbShapes && ind < nbShapes; i++) { //i+nbBases <= nbShapes
        TopTools_SequenceOfShape usedBases;
        Standard_Integer j = 1;
        for (; j <= nbBases; j++) {
          ind = i + (j-1)*step;
          TopoDS_Shape aWireProf = aSeqBases.Value(ind);
          usedBases.Append(aWireProf);
          if (nbLocs) {
            TopoDS_Shape aShapeLoc = aSeqLocs.Value(j);
            TopoDS_Vertex aVert = TopoDS::Vertex(aShapeLoc);
            aBuilder.Add(aWireProf, aVert, theWithContact, theWithCorrect);
          }
          else
            aBuilder.Add(aWireProf, theWithContact, theWithCorrect);
        }
        if (!aBuilder.IsReady()) {
          Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
        }
      
        aBuilder.SetTolerance(aTolConf, aTolConf, aTolAng);

        Standard_Boolean isDone = BuildPipeShell(aBuilder);

        if (isDone && NeedCreateSolid) {
          isDone = aBuilder.MakeSolid();
        }

        if (!isDone) {
          Standard_ConstructionError::Raise("Pipe construction failure");
        }
        aShape = aBuilder.Shape();

        if (isCreateGroups) {
          // Make groups.
          TopTools_SequenceOfShape aSeqGroups[5];

          if (!DoGroups(aBuilder, aSeqGroups)) {
            Standard_ConstructionError::Raise("Generate groups failure");
          }

          // Fill the groups.
          Handle(TColStd_HArray1OfInteger) aGroupIds[5];
          TopTools_IndexedMapOfShape       anIndices;
          const TopoDS_Shape               aResult = aBuilder.Shape();

          TopExp::MapShapes(aResult, anIndices);

          if (!FillGroups(aSeqGroups, anIndices, theGroups)) {
            Standard_ConstructionError::Raise("Generate groups failure");
          }
        }
        aSeqFaces.Append(aShape);
        for (j = 1; j <=usedBases.Length(); j++)
          aBuilder.Delete(usedBases.Value(j));
      }
  }

  return aShape;
}

//=======================================================================
//function : CreatePipeForShellSections
//purpose  : auxilary for Execute()
//=======================================================================
static TopoDS_Shape CreatePipeForShellSections(const TopoDS_Wire& aWirePath,
                                               GEOMImpl_IPipe* aCI)
{
  int i,j;
  BRep_Builder B;

  GEOMImpl_IPipeShellSect* aCIDS = (GEOMImpl_IPipeShellSect*)aCI;
  Handle(TColStd_HSequenceOfTransient) aBasesObjs = aCIDS->GetBases();
  Handle(TColStd_HSequenceOfTransient) aSubBasesObjs = aCIDS->GetSubBases();
  Handle(TColStd_HSequenceOfTransient) aLocObjs = aCIDS->GetLocations();
  Standard_Boolean aWithContact = (aCIDS->GetWithContactMode());
  Standard_Boolean aWithCorrect = (aCIDS->GetWithCorrectionMode());
  Standard_Boolean isGenerateGroups = aCIDS->GetGenerateGroups();

  Standard_Integer nbBases = aBasesObjs->Length(),
    nbSubBases = (aSubBasesObjs.IsNull() ? 0 :aSubBasesObjs->Length()),
    nbLocs = (aLocObjs.IsNull() ? 0 :aLocObjs->Length());

  if (nbLocs != nbBases) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise("Number of sections is not equal to number of locations ");
  }
  if (nbSubBases && nbSubBases != nbBases) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise("Number of sections is not equal to number of subsections ");
  }

  TopTools_SequenceOfShape VLocs;
  for (i=1; i<=nbBases; i++) {
    Handle(Standard_Transient) anItemLoc = aLocObjs->Value(i);
    if (anItemLoc.IsNull())
      continue;
    Handle(GEOM_Function) aRefLoc = Handle(GEOM_Function)::DownCast(anItemLoc);
    TopoDS_Shape aShapeLoc = aRefLoc->GetValue();
    if (aShapeLoc.IsNull() || aShapeLoc.ShapeType() != TopAbs_VERTEX)
      continue;
    VLocs.Append(aShapeLoc);
  }
  nbLocs = VLocs.Length();
  if (nbLocs != nbBases) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise("One of location shapes is not a vertex");
  }
  // split wire path by location points
  TColgp_SequenceOfPnt PLocs;
  for (i=1; i<=nbLocs; i++) {
    TopoDS_Vertex V = TopoDS::Vertex(VLocs.Value(i));
    PLocs.Append(BRep_Tool::Pnt(V));
  }

  TopTools_SequenceOfShape Edges;
  TopTools_SequenceOfShape Wires;
  ShapeAnalysis_Edge sae;

  if (nbLocs==2) {
    TopExp_Explorer anExp;
    for (anExp.Init(aWirePath, TopAbs_EDGE); anExp.More(); anExp.Next()) {
      Edges.Append(anExp.Current());
    }
    Standard_Integer Num1 = 0;
    Standard_Integer Num2 = 0;
    for (i=1; i<=Edges.Length(); i++) {
      TopoDS_Edge E = TopoDS::Edge(Edges.Value(i));
      double tol = BRep_Tool::Tolerance(E);
      TopoDS_Vertex V1 = sae.FirstVertex(E);
      TopoDS_Vertex V2 = sae.LastVertex(E);
      gp_Pnt P1 = BRep_Tool::Pnt(V1);
      gp_Pnt P2 = BRep_Tool::Pnt(V2);
      if (P1.Distance(PLocs.First()) < tol) {
        Num1 = i;
      }
      if (P2.Distance(PLocs.Last()) < tol) {
        Num2 = i;
      }
    }
    if (Num1>0 && Num2>0) {
      TopoDS_Wire W;
      B.MakeWire(W);
      for (i=Num1; i<=Num2; i++) {
        B.Add(W,Edges.Value(i));
      }
      Wires.Append(W);
    }
    else {
      Wires.Append(aWirePath);
    }
  }
  else {
    TopExp_Explorer anExp;
    for (anExp.Init(aWirePath, TopAbs_EDGE); anExp.More(); anExp.Next()) {
      Edges.Append(anExp.Current());
    }
    TopoDS_Edge edge = TopoDS::Edge(Edges.First());
    double tol = BRep_Tool::Tolerance(edge);
    TopoDS_Vertex VF = sae.FirstVertex(edge);
    gp_Pnt PF = BRep_Tool::Pnt(VF);
    //cout<<"PF("<<PF.X()<<","<<PF.Y()<<","<<PF.Z()<<")"<<endl;
    if (PF.Distance(PLocs.First()) > tol) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise
        ("First location shapes is not coincided with first vertex of aWirePath");
    }
    VLocs.ChangeValue(1) = VF;
    edge = TopoDS::Edge(Edges.Last());
    tol = BRep_Tool::Tolerance(edge);
    TopoDS_Vertex VL = sae.LastVertex(edge);
    gp_Pnt PL = BRep_Tool::Pnt(VL);
    if (PL.Distance(PLocs.Last()) > tol) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise
        ("Last location shapes is not coincided with last vertex of aWirePath");
    }
    VLocs.ChangeValue(nbLocs) = VL;
    int jcurr = 2;
    TopTools_SequenceOfShape tmpEdges;
    for (i=1; i<=Edges.Length() && jcurr<nbLocs; i++) {
      TopoDS_Edge E = TopoDS::Edge(Edges.Value(i));
      tol = BRep_Tool::Tolerance(E);
      TopoDS_Vertex V1 = sae.FirstVertex(E);
      TopoDS_Vertex V2 = sae.LastVertex(E);
      gp_Pnt P1 = BRep_Tool::Pnt(V1);
      gp_Pnt P2 = BRep_Tool::Pnt(V2);
      if (P2.Distance(PLocs.Value(jcurr)) < tol) {
        // make wire from current edge and add created
        // wire to Wires
        TopoDS_Wire W;
        B.MakeWire(W);
        for (j=1; j<=tmpEdges.Length(); j++)
          B.Add(W,tmpEdges.Value(j));
        B.Add(W,E);
        Wires.Append(W);
        VLocs.ChangeValue(jcurr) = V2;
        jcurr++;
        tmpEdges.Clear();
      }
      else {
        // find distance between E and aLocs(jcurr)
        double fp,lp;
        Handle(Geom_Curve) C = BRep_Tool::Curve(E,fp,lp);
        GeomAPI_ProjectPointOnCurve PPCurve (PLocs.Value(jcurr),C);
        if (PPCurve.NbPoints()>0 &&
            PLocs.Value(jcurr).Distance(PPCurve.Point(1)) < tol) {
          double param = PPCurve.Parameter(1);
          gp_Pnt PC1;
          C->D0(param,PC1);
          // split current edge
          Handle(Geom_TrimmedCurve) tc1 = new Geom_TrimmedCurve(C,fp,param);
          Handle(Geom_TrimmedCurve) tc2 = new Geom_TrimmedCurve(C,param,lp);
          TopoDS_Edge E1,E2;
          gp_Pnt Pfp;
          C->D0(fp,Pfp);
          if (Pfp.Distance(P1)<tol) {
            B.MakeEdge(E1,tc1,tol);
            B.Add(E1,V1);
            TopoDS_Shape tmpV = VLocs.Value(jcurr).Oriented(TopAbs_REVERSED);
            B.Add(E1,TopoDS::Vertex(tmpV));
            tmpEdges.Append(E1);
            B.MakeEdge(E2,tc2,tol);
            tmpV = VLocs.Value(jcurr).Oriented(TopAbs_FORWARD);
            B.Add(E2,TopoDS::Vertex(tmpV));
            B.Add(E2,V2);
          }
          else {
            B.MakeEdge(E1,tc2,tol);
            TopoDS_Shape tmpV = VLocs.Value(jcurr).Oriented(TopAbs_FORWARD);
            B.Add(E1,TopoDS::Vertex(tmpV));
            B.Add(E1,V1);
            E1.Reverse();
            tmpEdges.Append(E1);
            B.MakeEdge(E2,tc1,tol);
            B.Add(E2,V2);
            tmpV = VLocs.Value(jcurr).Oriented(TopAbs_REVERSED);
            B.Add(E2,TopoDS::Vertex(tmpV));
            E2.Reverse();
          }
          // create wire from tmpEdges
          TopoDS_Wire W;
          B.MakeWire(W);
          for (j=1; j<=tmpEdges.Length(); j++)
            B.Add(W,tmpEdges.Value(j));
          Wires.Append(W);
          jcurr++;
          tmpEdges.Clear();
          Edges.Remove(i);
          Edges.InsertAfter(i-1,E1);
          Edges.InsertAfter(i,E2);
        }
        else {
          tmpEdges.Append(E);
        }
      }
    }
    // create wire from other edges
    TopoDS_Wire W;
    B.MakeWire(W);
    for (; i<=Edges.Length(); i++)
      B.Add(W,Edges.Value(i));
    Wires.Append(W);
  }

  if (Wires.Length() != nbLocs-1) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise
      ("One of location shapes is not lied on the path");
  }

  TopTools_SequenceOfShape aGroups[5];
  TopoDS_Compound          aComp;
  B.MakeCompound(aComp);
  for (i = 1; i < nbBases; i++) {
    TopoDS_Wire WPath = TopoDS::Wire(Wires.Value(i));
    // 1 section
    Handle(Standard_Transient) anItem1 = aBasesObjs->Value(i);
    if (anItem1.IsNull())
      continue;
    Handle(GEOM_Function) aRefBase1 = Handle(GEOM_Function)::DownCast(anItem1);
    if (aRefBase1.IsNull())
      continue;
    TopoDS_Shape aShBase1 = aRefBase1->GetValue();
    if (aShBase1.IsNull())
      continue;
    TopAbs_ShapeEnum aType1 = aShBase1.ShapeType();
    // 2 section
    Handle(Standard_Transient) anItem2 = aBasesObjs->Value(i+1);
    if (anItem2.IsNull())
      continue;
    Handle(GEOM_Function) aRefBase2 = Handle(GEOM_Function)::DownCast(anItem2);
    if (aRefBase2.IsNull())
      continue;
    TopoDS_Shape aShBase2 = aRefBase2->GetValue();
    if (aShBase2.IsNull())
      continue;
    TopAbs_ShapeEnum aType2 = aShBase2.ShapeType();

    bool OkSec = (aType1==TopAbs_SHELL || aType1==TopAbs_FACE) &&
                 (aType2==TopAbs_SHELL || aType2==TopAbs_FACE);
    if (!OkSec) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("One of section shapes has invalid type");
    }

    bool CreateFewSolids = false;
    // compare sections
    TopExp_Explorer anExp;
    Standard_Integer nbf1 = 0;
    for (anExp.Init(aShBase1, TopAbs_FACE); anExp.More(); anExp.Next()) {
      nbf1++;
    }
    Standard_Integer nbf2 = 0;
    for (anExp.Init(aShBase2, TopAbs_FACE); anExp.More(); anExp.Next()) {
      nbf2++;
    }
    if (nbf1==nbf2) {
      CreateFewSolids = true;
    }

    if (!CreateFewSolids) {
      // we can create only one solid
      TopoDS_Shape aWire1, aWire2;
      // prepare aWire1
      if (aType1==TopAbs_SHELL) {
        // create wire as boundary contour if shell is no closed
        // get free boundary shapes
        ShapeAnalysis_FreeBounds anAnalizer(aShBase1);
        TopoDS_Compound aClosed = anAnalizer.GetClosedWires();
        //TopExp_Explorer anExp;
        Standard_Integer NbWires = 0;
        for (anExp.Init(aClosed, TopAbs_WIRE); anExp.More(); anExp.Next()) {
          NbWires++;
          aWire1 = anExp.Current();
        }
        if (NbWires!=1) {
          // bad case
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Bad shell is used as section ");
        }
      }
      else { // aType1==TopAbs_FACE
        TopExp_Explorer aExpW(aShBase1,TopAbs_WIRE);
        aWire1 = aExpW.Current();
      }
      // prepare aWire2
      if (aType2==TopAbs_SHELL) {
        // create wire as boundary contour if shell is no closed
        // get free boundary shapes
        ShapeAnalysis_FreeBounds anAnalizer(aShBase2);
        TopoDS_Compound aClosed = anAnalizer.GetClosedWires();
        //TopExp_Explorer anExp;
        Standard_Integer NbWires = 0;
        for (anExp.Init(aClosed, TopAbs_WIRE); anExp.More(); anExp.Next()) {
          NbWires++;
          aWire2 = anExp.Current();
        }
        if (NbWires!=1) {
          // bad case
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Bad shell is used as section ");
        }
      }
      else { // aType2==TopAbs_FACE
        TopExp_Explorer aExpW(aShBase2,TopAbs_WIRE);
        aWire2 = aExpW.Current();
      }
      // make pipe using aWire1 and aWire2
      if (!aWire1.IsNull() && !aWire2.IsNull()) {
        //BRepOffsetAPI_MakePipeShell aBuilder(aWirePath);
        BRepOffsetAPI_MakePipeShell aBuilder(WPath);
        GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(WPath);
        if (theBestMode == GeomFill_IsDiscreteTrihedron)
          aBuilder.SetDiscreteMode();
        aBuilder.Add(aWire1, TopoDS::Vertex(VLocs(i)),
                     aWithContact, aWithCorrect);
        aBuilder.Add(aWire2, TopoDS::Vertex(VLocs(i+1)),
                     aWithContact, aWithCorrect);
        if (!aBuilder.IsReady()) {
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
        }

        BuildPipeShell(aBuilder);

        TopoDS_Shape             aShape = aBuilder.Shape();
        TopTools_SequenceOfShape aLocalGroups[5];

        // Create groups.
        if (isGenerateGroups) {
          // Make groups.
          if (!DoGroups(aBuilder, aLocalGroups)) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Generate groups failure");
          }

          // Clear the groups Down and Up.
          aLocalGroups[GROUP_DOWN].Clear();
          aLocalGroups[GROUP_UP].Clear();
        }

        TopoDS_Shell aShell;
        B.MakeShell(aShell);
        for (anExp.Init(aShape, TopAbs_FACE); anExp.More(); anExp.Next()) {
          B.Add(aShell,anExp.Current());
        }
        for (anExp.Init(aShBase1, TopAbs_FACE); anExp.More(); anExp.Next()) {
          B.Add(aShell,anExp.Current());

          if (isGenerateGroups && i == 1) {
            aLocalGroups[GROUP_DOWN].Append(anExp.Current());
          }
        }
        for (anExp.Init(aShBase2, TopAbs_FACE); anExp.More(); anExp.Next()) {
          B.Add(aShell,anExp.Current());

          if (isGenerateGroups && i == nbBases - 1) {
            aLocalGroups[GROUP_UP].Append(anExp.Current());
          }
        }
        // make sewing for this shell
        Handle(BRepBuilderAPI_Sewing) aSewing = new BRepBuilderAPI_Sewing;
        aSewing->SetTolerance(Precision::Confusion());
        aSewing->SetFaceMode(Standard_True);
        aSewing->SetFloatingEdgesMode(Standard_False);
        aSewing->SetNonManifoldMode(Standard_False);
        for (anExp.Init(aShell, TopAbs_FACE); anExp.More(); anExp.Next()) {
          aSewing->Add(anExp.Current());
        }
        aSewing->Perform();
        const TopoDS_Shape aSewShape = aSewing->SewedShape();
        if (aSewShape.ShapeType() == TopAbs_SHELL) {
          aShell = TopoDS::Shell(aSewShape);
          GProp_GProps aSystem;
          BRepGProp::VolumeProperties(aShell, aSystem);
          if (aSystem.Mass()<0) {
            aShell.Reverse();
          }
          if (BRep_Tool::IsClosed(aShell)) {
            TopoDS_Solid aSolid;
            B.MakeSolid(aSolid);
            B.Add(aSolid,aShell);
            B.Add(aComp,aSolid);
          }
          else {
            B.Add(aComp,aShell);
          }
        }
        else {
          B.Add(aComp,aShell);
        }

        if (isGenerateGroups) {
          Standard_Integer iGrp;

          for (iGrp = 0; iGrp < 5; ++iGrp) {
            Standard_Integer j;

            // For each sub-shape of pipe
            for (j = 1; j <= aLocalGroups[iGrp].Length(); ++j) {
              const TopoDS_Shape &aGrpShape = aLocalGroups[iGrp].Value(j);

              if (aSewing->IsModifiedSubShape(aGrpShape)) {
                // Use the shape modified by sewing.
                const TopoDS_Shape &aModifGrpShape =
                  aSewing->ModifiedSubShape(aGrpShape);

                aGroups[iGrp].Append(aModifGrpShape);
              } else {
                // Use the shape as it is.
                aGroups[iGrp].Append(aGrpShape);
              }
            }
          }
        }
      }
    }
    else {
      // main block - creation few solids (for each pair of faces)
      TopTools_MapOfShape aFaces1,aFaces2;
      for (anExp.Init(aShBase1, TopAbs_FACE); anExp.More(); anExp.Next()) {
        aFaces1.Add(anExp.Current());
      }
      for (anExp.Init(aShBase2, TopAbs_FACE); anExp.More(); anExp.Next()) {
        aFaces2.Add(anExp.Current());
      }
      // creating map of edge faces
      TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces1;
      TopExp::MapShapesAndAncestors(aShBase1, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces1);
      TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces2;
      TopExp::MapShapesAndAncestors(aShBase2, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces2);

      // constuct map face->face
      TopTools_IndexedDataMapOfShapeShape FF;
      TopoDS_Shape FS1,FS2;
      if (nbSubBases==0) {
        // find edge the most distant from location point
        // (this edge is not shared by two faces)
        double maxdist = 0.;
        TopoDS_Shape E1;
        TopoDS_Vertex V11,V21;
        for (j=1; j<=aMapEdgeFaces1.Extent(); j++) {
          TopoDS_Shape tmp = aMapEdgeFaces1.FindKey(j);
          const TopTools_ListOfShape& aList = aMapEdgeFaces1.FindFromKey(tmp);
          if (aList.Extent()>1)
            continue;
          TopExp_Explorer expv;
          expv.Init(tmp, TopAbs_VERTEX);
          TopoDS_Vertex V1 = TopoDS::Vertex(expv.Current());
          expv.Next();
          TopoDS_Vertex V2 = TopoDS::Vertex(expv.Current());
          gp_Pnt P1 = BRep_Tool::Pnt(V1);
          gp_Pnt P2 = BRep_Tool::Pnt(V2);
          double dist = PLocs.Value(i).Distance(P1) + PLocs.Value(i).Distance(P2);
          if (dist>maxdist) {
            E1 = tmp;
            V11 = V1;
            V21 = V2;
            TopTools_ListIteratorOfListOfShape anIter(aList);
            FS1 = anIter.Value();
            maxdist = dist;
          }
        }
        // main direction for comparing
        gp_Vec VM(PLocs.Value(i),PLocs.Value(i+1));
        // find corresponding edge from next section
        double minang = M_PI;
        gp_Pnt P11 = BRep_Tool::Pnt(V11);
        gp_Pnt P21 = BRep_Tool::Pnt(V21);
        TopoDS_Shape E2;
        TopoDS_Vertex V12,V22;
        for (j=1; j<=aMapEdgeFaces2.Extent(); j++) {
          TopoDS_Shape tmp = aMapEdgeFaces2.FindKey(j);
          const TopTools_ListOfShape& aList = aMapEdgeFaces2.FindFromKey(tmp);
          if (aList.Extent()>1)
            continue;
          TopExp_Explorer expv;
          expv.Init(tmp, TopAbs_VERTEX);
          TopoDS_Vertex V1tmp = TopoDS::Vertex(expv.Current());
          expv.Next();
          TopoDS_Vertex V2tmp = TopoDS::Vertex(expv.Current());
          gp_Pnt P1tmp = BRep_Tool::Pnt(V1tmp);
          gp_Pnt P2tmp = BRep_Tool::Pnt(V2tmp);
          double d1 = P1tmp.Distance(P11) + P2tmp.Distance(P21);
          double d2 = P1tmp.Distance(P21) + P2tmp.Distance(P11);
          TopoDS_Vertex V1,V2;
          gp_Pnt P1,P2;
          if (d1>d2) {
            V1 = V2tmp; P1 = P2tmp;
            V2 = V1tmp; P2 = P1tmp;
          }
          else {
            V1 = V1tmp; P1 = P1tmp;
            V2 = V2tmp; P2 = P2tmp;
          }
          gp_Vec Vec1(P11,P1);
          gp_Vec Vec2(P21,P2);
          double ang = fabs(Vec1.Angle(VM)) + fabs(Vec2.Angle(VM));
          if (ang<minang) {
            E2 = tmp;
            V12 = V1;
            V22 = V2;
            TopTools_ListIteratorOfListOfShape anIter(aList);
            FS2 = anIter.Value();
            minang = ang;
          }
        }
        // put all pairs to map FF
        FF.Add(FS1,FS2);
        FF.Add(E1,E2);
        FF.Add(V11,V12);
        FF.Add(V21,V22);

        // add pairs of edges to FF
        bool stat =  FillForOtherEdges(FS1,E1,V11,FF);
        if (!stat) {
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("FindForOtherEdges: Can not mapping other egdes");
        }

      }
      else {
        { // 1 section
          Handle(Standard_Transient) anItem = aSubBasesObjs->Value(i);
          if (anItem.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          Handle(GEOM_Function) aRefBase = Handle(GEOM_Function)::DownCast(anItem);
          if (aRefBase.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          TopoDS_Shape aSh = aRefBase->GetValue();
          if (aSh.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          if (aSh.ShapeType()!=TopAbs_FACE) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          FS1 = aSh;
        }
        { // 2 section
          Handle(Standard_Transient) anItem = aSubBasesObjs->Value(i+1);
          if (anItem.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          Handle(GEOM_Function) aRefBase = Handle(GEOM_Function)::DownCast(anItem);
          if (aRefBase.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          TopoDS_Shape aSh = aRefBase->GetValue();
          if (aSh.IsNull()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          if (aSh.ShapeType()!=TopAbs_FACE) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid subbase shape");
          }
          FS2 = aSh;
        }

        if (!aFaces1.Contains(FS1) || !aFaces2.Contains(FS2)) {
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Invalid subbase shape");
        }

        FF.Add(FS1,FS2);

        // add pairs of edges to FF
        bool stat =  FillCorrespondingEdges(FS1, FS2, TopoDS::Vertex(VLocs(i)),
                                            TopoDS::Vertex(VLocs(i+1)), WPath, FF);
        if (!stat) {
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Can not create correct pipe");
        }
      }

      FindNextPairOfFaces(FS1, aMapEdgeFaces1, aMapEdgeFaces2, FF, aCI);

      // make pipe for each pair of faces
      for (j=1; j<=FF.Extent(); j++) {
        TopoDS_Shape F1 = FF.FindKey(j);
        if (F1.ShapeType() != TopAbs_FACE)
          continue;
        TopoDS_Shape F2 = FF.FindFromIndex(j);
        TopExp_Explorer aExpW1(F1,TopAbs_WIRE);
        TopoDS_Wire aWire1 = TopoDS::Wire(aExpW1.Current());
        TopExp_Explorer aExpW2(F2,TopAbs_WIRE);
        TopoDS_Wire aWire2 = TopoDS::Wire(aExpW2.Current());
        // make pipe using aWire1 and aWire2
        if (!aWire1.IsNull() && !aWire2.IsNull()) {
          BRepOffsetAPI_MakePipeShell aBuilder(WPath);
          GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(WPath);
          if (theBestMode == GeomFill_IsDiscreteTrihedron)
            aBuilder.SetDiscreteMode();
          aBuilder.Add(aWire1, TopoDS::Vertex(VLocs(i)),
                       aWithContact, aWithCorrect);
          aBuilder.Add(aWire2, TopoDS::Vertex(VLocs(i+1)),
                       aWithContact, aWithCorrect);
          if (!aBuilder.IsReady()) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Invalid input data for building PIPE: bases are invalid");
          }

          BuildPipeShell(aBuilder);

          TopoDS_Shape             aShape = aBuilder.Shape();
          TopTools_SequenceOfShape aLocalGroups[5];

          // Create groups.
          if (isGenerateGroups) {
            // Make groups.
            if (!DoGroups(aBuilder, aLocalGroups)) {
              if (aCI) delete aCI;
              Standard_ConstructionError::Raise("Generate groups failure");
            }

            // Clear the groups Down and Up.
            aLocalGroups[GROUP_DOWN].Clear();
            aLocalGroups[GROUP_UP].Clear();

            if (i == 1) {
              aLocalGroups[GROUP_DOWN].Append(F1);
            }

            if (i == nbBases - 1) {
              aLocalGroups[GROUP_UP].Append(F2);
            }
          }

          TopoDS_Shell aShell;
          B.MakeShell(aShell);
          for (anExp.Init(aShape, TopAbs_FACE); anExp.More(); anExp.Next()) {
            B.Add(aShell,anExp.Current());
          }

          B.Add(aShell,F1);
          B.Add(aShell,F2);
          // make sewing for this shell
          Handle(BRepBuilderAPI_Sewing) aSewing = new BRepBuilderAPI_Sewing;
          aSewing->SetTolerance(Precision::Confusion());
          aSewing->SetFaceMode(Standard_True);
          aSewing->SetFloatingEdgesMode(Standard_False);
          aSewing->SetNonManifoldMode(Standard_False);
          for (anExp.Init(aShell, TopAbs_FACE); anExp.More(); anExp.Next()) {
            aSewing->Add(anExp.Current());
          }
          aSewing->Perform();
          const TopoDS_Shape aSewShape = aSewing->SewedShape();
          if (aSewShape.ShapeType() == TopAbs_SHELL) {
            aShell = TopoDS::Shell(aSewShape);
            GProp_GProps aSystem;
            BRepGProp::VolumeProperties(aShell, aSystem);
            if (aSystem.Mass()<0) {
              //cout<<"aSewShape is reversed"<<endl;
              aShell.Reverse();
            }
            if (BRep_Tool::IsClosed(aShell)) {
              TopoDS_Solid aSolid;
              B.MakeSolid(aSolid);
              B.Add(aSolid,aShell);
              B.Add(aComp,aSolid);
            }
            else {
              B.Add(aComp,aShell);
            }
          }
          else {
            B.Add(aComp,aShell);
          }

          if (isGenerateGroups) {
            // Replase Group shapes by modified ones.
            Standard_Integer iGrp;

            // For each group.
            for (iGrp = 0; iGrp < 5; ++iGrp) {
              Standard_Integer j;

              // For each sub-shape of pipe
              for (j = 1; j <= aLocalGroups[iGrp].Length(); ++j) {
                const TopoDS_Shape &aGrpShape = aLocalGroups[iGrp].Value(j);

                if (aSewing->IsModifiedSubShape(aGrpShape)) {
                  // Use the shape modified by sewing.
                  const TopoDS_Shape &aModifGrpShape =
                    aSewing->ModifiedSubShape(aGrpShape);

                  aGroups[iGrp].Append(aModifGrpShape);
                } else {
                  // Use the shape as it is.
                  aGroups[iGrp].Append(aGrpShape);
                }
              }
            }
          }
        }
      }
    }
  }

  if (isGenerateGroups) {
    // Fill the groups.
    Handle(TColStd_HArray1OfInteger) aGroupIds[5];
    TopTools_IndexedMapOfShape       anIndices;

    TopExp::MapShapes(aComp, anIndices);

    if (!FillGroups(aGroups, anIndices, aGroupIds)) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("Generate groups failure");
    }

    StoreGroups(aCI, aGroupIds);
  }

  return aComp;
}

//=======================================================================
//function : CreatePipeShellsWithoutPath
//purpose  : auxilary for Execute()
//=======================================================================
static TopoDS_Shape CreatePipeShellsWithoutPath(GEOMImpl_IPipe* aCI)
{
  //cout<<"CreatePipeShellsWithoutPath"<<endl;
  int i,j;
  BRep_Builder B;

  GEOMImpl_IPipeShellSect* aCIDS = (GEOMImpl_IPipeShellSect*)aCI;
  // shell sections
  Handle(TColStd_HSequenceOfTransient) aBasesObjs = aCIDS->GetBases();
  // vertex for recognition
  Handle(TColStd_HSequenceOfTransient) VObjs = aCIDS->GetLocations();
  Standard_Boolean isGenerateGroups = aCIDS->GetGenerateGroups();

  Standard_Integer nbBases = aBasesObjs->Length(),
    nbv = (VObjs.IsNull() ? 0 :VObjs->Length());

  if (nbv != nbBases) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise("Number of shapes for recognition is invalid");
  }

  TopTools_SequenceOfShape aGroups[5];
  TopTools_SequenceOfShape SecVs,Bases;
  for (i=1; i<=nbBases; i++) {
    // vertex
    Handle(Standard_Transient) anItem = VObjs->Value(i);
    if (anItem.IsNull())
      continue;
    Handle(GEOM_Function) aRef = Handle(GEOM_Function)::DownCast(anItem);
    TopoDS_Shape V = aRef->GetValue();
    if (V.IsNull() || V.ShapeType() != TopAbs_VERTEX)
      continue;
    SecVs.Append(V);
    // section
    anItem = aBasesObjs->Value(i);
    if (anItem.IsNull())
      continue;
    aRef = Handle(GEOM_Function)::DownCast(anItem);
    TopoDS_Shape aSh = aRef->GetValue();
    if (aSh.IsNull())
      continue;
    Bases.Append(aSh);
  }
  nbv = SecVs.Length();
  nbBases = Bases.Length();
  if (nbv != nbBases) {
    if (aCI) delete aCI;
    Standard_ConstructionError::Raise("One of shapes for recognition is not a vertex");
  }

  TopoDS_Compound aComp;
  B.MakeCompound(aComp);

  for (i = 1; i < nbBases; i++) {
    MESSAGE ("Make pipe between sections "<<i<<" and "<<i+1);
    TopoDS_Shape aShBase1 = Bases.Value(i);
    TopoDS_Shape aShBase2 = Bases.Value(i+1);
    TopExp_Explorer anExp;
    Standard_Integer nbf1 = 0;
    for (anExp.Init(aShBase1, TopAbs_FACE); anExp.More(); anExp.Next()) {
      nbf1++;
    }
    Standard_Integer nbf2 = 0;
    for (anExp.Init(aShBase2, TopAbs_FACE); anExp.More(); anExp.Next()) {
      nbf2++;
    }
    //cout<<"nbf1="<<nbf1<<" nbf2="<<nbf2<<endl;
    if (nbf1!=nbf2) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("Different number of faces in the sections");
    }

    TopTools_MapOfShape aFaces1,aFaces2;
    TopTools_MapOfShape aBndEdges1;

    for (anExp.Init(aShBase1, TopAbs_FACE); anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aBaseFace1 = anExp.Current();

      if (aFaces1.Add(aBaseFace1)) {
        // Get boundary edges.
        TopExp_Explorer anExpE(aBaseFace1, TopAbs_EDGE);

        for (; anExpE.More(); anExpE.Next()) {
          const TopoDS_Shape &aBaseEdge1 = anExpE.Current();

          if (!aBndEdges1.Add(aBaseEdge1)) {
            aBndEdges1.Remove(aBaseEdge1);
          }
        }
      }
    }
    for (anExp.Init(aShBase2, TopAbs_FACE); anExp.More(); anExp.Next()) {
      aFaces2.Add(anExp.Current());
    }

    // creating map of edge faces
    TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces1;
    TopExp::MapShapesAndAncestors(aShBase1, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces1);
    TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces2;
    TopExp::MapShapesAndAncestors(aShBase2, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces2);

    // constuct map face->face (and sub-shapes)
    TopTools_IndexedDataMapOfShapeShape FF;
    //TopoDS_Shape FS1 = SecFs.Value(i), FS2 = SecFs.Value(i+1);
    TopoDS_Shape FS1, FS2;
    TopoDS_Vertex V1 = TopoDS::Vertex(SecVs(i));
    TopoDS_Vertex V2 = TopoDS::Vertex(SecVs(i+1));
    FindFirstPairFaces(aShBase1, aShBase2, V1, V2, FS1, FS2);

    FF.Add(FS1,FS2);
    MESSAGE ("  first pair of corresponding faces is found");

    // add pairs of edges and vertexes to FF
    bool stat =  FillCorrespondingEdges(FS1, FS2, V1, V2, FF);
    if (!stat) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("Can not create correct pipe");
    }
    MESSAGE ("  correspondences for sub-shapes of first pair of faces is found");

    FindNextPairOfFaces(FS1, aMapEdgeFaces1, aMapEdgeFaces2, FF, aCI);
    MESSAGE ("  other correspondences is found, make pipe for all pairs of faces");

    // make pipe for each pair of faces
    // auxilary map vertex->edge for created pipe edges
    TopTools_IndexedDataMapOfShapeShape VPE;
    ShapeAnalysis_Edge sae;
    //cout<<"FF.Extent()="<<FF.Extent()<<endl;
    int nbff = 0;
    for (j=1; j<=FF.Extent(); j++) {
      TopoDS_Shape F1 = FF.FindKey(j);
      if (F1.ShapeType() != TopAbs_FACE)
        continue;
      TopoDS_Shape F2 = FF.FindFromIndex(j);
      nbff++;

      //if (nbff!=3) continue;

      MESSAGE ("    make pipe for "<<nbff<<" face");

      Handle(Geom_Surface) S1 = BRep_Tool::Surface(TopoDS::Face(F1));
      if (S1->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
        Handle(Geom_RectangularTrimmedSurface) RTS =
          Handle(Geom_RectangularTrimmedSurface)::DownCast(S1);
        S1 = RTS->BasisSurface();
      }
      Handle(Geom_Plane) Pln1 = Handle(Geom_Plane)::DownCast(S1);
      if (Pln1.IsNull()) {
        if (aCI) delete aCI;
        Standard_ConstructionError::Raise("Surface from face is not plane");
      }
      gp_Vec aDir1(Pln1->Axis().Direction());

      Handle(Geom_Surface) S2 = BRep_Tool::Surface(TopoDS::Face(F2));
      if (S2->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
        Handle(Geom_RectangularTrimmedSurface) RTS =
          Handle(Geom_RectangularTrimmedSurface)::DownCast(S2);
        S2 = RTS->BasisSurface();
      }
      Handle(Geom_Plane) Pln2 =
          Handle(Geom_Plane)::DownCast(S2);
      if (Pln2.IsNull()) {
        if (aCI) delete aCI;
        Standard_ConstructionError::Raise("Surface from face is not plane");
      }
      gp_Vec aDir2(Pln2->Axis().Direction());

      gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(SecVs(i)));
      gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(SecVs(i+1)));
      gp_Vec aDir(P1,P2);
      if (fabs(aDir.Angle(aDir1)) > M_PI/2.)
        aDir1.Reverse();
      if (fabs(aDir.Angle(aDir2)) > M_PI/2.)
        aDir2.Reverse();

      TopExp_Explorer anExpE(F1,TopAbs_EDGE);
      TopTools_SequenceOfShape aNewFs;
      TopTools_SequenceOfShape aLocalGroups[5];

      //int nbee=0;
      for (; anExpE.More(); anExpE.Next()) {
        TopoDS_Edge E1 = TopoDS::Edge(anExpE.Current());
        //nbee++;
        if (!FF.Contains(E1))
          MESSAGE ("map FF not contains key E1");

        if (VPE.Contains(E1)) {
          aNewFs.Append(VPE.FindFromKey(E1));
#ifdef _DEBUG_
          MESSAGE ("    using existed face");
#endif
          continue;
        }

        TopoDS_Edge E3 = TopoDS::Edge(FF.FindFromKey(E1));
        TopoDS_Vertex V1 = sae.FirstVertex(E1);
        TopoDS_Vertex V2 = sae.LastVertex(E1);
        if (!FF.Contains(V1))
          MESSAGE ("map FF not contains key V1");
        if (!FF.Contains(V2))
          MESSAGE ("map FF not contains key V2");
        TopoDS_Vertex V3 = TopoDS::Vertex(FF.FindFromKey(V2));
        TopoDS_Vertex V4 = TopoDS::Vertex(FF.FindFromKey(V1));
        TopoDS_Vertex Vtmp = sae.FirstVertex(E3);
        if (Vtmp.IsSame(V4))
          E3.Reverse();
        gp_Pnt P1 = BRep_Tool::Pnt(V1);
        gp_Pnt P2 = BRep_Tool::Pnt(V2);
        gp_Pnt P3 = BRep_Tool::Pnt(V3);
        gp_Pnt P4 = BRep_Tool::Pnt(V4);
        // make E2
        TopoDS_Edge E2;
        Handle(Geom_BSplineCurve) C2;
        if (VPE.Contains(V2)) {
          E2 = TopoDS::Edge(VPE.FindFromKey(V2));
          double fp,lp;
          C2 = Handle(Geom_BSplineCurve)::DownCast(BRep_Tool::Curve(E2,fp,lp));
        }
        else {
          Handle(TColgp_HArray1OfPnt) HAP = new TColgp_HArray1OfPnt(1,2);
          HAP->SetValue(1,P2);
          HAP->SetValue(2,P3);
          GeomAPI_Interpolate anInt(HAP,Standard_False,1.e-7);
          anInt.Load(aDir1,aDir2);
          anInt.Perform();
          C2 = anInt.Curve();
          B.MakeEdge(E2,C2,1.e-7);
          B.Add(E2,TopoDS::Vertex(V2.Oriented(TopAbs_FORWARD)));
          B.Add(E2,TopoDS::Vertex(V3.Oriented(TopAbs_REVERSED)));
          VPE.Add(V2,E2);
        }
        // make E4
        TopoDS_Edge E4;
        Handle(Geom_BSplineCurve) C4;
        if (VPE.Contains(V1)) {
          E4 = TopoDS::Edge(VPE.FindFromKey(V1));
          double fp,lp;
          C4 = Handle(Geom_BSplineCurve)::DownCast(BRep_Tool::Curve(E4,fp,lp));
        }
        else {
          Handle(TColgp_HArray1OfPnt) HAP = new TColgp_HArray1OfPnt(1,2);
          HAP->SetValue(1,P1);
          HAP->SetValue(2,P4);
          GeomAPI_Interpolate anInt(HAP,Standard_False,1.e-7);
          anInt.Load(aDir1,aDir2);
          anInt.Perform();
          C4 = anInt.Curve();
          B.MakeEdge(E4,anInt.Curve(),1.e-7);
          B.Add(E4,TopoDS::Vertex(V1.Oriented(TopAbs_FORWARD)));
          B.Add(E4,TopoDS::Vertex(V4.Oriented(TopAbs_REVERSED)));
          VPE.Add(V1,E4);
        }

        TopoDS_Wire W;
        B.MakeWire(W);
        B.Add(W,E1);
        B.Add(W,E2);
        B.Add(W,E3);
        B.Add(W,E4.Reversed());

        // make surface

        double fp,lp;
        Handle(Geom_Curve) C1 = BRep_Tool::Curve(E1,fp,lp);
        //bool IsConicC1 = false;
        //if (C1->IsKind(STANDARD_TYPE(Geom_Conic))) {
        //  IsConicC1 = true;
        //  cout<<"C1 - Geom_Conic"<<endl;
        //}
        if (C1->IsKind(STANDARD_TYPE(Geom_Line)) || C1->IsKind(STANDARD_TYPE(Geom_Conic))) {
          C1 = new Geom_TrimmedCurve(C1,fp,lp);
        }
        //if (IsConicC1) {
        //  double tol = BRep_Tool::Tolerance(E1);
        //  GeomConvert_ApproxCurve ApxC1(C1,tol,GeomAbs_C1,10,5);
        //  C1 = ApxC1.Curve();
        //}
        Handle(Geom_Curve) C3 = BRep_Tool::Curve(E3,fp,lp);
        if (C3->IsKind(STANDARD_TYPE(Geom_Line)) || C3->IsKind(STANDARD_TYPE(Geom_Conic))) {
          C3 = new Geom_TrimmedCurve(C3,fp,lp);
        }
        //filebuf fic;
        //ostream os(&fic);
        //os.precision(15);
        Handle(Geom_BSplineCurve) CE1 =
          GeomConvert::CurveToBSplineCurve(C1,Convert_RationalC1);
        if (CE1->Degree()<3)
          CE1->IncreaseDegree(3);
        Handle(Geom_BSplineCurve) CE2 =
          GeomConvert::CurveToBSplineCurve(C2,Convert_RationalC1);
        if (CE2->Degree()<3)
          CE2->IncreaseDegree(3);
        Handle(Geom_BSplineCurve) CE3 =
          GeomConvert::CurveToBSplineCurve(C3,Convert_RationalC1);
        if (CE3->Degree()<3)
          CE3->IncreaseDegree(3);
        Handle(Geom_BSplineCurve) CE4 =
          GeomConvert::CurveToBSplineCurve(C4,Convert_RationalC1);
        if (CE4->Degree()<3)
          CE4->IncreaseDegree(3);

        Handle(Geom_Surface) BS;
        try {
          GeomFill_BSplineCurves GF(CE1,CE2,CE3,CE4,GeomFill_CoonsStyle);
          //GeomFill_BSplineCurves GF(CE1,CE2,CE3,CE4,GeomFill_StretchStyle);
          BS = GF.Surface();
        }
        catch(...) {
          MESSAGE ("      can not create BSplineSurface - create Bezier");
          int NbP=26;
          TColgp_Array2OfPnt Points(1,NbP,1,NbP);
          double fp1,lp1,fp2,lp2;
          Handle(Geom_Curve) C1 = BRep_Tool::Curve(E1,fp1,lp1);
          Handle(Geom_Curve) C3 = BRep_Tool::Curve(E3,fp2,lp2);
          gp_Pnt P1C1,P2C1;
          C1->D0(fp1,P1C1);
          C1->D0(lp1,P2C1);
          gp_Pnt P1C3,P2C3;
          C3->D0(fp2,P1C3);
          C3->D0(lp2,P2C3);
          int n1,n2;
          double fp,lp;
          // get points from C1
          if (P1.Distance(P1C1)<1.e-6) {
            fp = fp1;
            lp = lp1;
          }
          else {
            fp = lp1;
            lp = fp1;
          }
          double step = (lp-fp)/(NbP-1);
          Points.SetValue(1,1,P1);
          double par = fp;
          for (n1=2; n1<NbP; n1++) {
            gp_Pnt P;
            par += step;
            C1->D0(par,P);
            Points.SetValue(1,n1,P);
          }
          Points.SetValue(1,NbP,P2);
          // get points from C3
          if (P4.Distance(P1C3)<1.e-6) {
            fp = fp2;
            lp = lp2;
          }
          else {
            fp = lp2;
            lp = fp2;
          }
          step = (lp-fp)/(NbP-1);
          Points.SetValue(NbP,1,P4);
          par = fp;
          for (n1=2; n1<NbP; n1++) {
            gp_Pnt P;
            par += step;
            C3->D0(par,P);
            Points.SetValue(NbP,n1,P);
          }
          Points.SetValue(NbP,NbP,P3);
          // create isolines and get points from them
          for (n1=1; n1<=NbP; n1++) {
            gp_Pnt PI1 = Points.Value(1,n1);
            gp_Pnt PI2 = Points.Value(NbP,n1);
            Handle(TColgp_HArray1OfPnt) HAP = new TColgp_HArray1OfPnt(1,2);
            HAP->SetValue(1,PI1);
            HAP->SetValue(2,PI2);
            GeomAPI_Interpolate anInt(HAP,Standard_False,1.e-7);
            anInt.Load(aDir1,aDir2);
            anInt.Perform();
            const Handle(Geom_BSplineCurve) iso = anInt.Curve();
            fp = iso->FirstParameter();
            lp = iso->LastParameter();
            step = (lp-fp)/(NbP-1);
            par = fp;
            TopoDS_Compound VComp;
            B.MakeCompound(VComp);
            for (n2=2; n2<NbP; n2++) {
              gp_Pnt P;
              par += step;
              iso->D0(par,P);
              Points.SetValue(n2,n1,P);
            }
          }
          // create surface and face
          //Handle(Geom_BezierSurface) BS = new Geom_BezierSurface(Points);
          BS = new Geom_BezierSurface(Points);
        }

        BRepBuilderAPI_MakeFace BB(BS,W);
        TopoDS_Face NewF = BB.Face();
        Handle(ShapeFix_Face) sff = new ShapeFix_Face(NewF);
        sff->Perform();
        sff->FixOrientation();
        TopoDS_Face FixedFace = sff->Face();
        aNewFs.Append(FixedFace);
        VPE.Add(E1,FixedFace);

        if (isGenerateGroups) {
          if (aBndEdges1.Contains(E1)) {
            // This is a boundary face.
            aLocalGroups[GROUP_OTHER].Append(FixedFace);
          }
        }
      }
      // make shell
      TopoDS_Shell aShell;
      B.MakeShell(aShell);
      for (int nf=1; nf<=aNewFs.Length(); nf++) {
        B.Add(aShell,aNewFs(nf));
      }
      B.Add(aShell,F1);
      B.Add(aShell,F2);

      // Create groups.
      if (isGenerateGroups && i == 1) {
        aLocalGroups[GROUP_DOWN].Append(F1);
      }

      if (isGenerateGroups && i == nbBases - 1) {
        aLocalGroups[GROUP_UP].Append(F2);
      }

      // make sewing for this shell
      Handle(BRepBuilderAPI_Sewing) aSewing = new BRepBuilderAPI_Sewing;
      aSewing->SetTolerance(Precision::Confusion());
      aSewing->SetFaceMode(Standard_True);
      aSewing->SetFloatingEdgesMode(Standard_False);
      aSewing->SetNonManifoldMode(Standard_False);
      for (anExp.Init(aShell, TopAbs_FACE); anExp.More(); anExp.Next()) {
        aSewing->Add(anExp.Current());
      }
      aSewing->Perform();
      MESSAGE ("    shell for face "<<nbff<<" is created");
      const TopoDS_Shape aSewShape = aSewing->SewedShape();
      if (aSewShape.ShapeType() == TopAbs_SHELL) {
        aShell = TopoDS::Shell(aSewShape);
        GProp_GProps aSystem;
        BRepGProp::VolumeProperties(aShell, aSystem);
        if (aSystem.Mass()<0) {
          //cout<<"aSewShape is reversed"<<endl;
          aShell.Reverse();
        }
        if (BRep_Tool::IsClosed(aShell)) {
          TopoDS_Solid aSolid;
          B.MakeSolid(aSolid);
          B.Add(aSolid,aShell);
          B.Add(aComp,aSolid);
          MESSAGE ("    solid for face "<<nbff<<" is created");
        }
        else {
          B.Add(aComp,aShell);
          MESSAGE ("    solid for face "<<nbff<<" is not created");
        }
      }
      else {
        B.Add(aComp,aShell);
        MESSAGE ("    solid for face "<<nbff<<" is not created");
      }

      if (isGenerateGroups) {
        Standard_Integer iGrp;

        for (iGrp = 0; iGrp < 5; ++iGrp) {
          Standard_Integer j;

          // For each sub-shape of pipe
          for (j = 1; j <= aLocalGroups[iGrp].Length(); ++j) {
            const TopoDS_Shape &aGrpShape = aLocalGroups[iGrp].Value(j);

            if (aSewing->IsModifiedSubShape(aGrpShape)) {
              // Use the shape modified by sewing.
              const TopoDS_Shape &aModifGrpShape =
                aSewing->ModifiedSubShape(aGrpShape);

              aGroups[iGrp].Append(aModifGrpShape);
            } else {
              // Use the shape as it is.
              aGroups[iGrp].Append(aGrpShape);
            }
          }
        }
      }
    }
  }

  if (isGenerateGroups) {
    // Fill the groups.
    Handle(TColStd_HArray1OfInteger) aGroupIds[5];
    TopTools_IndexedMapOfShape       anIndices;

    TopExp::MapShapes(aComp, anIndices);

    if (!FillGroups(aGroups, anIndices, aGroupIds)) {
      if (aCI) delete aCI;
      Standard_ConstructionError::Raise("Generate groups failure");
    }

    StoreGroups(aCI, aGroupIds);
  }

  return aComp;
}

//=======================================================================
//function : CreatePipeBiNormalAlongVector
//purpose  : auxilary for Execute()
//=======================================================================
static TopoDS_Shape CreatePipeBiNormalAlongVector(const TopoDS_Wire& aWirePath,
                                                  GEOMImpl_IPipe* aCI)
{
  GEOMImpl_IPipeBiNormal* aCIBN = (GEOMImpl_IPipeBiNormal*)aCI;

  Handle(GEOM_Function) aRefBase = aCIBN->GetBase();
  Handle(GEOM_Function) aRefVec = aCIBN->GetVector();
  TopoDS_Shape aShapeBase = aRefBase->GetValue();
  TopoDS_Shape aShapeVec = aRefVec->GetValue();

  if (aShapeBase.IsNull()) {
    if (aCIBN) delete aCIBN;
    Standard_NullObject::Raise("MakePipe aborted : null base argument");
  }

  // Make copy to prevent modifying of base object: 0021525
  BRepBuilderAPI_Copy Copy (aShapeBase);
  if (Copy.IsDone())
    aShapeBase = Copy.Shape();

  TopoDS_Shape aProf;
  if (aShapeBase.ShapeType() == TopAbs_VERTEX) {
    aProf = aShapeBase;
  }
  else if (aShapeBase.ShapeType() == TopAbs_EDGE) {
    aProf = BRepBuilderAPI_MakeWire(TopoDS::Edge(aShapeBase)).Shape();
  }
  else if (aShapeBase.ShapeType() == TopAbs_WIRE) {
    aProf = aShapeBase;
  }
  else if (aShapeBase.ShapeType() == TopAbs_FACE) {
    TopExp_Explorer wexp (aShapeBase,TopAbs_WIRE);
    aProf = wexp.Current();
  }
  else {
    Standard_TypeMismatch::Raise
      ("MakePipe aborted : invalid type of base");
  }
  BRepOffsetAPI_MakePipeShell PipeBuilder (aWirePath);
  PipeBuilder.Add(aProf);

  if (aShapeVec.IsNull()) {
    if (aCIBN) delete aCIBN;
    Standard_NullObject::Raise
      ("MakePipe aborted : null vector argument");
  }
  if (aShapeVec.ShapeType() != TopAbs_EDGE)
    Standard_TypeMismatch::Raise
      ("MakePipe aborted: invalid type of vector");
  TopoDS_Edge anEdge = TopoDS::Edge(aShapeVec);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull())
    Standard_NullObject::Raise
      ("MakePipe aborted: vector is not defined");
  gp_Vec aVec(BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
  gp_Dir BiNormal(aVec);
  PipeBuilder.SetMode(BiNormal);

  Standard_Boolean isDone = BuildPipeShell(PipeBuilder);

  if (isDone && aShapeBase.ShapeType() == TopAbs_FACE) {
      PipeBuilder.MakeSolid();
  }

  if (!CreateGroups(PipeBuilder, aCIBN)) {
    if (aCIBN) delete aCIBN;
    Standard_ConstructionError::Raise("Generate groups failure");
  }

  return PipeBuilder.Shape();
}

//=======================================================================
//function : FillGroups
//purpose  : auxilary for DoGroups()
//=======================================================================
bool FillGroups(const TopTools_SequenceOfShape         *theGroups,
                const TopTools_IndexedMapOfShape       &theIndices,
                      Handle(TColStd_HArray1OfInteger) *theGroupIds)
{
  Standard_Integer i;

  for (i = 0; i < 5; ++i) {
    if (!theGroups[i].IsEmpty()) {
      const Standard_Integer aNbShapes = theGroups[i].Length();
      Standard_Integer       j;

      theGroupIds[i] = new TColStd_HArray1OfInteger(1, aNbShapes);

      for (j = 1; j <= aNbShapes; ++j) {
        const TopoDS_Shape     &aShape  = theGroups[i].Value(j);
        const Standard_Integer  anIndex = theIndices.FindIndex(aShape);

        if (anIndex == 0) {
          return false;
        }

        theGroupIds[i]->SetValue(j, anIndex);
      }
    }
  }

  return true;
}

//=======================================================================
//function : StoreGroups
//purpose  : auxilary for CreateGroups()
//=======================================================================
void StoreGroups(GEOMImpl_IPipe                   *theCI,
                 Handle(TColStd_HArray1OfInteger) *theGroups)
{
  if (theGroups[GROUP_DOWN].IsNull() == Standard_False) {
    theCI->SetGroupDown(theGroups[GROUP_DOWN]);
  }

  if (theGroups[GROUP_UP].IsNull() == Standard_False) {
    theCI->SetGroupUp(theGroups[GROUP_UP]);
  }

  if (theGroups[GROUP_SIDE1].IsNull() == Standard_False) {
    theCI->SetGroupSide1(theGroups[GROUP_SIDE1]);
  }

  if (theGroups[GROUP_SIDE2].IsNull() == Standard_False) {
    theCI->SetGroupSide2(theGroups[GROUP_SIDE2]);
  }

  if (theGroups[GROUP_OTHER].IsNull() == Standard_False) {
    theCI->SetGroupOther(theGroups[GROUP_OTHER]);
  }
}

//=======================================================================
//function : CreateDownUpGroups
//purpose  : auxilary for DoGroups()
//=======================================================================
static bool CreateDownUpGroups(BRepPrimAPI_MakeSweep    *theSweep,
                               TopTools_SequenceOfShape *theGroups,
                               Standard_Boolean         &IsDoSides)
{
  const TopoDS_Shape     aDownShape   = theSweep->FirstShape();
  const TopAbs_ShapeEnum aType        = aDownShape.ShapeType();
  TopAbs_ShapeEnum       anUpDownType = TopAbs_SHAPE;

  IsDoSides  = Standard_False;

  switch (aType) {
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      anUpDownType = TopAbs_EDGE;

      if (GEOMUtils::IsOpenPath(aDownShape)) {
        IsDoSides = Standard_True;
      }
      break;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      anUpDownType = TopAbs_FACE;
      break;
    default:
      break;
  }

  if (anUpDownType == TopAbs_SHAPE) {
    // Invalid Up and Down group type.
    return false;
  }

  TopExp_Explorer     anExp(aDownShape, anUpDownType);
  TopTools_MapOfShape aMapFence;

  // Create Down group.
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aShape = anExp.Current();

    if (aMapFence.Add(aShape)) {
      theGroups[GROUP_DOWN].Append(aShape);
    }
  }

  // Create Up group
  const TopoDS_Shape anUpShape = theSweep->LastShape();

  aMapFence.Clear();
  anExp.Init(anUpShape, anUpDownType);

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aShape = anExp.Current();

    if (aMapFence.Add(aShape)) {
      theGroups[GROUP_UP].Append(aShape);
    }
  }

  return true;
}

//=======================================================================
//function : DoGroups
//purpose  : auxilary for CreateGroups()
//=======================================================================
bool DoGroups(BRepOffsetAPI_MakePipeShell &theSweep,
              TopTools_SequenceOfShape    *theGroups)
{
  Standard_Boolean isDoSides = Standard_False;

  if (!CreateDownUpGroups(&theSweep, theGroups, isDoSides)) {
    // Up and Down groups creation failure
    return false;
  }

  const TopoDS_Shape aDownShape = theSweep.FirstShape();

  if (isDoSides) {
    // Create Side1 and Side2 groups.
    const TopAbs_ShapeEnum aType = aDownShape.ShapeType();
    TopoDS_Vertex          aV[2];
    Standard_Integer       i;

    if (aType == TopAbs_EDGE) {
      TopExp::Vertices(TopoDS::Edge(aDownShape), aV[0], aV[1], Standard_True);
    } else { // aType == TopAbs_WIRE
      TopExp::Vertices(TopoDS::Wire(aDownShape), aV[0], aV[1]);
    }

    for (i = 0; i < 2; ++i) {
      if (aV[i].IsNull() == Standard_False) {
        const TopTools_ListOfShape &aLstSide = theSweep.Generated(aV[i]);

        if (!aLstSide.IsEmpty()) {
          TopTools_ListIteratorOfListOfShape aSideIt(aLstSide);
          TopTools_MapOfShape                aMapFence;
          const Standard_Integer             anIdSide =
                      (i == 0 ? GROUP_SIDE1 : GROUP_SIDE2);

          for (; aSideIt.More(); aSideIt.Next()) {
            const TopoDS_Shape &aSideShape = aSideIt.Value();

            if (aSideShape.ShapeType() == TopAbs_EDGE) {
              if (aMapFence.Add(aSideShape)) {
                theGroups[anIdSide].Append(aSideShape);
              }
            } else {
              // Only edges can be is Side1 and Side2 groups.
              return false;
            }
          }
        }
      }
    }
  } else {
    // Create Other group. Get boudnary edges of the profile.
    TopTools_MapOfShape aMapBndEdges;
    TopExp_Explorer     anExp(aDownShape, TopAbs_EDGE);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &anEdge = anExp.Current();

      if (!aMapBndEdges.Add(anEdge)) {
        aMapBndEdges.Remove(anEdge);
      }
    }

    // Fill the map of faces generated from profile's boundary edges.
    TopTools_MapIteratorOfMapOfShape anIter(aMapBndEdges);
    TopTools_MapOfShape              aMapFence;

    for (; anIter.More(); anIter.Next()) {
      const TopTools_ListOfShape &aLstOther = theSweep.Generated(anIter.Key());

      if (!aLstOther.IsEmpty()) {
        TopTools_ListIteratorOfListOfShape anOtherIt(aLstOther);

        for (; anOtherIt.More(); anOtherIt.Next()) {
          const TopoDS_Shape &anOtherShape = anOtherIt.Value();

          if (anOtherShape.ShapeType() == TopAbs_FACE) {
            if (aMapFence.Add(anOtherShape)) {
              theGroups[GROUP_OTHER].Append(anOtherShape);
            }
          } else {
            // Only faces can be in Other group.
            return false;
          }
        }
      }
    }
  }

  return true;
}

//=======================================================================
//function : CreateGroups
//purpose  : auxilary for Execute()
//=======================================================================
bool CreateGroups(BRepOffsetAPI_MakePipeShell &theSweep,
                  GEOMImpl_IPipe              *theCI)
{
  if (!theCI->GetGenerateGroups()) {
    // Nothing to do.
    return true;
  }

  // Make groups.
  TopTools_SequenceOfShape aGroups[5];

  if (!DoGroups(theSweep, aGroups)) {
    return false;
  }

  // Fill the groups.
  Handle(TColStd_HArray1OfInteger) aGroupIds[5];
  TopTools_IndexedMapOfShape       anIndices;
  const TopoDS_Shape               aResult = theSweep.Shape();

  TopExp::MapShapes(aResult, anIndices);

  if (!FillGroups(aGroups, anIndices, aGroupIds)) {
    return false;
  }

  // Store groups.
  StoreGroups(theCI, aGroupIds);

  return true;
}

//=======================================================================
//function : DoGroups
//purpose  : auxilary for CreateGroups()
//=======================================================================
static bool DoGroups(const TopoDS_Shape             &theProfile,
                     const TopoDS_Shape             &thePath,
                           BRepOffsetAPI_MakePipe   &theSweep,
                           TopTools_SequenceOfShape *theGroups)
{
  Standard_Boolean isDoSides = Standard_False;

  if (!CreateDownUpGroups(&theSweep, theGroups, isDoSides)) {
    // Up and Down groups creation failure
    return false;
  }

  if (isDoSides) {
    // Create Side1 and Side2 groups.
    const TopAbs_ShapeEnum aType = theProfile.ShapeType();
    TopoDS_Vertex          aV[2];
    Standard_Integer       i;

    if (aType == TopAbs_EDGE) {
      TopExp::Vertices(TopoDS::Edge(theProfile), aV[0], aV[1], Standard_True);
    } else { // aType == TopAbs_WIRE
      TopExp::Vertices(TopoDS::Wire(theProfile), aV[0], aV[1]);
    }

    for (i = 0; i < 2; ++i) {
      if (aV[i].IsNull() == Standard_False) {
        TopExp_Explorer        anExpP(thePath, TopAbs_EDGE);
        TopTools_MapOfShape    aMapFence;
        const Standard_Integer anIdSide =
                      (i == 0 ? GROUP_SIDE1 : GROUP_SIDE2);

        for (; anExpP.More(); anExpP.Next()) {
          const TopoDS_Shape aSideShape =
            theSweep.Generated(anExpP.Current(), aV[i]);

          if (aSideShape.ShapeType() == TopAbs_EDGE) {
            if (aMapFence.Add(aSideShape)) {
              theGroups[anIdSide].Append(aSideShape);
            }
          } else {
            // Only edges can be is Side1 and Side2 groups.
            return false;
          }
        }
      }
    }
  } else {
    // Create Other group. Get boudnary edges of the profile.
    TopTools_MapOfShape aMapBndEdges;
    TopExp_Explorer     anExp(theProfile, TopAbs_EDGE);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &anEdge = anExp.Current();

      if (!aMapBndEdges.Add(anEdge)) {
        aMapBndEdges.Remove(anEdge);
      }
    }

    TopExp_Explorer        anExpP(thePath, TopAbs_EDGE);
    TopTools_MapOfShape    aMapFence;

    for (; anExpP.More(); anExpP.Next()) {
      TopTools_MapIteratorOfMapOfShape anIter(aMapBndEdges);

      for (; anIter.More(); anIter.Next()) {
        const TopoDS_Shape anOtherShape =
          theSweep.Generated(anExpP.Current(), anIter.Key());

        if (anOtherShape.ShapeType() == TopAbs_FACE) {
          if (aMapFence.Add(anOtherShape)) {
            theGroups[GROUP_OTHER].Append(anOtherShape);
          }
        } else {
          // Only faces can be in Other group.
          return false;
        }
      }
    }
  }

  return true;
}

//=======================================================================
//function : CreateGroups
//purpose  : auxilary for Execute()
//=======================================================================
static bool CreateGroups(const TopoDS_Shape     &theProfile,
                         const TopoDS_Shape     &thePath,
                         BRepOffsetAPI_MakePipe &theSweep,
                         GEOMImpl_IPipe         *theCI)
{
  if (!theCI->GetGenerateGroups()) {
    // Nothing to do.
    return true;
  }

  // Make groups.
  TopTools_SequenceOfShape aGroups[5];

  if (!DoGroups(theProfile, thePath, theSweep, aGroups)) {
    return false;
  }

  // Fill the groups.
  Handle(TColStd_HArray1OfInteger) aGroupIds[5];
  TopTools_IndexedMapOfShape       anIndices;
  const TopoDS_Shape               aResult = theSweep.Shape();

  TopExp::MapShapes(aResult, anIndices);

  if (!FillGroups(aGroups, anIndices, aGroupIds)) {
    return false;
  }

  // Store groups.
  StoreGroups(theCI, aGroupIds);

  return true;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PipeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  Standard_Integer aType = aFunction->GetType();

  GEOMImpl_IPipe* aCI = 0;
  if (aType == PIPE_BASE_PATH)
    aCI = new GEOMImpl_IPipe (aFunction);
  else if (aType == PIPE_DIFFERENT_SECTIONS)
    aCI = new GEOMImpl_IPipeDiffSect (aFunction);
  else if (aType == PIPE_SHELL_SECTIONS)
    aCI = new GEOMImpl_IPipeShellSect (aFunction);
  else if (aType == PIPE_SHELLS_WITHOUT_PATH)
    aCI = new GEOMImpl_IPipeShellSect (aFunction);
  else if (aType == PIPE_BI_NORMAL_ALONG_VECTOR)
    aCI = new GEOMImpl_IPipeBiNormal (aFunction);
  else
    return 0;

  TopoDS_Wire aWirePath;
  if (aType != PIPE_SHELLS_WITHOUT_PATH) {
    // working with path
    Handle(GEOM_Function) aRefPath = aCI->GetPath();
    TopoDS_Shape aShapePath = aRefPath->GetValue();

    if (aShapePath.IsNull()) {
      MESSAGE ("Driver : path is null");
      if (aCI) delete aCI;
      Standard_NullObject::Raise("MakePipe aborted : null path argument");
    }

    // Get path contour
    bool isOk = false;
    if (aShapePath.ShapeType() == TopAbs_COMPOUND) {
      TopTools_SequenceOfShape anEdges;
      TopExp_Explorer anExp;
      BRep_Builder B;
      TopoDS_Wire W;
      B.MakeWire(W);
      for (anExp.Init(aShapePath, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        B.Add(W, anExp.Current());
        isOk = true;
      }
      if (isOk)
        aWirePath = W;
    }
    else if (aShapePath.ShapeType() == TopAbs_WIRE) {
      aWirePath = TopoDS::Wire(aShapePath);
      isOk = true;
    }
    else {
      if (aShapePath.ShapeType() == TopAbs_EDGE) {
        TopoDS_Edge anEdge = TopoDS::Edge(aShapePath);
        aWirePath = BRepBuilderAPI_MakeWire(anEdge);
        isOk = true;
      }
    }
    if (!isOk) {
      if (aCI) delete aCI;
      Standard_TypeMismatch::Raise("MakePipe aborted : path shape is neither a wire nor an edge");
    }

    // Check if it is possible to create groups.
    if (aCI->GetGenerateGroups() && !GEOMUtils::IsOpenPath(aWirePath)) {
      if (aCI) {
        delete aCI;
      }

      Standard_ConstructionError::Raise
        ("Can't create groups if the path is closed");
    }
  }

  TopoDS_Shape           aShape;
  const Standard_Boolean isGenerateGroups = aCI->GetGenerateGroups();

  if (aType == PIPE_BASE_PATH) {
    Handle(GEOM_Function) aRefBase = aCI->GetBase();
    TopoDS_Shape aShapeBase;

    // Make copy to prevent modifying of base object 0020766 : EDF 1320
    BRepBuilderAPI_Copy Copy(aRefBase->GetValue());
    if (Copy.IsDone())
      aShapeBase = Copy.Shape();

    if (aShapeBase.IsNull()) {
      if (aCI) delete aCI;
      Standard_NullObject::Raise("MakePipe aborted : null base argument");
    }

    // Make pipe
    if (aShapeBase.ShapeType() == TopAbs_EDGE ||
        aShapeBase.ShapeType() == TopAbs_WIRE)
    {
      TopoDS_Wire Profile;
      if (aShapeBase.ShapeType() == TopAbs_WIRE)
        Profile = TopoDS::Wire(aShapeBase);
      else
      {
        BRep_Builder BB;
        BB.MakeWire(Profile);
        BB.Add(Profile, aShapeBase);
      }

      BRepOffsetAPI_MakePipeShell Sweep (aWirePath);
      BRepBuilderAPI_MakeFace FaceBuilder (aWirePath, Standard_True); //to find the plane of spine
      if (FaceBuilder.IsDone())
        Sweep.SetMode(FaceBuilder.Face());
      Sweep.Add(Profile);

      Standard_Boolean isDone = BuildPipeShell(Sweep);

      if (!isDone)
      {
        if (aCI) delete aCI;
        Standard_ConstructionError::Raise("MakePipeShell failed");
      }
      else
        aShape = Sweep.Shape(); //result is good

      if (!CreateGroups(Sweep, aCI)) {
        if (aCI) delete aCI;
        Standard_ConstructionError::Raise("Generate groups failure");
      }
    }
    else
    {
      GeomFill_Trihedron theBestMode = EvaluateBestSweepMode(aWirePath);
      BRepOffsetAPI_MakePipe aMkPipe
        (aWirePath, aShapeBase, theBestMode, Standard_True);

      if (aMkPipe.IsDone() && aMkPipe.ErrorOnSurface() <= TolPipeSurf) {
        aShape = aMkPipe.Shape();

        if (!CreateGroups(aShapeBase, aWirePath, aMkPipe, aCI)) {
          if (aCI) delete aCI;
          Standard_ConstructionError::Raise("Generate groups failure");
        }
      } else if (theBestMode != GeomFill_IsDiscreteTrihedron) {
        // Try to use Descrete Trihedron mode.
        BRepOffsetAPI_MakePipe aMkPipeDescrete
          (aWirePath, aShapeBase, GeomFill_IsDiscreteTrihedron, Standard_True);

        if (aMkPipeDescrete.IsDone()) {
          aShape = aMkPipeDescrete.Shape();

          if (!CreateGroups(aShapeBase, aWirePath, aMkPipeDescrete, aCI)) {
            if (aCI) delete aCI;
            Standard_ConstructionError::Raise("Generate groups failure");
          }
        }
      }
    }
  }

  //building pipe with different sections
  else if (aType == PIPE_DIFFERENT_SECTIONS) {
    GEOMImpl_IPipeDiffSect* aCIDS = (GEOMImpl_IPipeDiffSect*)aCI;
    Handle(TColStd_HSequenceOfTransient) aBasesObjs = aCIDS->GetBases ();
    Handle(TColStd_HSequenceOfTransient) aLocObjs = aCIDS->GetLocations ();
    Standard_Boolean aWithContact = (aCIDS->GetWithContactMode());
    Standard_Boolean aWithCorrect = (aCIDS->GetWithCorrectionMode());
    Standard_Boolean isBySteps = aCIDS->GetIsBySteps();

    if (aCI) {
      delete aCI;
      aCI = 0;
    }

    Standard_Integer nbBases = aBasesObjs->Length();
    Standard_Integer nbLocs  = (aLocObjs.IsNull() ? 0 : aLocObjs->Length());

    Handle(TopTools_HSequenceOfShape) aHSeqBases = new TopTools_HSequenceOfShape;
    Handle(TopTools_HSequenceOfShape) aHSeqLocs  = new TopTools_HSequenceOfShape;
    Standard_Integer i;

    for (i = 1; i <= nbBases; i++) {
      Handle(Standard_Transient) anItem = aBasesObjs->Value(i);
      if (anItem.IsNull())
        continue;
      Handle(GEOM_Function) aRefBase = Handle(GEOM_Function)::DownCast(anItem);
      if (aRefBase.IsNull())
        continue;
      if (aRefBase->GetValue().IsNull())
        continue;

      aHSeqBases->Append(aRefBase->GetValue());
    }
    for (i = 1; i <= nbLocs; i++) {
      Handle(Standard_Transient) anItemLoc = aLocObjs->Value(i);
      if (anItemLoc.IsNull())
        continue;
      Handle(GEOM_Function) aRefLoc = Handle(GEOM_Function)::DownCast(anItemLoc);
      TopoDS_Shape aShapeLoc = aRefLoc->GetValue();
      if (aShapeLoc.IsNull() || aShapeLoc.ShapeType() != TopAbs_VERTEX)
        continue;

      aHSeqLocs->Append(aShapeLoc);
    }


    Handle(TColStd_HArray1OfInteger) *pGroups = NULL;
    Handle(TColStd_HArray1OfInteger)  aGroups[5];

    if (isGenerateGroups) {
      pGroups = aGroups;
    }

    aShape = CreatePipeWithDifferentSections
              (aWirePath, aHSeqBases, aHSeqLocs,
               aWithContact, aWithCorrect, isBySteps, pGroups);

    if (isGenerateGroups) {
      // Store created groups.
      GEOMImpl_IPipeDiffSect aPipeDS(aFunction);

      StoreGroups(&aPipeDS, aGroups);
    }
  }

  //building pipe with shell sections
  else if (aType == PIPE_SHELL_SECTIONS) {
    aShape = CreatePipeForShellSections(aWirePath,aCI);
  }

  //building pipe shell sections without path
  else if (aType == PIPE_SHELLS_WITHOUT_PATH) {
    aShape = CreatePipeShellsWithoutPath(aCI);
  }

  //building a pipe with constant bi-normal along given vector
  else if (aType == PIPE_BI_NORMAL_ALONG_VECTOR) {
    aShape = CreatePipeBiNormalAlongVector(aWirePath, aCI);
  }

  if (aCI) {
    delete aCI;
    aCI = 0;
  }

  if (aShape.IsNull()) return 0;

  if ( !GEOMUtils::CheckShape(aShape) && !GEOMUtils::FixShapeTolerance(aShape) ) 
    Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");

  if (aType != PIPE_BASE_PATH &&
      aType != PIPE_SHELLS_WITHOUT_PATH) {
    TopExp_Explorer anExpV (aShape, TopAbs_VERTEX);
    if (anExpV.More()) {
      Standard_Real aVertMaxTol = -RealLast();
      for (; anExpV.More(); anExpV.Next()) {
        TopoDS_Vertex aVertex = TopoDS::Vertex(anExpV.Current());
        Standard_Real aTol = BRep_Tool::Tolerance(aVertex);
        if (aTol > aVertMaxTol)
          aVertMaxTol = aTol;
      }
      aVertMaxTol += Precision::Confusion();

      TopTools_DataMapOfShapeListOfShape  aMapModif;
      TopTools_DataMapOfShapeListOfShape *pMapModif = NULL;

      if (isGenerateGroups) {
        pMapModif = &aMapModif;
      }

      TopoDS_Shape aNewShape = GEOMImpl_GlueDriver::GlueFaces
        (aShape, aVertMaxTol, Standard_True, pMapModif);

      if (isGenerateGroups && !aMapModif.IsEmpty()) {
        // Update groups.
        GEOMImpl_IPipe                   aCI(aFunction);
        Handle(TColStd_HArray1OfInteger) aGroupIDs[5] =
          { aCI.GetGroupDown(), aCI.GetGroupUp(), aCI.GetGroupSide1(),
            aCI.GetGroupSide2(), aCI.GetGroupOther() };
        TopTools_IndexedMapOfShape       anIndices;
        TopTools_IndexedMapOfShape       aNewIndices;
        TopTools_SequenceOfShape         aNewShapes[5];
        TopTools_MapOfShape              aMapReplaced;
        TopTools_MapOfShape              aMapGlued;
        Standard_Integer                 iGrp;
        Standard_Integer                 i;

        TopExp::MapShapes(aShape,    anIndices);
        TopExp::MapShapes(aNewShape, aNewIndices);

        for (iGrp = 0; iGrp < 5; ++iGrp) {
          if (aGroupIDs[iGrp].IsNull() == Standard_False) {
            const Standard_Integer aLower  = aGroupIDs[iGrp]->Lower();
            const Standard_Integer anUpper = aGroupIDs[iGrp]->Upper();

            for (i = aLower; i <= anUpper; ++i) {
              const Standard_Integer  anIndex   = aGroupIDs[iGrp]->Value(i);
              const TopoDS_Shape     &aSubShape = anIndices.FindKey(anIndex);

              if (aMapModif.IsBound(aSubShape)) {
                const TopTools_ListOfShape         &aListModif =
                  aMapModif.Find(aSubShape);
                TopTools_ListIteratorOfListOfShape  anIter(aListModif);

                for (; anIter.More(); anIter.Next()) {
                  const TopoDS_Shape &aNewShape = anIter.Value();

                  if (aMapReplaced.Add(aNewShape)) {
                    aNewShapes[iGrp].Append(aNewShape);
                  } else {
                    // This is a glued shape. It means that it is internal
                    // one and should be removed from groups later.
                    aMapGlued.Add(aNewShape);
                  }
                }
              } else {
                // Shape is not modified.
                aNewShapes[iGrp].Append(aSubShape);
              }
            }
          }
        }

        if (!aMapGlued.IsEmpty()) {
          // Remove glued (internal) shapes from groups.
          for (iGrp = 0; iGrp < 5; ++iGrp) {
            Standard_Integer aNbShapes = aNewShapes[iGrp].Length();

            for (i = 1; i < aNbShapes; ++i) {
              const TopoDS_Shape &aNewShape = aNewShapes[iGrp].Value(i);

              if (aMapGlued.Contains(aNewShape)) {
                aNewShapes[iGrp].Remove(i);
                --i;
                --aNbShapes;
              }
            }
          }
        }

        // Store modified groups.
        Handle(TColStd_HArray1OfInteger) aNewGroupIDs[5];

        if (!FillGroups(aNewShapes, aNewIndices, aNewGroupIDs)) {
          Standard_ConstructionError::Raise("Generate groups failure");
        }

        StoreGroups(&aCI, aNewGroupIDs);
      }

      aShape = aNewShape;
    }
  }

  // Note: group indices should not be changed after the next call.
  TopoDS_Shape aRes = GEOMUtils::CompsolidToCompound(aShape);
  aFunction->SetValue(aRes);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif
  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_PipeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case PIPE_BASE_PATH:
  {
    theOperationName = "PIPE";
    GEOMImpl_IPipe aCI( function );
    AddParam( theParams, "Base Object", aCI.GetBase() );
    AddParam( theParams, "Path Object", aCI.GetPath() );
    break;
  }
  case PIPE_BI_NORMAL_ALONG_VECTOR:
  {
    theOperationName = "PIPE";
    GEOMImpl_IPipeBiNormal aCI( function );
    AddParam( theParams, "Base Object", aCI.GetBase() );
    AddParam( theParams, "Path Object", aCI.GetPath() );
    AddParam( theParams, "BiNormal", aCI.GetVector() );
    break;
  }
  case PIPE_DIFFERENT_SECTIONS:
  {
    theOperationName = "PIPE";
    GEOMImpl_IPipeDiffSect aCI( function );
    AddParam( theParams, "Bases", aCI.GetBases() );
    AddParam( theParams, "Locations", aCI.GetLocations() );
    AddParam( theParams, "Path", aCI.GetPath() );

    if (!aCI.GetIsBySteps()) {
      AddParam( theParams, "With contact", aCI.GetWithContactMode() );
      AddParam( theParams, "With correction", aCI.GetWithCorrectionMode() );
    }

    AddParam( theParams, "Step by step", aCI.GetIsBySteps() );
    break;
  }
  case PIPE_SHELL_SECTIONS:
  {
    theOperationName = "PIPE";
    GEOMImpl_IPipeShellSect aCI( function );
    AddParam( theParams, "Bases", aCI.GetBases() );
    AddParam( theParams, "Sub-Bases", aCI.GetSubBases() );
    AddParam( theParams, "Locations", aCI.GetLocations() );
    AddParam( theParams, "Path", aCI.GetPath() );
    AddParam( theParams, "With contact", aCI.GetWithContactMode() );
    AddParam( theParams, "With correction", aCI.GetWithCorrectionMode() );
    break;
  }
  case PIPE_SHELLS_WITHOUT_PATH:
  {
    theOperationName = "PIPE"; // MakePipeShellsWithoutPath
    GEOMImpl_IPipeShellSect aCI( function );
    AddParam( theParams, "Bases", aCI.GetBases() );
    AddParam( theParams, "Locations", aCI.GetLocations() );
    break;
  }
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PipeDriver,GEOM_BaseDriver);
