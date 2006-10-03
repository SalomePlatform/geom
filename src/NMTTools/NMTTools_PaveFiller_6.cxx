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
// File:	NMTTools_PaveFiller_6.cxx
// Created:	Fri Dec 19 10:27:31 2003
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <NMTTools_PaveFiller.ixx>

#include <Precision.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <BndLib_Add3dCurve.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepLib.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerInteger.hxx>

#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BooleanOperations_OnceExplorer.hxx>
#include <BooleanOperations_ShapesDataStructure.hxx>

#include <IntTools_SequenceOfPntOn2Faces.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_Tools.hxx>

#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_ListIteratorOfListOfInterference.hxx>
#include <BOPTools_CArray1OfInterferenceLine.hxx>
#include <BOPTools_InterferenceLine.hxx>
#include <BOPTools_ListOfInterference.hxx>
#include <BOPTools_Interference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_Tools2D.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_IndexedDataMapOfShapePaveBlock.hxx>
#include <NMTTools_CommonBlockAPI.hxx>
#include <NMTTools_Tools.hxx>

static 
  Standard_Boolean IsPairFound(const Standard_Integer nF1,
			       const Standard_Integer nF2,
			       BOPTools_InterferencePool* myIntrPool,
			       BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWhat,
			       BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWith);

static
  void FMapWhat(const Standard_Integer nF,
		BOPTools_InterferencePool* myIntrPool,
		TColStd_IndexedMapOfInteger& aMapWhat);
static
  void FMapWith(const Standard_Integer nF,
		BOPTools_InterferencePool* myIntrPool,
		TColStd_IndexedMapOfInteger& aMapWith);
static
  Standard_Boolean IsFound(const TColStd_IndexedMapOfInteger& aMapWhat,
			   const TColStd_IndexedMapOfInteger& aMapWith);


//=======================================================================
// function: PerformFF
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformFF() 
{
  myIsDone=Standard_False;
  //
  Standard_Boolean bIsFound, bJustAdd, bIsComputed;
  Standard_Integer n1, n2, anIndexIn, nF1, nF2, aBlockLength, aNbFFs;
  Standard_Boolean bToApproxC3d, bToApproxC2dOnS1, bToApproxC2dOnS2, bIsDone;
  Standard_Integer aNbCurves, aNbPoints;
  Standard_Real anApproxTol, aTolR3D, aTolR2D;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aMapWhat, aMapWith;
  IntTools_SequenceOfPntOn2Faces aPnts;
  IntTools_SequenceOfCurves aCvs;
  BooleanOperations_KindOfInterference aTypeFF=BooleanOperations_SurfaceSurface;
  //
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  //
  //  F/F Interferences  [BooleanOperations_SurfaceSurface]
  myDSIt.Initialize(TopAbs_FACE, TopAbs_FACE);
  //
  // BlockLength correction
  aNbFFs=ExpectedPoolLength();
  aBlockLength=aFFs.BlockLength();
  if (aNbFFs > aBlockLength) {
    aFFs.SetBlockLength(aNbFFs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, bJustAdd);
    //
    bIsComputed=myIntrPool->IsComputed(n1, n2);
    if (bIsComputed) {
      continue;
    }
    //
    nF1 = n2;
    nF2 = n1;
    if(n1 < n2) {
      nF1 = n1;
      nF2 = n2;
    }
    anIndexIn=0;
    aPnts.Clear();
    aCvs.Clear();
    //
    bIsFound=IsPairFound(nF1, nF2, myIntrPool, aMapWhat, aMapWith);
    //
    if (bJustAdd) {
      if (!bIsFound) {
	myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
      }
      else{
	BOPTools_SSInterference anInterf (nF1, nF2, 1.e-07, 1.e-07, aCvs, aPnts);
	anIndexIn=aFFs.Append(anInterf);
	myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
      }
      continue;
    }
    //
    const TopoDS_Face aF1=TopoDS::Face(myDS->Shape(nF1));
    const TopoDS_Face aF2=TopoDS::Face(myDS->Shape(nF2));
    //
    // FF
    bToApproxC3d     = mySectionAttribute.Approximation();
    bToApproxC2dOnS1 = mySectionAttribute.PCurveOnS1();
    bToApproxC2dOnS2 = mySectionAttribute.PCurveOnS2();
    //
    anApproxTol=1.e-7;
    //
    IntTools_FaceFace aFF;
    //
    aFF.SetParameters (bToApproxC3d, bToApproxC2dOnS1, 
		       bToApproxC2dOnS2, anApproxTol);
    //
    aFF.Perform(aF1, aF2);
    //
    bIsDone=aFF.IsDone();
    //
    if (!bIsDone) {
      if (!bIsFound) {
	myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
      }
      else {
	BOPTools_SSInterference anInterf (nF1, nF2, 1.e-07, 1.e-07, aCvs, aPnts);
	anIndexIn=aFFs.Append(anInterf);
	myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
      }
      continue;
    }
    //
    aTolR3D=aFF.TolReached3d();
    aTolR2D=aFF.TolReached2d();
    if (aTolR3D < 1.e-7){
      aTolR3D=1.e-7;
    } 
    //
    aFF.PrepareLines3D();
    //
    const IntTools_SequenceOfCurves& aCvsX=aFF.Lines();
    const IntTools_SequenceOfPntOn2Faces& aPntsX=aFF.Points();
    //
    aNbCurves=aCvsX.Length();
    aNbPoints=aPntsX.Length();
    //
    if (!aNbCurves && !aNbPoints) {
      BOPTools_SSInterference anInterf (nF1, nF2, 1.e-07, 1.e-07, aCvs, aPnts);
      anIndexIn=aFFs.Append(anInterf);
      myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
      continue;
    }
    //
    {
      BOPTools_SSInterference anInterf (nF1, nF2, aTolR3D, aTolR2D, aCvsX, aPntsX);
      anIndexIn=aFFs.Append(anInterf);
      myIntrPool->AddInterference (nF1, nF2, aTypeFF, anIndexIn);
    }
    //
  }// for (; myDSIt.More(); myDSIt.Next()) 
  //
  myIsDone=Standard_True;
}
//=======================================================================
// function: MakeBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::MakeBlocks()
{
  myIsDone=Standard_False;
  //
  Standard_Boolean bIsExistingPaveBlock, bIsValidIn2D;
  Standard_Integer i, aNbFFs, nF1, nF2, aBid=0;
  Standard_Integer nV1, nV2, j, aNbCurves;
  Standard_Real aTolR3D, aTol2D, aT1, aT2, aTolPPC=Precision::PConfusion();
  NMTTools_IndexedDataMapOfShapePaveBlock aMEPB;
  BooleanOperations_IndexedDataMapOfShapeInteger aMapEI;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;  
  //
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  //
  // 1. Produce Section Edges from intersection curves
  //    between each pair of faces
  //
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    // 
    // Faces
    aFFi.Indices(nF1, nF2);
    const TopoDS_Face aF1=TopoDS::Face(myDS->Shape(nF1));
    const TopoDS_Face aF2=TopoDS::Face(myDS->Shape(nF2));
    //
    BOPTools_ListOfPaveBlock aLPB, aLPBC;
    //
    {
      Standard_Integer nFj1, nFj2, nE;
      TColStd_IndexedMapOfInteger aMFence;
      BOPTools_ListOfPaveBlock aLPBCx;
      //
      for (j=1; j<=aNbFFs; ++j) {
	BOPTools_SSInterference& aFFj=aFFs(j);
	aFFj.Indices(nFj1, nFj2);
	//
	if ((nF1==nFj1 && nFj2!=nF2) ||
	    (nF1==nFj2 && nFj1!=nF2) ||
	    (nF2==nFj1 && nFj2!=nF1) ||
	    (nF2==nFj2 && nFj1!=nF1)) {
	  RealSplitsInFace (aBid, nFj1, nFj2, aLPBCx);
	  RealSplitsInFace (aBid, nFj2, nFj1, aLPBCx);
	  RealSplitsOnFace (aBid, nFj1, nFj2, aLPBCx);
	}
      }
      //
      anIt.Initialize(aLPBCx);
      for (; anIt.More(); anIt.Next()) {
	const BOPTools_PaveBlock& aPBCx=anIt.Value();
	//
	nE=aPBCx.Edge();
	if (!aMFence.Contains(nE)) {
	  aMFence.Add(nE);
	  aLPBC.Append(aPBCx);
	}
      }
    }
    //
    //
    RealSplitsInFace (aBid, nF1, nF2, aLPB);
    RealSplitsInFace (aBid, nF2, nF1, aLPB);
    RealSplitsOnFace (aBid, nF1, nF2, aLPB);
    //
    anIt.Initialize(aLPB);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      aFFi.AppendBlock(aPB);
    }
    //
    BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
    aNbCurves=aSCvs.Length();
    if (!aNbCurves) {
      continue;
    }
    //
    aTolR3D=aFFi.TolR3D();
    aTol2D=(aTolR3D < 1.e-3) ? 1.e-3 : aTolR3D;
    //
    BOPTools_PaveSet aPSF;
    //
    PrepareSetForFace (nF1, nF2, aLPBC, aPSF);
    //PrepareSetForFace (nF1, nF2, aPSF);
    //
    // Put Paves On Curves
    for (j=1; j<=aNbCurves; ++j) {
      BOPTools_Curve& aBC=aSCvs(j);
      // DEBUG
      const IntTools_Curve& aC=aBC.Curve();
      Handle (Geom_Curve) aC3D= aC.Curve();
      //
      PutPaveOnCurve (aPSF, aTolR3D, aBC);
    }
    //
    // Put bounding paves on curves
    for (j=1; j<=aNbCurves; ++j) {
      BOPTools_Curve& aBC=aSCvs(j);
      PutBoundPaveOnCurve (aBC, aFFi);
    }
    //
    //  Pave Blocks on Curves
    for (j=1; j<=aNbCurves; ++j) {
      BOPTools_Curve& aBC=aSCvs(j);
      const IntTools_Curve& aIC= aBC.Curve();
      BOPTools_PaveSet& aPaveSet=aBC.Set();
      //
      BOPTools_PaveBlockIterator aPBIter(0, aPaveSet);
      for (; aPBIter.More(); aPBIter.Next()) {
	BOPTools_PaveBlock& aPBNew=aPBIter.Value();
	aPBNew.SetCurve(aIC);
	aPBNew.SetFace1(nF1);
	aPBNew.SetFace2(nF2);
	//
	nV1=aPBNew.Pave1().Index();
	nV2=aPBNew.Pave2().Index();
	aT1=aPBNew.Pave1().Param();
	aT2=aPBNew.Pave2().Param();
	// ???
	if((nV1==nV2) && (Abs(aT2 - aT1) < aTolPPC)) {
	  continue;// mkk ft
	}
	//
	// 1
	const BOPTools_ListOfPaveBlock& aLPBFF=aFFi.PaveBlocks();
	bIsExistingPaveBlock=IsExistingPaveBlock(aPBNew, aLPBFF, aTolR3D);
	//bIsExistingPaveBlock=IsExistingPaveBlock(aPBNew, aFFi);
	if (bIsExistingPaveBlock) {
	  continue;
	}
	// 2
	bIsExistingPaveBlock=IsExistingPaveBlock(aPBNew, aLPBC, aTolR3D);
	if (bIsExistingPaveBlock) {
	  continue;
	}
	// Checking of validity in 2D
	//
	bIsValidIn2D=myContext.IsValidBlockForFaces(aT1, aT2, aIC, aF1, aF2, aTol2D);
	if (!bIsValidIn2D) {
	  continue;
	}
	//
	//aBC.AppendNewBlock(aPBNew);
	//
	// Make Section Edge  
	TopoDS_Edge aES;
	//
	const TopoDS_Vertex aV1=TopoDS::Vertex(myDS->Shape(nV1));
	const TopoDS_Vertex aV2=TopoDS::Vertex(myDS->Shape(nV2));
	//
	BOPTools_Tools::MakeSectEdge (aIC, aV1, aT1, aV2, aT2, aES);
	//
	{
	  Standard_Real aTolR2D;
	  //
	  aTolR2D=aFFi.TolR2D();
	  Handle(Geom2d_Curve) aC2D1=aIC.FirstCurve2d();
	  Handle(Geom2d_Curve) aC2D2=aIC.SecondCurve2d();
	  //
	  NMTTools_Tools::MakePCurve(aES, aF1, aC2D1, aTolR2D);
	  NMTTools_Tools::MakePCurve(aES, aF2, aC2D2, aTolR2D);
	}
	//
	aMEPB.Add(aES, aPBNew);
	aMapEI.Add(aES, i);
      }
    } // end of for (j=1; j<=aNbCurves; ++j)
  }// for (i=1; i<=aNbFFs; ++i)
  //=============================================================
  //
  // II. Post treatment 
  //
  // Input data: aMEPB, aMapEI
  // Result    : section edges in myDS
  //
  Standard_Integer aNbSE;
  //
  aNbSE=aMEPB.Extent();
  if (!aNbSE) {
    // there is nothing to do here
    return;
  } 
  //
  BRep_Builder aBB;
  TopoDS_Compound aCompound;
  //
  // 1. Make compound from SE
  aBB.MakeCompound(aCompound);
  for (i=1; i<=aNbSE; ++i) {
    const TopoDS_Shape& aSE=aMEPB.FindKey(i);
    aBB.Add(aCompound, aSE);
  }
  //
  //
  // 2. Intersect SE using auxiliary Filler
  NMTDS_ShapesDataStructure tDS;
  //
  tDS.SetCompositeShape(aCompound);
  tDS.Init();
  //
  BOPTools_InterferencePool tIP(tDS);
  NMTTools_PaveFiller tPF(tIP);
  //
  // 2.1.VV
  tPF.Init();
  tPF.PerformVV();
  tPF.PerformNewVertices();
  //
  // 2.2.VE
  tPF.myPavePool.Resize (tPF.myNbEdges);
  tPF.PrepareEdges();
  tPF.PerformVE();
  //
  // 2.3.VF
  tPF.PerformVF();
  //
  // 2.4.EE
  tPF.myCommonBlockPool.Resize (tPF.myNbEdges);
  tPF.mySplitShapesPool.Resize (tPF.myNbEdges);
  tPF.myPavePoolNew    .Resize (tPF.myNbEdges);
  
  tPF.PreparePaveBlocks(TopAbs_VERTEX, TopAbs_EDGE);
  tPF.PreparePaveBlocks(TopAbs_EDGE, TopAbs_EDGE);
  //
  tPF.PerformEE();
  //
  tPF.RefinePavePool ();
  //
  tPF.myPavePoolNew.Destroy();
  //
  tPF.MakeSplitEdges();
  tPF.UpdateCommonBlocks();
  //
  // 3. Treatment of the result of intersection
  //
  Standard_Integer aNbOld, aNbLines, aNbPB, mV1, mV2, nE, mE, iFF;
  TopAbs_ShapeEnum aType;
  BOPTools_ListIteratorOfListOfPaveBlock aIt;
  BOPTColStd_IndexedDataMapOfIntegerInteger aMNewOld;
  //
  const BOPTools_SplitShapesPool& aSSP=tPF.mySplitShapesPool;
  const NMTTools_CommonBlockPool& aCBP=tPF.myCommonBlockPool;
  //
  aNbLines=tDS.NumberOfInsertedShapes();
  aNbOld=tDS.NumberOfShapesOfTheObject();
  // 
  // 3.1 Links between indices in tDS and DS (kept in aMNewOld)
  //
  // 3.1.1.Old vertices [ links ]
  for (i=1; i<=aNbOld; ++i) {
    const TopoDS_Shape& aV=tDS.Shape(i);
    aType=aV.ShapeType();
    if (aType!=TopAbs_VERTEX) {
      continue;
    }
    //
    for (j=1; j<=aNbSE; ++j) {
      const BOPTools_PaveBlock& aPBSE=aMEPB(j);
      nV1=aPBSE.Pave1().Index();
      const TopoDS_Shape aV1=myDS->Shape(nV1);
      if (aV1.IsSame(aV)) {
	aMNewOld.Add(i, nV1);
	break;
      }
      nV2=aPBSE.Pave2().Index();
      const TopoDS_Shape aV2=myDS->Shape(nV2);
      if (aV2.IsSame(aV)) {
	aMNewOld.Add(i, nV2);
	break;
      }
    }
  }
  //
  // 3.1.2. New vertices [ links ]
  i=tDS.NumberOfSourceShapes()+1;
  for (; i<=aNbLines; ++i) {
    const TopoDS_Shape& aV=tDS.Shape(i);
    aType=aV.ShapeType();
    if (aType!=TopAbs_VERTEX) {
      continue;
    }
    //
    // Insert new vertex in myDS
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    myDS->InsertShapeAndAncestorsSuccessors(aV, anASSeq);
    nV1=myDS->NumberOfInsertedShapes();
    // link
    aMNewOld.Add(i, nV1);
  }
  //
  // 3.2. Treatment of section edges (SE)
  for (i=1; i<=aNbOld; ++i) {
    const TopoDS_Shape& aE=tDS.Shape(i);
    aType=aE.ShapeType();
    if (aType!=TopAbs_EDGE) {
      continue;
    }
    //
    //  block of section edge that we already have for this SE
    BOPTools_PaveBlock& aPBSE=aMEPB.ChangeFromKey(aE);
    //
    // Corresponding FF-interference
    iFF=aMapEI.FindFromKey(aE);
    BOPTools_SSInterference& aFFi=aFFs(iFF);
    BOPTools_SequenceOfCurves& aSCvs=aFFi.Curves();
    //
    BOPTools_Curve& aBC=aSCvs(1); 
    //
    const BOPTools_ListOfPaveBlock& aLPB=aSSP(tDS.RefEdge(i));
    aNbPB=aLPB.Extent();
    //
    if (!aNbPB) {
      // no pave blocks -> use aPBSE and whole edge aE
      BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
      //
      nV1=aPBSE.Pave1().Index();
      const TopoDS_Shape aV1=myDS->Shape(nV1);
      nV2=aPBSE.Pave2().Index();
      const TopoDS_Shape aV2=myDS->Shape(nV2);
      //
      anASSeq.SetNewSuccessor(nV1);
      anASSeq.SetNewOrientation(aV1.Orientation());
      anASSeq.SetNewSuccessor(nV2);
      anASSeq.SetNewOrientation(aV2.Orientation());
      //
      myDS->InsertShapeAndAncestorsSuccessors(aE, anASSeq);
      nE=myDS->NumberOfInsertedShapes();
      //
      aPBSE.SetEdge(nE);
      aBC.AppendNewBlock(aPBSE);
      //
      continue;
    }
    //
    nF1=aPBSE.Face1();
    nF2=aPBSE.Face2();
    //
    //modified by NIZNHY-PKV Thu Jan 26 10:17:04 2006f
    //const TopoDS_Face& aF1=TopoDS::Face(myDS->Shape(nF1));
    //const TopoDS_Face& aF2=TopoDS::Face(myDS->Shape(nF2));
    //modified by NIZNHY-PKV Thu Jan 26 10:17:08 2006t
    //
    const NMTTools_ListOfCommonBlock& aLCB=aCBP(tDS.RefEdge(i));
    NMTTools_CommonBlockAPI aCBAPI(aLCB);
    //
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      BOPTools_PaveBlock aPB=aIt.Value();
      //
      ////modified by NIZNHY-PKV Thu Jan 26 10:16:36 2006f
      const TopoDS_Face aF1=TopoDS::Face(myDS->Shape(nF1));
      const TopoDS_Face aF2=TopoDS::Face(myDS->Shape(nF2));
      ////modified by NIZNHY-PKV Thu Jan 26 10:16:39 2006t
      //
      if (aCBAPI.IsCommonBlock(aPB)) {
	// it can be Common Block
	Standard_Real aTolEx;
	Handle(Geom2d_Curve) aC2D1, aC2D2;
	TopoDS_Face aF1FWD, aF2FWD;
	//
	NMTTools_CommonBlock& aCB=aCBAPI.CommonBlock(aPB);
	const BOPTools_ListOfPaveBlock& aLPBx=aCB.PaveBlocks();
	//
	aPB=aCB.PaveBlock1();
	mE=aPB.Edge(); // index of edge in tDS
	const TopoDS_Edge& aEx=TopoDS::Edge(tDS.Shape(mE));
	aTolEx=BRep_Tool::Tolerance(aEx);
        //
	// <- Block A was here
	//
	aF1FWD=aF1;
	aF1FWD.Orientation(TopAbs_FORWARD);
	NMTTools_Tools::MakePCurve(aEx, aF1FWD, aC2D1, aTolEx);
	//
	aF2FWD=aF2;
	aF2FWD.Orientation(TopAbs_FORWARD);
	NMTTools_Tools::MakePCurve(aEx, aF2FWD, aC2D2, aTolEx);
      } //if (aCBAPI.IsCommonBlock(aPB))
      //
      // new SE
      mE=aPB.Edge(); // index of edge in tDS
      const TopoDS_Shape& aSp=tDS.Shape(mE);
      //
      const BOPTools_Pave& aPave1=aPB.Pave1();
      aT1=aPave1.Param();
      mV1=aPave1.Index();            // index in tDS
      nV1=aMNewOld.FindFromKey(mV1); // index in myDS
      const TopoDS_Shape aV1=myDS->Shape(nV1);
      //
      const BOPTools_Pave& aPave2=aPB.Pave2();
      aT2=aPave2.Param();
      mV2=aPave2.Index();
      nV2=aMNewOld.FindFromKey(mV2);
      const TopoDS_Shape aV2=myDS->Shape(nV2);
      //
      if (!aMNewOld.Contains(mE)) {
	// add new SE to the myDS
	BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
	//
	anASSeq.SetNewSuccessor(nV1);
	anASSeq.SetNewOrientation(aV1.Orientation());

	anASSeq.SetNewSuccessor(nV2);
	anASSeq.SetNewOrientation(aV2.Orientation());
	
	myDS->InsertShapeAndAncestorsSuccessors(aSp, anASSeq);
	nE=myDS->NumberOfInsertedShapes();
	//
	aMNewOld.Add(mE, nE);
      }
      else {
	nE=aMNewOld.FindFromKey(mE);
      }
      // Form PaveBlock;
      BOPTools_PaveBlock aPBx;
      BOPTools_Pave aP1, aP2;
      //
      aPBx.SetFace1(nF1);
      aPBx.SetFace1(nF2);
      //
      aP1.SetIndex(nV1);
      aP1.SetParam(aT1);
      //
      aP2.SetIndex(nV2);
      aP2.SetParam(aT2);
      //
      aPBx.SetPave1(aP1);
      aPBx.SetPave2(aP2);
      //
      aPBx.SetEdge(nE);
      //
      aBC.AppendNewBlock(aPBx);
    }// for (; aIt.More(); aIt.Next()) 
  }// for (i=1; i<=aNbOld; ++i) 
  //
  myIsDone=Standard_True;
}
//=======================================================================
// function: MakePCurves
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::MakePCurves()
{
  Standard_Integer i, aNb,  nF1, nF2, nE;
  TopoDS_Face aF1FWD, aF2FWD;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;
  //
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  //
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; i++) {
    BOPTools_SSInterference& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    const TopoDS_Face aF1=TopoDS::Face(myDS->Shape(nF1));
    const TopoDS_Face aF2=TopoDS::Face(myDS->Shape(nF2));
    //
    aF1FWD=aF1;
    aF1FWD.Orientation(TopAbs_FORWARD);
    aF2FWD=aF2;
    aF2FWD.Orientation(TopAbs_FORWARD);
    //
    // In, On parts processing
    const BOPTools_ListOfPaveBlock& aLPBInOn=aFF.PaveBlocks();
    //
    anIt.Initialize(aLPBInOn);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      nE=aPB.Edge();
      const TopoDS_Edge aE=TopoDS::Edge(myDS->Shape(nE));
      
      BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF1FWD);
      BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF2FWD);
    }
  } 
}
//=======================================================================
// function: IsExistingPaveBlock
// purpose: 
//=======================================================================
   Standard_Boolean NMTTools_PaveFiller::IsExistingPaveBlock(const BOPTools_PaveBlock& aPBNew,
							     const BOPTools_ListOfPaveBlock& aLPBR,
							     const Standard_Real aTolR3D)
{
  Standard_Boolean bFlag;
  Standard_Integer nVNew1, nVNew2, nV1, nV2, iC;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;
  //
  bFlag=Standard_False;
  nVNew1=aPBNew.Pave1().Index();
  nVNew2=aPBNew.Pave2().Index();
  //
  anIt.Initialize(aLPBR);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPBR=anIt.Value();
    nV1=aPBR.Pave1().Index();
    nV2=aPBR.Pave2().Index();
    if (nVNew1==nV1 || nVNew1==nV2 || nVNew2==nV1 || nVNew2==nV2) {
      //
      iC=CheckIntermediatePoint(aPBNew, aPBR, aTolR3D);
      if (!iC) {
	return !bFlag;
      }
    }
  }
  return bFlag;
}
//=======================================================================
// function: CheckIntermediatePoint
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::CheckIntermediatePoint(const BOPTools_PaveBlock& aPB,
							       const BOPTools_PaveBlock& aPBR,
							       const Standard_Real aTolC)
							 
{
  Standard_Real aT11, aT12, aTM, aTmp;
  Standard_Integer iVM, nE2;
  gp_Pnt aPM;
  BRep_Builder aBB;
  TopoDS_Vertex aVM;
  // 
  // Vertex
  const BOPTools_Pave& aPave11=aPB.Pave1();
  aT11=aPave11.Param();
  // 
  const BOPTools_Pave& aPave12=aPB.Pave2();
  aT12=aPave12.Param();
  //
  aTM=IntTools_Tools::IntermediatePoint (aT11, aT12);
  //
  const IntTools_Curve& aIC=aPB.Curve();
  aIC.D0(aTM, aPM);
  //
  aBB.MakeVertex (aVM, aPM, aTolC);
  //
  //Edge
  nE2=aPBR.Edge();
  const TopoDS_Edge aE2=TopoDS::Edge(myDS->GetShape(nE2));
  // VE
  iVM=myContext.ComputeVE(aVM, aE2, aTmp); 
  //
  return iVM;
}
//=======================================================================
// function: PutBoundPaveOnCurve
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PutBoundPaveOnCurve(BOPTools_Curve& aBC,
						BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bHasBounds, bVF;
  Standard_Integer nF1, nF2;
  Standard_Real aT1, aT2, aTolR3D;
  gp_Pnt aP1, aP2;
  //
  const IntTools_Curve& aIC=aBC.Curve();
  bHasBounds=aIC.HasBounds ();
  if (!bHasBounds){
    return;
  }
  //
  // Bounds
  aIC.Bounds (aT1, aT2, aP1, aP2);
  //
  // Faces
  aFFi.Indices(nF1, nF2);
  aTolR3D=aFFi.TolR3D();
  //
  const TopoDS_Face aF1=TopoDS::Face(myDS->GetShape(nF1));
  const TopoDS_Face aF2=TopoDS::Face(myDS->GetShape(nF2));
  //
  bVF=myContext.IsValidPointForFaces (aP1, aF1, aF2, aTolR3D);
  if (bVF) {
    PutBoundPaveOnCurve (aP1, aT1, aBC, aFFi);
  }
  //
  bVF=myContext.IsValidPointForFaces (aP2, aF1, aF2, aTolR3D);
  if (bVF) {
    PutBoundPaveOnCurve (aP2, aT2, aBC, aFFi);
  }
}
//=======================================================================
// function: PutBoundPaveOnCurve
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PutBoundPaveOnCurve(const gp_Pnt& aP,
						const Standard_Real aT,
						BOPTools_Curve& aBC,
						BOPTools_SSInterference& aFFi)
{ 
  Standard_Boolean bFound1, bFound2;
  Standard_Integer nV;
  Standard_Real aTolV=aFFi.TolR3D();

  BOPTools_Pave aPave1, aPave2, aPave;
  BOPTools_PaveSet& aCPS=aBC.Set();
  BOPTools_PaveSet& aFFiPS=aFFi.NewPaveSet();
  const IntTools_Curve& aIC=aBC.Curve();
  //
  bFound1=FindPave(aP, aTolV, aCPS  , aPave1);
  bFound2=FindPave(aP, aTolV, aFFiPS, aPave2);
  //
  if (!bFound1 && !bFound2) {
    TopoDS_Vertex aNewVertex;
    BOPTools_Tools::MakeNewVertex(aP, aTolV, aNewVertex);
    //
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
    nV=myDS->NumberOfInsertedShapes();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);

    aCPS.Append(aPave);
    aFFiPS.Append(aPave);
    //
    // Append Techno Vertex to the Curve
    TColStd_ListOfInteger& aTVs=aBC.TechnoVertices();
    aTVs.Append(nV);
  }
  if (bFound1 && !bFound2) {
    nV=aPave1.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aFFiPS.Append(aPave);
    //
    const TopoDS_Vertex aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
  
  if (!bFound1 && bFound2) {
    nV=aPave2.Index();
    aPave.SetIndex(nV);
    aPave.SetParam(aT);
    aCPS.Append(aPave);
    //
    const TopoDS_Vertex aV=TopoDS::Vertex(myDS->Shape(nV));
    BOPTools_Tools::UpdateVertex (aIC, aT, aV);
  }
}
//=======================================================================
// function: FindPave
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_PaveFiller::FindPave(const gp_Pnt& aP,
						 const Standard_Real aTolPV, 
						 const BOPTools_PaveSet& aPS,
						 BOPTools_Pave& aPave)
{
  Standard_Integer nV;
  Standard_Boolean bIsVertex=Standard_False;
 
  const BOPTools_ListOfPave& aLP=aPS.Set();
  BOPTools_ListIteratorOfListOfPave anIt(aLP);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Pave& aPC=anIt.Value();
    nV=aPC.Index();
    const TopoDS_Vertex aV=TopoDS::Vertex(myDS->Shape(nV));
    bIsVertex=IntTools_Tools::IsVertex (aP, aTolPV, aV);
    if (bIsVertex) {
      aPave=aPC;
      return bIsVertex;
    }
  }
  return bIsVertex;
}
//=======================================================================
// function: PrepareSetForFace
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PrepareSetForFace(const Standard_Integer nF1,
					      const Standard_Integer nF2,
					      const BOPTools_ListOfPaveBlock& aLPBC,
					      BOPTools_PaveSet& aPSF)
{
  Standard_Integer nV1, nV2; 
  TColStd_MapOfInteger aMap;
  BOPTools_ListOfPaveBlock aLPB1, aLPB2, aLPBS;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;
  //
  RealSplitsFace(nF1, aLPB1);
  anIt.Initialize(aLPB1);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB=anIt.Value();
    aLPBS.Append(aPB);
  }
  //
  RealSplitsFace(nF2, aLPB2);
  anIt.Initialize(aLPB2);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB=anIt.Value();
    aLPBS.Append(aPB);
  }
  //
  anIt.Initialize(aLPBC);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB=anIt.Value();
    aLPBS.Append(aPB);
  }
  //
  anIt.Initialize(aLPBS);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_PaveBlock& aPB=anIt.Value();
    const BOPTools_Pave& aPave1=aPB.Pave1();
    nV1=aPave1.Index();
    if (!aMap.Contains(nV1)) {
      aMap.Add(nV1);
      aPSF.Append(aPave1);
    }
    const BOPTools_Pave& aPave2=aPB.Pave2();
    nV2=aPave2.Index();
    if (!aMap.Contains(nV2)) {
      aMap.Add(nV2);
      aPSF.Append(aPave2);
    }
  }
}
//=======================================================================
// function: PutPaveOnCurve
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PutPaveOnCurve(const BOPTools_PaveSet& aPaveSet,
					   const Standard_Real aTolR3D,
					   BOPTools_Curve& aBC)
{ 
  Standard_Integer nV;
  Standard_Boolean bIsVertexOnLine;
  Standard_Real aT;
  BOPTools_ListIteratorOfListOfPave anIt;
  Bnd_Box aBBC;
  GeomAdaptor_Curve aGAC;
  //
  const IntTools_Curve& aC=aBC.Curve();
  Handle (Geom_Curve) aC3D= aC.Curve();
  aGAC.Load(aC3D);
  BndLib_Add3dCurve::Add(aGAC, aTolR3D, aBBC);
  //
  const BOPTools_ListOfPave& aLP=aPaveSet.Set();
  anIt.Initialize(aLP);
  for (; anIt.More(); anIt.Next()) {
    const BOPTools_Pave& aPave=anIt.Value();
    //
    nV=aPave.Index();
    const TopoDS_Vertex aV=TopoDS::Vertex(myDS->Shape(nV));
    //
    Bnd_Box aBBV;
    BRepBndLib::Add(aV, aBBV);
    if (aBBC.IsOut(aBBV)){
      continue; 
    }
    //
    bIsVertexOnLine=myContext.IsVertexOnLine(aV, aC, aTolR3D, aT);
    //
    if (bIsVertexOnLine) {
      BOPTools_Pave aPaveNew(nV, aT, BooleanOperations_SurfaceSurface);
      BOPTools_PaveSet& aPS=aBC.Set();
      aPS.Append(aPaveNew);
      //<-B
      BOPTools_Tools::UpdateVertex (aC, aT, aV);
    }
  }
}
/////////////
//=======================================================================
// function: IsPairFound
// purpose: 
//=======================================================================
Standard_Boolean IsPairFound(const Standard_Integer nF1,
			     const Standard_Integer nF2,
			     BOPTools_InterferencePool* myIntrPool,
			     BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWhat,
			     BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger& aMapWith)
{
  Standard_Boolean bIsFound;
  //
  if (!aMapWhat.Contains(nF1)) {
    TColStd_IndexedMapOfInteger aMWhat;
    FMapWhat(nF1, myIntrPool, aMWhat);
    aMapWhat.Add(nF1, aMWhat);
  }
  //
  if (!aMapWith.Contains(nF2)) {
    TColStd_IndexedMapOfInteger aMWith;
    FMapWith(nF2, myIntrPool, aMWith);
    aMapWith.Add(nF2, aMWith);
  }
  //
  const TColStd_IndexedMapOfInteger& aMWht=aMapWhat.FindFromKey(nF1);
  const TColStd_IndexedMapOfInteger& aMWit=aMapWith.FindFromKey(nF2);
  //
  bIsFound=IsFound(aMWht, aMWit);
  //
  return bIsFound;
}
//=======================================================================
// function: FMapWhat
// purpose: 
//=======================================================================
void FMapWhat(const Standard_Integer nF,
	      BOPTools_InterferencePool* myIntrPool,
	      TColStd_IndexedMapOfInteger& aMapWhat)
		    
{
  Standard_Integer nE, nV;
 
  

  BooleanOperations_ShapesDataStructure* myDS=myIntrPool->DS();
  BooleanOperations_OnceExplorer aExp(*myDS);
  //
  //  What
  aMapWhat.Add(nF);
  aExp.Init(nF, TopAbs_VERTEX);
  for (; aExp.More(); aExp.Next()) {
    nV=aExp.Current();
    aMapWhat.Add(nV);
  }
  //
  aExp.Init(nF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    nE=aExp.Current();
    aMapWhat.Add(nE);
  }
}
//=======================================================================
// function: FMapWith
// purpose: 
//=======================================================================
void FMapWith(const Standard_Integer nF,
	      BOPTools_InterferencePool* myIntrPool,
	      TColStd_IndexedMapOfInteger& aMapWith)
{
  TColStd_IndexedMapOfInteger aMapWhat;
  
  FMapWhat(nF, myIntrPool, aMapWhat);
  //
  // With
  Standard_Integer i, aNb, anIndex, aWhat, aWith;
  BOPTools_ListIteratorOfListOfInterference anIt;
  
  const BOPTools_CArray1OfInterferenceLine& anArrIL= myIntrPool->InterferenceTable();

  aNb=aMapWhat.Extent();
  for (i=1; i<=aNb; i++) {
    aWhat=aMapWhat(i);
    
    const BOPTools_InterferenceLine& aWithLine=anArrIL(aWhat);
  
    const BOPTools_ListOfInterference& aLI=aWithLine.List();
    anIt.Initialize(aLI);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Interference& anIntf=anIt.Value();
      anIndex=anIntf.Index();
      if (anIndex) {
	aWith=anIntf.With();
	aMapWith.Add(aWith);
      }
    }
  }
}
//=======================================================================
// function: IsFound
// purpose: 
//=======================================================================
Standard_Boolean IsFound(const TColStd_IndexedMapOfInteger& aMapWhat,
			 const TColStd_IndexedMapOfInteger& aMapWith)
{
  Standard_Boolean bFlag=Standard_False;
  Standard_Integer i, aNb, aWhat;

  aNb=aMapWhat.Extent();
  for (i=1; i<=aNb; i++) {
    aWhat=aMapWhat(i);
    if (aMapWith.Contains(aWhat)) {
      return !bFlag;
    }
  }
  return bFlag;
}
//
//   Block A
//
      //
        // Commented out by EAP in the frame of PAL9151
        //
// 	Standard_Boolean bHasPCOnF, bFound;
// 	Standard_Integer nF, k, nEOrx, nF1x, nF2x;
// 	Standard_Real aTolEx, aT1x, aT2x;
// 	BOPTools_ListIteratorOfListOfPaveBlock aItPBx;
	//
// 	for (k=0; k<2; ++k) {	
// 	  nF=(!k) ? nF1 : nF2;
// 	  const TopoDS_Face& aF=TopoDS::Face(myDS->Shape(nF));
// 	  //
// 	  bHasPCOnF=BOPTools_Tools2D::HasCurveOnSurface(aEx, aF); 
// 	  if (bHasPCOnF) {
// 	    continue;
// 	  }
// 	  //
// 	  bFound=Standard_False;
// 	  aItPBx.Initialize(aLPBx);
// 	  for (; aItPBx.More(); aItPBx.Next()) {
// 	    BOPTools_PaveBlock& aPBx=aIt.Value();
// 	    nEOrx=aPBx.OriginalEdge();
// 	    const TopoDS_Shape& aEOrx=tDS.Shape(nEOrx);
// 	    BOPTools_PaveBlock& aPBSEx=aMEPB.ChangeFromKey(aEOrx);
// 	    aT1x=aPBSEx.Pave1().Param();
// 	    aT2x=aPBSEx.Pave2().Param();
// 	    const IntTools_Curve& aICx=aPBSEx.Curve();
// 	    //
// 	    nF1x=aPBSEx.Face1();
// 	    nF2x=aPBSEx.Face2();
// 	    //
// 	    if (nF1x==nF) {
// 	      Handle(Geom2d_Curve) aC2D1x=aICx.FirstCurve2d();
// 	      Handle(Geom2d_TrimmedCurve)aC2D1xT =new Geom2d_TrimmedCurve(aC2D1x, aT1x, aT2x);
// 	      aBB.UpdateEdge(aEx, aC2D1xT, aF, aTolEx);
// 	      bFound=!bFound;
// 	      break;
// 	    }
// 	    //
// 	    if (nF2x==nF) {
// 	      Handle(Geom2d_Curve) aC2D2x=aICx.SecondCurve2d();
// 	      Handle(Geom2d_TrimmedCurve)aC2D2xT =new Geom2d_TrimmedCurve(aC2D2x, aT1x, aT2x);
// 	      aBB.UpdateEdge(aEx, aC2D2xT, aF, aTolEx);
// 	      bFound=!bFound;
// 	      break;
// 	    }
// 	  }
// 	  if (bFound){
// 	    BRepLib::SameParameter(aEx, aTolEx, Standard_True);
// 	  }
// 	}
        //
        // The code till the if block end is restored from V2_2_2 revision

