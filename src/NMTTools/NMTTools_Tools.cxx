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
// File:	NMTTools_Tools.cxx
// Created:	Mon Dec  8 10:35:15 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_Tools.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt2d.hxx>

#include <Geom_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include <TopExp.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <BOPTools_VVInterference.hxx>
#include <BOPTools_SSInterference.hxx>

#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools.hxx>
#include <NMTTools_ListIteratorOfListOfCoupleOfShape.hxx>
#include <NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx>
#include <NMTTools_CoupleOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BRepLib.hxx>
#include <BOPTools_Tools3D.hxx>
#include <TopExp_Explorer.hxx>

static 
  void ProcessBlock(const Standard_Integer iV,
		    const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMCV,
		    TColStd_IndexedMapOfInteger& aProcessed,
		    TColStd_IndexedMapOfInteger& aChain);
static
  void ProcessBlock(const TopoDS_Shape& aF,
		    const NMTTools_IndexedDataMapOfShapeIndexedMapOfShape& aMCV,
		    TopTools_IndexedMapOfShape& aProcessed,
		    TopTools_IndexedMapOfShape& aChain);

//=======================================================================
// function: MakePCurve
// purpose: 
//=======================================================================
  void  NMTTools_Tools::MakePCurve(const TopoDS_Edge& aE,
				   const TopoDS_Face& aF,
				   const Handle(Geom2d_Curve)& aC2Dx,
				   const Standard_Real aTolR2D)
{
  Standard_Integer k, aNbV;   
  Standard_Real aTolEdge, aTolFact, aTolV, aTolVmax;
  Standard_Real aTFirst, aTLast, aOutFirst, aOutLast, aOutTol;
  TopoDS_Face aFFWD;
  TopTools_IndexedMapOfShape aVMap;
  BRep_Builder aBB;
  //
  aFFWD=aF;
  aFFWD.Orientation(TopAbs_FORWARD);
  //
  aTolEdge=BRep_Tool::Tolerance(aE);
  aTolFact=Max(aTolEdge, aTolR2D);
  //
  TopExp::MapShapes(aE, TopAbs_VERTEX, aVMap);
  //
  aTolVmax=-1.;
  aNbV=aVMap.Extent();
  for (k=1; k<=aNbV; ++k) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aVMap(k));
    aTolV=BRep_Tool::Tolerance(aV);
    if (aTolV>aTolVmax) {
      aTolVmax=aTolV;
    }
  }
  //
  if (aTolFact>aTolVmax) {
    aTolFact=aTolVmax;
  }
  //
  const Handle(Geom_Curve)& aC3DE=BRep_Tool::Curve(aE, aTFirst, aTLast);
  Handle(Geom_TrimmedCurve)aC3DETrim=new Geom_TrimmedCurve(aC3DE, aTFirst, aTLast);
  //
  Handle(Geom2d_Curve) aC2D, aC2DA;
  //
  aC2D=aC2Dx;
  if (aC2D.IsNull()) {
    BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aFFWD);
    BOPTools_Tools2D::CurveOnSurface(aE, aFFWD, aC2D, aOutFirst, aOutLast, aOutTol, Standard_True);
  }
  if (aC3DE->IsPeriodic()) {
    BOPTools_Tools2D::AdjustPCurveOnFace(aFFWD, aTFirst, aTLast,  aC2D, aC2DA); 
  }
  else {
    BOPTools_Tools2D::AdjustPCurveOnFace(aFFWD, aC3DETrim, aC2D, aC2DA); 
  }
  //
  aBB.UpdateEdge(aE, aC2DA, aFFWD, aTolFact);
  BRepLib::SameParameter(aE);
}
  
//=======================================================================
// function: IsSplitInOnFace
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_Tools::IsSplitInOnFace(const TopoDS_Edge& aE,
						   const TopoDS_Face& aF,
						   IntTools_Context& aContext)
{
  Standard_Boolean bFlag;
  Standard_Real aT, aTolE, aTolF, aTol, aDist, aU, aV;
  gp_Pnt aP;
  gp_Pnt2d aP2D;
  //
  aTolE=BRep_Tool::Tolerance(aE);
  aTolF=BRep_Tool::Tolerance(aF);
  aTol=aTolE+aTolF;
  //
  GeomAPI_ProjectPointOnSurf& aProjector=aContext.ProjPS(aF);
  //
  aT=BOPTools_Tools2D::IntermediatePoint(aE);
  BOPTools_Tools::PointOnEdge(aE, aT, aP);
  //
  aProjector.Perform(aP);
  bFlag=aProjector.IsDone();
  if (!bFlag) {
    return bFlag;
  }
  //
  aDist=aProjector.LowerDistance();
  bFlag=(aDist <= aTol);
  if (!bFlag) {
    return bFlag;
  }
  //
  aProjector.LowerDistanceParameters(aU, aV);
  aP2D.SetCoord(aU, aV);
  bFlag=aContext.IsPointInOnFace (aF, aP2D);
  return bFlag;
}
//=======================================================================
// function: NMTTools_Tools::MakeNewVertex
// purpose : 
//=======================================================================
  void NMTTools_Tools::MakeNewVertex(const TopTools_ListOfShape& aLVs,
				     TopoDS_Vertex& aNewVertex)
{
  Standard_Integer aNb;
  Standard_Real aTi, aDi, aDmax=-1.e5;
  gp_Pnt aPi, aP;
  gp_XYZ aXYZ(0.,0.,0.), aXYZi;
  TopTools_ListIteratorOfListOfShape anIt;
  //
  aNb=aLVs.Extent();
  if (!aNb) {
    return;
  }
  //
  anIt.Initialize(aLVs);
  for (; anIt.More(); anIt.Next()) {
    TopoDS_Vertex aVi=TopoDS::Vertex(anIt.Value());
    aPi=BRep_Tool::Pnt(aVi);
    aXYZi=aPi.XYZ();
    aXYZ=aXYZ+aXYZi;
  }
  //
  aXYZ.Divide((Standard_Real)aNb);
  aP.SetXYZ(aXYZ);
  //
  anIt.Initialize(aLVs);
  for (; anIt.More(); anIt.Next()) {
    TopoDS_Vertex aVi=TopoDS::Vertex(anIt.Value());
    aPi=BRep_Tool::Pnt(aVi);
    aTi=BRep_Tool::Tolerance(aVi);
    aDi=aP.Distance(aPi);
    aDi=aDi+aTi;
    if (aDi > aDmax) {
      aDmax=aDi;
    }
  }
  BRep_Builder aBB;
  aBB.MakeVertex (aNewVertex, aP, aDmax);
}
//=======================================================================
// function: FindChains
// purpose : 
//=======================================================================
  void NMTTools_Tools::FindChains(const BOPTools_CArray1OfSSInterference& FFs,
				  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapChains)
{
  Standard_Boolean bIsTangentFaces;
  Standard_Integer j, aNb, anIndex1, anIndex2;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aMCV;
  //
  aNb=FFs.Extent();
  for (j=1; j<=aNb; ++j) {
    const BOPTools_SSInterference& aFF=FFs(j);
    //
    bIsTangentFaces=aFF.IsTangentFaces();
    if (!bIsTangentFaces) {
      continue;
    }
    //
    aFF.Indices(anIndex1, anIndex2);
    //
    if (aMCV.Contains(anIndex1)) {
      TColStd_IndexedMapOfInteger& aMV=aMCV.ChangeFromKey(anIndex1);
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
    }
    else {
      TColStd_IndexedMapOfInteger aMV;
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
      aMCV.Add(anIndex1, aMV);
    }
    //
    if (aMCV.Contains(anIndex2)) {
      TColStd_IndexedMapOfInteger& aMV=aMCV.ChangeFromKey(anIndex2);
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
    }
    else {
      TColStd_IndexedMapOfInteger aMV;
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
      aMCV.Add(anIndex2, aMV);
    }
  }
  NMTTools_Tools::FindChains(aMCV, aMapChains);
}
//=======================================================================
// function: FindChains
// purpose : 
//=======================================================================
  void NMTTools_Tools::FindChains(const BOPTools_CArray1OfVVInterference& VVs,
				  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapChains)
{
  Standard_Integer j, aNb, anIndex1, anIndex2;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aMCV;
  //
  aNb=VVs.Extent();
  for (j=1; j<=aNb; ++j) {
    const BOPTools_VVInterference& VV=VVs(j);
    VV.Indices(anIndex1, anIndex2);
    //
    if (aMCV.Contains(anIndex1)) {
      TColStd_IndexedMapOfInteger& aMV=aMCV.ChangeFromKey(anIndex1);
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
    }
    else {
      TColStd_IndexedMapOfInteger aMV;
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
      aMCV.Add(anIndex1, aMV);
    }
    //
    if (aMCV.Contains(anIndex2)) {
      TColStd_IndexedMapOfInteger& aMV=aMCV.ChangeFromKey(anIndex2);
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
    }
    else {
      TColStd_IndexedMapOfInteger aMV;
      aMV.Add(anIndex1);
      aMV.Add(anIndex2);
      aMCV.Add(anIndex2, aMV);
    }
  }
  NMTTools_Tools::FindChains(aMCV, aMapChains);
}

//=======================================================================
// function: FindChains
// purpose : 
//=======================================================================
  void NMTTools_Tools::FindChains(const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMCV,
				  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapChains)
{
  Standard_Integer  i, j, aNbCV, aNbV, iV, iVx;
  TColStd_IndexedMapOfInteger aProcessed, aChain;
  //
  aNbCV=aMCV.Extent();
  for (i=1; i<=aNbCV; ++i) {
    iV=aMCV.FindKey(i);
    if (aProcessed.Contains(iV)) {
      continue;
    }
    //
    aProcessed.Add(iV);
    aChain.Add(iV);
    //
    const TColStd_IndexedMapOfInteger& aMV=aMCV(i);
    aNbV=aMV.Extent();
    for (j=1; j<=aNbV; ++j) {
      iVx=aMV(j);
      ProcessBlock(iVx, aMCV, aProcessed, aChain);
    }
    aMapChains.Add(i, aChain);
    aChain.Clear();
  }
}
//=======================================================================
// function: ProcessBlock
// purpose: 
//=======================================================================
void ProcessBlock(const Standard_Integer iV,
		  const BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMCV,
		  TColStd_IndexedMapOfInteger& aProcessed,
		  TColStd_IndexedMapOfInteger& aChain)
{
  Standard_Integer j, aNbV, iVx;
  //
  if (aProcessed.Contains(iV)) {
    return;
  }
  aProcessed.Add(iV);
  aChain.Add(iV);
  //
  const TColStd_IndexedMapOfInteger& aMV=aMCV.FindFromKey(iV);
  aNbV=aMV.Extent();
  for (j=1; j<=aNbV; ++j) {
    iVx=aMV(j);
    ProcessBlock(iVx, aMCV, aProcessed, aChain);
  }
}
//=======================================================================
// function: AreFacesSameDomain
// purpose : 
//=======================================================================
  Standard_Boolean NMTTools_Tools::AreFacesSameDomain(const TopoDS_Face& aF1x,
						      const TopoDS_Face& aF2y,
						      IntTools_Context& aCtx)
{
  Standard_Boolean bFlag;
  Standard_Integer i, aNbE1, aNbE2;
  
  TopTools_IndexedMapOfShape aME1, aME2;
  TopoDS_Edge aEF1, aEF2;
  TopoDS_Face aF1, aF2;
  //
  aF1=aF1x;
  aF1.Orientation(TopAbs_FORWARD);
  aF2=aF2y;
  aF2.Orientation(TopAbs_FORWARD);
  //
  TopExp::MapShapes(aF1, TopAbs_EDGE, aME1);
  TopExp::MapShapes(aF2, TopAbs_EDGE, aME2);
  //
  bFlag=Standard_False;
  //
  aNbE1=aME1.Extent();
  aNbE2=aME2.Extent();
  //
  if(!aNbE1 || !aNbE2){
    return bFlag;
  }
  //
  if(aNbE1!=aNbE2) {
    return bFlag;
  }
  //
  for (i=1; i<=aNbE1; ++i) {
    const TopoDS_Edge& aE1=TopoDS::Edge(aME1(i));
    if (BRep_Tool::Degenerated(aE1)) {
      // not try to compare deg edges because it 
      // can not have same TShape on different faces at all
      continue; 
    }
    if (!aME2.Contains(aE1)) {
      return bFlag;
    }
  }
  //
  Standard_Real aTolF1, aTolF2, aTol;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  TopExp_Explorer anExp;
  //
  aTolF1=BRep_Tool::Tolerance(aF1);
  aTolF2=BRep_Tool::Tolerance(aF2);
  aTol=aTolF1+aTolF2;
  //
  anExp.Init(aF1, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& aE1=TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(aE1)) {
      BOPTools_Tools3D::PointNearEdge(aE1, aF1, aP2D, aP);
      bFlag=aCtx.IsValidPointForFace(aP, aF2, aTol);
      break;
    }
  }
  return bFlag;
  /*
  //
  Standard_Real aU1, aU2, aV1, aV2;
  Standard_Real dU, dV, aU, aV;
  Standard_Integer aNbP=5, aNbP1, j;
  gp_Pnt2d aP2D;
  gp_Pnt aP;
  //
  aTolF1=BRep_Tool::Tolerance(aF1);
  aTolF2=BRep_Tool::Tolerance(aF2);
  aTol=aTolF1+aTolF2;
  //
  BRepTools::UVBounds(aF1, aU1, aU2, aV1, aV2);
  Handle(Geom_Surface) aS1=BRep_Tool::Surface(aF1);
  //
  aNbP1=aNbP+1;
  dU=(aU2-aU1)/aNbP1;
  dV=(aV2-aV1)/aNbP1;
  //
  for (i=1; i<=aNbP; ++i) {
    aU=aU1+i*dU;
    for (j=1; j<=aNbP; ++j) {
      aV=aV1+j*dV;
      aP2D.SetCoord(aU, aV);
      //
      if(aCtx.IsPointInFace(aF1, aP2D)) {
	aP=aS1->Value(aU, aV);
	bFlag=aCtx.IsValidPointForFace(aP, aF2, aTol);
	if (!bFlag) {
	  return bFlag;
	}
      }
    }
  }
  */
  //
  return bFlag;
}
//=======================================================================
// function: FindChains
// purpose : 
//=======================================================================
  void NMTTools_Tools::FindChains(const NMTTools_ListOfCoupleOfShape& aLCS,
				  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape& aMapChains)
{
  NMTTools_ListIteratorOfListOfCoupleOfShape aItCS; 
  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape aMCV;
  //
  aItCS.Initialize(aLCS);
  for (; aItCS.More(); aItCS.Next()) {
    const NMTTools_CoupleOfShape& aCS=aItCS.Value();
    //
    const TopoDS_Shape& aF1=aCS.Shape1();
    const TopoDS_Shape& aF2=aCS.Shape2();
    //
    //
    if (aMCV.Contains(aF1)) {
      TopTools_IndexedMapOfShape& aMV=aMCV.ChangeFromKey(aF1);
      aMV.Add(aF1);
      aMV.Add(aF2);
    }
    else {
      TopTools_IndexedMapOfShape aMV;
      aMV.Add(aF1);
      aMV.Add(aF2);
      aMCV.Add(aF1, aMV);
    }
    //
    if (aMCV.Contains(aF2)) {
      TopTools_IndexedMapOfShape& aMV=aMCV.ChangeFromKey(aF2);
      aMV.Add(aF1);
      aMV.Add(aF2);
    }
    else {
      TopTools_IndexedMapOfShape aMV;
      aMV.Add(aF1);
      aMV.Add(aF2);
      aMCV.Add(aF2, aMV);
    }
  }
  NMTTools_Tools::FindChains(aMCV, aMapChains);
}
//=======================================================================
// function: FindChains
// purpose : 
//=======================================================================
  void NMTTools_Tools::FindChains(const NMTTools_IndexedDataMapOfShapeIndexedMapOfShape& aMCV,
				  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape& aMapChains)
{
  Standard_Integer  i, j, aNbCV, aNbV;
  TopTools_IndexedMapOfShape aProcessed, aChain;
  //
  aNbCV=aMCV.Extent();
  for (i=1; i<=aNbCV; ++i) {
    const TopoDS_Shape& aF=aMCV.FindKey(i);
    if (aProcessed.Contains(aF)) {
      continue;
    }
    //
    aProcessed.Add(aF);
    aChain.Add(aF);
    //
    const TopTools_IndexedMapOfShape& aMV=aMCV(i);
    aNbV=aMV.Extent();
    for (j=1; j<=aNbV; ++j) {
      const TopoDS_Shape& aFx=aMV(j);
      ProcessBlock(aFx, aMCV, aProcessed, aChain);
    }
    aMapChains.Add(aF, aChain);
    aChain.Clear();
  }
}
//=======================================================================
// function: ProcessBlock
// purpose: 
//=======================================================================
void ProcessBlock(const TopoDS_Shape& aF,
		  const NMTTools_IndexedDataMapOfShapeIndexedMapOfShape& aMCV,
		  TopTools_IndexedMapOfShape& aProcessed,
		  TopTools_IndexedMapOfShape& aChain)
{
  Standard_Integer j, aNbV;
  //
  if (aProcessed.Contains(aF)) {
    return;
  }
  aProcessed.Add(aF);
  aChain.Add(aF);
  //
  const TopTools_IndexedMapOfShape& aMV=aMCV.FindFromKey(aF);
  aNbV=aMV.Extent();
  for (j=1; j<=aNbV; ++j) {
    const TopoDS_Shape& aFx=aMV(j);
    ProcessBlock(aFx, aMCV, aProcessed, aChain);
  }
}
