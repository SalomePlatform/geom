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

#ifndef _GEOMUtils_TrsfCurve2d_HXX_
#define _GEOMUtils_TrsfCurve2d_HXX_

#include <GEOMUtils_Trsf2d.hxx>

#include <Basics_OCCTVersion.hxx>

#include <Geom2dHatch_Hatcher.hxx>
#include <GeomAbs_IsoType.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TopoDS_Face.hxx>


namespace GEOMUtils
{
  /*!
   *  This class represents an adaptor curve that represents an original curve
   *  transformed by an anisotropic transformation.
   */
  class TrsfCurve2d : public Adaptor2d_Curve2d
  {

  public:
    
    /**
     * Constructor. Initializes the object with the transformation parameters.
     * Input parameters are not checked for validity. It is under responsibility
     * of the caller.
     */
    Standard_EXPORT TrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                const Trsf2d               &theTrsf);
    
    /**
     * Constructor. Initializes the object with the curve, first and last
     * parameters and transformation. Input parameters are not checked
     * for validity. It is under responsibility of the caller.
     */
    Standard_EXPORT TrsfCurve2d(const Handle(Geom2d_Curve) &theCurve,
                                const Standard_Real         theUFirst,
                                const Standard_Real         theULast,
                                const Trsf2d               &theTrsf);

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Real FirstParameter() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Real LastParameter() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT const Handle(Geom2d_Curve)& Curve() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT GeomAbs_CurveType GetType() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void Load(const Handle(Geom2d_Curve) &C);

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void Load(const Handle(Geom2d_Curve) &C,
                              const Standard_Real         UFirst,
                              const Standard_Real         ULast);

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT GeomAbs_Shape Continuity() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Integer NbIntervals(const GeomAbs_Shape S) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void Intervals(TColStd_Array1OfReal &T,
                                   const GeomAbs_Shape   S) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Handle(Adaptor2d_HCurve2d) Trim
              (const Standard_Real First, const Standard_Real Last,
               const Standard_Real ) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Boolean IsClosed() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Boolean IsPeriodic() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Real Period() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Pnt2d Value(const Standard_Real U) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void D0(const Standard_Real U, gp_Pnt2d &P) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void D1(const Standard_Real U,
                            gp_Pnt2d &P, gp_Vec2d &V) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void D2(const Standard_Real U, gp_Pnt2d &P,
                            gp_Vec2d &V1, gp_Vec2d &V2) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT void D3(const Standard_Real U, gp_Pnt2d &P,
                            gp_Vec2d &V1, gp_Vec2d &V2, gp_Vec2d &V3) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Vec2d DN(const Standard_Real    U,
                                const Standard_Integer N) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Real Resolution(const Standard_Real Ruv) const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Lin2d Line() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Circ2d  Circle() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Elips2d Ellipse() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Hypr2d Hyperbola() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT gp_Parab2d Parabola() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Integer Degree() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Boolean IsRational() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Integer NbPoles() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Integer NbKnots() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Handle(Geom2d_BezierCurve) Bezier() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Handle(Geom2d_BSplineCurve) BSpline() const;

    /**
     * Redefined method from the base class.
     */
    Standard_EXPORT Standard_Integer NbSamples() const;

  private:

    Geom2dAdaptor_Curve myCurve;
    Trsf2d              myTrsf;

  };
}

#endif
