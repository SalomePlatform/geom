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

#ifndef _GEOMImpl_IMeasureOperations_HXX_
#define _GEOMImpl_IMeasureOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Status.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>

class GEOM_Engine;
class GEOM_Object;

class GEOMImpl_IMeasureOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IMeasureOperations(GEOM_Engine* theEngine);
  Standard_EXPORT ~GEOMImpl_IMeasureOperations();

  enum ShapeKind {
    SK_NO_SHAPE,
    // COMPOSITEs
    SK_COMPOUND,
    SK_COMPSOLID,
    SK_SHELL,
    SK_WIRE,
    // SOLIDs
    SK_SPHERE,       // full sphere
    SK_CYLINDER,     // cylinder
    SK_BOX,          // box with faces, parallel to global coordinate planes
    SK_ROTATED_BOX,  // other box
    SK_TORUS,        // full torus
    SK_CONE,         // cone
    SK_POLYHEDRON,   // solid, bounded by polygons
    SK_SOLID,        // other solid
    // FACEs
    SK_SPHERE2D,     // spherical face (closed)
    SK_CYLINDER2D,   // cylindrical face with defined height
    SK_TORUS2D,      // toroidal face (closed)
    SK_CONE2D,       // conical face with defined height
    SK_DISK_CIRCLE,  // planar, bounded by circle
    SK_DISK_ELLIPSE, // planar, bounded by ellipse
    SK_POLYGON,      // planar, bounded by segments
    SK_PLANE,        // infinite planar
    SK_PLANAR,       // other planar
    SK_FACE,         // other face
    // EDGEs
    SK_CIRCLE,       // full circle
    SK_ARC_CIRCLE,   // arc of circle
    SK_ELLIPSE,      // full ellipse
    SK_ARC_ELLIPSE,  // arc of ellipse
    SK_LINE,         // infinite segment
    SK_SEGMENT,      // segment
    SK_CRV_BSPLINE,  // B-Spline curve (open or closed)
    SK_CRV_BEZIER,   // Bezier curve (open or closed)
    SK_HYPERBOLA,    // hyperbola
    SK_PARABOLA,     // parabola
    SK_EDGE,         // other edge
    // VERTEX
    SK_VERTEX,       // vertex
    // ADVANCED shapes
    SK_LCS,          // local coordinate system
    // (other advanced shapes)
    SK_ADVANCED,     // all advanced shapes (temporary implementation)
  };

  /**
   * This enumeration represents the level of checking shape on
   * self-interference. It defines which interferferences will be checked.
   */
  enum SICheckLevel
  {
    SI_V_V = 0, // only V/V interferences
    SI_V_E,     // V/V and V/E interferences
    SI_E_E,     // V/V, V/E and E/E interferences
    SI_V_F,     // V/V, V/E, E/E and V/F interferences
    SI_E_F,     // V/V, V/E, E/E, V/F and E/F interferences
    SI_ALL      // all interferences
  };

  Standard_EXPORT ShapeKind KindOfShape (Handle(GEOM_Object) theShape,
                                         Handle(TColStd_HSequenceOfInteger)& theIntegers,
                                         Handle(TColStd_HSequenceOfReal)&    theDoubles);

  Standard_EXPORT void GetPosition (Handle(GEOM_Object) theShape,
                                    Standard_Real& Ox, Standard_Real& Oy, Standard_Real& Oz,
                                    Standard_Real& Zx, Standard_Real& Zy, Standard_Real& Zz,
                                    Standard_Real& Xx, Standard_Real& Xy, Standard_Real& Xz);

  Standard_EXPORT Handle(GEOM_Object) GetCentreOfMass (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(GEOM_Object) GetVertexByIndex (Handle(GEOM_Object) theShape,
                                                        Standard_Integer theIndex,
                                                        Standard_Boolean theUseOri);

  Standard_EXPORT Handle(GEOM_Object) GetNormal (Handle(GEOM_Object) theFace,
                                                 Handle(GEOM_Object) theOptionalPoint);

  Standard_EXPORT void GetBasicProperties (Handle(GEOM_Object) theShape,
                                           const Standard_Real theTolerance,
                                           Standard_Real& theLength,
                                           Standard_Real& theSurfArea,
                                           Standard_Real& theVolume);

  Standard_EXPORT void GetInertia (Handle(GEOM_Object) theShape,
                                   Standard_Real& I11, Standard_Real& I12, Standard_Real& I13,
                                   Standard_Real& I21, Standard_Real& I22, Standard_Real& I23,
                                   Standard_Real& I31, Standard_Real& I32, Standard_Real& I33,
                                   Standard_Real& Ix , Standard_Real& Iy , Standard_Real& Iz);

  Standard_EXPORT void GetBoundingBox (Handle(GEOM_Object) theShape,
                                       const Standard_Boolean precise,
                                       Standard_Real& Xmin, Standard_Real& Xmax,
                                       Standard_Real& Ymin, Standard_Real& Ymax,
                                       Standard_Real& Zmin, Standard_Real& Zmax);

  Standard_EXPORT Handle(GEOM_Object) GetBoundingBox (Handle(GEOM_Object) theShape,
                                                      const Standard_Boolean precise);

  Standard_EXPORT void GetTolerance (Handle(GEOM_Object) theShape,
                                     Standard_Real& FaceMin, Standard_Real& FaceMax,
                                     Standard_Real& EdgeMin, Standard_Real& EdgeMax,
                                     Standard_Real& VertMin, Standard_Real& VertMax);

  struct ShapeError {
    BRepCheck_Status error;
    std::list<int>   incriminated;
  };

  Standard_EXPORT bool CheckShape (Handle(GEOM_Object)     theShape,
                                   const Standard_Boolean  theIsCheckGeom,
                                   std::list<ShapeError>  &theErrors);

  Standard_EXPORT TCollection_AsciiString PrintShapeErrors
                                  (Handle(GEOM_Object)          theShape,
                                   const std::list<ShapeError> &theErrors);

  Standard_EXPORT bool CheckSelfIntersections (Handle(GEOM_Object) theShape,
                                               const SICheckLevel  theCheckLevel,
                                               Handle(TColStd_HSequenceOfInteger)& theIntersections);
  
  Standard_EXPORT bool CheckSelfIntersectionsFast (Handle(GEOM_Object) theShape,
                                                   float  deflection, 
                                                   double tolerance,
                                                   Handle(TColStd_HSequenceOfInteger)& theIntersections);

  Standard_EXPORT bool CheckBOPArguments (const Handle(GEOM_Object) &theShape);
  
  Standard_EXPORT bool FastIntersect (Handle(GEOM_Object) theShape1, Handle(GEOM_Object) theShape2,
                                      double tolerance, float deflection,
                                      Handle(TColStd_HSequenceOfInteger)& theIntersections1,
                                      Handle(TColStd_HSequenceOfInteger)& theIntersections2);

  Standard_EXPORT TCollection_AsciiString IsGoodForSolid (Handle(GEOM_Object) theShape);

  Standard_EXPORT TCollection_AsciiString WhatIs (Handle(GEOM_Object) theShape);

  Standard_EXPORT std::vector<bool> AreCoordsInside (Handle(GEOM_Object) theShape,
                                                     const std::vector<double>& coords,
                                                     double tolerance = Precision::Confusion());

  Standard_EXPORT Standard_Real GetMinDistance (Handle(GEOM_Object) theShape1,
                                                Handle(GEOM_Object) theShape2,
                                                Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1,
                                                Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2);

  Standard_EXPORT Standard_Integer ClosestPoints (Handle(GEOM_Object) theShape1,
                                                  Handle(GEOM_Object) theShape2,
                                                  Handle(TColStd_HSequenceOfReal)& theDoubles);

  Standard_EXPORT void PointCoordinates (Handle(GEOM_Object) theShape,
                                         Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ);

  Standard_EXPORT Standard_Real GetAngle (Handle(GEOM_Object) theLine1, Handle(GEOM_Object) theLine2);

  Standard_EXPORT Standard_Real GetAngleBtwVectors (Handle(GEOM_Object) theVec1, Handle(GEOM_Object) theVec2);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) PatchFace(Handle(GEOM_Object) theShape);

  // Methods for receiving radiuses of curvature of curves and surfaces
  // in the given point
  Standard_EXPORT Standard_Real CurveCurvatureByParam (Handle(GEOM_Object) theCurve,
                                                       Standard_Real& theParam);
  Standard_EXPORT Standard_Real CurveCurvatureByPoint (Handle(GEOM_Object) theCurve,
                                                       Handle(GEOM_Object) thePoint);
  Standard_EXPORT Standard_Real MaxSurfaceCurvatureByParam (Handle(GEOM_Object) theSurf,
                                                            Standard_Real& theUParam,
                                                            Standard_Real& theVParam);
  Standard_EXPORT Standard_Real MaxSurfaceCurvatureByPoint (Handle(GEOM_Object) theSurf,
                                                            Handle(GEOM_Object) thePoint);
  Standard_EXPORT Standard_Real MinSurfaceCurvatureByParam (Handle(GEOM_Object) theSurf,
                                                            Standard_Real& theUParam,
                                                            Standard_Real& theVParam);
  Standard_EXPORT Standard_Real MinSurfaceCurvatureByPoint (Handle(GEOM_Object) theSurf,
                                                            Handle(GEOM_Object) thePoint);

  // Methods checking the shapes which are not applicable to modelling operations
  typedef std::pair< Handle(GEOM_Object), Handle(GEOM_Object)> CoupleOfObjects;
  struct FailedChecks
  {
    Standard_Integer TypeOfCheck;
    CoupleOfObjects FailedShapes;
  };

  Standard_EXPORT std::list<CoupleOfObjects> SelfIntersected2D(const std::list<FailedChecks>& theChecks);
  Standard_EXPORT std::list<CoupleOfObjects> InterferingSubshapes(const std::list<FailedChecks>& theChecks,
                                                                  const int theShapeType1,
                                                                  const int theShapeType2);
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) SmallEdges(const std::list<FailedChecks>& theChecks);
  Standard_EXPORT std::list<CoupleOfObjects> DistantShapes(const std::list<FailedChecks>& theChecks,
                                                           const int theShapeType,
                                                           const int theSubShapeType,
                                                           double theTolerance);
  Standard_EXPORT void CheckConformityShape(Handle(GEOM_Object) theShape, std::list<FailedChecks>& theChecks);

  Standard_EXPORT double ComputeTolerance(Handle(GEOM_Object) theEdge, Handle(GEOM_Object) theFace);
  Standard_EXPORT double UpdateTolerance(Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(GEOM_Object) SurfaceCurvatureByPointAndDirection
                                      (Handle(GEOM_Object) theSurf,
                                       Handle(GEOM_Object) thePoint,
                                       Handle(GEOM_Object) theDirection);

  // Methods to convert X,Y,Z coordinates of point to U,V parameters on surface and back
  Standard_EXPORT Handle(TColStd_HArray1OfReal) XYZtoUV
                                      (Handle(GEOM_Object) theSurf,
                                       const Handle(TColStd_HArray1OfReal)& theXYZlist,
                                       bool isNormalized);

  Standard_EXPORT Handle(TColStd_HArray1OfReal) UVtoXYZ
                                      (Handle(GEOM_Object) theSurf,
                                       const Handle(TColStd_HArray1OfReal)& theUVlist,
                                       bool isNormalized);

  // Methods to compute proximity between two shapes
  Standard_EXPORT Handle(GEOM_Object) ShapeProximityCalculator(Handle(GEOM_Object) theShape1,
                                                               Handle(GEOM_Object) theShape2);
  Standard_EXPORT Standard_Real GetCoarseProximity(Handle(GEOM_Object) theCalculator,
                                                   bool doPythonDump = true);
  Standard_EXPORT Standard_Real GetPreciseProximity(Handle(GEOM_Object) theCalculator);
  Standard_EXPORT void SetShapeSampling(Handle(GEOM_Object) theCalculator,
                                        Handle(GEOM_Object) theShape,
                                        const Standard_Integer theNbSamples);

 private:

   void FillErrorsSub
           (const BRepCheck_Analyzer                   &theAna,
            const TopoDS_Shape                         &theShape,
            const TopAbs_ShapeEnum                     theSubType,
                  TopTools_DataMapOfIntegerListOfShape &theMapErrors) const;
   void FillErrors
             (const BRepCheck_Analyzer                   &theAna,
              const TopoDS_Shape                         &theShape,
                    TopTools_DataMapOfIntegerListOfShape &theMapErrors,
                    TopTools_MapOfShape                  &theMapShapes) const;

  void FillErrors (const BRepCheck_Analyzer &theAna,
                   const TopoDS_Shape       &theShape,
                   std::list<ShapeError>    &theErrors) const;

  Standard_Real getSurfaceCurvatures (const Handle(Geom_Surface)& aSurf,
                                      Standard_Real theUParam,
                                      Standard_Real theVParam,
                                      Standard_Boolean theNeedMaxCurv);
};

#endif
