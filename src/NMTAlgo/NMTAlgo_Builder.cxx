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
// See http://www.salome-platform.org/
//
// File:	NMTAlgo_Inter3d.cxx
// Created:	Tue Jan 27 15:14:13 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Builder.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BOPTColStd_Dump.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <IntTools_Context.hxx>

#include <BOPTools_Tools3D.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_Curve.hxx>
#include <BOPTools_SequenceOfCurves.hxx>
#include <BOPTools_SplitShapesPool.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>

#include <BOP_WireEdgeSet.hxx>
#include <BOP_FaceBuilder.hxx>
#include <BOP_BuilderTools.hxx>

#include <NMTDS_ShapesDataStructure.hxx>

#include <NMTTools_PaveFiller.hxx>
#include <NMTTools_ListOfCoupleOfShape.hxx>
#include <NMTTools_Tools.hxx>
#include <NMTTools_CoupleOfShape.hxx>
#include <NMTTools_ListIteratorOfListOfCoupleOfShape.hxx>
#include <NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx>

#include <TopoDS_Shell.hxx>
#include <NMTAlgo_Tools.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>

//=======================================================================
// function: NMTAlgo_Inter3d::NMTAlgo_Inter3d()
// purpose: 
//=======================================================================
  NMTAlgo_Builder::NMTAlgo_Builder()
:
  NMTAlgo_Algo()
{
}
//=======================================================================
// function: ~NMTAlgo_Builder
// purpose: 
//=======================================================================
  NMTAlgo_Builder::~NMTAlgo_Builder()
{
  Clear();
}
//=======================================================================
// function: Clear
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::Clear()
{
  NMTAlgo_Algo::Clear();
  //
  myImagesEdges.Clear();
  myImagesFaces.Clear();
  myIn2DParts.Clear();
  mySectionParts.Clear();
  mySDFaces.Clear();
}
//=======================================================================
// function: ComputeWithFiller
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::ComputeWithFiller(const NMTTools_DSFiller& aDSF)
{
  myErrorStatus=0;
  myIsDone=Standard_False;
  //
  SetFiller(aDSF);
  // edges
  FillImagesEdges();
  // faces
  FillIn2DParts();
  FillImagesFaces();
  FillSDFaces();
}

//=======================================================================
// function: FillSDFaces
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::FillSDFaces()
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  BOPTools_InterferencePool* pIP=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIP->SSInterferences();
  IntTools_Context& aCtx= pPF->ChangeContext();
  //
  Standard_Boolean bIsSDF;
  Standard_Integer i, j, aNbFF, nF1, nF2, aNbPBInOn, aNbC;
  TopTools_ListIteratorOfListOfShape aItF1, aItF2;
  NMTTools_ListOfCoupleOfShape aLCS;   
  //
  mySDFaces.Clear();
  //
  // 1. For each FF find among images of faces
  //    all pairs of same domain faces (SDF) [=> aLCS]
  aNbFF=aFFs.Extent();
  for (i=1; i<=aNbFF; ++i) {
    BOPTools_SSInterference& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    const TopoDS_Face& aF1=TopoDS::Face(aDS.Shape(nF1));
    const TopoDS_Face& aF2=TopoDS::Face(aDS.Shape(nF2));
    //
    // if there are no in/on 2D split parts the faces nF1, nF2
    // can not be SDF
    const BOPTools_ListOfPaveBlock& aLPBInOn=aFF.PaveBlocks();
    aNbPBInOn=aLPBInOn.Extent();
    if (!aNbPBInOn) {
      continue;
    }
    //
    // if there is at least one section edge between faces nF1, nF2
    // they can not be SDF
    BOPTools_SequenceOfCurves& aSC=aFF.Curves();
    aNbC=aSC.Length();
    if (aNbC) {
      continue;
    }
    //
    // the faces are suspected to be SDF.
    // Try to find SDF among images of nF1, nF2
    const TopTools_ListOfShape& aLF1=myImagesFaces.Image(aF1);
    const TopTools_ListOfShape& aLF2=myImagesFaces.Image(aF2);
    //
    aItF1.Initialize(aLF1);
    for (; aItF1.More(); aItF1.Next()) {
      const TopoDS_Face& aF1x=TopoDS::Face(aItF1.Value());
      //
      aItF2.Initialize(aLF2);
      for (; aItF2.More(); aItF2.Next()) {
	const TopoDS_Face& aF2y=TopoDS::Face(aItF2.Value());
	bIsSDF=NMTTools_Tools::AreFacesSameDomain(aF1x, aF2y, aCtx);
	if (bIsSDF) {
	  NMTTools_CoupleOfShape aCS;
	  //
	  aCS.SetShape1(aF1x);
	  aCS.SetShape2(aF2y);
	  aLCS.Append(aCS);
	}
      }
    }
  }//for (i=1; i<=aNbFF; ++i)
  //
  aNbC=aLCS.Extent();
  if (!aNbC) {
    return;
  }
  //
  // 2. Find Chains
  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape aMC;  
  //
  NMTTools_Tools::FindChains(aLCS, aMC); 
  //
  // 3. Fill the map of SDF mySDFaces
  aNbC=aMC.Extent();
  for (i=1; i<=aNbC; ++i) {
    const TopoDS_Shape& aF=aMC.FindKey(i);
    const TopTools_IndexedMapOfShape& aMSDF=aMC(i);
    //
    aNbFF=aMSDF.Extent();
    for (j=1; j<=aNbFF; ++j) {
      const TopoDS_Shape& aFSD=aMSDF(j);
      mySDFaces.Add(aFSD, aF);
    }
  }
  //
}
//=======================================================================
// function: FillImagesFaces
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::FillImagesFaces()
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  IntTools_Context& aCtx= pPF->ChangeContext();
  //
  Standard_Integer i, j, aNb, aNbE;
  TopTools_IndexedMapOfShape aMFence, aME;
  TColStd_IndexedMapOfInteger aMFP;
  //
  // 1. Select Faces to process (MFP)
  aNb=aDS.NumberOfShapesOfTheObject();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aF=aDS.Shape(i);
    if (aF.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    if (aMFence.Contains(aF)) {
      continue;
    }
    aMFence.Add(aF);
    //
    if (myIn2DParts.Contains(aF)) {
      aMFP.Add(i);
      continue;
    }
    //
    if (mySectionParts.Contains(aF)) {
      aMFP.Add(i);
      continue;
    }
    //
    aME.Clear();
    TopExp::MapShapes(aF, TopAbs_EDGE, aME);
    //
    aNbE=aME.Extent();
    for(j=1; j<=aNbE; ++j) {
      const TopoDS_Shape& aE=aME(j);
      //
      if (myImagesEdges.HasImage(aE)) {
	aMFP.Add(i);
	break;
      }
    }
  }// for (i=1; i<=aNb; ++i)
  //
  // 2. ProcessFaces
  Standard_Boolean bToReverse, bIsClosed, bIsDegenerated;
  Standard_Integer aNbF, nF;
  TopoDS_Face aFF;
  TopoDS_Edge aSp;
  TopExp_Explorer anExp;
  TopTools_ListIteratorOfListOfShape aIt;
  BRepAlgo_Image aImagesFaces;
  TopAbs_Orientation anOriF;
  //
  aNbF=aMFP.Extent();
  for (i=1; i<=aNbF; ++i) {
    nF=aMFP(i);
    const TopoDS_Face& aF=TopoDS::Face(aDS.Shape(nF));
    anOriF=aF.Orientation();
    aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);
    //
    aMFence.Clear();
    //
    // 2.1. Fill WES 
    BOP_WireEdgeSet aWES;
    aWES.Initialize(aFF);
    //
    //  2.1.1. Add Split parts
    anExp.Init(aFF, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
      //
      if (!myImagesEdges.HasImage(aE)) {
	aWES.AddStartElement(aE);
	continue;
      }
      //
      bIsDegenerated=BRep_Tool::Degenerated(aE);
      bIsClosed=BRep_Tool::IsClosed(aE, aF);
      //
      const TopTools_ListOfShape& aLIE=myImagesEdges.Image(aE);
      aIt.Initialize(aLIE);
      for (; aIt.More(); aIt.Next()) {
	aSp=TopoDS::Edge(aIt.Value());
	//
	if (bIsDegenerated) {
	  aSp.Orientation(aE.Orientation());
	  aWES.AddStartElement(aSp);
	  continue;
	}
	//
	if (bIsClosed){
	  if (!aMFence.Contains(aSp)){
	    aMFence.Add(aSp);
	    //
	    if (!BRep_Tool::IsClosed(aSp, aF)){
	      BOPTools_Tools3D::DoSplitSEAMOnFace(aSp, aF);
	    }
	    //
	    aSp.Orientation(TopAbs_FORWARD);
	    aWES.AddStartElement(aSp);
	    //
	    aSp.Orientation(TopAbs_REVERSED);
	    aWES.AddStartElement(aSp);
	    continue;
	  }
	}
	//
	bToReverse=BOPTools_Tools3D::IsSplitToReverse1(aSp, aE, aCtx);
	if (bToReverse) {
	  aSp.Reverse();
	}
	aWES.AddStartElement(aSp);
      }
    }
    //
    // 2.1.2. Add In2D Parts
    if (myIn2DParts.Contains(aF)) {
      const TopTools_ListOfShape& aLE=myIn2DParts.FindFromKey(aF);
      aIt.Initialize(aLE);
      for (; aIt.More(); aIt.Next()) {
	aSp=TopoDS::Edge(aIt.Value());
	//
	aSp.Orientation(TopAbs_FORWARD);
	aWES.AddStartElement(aSp);
	//
	aSp.Orientation(TopAbs_REVERSED);
	aWES.AddStartElement(aSp);
	}
    }
    //
    // 2.1.3. Add Section Parts
    if (mySectionParts.Contains(aF)) {
      const TopTools_ListOfShape& aLE=mySectionParts.FindFromKey(aF);
      aIt.Initialize(aLE);
      for (; aIt.More(); aIt.Next()) {
	aSp=TopoDS::Edge(aIt.Value());
	//
	aSp.Orientation(TopAbs_FORWARD);
	aWES.AddStartElement(aSp);
	//
	aSp.Orientation(TopAbs_REVERSED);
	aWES.AddStartElement(aSp);
      }
    }
    //
    // 2.2. Build images Faces
    BOP_FaceBuilder aFB;
    //
    aFB.SetTreatment(0);  // 0-Do Internal Edges
    aFB.SetTreatSDScales(0); // what is 0 ??
    //
    aFB.Do(aWES);
    //
    TopTools_ListOfShape aLFR;
    //
    const TopTools_ListOfShape& aLF=aFB.NewFaces();

    aIt.Initialize(aLF);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape& aFR=aIt.Value();
      if (anOriF==TopAbs_REVERSED) {
	aFR.Orientation(TopAbs_REVERSED);
      }
      aLFR.Append(aFR);
    }
    
    //
    // 2.3. Collect images Faces
    myImagesFaces.Bind(aF, aLFR);
  }//for (i=1; i<=aNbF; ++i)
}
//=======================================================================
// function: FillIn2DParts
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::FillIn2DParts()
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  BOPTools_InterferencePool* pIP=(BOPTools_InterferencePool*)&myDSFiller->InterfPool();
  BOPTools_CArray1OfSSInterference& aFFs=pIP->SSInterferences();
  //
  Standard_Integer i, j, aNb, nF1, nF2, aNbFF, iFF, nSpIn, nSpSc, aNbCurves;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aFFMap;
  BOPTools_ListOfPaveBlock aLPBIn, aLPBSc;
  BOPTools_ListIteratorOfListOfPaveBlock aItPBIn, aItPBSc;
  TopTools_IndexedMapOfShape aMF, aMFence;
  TopTools_ListOfShape aLSpIn, aLSpSc;
  //
  BOP_BuilderTools::DoMap(aFFs, aFFMap);
  //
  // 1. Collect Splits In 2D   (myIn2DParts) and 
  //            Section Edges  (mySectionParts) 
  //    for each source face that involved in FF
  aNb=aFFMap.Extent();
  for (i=1; i<=aNb; ++i) {
    nF1=aFFMap.FindKey(i);
    const TopoDS_Shape& aF=aDS.Shape(nF1);
    //
    if (aMF.Contains(aF)) {
      continue;
    }
    aMF.Add(aF);
    //
    aLPBIn.Clear();
    aLPBSc.Clear();
    //
    const TColStd_IndexedMapOfInteger& aFFIndicesMap=aFFMap.FindFromIndex(i);
    //
    aNbFF=aFFIndicesMap.Extent();
    for (j=1; j<=aNbFF; ++j) {
      iFF=aFFIndicesMap(j);
      BOPTools_SSInterference& aFF=aFFs(iFF);
      nF2=aFF.OppositeIndex(nF1);
      //
      // In 2D
      pPF->RealSplitsInFace(0, nF2, nF1, aLPBIn);
      //
      // Sections
      BOPTools_SequenceOfCurves& aSC=aFF.Curves();
      aNbCurves=aSC.Length();
      if (!aNbCurves) {
	continue;
      }
      //
      const BOPTools_Curve& aBC=aSC(1);
      const BOPTools_ListOfPaveBlock& aLPB=aBC.NewPaveBlocks();
      //
      aItPBSc.Initialize(aLPB);
      for (; aItPBSc.More(); aItPBSc.Next()) {
	const BOPTools_PaveBlock& aPBSc=aItPBSc.Value();
	aLPBSc.Append(aPBSc);
      }
    }// for (j=1; j<=aNbFF; ++j) 
    //
    // In 2D Parts 
    aMFence.Clear();
    //
    aLSpIn.Clear();
    aItPBIn.Initialize(aLPBIn);
    for (; aItPBIn.More(); aItPBIn.Next()) {
      const BOPTools_PaveBlock& aPBR=aItPBIn.Value();
      nSpIn=aPBR.Edge();
      const TopoDS_Shape& aSpIn=aDS.Shape(nSpIn);
      if (!aMFence.Contains(aSpIn)){
	aMFence.Add(aSpIn);
	aLSpIn.Append(aSpIn);
      }
    }
    myIn2DParts.Add(aF, aLSpIn);
    //
    // Section Parts
    aLSpSc.Clear();
    aItPBSc.Initialize(aLPBSc);
    for (; aItPBSc.More(); aItPBSc.Next()) {
      const BOPTools_PaveBlock& aPBSc=aItPBSc.Value();
      nSpSc=aPBSc.Edge();
      const TopoDS_Shape& aSpSc=aDS.Shape(nSpSc);
      if (!aMFence.Contains(aSpSc)){
	aMFence.Add(aSpSc);
	aLSpSc.Append(aSpSc);
      }
    }
    mySectionParts.Add(aF, aLSpSc);
  } //for (i=1; i<=aNb; ++i)
}

//=======================================================================
// function: FillImagesEdges
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::FillImagesEdges()
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  const BOPTools_SplitShapesPool& aSSP=pPF->SplitShapesPool();
  //
  Standard_Integer nE, aNb, aNbSp, nSp;
  BOPTools_ListIteratorOfListOfPaveBlock aIt;
  TopTools_IndexedMapOfShape aMFence;
  TopTools_ListOfShape aLSp;
  //
  aNb=aDS.NumberOfShapesOfTheObject();
  for (nE=1; nE<=aNb; ++nE) {
    const TopoDS_Shape& aE=aDS.Shape(nE);
    if (aE.ShapeType()!=TopAbs_EDGE) {
      continue;
    }
    if (aMFence.Contains(aE)) {
      continue;
    }
    aMFence.Add(aE);
    //
    const BOPTools_ListOfPaveBlock& aLPB=aSSP(aDS.RefEdge(nE));
    aNbSp=aLPB.Extent();
    //
    if (!aNbSp) {// no splits  
      //myImagesEdges.Bind(aE, aE);
      continue;
    }
    //
    //modified by NIZNHY-PKV Fri Jan 21 17:01:10 2005 f
    if (aNbSp==1) {
      const BOPTools_PaveBlock& aPB1=aLPB.First();
      const BOPTools_PaveBlock& aPBR1=pPF->RealPaveBlock(aPB1);
      nSp=aPBR1.Edge();
      const TopoDS_Shape& aSp1=aDS.Shape(nSp);
      if (aSp1.IsSame(aE)) {
	continue;
      }
    }
    //modified by NIZNHY-PKV Fri Jan 21 17:01:14 2005 t
    //
    aLSp.Clear();
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      const BOPTools_PaveBlock& aPB=aIt.Value();
      const BOPTools_PaveBlock& aPBR=pPF->RealPaveBlock(aPB);
      nSp=aPBR.Edge();
      const TopoDS_Shape& aSp=aDS.Shape(nSp);
      aLSp.Append(aSp);
    }
    myImagesEdges.Bind(aE, aLSp); 
  }
}
//=======================================================================
// function: SplitVertices
// purpose: 
//=======================================================================
  void NMTAlgo_Builder::SplitVertices()
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  const BOPTools_SplitShapesPool& aSSP=pPF->SplitShapesPool();
  //
  Standard_Integer nE, aNb, aNbSp, nV1, nV2;
  BOPTools_ListIteratorOfListOfPaveBlock aIt;
  //
  myQueryShapes.Clear();
  //
  aNb=aDS.NumberOfShapesOfTheObject();
  for (nE=1; nE<=aNb; ++nE) {
    const TopoDS_Shape& aE=aDS.Shape(nE);
    if (aE.ShapeType()!=TopAbs_EDGE) {
      continue;
    }
    //
    const BOPTools_ListOfPaveBlock& aLPB=aSSP(aDS.RefEdge(nE));
    aNbSp=aLPB.Extent();
    //
    if (!aNbSp) {// no splits  
      continue;
    }
    //
    aIt.Initialize(aLPB);
    for (; aIt.More(); aIt.Next()) {
      const BOPTools_PaveBlock& aPB=aIt.Value();
      const BOPTools_PaveBlock& aPBR=pPF->RealPaveBlock(aPB);
      //
      nV1=aPBR.Pave1().Index();
      if (aDS.IsNewShape(nV1)) {
	const TopoDS_Shape& aV1=aDS.Shape(nV1);
	myQueryShapes.Add(aV1);
      }
      //
      nV2=aPBR.Pave2().Index();
      if (aDS.IsNewShape(nV2)) {
	const TopoDS_Shape& aV2=aDS.Shape(nV2);
	myQueryShapes.Add(aV2);
      }
    }
  }
}
//=======================================================================
// function: IsSectionEdge
// purpose: 
//=======================================================================
  Standard_Boolean NMTAlgo_Builder::IsSectionEdge(const TopoDS_Edge& aE)const
{
  return myImagesEdges.HasImage(aE);
}
//=======================================================================
// function: IsSameDomainF
// purpose: 
//=======================================================================
  Standard_Boolean NMTAlgo_Builder::HasSameDomainF(const TopoDS_Face& aF1)const
{
  Standard_Boolean bFlag=Standard_False;
  TopTools_ListIteratorOfListOfShape aItF1;
  //
  const TopTools_ListOfShape& aLF1=myImagesFaces.Image(aF1);
  aItF1.Initialize(aLF1);
  for (; aItF1.More(); aItF1.Next()) {
    const TopoDS_Shape& aF1x=aItF1.Value();
    //
    if (mySDFaces.Contains(aF1x)){
      return !bFlag;
    }
  }
  return bFlag;
}
//=======================================================================
// function: IsSameDomainF
// purpose: 
//=======================================================================
  Standard_Boolean NMTAlgo_Builder::IsSameDomainF(const TopoDS_Face& aF1,
						  const TopoDS_Face& aF2)const
{
  Standard_Boolean bFlag=Standard_False;
  TopTools_ListIteratorOfListOfShape aItF1, aItF2;
  //
  const TopTools_ListOfShape& aLF1=myImagesFaces.Image(aF1);
  const TopTools_ListOfShape& aLF2=myImagesFaces.Image(aF2);
  //
  aItF1.Initialize(aLF1);
  for (; aItF1.More(); aItF1.Next()) {
    const TopoDS_Shape& aF1x=aItF1.Value();
    //
    if (!mySDFaces.Contains(aF1x)){
      continue;
    }
    const TopoDS_Shape& aFSD1x=mySDFaces.FindFromKey(aF1x);
    //
    aItF2.Initialize(aLF2);
    for (; aItF2.More(); aItF2.Next()) {
      const TopoDS_Shape& aF2y=aItF2.Value();
      if (!mySDFaces.Contains(aF2y)){
	continue;
      }
      const TopoDS_Shape& aFSD2y=mySDFaces.FindFromKey(aF2y);
      if (aFSD1x.IsSame(aFSD2y)) {
	return !bFlag;
      }
    }
  }
  return bFlag;
}
