// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

// File:        GEOMAlgo_WireSolid.cxx
// Created:     Wed Jan 12 10:19:31 2005
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_WireSolid.hxx>

#include <Basics_OCCTVersion.hxx>

#include <Standard_Failure.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <TopoDS_Solid.hxx>
#if OCC_VERSION_LARGE > 0x06070100
#include <IntTools_Context.hxx>
#else
#include <BOPInt_Context.hxx>
#endif
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRep_Tool.hxx>
#include <BOPTools_AlgoTools.hxx>

#include <Basics_OCCTVersion.hxx>

//=======================================================================
//function : GEOMAlgo_WireSolid
//purpose  :
//=======================================================================
GEOMAlgo_WireSolid::GEOMAlgo_WireSolid()
:
  GEOMAlgo_ShapeSolid()
{
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_WireSolid::~GEOMAlgo_WireSolid()
{
}
//=======================================================================
// function: Perform
// purpose:
//=======================================================================
void GEOMAlgo_WireSolid::Perform()
{
  myErrorStatus=0;
  //
  try {
    if (myDSFiller==NULL) {
      myErrorStatus=10;
      return;
    }
    if(myDSFiller->ErrorStatus()) {
      myErrorStatus=11;
      return;
    }
    //
    Standard_Integer aNbArgs;
    //
    const BOPDS_DS& aDS=myDSFiller->DS();
    const BOPCol_ListOfShape& aLS=aDS.Arguments();
    aNbArgs=aLS.Extent();
    if (!aNbArgs) {
      myErrorStatus=13;
      return;
    }
    //
    BuildResult();
  }
  //
  catch (Standard_Failure) {
    myErrorStatus= 12;
  }
}
//=======================================================================
// function: BuildResult
// purpose:
//=======================================================================
void GEOMAlgo_WireSolid::BuildResult()
{
  Standard_Boolean bHasPaveBlocks;
  Standard_Integer i, iRank, aNbPB,  iBeg, iEnd, aNbArgs, nE;// nSp
  Standard_Real aTol;
  TopAbs_ShapeEnum aType;
  TopAbs_State aState;
  TopoDS_Edge aE;
  //
  myErrorStatus=0;
  myLSIN.Clear();
  myLSOUT.Clear();
  myLSON.Clear();
  //
  const BOPDS_DS& aDS=myDSFiller->DS();
  BOPDS_DS* pDS=(BOPDS_DS*)&aDS;
  //
  const BOPCol_ListOfShape& aLS=pDS->Arguments();
  aNbArgs=aLS.Extent();
  if (aNbArgs!=2) {
    myErrorStatus=14;
    return;
  }
  //
  iRank=-1;
  const TopoDS_Shape& aObj=aLS.First();
  if (aObj.ShapeType()==TopAbs_WIRE) {
    iRank=0;
  }
  const TopoDS_Shape& aTool=aLS.Last();
  if (aTool.ShapeType()==TopAbs_WIRE) {
    iRank=1;
  }
  //
  if (iRank==-1) {
    myErrorStatus=15;
    return;
  }
  //
  aTol=1.e-7;
  //
  const TopoDS_Solid& aSolid=(iRank==0) ?  *((TopoDS_Solid*)&aTool) :
    *((TopoDS_Solid*)&aObj);
  //
#if OCC_VERSION_LARGE > 0x06070100
  Handle(IntTools_Context) aCtx=myDSFiller->Context();
#else
  Handle(BOPInt_Context) aCtx=myDSFiller->Context();
#endif
  //BRepClass3d_SolidClassifier& aSC=aCtx->SolidClassifier(aSolid);
  //
  const BOPDS_IndexRange& aRange=pDS->Range(iRank);
  aRange.Indices(iBeg, iEnd);
  //
  for (i=iBeg; i<=iEnd; ++i) {
    const TopoDS_Shape& aS=pDS->Shape(i);
    aType=aS.ShapeType();
    if (aType!=TopAbs_EDGE) {
      continue;
    }
    //
    aE=*((TopoDS_Edge*)&pDS->Shape(i));
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //
    bHasPaveBlocks=pDS->HasPaveBlocks(i);
    if (!bHasPaveBlocks) {
      continue;
    }
    //
    aState=TopAbs_UNKNOWN;
    //
    const BOPDS_ListOfPaveBlock& aLPB=pDS->PaveBlocks(i);
    aNbPB=aLPB.Extent();
    if (!aNbPB) {
      aState=BOPTools_AlgoTools::ComputeStateByOnePoint(aE, aSolid, aTol, aCtx);
    }
    else if (aNbPB==1) {
      const Handle(BOPDS_PaveBlock)& aPB=aLPB.First();
      if (pDS->IsCommonBlock(aPB)) {
	aState=TopAbs_ON;
      }
      else{
	nE=aPB->Edge();
	aE=*((TopoDS_Edge*)&pDS->Shape(nE));
	aState=BOPTools_AlgoTools::ComputeStateByOnePoint(aE, aSolid, aTol, aCtx);
      }
      //----------
      if (aState==TopAbs_ON) {
	myLSON.Append(aE);
      }
      else if (aState==TopAbs_OUT) {
	myLSOUT.Append(aE);
      }
      else if (aState==TopAbs_IN) {
	myLSIN.Append(aE);
      }
    }
  }
}
