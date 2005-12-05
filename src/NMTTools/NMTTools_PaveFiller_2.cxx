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
// File:	NMTTools_PaveFiller_2.cxx
// Created:	Mon Dec  8 12:02:56 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>

#include <Precision.hxx>

#include <gp_Pnt.hxx>

#include <Geom_Curve.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BOPTools_Pave.hxx>
#include <BOPTools_PaveSet.hxx>
#include <BOPTools_CArray1OfVEInterference.hxx>
#include <BOPTools_VEInterference.hxx>

#include <BooleanOperations_AncestorsSeqAndSuccessorsSeq.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
//
#include <BOPTools_IndexedMapOfCoupleOfInteger.hxx>
#include <BOPTools_CoupleOfInteger.hxx>

//=======================================================================
// function: PrepareEdges
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PrepareEdges() 
{
  Standard_Integer  i, nV, ii, aNBSuc, ip, aNbShapesObject;
  Standard_Real aT;
  TopAbs_Orientation anOr;
  TopoDS_Edge   aE;
  TopoDS_Vertex aV;
  //
  aNbShapesObject=myDS->NumberOfShapesOfTheObject();
  for (i=1; i<=myNbSources; ++i) {
    if (myDS->GetShapeType(i)==TopAbs_EDGE) {
      aE=TopoDS::Edge(myDS->Shape(i));
      //
      if (BRep_Tool::Degenerated(aE)){
	continue;
      }
      //
      BOPTools_PaveSet& aPaveSet=myPavePool(myDS->RefEdge(i));
      //
      // A <-
      aNBSuc=myDS->NumberOfSuccessors(i);
      for (ii=1; ii <=aNBSuc; ii++) {
	nV=myDS->GetSuccessor(i, ii);
	anOr=myDS->GetOrientation(i, ii);
	aV=TopoDS::Vertex(myDS->Shape(nV));
	aV.Orientation(anOr);
	aT=BRep_Tool::Parameter(aV, aE);
	//
	ip=FindSDVertex(nV);
	if (ip) {
	  aV=TopoDS::Vertex(myDS->Shape(ip));
	  aV.Orientation(anOr);// XX ? if the edge is closed it'll be amazing result 
	  nV=ip;
	}
	//
	BOPTools_Pave aPave(nV, aT); 
	aPaveSet.Append (aPave);
      }
    }
  }
}
//=======================================================================
// function: PerformVE
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformVE() 
{
  myIsDone=Standard_False;
  //
  Standard_Boolean bJustAddInterference;
  Standard_Integer n1, n2, anIndexIn, aFlag, aWhat, aWith, aNbVEs, aBlockLength, iSDV, nV1;
  Standard_Real aT;
  TopoDS_Vertex aV1;
  TopoDS_Edge aE2;
  BOPTools_IndexedMapOfCoupleOfInteger aSnareMap;
  BOPTools_CoupleOfInteger aCouple;
  //
  BOPTools_CArray1OfVEInterference& aVEs=myIntrPool->VEInterferences();
  //
  myDSIt.Initialize (TopAbs_VERTEX, TopAbs_EDGE);
  //
  // BlockLength correction
  aNbVEs=ExpectedPoolLength();
  aBlockLength=aVEs.BlockLength();
  if (aNbVEs > aBlockLength) {
    aVEs.SetBlockLength(aNbVEs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, bJustAddInterference);
    //
    if (! myIntrPool->IsComputed(n1, n2)) {
      if (! IsSuccesstorsComputed(n1, n2)) {
	anIndexIn=0;
	aWhat=n1; // Vertex
	aWith=n2; // Edge
	SortTypes(aWhat, aWith);
	//
	if(bJustAddInterference) {
	 myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexEdge, anIndexIn);
	 continue;
	}
	// Edge
	aE2=TopoDS::Edge(myDS->Shape(aWith));
	//
	if (BRep_Tool::Degenerated(aE2)){
	  continue;
	}
	// Vertex
	nV1=aWhat;
	aV1=TopoDS::Vertex(myDS->Shape(aWhat));
	//
	iSDV=FindSDVertex(aWhat);
	if (iSDV) {
	  nV1=iSDV;
	  aV1=TopoDS::Vertex(myDS->Shape(nV1));
	}
	//
	aFlag=myContext.ComputeVE (aV1, aE2, aT);
	//
	if (!aFlag) {
	  //
	  // Add Interference to the Pool
	  BOPTools_VEInterference anInterf (aWhat, aWith, aT);
	  anIndexIn=aVEs.Append(anInterf);
	  //
	  // Add Pave to the Edge's myPavePool
	  aCouple.SetCouple(nV1, aWith);
	  if (!aSnareMap.Contains(aCouple)){
	    aSnareMap.Add(aCouple);
	    //
	    BOPTools_Pave aPave(nV1, aT, BooleanOperations_VertexEdge);
	    aPave.SetInterference(anIndexIn);
	    BOPTools_PaveSet& aPaveSet= myPavePool(myDS->RefEdge(aWith));
	    aPaveSet.Append(aPave);
	  }
	  //
	  // State for the Vertex in DS;
	  myDS->SetState (aWhat, BooleanOperations_ON);
	  // Insert Vertex in Interference Object
	  BOPTools_VEInterference& aVE=aVEs(anIndexIn);
	  aVE.SetNewShape(aWhat);
	}
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexEdge, anIndexIn);
      }
    }
  }
  myIsDone=Standard_True;
}

/*
// A
//
      //                                                   cto900/M2
      // Some of Edges can be [Semi] Infinite.  Such  Edges have no 
      // vertices on correspondant INF ends.   So we  must  provide 
      // these vertices formally (to obtain  Shrunk  Ranges for e.g). 
      // In reality this vertex(-es) does not belong to the INF Edge.
      // It just has reference in the DS.
      //                            PKV Tue Apr 23 10:21:45 2002                 
      {
	Standard_Real aT1, aT2, aTolE;
	Standard_Boolean bInf1, bInf2;
	gp_Pnt aPx;
	TopoDS_Vertex aVx; 
	BRep_Builder aBB;
	BooleanOperations_AncestorsSeqAndSuccessorsSeq anASSeq; 
	//
	aTolE=BRep_Tool::Tolerance(aE);
	Handle(Geom_Curve) aC3D=BRep_Tool::Curve (aE, aT1, aT2);
	bInf1=Precision::IsNegativeInfinite(aT1);
	bInf2=Precision::IsPositiveInfinite(aT2);

	if (bInf1) {
	  aC3D->D0(aT1, aPx);
	  aBB.MakeVertex(aVx, aPx, aTolE);
	  myDS->InsertShapeAndAncestorsSuccessors(aVx, anASSeq);
	  nV=myDS->NumberOfInsertedShapes();
	  BOPTools_Pave aPave(nV, aT1); 
	  aPaveSet.Append (aPave);
	}

	if (bInf2) {
	  aC3D->D0(aT2, aPx);
	  aBB.MakeVertex(aVx, aPx, aTolE);
	  myDS->InsertShapeAndAncestorsSuccessors(aVx, anASSeq);
	  nV=myDS->NumberOfInsertedShapes();
	  BOPTools_Pave aPave(nV, aT2);
	  aPaveSet.Append (aPave); 
	}
      }
*/
