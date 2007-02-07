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
// File:	NMTDS_Iterator.cxx
// Created:	Sun May 07 15:04:41 2006
// Author:	Peter KURNEV
//		<peter@PREFEX>


#include <NMTDS_Iterator.ixx>
#include <NMTDS_CArray1OfIndexRange.hxx>
#include <NMTDS_IndexRange.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <NMTDS_PassKeyBoolean.hxx>
#include <NMTDS_MapOfPassKeyBoolean.hxx>
#include <NMTDS_IndexedDataMapOfShapeBox.hxx>
#include <NMTDS_IndexedDataMapOfIntegerShape.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <NMTDS_BoxBndTree.hxx>
#include <NCollection_UBTreeFiller.hxx>

static 
  void ComputeBoxEx(const Standard_Integer aIx,
		    NMTDS_ShapesDataStructure* pDS,
		    Bnd_Box& aBoxEx);

static inline
  Standard_Boolean IsValidType(const TopAbs_ShapeEnum aT);

static
  Standard_Integer TypeToInteger(const TopAbs_ShapeEnum aType1,
				 const TopAbs_ShapeEnum aType2);


//=======================================================================
//function : NMTDS_Iterator
//purpose  : 
//=======================================================================
  NMTDS_Iterator::NMTDS_Iterator()
{
  myPDS=NULL; 
  myLength=0;
}
//=======================================================================
//function : ~NMTDS_Iterator
//purpose  : 
//=======================================================================
  NMTDS_Iterator::~NMTDS_Iterator()
{
}
//=======================================================================
// function: SetDS
// purpose: 
//=======================================================================
  void NMTDS_Iterator::SetDS(const NMTDS_PShapesDataStructure& aDS)
{
  myPDS=aDS;
}
//=======================================================================
// function: DS
// purpose: 
//=======================================================================
  const NMTDS_ShapesDataStructure&  NMTDS_Iterator::DS()const
{
  return *myPDS;
}
//=======================================================================
// function: ExpectedLength
// purpose: 
//=======================================================================
  Standard_Integer NMTDS_Iterator::ExpectedLength() const
{
  return myLength;
}
//=======================================================================
// function: Initialize
// purpose: 
//=======================================================================
  void NMTDS_Iterator::Initialize(const TopAbs_ShapeEnum aType1,
				  const TopAbs_ShapeEnum aType2)
{
  Standard_Integer iX;
  //
  iX=TypeToInteger(aType1, aType2);
  if (iX>=0) {
    myIterator.Initialize(myLists[iX]);
    myLength=myLists[iX].Extent();
  }
  else {
    myIterator.Initialize(myEmptyList);
    myLength=0;
  }
}
//=======================================================================
// function: More
// purpose: 
//=======================================================================
  Standard_Boolean NMTDS_Iterator::More()const
{
  return myIterator.More();
}
//=======================================================================
// function: Next
// purpose: 
//=======================================================================
  void NMTDS_Iterator::Next()
{
  myIterator.Next();
}
//=======================================================================
// function: Current
// purpose: 
//=======================================================================
  void NMTDS_Iterator::Current(Standard_Integer& aIndex1,
			       Standard_Integer& aIndex2,
			       Standard_Boolean& aWithSubShape) const
{
  const NMTDS_PassKeyBoolean& aPKB=myIterator.Value();
  aPKB.Ids(aIndex1, aIndex2);
  aWithSubShape=aPKB.Flag();
}
//=======================================================================
// function: SDVertices
// purpose: 
//=======================================================================
  const TColStd_DataMapOfIntegerListOfInteger& NMTDS_Iterator::SDVertices()const
{
  return myMVSD;
}

//=======================================================================
// function: Prepare
// purpose: 
//=======================================================================
  void NMTDS_Iterator::Prepare()
{
  Standard_Boolean bFlag;
  Standard_Integer aNb, i, aNbB, aNbR;
  Standard_Integer i1, i2, aNbSD, iX, j, iDS, jB, iR, k, aNbLV;
  TColStd_ListIteratorOfListOfInteger aIt;
  TColStd_DataMapOfIntegerInteger aMII;
  TColStd_MapOfInteger aMFence;
  TopTools_DataMapOfShapeInteger aMSI;
  TopAbs_ShapeEnum aTi, aTj;
  NMTDS_PassKeyBoolean aPKXB; 
  NMTDS_MapOfPassKeyBoolean aMPKXB;
  NMTDS_IndexedDataMapOfShapeBox aMSB;
  Handle(Bnd_HArray1OfBox) aHAB;
  Bnd_BoundSortBox aBSB;
  //
  NMTDS_BoxBndTreeSelector aSelector;
  NMTDS_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  if (myPDS==NULL){
    return;
  }
  //
  myLength=0;
  for (i=0; i<6; ++i) {
    myLists[i].Clear();
  }
  myMVSD.Clear();
  //
  const NMTDS_CArray1OfIndexRange& aRanges=myPDS->Ranges();
  aNbR=aRanges.Extent();
  //
  aNb=myPDS->NumberOfShapesOfTheObject();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aS=myPDS->Shape(i);
    aTi=aS.ShapeType();
    if (IsValidType(aTi)) {
      Bnd_Box aBoxEx;
      //
      ComputeBoxEx(i, myPDS, aBoxEx);
      aMSI.Bind(aS, i);
      aMSB.Add(aS, aBoxEx);
    }
  }
  //
  aNbB=aMSB.Extent();
  //
  for (i=1; i<=aNbB; ++i) {
    const TopoDS_Shape& aS=aMSB.FindKey(i);
    const Bnd_Box& aBoxEx=aMSB(i);
    //
    aTreeFiller.Add(i, aBoxEx);
    //
    iDS=aMSI.Find(aS);
    aMII.Bind(i, iDS);
  }
  //
  aTreeFiller.Fill();
  //
  for (iR=1; iR<aNbR; ++iR) {
    const NMTDS_IndexRange& aR=aRanges(iR);
    i1=aR.First();
    i2=aR.Last();
    for (i=i1; i<=i2; ++i) {
      const TopoDS_Shape& aSi=myPDS->Shape(i);
      aTi=aSi.ShapeType();
      if (!IsValidType(aTi)){
	continue;
      }
      const Bnd_Box& aBoxEx=aMSB.FindFromKey(aSi);
      aSelector.Clear();
      aSelector.SetBox(aBoxEx);
      //
      aNbSD=aBBTree.Select(aSelector);
      //
      if (!aNbSD){
	continue;
      }
      //
      const TColStd_ListOfInteger& aLI=aSelector.Indices();
      //
      k=0;
      TColStd_ListOfInteger aLV;
      //
      aIt.Initialize(aLI);
      for (; aIt.More(); aIt.Next()) {
	jB=aIt.Value();  // box index in MII
	j=aMII.Find(jB); // DS index
	if (j>=i1 && j<=i2) {
	  continue;// same range
	}
	//
	aPKXB.SetIds(i, j);
	if (aMPKXB.Add(aPKXB)) {
	  bFlag=Standard_False;// Bounding boxes are intersected
	  const Bnd_Box& aBoxi=myPDS->GetBoundingBox(i);
	  const Bnd_Box& aBoxj=myPDS->GetBoundingBox(j);
	  if (aBoxi.IsOut(aBoxj)) {
	    bFlag=!bFlag; //Bounding boxes of Sub-shapes are intersected
	  }
	  const TopoDS_Shape& aSj=myPDS->Shape(j);
	  aTj=aSj.ShapeType();
	  iX=TypeToInteger(aTi, aTj);
	  
	  aPKXB.SetFlag(bFlag);
	  myLists[iX].Append(aPKXB);
	  //
	  // VSD prepare
	  if (iX==5) { //VV
	    if (aMFence.Add(j)) {
	      aLV.Append(j);
	    }
	  }
	}// if (aMPKXB.Add(aPKXB)) {
      }// for (; aIt.More(); aIt.Next()) {
      //
      // VSD treatment
      aNbLV=aLV.Extent();
      if (aNbLV) {
	myMVSD.Bind(i, aLV);
      }
    }//for (i=i1; i<=i2; ++i) {
  }//for (iR=1; iR<aNbR; ++iR) {
}

//=======================================================================
// function: IsValidType
// purpose: 
//=======================================================================
Standard_Boolean IsValidType(const TopAbs_ShapeEnum aTi)
{
  return (aTi==TopAbs_VERTEX || aTi==TopAbs_EDGE || aTi==TopAbs_FACE);
}
//=======================================================================
// function: ComputeBoxEx
// purpose: 
//=======================================================================
  void ComputeBoxEx(const Standard_Integer aIx,
		    NMTDS_ShapesDataStructure* pDS,
		    Bnd_Box& aBoxEx)
{
  Standard_Integer i, aNbS, iS;
  //
  const Bnd_Box& aBox=pDS->GetBoundingBox(aIx);
  aBoxEx.Add(aBox);
  //
  aNbS=pDS->NumberOfSuccessors(aIx);
  for (i=1; i<=aNbS; ++i) {
    Bnd_Box aBoxS;
    iS=pDS->GetSuccessor(aIx, i);
    ComputeBoxEx(iS, pDS, aBoxS);
    aBoxEx.Add(aBoxS);
  }
}

//=======================================================================
// function: TypeToInteger
// purpose: 
//=======================================================================
Standard_Integer TypeToInteger(const TopAbs_ShapeEnum aType1,
			       const TopAbs_ShapeEnum aType2)
{
  Standard_Integer iRet, iT1, iT2, iX;
  //
  iRet=-1;
  iT1=(Standard_Integer)aType1;
  iT2=(Standard_Integer)aType2;
  //
  iX=iT2*10+iT1;
  switch (iX) {
    case 77:
      iRet=5; // VV
      break;
    case 76:
    case 67:
      iRet=4; // VE
      break;
    case 74:
    case 47:
      iRet=2; // VF
      break;
    case 66:
      iRet=3; // EE
      break;
    case 64:
    case 46:
      iRet=1; // EF
      break;
    case 44:
      iRet=0; // FF
      break;
    default:
      break;
  }
  return iRet; 
}

/*
//
  // check
  TColStd_DataMapIteratorOfDataMapOfIntegerListOfInteger aIt1;
  TColStd_DataMapIteratorOfDataMapOfIntegerInteger aIt2;
  //
  printf(" \n");
  printf(" aMVLV.Extent()=%d\n", aMVLV.Extent());
  aIt1.Initialize(aMVLV);
  for (; aIt1.More(); aIt1.Next()) {
    i=aIt1.Key();
    printf(" i=%d (", i);
    const TColStd_ListOfInteger& aLV=aIt1.Value();
    aIt.Initialize(aLV);
    for (; aIt.More(); aIt.Next()) {
      j=aIt.Value();
      printf(" %d", j);
    }
    printf(")\n");
  }
  //
  printf(" \n");
  printf(" aMVV.Extent()=%d\n", aMVV.Extent());
  aIt2.Initialize(aMVV);
  for (; aIt2.More(); aIt2.Next()) {
    i=aIt2.Key();
    j=aIt2.Value();
    printf(" (%d, %d)\n", i, j);
  }
  printf("\n");
*/
