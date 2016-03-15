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

// File:        GEOMAlgo_VertexSolid.cxx
// Created:     Wed Jan 12 16:36:40 2005
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_VertexSolid.hxx>

#include <Basics_OCCTVersion.hxx>

#include <gp_Pnt.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_State.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>

#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
//
#include <BOPCol_ListOfShape.hxx>
#if OCC_VERSION_LARGE > 0x06070100
#include <IntTools_Context.hxx>
#else
#include <BOPInt_Context.hxx>
#endif
//
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_Interf.hxx>

//=======================================================================
//function : GEOMAlgo_VertexSolid
//purpose  :
//=======================================================================
GEOMAlgo_VertexSolid::GEOMAlgo_VertexSolid()
:
  GEOMAlgo_ShapeSolid()
{
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_VertexSolid::~GEOMAlgo_VertexSolid()
{
}
//=======================================================================
// function: Perform
// purpose:
//=======================================================================
void GEOMAlgo_VertexSolid::Perform()
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
    Standard_Integer aNbF, aNbArgs;
    TopTools_IndexedMapOfShape aM;
    //
    const BOPDS_DS& aDS=myDSFiller->DS();
    const BOPCol_ListOfShape& aLS=aDS.Arguments();
    aNbArgs=aLS.Extent();
    if (aNbArgs!=2) {
      myErrorStatus=14;
      return;
    }
    
    const TopoDS_Shape& aObj=aLS.First();
    //
    TopExp::MapShapes(aObj, TopAbs_FACE, aM);
    aNbF=aM.Extent();
    myRank=(aNbF) ? 1 : 0;
    //
    BuildResult();
  }
  //
  catch (Standard_Failure) {
    myErrorStatus = 12;
  }
}
//=======================================================================
// function: BuildResult
// purpose:
//=======================================================================
void GEOMAlgo_VertexSolid::BuildResult()
{
  Standard_Integer i, iBeg, iEnd, aNbVV, aNbVE, aNbVF, j, iFound, aNbRanges;
  Standard_Real aTol;
  TopAbs_State aSt;
  TopAbs_ShapeEnum aType;
  gp_Pnt aP3D;
  //
  myLSIN.Clear();
  myLSOUT.Clear();
  myLSON.Clear();
  //
  const BOPDS_DS& aDS=myDSFiller->DS();
  BOPDS_DS* pDS=(BOPDS_DS*)&aDS;
  //
  BOPDS_VectorOfInterfVV& aVVs=pDS->InterfVV();
  BOPDS_VectorOfInterfVE& aVEs=pDS->InterfVE();
  BOPDS_VectorOfInterfVF& aVFs=pDS->InterfVF();
  //
  const BOPCol_ListOfShape& aLS=aDS.Arguments();
  const TopoDS_Shape& aObj=aLS.First();
  //
  const TopoDS_Shape& aTool=aLS.Last();
  const TopoDS_Solid& aSolid=(myRank==0) ? TopoDS::Solid(aTool) : TopoDS::Solid(aObj);
  //
#if OCC_VERSION_LARGE > 0x06070100
  Handle(IntTools_Context) aCtx=myDSFiller->Context();
#else
  Handle(BOPInt_Context) aCtx=myDSFiller->Context();
#endif
  BRepClass3d_SolidClassifier& aSC=aCtx->SolidClassifier(aSolid);
  //
  aNbRanges=aDS.NbRanges();
  const BOPDS_IndexRange& aRange=aDS.Range(myRank);
  aRange.Indices(iBeg, iEnd);
  //
  for (i=iBeg; i<=iEnd; ++i) {
    const TopoDS_Shape& aS=aDS.Shape(i);
    aType=aS.ShapeType();
    if (aType!=TopAbs_VERTEX) {
      continue; 
    }
    //
    const TopoDS_Vertex& aV=TopoDS::Vertex(aS);
    //
    iFound=0;
    //
    // 1
    aNbVV=aVVs.Extent();
    for (j=0; j<aNbVV; ++j) {
      BOPDS_InterfVV& aVV=aVVs(j);
      if (aVV.Contains(i)) {
	myLSON.Append(aV);
	iFound=1;
	break;
      }
    }
    if (iFound) {
      continue; 
    }
    // 2
    aNbVE=aVEs.Extent();
    for (j=0; j<aNbVE; ++j) {
      BOPDS_InterfVE& aVE=aVEs(j);
      if (aVE.Contains(i)) {
	myLSON.Append(aV);
	iFound=1;
	break;
      }
    }
    if (iFound) {
      continue; 
    }
    // 3
    aNbVF=aVFs.Extent();
    for (j=0; j<aNbVF; ++j) {
      BOPDS_InterfVF& aVF=aVFs(j);
      if (aVF.Contains(i)) {
	myLSON.Append(aV);
	iFound=1;
	break;
      }
    }
    if (iFound) {
      continue; 
    }
    //
    // 4
    aP3D=BRep_Tool::Pnt(aV);
    aTol=1.e-7;
    aSC.Perform(aP3D, aTol);
    aSt=aSC.State();
    if (aSt==TopAbs_IN) {
      myLSIN.Append(aV);
    }
    else if (aSt==TopAbs_OUT) {
      myLSOUT.Append(aV);
    }
  }//for (i=iBeg; i<iEnd; ++i) {
}
