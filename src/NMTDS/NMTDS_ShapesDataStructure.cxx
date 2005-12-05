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
// File:	NMTDS_ShapesDataStructure.cxx
// Created:	Mon Dec  1 10:21:04 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTDS_ShapesDataStructure.ixx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors.hxx>
#include <NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx>
#include <NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors.hxx>
#include <BooleanOperations_ShapeAndInterferences.hxx>
#include <NMTDS_IndexRange.hxx>

//===========================================================================
//function : NMTDS_ShapesDataStructure::NMTDS_ShapesDataStructure
//purpose  : 
//===========================================================================
  NMTDS_ShapesDataStructure::NMTDS_ShapesDataStructure()
:
  BooleanOperations_ShapesDataStructure()
{
  //printf(" NMTDS_ShapesDataStructure CREATE:%x\n", (int)this);
}
//modified by NIZNHY-PKV Wed Feb  2 11:45:04 2005f
//===========================================================================
//function : ~
//purpose  : 
//===========================================================================
  NMTDS_ShapesDataStructure::~NMTDS_ShapesDataStructure()
{
  //printf(" NMTDS_ShapesDataStructure DELETE:%x\n", (int)this);
}
//modified by NIZNHY-PKV Wed Feb  2 11:45:06 2005t
//===========================================================================
//function : SetCompositeShape
//purpose  : 
//===========================================================================
  void NMTDS_ShapesDataStructure::SetCompositeShape(const TopoDS_Shape& aS)
{
  myCompositeShape=aS;
}
//===========================================================================
//function : CompositeShape
//purpose  : 
//===========================================================================
  const TopoDS_Shape& NMTDS_ShapesDataStructure::CompositeShape()const
{
  return myCompositeShape;
}
//===========================================================================
//function : Ranges
//purpose  : 
//===========================================================================
  const NMTDS_CArray1OfIndexRange& NMTDS_ShapesDataStructure::Ranges()const
{
  return myRanges;
}
//===========================================================================
//function : Init
//purpose  : 
//===========================================================================
  void NMTDS_ShapesDataStructure::Init()
{
  Standard_Integer i, j, aNbSx, aNbS, aShift, aNbRanges;
  Standard_Integer iFirst, iLast;
  NMTDS_ListOfIndexedDataMapOfShapeAncestorsSuccessors aLx;
  NMTDS_ListIteratorOfListOfIndexedDataMapOfShapeAncestorsSuccessors aLit;
  TopoDS_Iterator anIt;
  //
  aNbS=0;
  anIt.Initialize(myCompositeShape);
  for (i=0; anIt.More(); anIt.Next(), ++i) {
    const TopoDS_Shape& aSx=anIt.Value();
    BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors aIndDatMapShape;
    FillIndexedMapOfShapesAncestorsAndSuccessors(aSx, aIndDatMapShape);
    aNbSx=aIndDatMapShape.Extent();
    aNbS+=aNbSx;
    aLx.Append(aIndDatMapShape);
  }
  //
  // Fill myRanges
  myRanges.Resize(i);
  aLit.Initialize(aLx);
  for (i=1; aLit.More(); aLit.Next(), ++i) {
    const BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors& aM=aLit.Value();
    aNbSx=aM.Extent();
    if (i==1) {
      iFirst=1;
      iLast=aNbSx;
      myRanges(i).SetFirst(iFirst);
      myRanges(i).SetLast(iLast);
      continue;
    }
    iFirst=myRanges(i-1).Last()+1;
    iLast=iFirst+aNbSx-1;
    myRanges(i).SetFirst(iFirst);
    myRanges(i).SetLast(iLast);
  }
  //
  myNumberOfShapesOfTheObject=aNbS;
  myNumberOfShapesOfTheTool=aNbS;
  myLength=3*aNbS;
  //
  // Allocate the whole Table
  myListOfShapeAndInterferences = (BooleanOperations_PShapeAndInterferences)
    Standard::Allocate(myLength*sizeof(BooleanOperations_ShapeAndInterferences));
  //
  // Fill the table
  for (i=0; i<2; ++i) {
    aShift=0;
    if (i) {
      aShift=myNumberOfShapesOfTheObject;
    }
    aLit.Initialize(aLx);
    for (; aLit.More(); aLit.Next()) {
      const BooleanOperations_IndexedDataMapOfShapeAncestorsSuccessors& aM=aLit.Value();
      aNbSx=aM.Extent();
      for (j=1; j<=aNbSx; ++j) {
	const TopoDS_Shape& aSx=aM.FindKey(j);
	const BooleanOperations_AncestorsSeqAndSuccessorsSeq& aASx=aM.FindFromIndex(j);
	InsertShapeAndAncestorsSuccessors(aSx, aASx, aShift);
      }
      aShift+=aNbSx;
    }
  }
  //
  // myShapeIndexMap
  myShapeIndexMap.Clear();
  //
  aNbRanges=myRanges.Extent();
  for (i=1; i<=aNbRanges; ++i){
    BooleanOperations_IndexedDataMapOfShapeInteger aSIM;
    //
    const NMTDS_IndexRange& aR=myRanges(i);
    iFirst=aR.First();
    iLast =aR.Last();
    for (j=iFirst; j<=iLast; ++j) { 
      const TopoDS_Shape& aS=GetShape(j);
      aSIM.Add(aS, j);
    }
    myShapeIndexMap.Add(i, aSIM);
  }
  //
  // myRefEdges
  iLast=myNumberOfShapesOfTheObject+myNumberOfShapesOfTheTool;
  myRefEdges.Resize(iLast);

  for (i=1; i<=iLast; ++i) {
    const TopoDS_Shape& aS=Shape(i);
    myRefEdges(i)=0;
    if (aS.ShapeType()==TopAbs_EDGE) {
      myNbEdges++;
      myRefEdges(i)=myNbEdges;
    }
  }
}
//===========================================================================
//function : ShapeRangeIndex
//purpose  : 
//===========================================================================
  Standard_Integer NMTDS_ShapesDataStructure::ShapeRangeIndex(const Standard_Integer aId)const
{
  Standard_Boolean bFound;
  Standard_Integer i, aNbR, aIdx, aNbS, aZero=0;
  //
  aNbS=myNumberOfShapesOfTheObject+myNumberOfShapesOfTheTool;
  
  aIdx=aId;
  if (aIdx > aNbS || aIdx < 1){
    return aZero;
  }
  //  
  if (aIdx > myNumberOfShapesOfTheObject) {
    aIdx-=myNumberOfShapesOfTheObject;
  }
  //
  aNbR=myRanges.Extent();
  for (i=1; i<=aNbR; ++i) {
    const NMTDS_IndexRange& aRange=myRanges(i);
    bFound=aRange.IsInRange(aIdx);
    if (bFound) {
     return i;
    }
  }
  return aZero;
}
//===========================================================================
//function : Rank
//purpose  : 
//===========================================================================
  Standard_Integer NMTDS_ShapesDataStructure::Rank(const Standard_Integer aId)const
{
  Standard_Boolean bFound;
  Standard_Integer i, aNbR, aNbS, aZero=0;
  //
  aNbS=myNumberOfShapesOfTheObject;
  
  if (aId > aNbS || aId < 1){
    return aZero;
  }
  //  
  aNbR=myRanges.Extent();
  for (i=1; i<=aNbR; ++i) {
    const NMTDS_IndexRange& aRange=myRanges(i);
    bFound=aRange.IsInRange(aId);
    if (bFound) {
     return i;
    }
  }
  return aZero;
}
//===========================================================================
//function : ShapeIndex
//purpose  : 
//===========================================================================
  Standard_Integer NMTDS_ShapesDataStructure::ShapeIndex(const TopoDS_Shape& aS,
							 const Standard_Integer aRank)const
{
  Standard_Boolean bFound;
  Standard_Integer aIndex=0;
  //
  bFound=myShapeIndexMap.Contains(aRank);
  if (!bFound) {
    return aIndex;
  }
  //
  const BooleanOperations_IndexedDataMapOfShapeInteger& aSIM=myShapeIndexMap.FindFromKey(aRank);
  //
  bFound=aSIM.Contains(aS);
  if (!bFound) {
    return aIndex;
  }
  //
  aIndex=aSIM.FindFromKey(aS);
  return aIndex;
}
