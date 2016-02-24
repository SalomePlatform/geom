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

#include <Standard_Stream.hxx>

#include <GEOMImpl_ProjectionDriver.hxx>

#include <GEOMImpl_IMirror.hxx>
#include <GEOMImpl_IProjection.hxx>
#include <GEOMImpl_IProjOnCyl.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>
#include <GEOMUtils_HTrsfCurve2d.hxx>

#include <Approx_Curve2d.hxx>
#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_ProjectionDriver::GetID()
{
  static Standard_GUID aProjectionDriver ("FF1BBB70-5D14-4df2-980B-3A668264EA16");
  return aProjectionDriver; 
}


//=======================================================================
//function : GEOMImpl_ProjectionDriver
//purpose  : 
//=======================================================================

GEOMImpl_ProjectionDriver::GEOMImpl_ProjectionDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_ProjectionDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull())  return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if (aFunction.IsNull()) return 0;

  Standard_Integer aType = aFunction->GetType();

  if (aType == PROJECTION_COPY) {
    // Projection
    TopoDS_Shape aShape;
    gp_Trsf aTrsf;

    GEOMImpl_IMirror TI (aFunction);

    Handle(GEOM_Function) anOriginalFunction = TI.GetOriginal();
    if (anOriginalFunction.IsNull()) return 0;

    TopoDS_Shape anOriginal = anOriginalFunction->GetValue();
    if (anOriginal.IsNull()) return 0;

    // Source shape (point, edge or wire)
    if (anOriginal.ShapeType() != TopAbs_VERTEX &&
        anOriginal.ShapeType() != TopAbs_EDGE &&
        anOriginal.ShapeType() != TopAbs_WIRE) {
      Standard_ConstructionError::Raise
        ("Projection aborted : the source shape is neither a vertex, nor an edge or a wire");
    }

    // Target face
    Handle(GEOM_Function) aTargetFunction = TI.GetPlane();
    if (aTargetFunction.IsNull()) return 0;
    TopoDS_Shape aFaceShape = aTargetFunction->GetValue();
    //if (aFaceShape.IsNull() || aFaceShape.ShapeType() != TopAbs_FACE) {
    //  Standard_ConstructionError::Raise
    //    ("Projection aborted : the target shape is not a face");
    //}

    Standard_Real tol = 1.e-4;        

    if (anOriginal.ShapeType() == TopAbs_VERTEX) {
      if (aFaceShape.IsNull() || aFaceShape.ShapeType() != TopAbs_FACE) {
        Standard_ConstructionError::Raise
          ("Projection aborted : the target shape is not a face");
      }
      TopoDS_Face aFace = TopoDS::Face(aFaceShape);
      Handle(Geom_Surface) surface = BRep_Tool::Surface(aFace);
      double U1, U2, V1, V2;
      //surface->Bounds(U1, U2, V1, V2);
      BRepTools::UVBounds(aFace, U1, U2, V1, V2);

      // projector
      GeomAPI_ProjectPointOnSurf proj;
      proj.Init(surface, U1, U2, V1, V2, tol);

      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(anOriginal));
      proj.Perform(aPnt);
      if (!proj.IsDone()) {
        Standard_ConstructionError::Raise
          ("Projection aborted : the algorithm failed");
      }
      int nbPoints = proj.NbPoints();
      if (nbPoints < 1) {
        Standard_ConstructionError::Raise("No solution found");
      }

      Quantity_Parameter U, V;
      proj.LowerDistanceParameters(U, V);
      gp_Pnt2d aProjPnt (U, V);

      // classifier
      BRepClass_FaceClassifier aClsf (aFace, aProjPnt, tol);
      if (aClsf.State() != TopAbs_IN && aClsf.State() != TopAbs_ON) {
        bool isSol = false;
        double minDist = RealLast();
        for (int i = 1; i <= nbPoints; i++) {
          Quantity_Parameter Ui, Vi;
          proj.Parameters(i, Ui, Vi);
          aProjPnt = gp_Pnt2d(Ui, Vi);
          aClsf.Perform(aFace, aProjPnt, tol);
          if (aClsf.State() == TopAbs_IN || aClsf.State() == TopAbs_ON) {
            isSol = true;
            double dist = proj.Distance(i);
            if (dist < minDist) {
              minDist = dist;
              U = Ui;
              V = Vi;
            }
          }
        }
        if (!isSol) {
          Standard_ConstructionError::Raise("No solution found");
        }
      }

      gp_Pnt surfPnt = surface->Value(U, V);

      aShape = BRepBuilderAPI_MakeVertex(surfPnt).Shape();
    }
    else {
      //see BRepTest_BasicCommands.cxx for example of BRepOffsetAPI_NormalProjection
      BRepOffsetAPI_NormalProjection OrtProj (aFaceShape);
      OrtProj.Add(anOriginal);

      // Compute maximal tolerance of projection.
      TopExp_Explorer anExp(anOriginal,TopAbs_VERTEX);
      Standard_Real   aMaxTol = Precision::Confusion();

      for(; anExp.More(); anExp.Next()) { 
        const TopoDS_Vertex aVtx    = TopoDS::Vertex(anExp.Current());
        const Standard_Real aCurTol = BRep_Tool::Tolerance(aVtx);

        if (aMaxTol < aCurTol) {
          aMaxTol = aCurTol;
        }
      }

      Standard_Real tol2d = Pow(aMaxTol, 2./3);
      GeomAbs_Shape Continuity = GeomAbs_C2;
      Standard_Integer MaxDeg = 14;
      Standard_Integer MaxSeg = 16;

      OrtProj.SetParams(aMaxTol, tol2d, Continuity, MaxDeg, MaxSeg);

      try {
        OrtProj.Build();
      } catch (Standard_Failure) {
        Handle(Standard_Failure) aFail = Standard_Failure::Caught();
        TCollection_AsciiString aMsg (aFail->GetMessageString());
        if (!aMsg.Length())
          aMsg = "Projection aborted : possibly the source shape intersects the cylinder's axis";
        Standard_ConstructionError::Raise(aMsg.ToCString());
      }
      if (!OrtProj.IsDone()) {
        Standard_ConstructionError::Raise
          ("Projection aborted : BRepOffsetAPI_NormalProjection failed");
      }

      aShape = OrtProj.Shape();

      // check that the result shape is an empty compound
      // (IPAL22905: TC650: Projection on face dialog problems)
      if( !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND )
      {
        TopoDS_Iterator anIter( aShape );
        if( !anIter.More() )
          Standard_ConstructionError::Raise("Projection aborted : empty compound produced");
      }
    }

    if (aShape.IsNull()) return 0;

    aFunction->SetValue(aShape);
#if OCC_VERSION_MAJOR < 7
    log.SetTouched(Label());
#else
    log->SetTouched(Label());
#endif
  } else if (aType == PROJECTION_ON_WIRE) {
    // Perform projection of point on a wire or an edge.
    GEOMImpl_IProjection aProj (aFunction);
    Handle(GEOM_Function) aPointFunction = aProj.GetPoint();
    Handle(GEOM_Function) aShapeFunction = aProj.GetShape();

    if (aPointFunction.IsNull() || aShapeFunction.IsNull()) {
      return 0;
    }

    TopoDS_Shape aPoint = aPointFunction->GetValue();
    TopoDS_Shape aShape = aShapeFunction->GetValue();

    if (aPoint.IsNull() || aShape.IsNull()) {
      return 0;
    }

    // Check shape types.
    if (aPoint.ShapeType() != TopAbs_VERTEX) {
      Standard_ConstructionError::Raise
        ("Projection aborted : the point is not a vertex");
    }

    if (aShape.ShapeType() != TopAbs_EDGE &&
        aShape.ShapeType() != TopAbs_WIRE) {
      Standard_ConstructionError::Raise
        ("Projection aborted : the shape is neither an edge nor a wire");
    }

    // Perform projection.
    BRepExtrema_DistShapeShape aDistShSh(aPoint, aShape, Extrema_ExtFlag_MIN);

    if (aDistShSh.IsDone() == Standard_False) {
      Standard_ConstructionError::Raise("Projection not done");
    }

    Standard_Boolean hasValidSolution = Standard_False;
    Standard_Integer aNbSolutions     = aDistShSh.NbSolution();
    Standard_Integer i;
    double           aParam   = 0.;
    Standard_Real    aTolConf = BRep_Tool::Tolerance(TopoDS::Vertex(aPoint));
    Standard_Real    aTolAng  = 1.e-4;        

    for (i = 1; i <= aNbSolutions; i++) {
      Standard_Boolean        isValid       = Standard_False;
      BRepExtrema_SupportType aSupportType  = aDistShSh.SupportTypeShape2(i);
      TopoDS_Shape            aSupportShape = aDistShSh.SupportOnShape2(i);

      if (aSupportType == BRepExtrema_IsOnEdge) {
        // Minimal distance inside edge is really a projection.
        isValid = Standard_True;
        aDistShSh.ParOnEdgeS2(i, aParam);
      } else if (aSupportType == BRepExtrema_IsVertex) {
        TopExp_Explorer anExp(aShape, TopAbs_EDGE);

        if (aDistShSh.Value() <= aTolConf) {
          // The point lies on the shape. This means this point
          // is really a projection.
          for (; anExp.More() && !isValid; anExp.Next()) {
            TopoDS_Edge aCurEdge = TopoDS::Edge(anExp.Current());

            if (aCurEdge.IsNull() == Standard_False) {
              TopoDS_Vertex aVtx[2];
                        
              TopExp::Vertices(aCurEdge, aVtx[0], aVtx[1]);

              for (int j = 0; j < 2; j++) {
                if (aSupportShape.IsSame(aVtx[j])) {
                  // The current edge is a projection edge.
                  isValid       = Standard_True;
                  aSupportShape = aCurEdge;
                  aParam        = BRep_Tool::Parameter(aVtx[j], aCurEdge);
                  break;
                }
              }
            }
          }
        } else {
          // Minimal distance to vertex is not always a real projection.
          gp_Pnt aPnt    = BRep_Tool::Pnt(TopoDS::Vertex(aPoint));
          gp_Pnt aPrjPnt = BRep_Tool::Pnt(TopoDS::Vertex(aSupportShape));
          gp_Vec aDProjP(aPrjPnt, aPnt);

          for (; anExp.More() && !isValid; anExp.Next()) {
            TopoDS_Edge aCurEdge = TopoDS::Edge(anExp.Current());
 
            if (aCurEdge.IsNull() == Standard_False) {
              TopoDS_Vertex aVtx[2];
                          
              TopExp::Vertices(aCurEdge, aVtx[0], aVtx[1]);
 
              for (int j = 0; j < 2; j++) {
                if (aSupportShape.IsSame(aVtx[j])) {
                  // Check if the point is a projection to the current edge.
                  Standard_Real      anEdgePars[2];
                  Handle(Geom_Curve) aCurve =
                    BRep_Tool::Curve(aCurEdge, anEdgePars[0], anEdgePars[1]);
                  gp_Pnt             aVal;
                  gp_Vec             aD1;

                  aParam = BRep_Tool::Parameter(aVtx[j], aCurEdge);
                  aCurve->D1(aParam, aVal, aD1);

                  if (Abs(aD1.Dot(aDProjP)) <= aTolAng) {
                    // The current edge is a projection edge.
                    isValid       = Standard_True;
                    aSupportShape = aCurEdge;
                    break;
                  }
                }
              }
            }
          }
        }
      }
      

      if (isValid) {
        if (hasValidSolution) {
          Standard_ConstructionError::Raise
            ("Projection aborted : multiple solutions");
        }

        // Store the valid solution.
        hasValidSolution = Standard_True;

        // Normalize parameter.
        TopoDS_Edge aSupportEdge = TopoDS::Edge(aSupportShape);
        Standard_Real aF, aL;

        BRep_Tool::Range(aSupportEdge, aF, aL);

        if (Abs(aL - aF) <= aTolConf) {
          Standard_ConstructionError::Raise
            ("Projection aborted : degenerated projection edge");
        }

        aParam = (aParam - aF)/(aL - aF);
        aProj.SetU(aParam);

        // Compute edge index.
        TopExp_Explorer anExp(aShape, TopAbs_EDGE);
        int anIndex = 0;

        for (; anExp.More(); anExp.Next(), anIndex++) {
          if (aSupportShape.IsSame(anExp.Current())) {
            aProj.SetIndex(anIndex);
            break;
          }
        }

        if (!anExp.More()) {
          Standard_ConstructionError::Raise
            ("Projection aborted : Can't define edge index");
        }

        // Construct a projection vertex.
        const gp_Pnt &aPntProj = aDistShSh.PointOnShape2(i);
        TopoDS_Shape  aProj    = BRepBuilderAPI_MakeVertex(aPntProj).Shape();
        
        aFunction->SetValue(aProj);
      }
    }

    if (!hasValidSolution) {
      Standard_ConstructionError::Raise("Projection aborted : no projection");
    }
  } else if (aType == PROJECTION_ON_CYLINDER) {
    GEOMImpl_IProjOnCyl   aProj (aFunction);
    Handle(GEOM_Function) aShapeFunction = aProj.GetShape();
      
    if (aShapeFunction.IsNull()) {
      return 0;
    }

    TopoDS_Shape aShape = aShapeFunction->GetValue();

    if (aShape.IsNull()) {
      return 0;
    }

    // Get the face.
    const TopAbs_ShapeEnum aType          = aShape.ShapeType();
    const Standard_Real    aRadius        = aProj.GetRadius();
    const Standard_Real    aStartAngle    = aProj.GetStartAngle();
    const Standard_Real    aLengthAngle   = aProj.GetAngleLength();
    const Standard_Real    aRotationAngle = aProj.GetAngleRotation();

    if (aType != TopAbs_WIRE && aType != TopAbs_FACE) {
      return 0;
    }

    if (aRadius <= Precision::Confusion()) {
      return 0;
    }

    TopoDS_Shape aProjShape = projectOnCylinder
      (aShape, aRadius, aStartAngle, aLengthAngle, aRotationAngle);

    if (aProjShape.IsNull()) {
      return 0;
    }

    aFunction->SetValue(aProjShape);
  }

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_ProjectionDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  Standard_Integer aType = function->GetType();

  theOperationName = "PROJECTION";

  switch ( aType ) {
  case PROJECTION_COPY:
  {
    GEOMImpl_IMirror aCI( function );

    AddParam( theParams, "Source object", aCI.GetOriginal() );
    AddParam( theParams, "Target face", aCI.GetPlane() );
    break;
  }
  case PROJECTION_ON_WIRE:
  {
    GEOMImpl_IProjection aProj (function);

    AddParam(theParams, "Point", aProj.GetPoint());
    AddParam(theParams, "Shape", aProj.GetShape());

    break;
  }
  case PROJECTION_ON_CYLINDER:
  {
    theOperationName = "PROJ_ON_CYL";

    GEOMImpl_IProjOnCyl aProj (function);
    const Standard_Real aLengthAngle = aProj.GetAngleLength();

    AddParam(theParams, "Shape",        aProj.GetShape());
    AddParam(theParams, "Radius",       aProj.GetRadius());
    AddParam(theParams, "Start angle",  aProj.GetStartAngle());

    if (aLengthAngle >= 0.) {
      AddParam(theParams, "Length angle", aLengthAngle);
    }

    AddParam(theParams, "Rotation angle", aProj.GetAngleRotation());

    break;
  }
  default:
    return false;
  }

  return true;
}

//================================================================================
/*!
 * \brief Performs projection of a planar wire or a face on a cylinder.
 */
//================================================================================

TopoDS_Shape GEOMImpl_ProjectionDriver::projectOnCylinder
                                (const TopoDS_Shape  &theShape,
                                 const Standard_Real  theRadius,
                                 const Standard_Real  theStartAngle,
                                 const Standard_Real  theAngleLength,
                                 const Standard_Real  theAngleRotation) const
{
  TopoDS_Shape aResult;

  // Get the face.
  const TopAbs_ShapeEnum aType = theShape.ShapeType();
  TopoDS_Face            aFace;

  if (aType == TopAbs_WIRE) {
    // Try to create a planar face.
    TopoDS_Wire             aWire = TopoDS::Wire(theShape);
    BRepBuilderAPI_MakeFace aMkFace(aWire, Standard_True);

    if (aMkFace.IsDone()) {
      aFace = aMkFace.Face();
    } else {
      // Check if the wire is a straight line.
      TopExp_Explorer anEExp(aWire, TopAbs_EDGE);
      TopoDS_Edge     anEdge;

      for (; anEExp.More(); anEExp.Next()) {
        anEdge = TopoDS::Edge(anEExp.Current());

        if (!BRep_Tool::Degenerated(anEdge)) {
          break;
        }
      }

      if (anEExp.More()) {
        // Not degenerated edge found. Try to create a plane.
        Standard_Real      aPar[2];
        Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aPar[0], aPar[1]);
        gp_Pnt             aP0    = aCurve->Value(aPar[0]);
        gp_Pnt             aP1    = aCurve->Value(0.5*(aPar[1] + aPar[0]));
        gp_Vec             aX(aP1.XYZ().Subtracted(aP0.XYZ()));
        Standard_Real      aTolConf = Precision::Confusion();

        if (aX.Magnitude() > aTolConf) {
          aX.Normalize();

          // Get the plane normal ortogonal to Z axis.
          gp_Vec aZ(0., 0., 1.);
          gp_Vec aN = aX.Crossed(aZ);

          if (aN.Magnitude() <= aTolConf) {
            // aX is parallel to aZ. Get the plane normal ortogonal to Y axis.
            gp_Vec aY(0., 1., 0.);

            aN = aX.Crossed(aY);
          }

          if (aN.Magnitude() >      aTolConf) {
            gp_Ax3                  anAxis(aP0, gp_Dir(aN), gp_Dir(aX));
            Handle(Geom_Plane)      aPlane = new Geom_Plane(anAxis);
            BRepBuilderAPI_MakeFace aMkFace(aPlane, aWire);

            if (aMkFace.IsDone()) {
              aFace = aMkFace.Face();
            }
          }
        }
      }
    }
  } else if (aType == TopAbs_FACE) {
    aFace = TopoDS::Face(theShape);
  }

  if (aFace.IsNull()) {
    return aResult;
  }

  // Compute 2d translation transformation.
  TopoDS_Wire            anOuterWire = BRepTools::OuterWire(aFace);
  BRepTools_WireExplorer aOWExp(anOuterWire, aFace);

  if (!aOWExp.More()) {
    // NEVERREACHED
    return aResult;
  }

  // Rotate 2D presentation of face.
  TopoDS_Vertex       aFirstVertex = aOWExp.CurrentVertex();
  TopoDS_Edge         aFirstEdge   = aOWExp.Current();
  gp_Pnt              aPnt         = BRep_Tool::Pnt(aFirstVertex);
  BRepAdaptor_Curve2d anAdaptorCurve(aFirstEdge, aFace);
  Standard_Real       aParam       =
    BRep_Tool::Parameter(aFirstVertex, aFirstEdge, aFace);
  gp_Pnt2d            aPntUV;
  gp_Vec2d            aVecUV;
  gp_Vec2d            aVecU0(1., 0);

  anAdaptorCurve.D1(aParam, aPntUV, aVecUV);

  if (aVecUV.Magnitude() <= gp::Resolution()) {
    return aResult;
  }

  if (aFirstEdge.Orientation() == TopAbs_REVERSED) {
    aVecUV.Reverse();
  }

  const Standard_Real    anAngle    = aVecUV.Angle(aVecU0) + theAngleRotation;
  const Standard_Boolean isToRotate = Abs(anAngle) > Precision::Angular();
  gp_Trsf2d              aRotTrsf;
  Bnd_Box2d              aUVBox;
  Standard_Real          aPar[2];

  if (isToRotate) {
    aRotTrsf.SetRotation(aPntUV, anAngle);
  }

  for (; aOWExp.More(); aOWExp.Next()) {
    TopoDS_Edge                 anEdge   = aOWExp.Current();
    Handle(Geom2d_Curve)        aCurve   =
        BRep_Tool::CurveOnSurface(anEdge, aFace, aPar[0], aPar[1]);

    if (aCurve.IsNull()) {
      continue;
    }

    if (isToRotate) {
      aCurve = Handle(Geom2d_Curve)::DownCast(aCurve->Transformed(aRotTrsf));
    }

    BndLib_Add2dCurve::Add(aCurve, aPar[0], aPar[1], 0., aUVBox);
  }

  Standard_Real aU[2];
  Standard_Real aV[2];

  aUVBox.Get(aU[0], aV[0], aU[1], aV[1]);

  // Compute anisotropic transformation from a face's 2d space
  // to cylinder's 2d space.
  GEOMUtils::Trsf2d aTrsf2d
            (1./theRadius, 0., theStartAngle - aU[0]/theRadius,
             0.,           1., aPnt.Z() - aPntUV.Y());

  // Compute scaling trsf.
  const Standard_Boolean isToScale = theAngleLength >= Precision::Angular();
  gp_Trsf2d aScaleTrsf;

  if (isToScale) {
    // Perform 2d scaling.
    gp_Pnt2d            aMidPnt(0.5*(aU[1] + aU[0]), 0.5*(aV[1] + aV[0]));
    const Standard_Real aScaleFactor = theAngleLength*theRadius/(aU[1] - aU[0]);

    aTrsf2d.TransformD0(aMidPnt);

    aScaleTrsf.SetScale(aMidPnt, aScaleFactor);
  }

  // Get 2d presentation of a face.
  Handle(Geom_Surface) aCylinder =
    new Geom_CylindricalSurface(gp_Ax3(), theRadius);
  GeomAdaptor_Surface  aGACyl(aCylinder);
  TopExp_Explorer      anExp(aFace, TopAbs_WIRE);
  TopTools_ListOfShape aWires;
  Standard_Real        aUResol = aGACyl.UResolution(Precision::Confusion());
  Standard_Real        aVResol = aGACyl.VResolution(Precision::Confusion());

  for (; anExp.More(); anExp.Next()) {
    TopoDS_Wire             aWire = TopoDS::Wire(anExp.Current());
    BRepTools_WireExplorer  aWExp(aWire, aFace);
    BRepBuilderAPI_MakeWire aMkWire;

    for (; aWExp.More(); aWExp.Next()) {
      TopoDS_Edge                 anEdge   = aWExp.Current();
      Handle(Geom2d_Curve)        aCurve   =
        BRep_Tool::CurveOnSurface(anEdge, aFace, aPar[0], aPar[1]);

      if (aCurve.IsNull()) {
        continue;
      }

      if (isToRotate) {
        aCurve = Handle(Geom2d_Curve)::DownCast(aCurve->Transformed(aRotTrsf));
      }

      // Transform the curve to cylinder's parametric space.
#if OCC_VERSION_MAJOR < 7
      GEOMUtils::Handle(HTrsfCurve2d) aTrsfCurve =
#else
      Handle(GEOMUtils::HTrsfCurve2d) aTrsfCurve =
#endif
        new GEOMUtils::HTrsfCurve2d(aCurve, aPar[0], aPar[1], aTrsf2d);
      Approx_Curve2d                  aConv (aTrsfCurve, aPar[0], aPar[1],
                                                         aUResol, aVResol, GeomAbs_C1,
                                             9, 1000);

      if (!aConv.IsDone() && !aConv.HasResult()) {
        return aResult;
      }

      Handle(Geom2d_Curve) aCylCurve = aConv.Curve();

      if (isToScale) {
        aCylCurve->Transform(aScaleTrsf);
      }

      // Create edge and add it to the wire.
      BRepBuilderAPI_MakeEdge aMkEdge(aCylCurve, aCylinder);

      if (!aMkEdge.IsDone()) {
        return aResult;
      }

      aMkWire.Add(aMkEdge.Edge());

      if (!aMkWire.IsDone()) {
        return aResult;
      }
    }

    if (aWire.IsSame(anOuterWire)) {
      // Make the outer wire first.
      aWires.Prepend(aMkWire.Wire());
    } else {
      aWires.Append(aMkWire.Wire());
    }
  }

  // Create a face.
  if (aWires.IsEmpty()) {
    return aResult;
  }

  TopTools_ListIteratorOfListOfShape aWIter(aWires);
  TopoDS_Wire                        aWire = TopoDS::Wire(aWIter.Value());
  BRepBuilderAPI_MakeFace aMkFace(aCylinder, aWire);

  if (!aMkFace.IsDone()) {
    return aResult;
  }

  for (aWIter.Next(); aWIter.More(); aWIter.Next()) {
    aWire = TopoDS::Wire(aWIter.Value());
    aMkFace.Add(aWire);

    if (!aMkFace.IsDone()) {
      return aResult;
    }
  }

  // Build 3D curves.
  TopoDS_Face  aCylFace = aMkFace.Face();
  TopoDS_Shape aResShape;

  BRepLib::BuildCurves3d(aCylFace);

  // Check shape.
  if (aType == TopAbs_WIRE) {
    TopExp_Explorer aResExp(aCylFace, TopAbs_WIRE);

    if (aResExp.More()) {
      aResShape = aResExp.Current();
    }
  } else {
    aResShape = aCylFace;
  }

  if (aResShape.IsNull() == Standard_False) {
    if (!GEOMUtils::CheckShape(aResShape, true)) {
      if (!GEOMUtils::FixShapeTolerance(aResShape)) {
        return aResult;
      }
    }

    aResult = aResShape;
  }

  return aResult;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ProjectionDriver,GEOM_BaseDriver);
