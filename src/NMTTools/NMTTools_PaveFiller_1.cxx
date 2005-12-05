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
// File:	NMTTools_PaveFiller_1.cxx
// Created:	Mon Dec  8 11:47:55 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>

#include <TColStd_IndexedMapOfInteger.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopTools_ListOfShape.hxx>

#include <IntTools_Tools.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>
#include <BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_Tools.hxx>
#include <TopTools_IndexedMapOfShape.hxx>


  
//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformVV() 
{
  myIsDone=Standard_False;
  //
  Standard_Integer n1, n2,anIndexIn, aFlag, aWhat, aWith, aNbVVs, aBlockLength;
  Standard_Boolean bJustAddInterference;
  //
  BOPTools_CArray1OfVVInterference& aVVs=myIntrPool->VVInterferences();
  //
  // BlockLength correction
  aNbVVs=ExpectedPoolLength();
  aBlockLength=aVVs.BlockLength();
  if (aNbVVs > aBlockLength) {
    aVVs.SetBlockLength(aNbVVs);
  }
  //
  // V/V  BooleanOperations_VertexVertex
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, bJustAddInterference);
    //
    if (!myIntrPool->IsComputed(n1, n2)) {
      anIndexIn=0;
      aWhat=n1;
      aWith=n2;
      SortTypes(aWhat, aWith);
      if (!bJustAddInterference) {
	const TopoDS_Shape& aS1=myDS->GetShape(aWhat);
	const TopoDS_Shape& aS2=myDS->GetShape(aWith);
	//
	const TopoDS_Vertex& aV1=TopoDS::Vertex(aS1);
	const TopoDS_Vertex& aV2=TopoDS::Vertex(aS2);
	aFlag=IntTools_Tools::ComputeVV (aV1, aV2);
	//
	if (!aFlag) {
	  BOPTools_VVInterference anInterf (aWhat, aWith);
	  anIndexIn=aVVs.Append(anInterf);
	}
      }
      myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexVertex, anIndexIn);
    }
  }
  myIsDone=Standard_True;
}
//=======================================================================
// function: PerformNewVertices
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformNewVertices() 
{
  myIsDone=Standard_False;
  //
  Standard_Integer i, aNb, anIndex1, anIndex2, aNewShape;
  TopoDS_Vertex aNewVertex;
  BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
  //
  Standard_Integer aNbChains, j, aNbV, aIdV, aNbL;
  TColStd_IndexedMapOfInteger aMapWhole;
  BOPTColStd_IndexedDataMapOfIntegerIndexedMapOfInteger aMapChains;//aMCV
  TopTools_ListOfShape aLV;
  TopTools_IndexedMapOfShape aM;
  //
  // 1. VV Interferences
  BOPTools_CArray1OfVVInterference& VVs=myIntrPool->VVInterferences();
  //
  NMTTools_Tools::FindChains(VVs, aMapChains);
  //
  aNbChains=aMapChains.Extent();
  for (i=1; i<=aNbChains; ++i) {
    const TColStd_IndexedMapOfInteger& aChain=aMapChains(i);
    //
    aM.Clear();
    aLV.Clear();
    aNbV=aChain.Extent();
    for (j=1; j<=aNbV; ++j) {
      aIdV=aChain(j);
      const TopoDS_Shape& aV=myDS->Shape(aIdV);
      if (!aM.Contains(aV)) {
	aM.Add(aV);
	aLV.Append(aV);
      }
    }
    //
    aNbL=aLV.Extent();
    if (aNbL==1){
      aNewShape=aChain(1);
    }
    else if (aNbL>1) {
      //
      // Make new Vertex
      NMTTools_Tools::MakeNewVertex(aLV, aNewVertex);
      // Insert New Vertex in DS;
      // aNewShape is # of DS-line, where aNewVertex is kept
      myDS->InsertShapeAndAncestorsSuccessors(aNewVertex, anASSeq);
      aNewShape=myDS->NumberOfInsertedShapes();
      //
      // State of New Vertex is ON
      myDS->SetState (aNewShape, BooleanOperations_ON);
    }
    //
    // Insert New Vertex in Interference
    aNb=VVs.Extent();
    for (j=1; j<=aNb; ++j) {
      BOPTools_VVInterference& VV=VVs(j);
      anIndex1=VV.Index1();
      anIndex2=VV.Index2();
      if (aChain.Contains(anIndex1) || aChain.Contains(anIndex2)) {
	VV.SetNewShape(aNewShape);
      }
    }
  }
  myIsDone=Standard_True;
}

//=======================================================================
// function: FindSDVertex
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::FindSDVertex(const Standard_Integer nV)const
{
  Standard_Integer i, aNb, anIndex1, anIndex2, aNewShape=0;

  BOPTools_CArray1OfVVInterference& VVs=myIntrPool->VVInterferences();
  aNb=VVs.Extent();
  
  for (i=1; i<=aNb; i++) {
    const BOPTools_VVInterference& VV=VVs(i);
    anIndex1=VV.Index1();
    anIndex2=VV.Index2();
    if (nV==anIndex1 || nV==anIndex2) {
      aNewShape=VV.NewShape();
      return aNewShape;
    }
  }
  return aNewShape;
}
