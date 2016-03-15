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

// File:        GEOMAlgo_ShellSolid.cxx
// Created:     Wed Jan 12 12:49:45 2005
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_ShellSolid.hxx>

#include <Basics_OCCTVersion.hxx>

#include <Standard_Failure.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPTools_AlgoTools.hxx>

#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#if OCC_VERSION_LARGE > 0x06070100
#include <IntTools_Context.hxx>
#else
#include <BOPInt_Context.hxx>
#endif
#include <BOPDS_DS.hxx>
#include <BOPAlgo_Builder.hxx>

#include <GEOMAlgo_AlgoTools.hxx>
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class : GEOMAlgo_ShellSolidBuilder
//purpose  : 
//=======================================================================
class GEOMAlgo_ShellSolidBuilder : public BOPAlgo_Builder {
 public:
  Standard_EXPORT
    GEOMAlgo_ShellSolidBuilder();

  Standard_EXPORT
    virtual ~GEOMAlgo_ShellSolidBuilder();

 protected:
  Standard_EXPORT
    virtual void PerformInternal(const BOPAlgo_PaveFiller& theFiller);
};

//=======================================================================
//function : GEOMAlgo_ShellSolidBuilder
//purpose  : 
//=======================================================================
GEOMAlgo_ShellSolidBuilder::GEOMAlgo_ShellSolidBuilder()
:
  BOPAlgo_Builder()
{
}
//=======================================================================
//function : ~GEOMAlgo_ShellSolidBuilder
//purpose  : 
//=======================================================================
GEOMAlgo_ShellSolidBuilder::~GEOMAlgo_ShellSolidBuilder()
{
}
//=======================================================================
//function : PerformInternal
//purpose  : 
//=======================================================================
void GEOMAlgo_ShellSolidBuilder::PerformInternal(const BOPAlgo_PaveFiller& theFiller)
{
  myErrorStatus=0;
   //
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (myErrorStatus) {
    return;
  }
  //
  // 3. Fill Images
  // 3.1 Vertice
  FillImagesVertices();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_VERTEX);
  if (myErrorStatus) {
    return;
  }
  // 3.2 Edges
  FillImagesEdges();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_EDGE);
  if (myErrorStatus) {
    return;
  } 
  //
  // 3.3 Wires
  FillImagesContainers(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_WIRE);
  if (myErrorStatus) {
    return;
  }
  
  // 3.4 Faces
  FillImagesFaces();
  if (myErrorStatus) {
    return;
  }
  //
  BuildResult(TopAbs_FACE);
  if (myErrorStatus) {
    return;
  }
}
/////////////////////////////////////////////////////////////////////////
//=======================================================================
//function : GEOMAlgo_ShellSolid
//purpose  :
//=======================================================================
GEOMAlgo_ShellSolid::GEOMAlgo_ShellSolid()
:
  GEOMAlgo_ShapeSolid()
{
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_ShellSolid::~GEOMAlgo_ShellSolid()
{
}
//=======================================================================
// function:
// purpose:
//=======================================================================
void GEOMAlgo_ShellSolid::Perform()
{
  myErrorStatus=0;
  //
  try {
    Standard_Integer aNbArgs, iRank, iErr, iBeg, iEnd, i, aNbSp;
    Standard_Real aTol;
    TopAbs_ShapeEnum aType;
    TopAbs_State aState;
    gp_Pnt aP;
    gp_Pnt2d aP2D;
    TopoDS_Face aF;
    //
    myLSIN.Clear();
    myLSOUT.Clear();
    myLSON.Clear();
    //
    aTol=1.e-7;
    //
    if (myDSFiller==NULL) {
      myErrorStatus=10;
      return;
    }
    if(myDSFiller->ErrorStatus()) {
      myErrorStatus=11;
      return;
    }
    //
    const BOPDS_DS& aDS=myDSFiller->DS();
    BOPDS_DS* pDS=(BOPDS_DS*)&aDS;
    const BOPCol_ListOfShape& aLS=pDS->Arguments();
    //
    aNbArgs=aLS.Extent();
    if (aNbArgs!=2) {
      myErrorStatus=13;
      return;
    }
    //
    iRank=-1;
    const TopoDS_Shape& aObj=aLS.First();
    if (aObj.ShapeType()==TopAbs_SHELL) {
      iRank=0;
    }
    const TopoDS_Shape& aTool=aLS.Last();
    if (aTool.ShapeType()==TopAbs_SHELL) {
      iRank=1;
    }
    //
    if (iRank==-1) {
      myErrorStatus=14;
      return;
    }
    //
#if OCC_VERSION_LARGE > 0x06070100
    Handle(IntTools_Context) aCtx=myDSFiller->Context();
#else
    Handle(BOPInt_Context) aCtx=myDSFiller->Context();
#endif
    const BOPDS_IndexRange& aRange=pDS->Range(iRank);
    aRange.Indices(iBeg, iEnd);
    const TopoDS_Solid& aSolid=(!iRank) ? *((TopoDS_Solid*)&aTool) : *((TopoDS_Solid*)&aObj);
    //BRepClass3d_SolidClassifier& aSC=aCtx->SolidClassifier(aSolid);
    //
    //------------------------------ShellSolidBuilder
    GEOMAlgo_ShellSolidBuilder aSSB;
    //
    aSSB.PerformWithFiller(*myDSFiller);
    iErr=aSSB.ErrorStatus();
    if (iErr) {
      myErrorStatus=15;
      return;
    }
    //
    const BOPCol_DataMapOfShapeListOfShape& aImages=aSSB.Images();
    //
    //-------------------------------
    for (i=iBeg; i<=iEnd; ++i) {
      const TopoDS_Shape& aS=pDS->Shape(i);
      aType=aS.ShapeType();
      if (aType!=TopAbs_FACE) {
        continue;
      }
      //
      aState=TopAbs_UNKNOWN;
      aF=*((TopoDS_Face*)&aS);
      //
      if (!aImages.IsBound(aS)) {
        iErr=GEOMAlgo_AlgoTools::PntInFace(aF, aP, aP2D);
        if (iErr) {
          myErrorStatus=16;
          return;
        }
        //
        aState=BOPTools_AlgoTools::ComputeState(aP, aSolid, aTol, aCtx);
      }
      else {
        const BOPCol_ListOfShape& aLSp=aImages.Find(aS);
        aNbSp=aLSp.Extent();
        if (aNbSp>0) {
          continue;
        }
        //
        if (aNbSp==1) {
          aF=*((TopoDS_Face*)&aLSp.First());
        }
        //
        iErr=GEOMAlgo_AlgoTools::PntInFace(aF, aP, aP2D);
        if (iErr) {
          myErrorStatus=16;
          return;
        }
        //
        aState=BOPTools_AlgoTools::ComputeState(aP, aSolid, aTol, aCtx);
      }
      //----------
      if (aState==TopAbs_ON) {
        myLSON.Append(aF);
      }
      else if (aState==TopAbs_OUT) {
        myLSOUT.Append(aF);
      }
      else if (aState==TopAbs_IN) {
        myLSIN.Append(aF);
      } 
      //----------
    }//for (i=iBeg; i<=iEnd; ++i) {
    
  }// try
  catch (Standard_Failure) {
    myErrorStatus=12;
  }
}
