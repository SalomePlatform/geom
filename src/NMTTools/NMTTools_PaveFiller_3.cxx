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
// File:	NMTTools_PaveFiller_3.cxx
// Created:	Mon Dec  8 16:06:56 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>

#include <BOPTools_VSInterference.hxx>
#include <BOPTools_CArray1OfVSInterference.hxx>

#include <NMTDS_ShapesDataStructure.hxx>

//=======================================================================
// function: PerformVF
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::PerformVF() 
{
  myIsDone=Standard_False;
  //
  Standard_Boolean aJustAddInterference;
  Standard_Integer n1, n2, anIndexIn, aFlag, aWhat, aWith, aNbVSs, aBlockLength, iSDV;
  Standard_Real aU, aV;
  TopoDS_Vertex aV1;
  TopoDS_Face aF2;
  //
  BOPTools_CArray1OfVSInterference& aVSs=myIntrPool->VSInterferences();
  //
  // V/E Interferences 
  myDSIt.Initialize(TopAbs_VERTEX, TopAbs_FACE);
  //
  // BlockLength correction
  aNbVSs=ExpectedPoolLength();
  aBlockLength=aVSs.BlockLength();
  if (aNbVSs > aBlockLength) {
    aVSs.SetBlockLength(aNbVSs);
  }
  //
  for (; myDSIt.More(); myDSIt.Next()) {
    myDSIt.Current(n1, n2, aJustAddInterference);
    //
    if (! myIntrPool->IsComputed(n1, n2)) {
      if (! IsSuccesstorsComputed(n1, n2)) {
	anIndexIn=0;
	aWhat=n1; // Vertex
	aWith=n2; // Face
	SortTypes(aWhat, aWith);
	//
	iSDV=FindSDVertex(aWhat);
	//
	if(aJustAddInterference) {
	  myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexSurface, anIndexIn);
	  continue;
	}
	//
	aV1=TopoDS::Vertex(myDS->Shape(aWhat));
	if (iSDV) {
	  aV1=TopoDS::Vertex(myDS->Shape(iSDV));
	}
	//
	aF2=TopoDS::Face(myDS->Shape(aWith));
	//
	aFlag=myContext.ComputeVS (aV1, aF2, aU, aV);
	//
	if (!aFlag) {
	  //
	  // Add Interference to the Pool
	  BOPTools_VSInterference anInterf (aWhat, aWith, aU, aV);
	  anIndexIn=aVSs.Append(anInterf);
	  //
	  // SetState for Vertex in DS;
	  myDS->SetState (aWhat, BooleanOperations_ON);
	  // Insert Vertex in Interference Object
	  BOPTools_VSInterference& aVS=aVSs(anIndexIn);
	  aVS.SetNewShape(aWhat);
	}
	myIntrPool->AddInterference(aWhat, aWith, BooleanOperations_VertexSurface, anIndexIn);
      }
    }
  }
  myIsDone=Standard_True;
}
