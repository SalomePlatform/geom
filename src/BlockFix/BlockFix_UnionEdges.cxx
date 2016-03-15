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

// File:      BlockFix_UnionEdges.cxx
// Created:   07.12.04 15:27:30
// Author:    Sergey KUUL

#include <BlockFix_UnionEdges.hxx>

#include <ShapeAnalysis_Edge.hxx>

#include <ShapeBuild_ReShape.hxx>

#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>

#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>

#include <GC_MakeCircle.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>

#include <Geom2dConvert.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#include <TColGeom_SequenceOfSurface.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_Array1OfBSplineCurve.hxx>
#include <TColGeom2d_HArray1OfBSplineCurve.hxx>
#include <TColGeom2d_SequenceOfBoundedCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include "utilities.h"

//=======================================================================
//function : IsToMerge
//purpose  : This method return Standard_True if two edges have common
//           vertex. This vertex is returned by output parameter. The
//           difference with the method TopExp::CommonVertex is only in
//           the case if there are two common vertices. In this case
//           this method returns the last vertex of theEdge1, not the first
//           one that TopExp::CommonVertex does.
//=======================================================================
static Standard_Boolean GetCommonVertex(const TopoDS_Edge   &theEdge1,
                                        const TopoDS_Edge   &theEdge2,
                                              TopoDS_Vertex &theCommon)
{
  Standard_Boolean   isFound = Standard_True;
  ShapeAnalysis_Edge aSae;
  TopoDS_Vertex      aVF1 = aSae.FirstVertex(theEdge1);
  TopoDS_Vertex      aVL1 = aSae.LastVertex(theEdge1);
  TopoDS_Vertex      aVF2 = aSae.FirstVertex(theEdge2);
  TopoDS_Vertex      aVL2 = aSae.LastVertex(theEdge2);

  if (aVL1.IsSame(aVF2) || aVL1.IsSame(aVL2)) {
    theCommon = aVL1;
  } else if (aVF1.IsSame(aVL2) || aVF1.IsSame(aVF2)) {
    theCommon = aVF1;
  } else {
    theCommon.Nullify();
    isFound = Standard_False;
  }

  return isFound;
}

//=======================================================================
//function : IsToMerge
//purpose  : This method return Standard_True if two consequent edges can
//           be merged. The edges can be merged if:
//             1. They belong to same faces.
//             2. They either both seam or both not seam on each face.
//             3. There are no another edges (e.g. seam) on each common face
//                that are connected to the common vertex of two edges.
//             4. They are based on coincident lines, or:
//             5. They are based on coincident circles, or:
//             6. They are based on either Bezier of BSplines.
//=======================================================================
static Standard_Boolean IsToMerge
    (const TopoDS_Edge                               &theEdge1,
     const TopoDS_Edge                               &theEdge2,
     const TopTools_IndexedDataMapOfShapeListOfShape &theMapEdgeFaces,
     const Standard_Real                              theTolerance)
{
  Standard_Boolean aResult  = Standard_False;
  Standard_Boolean isDegen1 = BRep_Tool::Degenerated(theEdge1);
  Standard_Boolean isDegen2 = BRep_Tool::Degenerated(theEdge2);

  if (isDegen1 && isDegen2) {
    // Both of edges are degenerated.
    aResult = Standard_True;
  } else if (!isDegen1 && !isDegen2) {
    // Both of edges are not degenerated.
    // Check if they belong to the same faces.
    Standard_Boolean isSame = Standard_False;
    Standard_Boolean has1   = theMapEdgeFaces.Contains(theEdge1);
    Standard_Boolean has2   = theMapEdgeFaces.Contains(theEdge1);

    if (has1 && has2) {
      const TopTools_ListOfShape &aLst1 = theMapEdgeFaces.FindFromKey(theEdge1);
      const TopTools_ListOfShape &aLst2 = theMapEdgeFaces.FindFromKey(theEdge2);

      if (aLst1.Extent() == aLst2.Extent()) {
        TopTools_ListIteratorOfListOfShape anIter1(aLst1);

        isSame = Standard_True;

        for (; anIter1.More() && isSame; anIter1.Next()) {
          TopoDS_Face aFace1 = TopoDS::Face(anIter1.Value());
          TopTools_ListIteratorOfListOfShape  anIter2(aLst2);

          for (; anIter2.More(); anIter2.Next()) {
            if (aFace1.IsSame(anIter2.Value())) {
              // Same face is detected. Break the loop.
              // Check if edges either both seam or both not seam on this face.
              Standard_Boolean isSeam1 = BRep_Tool::IsClosed(theEdge1, aFace1);
              Standard_Boolean isSeam2 = BRep_Tool::IsClosed(theEdge2, aFace1);

              isSame = (isSeam1 && isSeam2) || (isSeam1 == isSeam2);

              if (isSame) {
                // Check if there are no other edges (e.g. seam) on this face
                // that are connected to the common vertex.
                TopoDS_Vertex aVCommon;

                if (GetCommonVertex(theEdge1, theEdge2, aVCommon)) {
                  TopTools_IndexedDataMapOfShapeListOfShape aMapVE;

                  TopExp::MapShapesAndAncestors
                    (aFace1, TopAbs_VERTEX, TopAbs_EDGE, aMapVE);

                  if (aMapVE.Contains(aVCommon)) {
                    TopTools_ListIteratorOfListOfShape 
                      anItE(aMapVE.FindFromKey(aVCommon));

                    for (; anItE.More(); anItE.Next()) {
                      const TopoDS_Shape &anEdge = anItE.Value();

                      if (!theEdge1.IsSame(anEdge) &&
                          !theEdge2.IsSame(anEdge)) {
                        // There is another edge that shares the common vertex.
                        // Nothing to merge.
                        isSame = Standard_False;
                        break;
                      }
                    }
                  } else {
                    // Common vertex doesn't belong to the face.
                    // Nothing to merge. NEVERREACHED.
                    isSame = Standard_False;
                  }
                } else {
                  // No common vertex. Nothing to merge. NEVERREACHED.
                  isSame = Standard_False;
                }
              }

              break;
            }
          }

          if (isSame && !anIter2.More()) {
            // No same face is detected. Break the loop.
            isSame = Standard_False;
            break;
          }
        }
      }
    } else {
      isSame = (has1 == has2); // True if the both of has are negative.
    }

    if (isSame) {
      // Check edges geometry.
      Standard_Real aFP1;
      Standard_Real aFP2;
      Standard_Real aLP1;
      Standard_Real aLP2;
      Handle(Geom_Curve) aC3d1 = BRep_Tool::Curve(theEdge1, aFP1, aLP1);
      Handle(Geom_Curve) aC3d2 = BRep_Tool::Curve(theEdge2, aFP2, aLP2);

      if (aC3d1.IsNull() == Standard_False &&
          aC3d2.IsNull() == Standard_False) {
        // Get the basis curves.
        while(aC3d1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
          Handle(Geom_TrimmedCurve) aTc =
            Handle(Geom_TrimmedCurve)::DownCast(aC3d1);
          aC3d1 = aTc->BasisCurve();
        }

        while(aC3d2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
          Handle(Geom_TrimmedCurve) aTc =
            Handle(Geom_TrimmedCurve)::DownCast(aC3d2);
          aC3d2 = aTc->BasisCurve();
        }

        if(aC3d1->IsKind(STANDARD_TYPE(Geom_Line)) &&
           aC3d2->IsKind(STANDARD_TYPE(Geom_Line))) {
          // Two curves are lines.
          Handle(Geom_Line) aL1 = Handle(Geom_Line)::DownCast(aC3d1);
          Handle(Geom_Line) aL2 = Handle(Geom_Line)::DownCast(aC3d2);
          gp_Dir aDir1 = aL1->Position().Direction();
          gp_Dir aDir2 = aL2->Position().Direction();
    
          if(aDir1.IsParallel(aDir2, theTolerance)) {
            // Two coincident lines.
            aResult = Standard_True;
          }
        } else if(aC3d1->IsKind(STANDARD_TYPE(Geom_Circle)) &&
                  aC3d2->IsKind(STANDARD_TYPE(Geom_Circle))) {
          // Two curves are circles.
          Handle(Geom_Circle) aC1 = Handle(Geom_Circle)::DownCast(aC3d1);
          Handle(Geom_Circle) aC2 = Handle(Geom_Circle)::DownCast(aC3d2);
          gp_Pnt aP01 = aC1->Location();
          gp_Pnt aP02 = aC2->Location();

          if (aP01.Distance(aP02) <= Precision::Confusion()) {
            // Two coincident circles.
            aResult = Standard_True;
          }
        } else if (aC3d1->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) ||
                   aC3d1->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
          if (aC3d2->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) ||
              aC3d2->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
            // Both of the curves are either bezier or BSplines.
            aResult = Standard_True;
          }
        }
      }
    }
  }

  return aResult;
}

//=======================================================================
//function : BlockFix_UnionEdges()
//purpose  : Constructor
//=======================================================================
BlockFix_UnionEdges::BlockFix_UnionEdges (  )
{
}

//=======================================================================
//function : GlueEdgesWithPCurves
//purpose  : Glues the pcurves of the sequence of edges
//           and glues their 3d curves
//=======================================================================
static TopoDS_Edge GlueEdgesWithPCurves(const TopTools_SequenceOfShape& aChain,
                                        const TopoDS_Vertex& FirstVertex,
                                        const TopoDS_Vertex& LastVertex)
{
  Standard_Integer i, j;

  TopoDS_Edge FirstEdge = TopoDS::Edge(aChain(1));
  //TColGeom2d_SequenceOfCurve PCurveSeq;
  TColGeom_SequenceOfSurface SurfSeq;
  //TopTools_SequenceOfShape LocSeq;
  
  BRep_ListIteratorOfListOfCurveRepresentation itr( (Handle(BRep_TEdge)::DownCast(FirstEdge.TShape()))->Curves() );
  for (; itr.More(); itr.Next())
  {
    Handle(BRep_CurveRepresentation) CurveRep = itr.Value();
    if (CurveRep->IsCurveOnSurface())
    {
      //PCurveSeq.Append(CurveRep->PCurve());
      SurfSeq.Append(CurveRep->Surface());
      /*
      TopoDS_Shape aLocShape;
      aLocShape.Location(CurveRep->Location());
      LocSeq.Append(aLocShape);
      */
    }
  }

  Standard_Real fpar, lpar;
  BRep_Tool::Range(FirstEdge, fpar, lpar);
  TopoDS_Edge PrevEdge = FirstEdge;
  TopoDS_Vertex CV;
  Standard_Real MaxTol = 0.;
  
  TopoDS_Edge ResEdge;
  BRep_Builder BB;

  Standard_Integer nb_curve = aChain.Length();   //number of curves
  TColGeom_Array1OfBSplineCurve tab_c3d(0,nb_curve-1);                    //array of the curves
  TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances
    
  TopoDS_Vertex PrevVertex = FirstVertex;
  for (i = 1; i <= nb_curve; i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aChain(i));
    TopoDS_Vertex VF, VL;
    TopExp::Vertices(anEdge, VF, VL);
    Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));
    
    Standard_Real Tol1 = BRep_Tool::Tolerance(VF);
    Standard_Real Tol2 = BRep_Tool::Tolerance(VL);
    if (Tol1 > MaxTol)
      MaxTol = Tol1;
    if (Tol2 > MaxTol)
      MaxTol = Tol2;
    
    if (i > 1)
    {
      GetCommonVertex(PrevEdge, anEdge, CV);
      Standard_Real Tol = BRep_Tool::Tolerance(CV);
      tabtolvertex(i-2) = Tol;
    }
    
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, fpar, lpar);
    Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(aCurve, fpar, lpar);
    tab_c3d(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
    GeomConvert::C0BSplineToC1BSplineCurve(tab_c3d(i-1), Precision::Confusion());
    if (ToReverse)
      tab_c3d(i-1)->Reverse();
    PrevVertex = (ToReverse)? VF : VL;
    PrevEdge = anEdge;
  }
  Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
  Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
  GeomConvert::ConcatC1(tab_c3d,
                        tabtolvertex,
                        ArrayOfIndices,
                        concatcurve,
                        Standard_False,
                        Precision::Confusion());   //C1 concatenation
  
  if (concatcurve->Length() > 1)
  {
    GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));
    
    for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
      Concat.Add( concatcurve->Value(i), MaxTol, Standard_True );
    
    concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
  }
  Handle(Geom_BSplineCurve) ResCurve = concatcurve->Value(concatcurve->Lower());
  
  TColGeom2d_SequenceOfBoundedCurve ResPCurves;
  TopLoc_Location aLoc;
  for (j = 1; j <= SurfSeq.Length(); j++)
  {
    TColGeom2d_Array1OfBSplineCurve tab_c2d(0,nb_curve-1); //array of the pcurves
    
    PrevVertex = FirstVertex;
    PrevEdge = FirstEdge;
    //TopLoc_Location theLoc = LocSeq(j).Location();
    for (i = 1; i <= nb_curve; i++)
    {
      TopoDS_Edge anEdge = TopoDS::Edge(aChain(i));
      TopoDS_Vertex VF, VL;
      TopExp::Vertices(anEdge, VF, VL);
      Standard_Boolean ToReverse = (!VF.IsSame(PrevVertex));

      /*
      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), anEdge.Location()*theLoc, fpar, lpar);
      */
      Handle(Geom2d_Curve) aPCurve =
        BRep_Tool::CurveOnSurface(anEdge, SurfSeq(j), aLoc, fpar, lpar);
      Handle(Geom2d_TrimmedCurve) aTrPCurve = new Geom2d_TrimmedCurve(aPCurve, fpar, lpar);
      tab_c2d(i-1) = Geom2dConvert::CurveToBSplineCurve(aTrPCurve);
      Geom2dConvert::C0BSplineToC1BSplineCurve(tab_c2d(i-1), Precision::Confusion());
      if (ToReverse)
        tab_c2d(i-1)->Reverse();
      PrevVertex = (ToReverse)? VF : VL;
      PrevEdge = anEdge;
    }
    Handle(TColGeom2d_HArray1OfBSplineCurve)  concatc2d;     //array of the concatenated curves
    Handle(TColStd_HArray1OfInteger)        ArrayOfInd2d;  //array of the remining Vertex
    Geom2dConvert::ConcatC1(tab_c2d,
                            tabtolvertex,
                            ArrayOfInd2d,
                            concatc2d,
                            Standard_False,
                            Precision::Confusion());   //C1 concatenation
    
    if (concatc2d->Length() > 1)
    {
      Geom2dConvert_CompCurveToBSplineCurve Concat2d(concatc2d->Value(concatc2d->Lower()));
      
      for (i = concatc2d->Lower()+1; i <= concatc2d->Upper(); i++)
        Concat2d.Add( concatc2d->Value(i), MaxTol, Standard_True );
      
      concatc2d->SetValue(concatc2d->Lower(), Concat2d.BSplineCurve());
    }
    Handle(Geom2d_BSplineCurve) aResPCurve = concatc2d->Value(concatc2d->Lower());
    ResPCurves.Append(aResPCurve);
  }
  
  ResEdge = BRepLib_MakeEdge(ResCurve,
                             FirstVertex, LastVertex,
                             ResCurve->FirstParameter(), ResCurve->LastParameter());
  BB.SameRange(ResEdge, Standard_False);
  BB.SameParameter(ResEdge, Standard_False);
  for (j = 1; j <= ResPCurves.Length(); j++)
  {
    BB.UpdateEdge(ResEdge, ResPCurves(j), SurfSeq(j), aLoc, MaxTol);
    BB.Range(ResEdge, SurfSeq(j), aLoc, ResPCurves(j)->FirstParameter(), ResPCurves(j)->LastParameter());
  }

  BRepLib::SameParameter(ResEdge, MaxTol, Standard_True);
  
  return ResEdge;
}

//=======================================================================
//function : MergeEdges
//purpose  : auxilary
//=======================================================================
static Standard_Boolean MergeEdges(const TopTools_SequenceOfShape& SeqEdges,
                                   const Standard_Real Tol,
                                   TopoDS_Edge& anEdge)
{
  // make chain for union
  BRep_Builder B;
  ShapeAnalysis_Edge sae;
  TopoDS_Edge FirstE = TopoDS::Edge(SeqEdges.Value(1));
  TopoDS_Edge LastE = FirstE;
  TopoDS_Vertex VF = sae.FirstVertex(FirstE);
  TopoDS_Vertex VL = sae.LastVertex(LastE);
  TopTools_SequenceOfShape aChain;
  aChain.Append(FirstE);
  TColStd_MapOfInteger IndUsedEdges;
  IndUsedEdges.Add(1);
  Standard_Integer j;
  for(j=2; j<=SeqEdges.Length(); j++) {
    for(Standard_Integer k=2; k<=SeqEdges.Length(); k++) {
      if(IndUsedEdges.Contains(k)) continue;
      TopoDS_Edge edge = TopoDS::Edge(SeqEdges.Value(k));
      TopoDS_Vertex VF2 = sae.FirstVertex(edge);
      TopoDS_Vertex VL2 = sae.LastVertex(edge);
      if(sae.FirstVertex(edge).IsSame(VL)) {
        aChain.Append(edge);
        LastE = edge;
        VL = sae.LastVertex(LastE);
        IndUsedEdges.Add(k);
      }
      else if(sae.LastVertex(edge).IsSame(VF)) {
        aChain.Prepend(edge);
        FirstE = edge;
        VF = sae.FirstVertex(FirstE);
        IndUsedEdges.Add(k);
      }
    }
  }
  if(aChain.Length()<SeqEdges.Length()) {
    MESSAGE ("can not create correct chain...");
    return Standard_False;
  }

  // union edges in chain
  // first step: union lines and circles
  TopLoc_Location Loc;
  Standard_Real fp1,lp1,fp2,lp2;
  for(j=1; j<aChain.Length(); j++) {
    TopoDS_Edge edge1 = TopoDS::Edge(aChain.Value(j));
    Handle(Geom_Curve) c3d1 = BRep_Tool::Curve(edge1,Loc,fp1,lp1);
    if(c3d1.IsNull()) break;
    while(c3d1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d1);
      c3d1 = tc->BasisCurve();
    }
    TopoDS_Edge edge2 = TopoDS::Edge(aChain.Value(j+1));
    Handle(Geom_Curve) c3d2 = BRep_Tool::Curve(edge2,Loc,fp2,lp2);
    if(c3d2.IsNull()) break;
    while(c3d2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) tc =
        Handle(Geom_TrimmedCurve)::DownCast(c3d2);
      c3d2 = tc->BasisCurve();
    }
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Line)) && c3d2->IsKind(STANDARD_TYPE(Geom_Line)) ) {
      // union lines
      Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast(c3d1);
      Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast(c3d2);
      gp_Dir Dir1 = L1->Position().Direction();
      gp_Dir Dir2 = L2->Position().Direction();
      //if(!Dir1.IsEqual(Dir2,Precision::Angular())) {
      //if(!Dir1.IsParallel(Dir2,Precision::Angular())) {
      if(!Dir1.IsParallel(Dir2,Tol)) {
        continue;
      }
      // can union lines => create new edge
      TopoDS_Vertex V1 = sae.FirstVertex(edge1);
      gp_Pnt PV1 = BRep_Tool::Pnt(V1);
      TopoDS_Vertex V2 = sae.LastVertex(edge2);
      gp_Pnt PV2 = BRep_Tool::Pnt(V2);
      gp_Vec Vec(PV1,PV2);
      Handle(Geom_Line) L = new Geom_Line(gp_Ax1(PV1,Vec));
      Standard_Real dist = PV1.Distance(PV2);
      Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(L,0.0,dist);
      TopoDS_Edge E;
      B.MakeEdge (E,tc,Precision::Confusion());
      B.Add (E,V1);  B.Add (E,V2);
      B.UpdateVertex(V1, 0., E, 0.);
      B.UpdateVertex(V2, dist, E, 0.);
      aChain.Remove(j);
      aChain.SetValue(j,E);
      j--;
    }
    if( c3d1->IsKind(STANDARD_TYPE(Geom_Circle)) && c3d2->IsKind(STANDARD_TYPE(Geom_Circle)) ) {
      // union circles
      Handle(Geom_Circle) C1 = Handle(Geom_Circle)::DownCast(c3d1);
      Handle(Geom_Circle) C2 = Handle(Geom_Circle)::DownCast(c3d2);
      gp_Pnt P01 = C1->Location();
      gp_Pnt P02 = C2->Location();
      if (P01.Distance(P02) > Precision::Confusion()) continue;
      // can union circles => create new edge
      TopoDS_Vertex V1 = sae.FirstVertex(edge1);
      gp_Pnt PV1 = BRep_Tool::Pnt(V1);
      TopoDS_Vertex V2 = sae.LastVertex(edge2);
      gp_Pnt PV2 = BRep_Tool::Pnt(V2);
      TopoDS_Vertex VM = sae.LastVertex(edge1);
      gp_Pnt PVM = BRep_Tool::Pnt(VM);
      GC_MakeCircle MC (PV1,PVM,PV2);
      Handle(Geom_Circle) C;
      TopoDS_Edge E;

      if (MC.IsDone()) {
        C = MC.Value();
      }

      if (C.IsNull()) {
        // jfa for Mantis issue 0020228
        if (PV1.Distance(PV2) > Precision::Confusion()) continue;
        // closed chain. Make a closed circular edge starting from V1.
        gp_Ax1 anAxis = C1->Axis();

        if (edge1.Orientation() == TopAbs_REVERSED) {
          anAxis.Reverse();
        }

        const gp_Pnt &aP0 = anAxis.Location();
        gp_Dir        aDX(PV1.XYZ().Subtracted(aP0.XYZ()));
        gp_Ax2        aNewAxis(aP0, anAxis.Direction(), aDX);

        C = new Geom_Circle(aNewAxis, C1->Radius());

        B.MakeEdge (E,C,Precision::Confusion());
        B.Add(E,V1);
        B.Add(E,V2);
        B.UpdateVertex(V1, 0., E, 0.);
        B.UpdateVertex(V2, 2.*M_PI, E, 0.);
      }
      else {
        gp_Pnt P0 = C->Location();
        gp_Dir D1(gp_Vec(P0,PV1));
        gp_Dir D2(gp_Vec(P0,PV2));
        Standard_Real fpar = C->XAxis().Direction().Angle(D1);
        if(fabs(fpar)>Precision::Confusion()) {
          // check orientation
          gp_Dir ND =  C->XAxis().Direction().Crossed(D1);
          if(ND.IsOpposite(C->Axis().Direction(),Precision::Confusion())) {
            fpar = -fpar;
          }
        }
        Standard_Real lpar = C->XAxis().Direction().Angle(D2);
        if(fabs(lpar)>Precision::Confusion()) {
          // check orientation
          gp_Dir ND =  C->XAxis().Direction().Crossed(D2);
          if(ND.IsOpposite(C->Axis().Direction(),Precision::Confusion())) {
            lpar = -lpar;
          }
        }
        if (lpar < fpar) lpar += 2*M_PI;
        Handle(Geom_TrimmedCurve) tc = new Geom_TrimmedCurve(C,fpar,lpar);
        B.MakeEdge (E,tc,Precision::Confusion());
        B.Add(E,V1);
        B.Add(E,V2);
        B.UpdateVertex(V1, fpar, E, 0.);
        B.UpdateVertex(V2, lpar, E, 0.);
      }
      aChain.Remove(j);
      aChain.SetValue(j,E);
      j--;
    }
  }
  if (j < aChain.Length()) {
    MESSAGE ("null curve3d in edge...");
    return Standard_False;
  }
  if (aChain.Length() > 1) {
    // second step: union edges with various curves
    // skl for bug 0020052 from Mantis: perform such unions
    // only if curves are bspline or bezier
    bool NeedUnion = true;
    for(j=1; j<=aChain.Length(); j++) {
      TopoDS_Edge edge = TopoDS::Edge(aChain.Value(j));
      Handle(Geom_Curve) c3d = BRep_Tool::Curve(edge,Loc,fp1,lp1);
      if(c3d.IsNull()) continue;
      while(c3d->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
        Handle(Geom_TrimmedCurve) tc =
          Handle(Geom_TrimmedCurve)::DownCast(c3d);
        c3d = tc->BasisCurve();
      }
      if( ( c3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) ||
            c3d->IsKind(STANDARD_TYPE(Geom_BezierCurve)) ) ) continue;
      NeedUnion = false;
      break;
    }
    if(NeedUnion) {
      MESSAGE ("can not make analitical union => make approximation");
      TopoDS_Edge E = GlueEdgesWithPCurves(aChain, VF, VL);
      aChain.SetValue(1,E);
    }
    else {
      MESSAGE ("can not make approximation for such types of curves");
      return Standard_False;
    }
  }

  anEdge = TopoDS::Edge(aChain.Value(1));
  return Standard_True;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
TopoDS_Shape BlockFix_UnionEdges::Perform(const TopoDS_Shape& theShape,
                                          const Standard_Real theTol)
{
  // Fill Map of edges as keys and list of faces as items.
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
  Standard_Boolean isModified = Standard_False;

  TopExp::MapShapesAndAncestors
    (theShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

  // processing each face
  Handle(ShapeBuild_ReShape) aContext = new ShapeBuild_ReShape;
  TopTools_MapOfShape        aProcessed;
  TopTools_MapOfShape        aModifiedFaces;
  TopExp_Explorer            anExpF(theShape, TopAbs_FACE);

  for (; anExpF.More(); anExpF.Next()) {
    // Processing of each wire of the face
    TopoDS_Face aFace = TopoDS::Face(anExpF.Current());

    if (!aProcessed.Add(aFace)) {
      continue;
    }

    TopExp_Explorer anExpW(aFace, TopAbs_WIRE);

    for (; anExpW.More(); anExpW.Next()) {
      // Get the ordered list of edges in the wire.
      TopoDS_Wire            aWire = TopoDS::Wire(anExpW.Current());
      BRepTools_WireExplorer aWExp(aWire, aFace);
      TopTools_ListOfShape   aChainEdges;
      Standard_Integer       aNbEdges = 0;

      for (; aWExp.More(); aWExp.Next(), aNbEdges++) {
        aChainEdges.Append(aWExp.Current());
      }

      if (aNbEdges < 2) {
        // Nothing to merge.
        continue;
      }

      // Fill the list of flags that neighbour edges can be merged.
      TColStd_ListOfInteger aChainCanMerged;
      TopoDS_Edge           anEdge1 = TopoDS::Edge(aChainEdges.Last());
      TopoDS_Edge           anEdge2;
      Standard_Boolean      isToMerge;
      TopTools_ListIteratorOfListOfShape anIter(aChainEdges);
      Standard_Boolean      isFirstMerge = Standard_False;
      Standard_Boolean      isFirst = Standard_True;
      Standard_Boolean      isReorder = Standard_False;

      // The first element is the flag between last and first edges.
      for (; anIter.More(); anIter.Next()) {
        anEdge2 = TopoDS::Edge(anIter.Value());
 
        if (aProcessed.Contains(anEdge1) || aProcessed.Contains(anEdge2)) {
          // No need to merge already processed edges.
          isToMerge = Standard_False;
        } else {
          isToMerge = IsToMerge(anEdge1, anEdge2, aMapEdgeFaces, theTol);
        }

        aChainCanMerged.Append(isToMerge);
        anEdge1 = anEdge2;

        if (isFirst) {
          isFirstMerge = isToMerge;
          isFirst      = Standard_False;
        } else if (isFirstMerge && !isToMerge) {
          isReorder = Standard_True;
        }
      }

      // Fill the map of processed shape by the edges.
      for (anIter.Initialize(aChainEdges); anIter.More(); anIter.Next()) {
        aProcessed.Add(anIter.Value());
      }

      // Reorder edges in the chain.
      if (isReorder) {
        // Find the first edge that can't be merged.
        while (aChainCanMerged.First()) {
          TopoDS_Shape aTmpShape = aChainEdges.First();

          isToMerge = aChainCanMerged.First();
          aChainCanMerged.RemoveFirst();
          aChainCanMerged.Append(isToMerge);
          aChainEdges.RemoveFirst();
          aChainEdges.Append(aTmpShape);
        }
      }

      // Merge parts of chain to be merged.
      TColStd_ListIteratorOfListOfInteger aFlagIter(aChainCanMerged);
      anIter.Initialize(aChainEdges);

      while (anIter.More()) {
        TopTools_SequenceOfShape aSeqEdges;

        aSeqEdges.Append(anIter.Value());
        aFlagIter.Next();
        anIter.Next();

        for (; anIter.More(); anIter.Next(), aFlagIter.Next()) {
          if (aFlagIter.Value()) {
            // Continue the chain.
            aSeqEdges.Append(anIter.Value());
          } else {
            // Stop the chain.
            break;
          }
        }

        const Standard_Integer aNbEdges = aSeqEdges.Length();

        if (aNbEdges > 1) {
          // There are several edges to be merged.
          TopoDS_Edge aMergedEdge;

          if (MergeEdges(aSeqEdges, theTol, aMergedEdge)) {
            isModified = Standard_True;
            // now we have only one edge - aMergedEdge.
            // we have to replace old ListEdges with this new edge
            const TopoDS_Shape &anEdge = aSeqEdges.Value(1);

            aContext->Replace(anEdge, aMergedEdge);

            for (Standard_Integer j = 2; j <= aNbEdges; j++) {
              aContext->Remove(aSeqEdges(j));
            }

            // Fix affected faces.
            if (aMapEdgeFaces.Contains(anEdge)) {
              const TopTools_ListOfShape &aList =
                aMapEdgeFaces.FindFromKey(anEdge);
              TopTools_ListIteratorOfListOfShape anIter(aList);

              for (; anIter.More(); anIter.Next()) {
                aModifiedFaces.Add(anIter.Value());
              }
            }
          }
        }
      }
    }
  }

  if (isModified) {
    // Fix modified faces.
    TopTools_MapIteratorOfMapOfShape aModifIt(aModifiedFaces);

    for (; aModifIt.More(); aModifIt.Next()) {
      TopoDS_Face aModifiedFace =
        TopoDS::Face(aContext->Apply(aModifIt.Key()));
      Handle(ShapeFix_Face) aSff = new ShapeFix_Face(aModifiedFace);

      aSff->SetContext(aContext);
      aSff->SetPrecision(theTol);
      aSff->SetMinTolerance(theTol);
      aSff->SetMaxTolerance(Max(1., theTol*1000.));
      aSff->Perform();
      aContext->Replace(aModifiedFace, aSff->Face());
    }

    // Check if the result shape contains shells.
    // If yes, fix the faces orientation in the shell.
    TopoDS_Shape    aModifShape = aContext->Apply(theShape);
    TopExp_Explorer anExpSh(aModifShape, TopAbs_SHELL);

    for (; anExpSh.More(); anExpSh.Next()) {
      TopoDS_Shell           aShell = TopoDS::Shell(anExpSh.Current());
      Handle(ShapeFix_Shell) aSfsh = new ShapeFix_Shell;

      aSfsh->FixFaceOrientation(aShell);
      aContext->Replace(aShell, aSfsh->Shell());
    }
  }

  const TopoDS_Shape aResult = aContext->Apply(theShape);

  return aResult;
}
