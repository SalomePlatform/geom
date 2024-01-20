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

#include <Basics_OCCTVersion.hxx>

#include <GEOMImpl_PointDriver.hxx>
#include <GEOMImpl_IPoint.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMAlgo_AlgoTools.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <IntTools.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <gp_Pnt.hxx>

#include <Precision.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_NotImplemented.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PointDriver::GetID()
{
  static Standard_GUID aPointDriver("FF1BBB02-5D14-4df2-980B-3A668264EA16");
  return aPointDriver;
}


//=======================================================================
//function : GEOMImpl_PointDriver
//purpose  :
//=======================================================================
GEOMImpl_PointDriver::GEOMImpl_PointDriver()
{
}

//=======================================================================
//function : getExtremaSolution
//purpose  : local function
//=======================================================================
static Standard_Boolean getExtremaSolution
(const gp_Pnt&       theInitPnt,
 const TopoDS_Shape& theRefShape,
 gp_Pnt& thePnt)
{
  BRepBuilderAPI_MakeVertex mkVertex (theInitPnt);
  TopoDS_Vertex anInitV = TopoDS::Vertex(mkVertex.Shape());
  
  BRepExtrema_DistShapeShape anExt (anInitV, theRefShape);
  if ( !anExt.IsDone() || anExt.NbSolution() < 1 )
    return Standard_False;
  thePnt = anExt.PointOnShape2(1);
  Standard_Real aMinDist2 = theInitPnt.SquareDistance( thePnt );
  for ( Standard_Integer j = 2, jn = anExt.NbSolution(); j <= jn; j++ )
  {
    gp_Pnt aPnt = anExt.PointOnShape2(j);
    Standard_Real aDist2 = theInitPnt.SquareDistance( aPnt );
    if ( aDist2 > aMinDist2)
      continue;
    aMinDist2 = aDist2;
    thePnt = aPnt;
  }
  return Standard_True;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PointDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if (Label().IsNull())  return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPoint aPI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  gp_Pnt aPnt;
  TopoDS_Compound aCompound;
  bool retCompound = false;

  if (aType == POINT_XYZ) {
    aPnt = gp_Pnt(aPI.GetX(), aPI.GetY(), aPI.GetZ());
  }
  else if (aType == POINT_XYZ_REF) {
    Handle(GEOM_Function) aRefFunc = aPI.GetRef();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    if (aRefShape.ShapeType() != TopAbs_VERTEX) {
      Standard_TypeMismatch::Raise
        ("Point creation aborted : referenced shape is not a vertex");
    }
    gp_Pnt P = BRep_Tool::Pnt(TopoDS::Vertex(aRefShape));
    aPnt = gp_Pnt(P.X() + aPI.GetX(), P.Y() + aPI.GetY(), P.Z() + aPI.GetZ());
  }
  else if (aType == POINT_CURVE_PAR) {
    Handle(GEOM_Function) aRefFunc = aPI.GetCurve();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    if (aRefShape.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Point On Curve creation aborted : curve shape is not an edge");
    }
    Standard_Real aFP, aLP, aP;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aRefShape), aFP, aLP);
    if ( !aCurve.IsNull() ) {
      if (aPI.GetTakeOrientationIntoAccount() &&
          aRefShape.Orientation() == TopAbs_REVERSED) {
        aP = 1. - aPI.GetParameter();
      } else {
        aP = aPI.GetParameter();
      }

      aP = aFP + (aLP - aFP) * aP;
      aPnt = aCurve->Value(aP);
    }
    else {
      // null curve, e.g. degenerated edge
      TopoDS_Iterator It(aRefShape, Standard_False, Standard_False);
      TopoDS_Vertex aVertex;
      if ( It.More() ) {
        TopoDS_Shape aShape = It.Value();
        if ( !aShape.IsNull() )
          aVertex = TopoDS::Vertex( aShape );
      }
      if ( !aVertex.IsNull() ) {
        aPnt = BRep_Tool::Pnt( aVertex );
      }
      else {
        Standard_TypeMismatch::Raise
          ("Point On Curve creation aborted : null curve");
      }
    }
  }
  else if (aType == POINT_CURVE_COORD) {
    Handle(GEOM_Function) aRefFunc = aPI.GetCurve();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    if (aRefShape.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Point On Curve creation aborted : curve shape is not an edge");
    }
    gp_Pnt anInitPnt (aPI.GetX(), aPI.GetY(), aPI.GetZ());
    if (!getExtremaSolution(anInitPnt, aRefShape, aPnt)) {
      Standard_ConstructionError::Raise
        ("Point On Curve creation aborted : cannot project point");
    }
  }
  else if (aType == POINT_CURVE_LENGTH) {
    // RefCurve
    Handle(GEOM_Function) aRefFunc = aPI.GetCurve();
    if (aRefFunc.IsNull()) {
      Standard_NullObject::Raise
        ("Point On Curve creation aborted : curve object is null");
    }
    TopoDS_Shape aRefShape1 = aRefFunc->GetValue();
    if (aRefShape1.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Point On Curve creation aborted : curve shape is not an edge");
    }
    TopoDS_Edge aRefEdge = TopoDS::Edge(aRefShape1);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(aRefEdge, V1, V2, Standard_True);

    // RefPoint
    TopoDS_Vertex aRefVertex;
    Handle(GEOM_Function) aRefPoint = aPI.GetRef();
    if (aRefPoint.IsNull()) {
      aRefVertex = V1;
    }
    else {
      TopoDS_Shape aRefShape2 = aRefPoint->GetValue();
      if (aRefShape2.ShapeType() != TopAbs_VERTEX) {
        Standard_TypeMismatch::Raise
          ("Point On Curve creation aborted : start point shape is not a vertex");
      }
      aRefVertex = TopoDS::Vertex(aRefShape2);
    }
    gp_Pnt aRefPnt = BRep_Tool::Pnt(aRefVertex);

    // Length
    Standard_Real aLength = aPI.GetLength();
    //Standard_Real theCurveLength = IntTools::Length(aRefEdge);
    //if (aLength > theCurveLength) {
    //  Standard_ConstructionError::Raise
    //    ("Point On Curve creation aborted : given length is greater than edges length");
    //}

    // Check orientation
    Standard_Real UFirst, ULast;
    Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(aRefEdge, UFirst, ULast);

    if ( !EdgeCurve.IsNull() ) {
      Handle(Geom_Curve) ReOrientedCurve = EdgeCurve;

      Standard_Real dU = ULast - UFirst;
      Standard_Real par1 = UFirst + 0.1 * dU;
      Standard_Real par2 = ULast  - 0.1 * dU;
      
      gp_Pnt P1 = EdgeCurve->Value(par1);
      gp_Pnt P2 = EdgeCurve->Value(par2);
      
      if (aRefPnt.SquareDistance(P2) < aRefPnt.SquareDistance(P1)) {
        ReOrientedCurve = EdgeCurve->Reversed();
        UFirst = EdgeCurve->ReversedParameter(ULast);
      }
      
      // Get the point by length
      GeomAdaptor_Curve AdapCurve = GeomAdaptor_Curve(ReOrientedCurve);
      GCPnts_AbscissaPoint anAbsPnt (AdapCurve, aLength, UFirst); 
      Standard_Real aParam = anAbsPnt.Parameter();
      aPnt = AdapCurve.Value(aParam);
    }
    else {
      // null curve, e.g. degenerated edge
      TopoDS_Iterator It(aRefEdge, Standard_False, Standard_False);
      TopoDS_Vertex aVertex;
      if ( It.More() ) {
        TopoDS_Shape aShape = It.Value();
        if ( !aShape.IsNull() )
          aVertex = TopoDS::Vertex( aShape );
      }
      if ( !aVertex.IsNull() ) {
        aPnt = BRep_Tool::Pnt( aVertex );
      }
      else {
        Standard_TypeMismatch::Raise
          ("Point On Curve creation aborted : null curve");
      }
    }
  }
  else if (aType == POINT_SURFACE_PAR) {
    Handle(GEOM_Function) aRefFunc = aPI.GetSurface();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    if (aRefShape.ShapeType() != TopAbs_FACE) {
      Standard_TypeMismatch::Raise
        ("Point On Surface creation aborted : surface shape is not a face");
    }
    TopoDS_Face F = TopoDS::Face(aRefShape);
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
    Standard_Real U1,U2,V1,V2;
    BRepTools::UVBounds(F,U1,U2,V1,V2);
    Standard_Real U = U1 + (U2-U1) * aPI.GetParameter();
    Standard_Real V = V1 + (V2-V1) * aPI.GetParameter2();
    aPnt = aSurf->Value(U,V);
  }
  else if (aType == POINT_SURFACE_COORD) {
    Handle(GEOM_Function) aRefFunc = aPI.GetSurface();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    if (aRefShape.ShapeType() != TopAbs_FACE) {
      Standard_TypeMismatch::Raise
        ("Point On Surface creation aborted : surface shape is not a face");
    }
    gp_Pnt anInitPnt (aPI.GetX(), aPI.GetY(), aPI.GetZ());
    if (!getExtremaSolution(anInitPnt, aRefShape, aPnt)) {
      Standard_ConstructionError::Raise
        ("Point On Surface creation aborted : cannot project point");
    }
  }
  else if (aType == POINT_FACE_ANY) {
    Handle(GEOM_Function) aRefFunc = aPI.GetSurface();
    TopoDS_Shape aRefShape = aRefFunc->GetValue();
    int aNbPnts = aPI.GetNumberOfPoints();
    if (aNbPnts < 1) {
      Standard_TypeMismatch::Raise
        ("Point On Surface creation aborted : number of points is zero or negative");
    }
    if (aRefShape.ShapeType() != TopAbs_FACE) {
      Standard_TypeMismatch::Raise
        ("Point On Surface creation aborted : surface shape is not a face");
    }
    TopoDS_Face F = TopoDS::Face(aRefShape);
    if (aNbPnts == 1)
    {
      gp_Pnt2d aP2d;
      GEOMAlgo_AlgoTools::PntInFace(F, aPnt, aP2d);
    }
    else
    {
#if OCC_VERSION_LARGE < 0x07050304
      Standard_NotImplemented::Raise("Point cloud creation aborted. Improper OCCT version: please, use OCCT 7.5.3p4 or newer.");
#else
      if (GEOMAlgo_AlgoTools::PointCloudInFace(F, aNbPnts, aCompound) < 0)
        Standard_ConstructionError::Raise("Point cloud creation aborted : algorithm failed");
      retCompound = true;
#endif
    }
  }
  else if (aType == POINT_LINES_INTERSECTION) {
    Handle(GEOM_Function) aRef1 = aPI.GetLine1();
    Handle(GEOM_Function) aRef2 = aPI.GetLine2();

    TopoDS_Shape aRefShape1 = aRef1->GetValue();
    TopoDS_Shape aRefShape2 = aRef2->GetValue();

    if ( (aRefShape1.ShapeType() != TopAbs_EDGE && aRefShape1.ShapeType() != TopAbs_WIRE)
      || (aRefShape2.ShapeType() != TopAbs_EDGE && aRefShape2.ShapeType() != TopAbs_WIRE) ) {
      Standard_TypeMismatch::Raise
        ("Creation Point On Lines Intersection Aborted : Line shape is not an edge or wire");
    }

    if (aRefShape1.IsSame(aRefShape2))
      Standard_ConstructionError::Raise("The lines to make intersection must be different");

    //Calculate Lines Intersection Point
    BRepExtrema_DistShapeShape dst (aRefShape1, aRefShape2);
    if (dst.IsDone()) {
      gp_Pnt P1, P2;
      BRep_Builder B;
      B.MakeCompound( aCompound );
      for (int i = 1; i <= dst.NbSolution(); i++) {
        P1 = dst.PointOnShape1(i);
        P2 = dst.PointOnShape2(i);
        Standard_Real Dist = P1.Distance(P2);
        if ( Dist <= Precision::Confusion() && dst.NbSolution() > 1) {
          BRepBuilderAPI_MakeVertex mkVertex (P1);
          B.Add(aCompound, mkVertex.Shape());
          retCompound = true;
        } else if ( Dist <= Precision::Confusion() ) {
          aPnt = P1;
        } else {
          Standard_TypeMismatch::Raise ("Shapes have not an Intersection Point");
        }
      }
    }
  }
  else {
    return 0;
  }

  TopoDS_Shape aShape;
  if ( retCompound ) {
    aShape = aCompound;
  } else {
    BRepBuilderAPI_MakeVertex mkVertex (aPnt);
    aShape = mkVertex.Shape();
  }

  //aShape.Infinite(Standard_True); // VSR: 05/04/2010: Fix 20668 (Fit All for points & lines)
  aFunction->SetValue(aShape);

  log->SetTouched(Label());

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_PointDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPoint aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "POINT";

  switch ( aType ) {
  case POINT_XYZ:
    AddParam( theParams, "X", aCI.GetX() );
    AddParam( theParams, "Y", aCI.GetY() );
    AddParam( theParams, "Z", aCI.GetZ() );
    break;
  case POINT_XYZ_REF:
    AddParam( theParams, "Point", aCI.GetRef() );
    AddParam( theParams, "Dx", aCI.GetX() );
    AddParam( theParams, "Dy", aCI.GetY() );
    AddParam( theParams, "Dz", aCI.GetZ() );
    break;
  case POINT_CURVE_PAR:
    AddParam( theParams, "Edge", aCI.GetCurve() );
    AddParam( theParams, "Parameter", aCI.GetParameter() );
    AddParam( theParams, "Use Orientation", aCI.GetTakeOrientationIntoAccount() );
    break;
  case POINT_CURVE_COORD:
    AddParam( theParams, "X", aCI.GetX() );
    AddParam( theParams, "Y", aCI.GetY() );
    AddParam( theParams, "Z", aCI.GetZ() );
    AddParam( theParams, "Edge", aCI.GetCurve() );
    break;
  case POINT_CURVE_LENGTH:
    AddParam( theParams, "Edge", aCI.GetCurve() );
    AddParam( theParams, "Start Point", aCI.GetRef(), "First Vertex" );
    AddParam( theParams, "Length", aCI.GetLength() );
    break;
  case POINT_SURFACE_PAR:
    AddParam( theParams, "Face", aCI.GetSurface() );
    AddParam( theParams, "U-Parameter", aCI.GetParameter() );
    AddParam( theParams, "V-Parameter", aCI.GetParameter2() );
    break;
  case POINT_SURFACE_COORD:
    AddParam( theParams, "X", aCI.GetX() );
    AddParam( theParams, "Y", aCI.GetY() );
    AddParam( theParams, "Z", aCI.GetZ() );
    AddParam( theParams, "Face", aCI.GetSurface() );
    break;
  case POINT_FACE_ANY:
    AddParam( theParams, "Face", aCI.GetSurface() );
    break;
  case POINT_LINES_INTERSECTION:
    AddParam( theParams, "Line 1", aCI.GetLine1() );
    AddParam( theParams, "Line 2", aCI.GetLine2() );
    break;
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PointDriver,GEOM_BaseDriver)
