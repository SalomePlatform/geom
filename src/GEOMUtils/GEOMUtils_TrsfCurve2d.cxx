// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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


#include <GEOMUtils_TrsfCurve2d.hxx>
#include <GEOMUtils_HTrsfCurve2d.hxx>

//=======================================================================
//function : TrsfCurve2d
//purpose  :
//=======================================================================
GEOMUtils::TrsfCurve2d::TrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                    const Trsf2d               &theTrsf)
: myCurve (theCurve),
  myTrsf  (theTrsf)
{
}

//=======================================================================
//function : TrsfCurve2d
//purpose  :
//=======================================================================
GEOMUtils::TrsfCurve2d::TrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                    const Standard_Real         theUFirst,
                                    const Standard_Real         theULast,
                                    const Trsf2d               &theTrsf)
: myCurve (theCurve, theUFirst, theULast),
  myTrsf  (theTrsf)
{
}

//=======================================================================
//function : FirstParameter
//purpose  :
//=======================================================================
Standard_Real GEOMUtils::TrsfCurve2d::FirstParameter() const
{
  return myCurve.FirstParameter();
}

//=======================================================================
//function : LastParameter
//purpose  :
//=======================================================================
Standard_Real GEOMUtils::TrsfCurve2d::LastParameter() const
{
  return myCurve.LastParameter();
}

//=======================================================================
//function : Curve
//purpose  :
//=======================================================================
const Handle(Geom2d_Curve) &GEOMUtils::TrsfCurve2d::Curve() const
{
  return myCurve.Curve();
}

//=======================================================================
//function : GetType
//purpose  :
//=======================================================================
GeomAbs_CurveType GEOMUtils::TrsfCurve2d::GetType() const
{
  return GeomAbs_OtherCurve;
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::Load(const Handle(Geom2d_Curve) &C)
{
  myCurve.Load(C);
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::Load(const Handle(Geom2d_Curve) &C,
                                  const Standard_Real         UFirst,
                                  const Standard_Real         ULast)
{
  myCurve.Load(C, UFirst, ULast);
}

//=======================================================================
//function : Continuity
//purpose  :
//=======================================================================
GeomAbs_Shape GEOMUtils::TrsfCurve2d::Continuity() const
{
  return myCurve.Continuity();
}

//=======================================================================
//function : NbIntervals
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::TrsfCurve2d::NbIntervals
                      (const GeomAbs_Shape S) const
{
  return myCurve.NbIntervals(S);
}

//=======================================================================
//function : Intervals
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::Intervals(TColStd_Array1OfReal &T,
                                       const GeomAbs_Shape   S) const
{
  myCurve.Intervals(T, S);
}

//=======================================================================
//function : Trim
//purpose  :
//=======================================================================
Handle(Adaptor2d_HCurve2d) GEOMUtils::TrsfCurve2d::Trim
              (const Standard_Real First, const Standard_Real Last,
               const Standard_Real /*Tol*/) const
{
  Handle(Geom2d_Curve)            aCurve = myCurve.Curve();
#if OCC_VERSION_MAJOR < 7
  GEOMUtils::Handle(HTrsfCurve2d) aAHCurve =
#else
  Handle(GEOMUtils::HTrsfCurve2d) aAHCurve =
#endif
    new GEOMUtils::HTrsfCurve2d(aCurve, First, Last, myTrsf);

  return aAHCurve;
}

//=======================================================================
//function : IsClosed
//purpose  :
//=======================================================================
Standard_Boolean GEOMUtils::TrsfCurve2d::IsClosed() const
{
  return myCurve.IsClosed();
}

//=======================================================================
//function : IsPeriodic
//purpose  :
//=======================================================================
Standard_Boolean GEOMUtils::TrsfCurve2d::IsPeriodic() const
{
  return myCurve.IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  :
//=======================================================================
Standard_Real GEOMUtils::TrsfCurve2d::Period() const
{
  return myCurve.Period();
}

//=======================================================================
//function : Value
//purpose  :
//=======================================================================
gp_Pnt2d GEOMUtils::TrsfCurve2d::Value(const Standard_Real U) const
{
  gp_Pnt2d aPnt = myCurve.Value(U);

  myTrsf.TransformD0(aPnt);

  return aPnt;
}

//=======================================================================
//function : D0
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::D0(const Standard_Real U, gp_Pnt2d &P) const
{
  myCurve.D0(U, P);
  myTrsf.TransformD0(P);
}

//=======================================================================
//function : D1
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::D1(const Standard_Real U,
                                gp_Pnt2d &P, gp_Vec2d &V) const
{
  myCurve.D1(U, P, V);
  myTrsf.TransformD1(P, V);
}

//=======================================================================
//function : D2
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::D2(const Standard_Real U, gp_Pnt2d &P,
                                gp_Vec2d &V1, gp_Vec2d &V2) const
{
  myCurve.D2(U, P, V1, V2);
  myTrsf.TransformD2(P, V1, V2);
}

//=======================================================================
//function : D3
//purpose  :
//=======================================================================
void GEOMUtils::TrsfCurve2d::D3(const Standard_Real U, gp_Pnt2d &P,
                                gp_Vec2d &V1, gp_Vec2d &V2, gp_Vec2d &V3) const
{
  myCurve.D3(U, P, V1, V2, V3);
  myTrsf.TransformD3(P, V1, V2, V3);
}

//=======================================================================
//function : DN
//purpose  :
//=======================================================================
gp_Vec2d GEOMUtils::TrsfCurve2d::DN(const Standard_Real    U,
                                    const Standard_Integer N) const
{
  gp_Pnt2d aPnt = myCurve.Value(U);
  gp_Vec2d aVec = myCurve.DN(U, N);

  myTrsf.TransformD1(aPnt, aVec);
  return aVec;
}

//=======================================================================
//function : Resolution
//purpose  :
//=======================================================================
Standard_Real GEOMUtils::TrsfCurve2d::Resolution(const Standard_Real Ruv) const
{
  return Precision::Parametric(Ruv);
}

//=======================================================================
//function : Line
//purpose  :
//=======================================================================
gp_Lin2d GEOMUtils::TrsfCurve2d::Line() const
{
  Standard_NoSuchObject::Raise();

  return gp_Lin2d();
}

//=======================================================================
//function : Circle
//purpose  :
//=======================================================================
gp_Circ2d  GEOMUtils::TrsfCurve2d::Circle() const
{
  Standard_NoSuchObject::Raise();

  return gp_Circ2d();
}

//=======================================================================
//function : Ellipse
//purpose  :
//=======================================================================
gp_Elips2d GEOMUtils::TrsfCurve2d::Ellipse() const
{
  Standard_NoSuchObject::Raise();

  return gp_Elips2d();
}

//=======================================================================
//function : Hyperbola
//purpose  :
//=======================================================================
gp_Hypr2d GEOMUtils::TrsfCurve2d::Hyperbola() const
{
  Standard_NoSuchObject::Raise();

  return gp_Hypr2d();
}

//=======================================================================
//function : Parabola
//purpose  :
//=======================================================================
gp_Parab2d GEOMUtils::TrsfCurve2d::Parabola() const
{
  Standard_NoSuchObject::Raise();

  return gp_Parab2d();
}

//=======================================================================
//function : Degree
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::TrsfCurve2d::Degree() const
{
  Standard_NoSuchObject::Raise();

  return 0;
}

//=======================================================================
//function : IsRational
//purpose  :
//=======================================================================
Standard_Boolean GEOMUtils::TrsfCurve2d::IsRational() const
{
  return Standard_False;
}

//=======================================================================
//function : NbPoles
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::TrsfCurve2d::NbPoles() const
{
  Standard_NoSuchObject::Raise();

  return 0;
}

//=======================================================================
//function : NbKnots
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::TrsfCurve2d::NbKnots() const
{
  Standard_NoSuchObject::Raise();

  return 0;
}

//=======================================================================
//function : Bezier
//purpose  :
//=======================================================================
Handle(Geom2d_BezierCurve) GEOMUtils::TrsfCurve2d::Bezier() const
{
  Standard_NoSuchObject::Raise();

  return NULL;
}

//=======================================================================
//function : BSpline
//purpose  :
//=======================================================================
Handle(Geom2d_BSplineCurve) GEOMUtils::TrsfCurve2d::BSpline() const
{
  Standard_NoSuchObject::Raise();

  return NULL;
}

//=======================================================================
//function : NbSamples
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::TrsfCurve2d::NbSamples() const
{
  return myCurve.NbSamples();
}
