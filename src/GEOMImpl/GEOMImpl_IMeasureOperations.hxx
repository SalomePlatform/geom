// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp_Ax3.hxx>

class GEOM_Engine;
class Handle(GEOM_Object);

class GEOMImpl_IMeasureOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IMeasureOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IMeasureOperations();

  Standard_EXPORT void GetPosition (Handle(GEOM_Object) theShape,
                                    Standard_Real& Ox, Standard_Real& Oy, Standard_Real& Oz,
                                    Standard_Real& Zx, Standard_Real& Zy, Standard_Real& Zz,
                                    Standard_Real& Xx, Standard_Real& Xy, Standard_Real& Xz);

  Standard_EXPORT Handle(GEOM_Object) GetCentreOfMass (Handle(GEOM_Object) theShape);

  Standard_EXPORT void GetBasicProperties (Handle(GEOM_Object) theShape,
                                           Standard_Real& theLength,
                                           Standard_Real& theSurfArea,
                                           Standard_Real& theVolume);

  Standard_EXPORT void GetInertia (Handle(GEOM_Object) theShape,
                                   Standard_Real& I11, Standard_Real& I12, Standard_Real& I13,
                                   Standard_Real& I21, Standard_Real& I22, Standard_Real& I23,
                                   Standard_Real& I31, Standard_Real& I32, Standard_Real& I33,
                                   Standard_Real& Ix , Standard_Real& Iy , Standard_Real& Iz);

  Standard_EXPORT void GetBoundingBox (Handle(GEOM_Object) theShape,
                                       Standard_Real& Xmin, Standard_Real& Xmax,
                                       Standard_Real& Ymin, Standard_Real& Ymax,
                                       Standard_Real& Zmin, Standard_Real& Zmax);

  Standard_EXPORT void GetTolerance (Handle(GEOM_Object) theShape,
                                     Standard_Real& FaceMin, Standard_Real& FaceMax,
                                     Standard_Real& EdgeMin, Standard_Real& EdgeMax,
                                     Standard_Real& VertMin, Standard_Real& VertMax);

  Standard_EXPORT bool CheckShape (Handle(GEOM_Object)      theShape,
                                   const Standard_Boolean   theIsCheckGeom,
                                   TCollection_AsciiString& theDump);

  Standard_EXPORT TCollection_AsciiString WhatIs (Handle(GEOM_Object) theShape);

  Standard_EXPORT Standard_Real GetMinDistance (Handle(GEOM_Object) theShape1,
                                                Handle(GEOM_Object) theShape2,
                                                Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1,
                                                Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2);

  Standard_EXPORT void PointCoordinates(Handle(GEOM_Object) theShape,
                                        Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ );

 public:
  Standard_EXPORT static gp_Ax3 GetPosition (const TopoDS_Shape& theShape);

 private:
  void StructuralDump (const BRepCheck_Analyzer& theAna,
                       const TopoDS_Shape&       theShape,
                       TCollection_AsciiString&  theDump);

  void GetProblemShapes (const BRepCheck_Analyzer&           theAna,
                         const TopoDS_Shape&                 theShape,
                         Handle(TopTools_HSequenceOfShape)&  sl,
                         Handle(TColStd_HArray1OfInteger)&   NbProblems,
                         TopTools_DataMapOfShapeListOfShape& theMap);

  void GetProblemSub (const BRepCheck_Analyzer&           theAna,
                      const TopoDS_Shape&                 theShape,
                      Handle(TopTools_HSequenceOfShape)&  sl,
                      Handle(TColStd_HArray1OfInteger)&   NbProblems,
                      const TopAbs_ShapeEnum              Subtype,
                      TopTools_DataMapOfShapeListOfShape& theMap);
};

#endif
