
#ifndef _GEOMImpl_IMeasureOperations_HXX_
#define _GEOMImpl_IMeasureOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <BRepCheck_Analyzer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TCollection_AsciiString.hxx>

class GEOM_Engine;
class Handle(GEOM_Object);

class GEOMImpl_IMeasureOperations : public GEOM_IOperations {
 public:
  GEOMImpl_IMeasureOperations(GEOM_Engine* theEngine, int theDocID);
  ~GEOMImpl_IMeasureOperations();

  Handle(GEOM_Object) GetCentreOfMass (Handle(GEOM_Object) theShape);

  void GetBasicProperties (Handle(GEOM_Object) theShape,
                           Standard_Real& theLength,
                           Standard_Real& theSurfArea,
                           Standard_Real& theVolume);

  void GetInertia (Handle(GEOM_Object) theShape,
                   Standard_Real& I11, Standard_Real& I12, Standard_Real& I13,
                   Standard_Real& I21, Standard_Real& I22, Standard_Real& I23,
                   Standard_Real& I31, Standard_Real& I32, Standard_Real& I33,
                   Standard_Real& Ix , Standard_Real& Iy , Standard_Real& Iz);

  void GetBoundingBox (Handle(GEOM_Object) theShape,
                       Standard_Real& Xmin, Standard_Real& Xmax,
                       Standard_Real& Ymin, Standard_Real& Ymax,
                       Standard_Real& Zmin, Standard_Real& Zmax);

  void GetTolerance (Handle(GEOM_Object) theShape,
                     Standard_Real& FaceMin, Standard_Real& FaceMax,
                     Standard_Real& EdgeMin, Standard_Real& EdgeMax,
                     Standard_Real& VertMin, Standard_Real& VertMax);

  bool CheckShape (Handle(GEOM_Object)      theShape,
                   TCollection_AsciiString& theDump);

  TCollection_AsciiString WhatIs (Handle(GEOM_Object) theShape);

  Standard_Real GetMinDistance (Handle(GEOM_Object) theShape1, Handle(GEOM_Object) theShape2,
                                Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1,
                                Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2);

 void PointCoordinates( Handle(GEOM_Object) theShape,
                        Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ );
 
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
