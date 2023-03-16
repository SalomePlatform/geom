// Copyright (C) 2022-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOMImpl_ShapeProximityDriver.hxx>
#include <GEOMImpl_IProximity.hxx>
#include <GEOMImpl_Types.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenLocateExtCS.hxx>
#include <Extrema_GenLocateExtSS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_LocateExtCC.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

namespace {
  static void tessellateShape(const TopoDS_Shape& theShape)
  {
    Standard_Boolean isTessellate = Standard_False;
    TopLoc_Location aLoc;
    for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More() && !isTessellate; anExp.Next())
    {
      Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation(TopoDS::Face(anExp.Value()), aLoc);
      isTessellate = aTria.IsNull();
    }
    for (TopExp_Explorer anExp(theShape, TopAbs_EDGE); anExp.More() && !isTessellate; anExp.Next())
    {
      Handle(Poly_Polygon3D) aPoly = BRep_Tool::Polygon3D(TopoDS::Edge(anExp.Value()), aLoc);
      isTessellate = aPoly.IsNull();
    }

    if (isTessellate)
    {
      BRepMesh_IncrementalMesh aMesher(theShape, 0.1);
      Standard_ProgramError_Raise_if(!aMesher.IsDone(), "Meshing failed");
    }
  }

  static Standard_Real paramOnCurve(const BRepAdaptor_Curve& theCurve, const gp_Pnt& thePoint, const Standard_Real theTol)
  {
    Extrema_ExtPC aParamSearch(thePoint, theCurve, theCurve.FirstParameter(), theCurve.LastParameter());
    if (aParamSearch.IsDone())
    {
      Standard_Integer anIndMin = 0, aNbExt = aParamSearch.NbExt();
      Standard_Real aSqDistMin = RealLast();
      for (Standard_Integer i = 1; i <= aNbExt; ++i)
      {
        if (aParamSearch.SquareDistance(i) < aSqDistMin)
        {
          anIndMin = i;
          aSqDistMin = aParamSearch.SquareDistance(i);
        }
      }
      if (anIndMin != 0 && aSqDistMin <= theTol * theTol)
      {
        return aParamSearch.Point(anIndMin).Parameter();
      }
    }
    return 0.5 * (theCurve.FirstParameter() + theCurve.LastParameter());
  }

  static void paramsOnSurf(const BRepAdaptor_Surface& theSurf, const gp_Pnt& thePoint, const Standard_Real theTol,
                           Standard_Real& theU, Standard_Real& theV)
  {
    Extrema_ExtPS aParamSearch(thePoint, theSurf, Precision::PConfusion(), Precision::PConfusion());
    if (aParamSearch.IsDone())
    {
      Standard_Integer anIndMin = 0, aNbExt = aParamSearch.NbExt();
      Standard_Real aSqDistMin = RealLast();
      for (Standard_Integer i = 1; i <= aNbExt; ++i)
      {
        if (aParamSearch.SquareDistance(i) < aSqDistMin)
        {
          anIndMin = i;
          aSqDistMin = aParamSearch.SquareDistance(i);
        }
      }
      if (anIndMin != 0 && aSqDistMin <= theTol * theTol)
      {
        return aParamSearch.Point(anIndMin).Parameter(theU, theV);
      }
    }
    theU = 0.5 * (theSurf.FirstUParameter() + theSurf.LastUParameter());
    theV = 0.5 * (theSurf.FirstVParameter() + theSurf.LastVParameter());
  }

  static Standard_Real extremaEE(const TopoDS_Edge& theEdge1, const TopoDS_Edge& theEdge2,
                                 gp_Pnt& thePoint1, gp_Pnt& thePoint2)
  {
    BRepAdaptor_Curve aCurve1(theEdge1);
    BRepAdaptor_Curve aCurve2(theEdge2);

    TopLoc_Location aLoc;
    Standard_Real aTol1 = BRep_Tool::Tolerance(theEdge1);
    Handle(Poly_Polygon3D) aPoly1 = BRep_Tool::Polygon3D (theEdge1, aLoc);
    if (!aPoly1.IsNull())
      aTol1 = Max (aTol1, aPoly1->Deflection());
    Standard_Real aTol2 = BRep_Tool::Tolerance(theEdge2);
    Handle(Poly_Polygon3D) aPoly2 = BRep_Tool::Polygon3D (theEdge2, aLoc);
    if (!aPoly2.IsNull())
      aTol2 = Max (aTol2, aPoly2->Deflection());
  
    Standard_Real aU1 = paramOnCurve(aCurve1, thePoint1, 2*aTol1);
    Standard_Real aU2 = paramOnCurve(aCurve2, thePoint2, 2*aTol2);

    Standard_Real aValue = -1.0;
    Extrema_LocateExtCC anExtr(aCurve1, aCurve2, aU1, aU2);
    if (anExtr.IsDone())
    {
      aValue = Sqrt(anExtr.SquareDistance());

      Extrema_POnCurv aP1, aP2;
      anExtr.Point(aP1, aP2);
      thePoint1 = aP1.Value();
      thePoint2 = aP2.Value();
    }
    return aValue;
  }

  static Standard_Real extremaPE(const gp_Pnt&      thePoint,
                                 const TopoDS_Edge& theEdge,
                                 gp_Pnt&            thePointOnEdge)
  {
    BRepAdaptor_Curve aCurve (theEdge);
    
    TopLoc_Location aLoc;
    Standard_Real aTol = BRep_Tool::Tolerance(theEdge);
    Handle(Poly_Polygon3D) aPoly = BRep_Tool::Polygon3D (theEdge, aLoc);
    if (!aPoly.IsNull())
      aTol = Max (aTol, aPoly->Deflection());
    
    Standard_Real aParam = paramOnCurve (aCurve, thePointOnEdge, 2*aTol);
    
    Standard_Real aValue = -1.0;
    Extrema_LocateExtPC anExtr (thePoint, aCurve, aParam, Precision::PConfusion());
    if (anExtr.IsDone())
    {
      aValue = Sqrt(anExtr.SquareDistance());
      
      Extrema_POnCurv aPointOnCurve = anExtr.Point();
      thePointOnEdge = aPointOnCurve.Value();
    }
    return aValue;
  }
  
  static Standard_Real extremaPF(const gp_Pnt&      thePoint,
                                 const TopoDS_Face& theFace,
                                 gp_Pnt&            thePointOnFace)
  {
    BRepAdaptor_Surface aSurf (theFace);
    
    TopLoc_Location aLoc;
    Standard_Real aTol = BRep_Tool::Tolerance(theFace);
    Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation (theFace, aLoc);
    if (!aTria.IsNull())
      aTol = Max (aTol, aTria->Deflection());
    
    Standard_Real aU, aV;
    paramsOnSurf(aSurf, thePointOnFace, 2*aTol, aU, aV);
    
    Standard_Real aValue = -1.0;
    Extrema_GenLocateExtPS anExtr (aSurf);
    anExtr.Perform (thePoint, aU, aV);
    if (anExtr.IsDone())
    {
      aValue = Sqrt(anExtr.SquareDistance());
      
      Extrema_POnSurf aPointOnSurf = anExtr.Point();
      thePointOnFace = aPointOnSurf.Value();
    }
    return aValue;
  }

  static Standard_Real extremaEF(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace,
                                 gp_Pnt& thePonE, gp_Pnt& thePonF)
  {
    BRepAdaptor_Curve aCurve(theEdge);
    BRepAdaptor_Surface aSurf(theFace);

    TopLoc_Location aLoc;
    Standard_Real aTolEdge = BRep_Tool::Tolerance(theEdge);
    Handle(Poly_Polygon3D) aPoly = BRep_Tool::Polygon3D (theEdge, aLoc);
    if (!aPoly.IsNull())
      aTolEdge = Max (aTolEdge, aPoly->Deflection());
    Standard_Real aTolFace = BRep_Tool::Tolerance(theFace);
    Handle(Poly_Triangulation) aTria = BRep_Tool::Triangulation (theFace, aLoc);
    if (!aTria.IsNull())
      aTolFace = Max (aTolFace, aTria->Deflection());
  
    Standard_Real aP = paramOnCurve(aCurve, thePonE, 2*aTolEdge);
    Standard_Real aU, aV;
    paramsOnSurf(aSurf, thePonF, 2*aTolFace, aU, aV);

    Standard_Real aValue = -1.0;
    Extrema_GenLocateExtCS anExtr(aCurve, aSurf, aP, aU, aV, Precision::PConfusion(), Precision::PConfusion());
    if (anExtr.IsDone())
    {
      aValue = Sqrt(anExtr.SquareDistance());
      thePonE = anExtr.PointOnCurve().Value();
      thePonF = anExtr.PointOnSurface().Value();
    }
    return aValue;
  }

  static Standard_Real extremaFF(const TopoDS_Face& theFace1, const TopoDS_Face& theFace2,
                                 gp_Pnt& thePoint1, gp_Pnt& thePoint2)
  {
    BRepAdaptor_Surface aSurf1(theFace1);
    BRepAdaptor_Surface aSurf2(theFace2);

    TopLoc_Location aLoc;
    Standard_Real aTol1 = BRep_Tool::Tolerance(theFace1);
    Handle(Poly_Triangulation) aTria1 = BRep_Tool::Triangulation (theFace1, aLoc);
    if (!aTria1.IsNull())
      aTol1 = Max (aTol1, aTria1->Deflection());
    Standard_Real aTol2 = BRep_Tool::Tolerance(theFace2);
    Handle(Poly_Triangulation) aTria2 = BRep_Tool::Triangulation (theFace2, aLoc);
    if (!aTria2.IsNull())
      aTol2 = Max (aTol2, aTria2->Deflection());
  
    Standard_Real aU1, aV1;
    paramsOnSurf(aSurf1, thePoint1, 2*aTol1, aU1, aV1);
    Standard_Real aU2, aV2;
    paramsOnSurf(aSurf2, thePoint2, 2*aTol2, aU2, aV2);

    Standard_Real aValue = -1.0;
    Extrema_GenLocateExtSS anExtr(aSurf1, aSurf2, aU1, aV1, aU2, aV2, Precision::PConfusion(), Precision::PConfusion());
    if (anExtr.IsDone())
    {
      aValue = Sqrt(anExtr.SquareDistance());
      thePoint1 = anExtr.PointOnS1().Value();
      thePoint2 = anExtr.PointOnS2().Value();
    }
    return aValue;
  }
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ShapeProximityDriver::GetID()
{
  static Standard_GUID aShapeProximityDriver("1C3449A6-E4EB-407D-B623-89261C4BA785");
  return aShapeProximityDriver;
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ShapeProximityDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if (Label().IsNull())
    return 0;

  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  GEOMImpl_IProximity aProximity (aFunction);

  Handle(GEOM_Function) aShapeFunc1, aShapeFunc2;
  aProximity.GetShapes(aShapeFunc1, aShapeFunc2);
  if (aShapeFunc1.IsNull() || aShapeFunc2.IsNull())
    return 0;

  TopoDS_Shape aShape1 = aShapeFunc1->GetValue();
  TopoDS_Shape aShape2 = aShapeFunc2->GetValue();

  Standard_Real aValue = -1.0;

  if (aFunction->GetType() == PROXIMITY_COARSE)
  {
    // tessellate shapes if there is no mesh exists
    tessellateShape(aShape1);
    tessellateShape(aShape2);

    // compute proximity basing on the tessellation
    BRepExtrema_ShapeProximity aCalcProx;
    aCalcProx.LoadShape1(aShape1);
    aCalcProx.LoadShape2(aShape2);
    aCalcProx.SetNbSamples1(aProximity.GetNbSamples(PROXIMITY_ARG_SAMPLES1));
    aCalcProx.SetNbSamples2(aProximity.GetNbSamples(PROXIMITY_ARG_SAMPLES2));
    aCalcProx.Perform();

    if (aCalcProx.IsDone())
    {
      aValue = aCalcProx.Proximity();
      aProximity.SetProximityPoints(aCalcProx.ProximityPoint1(),
                                    aCalcProx.ProximityPoint2());
      aProximity.SetStatusOfPoints((Standard_Integer)aCalcProx.ProxPntStatus1(),
                                   (Standard_Integer)aCalcProx.ProxPntStatus2());
    }
    else
      Standard_ConstructionError::Raise("Failed to compute coarse proximity");
  }
  else if (aFunction->GetType() == PROXIMITY_PRECISE)
  {
    // coarse proximity value
    // in some cases this value cannot be precised
    // it can be precised only if at least one point is in the middle of the shape
    aValue = aProximity.GetValue();

    TopAbs_ShapeEnum aType1 = aShape1.ShapeType();
    TopAbs_ShapeEnum aType2 = aShape2.ShapeType();

    gp_Pnt aPnt1, aPnt2;
    BRepExtrema_ProximityDistTool::ProxPnt_Status aStatus1, aStatus2;
    Standard_Integer intStatus1, intStatus2;
    aProximity.GetProximityPoints(aPnt1, aPnt2);
    aProximity.GetStatusOfPoints(intStatus1, intStatus2);
    aStatus1 = (BRepExtrema_ProximityDistTool::ProxPnt_Status)intStatus1;
    aStatus2 = (BRepExtrema_ProximityDistTool::ProxPnt_Status)intStatus2;

    if (aType1 == TopAbs_EDGE)
    {
      if (aType2 == TopAbs_EDGE)
      {
        if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
            aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaEE(TopoDS::Edge(aShape1), TopoDS::Edge(aShape2), aPnt1, aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaPE(aPnt1, TopoDS::Edge(aShape2), aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER)
        {
          aValue = extremaPE(aPnt2, TopoDS::Edge(aShape1), aPnt1);
        }
      }
      else if (aType2 == TopAbs_FACE)
      {
        if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
            aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaEF(TopoDS::Edge(aShape1), TopoDS::Face(aShape2), aPnt1, aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaPF(aPnt1, TopoDS::Face(aShape2), aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER)
        {
          aValue = extremaPE(aPnt2, TopoDS::Edge(aShape1), aPnt1);
        }
      }
    }
    else if (aType1 == TopAbs_FACE)
    {
      if (aType2 == TopAbs_EDGE)
      {
        if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
            aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaEF(TopoDS::Edge(aShape2), TopoDS::Face(aShape1), aPnt2, aPnt1);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaPE(aPnt1, TopoDS::Edge(aShape2), aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER)
        {
          aValue = extremaPF(aPnt2, TopoDS::Face(aShape1), aPnt1);
        }
      }
      else if (aType2 == TopAbs_FACE)
      {
        if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
            aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaFF(TopoDS::Face(aShape1), TopoDS::Face(aShape2), aPnt1, aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE)
        {
          aValue = extremaPF(aPnt1, TopoDS::Face(aShape2), aPnt2);
        }
        else if (aStatus1 == BRepExtrema_ProximityDistTool::ProxPnt_Status_MIDDLE &&
                 aStatus2 == BRepExtrema_ProximityDistTool::ProxPnt_Status_BORDER)
        {
          aValue = extremaPF(aPnt2, TopoDS::Face(aShape1), aPnt1);
        }
      }
    }

    if (aValue >= 0)
      aProximity.SetProximityPoints(aPnt1, aPnt2);
    else
      Standard_ConstructionError::Raise("Failed to compute precise proximity");
  }
  else
  {
    Standard_ConstructionError::Raise("incorrect algorithm");
  }

  aProximity.SetValue(aValue);
  log->SetTouched(Label());
  return 1;
}

//=======================================================================
//function : GetCreationInformation
//purpose  : Returns a name of creation operation and names and values of creation parameters
//=======================================================================
bool GEOMImpl_ShapeProximityDriver::GetCreationInformation(
    std::string&             theOperationName,
    std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull())
    return false;

  Handle(GEOM_Function) aFunc = GEOM_Function::GetFunction(Label());
  GEOMImpl_IProximity aProxFunc(aFunc);
  Handle(GEOM_Function) aShape1, aShape2;
  aProxFunc.GetShapes(aShape1, aShape2);

  if (aFunc->GetType() == PROXIMITY_COARSE)
    theOperationName = "PROXIMITY_COARSE";
  else if (aFunc->GetType() == PROXIMITY_PRECISE)
    theOperationName = "PROXIMITY_PRECISE";

  AddParam(theParams, "Shape1", aShape1);
  AddParam(theParams, "Shape2", aShape2);

  return false;
}

IMPLEMENT_STANDARD_RTTIEXT(GEOMImpl_ShapeProximityDriver, GEOM_BaseDriver)
