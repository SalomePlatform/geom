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
// File:	NMTTools_PaveFiller_7.cxx
// Created:	Thu Dec 18 15:14:55 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <TopAbs_Orientation.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_Pave.hxx>
#include <BOPTools_Tools.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BRep_Tool.hxx>
#include <NMTTools_ListIteratorOfListOfCommonBlock.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//=======================================================================
// function: MakeSplitEdges
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::MakeSplitEdges()
{
  myIsDone=Standard_False;
  //
  Standard_Boolean bIsNewVertex1, bIsNewVertex2;
  Standard_Integer i, aNbS, nV1, nV2, aNbPaveBlocks, aNewShapeIndex;
  Standard_Real    t1, t2;
  TopAbs_Orientation anOri;
  TopoDS_Edge aE, aESplit;
  TopoDS_Vertex aV1, aV2;
  //
  aNbS=myDS->NumberOfShapesOfTheObject();
  for (i=1; i<=aNbS; ++i) {
    if (myDS->GetShapeType(i) != TopAbs_EDGE)
      continue;
    //
    // Original Edge
    aE=TopoDS::Edge(myDS->Shape(i));
    if (BRep_Tool::Degenerated(aE)){
      continue;
    }
    //
    anOri=aE.Orientation(); 
    aE.Orientation(TopAbs_FORWARD);
    //
    // Making Split Edges
    //
    // Split Set for the Original Edge i
    BOPTools_ListOfPaveBlock& aSplitEdges=mySplitShapesPool(myDS->RefEdge(i));
    BOPTools_ListIteratorOfListOfPaveBlock aPBIt(aSplitEdges);
    //
    aNbPaveBlocks=aSplitEdges.Extent();
  
    for (; aPBIt.More(); aPBIt.Next()) {
      BOPTools_PaveBlock& aPB=aPBIt.Value();
      // aPave1
      const BOPTools_Pave& aPave1=aPB.Pave1();
      nV1=aPave1.Index();
      t1=aPave1.Param();
      aV1=TopoDS::Vertex(myDS->GetShape(nV1));
      aV1.Orientation(TopAbs_FORWARD);
      // aPave2
      const BOPTools_Pave& aPave2=aPB.Pave2();
      nV2=aPave2.Index();
      t2=aPave2.Param();
      aV2=TopoDS::Vertex(myDS->GetShape(nV2));
      aV2.Orientation(TopAbs_REVERSED);
      //xx
      if (aNbPaveBlocks==1) {
	bIsNewVertex1=myDS->IsNewShape (nV1);
	bIsNewVertex2=myDS->IsNewShape (nV2);
	if (!bIsNewVertex1 && !bIsNewVertex2) {
	  aPB.SetEdge(i);
	  continue;
	}
      }
      //xx
      BOPTools_Tools::MakeSplitEdge(aE, aV1, t1, aV2, t2, aESplit);  
      //
      // Add Split Part of the Original Edge to the DS
      BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;

      anASSeq.SetNewSuccessor(nV1);
      anASSeq.SetNewOrientation(aV1.Orientation());

      anASSeq.SetNewSuccessor(nV2);
      anASSeq.SetNewOrientation(aV2.Orientation());
      //
      if (anOri==TopAbs_INTERNAL) {
	anASSeq.SetNewAncestor(i);
	aESplit.Orientation(anOri);
      }
      //
      myDS->InsertShapeAndAncestorsSuccessors(aESplit, anASSeq);
      aNewShapeIndex=myDS->NumberOfInsertedShapes();
      myDS->SetState(aNewShapeIndex, BooleanOperations_UNKNOWN);
      //
      // Fill Split Set for the Original Edge
      aPB.SetEdge(aNewShapeIndex);
      //
    }
  } 
  myIsDone=Standard_True;
}
//=======================================================================
// function: UpdateCommonBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::UpdateCommonBlocks()
{
  myIsDone=Standard_False;
  //
  Standard_Integer nE, aNbS,  nSp, nEx, nSpx;
  NMTTools_ListIteratorOfListOfCommonBlock aCBIt;
  BOPTools_ListIteratorOfListOfPaveBlock aPBIt;
  //
  aNbS=myDS->NumberOfShapesOfTheObject();
  //
  for (nE=1; nE<=aNbS; ++nE) {
    if (myDS->GetShapeType(nE)!=TopAbs_EDGE){
      continue;
    }
    if (BRep_Tool::Degenerated(TopoDS::Edge(myDS->Shape(nE)))){
      continue;
    }
    //
    NMTTools_ListOfCommonBlock& aLCB=myCommonBlockPool(myDS->RefEdge(nE));
    BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool  (myDS->RefEdge(nE));
    //
    aCBIt.Initialize(aLCB);
    for (; aCBIt.More(); aCBIt.Next()) {
      NMTTools_CommonBlock& aCB=aCBIt.Value();
      BOPTools_PaveBlock& aPB=aCB.PaveBlock1(nE);
      nSp=SplitIndex(aPB);
      aPB.SetEdge(nSp);
      //
      const BOPTools_ListOfPaveBlock& aCBLPB=aCB.PaveBlocks();
      aPBIt.Initialize(aCBLPB);
      for (; aPBIt.More(); aPBIt.Next()) {
	BOPTools_PaveBlock& aPBx=aPBIt.Value();
	nEx=aPBx.OriginalEdge();
	if (nEx==nE) {
	  continue;
	}
	//
	nSpx=SplitIndex(aPBx);
	aPBx.SetEdge(nSpx);
      }
      //
    }
  }
}
//=======================================================================
// function: SplitIndex
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::SplitIndex(const BOPTools_PaveBlock& aPBx)const 
{
  Standard_Integer anOriginalEdge, anEdgeIndex=0;

  anOriginalEdge=aPBx.OriginalEdge();

  const BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(anOriginalEdge));
  //
  BOPTools_ListIteratorOfListOfPaveBlock anIt(aLPB);
  for (; anIt.More(); anIt.Next()) {
    BOPTools_PaveBlock& aPB=anIt.Value();
    if (aPB.IsEqual(aPBx)) {
      anEdgeIndex=aPB.Edge();
      return anEdgeIndex;
    }
  }
  return anEdgeIndex;
} 
//=======================================================================
// function: UpdatePaveBlocks
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::UpdatePaveBlocks()
{
  myIsDone=Standard_False;
  //
  
  Standard_Integer i, aNbFFs, nF1, nF2, aNbF, nF, iRankF, nE, nV1, nV2, aNbPB;
  Standard_Real aT1, aT2;
  TColStd_IndexedMapOfInteger aMF, aME;
  TopExp_Explorer aExp;
  TopoDS_Vertex aV1, aV2;
  TopoDS_Edge aE;
  BOPTools_Pave aPave1, aPave2;
  BOPTools_PaveBlock aPB;
  //
  BOPTools_CArray1OfSSInterference& aFFs=myIntrPool->SSInterferences();
  //
  aNbFFs=aFFs.Extent();
  for (i=1; i<=aNbFFs; ++i) {
    BOPTools_SSInterference& aFFi=aFFs(i);
    aFFi.Indices(nF1, nF2);
    aMF.Add(nF1);
    aMF.Add(nF2);
  }
  //
  aNbF=aMF.Extent();
  for(i=1; i<=aNbF; ++i) {
    nF=aMF(i);
    iRankF=myDS->Rank(nF);
    const TopoDS_Shape aF=myDS->Shape(nF);
    aExp.Init(aF, TopAbs_EDGE);
    for(; aExp.More();  aExp.Next()) {
      aE=TopoDS::Edge(aExp.Current());
      //
      if (BRep_Tool::Degenerated(aE)) {
	continue;
      }
      //
      nE=myDS->ShapeIndex(aE, iRankF);
      //
      if (aME.Contains(nE)) {
	continue;
      }
      aME.Add(nE);
      //
      BOPTools_ListOfPaveBlock& aLPB=mySplitShapesPool(myDS->RefEdge(nE));
      aNbPB=aLPB.Extent();
      if (aNbPB) {
	continue;
      }
      TopExp::Vertices(aE, aV1, aV2);
      //
      nV1=myDS->ShapeIndex(aV1, iRankF);
      aT1=BRep_Tool::Parameter(aV1, aE);
      aPave1.SetIndex(nV1);
      aPave1.SetParam(aT1);
      //
      nV2=myDS->ShapeIndex(aV2, iRankF);
      aT2=BRep_Tool::Parameter(aV2, aE);
      aPave2.SetIndex(nV2);
      aPave2.SetParam(aT2);
      //
      aPB.SetEdge(nE);
      aPB.SetOriginalEdge(nE);
      aPB.SetPave1(aPave1);
      aPB.SetPave2(aPave2);
      //
      aLPB.Append(aPB);
    }
  }
} 
