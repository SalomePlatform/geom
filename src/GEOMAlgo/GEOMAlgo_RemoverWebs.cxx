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
// File:        GEOMAlgo_RemoverWebs.cxx
// Created:     Thu Mar 28 07:40:32 2013
// Author:      Peter KURNEV

#include <GEOMAlgo_RemoverWebs.hxx>
#include <GEOMAlgo_ShapeAlgo.hxx>

#include <Basics_OCCTVersion.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <BRep_Builder.hxx>

#include <TopExp.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#if OCC_VERSION_LARGE > 0x06070100
#include <IntTools_Context.hxx>
#else
#include <BOPInt_Context.hxx>
#endif

#include <BOPAlgo_BuilderSolid.hxx>

#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPCol_MapOfShape.hxx>

//=======================================================================
//function : 
//purpose  :
//=======================================================================
GEOMAlgo_RemoverWebs::GEOMAlgo_RemoverWebs()
:
  GEOMAlgo_ShapeAlgo()
{
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_RemoverWebs::~GEOMAlgo_RemoverWebs()
{
}
//=======================================================================
//function : CheckData
//purpose  :
//=======================================================================
void GEOMAlgo_RemoverWebs::CheckData()
{
  TopoDS_Iterator aIt;
  //
  myErrorStatus=0;
  //
  if (myShape.IsNull()) {
    myErrorStatus=10;
    return;
  }
  //
  aIt.Initialize(myShape);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    if (aS.ShapeType()!=TopAbs_SOLID) {
      myErrorStatus=11;
      return;
    }
  }
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_RemoverWebs::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  // 1.
  CheckData();
  if(myErrorStatus) {
    return;
  }
  //
  // 2. Init myContext
  if (!myContext.IsNull()) {
    myContext.Nullify();
  }
#if OCC_VERSION_LARGE > 0x06070100
  myContext=new IntTools_Context;
#else
  myContext=new BOPInt_Context;
#endif
  //
  BuildSolid();
  //
}

//=======================================================================
//function : BuildSolid
//purpose  :
//=======================================================================
void GEOMAlgo_RemoverWebs::BuildSolid()
{
  Standard_Integer i, aNbF, aNbSx, iErr, aNbSI, aNbF2, aNbS, aNbR;  
  TopAbs_Orientation aOr;
  TopoDS_Iterator aIt1, aIt2;
  TopoDS_Shape aShape;
  BRep_Builder aBB;
  BOPCol_MapOfShape aMFence;
  BOPCol_IndexedMapOfShape aMSI;
  BOPCol_IndexedDataMapOfShapeListOfShape aMFS;
  BOPCol_ListOfShape aSFS;
  BOPCol_ListIteratorOfListOfShape aItLS;
  BOPAlgo_BuilderSolid aSB;
  //
  //modified by NIZNHY-PKV Thu Jul 11 06:54:51 2013f
  //
  // 0. 
  // The compound myShape may contain equal solids 
  // (itz.brep for e.g.). The block is to refine 
  // such data if it is necessary. The shape to treat 
  // will be aShape (not myShape).
  //
  aShape=myShape;
  //
  aIt1.Initialize(myShape);
  for (aNbS=0; aIt1.More(); aIt1.Next(), ++aNbS) {
    const TopoDS_Shape& aS=aIt1.Value();
    aMFence.Add(aS);
  }
  //
  aNbR=aMFence.Extent();
  if (aNbS!=aNbR) {
    BOPCol_MapIteratorOfMapOfShape aItMS;
    //
    BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aShape);  
    //
    aItMS.Initialize(aMFence);
    for (; aItMS.More(); aItMS.Next()) {
      const TopoDS_Shape& aS=aItMS.Key();
      aBB.Add(aShape, aS);
    }
  }
  //modified by NIZNHY-PKV Thu Jul 11 06:54:54 2013t
  //
  aNbF2=0;
  //
  // 1. aSFS: Faces 
  BOPTools::MapShapesAndAncestors(aShape, TopAbs_FACE, TopAbs_SOLID, aMFS);
  //
  aNbF=aMFS.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aFx=aMFS.FindKey(i);
    aOr=aFx.Orientation();
    if (aOr==TopAbs_INTERNAL) {
      TopoDS_Shape aFi;
      //
      aFi=aFx;
      aFi.Orientation(TopAbs_FORWARD);
      aSFS.Append(aFi);
      aFi.Orientation(TopAbs_REVERSED);
      aSFS.Append(aFi);
    }
    else {
      const BOPCol_ListOfShape& aLSx=aMFS(i);
      aNbSx=aLSx.Extent();
      if (aNbSx==1) {
        aSFS.Append(aFx);
      }
      else if (aNbSx==2) {
        ++aNbF2;
      }
    }
  }
  //
  if (!aNbF2) { // nothing to do here
    myResult=aShape;
    return;
  }
  //
  // 2 Internal shapes: edges, vertices
  aIt1.Initialize(aShape);
  for (; aIt1.More(); aIt1.Next()) {
    const TopoDS_Shape& aSD=aIt1.Value(); 
    //
    aIt2.Initialize(aSD);
    for (; aIt2.More(); aIt2.Next()) {
      const TopoDS_Shape& aSi=aIt2.Value(); 
      if (aSi.ShapeType()!=TopAbs_SHELL) {
        aOr=aSi.Orientation();
        if (aOr==TopAbs_INTERNAL) {
          aMSI.Add(aSi);
        }
      }
    }
  }
  aNbSI=aMSI.Extent();
  //
  // 3 Solids without internals
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, myResult);  
  //
  aSB.SetContext(myContext);
  aSB.SetShapes(aSFS);
  aSB.Perform();
  iErr=aSB.ErrorStatus();
  if (iErr) {
    myErrorStatus=20; // SolidBuilder failed
    return;
  }
  //
  const BOPCol_ListOfShape& aLSR=aSB.Areas();
  // 
  // 4 Add the internals
  if (aNbSI) {
    AddInternalShapes(aLSR, aMSI);
  }
  //
  aItLS.Initialize(aLSR);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSR=aItLS.Value();
    aBB.Add(myResult, aSR);
  }
}

//=======================================================================
//function : AddInternalShapes
//purpose  : 
//=======================================================================
void GEOMAlgo_RemoverWebs::AddInternalShapes(const BOPCol_ListOfShape& aLSR,
                                             const BOPCol_IndexedMapOfShape& aMSI)
{
  Standard_Integer i, aNbSI;
  TopAbs_State aState;  
  TopoDS_Solid aSd;
  BRep_Builder aBB;
  BOPCol_ListIteratorOfListOfShape aItLS;
#if OCC_VERSION_LARGE > 0x06070100
  Handle(IntTools_Context) aCtx=new IntTools_Context;
#else
  Handle(BOPInt_Context) aCtx=new BOPInt_Context;
#endif
  //
  aNbSI=aMSI.Extent();
  for (i=1; i<=aNbSI; ++i) {
    const TopoDS_Shape& aSI=aMSI(i);
    //
    aItLS.Initialize(aLSR);
    for (; aItLS.More(); aItLS.Next()) {
      aSd=*((TopoDS_Solid*)&aItLS.Value());
      //
      aState=BOPTools_AlgoTools::ComputeStateByOnePoint(aSI, aSd, 1.e-11, aCtx);
      if (aState==TopAbs_IN) {
        aBB.Add(aSd, aSI);
        //
        BRepClass3d_SolidClassifier& aSC=aCtx->SolidClassifier(aSd);
        aSC.Load(aSd);
      }
    }
  }
}
//
// myErrorStatus
// 0  - OK
// 10 - myShape is Null
// 11 - myShape contains non-solids
// 20 - BuilderSolid failed
