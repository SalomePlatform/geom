// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>
#include <BRepTools.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>

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
Standard_Integer GEOMImpl_ProjectionDriver::Execute(TFunction_Logbook& log) const
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

      //Standard_Real tol = 1.e-4;        
      //Standard_Real tol2d = Pow(tol, 2./3);
      //GeomAbs_Shape Continuity = GeomAbs_C2;  
      //Standard_Integer MaxDeg = 14;           
      //Standard_Integer MaxSeg = 16;           
      //OrtProj.SetParams(tol, tol2d, Continuity, MaxDeg, MaxSeg);
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
    log.SetTouched(Label()); 
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
  default:
    return false;
  }
  
  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_ProjectionDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ProjectionDriver,GEOM_BaseDriver);
