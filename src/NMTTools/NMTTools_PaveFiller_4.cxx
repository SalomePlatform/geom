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
// File:	NMTTools_PaveFiller_4.cxx
// Created:	Mon Dec  8 17:08:58 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>
//
#include <stdio.h>
#include <Precision.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <Bnd_Box.hxx>

#include <IntTools_ShrunkRange.hxx>
#include <IntTools_Range.hxx>
#include <IntTools_CommonPrt.hxx>
#include <IntTools_SequenceOfRanges.hxx>
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>

#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_PaveBlockIterator.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_IDMapOfPaveBlockIMapOfPaveBlock.hxx>
#include <BOPTools_IMapOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPave.hxx>
#include <BOPTools_SequenceOfPaveBlock.hxx>

#include <BOPTColStd_Dump.hxx>
#include <BOPTColStd_Failure.hxx>

#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeInteger.hxx>
#include <BooleanOperations_KindOfInterference.hxx>

#include <NMTDS_ShapesDataStructure.hxx>

#include <NMTTools_IndexedDataMapOfIndexedMapOfInteger.hxx>
#include <NMTTools_ListOfCommonBlock.hxx>
#include <NMTTools_CommonBlock.hxx>
#include <NMTTools_ListIteratorOfListOfCommonBlock.hxx>

//
static 
  void VertexParameters(const IntTools_CommonPrt& aCPart,
			Standard_Real& aT1, 
			Standard_Real& aT2);
static
  Standard_Boolean IsOnPave(const Standard_Real& aT1,
			    const IntTools_Range& aRange,
			    const Standard_Real& aTolerance);
static
  void ProcessBlock(const BOPTools_PaveBlock& aPB,
		    const BOPTools_IDMapOfPaveBlockIMapOfPaveBlock& aMapCB,
		    BOPTools_IMapOfPaveBlock& aProcessedBlocks,
		    BOPTools_IMapOfPaveBlock& aChain);
static
  void FindChains(const BOPTools_IDMapOfPaveBlockIMapOfPaveBlock& aMapCB,
		  NMTTools_ListOfCommonBlock& aLCB);

//=======================================================================
// function: PerformEE
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformEE() 
{
  myIsDone=Standard_False;
  //
  Standard_Boolean bJustAdd;
  Standard_Integer n1, n2, anIndexIn, nE1, nE2, aNbVEs, aBlockLength;
  Standard_Integer aTmp, aWhat, aWith, i, aNbCPrts, aDiscretize=30;
  Standard_Real aTolE1, aTolE2, aDeflection=0.01;
  BOPTools_ListIteratorOfListOfPaveBlock anIt1, anIt2;
  TopoDS_Edge aEWhat, aEWith; 
  TopoDS_Vertex aNewVertex;
  BooleanOperations_IndexedDataMapOfShapeInteger aMapVI;
  BOPTools_IDMapOfPaveBlockIMapOfPaveBlock aMapCB;
  //
  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  //
  // BlockLength correction
  aNbVEs=ExpectedPoolLength();
  aBlockLength=aEEs.BlockLength();
  if (aNbVEs > aBlockLength) {
    aEEs.SetBlockLength(aNbVEs);
  }
  //
  myDSIt.Initialize(TopAbs_EDGE, TopAbs_EDGE);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, bJustAdd);
    anIndexIn = 0;
    //
    if (myIntrPool->IsComputed(n1, n2)) {
      continue;
    }
    //
    nE1=n1; 
    nE2=n2; 
    SortTypes(nE1, nE2);
    //
    if(bJustAdd) {
      myIntrPool->AddInterference (nE1, nE2, BooleanOperations_EdgeEdge, anIndexIn);
      continue;
    }
    //
    const TopoDS_Edge& aE1=TopoDS::Edge(myDS->Shape(nE1));
    const TopoDS_Edge& aE2=TopoDS::Edge(myDS->Shape(nE2));
    //
    if (BRep_Tool::Degenerated(aE1) || BRep_Tool::Degenerated(aE2)){
      continue;
    }
    //
    aTolE1=BRep_Tool::Tolerance(aE1);
    aTolE2=BRep_Tool::Tolerance(aE2);
    //
    BOPTools_ListOfPaveBlock& aLPB1=mySplitShapesPool(myDS->RefEdge(nE1));
    //
    for (anIt1.Initialize(aLPB1); anIt1.More(); anIt1.Next()) {
      BOPTools_PaveBlock& aPB1=anIt1.Value();
      const IntTools_ShrunkRange& aShrunkRange1=aPB1.ShrunkRange();
      //
      const IntTools_Range& aSR1=aShrunkRange1.ShrunkRange();
      const Bnd_Box&        aBB1=aShrunkRange1.BndBox();
      //
      BOPTools_ListOfPaveBlock& aLPB2=mySplitShapesPool(myDS->RefEdge(nE2));
      //
      for (anIt2.Initialize(aLPB2); anIt2.More(); anIt2.Next()) {
	BOPTools_PaveBlock& aPB2=anIt2.Value();
	const IntTools_ShrunkRange& aShrunkRange2=aPB2.ShrunkRange();
      
	const IntTools_Range& aSR2=aShrunkRange2.ShrunkRange();
	const Bnd_Box&        aBB2=aShrunkRange2.BndBox();
	//
	if (aBB1.IsOut (aBB2)) {
	  continue;
	}
	// 
	// EE
	IntTools_EdgeEdge aEE;
	aEE.SetEdge1 (aE1);
	aEE.SetEdge2 (aE2);
	aEE.SetTolerance1 (aTolE1);
	aEE.SetTolerance2 (aTolE2);
	aEE.SetDiscretize (aDiscretize);
	aEE.SetDeflection (aDeflection);
	//
	IntTools_Range anewSR1 = aSR1;
	IntTools_Range anewSR2 = aSR2;
	//
	BOPTools_Tools::CorrectRange (aE1, aE2, aSR1, anewSR1);
	BOPTools_Tools::CorrectRange (aE2, aE1, aSR2, anewSR2);
	//
	aEE.SetRange1(anewSR1);
	aEE.SetRange2(anewSR2);
	  
	aEE.Perform();
	//
	anIndexIn=0;
	//
	if (aEE.IsDone()) {
	  // reverse order if it is necessary
	  aEWhat=aE1;
	  aEWith=aE2;
	  aWhat=nE1;
	  aWith=nE2;
	  if (aEE.Order()) {
	    aTmp=aWhat;
	    aWhat=aWith;
	    aWith=aTmp;
	    aEWhat=aE2;
	    aEWith=aE1;
	  }
	  //
	  const IntTools_SequenceOfCommonPrts& aCPrts=aEE.CommonParts();
	  aNbCPrts=aCPrts.Length();
	  for (i=1; i<=aNbCPrts; i++) {
	    const IntTools_CommonPrt& aCPart=aCPrts(i);
	    const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
	    //
	    anIndexIn=0;
	    //
	    TopAbs_ShapeEnum aType=aCPart.Type();
	    switch (aType) {
	      case TopAbs_VERTEX:  {
		Standard_Real aT1, aT2, aTol=Precision::PConfusion();
		Standard_Boolean bIsOnPave1, bIsOnPave2;
		IntTools_Range aR1, aR2;
		//
		VertexParameters(aCPart, aT1, aT2);
		// 
		//decide to keep the pave or not
		aR1 = (aEE.Order()) ? anewSR2 : anewSR1;
		aR2 = (aEE.Order()) ? anewSR1 : anewSR2;
		//
		bIsOnPave1=IsOnPave(aT1, aR1, aTol);
		bIsOnPave2=IsOnPave(aT2, aR2, aTol);
		//
		if(bIsOnPave1 || bIsOnPave2) {
		  myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		  continue;
		}
		//
		BOPTools_Tools::MakeNewVertex(aEWhat, aT1, aEWith, aT2, aNewVertex);
		//
		// Add Interference to the Pool
		BOPTools_EEInterference anInterf (aWhat, aWith, aCPart);
		//
		anIndexIn=aEEs.Append(anInterf);
		myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		//
		// Collect
		aMapVI.Add(aNewVertex, anIndexIn);
	      }
		break;
		
	      case TopAbs_EDGE: {
		Standard_Integer aNbComPrt2;
		Standard_Boolean aCoinsideFlag;
		//
		aNbComPrt2=aRanges2.Length();
		aCoinsideFlag=IsBlocksCoinside(aPB1, aPB2);
		//
		if (aNbComPrt2>1 || !aCoinsideFlag) {
		  myIntrPool->AddInterference (aWhat, aWith, BooleanOperations_EdgeEdge, anIndexIn);
		  break;
		}
		//
		// Fill aMapCB
		if (aMapCB.Contains(aPB1)) {
		  BOPTools_IMapOfPaveBlock& aMapPB=aMapCB.ChangeFromKey(aPB1);
		  aMapPB.Add(aPB1); 
		  aMapPB.Add(aPB2); 
		}
		else {
		  BOPTools_IMapOfPaveBlock aMapPB;
		  aMapPB.Add(aPB1); 
		  aMapPB.Add(aPB2); 
		  aMapCB.Add(aPB1, aMapPB);
		}
		//
		if (aMapCB.Contains(aPB2)) {
		  BOPTools_IMapOfPaveBlock& aMapPB=aMapCB.ChangeFromKey(aPB2);
		  aMapPB.Add(aPB1); 
		  aMapPB.Add(aPB2); 
		}
		else {
		  BOPTools_IMapOfPaveBlock aMapPB;
		  aMapPB.Add(aPB1); 
		  aMapPB.Add(aPB2); 
		  aMapCB.Add(aPB2, aMapPB);
		}
	      }
		break;
	    default:
	      break;
	    } // switch (aType) 
	  } // for (i=1; i<=aNbCPrts; i++) 
	}// if (aEE.IsDone())
      } // for (; anIt2.More(); anIt2.Next()) 
    } // for (; anIt1.More(); anIt1.Next()) 
  }// for (; myDSIt.More(); myDSIt.Next()) 
  //
  EENewVertices (aMapVI);
  EECommonBlocks(aMapCB);
  //
  myIsDone=Standard_True;
}
//=======================================================================
// function:EECommonBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::EECommonBlocks(const BOPTools_IDMapOfPaveBlockIMapOfPaveBlock& aMapCB)
{
  NMTTools_ListOfCommonBlock aLCB;
  //
  FindChains(aMapCB, aLCB);
  ReplaceCommonBlocks(aLCB);
}
//=======================================================================
// function:EENewVertices
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::EENewVertices (const BooleanOperations_IndexedDataMapOfShapeInteger& aMapVI) 
{
  Standard_Integer aNb, i, j, aNewShape, aNbEdges, aNbIEE, aNbVV, aNbSimple;
  Standard_Integer aWhat, aWith, i1, i2, nE1, nE2, nE, nV, aFlag;
  Standard_Real aT;
  TopoDS_Compound aCompound;
  BRep_Builder aBB;
  NMTTools_IndexedDataMapOfIndexedMapOfInteger aMNVE, aMNVIEE;
  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;	
  BOPTools_Pave aPave;
  TopoDS_Vertex aNewVertex;
  TopTools_IndexedMapOfShape aMNVComplex, aMNVSimple;
  //
  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  //
  aNb=aMapVI.Extent();
  //
  if (!aNb) { // no new vertices, no new problems 
    return;
  }
  //
  // 0. 
  if (aNb==1) {
    aNewVertex=TopoDS::Vertex(aMapVI.FindKey(1));
    EENewVertices(aNewVertex, aMapVI);
    return;
  }
  //
  // 1. Make compound from new vertices
  aBB.MakeCompound(aCompound);
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aV=aMapVI.FindKey(i);
    aBB.Add(aCompound, aV);
  }
  //
  // 2. VV intersection between these vertices 
  //       using the auxiliary Filler
  NMTDS_ShapesDataStructure tDS;
  //
  tDS.SetCompositeShape(aCompound);
  tDS.Init();
  //
  BOPTools_InterferencePool tInterfPool(tDS);
  NMTTools_PaveFiller tPaveFiller(tInterfPool);
  //
  tPaveFiller.Init();
  //
  tPaveFiller.PerformVV();
  tPaveFiller.PerformNewVertices();
  //
  const BOPTools_CArray1OfVVInterference& aVVInterfs=tInterfPool.VVInterfs();
  //
  // 3. Separate Comlex and Simple new vertices
  aNbVV=aVVInterfs.Extent();
  for (i=1; i<=aNbVV; ++i) {
    const BOPTools_VVInterference& aVV=aVVInterfs(i);
    aVV.Indices(aWhat, aWith);
    const TopoDS_Shape& aV1=tDS.Shape(aWhat);
    const TopoDS_Shape& aV2=tDS.Shape(aWith);
    aMNVComplex.Add(aV1);
    aMNVComplex.Add(aV2);
  }
  //
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aV=aMapVI.FindKey(i);
    if (!aMNVComplex.Contains(aV)) {
      aMNVSimple.Add(aV);
    }
  }
  //
  // 4. Treat Simple new Vertices
  aNbSimple=aMNVSimple.Extent();
  for (i=1; i<=aNbSimple; ++i) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aMNVSimple(i));
    EENewVertices(aV, aMapVI);
  }
  //
  // 3. Fill Maps : NewVertex-edges (aMNVE) 
  //                NewVertex-interferences (aMNVIEE)
  for (i=1; i<=aNbVV; ++i) {
    const BOPTools_VVInterference& aVV=aVVInterfs(i);
    aNewShape=aVV.NewShape();
    if (!aNewShape) {
      continue;
    }
    //
    if (!aMNVE.Contains(aNewShape)) {
      TColStd_IndexedMapOfInteger aMx;
      aMNVE.Add(aNewShape, aMx);
    }
    if (!aMNVIEE.Contains(aNewShape)) {
      TColStd_IndexedMapOfInteger aMx;
      aMNVIEE.Add(aNewShape, aMx);
    }
    //
    TColStd_IndexedMapOfInteger& aME=aMNVE.ChangeFromKey(aNewShape);
    TColStd_IndexedMapOfInteger& aMIEE=aMNVIEE.ChangeFromKey(aNewShape);
    //
    aVV.Indices(aWhat, aWith);
    //aWhat
    const TopoDS_Shape& aV1=tDS.Shape(aWhat);
    i1=aMapVI.FindFromKey(aV1);
    const BOPTools_EEInterference& aEE1=aEEs(i1);
    aEE1.Indices(nE1, nE2);
    aME.Add(nE1);
    aME.Add(nE2);
    aMIEE.Add(i1);
    //aWith
    const TopoDS_Shape& aV2=tDS.Shape(aWith);
    i2=aMapVI.FindFromKey(aV2);
    const BOPTools_EEInterference& aEE2=aEEs(i2);
    aEE2.Indices(nE1, nE2);
    aME.Add(nE1);
    aME.Add(nE2);
    aMIEE.Add(i2);
    //
    //printf(" VV: (%d, %d) -> %d\n", aWhat, aWith, aNewShape);
  }
  //
  // 4. Process new vertices
  aNb=aMNVE.Extent();
  for (i=1; i<=aNb; ++i) { // xx
    //
    //  new Vertex
    nV=aMNVE.FindKey(i);
    aNewVertex=TopoDS::Vertex(tDS.Shape(nV));
    //
    // Insert New Vertex in DS;
    myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
    aNewShape=myDS->NumberOfInsertedShapes();
    myDS->SetState (aNewShape, BooleanOperations_ON);
    //
    // Update index of NewShape in EE interferences
    const TColStd_IndexedMapOfInteger& aMIEE=aMNVIEE.FindFromKey(nV);//(i);
    aNbIEE=aMIEE.Extent();
    for (j=1; j<=aNbIEE; ++j) {
      i1=aMIEE(j);
      BOPTools_EEInterference& aEE1=aEEs(i1);
      aEE1.SetNewShape(aNewShape);
    }
    // 
    // Update Paves on edges
    const TColStd_IndexedMapOfInteger& aME=aMNVE(i);
    aNbEdges=aME.Extent();
    for (j=1; j<=aNbEdges; ++j) {
      nE=aME(j);
      const TopoDS_Edge& aE=TopoDS::Edge(myDS->Shape(nE));
      //
      aFlag=myContext.ComputeVE (aNewVertex, aE, aT);
      //
      if (!aFlag) {
	aPave.SetInterference(-1);
	aPave.SetType (BooleanOperations_EdgeEdge);
	aPave.SetIndex(aNewShape);
	aPave.SetParam(aT);
	//
	BOPTools_PaveSet& aPaveSet=myPavePoolNew(myDS->RefEdge(nE));
	aPaveSet.Append(aPave);
      }
    }
  }// for (i=1; i<=aNb; ++i) {// xx
}
//=======================================================================
// function:EENewVertices
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::EENewVertices (const TopoDS_Vertex& aNewVertex,
					   const BooleanOperations_IndexedDataMapOfShapeInteger& aMapVI) 
{
  Standard_Integer  i, aNewShape, nE1, nE2;
  Standard_Real  aT1, aT2;
  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;	
  BOPTools_Pave aPave;
  //
  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  //
  // one new vertex case is treated in usual way
  //
  // Insert New Vertex in DS;
  myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
  aNewShape=myDS->NumberOfInsertedShapes();
  myDS->SetState (aNewShape, BooleanOperations_ON);
  // Insert New Vertex in EE Interference
  i=aMapVI.FindFromKey(aNewVertex);
  BOPTools_EEInterference& aEEInterf= aEEs(i);
  aEEInterf.SetNewShape(aNewShape);
  // Extact interference info
  aEEInterf.Indices(nE1, nE2);
  const IntTools_CommonPrt& aCPart=aEEInterf.CommonPrt();
  VertexParameters(aCPart, aT1, aT2);
  //
  // Add Paves to the myPavePoolNew
  aPave.SetInterference(i);
  aPave.SetType (BooleanOperations_EdgeEdge);
  aPave.SetIndex(aNewShape);
  // Pave for edge nE1
  aPave.SetParam(aT1);
  BOPTools_PaveSet& aPaveSet1=myPavePoolNew(myDS->RefEdge(nE1));
  aPaveSet1.Append(aPave);
  // Pave for edge nE2
  aPave.SetParam(aT2);
  BOPTools_PaveSet& aPaveSet2=myPavePoolNew(myDS->RefEdge(nE2));
  aPaveSet2.Append(aPave);
}
//=======================================================================
// function: RefinePavePool
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::RefinePavePool()
{
  Standard_Integer  i, aNbNew;

  for (i=1; i<=myNbSources; i++) {

    if ((myDS->GetShape(i)).ShapeType()==TopAbs_EDGE) {
      BOPTools_PaveSet& aPS= myPavePool(myDS->RefEdge(i));
      //
      BOPTools_PaveSet& aNewPS= myPavePoolNew(myDS->RefEdge(i));
      BOPTools_ListOfPave& aNewLP=aNewPS.ChangeSet();
      //
      aNbNew=aNewLP.Extent();
      if (aNbNew) {
	BOPTools_ListIteratorOfListOfPave anIt(aNewLP);
	for (; anIt.More(); anIt.Next()) {
	  const BOPTools_Pave& aPave=anIt.Value();
	  aPS.Append(aPave);
	}
	// Clear the ListOfPaveBlock
	BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(i));
	aLPB.Clear();
	// Prepare the paveBlocks for that egde again
	PreparePaveBlocks(i);
      }
      aNewLP.Clear();
    }
  }
}
//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PreparePaveBlocks(const TopAbs_ShapeEnum aType1, 
					      const TopAbs_ShapeEnum aType2)
{
  myIsDone=Standard_False;
  //
  Standard_Boolean Ok1, Ok2, Ok3;
  Ok1= (aType1==TopAbs_VERTEX) &&  (aType2==TopAbs_EDGE) ;
  Ok2= (aType1==TopAbs_EDGE)   &&  (aType2==TopAbs_EDGE) ;
  Ok3= (aType1==TopAbs_EDGE)   &&  (aType2==TopAbs_FACE) ;
  if (!Ok1 && !Ok2 && !Ok3) {
    // error: Type mismatch
    return;
  }
  //
  Standard_Boolean aFlag = Standard_False;
  Standard_Integer n1, n2, nE1, nE2, aNbSplits;
  TColStd_MapOfInteger aMap;
  //
  myDSIt.Initialize(aType1, aType2);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, aFlag);
    nE1=n1; 
    nE2=n2; 
    SortTypes(nE1, nE2);
    //
    if (aType1==TopAbs_EDGE) {
      BOPTools_ListOfPaveBlock& aLPB1=mySplitShapesPool(myDS->RefEdge(nE1));
      aNbSplits=aLPB1.Extent();
      if (!aNbSplits) {
	if (!aMap.Contains(nE1)) { 
	  aMap.Add(nE1);
	  PreparePaveBlocks(nE1);
	  //
	  if (!myIsDone) {
	    return;
	  }
	}
      }
    }
    //
    if (aType2==TopAbs_EDGE) {
      BOPTools_ListOfPaveBlock& aLPB2=mySplitShapesPool(myDS->RefEdge(nE2));
      aNbSplits=aLPB2.Extent();
      if (!aNbSplits) {
	if (!aMap.Contains(nE2)) { 
	  aMap.Add(nE2);
	  PreparePaveBlocks(nE2);
	  //
	  if (!myIsDone) {
	    return;
	  }
	}
      }
    }// if (aType2==TopAbs_EDGE)
  }// for (; myDSIt.More(); myDSIt.Next()) 

  myIsDone=Standard_True;
}
//=======================================================================
// function: PreparePaveBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PreparePaveBlocks(const Standard_Integer nE)
{
  myIsDone=Standard_False;
  
  Standard_Integer nV1, nV2;

  TopoDS_Edge aE;
  TopoDS_Vertex aV1, aV2;
    
  // SplitShapesPool
  BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
  // Edge 
  aE=TopoDS::Edge(myDS->Shape(nE));
  //
  if (!BRep_Tool::Degenerated(aE)){
    //
    BOPTools_PaveSet& aPS=myPavePool(myDS->RefEdge(nE));
    
    BOPTools_PaveBlockIterator aPBIt(nE, aPS);
    for (; aPBIt.More(); aPBIt.Next()) {
      BOPTools_PaveBlock& aPB=aPBIt.Value();
      
      const IntTools_Range& aRange=aPB.Range();
      
      const BOPTools_Pave& aPave1=aPB.Pave1();
      nV1=aPave1.Index();
      aV1=TopoDS::Vertex(myDS->GetShape(nV1));
      
      const BOPTools_Pave& aPave2=aPB.Pave2();
      nV2=aPave2.Index();
      aV2=TopoDS::Vertex(myDS->GetShape(nV2));
      //
      // ShrunkRange
      IntTools_ShrunkRange aSR (aE, aV1, aV2, aRange, myContext);
      //
      Standard_Integer anErrorStatus;
      anErrorStatus=aSR.ErrorStatus();

      char buf[512];
      if (!aSR.IsDone()) {
	sprintf (buf, "Can not obtain ShrunkRange for Edge %d\n", nE);
	BOPTColStd_Dump::PrintMessage(buf);
	sprintf (buf, "Can not obtain ShrunkRange for Edge %d", nE);
	throw 
	  BOPTColStd_Failure(buf) ;
      }
      //
      if (anErrorStatus==6) {
	sprintf(buf,
		"Warning: [PreparePaveBlocks()] Max.Dummy Shrunk Range for Edge %d\n", nE);
	BOPTColStd_Dump::PrintMessage(buf);
      }
      else {
	// Check left paves and correct ShrunkRange if it is necessary
	CorrectShrunkRanges (0, aPave1, aSR);
	CorrectShrunkRanges (1, aPave2, aSR);
      }
      //
      aPB.SetShrunkRange(aSR);
      aLPB.Append(aPB);
    } //for (; aPBIt1.More(); aPBIt1.Next()) 
  }
  myIsDone=Standard_True;
}
//=======================================================================
// function: CorrectShrunkRanges
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::CorrectShrunkRanges(const Standard_Integer aSide,
						const BOPTools_Pave& aPave,
						IntTools_ShrunkRange& aShrunkRange)
{
  BooleanOperations_KindOfInterference aType;
  Standard_Integer anIndexInterf ;
  //
  aType=aPave.Type();
  if (aType!=BooleanOperations_EdgeEdge) {
    return;
  }
  //
  anIndexInterf=aPave.Interference();
  if (anIndexInterf<0) {
    // it can be EE interf between E and (e1,e2,..en) -> vertex
    // so we can't decide which aEE.CommonPrt() we should take.
    return;
  }

  BOPTools_CArray1OfEEInterference& aEEs=myIntrPool->EEInterferences();
  const BOPTools_EEInterference& aEE=aEEs(anIndexInterf);
  const IntTools_CommonPrt& aCP=aEE.CommonPrt();
  const TopoDS_Edge& aE1=aCP.Edge1();
  const TopoDS_Edge& aE2=aCP.Edge2();

  const IntTools_Range& aSR=aShrunkRange.ShrunkRange();
  const TopoDS_Edge& aE=aShrunkRange.Edge();
 
  IntTools_Range aNewRange;
  IntTools_Range aCPRange;

  if (aE1.IsSame(aE)) {
    const IntTools_Range& aR1=aCP.Range1();
    aCPRange=aR1;
  }
  if (aE2.IsSame(aE)) {
    const IntTools_SequenceOfRanges& aSeqR=aCP.Ranges2();
    const IntTools_Range& aR2=aSeqR(1);
     aCPRange=aR2;
  }
  //
  Standard_Real aCoeff=1.05, tV, tNV;
  tV=aPave.Param();
  if (aSide==0) { // Left
    if (aCPRange.Last() > aSR.First()) {
      tNV=aCPRange.Last();
      tNV=tV+aCoeff*(tNV-tV);
      aNewRange.SetFirst(tNV);
      aNewRange.SetLast (aSR.Last());

      if(aNewRange.First() > aNewRange.Last()) {
	aShrunkRange.SetShrunkRange(aNewRange);
      }
    }
  }
  else { // Right
    if (aCPRange.First() < aSR.Last()) {
      tNV=aCPRange.First();
      tNV=tV-aCoeff*(tV-tNV);
      aNewRange.SetFirst(aSR.First());
      aNewRange.SetLast (tNV);

      if(aNewRange.First() < aNewRange.Last()) {
	aShrunkRange.SetShrunkRange(aNewRange);
      }
    }
  }
}
//=======================================================================
// function:  IsBlocksCoinside
// purpose: 
//=======================================================================
  Standard_Boolean 
    NMTTools_PaveFiller::IsBlocksCoinside(const BOPTools_PaveBlock& aPB1,
					  const BOPTools_PaveBlock& aPB2) const
{
  Standard_Boolean bRetFlag=Standard_True;
  Standard_Real aTolV11, aTolV12, aTolV21, aTolV22;
  Standard_Real d1121, d1122, d1222, d1221, aTolSum, aCoeff=1.05;
  gp_Pnt aP11, aP12, aP21, aP22;

  const TopoDS_Vertex& aV11=TopoDS::Vertex(myDS->Shape(aPB1.Pave1().Index()));
  const TopoDS_Vertex& aV12=TopoDS::Vertex(myDS->Shape(aPB1.Pave2().Index()));
  const TopoDS_Vertex& aV21=TopoDS::Vertex(myDS->Shape(aPB2.Pave1().Index()));
  const TopoDS_Vertex& aV22=TopoDS::Vertex(myDS->Shape(aPB2.Pave2().Index()));

  aTolV11=BRep_Tool::Tolerance(aV11);
  aTolV12=BRep_Tool::Tolerance(aV12);
  aTolV21=BRep_Tool::Tolerance(aV21);
  aTolV22=BRep_Tool::Tolerance(aV22);
  
  aP11=BRep_Tool::Pnt(aV11);
  aP12=BRep_Tool::Pnt(aV12);
  aP21=BRep_Tool::Pnt(aV21);
  aP22=BRep_Tool::Pnt(aV22);

  d1121=aP11.Distance(aP21);
  aTolSum=aCoeff*(aTolV11+aTolV21);
  if (d1121<aTolSum) {
    d1222=aP12.Distance(aP22);
    aTolSum=aCoeff*(aTolV12+aTolV22);
    if (d1222<aTolSum) {
      return bRetFlag;
    }
  }
  //
  d1122=aP11.Distance(aP22);
  aTolSum=aCoeff*(aTolV11+aTolV22);
  if (d1122<aTolSum) {
    d1221=aP12.Distance(aP21);
    aTolSum=aCoeff*(aTolV12+aTolV21);
    if (d1221<aTolSum) {
      return bRetFlag;
    }
  }
  return !bRetFlag;
}
//=======================================================================
// function: ReplaceCommonBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::ReplaceCommonBlocks(const NMTTools_ListOfCommonBlock& aLCB)
{
  RemoveCommonBlocks(aLCB);
  SplitCommonBlocks(aLCB);
}
//=======================================================================
// function: SplitCommonBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::SplitCommonBlocks(const NMTTools_ListOfCommonBlock& aLCB)
{
  Standard_Integer nE;
  NMTTools_ListOfCommonBlock aLCBx;
  NMTTools_ListIteratorOfListOfCommonBlock anIt, anItCBx;
  BOPTools_ListIteratorOfListOfPaveBlock anItLPE;
  //
  anIt.Initialize(aLCB);
  for (; anIt.More(); anIt.Next()) {
    const NMTTools_CommonBlock& aCB=anIt.Value();
    //
    //XXX
    aLCBx.Clear();
    //XXX
    SplitCommonBlock(aCB, aLCBx);
    //
    anItCBx.Initialize(aLCBx);
    for (; anItCBx.More(); anItCBx.Next()) {
      const NMTTools_CommonBlock& aCBx=anItCBx.Value();
      const BOPTools_ListOfPaveBlock& aLPBx=aCBx.PaveBlocks();
      //
      anItLPE.Initialize(aLPBx);
      for (; anItLPE.More(); anItLPE.Next()) {
	const BOPTools_PaveBlock& aPBx=anItLPE.Value();
	nE=aPBx.OriginalEdge();
	NMTTools_ListOfCommonBlock& aLCBE=myCommonBlockPool(myDS->RefEdge(nE));
	aLCBE.Append(aCBx);
      }
    }
  }
}
//=======================================================================
// function: RemoveCommonBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::RemoveCommonBlocks(const NMTTools_ListOfCommonBlock& aLCB)
{
  Standard_Integer nE;
  NMTTools_ListOfCommonBlock aLCBx;
  NMTTools_ListIteratorOfListOfCommonBlock anItCB, anItCBE;
  BOPTools_ListIteratorOfListOfPaveBlock anItLPB;
  //
  anItCB.Initialize(aLCB);
  for (; anItCB.More(); anItCB.Next()) {
    const NMTTools_CommonBlock& aCB=anItCB.Value();
    const BOPTools_ListOfPaveBlock& aLPB=aCB.PaveBlocks();
    //
    // Remove aCB from each edge 
    anItLPB.Initialize(aLPB);
    for (; anItLPB.More(); anItLPB.Next()) {
      const BOPTools_PaveBlock& aPB=anItLPB.Value();
      nE=aPB.OriginalEdge();
      //
      NMTTools_ListOfCommonBlock& aLCBE=myCommonBlockPool(myDS->RefEdge(nE));
      anItCBE.Initialize(aLCBE);
      for (; anItCBE.More(); anItCBE.Next()) {
	const NMTTools_CommonBlock& aCBE=anItCBE.Value();
	if (aCBE.IsEqual(aCB)) {
	  aLCBE.Remove(anItCBE);
	  break;
	}
      }
    }
  }
}
//=======================================================================
// function: SplitCommonBlock
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::SplitCommonBlock(const NMTTools_CommonBlock& aCB,
					     NMTTools_ListOfCommonBlock& aLCBx)
{
  Standard_Integer i, j, k, nE, aNbE, aNbSPBx, aNbPB; 
  BOPTools_SequenceOfPaveBlock aSPBx;
  BOPTools_ListIteratorOfListOfPaveBlock anItLPB;
  BOPTools_ListIteratorOfListOfPave anIt;
  
  BOPTools_PaveBlockIterator anPBIt; 
  //
  const BOPTools_ListOfPaveBlock& aLPB=aCB.PaveBlocks();
  aNbE=aLPB.Extent();
  //
  // 1. Whether we realy need to split the common block ?
  anItLPB.Initialize(aLPB);
  for (; anItLPB.More(); anItLPB.Next()) {
    const BOPTools_PaveBlock& aPB=anItLPB.Value();
    nE=aPB.OriginalEdge();
    BOPTools_PaveSet& aPSE=myPavePoolNew(myDS->RefEdge(nE));
    aPSE.SortSet();
    //
    BOPTools_PaveSet aPSx;
    //
    const BOPTools_ListOfPave& aLPE=aPSE.Set();
    anIt.Initialize(aLPE);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Pave& aPx=anIt.Value();
      if (aPB.IsInBlock(aPx)) {
	aPSx.Append(aPx);
      }
    }
    aNbPB=aPSx.Set().Extent();
    break;
  }
  //
  if (!aNbPB) {
    // we need not split it
    aLCBx.Append(aCB);
    return;
  }
  //
  // 2. Get sequence of pave Blocks containing all new pave blocks
  // for each edges's source pave Block
  anItLPB.Initialize(aLPB);
  for (; anItLPB.More(); anItLPB.Next()) {
    const BOPTools_PaveBlock& aPB=anItLPB.Value();
    const BOPTools_Pave& aPave1=aPB.Pave1();
    const BOPTools_Pave& aPave2=aPB.Pave2();
    nE=aPB.OriginalEdge();
    //
    BOPTools_PaveSet aPSx;
    //
    // the set aPsx will contain bounadry paves aPave1, aPave2 and
    // all paves of the edge nE that are inside block aPB
    aPSx.Append(aPave1);
    aPSx.Append(aPave2);
    //
    BOPTools_PaveSet& aPSE=myPavePoolNew(myDS->RefEdge(nE));
    aPSE.SortSet();
    //
    const BOPTools_ListOfPave& aLPE=aPSE.Set();
    anIt.Initialize(aLPE);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_Pave& aPx=anIt.Value();
      if (aPB.IsInBlock(aPx)) {
	aPSx.Append(aPx);
      }
    }
    //
    // Form pave blocks from aPSx and collect them in aSPBx
    anPBIt.Initialize(nE, aPSx);
    for (; anPBIt.More(); anPBIt.Next()) {
      const BOPTools_PaveBlock& aPBx=anPBIt.Value();
      aSPBx.Append(aPBx);
    }
  }
  //
  // 3. Do new common blocks 
  //
  const TColStd_ListOfInteger& aLF=aCB.Faces();
  aNbSPBx=aSPBx.Length();
  aNbPB=aNbSPBx/aNbE;
  //
  for (i=1; i<=aNbPB; ++i) {
    NMTTools_CommonBlock aCBx;
    //
    aCBx.AddFaces(aLF);
    //
    for (j=1; j<=aNbE; ++j) {
      k=i+(j-1)*aNbPB;
      const BOPTools_PaveBlock& aPB=aSPBx(k);
      aCBx.AddPaveBlock(aPB);
    }
    aLCBx.Append(aCBx);
  }
}

//=======================================================================
// function: VertexParameters
// purpose: 
//=======================================================================
void VertexParameters(const IntTools_CommonPrt& aCPart,
		      Standard_Real& aT1, 
		      Standard_Real& aT2)
{
  const IntTools_Range& aR1=aCPart.Range1();
  aT1=0.5*(aR1.First()+aR1.Last());
  //
  if((aCPart.VertexParameter1() >= aR1.First()) &&
     (aCPart.VertexParameter1() <= aR1.Last())) {
    aT1 = aCPart.VertexParameter1();
  }
  //
  const IntTools_SequenceOfRanges& aRanges2=aCPart.Ranges2();
  const IntTools_Range& aR2=aRanges2(1);
  aT2=0.5*(aR2.First()+aR2.Last());
  //
  if((aCPart.VertexParameter2() >= aR2.First()) &&
     (aCPart.VertexParameter2() <= aR2.Last())) {
    aT2 = aCPart.VertexParameter2();
  }
}
//=======================================================================
// function: KeepPave
// purpose: 
//=======================================================================
Standard_Boolean IsOnPave(const Standard_Real& aT1,
			  const IntTools_Range& aRange,
			  const Standard_Real& aTolerance)
{
  Standard_Boolean firstisonpave1, firstisonpave2, bIsOnPave;
  //
  firstisonpave1  = (Abs(aRange.First() - aT1) < aTolerance);
  firstisonpave2  = (Abs(aRange.Last()  - aT1) < aTolerance);
  bIsOnPave=(firstisonpave1 || firstisonpave2);
  return bIsOnPave;
}

//=======================================================================
// function:FindChains
// purpose: 
//=======================================================================
void FindChains(const BOPTools_IDMapOfPaveBlockIMapOfPaveBlock& aMapCB,
		NMTTools_ListOfCommonBlock& aLCB)
{
  Standard_Integer  i, j, aNbCB, aNbPB;
  BOPTools_IMapOfPaveBlock aProcessedBlocks, aChain;
  //
  aNbCB=aMapCB.Extent();
  for (i=1; i<=aNbCB; ++i) {
    const BOPTools_PaveBlock& aPB=aMapCB.FindKey(i);
    if (aProcessedBlocks.Contains(aPB)) {
      continue;
    }
    //
    aProcessedBlocks.Add(aPB);
    aChain.Add(aPB);
    //
    const BOPTools_IMapOfPaveBlock& aMapPB=aMapCB(i);
    aNbPB=aMapPB.Extent();
    for (j=1; j<=aNbPB; ++j) {
      const BOPTools_PaveBlock& aPBx=aMapPB(j);
      ProcessBlock(aPBx, aMapCB, aProcessedBlocks, aChain);
    }
    //
    NMTTools_CommonBlock aCB;
    //
    aNbPB=aChain.Extent();
    for (j=1; j<=aNbPB; ++j) {
      const BOPTools_PaveBlock& aPBx=aChain(j);
      aCB.AddPaveBlock(aPBx);
    }
    aLCB.Append(aCB);
    aChain.Clear();
  }
}
//=======================================================================
// function:ProcessBlock
// purpose: 
//=======================================================================
void ProcessBlock(const BOPTools_PaveBlock& aPB,
		  const BOPTools_IDMapOfPaveBlockIMapOfPaveBlock& aMapCB,
		  BOPTools_IMapOfPaveBlock& aProcessedBlocks,
		  BOPTools_IMapOfPaveBlock& aChain)
{
  Standard_Integer j, aNbPB;
  //
  if (aProcessedBlocks.Contains(aPB)) {
    return;
  }
  aProcessedBlocks.Add(aPB);
  aChain.Add(aPB);
  //
  const BOPTools_IMapOfPaveBlock& aMapPB=aMapCB.FindFromKey(aPB);
  aNbPB=aMapPB.Extent();
  for (j=1; j<=aNbPB; ++j) {
    const BOPTools_PaveBlock& aPBx=aMapPB(j);
    ProcessBlock(aPBx, aMapCB, aProcessedBlocks, aChain);
  }
}
