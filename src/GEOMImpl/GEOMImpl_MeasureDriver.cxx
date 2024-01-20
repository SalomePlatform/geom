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

#include <Standard_Stream.hxx>

#include <GEOMImpl_MeasureDriver.hxx>
#include <GEOMImpl_IMeasure.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepOffsetAPI_NormalProjection.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GProp_GProps.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomLProp_CLProps.hxx>
#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_NullObject.hxx>
#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_MeasureDriver::GetID()
{
  static Standard_GUID aMeasureDriver("FF1BBB65-5D14-4df2-980B-3A668264EA16");
  return aMeasureDriver; 
}


//=======================================================================
//function : GEOMImpl_MeasureDriver
//purpose  : 
//=======================================================================
GEOMImpl_MeasureDriver::GEOMImpl_MeasureDriver() 
{
}

//! This function is designed to evaluate normal curvature of the surface
//! in the given point along the given direction.
//! param[in] theFace face of interest.
//! param[in] thePoint point of interest.
//! param[in] theDir edge, giving the direction of interest.
//! return Edge, representing the curvature vector
TopoDS_Shape EvaluateAlongCurvature(const TopoDS_Shape& theFace,
                                    const TopoDS_Shape& thePoint,
                                    const TopoDS_Shape& theDir)
{
  // Point
  if (thePoint.IsNull())
    Standard_NullObject::Raise("Point for curvature measurement is null");
  if (thePoint.ShapeType() != TopAbs_VERTEX)
    Standard_TypeMismatch::Raise("Point for curvature calculation is not a vertex");
  gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(thePoint));

  // Point projection on the face
  Standard_Real U, V;
  aPnt = GEOMUtils::ProjectPointOnFace(aPnt, theFace, U, V);
  gp_Pnt2d UV (U, V);

  // Face and Vector
  TopoDS_Face aFace = TopoDS::Face(theFace);
  gp_Vec aV = GEOMUtils::GetVector(theDir, Standard_False);

  // Calculate differential properties
  BRepAdaptor_Surface aSurfAdapt (aFace);
  BRepLProp_SLProps Props (aSurfAdapt, UV.X(), UV.Y(), 2, 1e-7);
  if (!Props.IsNormalDefined())
    Standard_ConstructionError::Raise
      ("Curvature calculation failed: normal direction is not defined");

  // Get differential properties
  gp_Vec n = Props.Normal();
  if (aV.IsParallel(n, Precision::Angular()))
    Standard_ConstructionError::Raise
      ("Curvature calculation failed: direction is normal to the face");

  if (!Props.IsCurvatureDefined())
    Standard_ConstructionError::Raise
      ("Curvature calculation failed: curvature is not defined");

  // Curvature
  Standard_Real k = 0.;

  if (Props.IsUmbilic()) {
    k = Props.MaxCurvature();
  }
  else {
    gp_Dir aMaxDir, aMinDir;
    Props.CurvatureDirections(aMaxDir, aMinDir);

    gp_Vec2d T (aV.Dot(aMinDir), aV.Dot(aMaxDir));
    if (Abs(T.X()) < Precision::Confusion() &&
        Abs(T.Y()) < Precision::Confusion())
      Standard_ConstructionError::Raise
                     ("Curvature calculation failed: direction is normal to the face");
    gp_Dir2d aDirT (T);
    Standard_Real cosAlpha = aDirT.X();

    Standard_Real aMinCurv = Props.MinCurvature();
    Standard_Real aMaxCurv = Props.MaxCurvature();

    // Euler formula: k = k1 * cos^2(alpha) + k2 * sin^2(alpha)
    //                k = k2 + (k1 - k2) * cos^2(alpha)
    k = aMaxCurv + (aMinCurv - aMaxCurv) * cosAlpha * cosAlpha;
  }

  if (Abs(k) < Precision::Confusion())
    Standard_Failure::Raise("ZERO_CURVATURE");

  // Radius or -radius of curvature
  Standard_Real r = 1.0 / k;

  // Result
  gp_Dir aNormal (n);
  gp_Pnt aPntEnd (aPnt.XYZ() + aNormal.XYZ() * r);
  BRepBuilderAPI_MakeEdge aBuilder (aPnt, aPntEnd);
  if (!aBuilder.IsDone())
    Standard_ConstructionError::Raise("Curvature calculation failed: edge is not built");

  return aBuilder.Shape();
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_MeasureDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMeasure aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == CDG_MEASURE)
  {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull())
      Standard_NullObject::Raise("Shape for centre of mass calculation is null");

    gp_Ax3 aPos = GEOMUtils::GetPosition(aShapeBase);
    gp_Pnt aCenterMass = aPos.Location();
    aShape = BRepBuilderAPI_MakeVertex(aCenterMass).Shape();
  }
  else if (aType == BND_BOX_MEASURE || aType == BND_BOX_MEASURE_PRECISE)
  {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull())
      Standard_NullObject::Raise("Shape for bounding box calculation is null");

    BRepBuilderAPI_Copy aCopyTool (aShapeBase);
    if (!aCopyTool.IsDone())
      Standard_NullObject::Raise("Shape for bounding box calculation is bad");

    aShapeBase = aCopyTool.Shape();

    // remove triangulation to obtain more exact boundaries
    BRepTools::Clean(aShapeBase);

    Bnd_Box B;
    BRepBndLib::Add(aShapeBase, B);

    if (aType == BND_BOX_MEASURE_PRECISE) {
      if (!GEOMUtils::PreciseBoundingBox(aShapeBase, B)) {
        Standard_NullObject::Raise("Bounding box cannot be precised");
      }
    }

    Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
    B.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

    if (Xmax - Xmin < Precision::Confusion()) Xmax += Precision::Confusion();
    if (Ymax - Ymin < Precision::Confusion()) Ymax += Precision::Confusion();
    if (Zmax - Zmin < Precision::Confusion()) Zmax += Precision::Confusion();

    gp_Pnt P1 (Xmin, Ymin, Zmin);
    gp_Pnt P2 (Xmax, Ymax, Zmax);

    BRepPrimAPI_MakeBox MB (P1, P2);
    MB.Build();
    if (!MB.IsDone()) StdFail_NotDone::Raise("Bounding box cannot be computed from the given shape");
    aShape = MB.Shape();
  }
  else if (aType == VERTEX_BY_INDEX)
  {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Shape for centre of mass calculation is null");
    }

    int index = aCI.GetIndex();
    bool useOri = aCI.GetUseOri();
    gp_Pnt aVertex;

    if (aShapeBase.ShapeType() == TopAbs_VERTEX) {
      if ( index != 1 )
        Standard_NullObject::Raise("Vertex index is out of range");
      else
        aVertex = BRep_Tool::Pnt(TopoDS::Vertex(aShapeBase));
    }
    else if (aShapeBase.ShapeType() == TopAbs_EDGE) {
      if (index < 0 || index > 1)
        Standard_NullObject::Raise("Vertex index is out of range");

      TopoDS_Edge anEdgeE = TopoDS::Edge(aShapeBase);
      if ( anEdgeE.Orientation() != TopAbs_FORWARD && 
           anEdgeE.Orientation() != TopAbs_REVERSED )
        anEdgeE.Orientation( TopAbs_FORWARD );

      TopoDS_Vertex aV[2];
      TopExp::Vertices(anEdgeE, aV[0], aV[1], useOri );
      aVertex = BRep_Tool::Pnt( aV[ index ]);
    }
    else if (aShapeBase.ShapeType() == TopAbs_WIRE) {
      TopTools_IndexedMapOfShape anEdgeShapes;
      TopTools_IndexedMapOfShape aVertexShapes;
      TopoDS_Vertex aV1, aV2;
      TopoDS_Wire aWire = TopoDS::Wire(aShapeBase);
      TopExp_Explorer exp (aWire, TopAbs_EDGE);
      for (; exp.More(); exp.Next()) {
        anEdgeShapes.Add(exp.Current());
        TopoDS_Edge E = TopoDS::Edge(exp.Current());
        TopExp::Vertices(E, aV1, aV2);
        if ( aVertexShapes.Extent() == 0)
          aVertexShapes.Add(aV1);
        if ( !aV1.IsSame( aVertexShapes(aVertexShapes.Extent()) ) )
          aVertexShapes.Add(aV1);
        if ( !aV2.IsSame( aVertexShapes(aVertexShapes.Extent()) ) )
          aVertexShapes.Add(aV2);
      }

      if (index < 0 || index > aVertexShapes.Extent())
        Standard_NullObject::Raise("Vertex index is out of range");

      if ( aWire.Orientation() == TopAbs_FORWARD || !useOri )
        aVertex = BRep_Tool::Pnt(TopoDS::Vertex(aVertexShapes(index+1)));
      else
        aVertex = BRep_Tool::Pnt(TopoDS::Vertex(aVertexShapes(aVertexShapes.Extent() - index)));
    }
    else {
      Standard_NullObject::Raise("Shape for vertex calculation is not an edge or wire");
    }

    aShape = BRepBuilderAPI_MakeVertex(aVertex).Shape();
  }
  else if (aType == VECTOR_FACE_NORMALE)
  {
    // Face
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) {
      Standard_NullObject::Raise("Face for normale calculation is null");
    }
    if (aShapeBase.ShapeType() != TopAbs_FACE) {
      Standard_NullObject::Raise("Shape for normale calculation is not a face");
    }
    TopoDS_Face aFace = TopoDS::Face(aShapeBase);

    // Point
    gp_Pnt p1 (0,0,0);

    Handle(GEOM_Function) aPntFunc = aCI.GetPoint();
    if (!aPntFunc.IsNull())
    {
      TopoDS_Shape anOptPnt = aPntFunc->GetValue();
      if (anOptPnt.IsNull())
        Standard_NullObject::Raise("Invalid shape given for point argument");
      p1 = BRep_Tool::Pnt(TopoDS::Vertex(anOptPnt));
    }
    else
    {
      gp_Ax3 aPos = GEOMUtils::GetPosition(aFace);
      p1 = aPos.Location();
    }

    // Point parameters on surface
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
    Handle(ShapeAnalysis_Surface) aSurfAna = new ShapeAnalysis_Surface (aSurf);
    gp_Pnt2d pUV = aSurfAna->ValueOfUV(p1, Precision::Confusion());

    // Normal direction
    gp_Vec Vec1,Vec2;
    BRepAdaptor_Surface SF (aFace);
    SF.D1(pUV.X(), pUV.Y(), p1, Vec1, Vec2);
    if (Vec1.Magnitude() < Precision::Confusion()) {
      gp_Vec tmpV;
      gp_Pnt tmpP;
      SF.D1(pUV.X(), pUV.Y()-0.1, tmpP, Vec1, tmpV);
    }
    else if (Vec2.Magnitude() < Precision::Confusion()) {
      gp_Vec tmpV;
      gp_Pnt tmpP;
      SF.D1(pUV.X()-0.1, pUV.Y(), tmpP, tmpV, Vec2);
    }

    gp_Vec V = Vec1.Crossed(Vec2);
    Standard_Real mod = V.Magnitude();
    if (mod < Precision::Confusion())
      Standard_NullObject::Raise("Normal vector of a face has null magnitude");

    // Set length of normal vector to average radius of curvature
    Standard_Real radius = 0.0;
    GeomLProp_SLProps aProperties (aSurf, pUV.X(), pUV.Y(), 2, Precision::Confusion());
    if (aProperties.IsCurvatureDefined()) {
      Standard_Real radius1 = Abs(aProperties.MinCurvature());
      Standard_Real radius2 = Abs(aProperties.MaxCurvature());
      if (Abs(radius1) > Precision::Confusion()) {
        radius = 1.0 / radius1;
        if (Abs(radius2) > Precision::Confusion()) {
          radius = (radius + 1.0 / radius2) / 2.0;
        }
      }
      else {
        if (Abs(radius2) > Precision::Confusion()) {
          radius = 1.0 / radius2;
        }
      }
    }

    // Set length of normal vector to average dimension of the face
    // (only if average radius of curvature is not appropriate)
    if (radius < Precision::Confusion()) {
        Bnd_Box B;
        Standard_Real Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
        BRepBndLib::Add(aFace, B);
        B.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
        radius = ((Xmax - Xmin) + (Ymax - Ymin) + (Zmax - Zmin)) / 3.0;
    }

    if (radius < Precision::Confusion())
      radius = 1.0;

    V *= radius / mod;

    // consider the face orientation
    if (aFace.Orientation() == TopAbs_REVERSED ||
        aFace.Orientation() == TopAbs_INTERNAL) {
      V = - V;
    }

    // Edge
    gp_Pnt p2 = p1.Translated(V);
    BRepBuilderAPI_MakeEdge aBuilder (p1, p2);
    if (!aBuilder.IsDone())
      Standard_NullObject::Raise("Vector construction failed");
    aShape = aBuilder.Shape();
  }
  else if (aType == CURVATURE_VEC_MEASURE) {
    Handle(GEOM_Function) aSrfFunc = aCI.GetBase();
    Handle(GEOM_Function) aPntFunc = aCI.GetPoint();
    Handle(GEOM_Function) aDirFunc = aCI.GetDirection();

    TopoDS_Shape aFace   = aSrfFunc->GetValue();
    TopoDS_Shape aVertex = aPntFunc->GetValue();
    TopoDS_Shape anEdge  = aDirFunc->GetValue();

    aShape = EvaluateAlongCurvature(aFace, aVertex, anEdge);
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

  log->SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_MeasureDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMeasure aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case CDG_MEASURE:
    theOperationName = "MASS_CENTER";
    AddParam( theParams, "Object", aCI.GetBase() );
    break;
  case BND_BOX_MEASURE:
  case BND_BOX_MEASURE_PRECISE:
    theOperationName = "BND_BOX";
    AddParam( theParams, "Object", aCI.GetBase() );
    break;
  case VERTEX_BY_INDEX:
    theOperationName = "GetVertexByIndex";
    AddParam( theParams, "Object", aCI.GetBase() );
    AddParam( theParams, "Index", aCI.GetIndex() );
    AddParam( theParams, "Oriented", aCI.GetUseOri() );
    break;
  case VECTOR_FACE_NORMALE:
    theOperationName = "NORMALE";
    AddParam( theParams, "Face", aCI.GetBase() );
    AddParam( theParams, "Point", aCI.GetPoint(), "face center" );
    break;
  case CURVATURE_VEC_MEASURE:
    theOperationName = "CURVATURE";
    AddParam( theParams, "Face", aCI.GetBase() );
    AddParam( theParams, "Point", aCI.GetPoint(), "point of interest" );
    AddParam( theParams, "Vector", aCI.GetDirection(), "direction of interest" );
    break;
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_MeasureDriver,GEOM_BaseDriver)
