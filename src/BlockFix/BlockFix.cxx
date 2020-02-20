// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        BlockFix.cxx
// Created:     Tue Dec  7 11:59:05 2004
// Author:      Pavel DURANDIN

#include <BlockFix.hxx>

#include <BlockFix_SphereSpaceModifier.hxx>
#include <BlockFix_PeriodicSurfaceModifier.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <BRepAdaptor_Surface.hxx>

#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRepTools_Substitution.hxx>

#include <BRepOffsetAPI_MakeFilling.hxx>

#include <ShapeFix.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>

#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <ShapeCustom.hxx>

#include <ShapeBuild_Edge.hxx>
#include <ShapeBuild_ReShape.hxx>

#include <ShapeFix_Wire.hxx>

#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <Geom2d_Curve.hxx>

#include <TColgp_SequenceOfPnt2d.hxx>

static Standard_Real ComputeMaxTolOfFace(const TopoDS_Face& theFace)
{
  Standard_Real MaxTol = BRep_Tool::Tolerance(theFace);

  TopTools_IndexedMapOfShape aMap;
  TopExp::MapShapes(theFace, TopAbs_EDGE, aMap);
  for (Standard_Integer i = 1; i <= aMap.Extent(); i++)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(aMap(i));
    Standard_Real aTol = BRep_Tool::Tolerance(anEdge);
    if (aTol > MaxTol)
      MaxTol = aTol;
  }

  aMap.Clear();
  TopExp::MapShapes(theFace, TopAbs_VERTEX, aMap);
  for (Standard_Integer i = 1; i <= aMap.Extent(); i++)
  {
    const TopoDS_Vertex& aVertex = TopoDS::Vertex(aMap(i));
    Standard_Real aTol = BRep_Tool::Tolerance(aVertex);
    if (aTol > MaxTol)
      MaxTol = aTol;
  }

  return MaxTol;
}

//=======================================================================
//function : FixResult
//purpose  : auxiliary
//=======================================================================
static void FixResult(const TopoDS_Shape& result,
                      Handle(ShapeBuild_ReShape)& Context,
                      const Standard_Real Tol)
{
  for (TopExp_Explorer ex_f(result,TopAbs_FACE); ex_f.More(); ex_f.Next()) {
    TopoDS_Shape aShape = Context->Apply(ex_f.Current().Oriented(TopAbs_FORWARD));
    // face could not be dropped or split on this step
    TopoDS_Face aFace = TopoDS::Face(aShape);
    TopLoc_Location L;
    Handle(Geom_Surface) Surf = BRep_Tool::Surface(aFace,L);

    if( Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) ||
        Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ) {

      Standard_Integer nbWires = 0;
      for (TopExp_Explorer ex_w(aFace,TopAbs_WIRE); ex_w.More(); ex_w.Next()) {
        nbWires++;
        Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire(TopoDS::Wire(ex_w.Current()),
                                                      aFace,
                                                      Precision::Confusion());
        sfw->FixReorder();
        if(sfw->StatusReorder ( ShapeExtend_FAIL ))
          continue;

        sfw->SetPrecision(2.*Tol);
        sfw->FixShifted();

        Standard_Boolean isDone = sfw->LastFixStatus ( ShapeExtend_DONE );
        isDone |= sfw->FixDegenerated();

        // remove degenerated edges from not degenerated points
        ShapeAnalysis_Edge sae;
        Handle(ShapeExtend_WireData) sewd = sfw->WireData();
        Standard_Integer i;
        for( i = 1; i<=sewd->NbEdges();i++) {
          TopoDS_Edge E = sewd->Edge(i);
          if(BRep_Tool::Degenerated(E)&&!sae.HasPCurve(E,aFace)) {
            sewd->Remove(i);
            isDone = Standard_True;
            i--;
          }
        }

        //isDone |= sfw->FixLacking(); // commented by skl 22.03.2005 (PAL8395)

        // remove neighbour seam edges
        if(isDone) {
          for( i = 1; i<sewd->NbEdges();i++) {
            if(sewd->IsSeam(i) && sewd->IsSeam(i+1)) {
              isDone = Standard_True;
              sewd->Remove(i);
              sewd->Remove(i);
              i--;
            }
          }
          if(sewd->IsSeam(1) && sewd->IsSeam(sewd->NbEdges())) {
            sewd->Remove(1);
            sewd->Remove(sewd->NbEdges());
          }
        }

        if(isDone) {
          TopoDS_Wire ResWire = sfw->Wire();
          Context->Replace(ex_w.Current(), ResWire);
        };
      }
      // Implement fix orientation in case of several wires
      if(nbWires > 1) {
        TopoDS_Face aFixedFace = TopoDS::Face(Context->Apply(aFace));
        Handle(ShapeFix_Face) sff = new ShapeFix_Face(aFixedFace);
        if(sff->FixOrientation())
          Context->Replace(aFixedFace,sff->Face());
      }
    }
  }
}

//=======================================================================
//function : RotateSphereSpace
//purpose  :
//=======================================================================
TopoDS_Shape BlockFix::RotateSphereSpace (const TopoDS_Shape& S,
                                          const Standard_Real Tol)
{
  // Create a modification description
  Handle(BlockFix_SphereSpaceModifier) SR = new BlockFix_SphereSpaceModifier;
  SR->SetTolerance(Tol);

  TopTools_DataMapOfShapeShape context;
  BRepTools_Modifier MD;
  TopoDS_Shape result = ShapeCustom::ApplyModifier ( S, SR, context,MD );

  Handle(ShapeBuild_ReShape) RS = new ShapeBuild_ReShape;
  FixResult(result,RS,Tol);
  result = RS->Apply(result);

  ShapeFix_Edge sfe;
  for(TopExp_Explorer exp(result,TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Edge E = TopoDS::Edge(exp.Current());
    sfe.FixVertexTolerance (E);
  }

  ShapeFix::SameParameter(result,Standard_False);
  return result;
}

//=======================================================================
//function : RefillProblemFaces
//purpose  :
//=======================================================================
TopoDS_Shape BlockFix::RefillProblemFaces (const TopoDS_Shape& aShape)
{
  Standard_Integer NbSamples = 10;

  TopTools_ListOfShape theFaces;

  TopExp_Explorer Explo(aShape, TopAbs_FACE);
  for (; Explo.More(); Explo.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(Explo.Current());
    BRepAdaptor_Surface BAsurf(aFace);
    GeomAbs_SurfaceType SurfType = BAsurf.GetType();
    if (SurfType >= GeomAbs_BezierSurface)
    {
      TopExp_Explorer fexp(aFace, TopAbs_EDGE);
      for (; fexp.More(); fexp.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge(fexp.Current());
        if (BRep_Tool::Degenerated(anEdge))
        {
          TopoDS_Vertex V1, V2;
          TopExp::Vertices(anEdge, V1, V2);
          if (V1.IsSame(V2))
          {
            gp_Pnt aPnt = BRep_Tool::Pnt(V1);
            Standard_Real TolV = BRep_Tool::Tolerance(V1);
            Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
            Handle(ShapeAnalysis_Surface) Analyser = new ShapeAnalysis_Surface(aSurf);
            if (Analyser->IsDegenerated(aPnt, TolV))
            {
              theFaces.Append(aFace);
              break;
            }
          }
        }
      }
    }
  }

  //Now all problem faces are collected in the list "theFaces"
  BRepTools_Substitution aSubst;
  TopTools_ListIteratorOfListOfShape itl(theFaces);
  for (; itl.More(); itl.Next())
  {
    TopoDS_Face aFace = TopoDS::Face(itl.Value());
    aFace.Orientation(TopAbs_FORWARD);
    Standard_Real MaxTolOfFace = ComputeMaxTolOfFace(aFace);
    BRepAdaptor_Surface BAsurf(aFace, Standard_False);
    BRepOffsetAPI_MakeFilling Filler(3, 10);
    TopExp_Explorer Explo(aFace, TopAbs_EDGE);
    for (; Explo.More(); Explo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
      if (BRep_Tool::Degenerated(anEdge) ||
          BRepTools::IsReallyClosed(anEdge, aFace))
        continue;
      
      Filler.Add(anEdge, GeomAbs_C0);
    }
    Standard_Real Umin, Umax, Vmin, Vmax;
    BRepTools::UVBounds(aFace, Umin, Umax, Vmin, Vmax);
    Standard_Real DeltaU = (Umax - Umin)/NbSamples,
      DeltaV = (Vmax - Vmin)/NbSamples;
    for (Standard_Integer i = 1; i < NbSamples; i++)
      for (Standard_Integer j = 1; j < NbSamples; j++) {
        Filler.Add(Umin + i*DeltaU, Vmin + j*DeltaV, aFace, GeomAbs_G1);
      }

    Filler.Build();
    if (Filler.IsDone())
    {
      TopoDS_Face aNewFace = TopoDS::Face(Filler.Shape());
      aNewFace.Orientation(TopAbs_FORWARD);
      Handle(Geom_Surface) aNewSurf = BRep_Tool::Surface(aNewFace);
      GeomAdaptor_Surface GAnewsurf(aNewSurf);
      Extrema_ExtPS Projector;
      Projector.Initialize(GAnewsurf, GAnewsurf.FirstUParameter(), GAnewsurf.LastUParameter(),
                           GAnewsurf.FirstVParameter(), GAnewsurf.LastVParameter(),
                           Precision::Confusion(), Precision::Confusion());
      Standard_Real MaxSqDist = 0.;
      for (Standard_Integer i = 0; i < NbSamples; i++)
        for (Standard_Integer j = 0; j < NbSamples; j++)
        {
          gp_Pnt aPoint = BAsurf.Value(Umin + DeltaU/2 + i*DeltaU,
                                       Vmin + DeltaV/2 + j*DeltaV);
          Projector.Perform(aPoint);
          if (Projector.IsDone())
          {
            Standard_Real LocalMinSqDist = RealLast();
            for (Standard_Integer ind = 1; ind <= Projector.NbExt(); ind++)
            {
              Standard_Real aSqDist = Projector.SquareDistance(ind);
              if (aSqDist < LocalMinSqDist)
                LocalMinSqDist = aSqDist;
            }
            if (!Precision::IsInfinite(LocalMinSqDist) &&
                LocalMinSqDist > MaxSqDist)
              MaxSqDist = LocalMinSqDist;
          }
        }
      Standard_Real MaxDist = Sqrt(MaxSqDist);
      if (MaxDist < Max(1.e-4, 1.5*MaxTolOfFace))
      {
        TopTools_IndexedMapOfShape Emap;
        TopExp::MapShapes(aFace, TopAbs_EDGE, Emap);
        for (Standard_Integer i = 1; i <= Emap.Extent(); i++)
        {
          TopoDS_Edge anEdge = TopoDS::Edge(Emap(i));
          anEdge.Orientation(TopAbs_FORWARD);
          TopTools_ListOfShape Ledge;
          if (!BRep_Tool::Degenerated(anEdge) &&
              !BRepTools::IsReallyClosed(anEdge, aFace))
          {
            const TopTools_ListOfShape& Ledges = Filler.Generated(anEdge);
            if (!Ledges.IsEmpty()) {
              TopoDS_Edge NewEdge = TopoDS::Edge(Ledges.First());
              Ledge.Append(NewEdge.Oriented(TopAbs_FORWARD));
            }
          }
          aSubst.Substitute(anEdge, Ledge);
        }
        TopTools_ListOfShape Lface;
        BRepAdaptor_Surface NewBAsurf(aNewFace);
        gp_Pnt MidPnt;
        gp_Vec D1U, D1V, Normal, NewNormal;
        Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
        aSurf->D1((Umin+Umax)*0.5, (Vmin+Vmax)*0.5, MidPnt, D1U, D1V);
        Normal = D1U ^ D1V;
        NewBAsurf.D1((NewBAsurf.FirstUParameter() + NewBAsurf.LastUParameter())*0.5,
                     (NewBAsurf.FirstVParameter() + NewBAsurf.LastVParameter())*0.5,
                     MidPnt, D1U, D1V);
        NewNormal = D1U ^ D1V;
        if (Normal * NewNormal < 0.)
          aNewFace.Reverse();
        Lface.Append(aNewFace);
        aSubst.Substitute(aFace, Lface);
      }
    }
  }
  aSubst.Build(aShape);

  TopoDS_Shape Result = aShape;
  if (aSubst.IsCopied(aShape))
    Result = aSubst.Copy(aShape).First();

  BRepTools::RemoveUnusedPCurves(Result);

  return Result;
}

//=======================================================================
//function : FixRanges
//purpose  :
//=======================================================================
TopoDS_Shape BlockFix::FixRanges (const TopoDS_Shape& S,
                                  const Standard_Real Tol)
{
  // Create a modification description
  Handle(BlockFix_PeriodicSurfaceModifier) SR = new BlockFix_PeriodicSurfaceModifier;
  SR->SetTolerance(Tol);

  TopTools_DataMapOfShapeShape context;
  BRepTools_Modifier MD;
  TopoDS_Shape result = ShapeCustom::ApplyModifier ( S, SR, context,MD );

  Handle(ShapeBuild_ReShape) RS = new ShapeBuild_ReShape;
  FixResult(result,RS,Tol);
  result = RS->Apply(result);

  ShapeFix_Edge sfe;
  for(TopExp_Explorer exp(result,TopAbs_EDGE); exp.More(); exp.Next()) {
    TopoDS_Edge E = TopoDS::Edge(exp.Current());
    sfe.FixVertexTolerance (E);
  }

  ShapeFix::SameParameter(result,Standard_False);

  return result;
}
