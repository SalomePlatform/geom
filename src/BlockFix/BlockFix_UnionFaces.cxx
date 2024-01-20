// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//  File:    BlockFix_UnionFaces.cxx
//  Created: Tue Dec  7 17:15:42 2004
//  Author:  Pavel DURANDIN

#include <BlockFix_UnionFaces.hxx>

#include <Basics_OCCTVersion.hxx>

#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeAnalysis_Edge.hxx>

#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>

#include <ShapeExtend_WireData.hxx>
#include <ShapeExtend_CompositeSurface.hxx>

#include <ShapeFix_Face.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeFix_SequenceOfWireSegment.hxx>
#include <ShapeFix_WireSegment.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Edge.hxx>

#include <IntPatch_ImpImpIntersection.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

#include <TColGeom_HArray2OfSurface.hxx>

#if OCC_VERSION_LARGE < 0x07070000
#include <GeomAdaptor_HSurface.hxx>
#else
#include <GeomAdaptor_Surface.hxx>
#endif

#include <GeomLib_IsPlanarSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#if OCC_VERSION_LARGE < 0x07070000
#include <BRepAdaptor_HSurface.hxx>
#endif
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>

#include <Geom2d_Line.hxx>

#include <gp_XY.hxx>
#include <gp_Pnt2d.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=======================================================================
//function : BlockFix_UnionFaces
//purpose  :
//=======================================================================
BlockFix_UnionFaces::BlockFix_UnionFaces()
  : myTolerance(Precision::Confusion()),
    myOptimumNbFaces(6)
{
}

//=======================================================================
//function : GetTolerance
//purpose  :
//=======================================================================
Standard_Real& BlockFix_UnionFaces::GetTolerance()
{
  return myTolerance;
}

//=======================================================================
//function : GetOptimumNbFaces
//purpose  :
//=======================================================================
Standard_Integer& BlockFix_UnionFaces::GetOptimumNbFaces()
{
  return myOptimumNbFaces;
}

//=======================================================================
//function : AddOrdinaryEdges
//purpose  : auxiliary
//           adds edges from the shape to the sequence
//           seams and equal edges are dropped
//           Returns true if one of original edges dropped
//=======================================================================
static Standard_Boolean AddOrdinaryEdges(TopTools_SequenceOfShape& edges,
                                         const TopoDS_Shape aShape,
                                         Standard_Integer& anIndex)
{
  //map of edges
  TopTools_MapOfShape aNewEdges;
  TopExp_Explorer exp(aShape,TopAbs_EDGE);
  //add edges without seams
  for(; exp.More(); exp.Next()) {
    TopoDS_Shape edge = exp.Current();
    if(aNewEdges.Contains(edge))
      aNewEdges.Remove(edge);
    else
      aNewEdges.Add(edge);
  }

  Standard_Boolean isDropped = Standard_False;
  //merge edges and drop seams
  for(Standard_Integer i = 1; i <= edges.Length(); i++) {
    TopoDS_Shape current = edges(i);
    if(aNewEdges.Contains(current)) {

      aNewEdges.Remove(current);
      edges.Remove(i);
      i--;

      if(!isDropped) {
        isDropped = Standard_True;
        anIndex = i;
      }
    }
  }

  //add edges to the sequence
  for(exp.ReInit(); exp.More(); exp.Next()) {
    const TopoDS_Shape &anEdge = exp.Current();

    if (aNewEdges.Contains(anEdge)) {
      edges.Append(anEdge);
    }
  }

  return isDropped;
}

//=======================================================================
//function : ClearRts
//purpose  : auxiliary
//=======================================================================
static Handle(Geom_Surface) ClearRts(const Handle(Geom_Surface)& aSurface)
{
  if(aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) rts =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    return rts->BasisSurface();
  }
  return aSurface;
}

//=======================================================================
//function : IsFacesOfSameSolids
//purpose  : auxiliary
//=======================================================================
static Standard_Boolean IsFacesOfSameSolids
       (const TopoDS_Face                               &theFace1,
        const TopoDS_Face                               &theFace2,
        const TopTools_IndexedDataMapOfShapeListOfShape &theMapFaceSolids)
{
  Standard_Boolean isSame = Standard_False;

  if (theMapFaceSolids.Contains(theFace1) &&
      theMapFaceSolids.Contains(theFace2)) {
    const TopTools_ListOfShape& aList1 = theMapFaceSolids.FindFromKey(theFace1);
    const TopTools_ListOfShape& aList2 = theMapFaceSolids.FindFromKey(theFace2);

    if (aList1.Extent() == aList2.Extent()) {
      TopTools_ListIteratorOfListOfShape anIter1(aList1);

      isSame = Standard_True;

      for (; anIter1.More(); anIter1.Next()) {
        const TopoDS_Shape                 &aSolid1 = anIter1.Value();
        TopTools_ListIteratorOfListOfShape  anIter2(aList2);

        for (; anIter2.More(); anIter2.Next()) {
          if (aSolid1.IsSame(anIter2.Value())) {
            // Same solid is detected. Break the loop
            break;
          }
        }

        if (!anIter2.More()) {
          // No same solid is detected. Break the loop.
          isSame = Standard_False;
          break;
        }
      }
    }
  }

  return isSame;
}

//=======================================================================
//function : DefineMaxTolerance
//purpose  : calculates maximum possible tolerance on edges of shape
//=======================================================================
static Standard_Real DefineMaxTolerance(const TopoDS_Shape& theShape)
{
  Standard_Real aTol = Precision::Confusion();

  Standard_Real MinSize = RealLast();
  TopExp_Explorer Explo(theShape, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    Bnd_Box aBox;
    BRepBndLib::Add(anEdge, aBox);
    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    aBox.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
    Standard_Real MaxSize = Max(Xmax - Xmin, Max(Ymax - Ymin, Zmax - Zmin));
    if (MaxSize < MinSize)
      MinSize = MaxSize;
  }

  if (!Precision::IsInfinite(MinSize))
    aTol = 0.1 * MinSize;

  return aTol;
}

//=======================================================================
//function : IsTangentFaces
//purpose  : decides: is edge on closed surface tangent or not
//=======================================================================
static Standard_Boolean IsTangentFaces(const TopoDS_Edge& theEdge,
                                       const TopoDS_Face& theFace)
{
  Standard_Real TolC0 = Max(0.001, 1.5*BRep_Tool::Tolerance(theEdge));

  Standard_Real aFirst;
  Standard_Real aLast;
    
// Obtaining of pcurves of edge on two faces.
  const Handle(Geom2d_Curve) aC2d1 = BRep_Tool::CurveOnSurface
                                                (theEdge, theFace, aFirst, aLast);
  TopoDS_Edge ReversedEdge = theEdge;
  ReversedEdge.Reverse();
  const Handle(Geom2d_Curve) aC2d2 = BRep_Tool::CurveOnSurface
                                                (ReversedEdge, theFace, aFirst, aLast);
  if (aC2d1.IsNull() || aC2d2.IsNull())
    return Standard_False;

// Obtaining of two surfaces from adjacent faces.
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace);

  if (aSurf.IsNull())
    return Standard_False;

// Computation of the number of samples on the edge.
  BRepAdaptor_Surface              aBAS(theFace);
#if OCC_VERSION_LARGE < 0x07070000
  Handle(BRepAdaptor_HSurface)     aBAHS      = new BRepAdaptor_HSurface(aBAS);
#else
  Handle(BRepAdaptor_Surface)      aBAHS      = new BRepAdaptor_Surface(aBAS);
#endif
  Handle(BRepTopAdaptor_TopolTool) aTool      = new BRepTopAdaptor_TopolTool(aBAHS);
  Standard_Integer                 aNbSamples =     aTool->NbSamples();
  const Standard_Integer           aNbSamplesMax =   23;
  aNbSamples = Min(aNbSamplesMax, aNbSamples);
  const Standard_Real              aTolAngle   =     M_PI/18;


// Computation of the continuity.
  Standard_Real    aPar;
  Standard_Real    aDelta = (aLast - aFirst)/(aNbSamples - 1);
  Standard_Integer i, nbNotDone = 0;

  for (i = 1, aPar = aFirst; i <= aNbSamples; i++, aPar += aDelta) {
    if (i == aNbSamples) aPar = aLast;

    LocalAnalysis_SurfaceContinuity aCont(aC2d1,  aC2d2,  aPar,
                                          aSurf, aSurf, GeomAbs_G1,
                                          0.001, TolC0, aTolAngle, 0.1, 0.1);
    if (!aCont.IsDone()) 
    {
      nbNotDone++;
      continue;
    }

    if (!aCont.IsG1())
      return Standard_False;
  }
  
  if (nbNotDone == aNbSamples)
    return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : HasSeamEdge
//purpose  : Detects if a face contains a seam edge
//=======================================================================
static Standard_Boolean HasSeamEdge(const TopoDS_Face& theFace)
{
  TopExp_Explorer Explo(theFace, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    if (BRepTools::IsReallyClosed(anEdge, theFace))
      return Standard_True;
  }

  return Standard_False;
}


//=======================================================================
//function : IsEdgeValidToMerge
//purpose  : Edge is valid if it is not seam or if it is a seam and the face
//           has another seam edge.
//=======================================================================
static Standard_Boolean IsEdgeValidToMerge(const TopoDS_Edge& theEdge,
                                           const TopoDS_Face& theFace,
                                           const Handle(Geom_Surface)& theSurface,
                                           Standard_Boolean& theIsEdgeOnSeam,
                                           Standard_Boolean& theToMakeUPeriodic,
                                           Standard_Boolean& theToMakeVPeriodic)
{
  Standard_Boolean isValid = Standard_True;

  theIsEdgeOnSeam |= BRep_Tool::IsClosed(theEdge, theFace);
  
  if (BRepTools::IsReallyClosed(theEdge, theFace)) {
    // Mantis issue 0023451, now code corresponds to the comment to this method
    isValid = Standard_False;

    // This is a seam edge. Check if there are another seam edges on the face.
    TopExp_Explorer anExp(theFace, TopAbs_EDGE);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aShEdge = anExp.Current();

      // Skip same edge.
      if (theEdge.IsSame(aShEdge)) {
        continue;
      }

      // Check if this edge is a seam.
      TopoDS_Edge anEdge = TopoDS::Edge(aShEdge);

      if (BRep_Tool::IsClosed(anEdge, theFace)) {
        // Mantis issue 0023451, now code corresponds to the comment to this method
        //isValid = Standard_False;
        isValid = Standard_True;
        break;
      }
    }
  }
  else if (theIsEdgeOnSeam)
  {
    Standard_Real fpar, lpar;
    Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface(theEdge, theFace, fpar, lpar);
    gp_Pnt2d P2d1 = aPCurve->Value(fpar);
    gp_Pnt2d P2d2 = aPCurve->Value(lpar);
    if (!theSurface->IsUPeriodic() &&
        theSurface->IsUClosed() &&
        Abs(P2d1.X() - P2d2.X()) < Abs(P2d1.Y() - P2d2.Y()))
    {
      if (IsTangentFaces(theEdge, theFace))
        theToMakeUPeriodic = Standard_True;
      else
        isValid = Standard_False;
    }
    if (!theSurface->IsVPeriodic() &&
        theSurface->IsVClosed() &&
        Abs(P2d1.Y() - P2d2.Y()) < Abs(P2d1.X() - P2d2.X()))
    {
      if (IsTangentFaces(theEdge, theFace))
        theToMakeVPeriodic = Standard_True;
      else
        isValid = Standard_False;
    }
  }

  return isValid;
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
TopoDS_Shape BlockFix_UnionFaces::Perform(const TopoDS_Shape& Shape)
{
  // Fill Map of faces as keys and list of solids or shells as items.
  TopTools_IndexedDataMapOfShapeListOfShape aMapFaceSoOrSh;

  TopAbs_ShapeEnum aType = Shape.ShapeType();

  if (aType != TopAbs_SHELL) {
    aType = TopAbs_SOLID;
  }

  TopExp::MapShapesAndAncestors
    (Shape, TopAbs_FACE, aType, aMapFaceSoOrSh);

  // processing each solid
  Handle(ShapeBuild_ReShape) aContext = new ShapeBuild_ReShape;
  TopTools_MapOfShape aProcessed;
  TopExp_Explorer exps;
  for (exps.Init(Shape, aType); exps.More(); exps.Next()) {
    TopoDS_Shape aSoOrSh = exps.Current();

    // creating map of edge faces
    TopTools_IndexedDataMapOfShapeListOfShape aMapEdgeFaces;
    TopExp::MapShapesAndAncestors(aSoOrSh, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);

    Standard_Integer NbModif = 0;
    Standard_Boolean hasFailed = Standard_False;
    Standard_Real tol = Min(Max(Precision::Confusion(), myTolerance/10.), 0.1);

    // count faces
    int nbf = 0;
    TopExp_Explorer exp;
    TopTools_MapOfShape mapF;
    for (exp.Init(aSoOrSh, TopAbs_FACE); exp.More(); exp.Next()) {
      if (mapF.Add(exp.Current()))
        nbf++;
    }

    bool doUnion = ((myOptimumNbFaces == 0) ||
                    ((myOptimumNbFaces > 0) && (nbf > myOptimumNbFaces)));

    // processing each face
    mapF.Clear();
    for (exp.Init(aSoOrSh, TopAbs_FACE); exp.More() && doUnion; exp.Next()) {
      TopoDS_Face aFace = TopoDS::Face(exp.Current().Oriented(TopAbs_FORWARD));

      if (aProcessed.Contains(aFace)) {
        continue;
      }

      Standard_Integer dummy;
      TopTools_SequenceOfShape edges;
      AddOrdinaryEdges(edges,aFace,dummy);

      TopTools_SequenceOfShape faces;
      faces.Append(aFace);

      //surface and location to construct result
      TopLoc_Location aBaseLocation;
      Handle(Geom_Surface) aBaseSurface = BRep_Tool::Surface(aFace,aBaseLocation);
      aBaseSurface = ClearRts(aBaseSurface);
      aBaseSurface = Handle(Geom_Surface)::DownCast(aBaseSurface->Copy());
      Standard_Boolean ToMakeUPeriodic = Standard_False, ToMakeVPeriodic = Standard_False;

      // find adjacent faces to union
      Standard_Integer i;
      for (i = 1; i <= edges.Length(); i++) {
        TopoDS_Edge edge = TopoDS::Edge(edges(i));
        Standard_Boolean IsEdgeOnSeam = Standard_False;
        if (BRep_Tool::Degenerated(edge) ||
            !IsEdgeValidToMerge(edge, aFace, aBaseSurface,
                                IsEdgeOnSeam, ToMakeUPeriodic, ToMakeVPeriodic))
          continue;

        const TopTools_ListOfShape& aList = aMapEdgeFaces.FindFromKey(edge);
        TopTools_ListIteratorOfListOfShape anIter(aList);
        for (; anIter.More(); anIter.Next()) {
          TopoDS_Face anCheckedFace = TopoDS::Face(anIter.Value().Oriented(TopAbs_FORWARD));
          if (anCheckedFace.IsSame(aFace))
            continue;

          if (aProcessed.Contains(anCheckedFace))
            continue;

          if (!IsEdgeValidToMerge(edge, anCheckedFace, aBaseSurface,
                                  IsEdgeOnSeam, ToMakeUPeriodic, ToMakeVPeriodic)) {
            // Skip seam edge.
            continue;
          }

          // Check if faces belong to same solids.
          if (!IsFacesOfSameSolids(aFace, anCheckedFace, aMapFaceSoOrSh)) {
            continue;
          }

          if (IsSameDomain(aFace,anCheckedFace)) {

            if (aList.Extent() != 2) {
              // non mainfold case is not processed
              continue;
            }

            //Prevent creating a face with parametric range more than period
            if (IsEdgeOnSeam &&
                (HasSeamEdge(aFace) || HasSeamEdge(anCheckedFace)))
              continue;

            // replacing pcurves
            TopoDS_Face aMockUpFace;
            BRep_Builder B;
            B.MakeFace(aMockUpFace,aBaseSurface,aBaseLocation,0.);
            MovePCurves(aMockUpFace,anCheckedFace);

            if (AddOrdinaryEdges(edges,aMockUpFace,dummy)) {
              // sequence edges is modified
              i = dummy;
            }

            faces.Append(anCheckedFace);
            aProcessed.Add(anCheckedFace);
            break;
          }
        }
      }

      // all faces collected in the sequence. Perform union of faces
      if (faces.Length() > 1) {
        NbModif++;
        TopoDS_Face aResult;
        BRep_Builder B;
        if (ToMakeUPeriodic || ToMakeVPeriodic)
        {
          Handle(Geom_BSplineSurface) aBSplineSurface = Handle(Geom_BSplineSurface)::DownCast(aBaseSurface);
          if (aBSplineSurface.IsNull())
          {
            Standard_Real aTol = 1.e-4;
            GeomAbs_Shape aUCont = GeomAbs_C1, aVCont = GeomAbs_C1;
            Standard_Integer degU = 14, degV = 14;
            Standard_Integer nmax = 16;
            Standard_Integer aPrec = 1;  
            GeomConvert_ApproxSurface Approximator(aBaseSurface,aTol,aUCont,aVCont,degU,degV,nmax,aPrec);
            aBSplineSurface = Approximator.Surface();
          }
          
          if (ToMakeUPeriodic)
            aBSplineSurface->SetUPeriodic();
          if (ToMakeVPeriodic)
            aBSplineSurface->SetVPeriodic();
          
          aBaseSurface = aBSplineSurface;
        }
        B.MakeFace(aResult,aBaseSurface,aBaseLocation,0);
        Standard_Integer nbWires = 0;

        // connecting wires
        while (edges.Length()>0) {

          Standard_Boolean isEdge3d = Standard_False;
          nbWires++;
          TopTools_MapOfShape aVertices;
          TopoDS_Wire aWire;
          B.MakeWire(aWire);

          TopoDS_Edge anEdge = TopoDS::Edge(edges(1));
          edges.Remove(1);

          isEdge3d |= !BRep_Tool::Degenerated(anEdge);
          B.Add(aWire,anEdge);
          TopoDS_Vertex V1,V2;
          TopExp::Vertices(anEdge,V1,V2);
          aVertices.Add(V1);
          aVertices.Add(V2);

          Standard_Boolean isNewFound = Standard_False;
          do {
            isNewFound = Standard_False;
            for(Standard_Integer j = 1; j <= edges.Length(); j++) {
              anEdge = TopoDS::Edge(edges(j));
              TopExp::Vertices(anEdge,V1,V2);
              if(aVertices.Contains(V1) || aVertices.Contains(V2)) {
                isEdge3d |= !BRep_Tool::Degenerated(anEdge);
                aVertices.Add(V1);
                aVertices.Add(V2);
                B.Add(aWire,anEdge);
                edges.Remove(j);
                j--;
                isNewFound = Standard_True;
              }
            }
          } while (isNewFound);

          // sorting any type of edges
          aWire = TopoDS::Wire(aContext->Apply(aWire));

          Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire(aWire,aResult,Precision::Confusion());
          sfw->FixReorder();
          Standard_Boolean isDegRemoved = Standard_False;
          if(!sfw->StatusReorder ( ShapeExtend_FAIL )) {
            // clear degenerated edges if at least one with 3d curve exist
            if(isEdge3d) {
              Handle(ShapeExtend_WireData) sewd = sfw->WireData();
              for(Standard_Integer j = 1; j<=sewd->NbEdges();j++) {
                TopoDS_Edge E = sewd->Edge(j);
                if(BRep_Tool::Degenerated(E)) {
                  sewd->Remove(j);
                  isDegRemoved = Standard_True;
                  j--;
                }
              }
            }
            sfw->FixShifted();
            if(isDegRemoved)
              sfw->FixDegenerated();
          }
          TopoDS_Wire aWireFixed = sfw->Wire();
          aContext->Replace(aWire,aWireFixed);
          // add resulting wire
          if (isEdge3d) {
            B.Add(aResult,aWireFixed);
          }
          else  {
            // sorting edges
            Handle(ShapeExtend_WireData) sbwd = sfw->WireData();
            Standard_Integer nbEdges = sbwd->NbEdges();
            // sort degenerated edges and create one edge instead of several ones
            ShapeAnalysis_WireOrder sawo(Standard_False, 0);
            ShapeAnalysis_Edge sae;
            Standard_Integer aLastEdge = nbEdges;
            for (Standard_Integer j = 1; j <= nbEdges; j++) {
              Standard_Real f,l;
              //smh protection on NULL pcurve
              Handle(Geom2d_Curve) c2d;
              if (!sae.PCurve(sbwd->Edge(j),aResult,c2d,f,l)) {
                aLastEdge--;
                continue;
              }
              sawo.Add(c2d->Value(f).XY(),c2d->Value(l).XY());
            }
            if (aLastEdge > 0) {
              sawo.Perform();

              // constructing one degenerative edge
              gp_XY aStart, anEnd, tmp;
              Standard_Integer nbFirst = sawo.Ordered(1);
              TopoDS_Edge anOrigE = TopoDS::Edge(sbwd->Edge(nbFirst).Oriented(TopAbs_FORWARD));
              ShapeBuild_Edge sbe;
              TopoDS_Vertex aDummyV;
              TopoDS_Edge E = sbe.CopyReplaceVertices(anOrigE,aDummyV,aDummyV);
              sawo.XY(nbFirst,aStart,tmp);
              sawo.XY(sawo.Ordered(aLastEdge),tmp,anEnd);

              gp_XY aVec = anEnd-aStart;
              Handle(Geom2d_Line) aLine = new Geom2d_Line(aStart,gp_Dir2d(anEnd-aStart));

              B.UpdateEdge(E,aLine,aResult,0.);
              B.Range(E,aResult,0.,aVec.Modulus());
              Handle(Geom_Curve) C3d;
              B.UpdateEdge(E,C3d,0.);
              B.Degenerated(E,Standard_True);
              TopoDS_Wire aW;
              B.MakeWire(aW);
              B.Add(aW,E);
              B.Add(aResult,aW);
            }
          }
        }

        // perform substitution of face
        aContext->Replace(aContext->Apply(aFace),aResult);

        ShapeFix_Face sff (aResult);
        //Initializing by tolerances
        sff.SetPrecision(myTolerance);
        sff.SetMinTolerance(tol);
        Standard_Real MaxTol = DefineMaxTolerance(aResult);
        sff.SetMaxTolerance(MaxTol);
        //Setting modes
        sff.FixOrientationMode() = 0;
        //sff.FixWireMode() = 0;
        sff.SetContext(aContext);
        // Applying the fixes
        sff.Perform();
        if(sff.Status(ShapeExtend_FAIL))
        hasFailed = Standard_True;

        // breaking down to several faces
        TopoDS_Shape theResult = aContext->Apply(aResult);
        for (TopExp_Explorer aFaceExp (theResult,TopAbs_FACE); aFaceExp.More(); aFaceExp.Next()) {
          TopoDS_Face aCurrent = TopoDS::Face(aFaceExp.Current().Oriented(TopAbs_FORWARD));
          Handle(TColGeom_HArray2OfSurface) grid = new TColGeom_HArray2OfSurface ( 1, 1, 1, 1 );
          grid->SetValue ( 1, 1, aBaseSurface );
          Handle(ShapeExtend_CompositeSurface) G = new ShapeExtend_CompositeSurface ( grid );
          ShapeFix_ComposeShell CompShell;
          CompShell.Init ( G, aBaseLocation, aCurrent, ::Precision::Confusion() );//myPrecision
          CompShell.SetContext( aContext );

          TopTools_SequenceOfShape parts;
          ShapeFix_SequenceOfWireSegment wires;
          for(TopExp_Explorer W_Exp(aCurrent,TopAbs_WIRE);W_Exp.More();W_Exp.Next()) {
            Handle(ShapeExtend_WireData) sbwd =
              new ShapeExtend_WireData ( TopoDS::Wire(W_Exp.Current() ));
            ShapeFix_WireSegment seg ( sbwd, TopAbs_REVERSED );
            wires.Append(seg);
          }

          CompShell.DispatchWires ( parts,wires );
          for (Standard_Integer j=1; j <= parts.Length(); j++ ) {
            ShapeFix_Face aFixOrient(TopoDS::Face(parts(j)));
            aFixOrient.SetContext(aContext);
            aFixOrient.FixOrientation();
          }

          TopoDS_Shape CompRes;
          if ( faces.Length() !=1 ) {
            TopoDS_Shell S;
            B.MakeShell ( S );
            for ( i=1; i <= parts.Length(); i++ )
              B.Add ( S, parts(i) );
            CompRes = S;
          }
          else CompRes = parts(1);

          aContext->Replace(aCurrent,CompRes);
        }

        // remove the remaining faces
        for(i = 2; i <= faces.Length(); i++)
          aContext->Remove(faces(i));
      }
    } // end processing each face

    //TopoDS_Shape aResult = Shape;
    if (NbModif > 0 && !hasFailed) {
      TopoDS_Shape aResult = aContext->Apply(aSoOrSh);

      ShapeFix_Edge sfe;
      for (exp.Init(aResult,TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge E = TopoDS::Edge(exp.Current());
        sfe.FixVertexTolerance (E);
        // ptv add fix same parameter
        sfe.FixSameParameter(E, myTolerance);
      }
    }

    for (exp.Init(aSoOrSh, TopAbs_FACE); exp.More(); exp.Next()) {
      TopoDS_Face aFace = TopoDS::Face(exp.Current().Oriented(TopAbs_FORWARD));
      Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
      sfw->SetContext(aContext);
      sfw->SetPrecision(myTolerance);
      sfw->SetMinTolerance(myTolerance);
      sfw->SetMaxTolerance(Max(1.,myTolerance*1000.));
      sfw->SetFace(aFace);
      for (TopoDS_Iterator iter (aFace,Standard_False); iter.More(); iter.Next()) {
        TopoDS_Shape aFaceCont = iter.Value();
        if (!aFaceCont.IsNull() && aFaceCont.ShapeType() == TopAbs_WIRE) {
          TopoDS_Wire wire = TopoDS::Wire(iter.Value());
          sfw->Load(wire);
          sfw->FixReorder();
          sfw->FixShifted();
        }
      }
    }
  } // end processing each solid

  const TopoDS_Shape aResShape = aContext->Apply(Shape);

  return aResShape;
}

//=======================================================================
//function : IsSameDomain
//purpose  :
//=======================================================================
bool getCylinder (Handle(Geom_Surface)& theInSurface, gp_Cylinder& theOutCylinder)
{
  bool isCylinder = false;

  if (theInSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    Handle(Geom_CylindricalSurface) aGC = Handle(Geom_CylindricalSurface)::DownCast(theInSurface);

    theOutCylinder = aGC->Cylinder();
    isCylinder = true;
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) aRS =
      Handle(Geom_SurfaceOfRevolution)::DownCast(theInSurface);
    Handle(Geom_Curve) aBasis = aRS->BasisCurve();

    while (aBasis->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) aTc =
        Handle(Geom_TrimmedCurve)::DownCast(aBasis);
      aBasis = aTc->BasisCurve();
    }

    if (aBasis->IsKind(STANDARD_TYPE(Geom_Line))) {
      Handle(Geom_Line) aBasisLine = Handle(Geom_Line)::DownCast(aBasis);
      gp_Dir aDir = aRS->Direction();
      gp_Dir aBasisDir = aBasisLine->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Confusion())) {
        // basis line is parallel to the revolution axis: it is a cylinder
        gp_Pnt aLoc = aRS->Location();
        Standard_Real aR = aBasisLine->Lin().Distance(aLoc);
        gp_Ax3 aCylAx (aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder = true;
      }
    }
  }
  else if (theInSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) aLES =
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(theInSurface);
    Handle(Geom_Curve) aBasis = aLES->BasisCurve();

    while (aBasis->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) aTc =
        Handle(Geom_TrimmedCurve)::DownCast(aBasis);
      aBasis = aTc->BasisCurve();
    }

    if (aBasis->IsKind(STANDARD_TYPE(Geom_Circle))) {
      Handle(Geom_Circle) aBasisCircle = Handle(Geom_Circle)::DownCast(aBasis);
      gp_Dir aDir = aLES->Direction();
      gp_Dir aBasisDir = aBasisCircle->Position().Direction();
      if (aBasisDir.IsParallel(aDir, Precision::Confusion())) {
        // basis circle is normal to the extrusion axis: it is a cylinder
        gp_Pnt aLoc = aBasisCircle->Location();
        Standard_Real aR = aBasisCircle->Radius();
        gp_Ax3 aCylAx (aLoc, aDir);

        theOutCylinder = gp_Cylinder(aCylAx, aR);
        isCylinder = true;
      }
    }
  }
  else {
  }

  return isCylinder;
}

Standard_Boolean BlockFix_UnionFaces::IsSameDomain(const TopoDS_Face& aFace,
                                                   const TopoDS_Face& aCheckedFace) const
{
  //checking the same handles
  TopLoc_Location L1, L2;
  Handle(Geom_Surface) S1, S2;

  S1 = BRep_Tool::Surface(aFace,L1);
  S2 = BRep_Tool::Surface(aCheckedFace,L2);

  if (S1 == S2 && L1 == L2)
    return true;

  // planar and cylindrical cases (IMP 20052)
  Standard_Real aPrec = Precision::Confusion();

  S1 = BRep_Tool::Surface(aFace);
  S2 = BRep_Tool::Surface(aCheckedFace);

  S1 = ClearRts(S1);
  S2 = ClearRts(S2);

  //Handle(Geom_OffsetSurface) aGOFS1, aGOFS2;
  //aGOFS1 = Handle(Geom_OffsetSurface)::DownCast(S1);
  //aGOFS2 = Handle(Geom_OffsetSurface)::DownCast(S2);
  //if (!aGOFS1.IsNull()) S1 = aGOFS1->BasisSurface();
  //if (!aGOFS2.IsNull()) S2 = aGOFS2->BasisSurface();

  // case of two elementary surfaces: use OCCT tool
  // elementary surfaces: ConicalSurface, CylindricalSurface,
  //                      Plane, SphericalSurface and ToroidalSurface
  if (S1->IsKind(STANDARD_TYPE(Geom_ElementarySurface)) &&
      S2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
#if OCC_VERSION_LARGE < 0x07070000
    Handle(GeomAdaptor_HSurface) aGA1 = new GeomAdaptor_HSurface(S1);
    Handle(GeomAdaptor_HSurface) aGA2 = new GeomAdaptor_HSurface(S2);
#else
    Handle(GeomAdaptor_Surface) aGA1 = new GeomAdaptor_Surface(S1);
    Handle(GeomAdaptor_Surface) aGA2 = new GeomAdaptor_Surface(S2);
#endif

    Handle(BRepTopAdaptor_TopolTool) aTT1 = new BRepTopAdaptor_TopolTool();
    Handle(BRepTopAdaptor_TopolTool) aTT2 = new BRepTopAdaptor_TopolTool();

    try {
      OCC_CATCH_SIGNALS;

      IntPatch_ImpImpIntersection anIIInt (aGA1, aTT1, aGA2, aTT2, aPrec, aPrec);

      if (!anIIInt.IsDone() || anIIInt.IsEmpty())
        return false;

      return anIIInt.TangentFaces();
    }
    catch (Standard_Failure&) {
      return false;
    }
  }

  // case of two planar surfaces:
  // all kinds of surfaces checked, including b-spline and bezier
  GeomLib_IsPlanarSurface aPlanarityChecker1 (S1, aPrec);
  if (aPlanarityChecker1.IsPlanar()) {
    GeomLib_IsPlanarSurface aPlanarityChecker2 (S2, aPrec);
    if (aPlanarityChecker2.IsPlanar()) {
      gp_Pln aPln1 = aPlanarityChecker1.Plan();
      gp_Pln aPln2 = aPlanarityChecker2.Plan();

      if (aPln1.Position().Direction().IsParallel(aPln2.Position().Direction(), aPrec) &&
          aPln1.Distance(aPln2) < aPrec) {
        return true;
      }
    }
  }

  // case of two cylindrical surfaces, at least one of which is a swept surface
  // swept surfaces: SurfaceOfLinearExtrusion, SurfaceOfRevolution
  if ((S1->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       S1->IsKind(STANDARD_TYPE(Geom_SweptSurface))) &&
      (S2->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       S2->IsKind(STANDARD_TYPE(Geom_SweptSurface))))
  {
    gp_Cylinder aCyl1, aCyl2;
    if (getCylinder(S1, aCyl1) && getCylinder(S2, aCyl2)) {
      if (fabs(aCyl1.Radius() - aCyl2.Radius()) < aPrec) {
        gp_Dir aDir1 = aCyl1.Position().Direction();
        gp_Dir aDir2 = aCyl2.Position().Direction();
        if (aDir1.IsParallel(aDir2, aPrec)) {
          gp_Pnt aLoc1 = aCyl1.Location();
          gp_Pnt aLoc2 = aCyl2.Location();
          gp_Vec aVec12 (aLoc1, aLoc2);
          if (aVec12.SquareMagnitude() < aPrec*aPrec ||
              aVec12.IsParallel(aDir1, aPrec)) {
            return true;
          }
        }
      }
    }
  }

  return false;
}

//=======================================================================
//function : MovePCurves
//purpose  :
//=======================================================================
void BlockFix_UnionFaces::MovePCurves(TopoDS_Face& aTarget,
                                      const TopoDS_Face& aSource) const
{
  BRep_Builder B;
  for(TopExp_Explorer wexp(aSource,TopAbs_WIRE);wexp.More();wexp.Next()) {
    Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire(TopoDS::Wire(wexp.Current()),
                                                  aTarget, Precision::Confusion());
    sfw->FixReorder();
    Standard_Boolean isReoredFailed = sfw->StatusReorder ( ShapeExtend_FAIL );
    sfw->FixEdgeCurves();
    if(isReoredFailed)
      continue;

    sfw->FixShifted();
    sfw->FixDegenerated();

    // remove degenerated edges from not degenerated points
    ShapeAnalysis_Edge sae;
    Handle(ShapeExtend_WireData) sewd = sfw->WireData();
    for(Standard_Integer i = 1; i<=sewd->NbEdges();i++) {
      TopoDS_Edge E = sewd->Edge(i);
      if(BRep_Tool::Degenerated(E)&&!sae.HasPCurve(E,aTarget)) {
        sewd->Remove(i);
        i--;
      }
    }

    TopoDS_Wire ResWire = sfw->Wire();
    B.Add(aTarget,ResWire);
  }
}
