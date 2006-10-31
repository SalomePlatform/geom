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
// File:	NMTAlgo_WESCorrector.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_WESCorrector.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BOP_ConnexityBlock.hxx>
#include <BOP_ListIteratorOfListOfConnexityBlock.hxx>

#include <BOPTColStd_ListOfListOfShape.hxx>
#include <BOPTColStd_ListIteratorOfListOfListOfShape.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <GEOMAlgo_WireSplitter.hxx>


static
  void Path(const TopoDS_Shape& aV1,
	    const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
	    TopTools_MapOfShape& aMVP,
	    TopTools_MapOfShape& aMEP,
	    TopTools_MapOfShape& aMEC);
static
  void MakeWire(const TopTools_ListOfShape& aLE, 
		TopoDS_Wire& newWire);

//=======================================================================
// function: 
// purpose: 
//=======================================================================
  GEOMAlgo_WESCorrector::GEOMAlgo_WESCorrector()
:
  GEOMAlgo_Algo()
{
}
//=======================================================================
// function: ~
// purpose: 
//=======================================================================
  GEOMAlgo_WESCorrector::~GEOMAlgo_WESCorrector()
{
}
//=======================================================================
// function: SetWES
// purpose: 
//=======================================================================
  void GEOMAlgo_WESCorrector::SetWES (const GEOMAlgo_WireEdgeSet& aWES)
{
  GEOMAlgo_WireEdgeSet* pWES=(GEOMAlgo_WireEdgeSet*) &aWES;
  myWES=pWES;
}
//=======================================================================
// function: WES
// purpose: 
//=======================================================================
  GEOMAlgo_WireEdgeSet& GEOMAlgo_WESCorrector::WES () 
{
  return *myWES;
}
//=======================================================================
// function: NewWES
// purpose: 
//=======================================================================
  GEOMAlgo_WireEdgeSet& GEOMAlgo_WESCorrector::NewWES () 
{
  return myNewWES;
}
//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
  void GEOMAlgo_WESCorrector::Perform()
{
  myErrorStatus=0;
  //
  DoConnexityBlocks();
  DoCorrections();
}
//=======================================================================
// function: DoConnexityBlocks
// purpose: 
//=======================================================================
  void GEOMAlgo_WESCorrector::DoConnexityBlocks()
{
  Standard_Boolean bRegular;
  Standard_Integer i, aNbV;
  TopTools_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator aItE;
  TopoDS_Shape aER;
  TopTools_MapOfShape aMER, aMEP, aMEC, aMVP;
  TopTools_MapIteratorOfMapOfShape aItM;
  TopTools_IndexedDataMapOfShapeListOfShape aMVE;
  //
  // 1. aMVE;
  const TopTools_ListOfShape& aLE=myWES->StartElements();
  aIt.Initialize(aLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (aMEP.Add(aE)) {
      TopExp::MapShapesAndAncestors(aE, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
    }
    else {
      aMER.Add(aE);
    }
  }
  //
  aMEP.Clear();
  //
  // 2. 
  aNbV=aMVE.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMVE.FindKey(i);
    if (aMVP.Contains(aV)) {
      continue;
    }
    aMVP.Add(aV);
    //
    const TopTools_ListOfShape& aLEi=aMVE(i);
    aIt.Initialize(aLEi);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aE=aIt.Value();
      if (aMEP.Contains(aE)) {
	continue;
      }
      //
      aMEP.Add(aE);
      aMEC.Add(aE);
      aItE.Initialize(aE);
      for (; aItE.More(); aItE.Next()) {
	const TopoDS_Shape& aVE=aItE.Value();
	Path(aVE, aMVE, aMVP, aMEP, aMEC);
      }
    }//for (; aIt.More(); aIt.Next()) {
    //
    BOP_ConnexityBlock aCB;
    TopTools_ListOfShape aLEC;
    TopTools_IndexedDataMapOfShapeListOfShape aMVER;
    //
    bRegular=Standard_True;
    aItM.Initialize(aMEC);
    for (; aItM.More(); aItM.Next()) {
      aER=aItM.Key();
      //
      if (aMER.Contains(aER)) {
	Standard_Boolean bClosed;
	//
	aER.Orientation(TopAbs_FORWARD);
	aLEC.Append(aER);
	aER.Orientation(TopAbs_REVERSED);
	aLEC.Append(aER);
	//
	bClosed=BRep_Tool::IsClosed(TopoDS::Edge(aER), myWES->Face());
	if (!bClosed) {
	  bRegular=Standard_False;
	}
      }
      else {
	aLEC.Append(aER);
      }
      //
      if (bRegular) {
	TopExp::MapShapesAndAncestors(aER, TopAbs_VERTEX, TopAbs_EDGE, aMVER);
      }
    }
    //
    if (bRegular) {
      Standard_Integer j, aNbVR, aNbER;
      //
      aNbVR=aMVER.Extent();
      for (j=1; j<=aNbVR; ++j) {
	const TopTools_ListOfShape& aLER=aMVE(j);
	aNbER=aLER.Extent();
	if (aNbER==1) {
	  const TopoDS_Edge& aEx=TopoDS::Edge(aER);
	  if (!BRep_Tool::IsClosed(aEx, myWES->Face())) {
	    bRegular=!bRegular;
	    break;
	  }
	}
	if (aNbER>2) {
	  bRegular=!bRegular;
	  break;
	}
      }
    }
    //
    aCB.SetShapes(aLEC);
    aCB.SetRegularity(bRegular);
    myConnexityBlocks.Append(aCB);
    aMEC.Clear();
  }//for (i=1; i<=aNbV; ++i) {
}
//=======================================================================
// function: Path
// purpose: 
//=======================================================================
void Path(const TopoDS_Shape& aV1,
	  const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
	  TopTools_MapOfShape& aMVP,
	  TopTools_MapOfShape& aMEP,
	  TopTools_MapOfShape& aMEC)
{
  TopoDS_Iterator aItE;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  if (aMVP.Contains(aV1)) {
    return;
  }
  aMVP.Add(aV1);
  //
  const TopTools_ListOfShape& aLE=aMVE.FindFromKey(aV1);
  aIt.Initialize(aLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    if (aMEP.Contains(aE)) {
      continue;
    }
    aMEP.Add(aE);
    aMEC.Add(aE);
    //
    aItE.Initialize(aE);
    for (; aItE.More(); aItE.Next()) {
      const TopoDS_Shape& aVE=aItE.Value();
      Path(aVE, aMVE, aMVP, aMEP, aMEC);
    }
  }
}
//=======================================================================
// function: DoCorrections
// purpose: 
//=======================================================================
  void GEOMAlgo_WESCorrector::DoCorrections()
{
  Standard_Boolean bIsRegular, bIsNothingToDo;
  Standard_Integer iErr;
  TopoDS_Wire aW;
  BOP_ListIteratorOfListOfConnexityBlock aCBIt;
  //
  const TopoDS_Face& aF=myWES->Face();
  //
  myNewWES.SetFace(aF);
  aCBIt.Initialize(myConnexityBlocks);
  for (; aCBIt.More(); aCBIt.Next()) {
    const BOP_ConnexityBlock& aCB=aCBIt.Value();
    const TopTools_ListOfShape& aLE=aCB.Shapes();

    bIsRegular=aCB.IsRegular();

    if (bIsRegular) {
      MakeWire(aLE, aW);
      myNewWES.AddShape (aW);
      continue;
    }
    //
    GEOMAlgo_WireSplitter aWS;
    //
    aWS.SetFace(aF);
    aWS.SetEdges(aLE);
    //
    aWS.Perform();
    iErr=aWS.ErrorStatus();
    if (iErr) {
      continue;
    }
    bIsNothingToDo=aWS.IsNothingToDo();
    if (bIsNothingToDo) {
      MakeWire(aLE, aW);
      myNewWES.AddShape (aW);
      continue;
    }
    //
    const BOPTColStd_ListOfListOfShape& aSSS=aWS.Shapes();
    
    BOPTColStd_ListIteratorOfListOfListOfShape aWireIt(aSSS);
    for (; aWireIt.More(); aWireIt.Next()) {
      const TopTools_ListOfShape& aLEx=aWireIt.Value();
      //
      MakeWire(aLEx, aW);
      myNewWES.AddShape (aW);
    }
  }
}
//=======================================================================
// function: MakeWire
// purpose: 
//=======================================================================
  void MakeWire(const TopTools_ListOfShape& aLE, 
		TopoDS_Wire& newWire)
{
  BRep_Builder aBB;
  aBB.MakeWire(newWire);

  TopTools_ListIteratorOfListOfShape anIt(aLE);
  for (; anIt.More(); anIt.Next()){
    const TopoDS_Edge& aE=TopoDS::Edge(anIt.Value());
    aBB.Add(newWire, aE);
  }
}
