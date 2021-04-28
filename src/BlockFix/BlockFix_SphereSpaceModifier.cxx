// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <BlockFix_SphereSpaceModifier.hxx>

#include <ShapeAnalysis.hxx>

#include <ShapeFix_Edge.hxx>

#include <TopExp.hxx>

#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>

#include <ElSLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <Geom2d_Curve.hxx>

#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BlockFix_SphereSpaceModifier, BRepTools_Modification)

//=======================================================================
//function : BlockFix_SphereSpaceModifier
//purpose  :
//=======================================================================
BlockFix_SphereSpaceModifier::BlockFix_SphereSpaceModifier()
{
  myMapOfFaces.Clear();
  myMapOfSpheres.Clear();
}

//=======================================================================
//function : ~BlockFix_SphereSpaceModifier
//purpose  :
//=======================================================================
BlockFix_SphereSpaceModifier::~BlockFix_SphereSpaceModifier() {}

//=======================================================================
//function : SetTolerance
//purpose  :
//=======================================================================
void BlockFix_SphereSpaceModifier::SetTolerance(const Standard_Real Tol)
{
  myTolerance = Tol;
}

//=======================================================================
//function : NewSurface
//purpose  :
//=======================================================================
static Standard_Boolean ModifySurface(const TopoDS_Face&          theFace,
                                      const Handle(Geom_Surface)& theSurface,
                                      Handle(Geom_Surface)&       theNewSurface)
{
  TopoDS_Face aFace = theFace;
  aFace.Orientation (TopAbs_FORWARD);

  Handle(Geom_Surface) aNewSurface;
  
  Handle(Geom_Surface) aSurf = theSurface;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) RTS =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
    aSurf = RTS->BasisSurface();
  }

  if (aSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    Standard_Real Umin, Umax, Vmin, Vmax;
    ShapeAnalysis::GetFaceUVBounds (aFace, Umin, Umax, Vmin, Vmax);
    Standard_Real PI2 = M_PI/2.;
    Handle(Geom_SphericalSurface) aSphere = Handle(Geom_SphericalSurface)::DownCast(aSurf);
    gp_Sphere sp = aSphere->Sphere();
    Standard_Real Radius = sp.Radius();
    gp_Ax3 ax3 = sp.Position();
    gp_Pnt aCentre = sp.Location();
    
    TopoDS_Wire aWire = BRepTools::OuterWire (aFace);
    BRepTopAdaptor_FClass2d aClassifier (aFace, Precision::PConfusion());
    TopTools_MapOfShape aEmap;
    const Standard_Real anOffsetValue = 0.01*M_PI;
    for (Standard_Integer ii = 1; ii <= 2; ii++)
    {
      TopoDS_Iterator itw (aWire);
      for (; itw.More(); itw.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge (itw.Value());
        if (aEmap.Contains (anEdge) ||
            anEdge.Orientation() == TopAbs_INTERNAL ||
            anEdge.Orientation() == TopAbs_EXTERNAL ||
            BRep_Tool::Degenerated (anEdge) ||
            BRepTools::IsReallyClosed (anEdge, aFace))
          continue;
        
        BRepAdaptor_Curve2d aBAcurve2d (anEdge, aFace);
        GeomAbs_CurveType aType = aBAcurve2d.GetType();
        if (ii == 1 && aType == GeomAbs_Line) //first pass: consider only curvilinear edges
          continue;
        
        Standard_Real aMidPar = (aBAcurve2d.FirstParameter() + aBAcurve2d.LastParameter())/2;
        gp_Pnt2d aMidP2d;
        gp_Vec2d aTangent;
        aBAcurve2d.D1 (aMidPar, aMidP2d, aTangent);
        if (anEdge.Orientation() == TopAbs_REVERSED)
          aTangent.Reverse();
        
        aTangent.Normalize();
        gp_Vec2d aNormal (aTangent.Y(), -aTangent.X());
        aNormal *= anOffsetValue;
        gp_Pnt2d anUpperPole = aMidP2d.Translated (aNormal);
        if (anUpperPole.Y() < -PI2 || anUpperPole.Y() > PI2)
        {
          aEmap.Add(anEdge);
          continue;
        }
        if (anUpperPole.X() < 0.)
          anUpperPole.SetX (anUpperPole.X() + 2.*M_PI);
        else if (anUpperPole.X() > 2.*M_PI)
          anUpperPole.SetX (anUpperPole.X() - 2.*M_PI);
        
        TopAbs_State aStatus = aClassifier.Perform (anUpperPole);
        if (aStatus != TopAbs_OUT)
        {
          aEmap.Add(anEdge);
          continue;
        }
        
        gp_Pnt anUpperPole3d = aSphere->Value (anUpperPole.X(), anUpperPole.Y());
        gp_Vec aVec (aCentre, anUpperPole3d);
        aVec.Reverse();
        gp_Pnt aLowerPole3d = aCentre.Translated (aVec);
        Standard_Real aU, aV;
        ElSLib::Parameters (sp, aLowerPole3d, aU, aV);
        gp_Pnt2d aLowerPole (aU, aV);
        aStatus = aClassifier.Perform (aLowerPole);
        if (aStatus != TopAbs_OUT)
        {
          aEmap.Add(anEdge);
          continue;
        }
        
        //Build a meridian
        gp_Vec anUp (aCentre, anUpperPole3d);
        anUp.Normalize();
        gp_Pnt aMidPnt = aSphere->Value (aMidP2d.X(), aMidP2d.Y());
        gp_Vec aMidOnEdge (aCentre, aMidPnt);
        aMidOnEdge.Normalize();
        gp_Vec AxisOfCircle = anUp ^ aMidOnEdge;
        gp_Vec XDirOfCircle = anUp ^ AxisOfCircle;
        gp_Ax2 anAxis (aCentre, AxisOfCircle, XDirOfCircle);
        Handle(Geom_Circle) aCircle = new Geom_Circle (anAxis, Radius);
        Handle(Geom_TrimmedCurve) aMeridian = new Geom_TrimmedCurve (aCircle, -PI2, PI2);
        
        //Check the meridian
        Standard_Boolean IsInnerPointFound = Standard_False;
        Standard_Integer NbSamples = 10;
        Standard_Real aDelta = M_PI / NbSamples;
        for (Standard_Integer jj = 1; jj < NbSamples; jj++)
        {
          Standard_Real aParam = -PI2 + jj*aDelta;
          gp_Pnt aPnt = aMeridian->Value (aParam);
          ElSLib::Parameters (sp, aPnt, aU, aV);
          gp_Pnt2d aP2d (aU, aV);
          aStatus = aClassifier.Perform (aP2d);
          if (aStatus != TopAbs_OUT)
          {
            IsInnerPointFound = Standard_True;
            break;
          }
        }
        if (IsInnerPointFound)
        {
          aEmap.Add(anEdge);
          continue;
        }
        
        gp_Ax3 anAxisOfNewSphere (aCentre, anUp, XDirOfCircle);
        aNewSurface = new Geom_SphericalSurface (anAxisOfNewSphere, Radius);
        break;
      } //for (; itw.More(); itw.Next()) (iteration on outer wire)
      if (!aNewSurface.IsNull())
        break;
    } //for (Standard_Integer ii = 1; ii <= 2; ii++) (two passes)
  }

  if (aNewSurface.IsNull())
    return Standard_False;

  theNewSurface = aNewSurface;
  return Standard_True;
}

Standard_Boolean BlockFix_SphereSpaceModifier::NewSurface(const TopoDS_Face& F,
                                                        Handle(Geom_Surface)& S,
                                                        TopLoc_Location& L,Standard_Real& Tol,
                                                        Standard_Boolean& RevWires,
                                                        Standard_Boolean& RevFace)
{
  TopLoc_Location LS;
  Handle(Geom_Surface) SIni = BRep_Tool::Surface(F, LS);

  //check if pole of the sphere in the parametric space
  if(ModifySurface(F, SIni, S)) {

    RevWires = Standard_False;
    RevFace = Standard_False;

    L = LS;
    Tol = BRep_Tool::Tolerance(F);

    Standard_Integer anIndex = myMapOfSpheres.Add(S);
    myMapOfFaces.Bind(F,anIndex);
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : NewCurve
//purpose  :
//=======================================================================
Standard_Boolean BlockFix_SphereSpaceModifier::NewCurve(const TopoDS_Edge& /*E*/,Handle(Geom_Curve)& /*C*/,
                                                        TopLoc_Location& /*L*/,Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : NewPoint
//purpose  :
//=======================================================================
Standard_Boolean BlockFix_SphereSpaceModifier::NewPoint(const TopoDS_Vertex& /*V*/,
                                                      gp_Pnt& /*P*/,
                                                      Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : NewCurve2d
//purpose  :
//=======================================================================
Standard_Boolean BlockFix_SphereSpaceModifier::NewCurve2d(const TopoDS_Edge& E,const TopoDS_Face& F,
                                                        const TopoDS_Edge& /*NewE*/,const TopoDS_Face& /*NewF*/,
                                                        Handle(Geom2d_Curve)& C,Standard_Real& Tol)
{
  //check if undelying surface of the face was modified
  if(myMapOfFaces.IsBound(F)) {
    Standard_Integer anIndex = myMapOfFaces.Find(F);

    Handle(Geom_Surface) aNewSphere = Handle(Geom_Surface)::DownCast(myMapOfSpheres.FindKey(anIndex));

    Standard_Real f,l;
    TopLoc_Location LC, LS;
    Handle(Geom_Curve) C3d = BRep_Tool::Curve ( E, LC, f, l );
    Handle(Geom_Surface) S = BRep_Tool::Surface(F, LS);

    //taking into account the orientation of the seam
    C = BRep_Tool::CurveOnSurface(E,F,f,l);
    Tol = BRep_Tool::Tolerance(E);

    BRep_Builder B;
    TopoDS_Edge TempE;
    B.MakeEdge(TempE);
    B.Add(TempE, TopExp::FirstVertex(E));
    B.Add(TempE, TopExp::LastVertex(E));

    if(!C3d.IsNull())
      B.UpdateEdge(TempE, Handle(Geom_Curve)::DownCast(C3d->Transformed(LC.Transformation())), Precision::Confusion());
    B.Range(TempE, f, l);

    Handle(ShapeFix_Edge) sfe = new ShapeFix_Edge;
    Handle(Geom_Surface) STemp = Handle(Geom_Surface)::DownCast(aNewSphere->Transformed(LS.Transformation()));
    TopLoc_Location LTemp;
    LTemp.Identity();

    Standard_Boolean isClosed = BRep_Tool::IsClosed (E, F);
    Standard_Real aWorkTol = 2*myTolerance+Tol;
    sfe->FixAddPCurve(TempE, STemp, LTemp, isClosed, Max(Precision::Confusion(), aWorkTol));
    sfe->FixSameParameter(TempE);

    //keep the orientation of original edge
    TempE.Orientation(E.Orientation());
    C = BRep_Tool::CurveOnSurface(TempE, STemp, LTemp, f, l);

    // shifting seam of sphere
    if(isClosed  && !C.IsNull()) {
      Standard_Real f2,l2;
      Handle(Geom2d_Curve) c22 =
        BRep_Tool::CurveOnSurface(TopoDS::Edge(TempE.Reversed()),STemp, LTemp,f2,l2);
      Standard_Real dPreci = Precision::PConfusion()*Precision::PConfusion();
      if((C->Value(f).SquareDistance(c22->Value(f2)) < dPreci)
         ||(C->Value(l).SquareDistance(c22->Value(l2)) < dPreci)) {
        gp_Vec2d shift(S->UPeriod(),0.);
        C->Translate(shift);
      }
    }
    //sphere was modified
    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : NewParameter
//purpose  :
//=======================================================================
Standard_Boolean BlockFix_SphereSpaceModifier::NewParameter(const TopoDS_Vertex& /*V*/,const TopoDS_Edge& /*E*/,
                                                            Standard_Real& /*P*/,Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : Continuity
//purpose  :
//=======================================================================
GeomAbs_Shape BlockFix_SphereSpaceModifier::Continuity(const TopoDS_Edge& E,const TopoDS_Face& F1,
                                                     const TopoDS_Face& F2,const TopoDS_Edge& /*NewE*/,
                                                     const TopoDS_Face& /*NewF1*/,const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E,F1,F2);
}
