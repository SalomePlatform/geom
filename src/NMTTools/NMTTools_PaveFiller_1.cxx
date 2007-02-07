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
// File:	NMTTools_PaveFiller_1.cxx
// Created:	Mon Dec  8 11:47:55 2003
// Author:	Peter KURNEV
//		<pkv@irinox>
#include <NMTTools_PaveFiller.ixx>

#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger.hxx>
  
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopTools_ListOfShape.hxx>

#include <IntTools_Tools.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BOPTools_CArray1OfVVInterference.hxx>
#include <BOPTools_VVInterference.hxx>
#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_Tools.hxx>

//=======================================================================
// function: PerformVV
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformVV() 
{
  myIsDone=Standard_False;
  //
  Standard_Integer anIndexIn, aWhat, aWith, aNbVVs, aBlockLength, aNbVSD; 
  Standard_Integer nVnew;
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger aIt1;
  TColStd_ListIteratorOfListOfInteger aIt;
  TopTools_ListOfShape aLV;
  TopoDS_Vertex aVnew;
  TopoDS_Shape aS;
  //
  myVSD.Clear();
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
  const TColStd_DataMapOfIntegerListOfInteger& aMVSD=myDSIt.SDVertices();
  aNbVSD=aMVSD.Extent();
  if (!aNbVSD) {
    return;
  }
  //
  aIt1.Initialize(aMVSD);
  for (; aIt1.More(); aIt1.Next()) {
    aLV.Clear();
    //
    aWhat=aIt1.Key();
    const TColStd_ListOfInteger& aLIV=aIt1.Value();
    //
    // new vertex
    aIt.Initialize(aLIV);
    for (; aIt.More(); aIt.Next()) {
      aWith=aIt.Value();
      aS=myDS->Shape(aWith);
      aLV.Append(aS);
    }
    aS=myDS->Shape(aWhat);
    aLV.Append(aS);
    //
    NMTTools_Tools::MakeNewVertex(aLV, aVnew);
    //
    BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq;
    //
    myDS->InsertShapeAndAncestorsSuccessors(aVnew, anASSeq);
    nVnew=myDS->NumberOfInsertedShapes();
    myDS->SetState (nVnew, BooleanOperations_ON);
    //
    // interferences
    aIt.Initialize(aLIV);
    for (; aIt.More(); aIt.Next()) {
      aWith=aIt.Value();
      BOPTools_VVInterference aVV(aWhat, aWith);
      aVV.SetNewShape(nVnew);
      anIndexIn=aVVs.Append(aVV);
      myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexVertex, anIndexIn);
      //
      // to find SD-Vertices
      myVSD.Bind(aWith, nVnew);
    }
     myVSD.Bind(aWhat, nVnew);
  }//for (; aIt1.More(); aIt1.Next()) {
  myIsDone=Standard_True;
}
//=======================================================================
// function: PerformNewVertices
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformNewVertices() 
{
}
//=======================================================================
// function: FindSDVertex
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::FindSDVertex(const Standard_Integer nV)const
{
  Standard_Integer nVSD;
  //
  nVSD=0;
  if (myVSD.IsBound(nV)) {
    nVSD=myVSD.Find(nV);
  }
  return nVSD;
}
