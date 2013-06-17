// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//              <pkv@irinox>
//
#include <GEOMAlgo_RemoverWebs.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <BRep_Builder.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <TopExp.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <IntTools_Context.hxx>

#include <GEOMAlgo_ShapeAlgo.hxx>
#include <GEOMAlgo_BuilderSolid.hxx>
#include <GEOMAlgo_Tools3D.hxx>


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
  myContext=new IntTools_Context;
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
  Standard_Integer i, aNbF, aNbSx, iErr, aNbSI, aNbF2;  
  TopAbs_Orientation aOr;
  TopoDS_Iterator aIt1, aIt2;
  BRep_Builder aBB;
  TopTools_IndexedMapOfShape aMSI;
  TopTools_IndexedDataMapOfShapeListOfShape aMFS;
  TopTools_ListOfShape aSFS;
  TopTools_ListIteratorOfListOfShape aItLS;
  GEOMAlgo_BuilderSolid aSB;
  //
  aNbF2=0;
  //
  // 1. aSFS: Faces 
  TopExp::MapShapesAndAncestors(myShape, TopAbs_FACE, TopAbs_SOLID, aMFS);
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
      const TopTools_ListOfShape& aLSx=aMFS(i);
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
    myResult=myShape;
    return;
  }
  //
  // 2 Internal shapes: edges, vertices
  aIt1.Initialize(myShape);
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
  GEOMAlgo_Tools3D::MakeContainer(TopAbs_COMPOUND, myResult);  
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
  const TopTools_ListOfShape& aLSR=aSB.Areas();
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
void GEOMAlgo_RemoverWebs::AddInternalShapes(const TopTools_ListOfShape& aLSR,
                                             const TopTools_IndexedMapOfShape& aMSI)
{
  Standard_Integer i, aNbSI;
  TopAbs_State aState;  
  TopoDS_Solid aSd;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aItLS;
  Handle(IntTools_Context) aCtx;
  //
  aCtx=new IntTools_Context;
  //
  aNbSI=aMSI.Extent();
  for (i=1; i<=aNbSI; ++i) {
    const TopoDS_Shape& aSI=aMSI(i);
    //
    aItLS.Initialize(aLSR);
    for (; aItLS.More(); aItLS.Next()) {
      aSd=*((TopoDS_Solid*)&aItLS.Value());
      //
      aState=GEOMAlgo_Tools3D::ComputeStateByOnePoint(aSI, aSd, 1.e-11, aCtx);
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
