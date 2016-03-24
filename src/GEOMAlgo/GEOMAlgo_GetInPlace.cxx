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
// File:     GEOMAlgo_GetInPlace.cxx
// Created:
// Author:   Peter KURNEV

#include <GEOMAlgo_GetInPlace.hxx>


#include <NCollection_UBTreeFiller.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <BRepBndLib.hxx>

#include <TopExp.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <GEOMAlgo_BoxBndTree.hxx>
#include <GEOMAlgo_CoupleOfShapes.hxx>


static
  void MapBRepShapes(const TopoDS_Shape& aS,
                     TopTools_IndexedMapOfShape& aM);


//=======================================================================
//function : GEOMAlgo_GetInPlace
//purpose  :
//=======================================================================
GEOMAlgo_GetInPlace::GEOMAlgo_GetInPlace()
:
  GEOMAlgo_GluerAlgo(),
  GEOMAlgo_Algo()
{
  myTolerance=0.0001;
  myTolMass=0.0001;
  myTolCG=0.0001;
  myFound=Standard_False;
  myCheckGeometry=Standard_True;
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_GetInPlace::~GEOMAlgo_GetInPlace()
{
}
//=======================================================================
//function : SetTolMass
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::SetTolMass(const Standard_Real theTol)
{
  myTolMass=theTol;
}
//=======================================================================
//function : TolMass
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_GetInPlace::TolMass()const
{
  return myTolMass;
}
//=======================================================================
//function : SetTolCG
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::SetTolCG(const Standard_Real theTol)
{
  myTolCG=theTol;
}
//=======================================================================
//function : TolCG
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_GetInPlace::TolCG()const
{
  return myTolCG;
}
//=======================================================================
//function : IsFound
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_GetInPlace::IsFound()const
{
  return myFound;
}
//=======================================================================
//function : SetShapeWhere
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::SetShapeWhere(const TopoDS_Shape& theShape)
{
  myShapeWhere=theShape;
}
//=======================================================================
//function : ShapeWhere
//purpose  :
//=======================================================================
const TopoDS_Shape& GEOMAlgo_GetInPlace::ShapeWhere()const
{
  return myShapeWhere;
}
//=======================================================================
//function : ShapesIn
//purpose  :
//=======================================================================
const GEOMAlgo_DataMapOfShapeMapOfShape& GEOMAlgo_GetInPlace::ShapesIn()const
{
  return myShapesIn;
}
//=======================================================================
//function : ShapesOn
//purpose  :
//=======================================================================
const GEOMAlgo_DataMapOfShapeMapOfShape& GEOMAlgo_GetInPlace::ShapesOn()const
{
  return myShapesOn;
}
//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::Clear()
{
  TopoDS_Shape aS;
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  GEOMAlgo_GluerAlgo::Clear();
  myIterator.Clear();
  myShapesIn.Clear();
  myShapesOn.Clear();
  myShapesInclusive.Clear();
  myMapShapePnt.Clear();
  myChecked.Clear();
  myResult= aS;
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::Perform()
{
  myFound=Standard_False;
  myErrorStatus=0;
  myWarningStatus=0;
  //
  Clear();
  if (myErrorStatus) {
    return;
  }
  //
  CheckData();
  if (myErrorStatus) {
    return;
  }
  //
  // Initialize the context
  GEOMAlgo_GluerAlgo::Perform();
  //
  Intersect();
  if (myErrorStatus) {
    return;
  }
  //
  PerformVV();
  if (myErrorStatus) {
    return;
  }
  //
  FillEdgesOn(myArgument);
  FillEdgesOn(myShapeWhere);
  if (myErrorStatus) {
    return;
  }
  //
  PerformVE();
  if (myErrorStatus) {
    return;
  }
  //
  PerformEE();
  if (myErrorStatus) {
    return;
  }
  //
  PerformVF();
  if (myErrorStatus) {
    return;
  }
  //
  FillFacesOn(myArgument);
  FillFacesOn(myShapeWhere);
  if (myErrorStatus) {
    return;
  }
  //
  PerformEF();
  if (myErrorStatus) {
    return;
  }
  //
  PerformFF();
  if (myErrorStatus) {
    return;
  }
  //
  FillSolidsOn(myArgument);
  FillSolidsOn(myShapeWhere);
  if (myErrorStatus) {
    return;
  }
  //
  PerformZF();
  if (myErrorStatus) {
    return;
  }
  //
  PerformZZ();
  if (myErrorStatus) {
    return;
  }
  //
  myImages.Clear();
  FillImages(myShapeWhere, Standard_True);
  FillImages(myArgument, Standard_False);

  if (myErrorStatus) {
    return;
  }
  //
  CheckGProps();
  if (myErrorStatus) {
    return;
  }
}
//=======================================================================
//function : CheckData
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::CheckData()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  if (myArgument.IsNull()) {
    myErrorStatus=2;
    return;
  }
  //
  if (myShapeWhere.IsNull()) {
    myErrorStatus=3;
    return;
  }
}
//=======================================================================
//function : Intersect
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::Intersect()
{
  Standard_Integer i, j, aNbS1, aNbS2, aNbSD;
  TColStd_ListIteratorOfListOfInteger aItLI;
  TopTools_IndexedMapOfShape aMS1, aMS2;
  TopTools_DataMapOfShapeListOfShape aDMSLS;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItDMSLS;
  TopTools_ListIteratorOfListOfShape aItLS;
  GEOMAlgo_CoupleOfShapes aCS;
  //
  GEOMAlgo_BoxBndTreeSelector aSelector;
  GEOMAlgo_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Clear();
  //
  MapBRepShapes(myArgument, aMS1);
  aNbS1=aMS1.Extent();
  for (i=1; i<=aNbS1; ++i) {
    Bnd_Box aBox1;
    //
    const TopoDS_Shape& aS1=aMS1(i);
    BRepBndLib::Add(aS1, aBox1);
    aBox1.Enlarge(myTolerance);
    //
    aTreeFiller.Add(i, aBox1);
  }
  //
  aTreeFiller.Fill();
  //
  MapBRepShapes(myShapeWhere, aMS2);
  aNbS2=aMS2.Extent();
  for (j=1; j<=aNbS2; ++j) {
    Bnd_Box aBox2;
    //
    const TopoDS_Shape& aS2=aMS2(j);
    BRepBndLib::Add(aS2, aBox2);
    aBox2.Enlarge(myTolerance);
    //
    aSelector.Clear();
    aSelector.SetBox(aBox2);
    aNbSD=aBBTree.Select(aSelector);
    if (!aNbSD) {
      continue;  // it should not be
    }
    //
    const TColStd_ListOfInteger& aLI=aSelector.Indices();
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      i=aItLI.Value();
      const TopoDS_Shape& aS1=aMS1(i);
      //
      if (aDMSLS.IsBound(aS1)) {
        TopTools_ListOfShape& aLS=aDMSLS.ChangeFind(aS1);
        aLS.Append(aS2);
      }
      else {
        TopTools_ListOfShape aLS;
        //
        aLS.Append(aS2);
        aDMSLS.Bind(aS1, aLS);
      }
    }
  }// for (j=1; j<=aNbS2; ++j) {
  //
  aItDMSLS.Initialize(aDMSLS);
  for (; aItDMSLS.More(); aItDMSLS.Next()) {
    const TopoDS_Shape& aS1=aItDMSLS.Key();
    const TopTools_ListOfShape& aLS2=aItDMSLS.Value();
    aCS.SetShape1(aS1);
    aItLS.Initialize(aLS2);
    for (; aItLS.More(); aItLS.Next()) {
      const TopoDS_Shape& aS2=aItLS.Value();
      aCS.SetShape2(aS2);
      myIterator.AppendPair(aCS);
    }
  }
}
//=======================================================================
//function : PerformVV
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformVV()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_VERTEX, TopAbs_VERTEX);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aV1=aCS.Shape1();
    const TopoDS_Shape& aV2=aCS.Shape2();
    //
    FillShapesOn(aV1, aV2);
    FillShapesOn(aV2, aV1);
  }
}
//=======================================================================
//function : FillEdgesOn
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillEdgesOn(const TopoDS_Shape &theShape)
{
  Standard_Integer i, aNbE;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aME;
  TopTools_MapIteratorOfMapOfShape aItMS;
  //
  TopExp::MapShapes(theShape, TopAbs_EDGE, aME);
  aNbE=aME.Extent();
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Edge& aE1=*((TopoDS_Edge*)&aME(i));
    if (BRep_Tool::Degenerated(aE1)) {
      continue;
    }
    //
    aIt.Initialize(aE1);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aV1=aIt.Value();
      if (myShapesOn.IsBound(aV1)) {
        const TopTools_MapOfShape& aMSOn=myShapesOn.Find(aV1);
        //aNbSOn=aMSOn.Extent();
        aItMS.Initialize(aMSOn);
        for (; aItMS.More(); aItMS.Next()) {
          const TopoDS_Shape& aV2=aItMS.Key();
          FillShapesOn(aE1, aV2);
        }
      }
    }
  }
}
//=======================================================================
//function : PerformVE
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformVE()
{
  Standard_Boolean bFound;
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  // 2. Fill Shapes In
  myIterator.Initialize(TopAbs_EDGE, TopAbs_VERTEX);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aE1=aCS.Shape1();
    const TopoDS_Shape& aV2=aCS.Shape2();
    //
    if (myShapesOn.IsBound(aE1)) {
      const TopTools_MapOfShape& aMSOn=myShapesOn.Find(aE1);
      if (aMSOn.Contains(aV2)) {
        continue;
      }
    }
    //
    bFound=CheckCoincidence(aE1, aV2);
    if (myErrorStatus) {
      return;
    }
    if (bFound) {
      FillShapesIn(aE1, aV2);
    }
  }
}
//=======================================================================
//function : PerformEE
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformEE()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_EDGE, TopAbs_EDGE);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aE1=aCS.Shape1();
    const TopoDS_Shape& aE2=aCS.Shape2();

    PerformEE(aE1, aE2);

    if (myErrorStatus) {
      return;
    }

    PerformEE(aE2, aE1);

    if (myErrorStatus) {
      return;
    }
  }
}
//=======================================================================
//function : PerformEE
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformEE(const TopoDS_Shape &theE1,
                                    const TopoDS_Shape &theE2)
{
  Standard_Boolean bHasOn, bHasIn, bFound;
  TopoDS_Iterator aIt;
  TopTools_MapOfShape aMSX;
  //
  bHasOn=myShapesOn.IsBound(theE1);
  bHasIn=myShapesIn.IsBound(theE1);
  const TopTools_MapOfShape& aMSOn=(bHasOn) ? myShapesOn.Find(theE1) : aMSX;
  const TopTools_MapOfShape& aMSIn=(bHasIn) ? myShapesIn.Find(theE1) : aMSX;
  //
  bFound=Standard_True;
  aIt.Initialize(theE2);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aV2=aIt.Value();
    if (!(aMSOn.Contains(aV2) || aMSIn.Contains(aV2))) {
      bFound=!bFound;
      break;
    }
  }
  if (!bFound) {
    return;
  }
  //
  bFound=CheckCoincidence(theE1, theE2);
  if (myErrorStatus) {
    return;
  }
  if (bFound) {
    FillShapesIn(theE1, theE2);
  }
}
//=======================================================================
//function : PerformVF
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformVF()
{
  Standard_Boolean bHasOn, bHasIn, bFound;
  Standard_Integer i, aNbE;
  TopTools_MapOfShape aMSX;
  TopTools_IndexedMapOfShape aME;
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_FACE, TopAbs_VERTEX);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aF1=aCS.Shape1();
    const TopoDS_Shape& aV2=aCS.Shape2();
    //
    aME.Clear();
    TopExp::MapShapes(aF1, TopAbs_EDGE, aME);
    //
    bFound=Standard_False;
    aNbE=aME.Extent();
    for (i=1; i<=aNbE; ++i) {
      const TopoDS_Edge& aE1=*((TopoDS_Edge*)&aME(i));
      if (BRep_Tool::Degenerated(aE1)) {
        continue;
      }
      //
      bHasOn=myShapesOn.IsBound(aE1);
      bHasIn=myShapesIn.IsBound(aE1);
      const TopTools_MapOfShape& aMSOn=(bHasOn) ? myShapesOn.Find(aE1) : aMSX;
      const TopTools_MapOfShape& aMSIn=(bHasIn) ? myShapesIn.Find(aE1) : aMSX;
      bFound= (aMSOn.Contains(aV2) || aMSIn.Contains(aV2));
      if (bFound) {
        break;
      }
    }
    //
    if (bFound) {
      continue;
    }
    //
    bFound=CheckCoincidence(aF1, aV2);
    if (myErrorStatus) {
      return;
    }
    if (bFound) {
      FillShapesIn(aF1, aV2);
    }
  }
}
//=======================================================================
//function : FillFacesOn
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillFacesOn(const TopoDS_Shape &theShape)
{
  Standard_Integer i, j, aNbF, aNbE;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aMF, aME;
  TopTools_MapIteratorOfMapOfShape aItMS;
  //
  TopExp::MapShapes(theShape, TopAbs_FACE, aMF);
  aNbF=aMF.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Face& aF1=*((TopoDS_Face*)&aMF(i));
    //
    aME.Clear();
    TopExp::MapShapes(aF1, TopAbs_EDGE, aME);
    aNbE=aME.Extent();
    for (j=1; j<=aNbE; ++j) {
      const TopoDS_Edge& aE1=*((TopoDS_Edge*)&aME(j));
      if (BRep_Tool::Degenerated(aE1)) {
        continue;
      }
      //
      if (myShapesOn.IsBound(aE1)) {
        const TopTools_MapOfShape& aMSOn=myShapesOn.Find(aE1);
        aItMS.Initialize(aMSOn);
        for (; aItMS.More(); aItMS.Next()) {
          const TopoDS_Shape& aS2=aItMS.Key();
          FillShapesOn(aF1, aS2);
        }
      }
      //
      if (myShapesIn.IsBound(aE1)) {
        const TopTools_MapOfShape& aMSIn=myShapesIn.Find(aE1);
        aItMS.Initialize(aMSIn);
        for (; aItMS.More(); aItMS.Next()) {
          const TopoDS_Shape& aS2=aItMS.Key();
          FillShapesOn(aF1, aS2);
        }
      }
    }//for (j=1; j<=aNbE; ++j) {
  }//for (i=1; i<=aNbF; ++i) {
}
//=======================================================================
//function : PerformEF
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformEF()
{
  Standard_Boolean  bFound, bHasOnF, bHasInF;
  TopoDS_Iterator aIt;
  TopTools_MapOfShape aMSX;
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_FACE, TopAbs_EDGE);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aF1=aCS.Shape1();
    const TopoDS_Shape& aE2=aCS.Shape2();
    //
    // 1.
    bHasOnF=myShapesOn.IsBound(aF1);
    const TopTools_MapOfShape& aMSOnF=(bHasOnF) ? myShapesOn.Find(aF1) : aMSX;
    bFound=aMSOnF.Contains(aE2);
    if (bFound) {
      continue;
    }
    //
    // 2.
    bHasInF=myShapesIn.IsBound(aF1);
    const TopTools_MapOfShape& aMSInF=(bHasInF) ? myShapesIn.Find(aF1) : aMSX;
    //
    aIt.Initialize(aE2);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aV2=aIt.Value();
      bFound=(aMSOnF.Contains(aV2) || aMSInF.Contains(aV2));
      if (!bFound) {
        break;
      }
    }
    if (!bFound) {
      continue;
    }
    //------------------------------
    bFound=CheckCoincidence(aF1, aE2);
    if (myErrorStatus) {
      return;
    }
    if (bFound) {
      FillShapesIn(aF1, aE2);
    }
  }
}
//=======================================================================
//function : PerformFF
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformFF()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_FACE, TopAbs_FACE);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aF1=aCS.Shape1();
    const TopoDS_Shape& aF2=aCS.Shape2();

    PerformFF(aF1, aF2);

    if (myErrorStatus) {
      return;
    }

    PerformFF(aF2, aF1);

    if (myErrorStatus) {
      return;
    }
  }
}
//=======================================================================
//function : PerformFF
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformFF(const TopoDS_Shape &theF1,
                                    const TopoDS_Shape &theF2)
{
  Standard_Boolean  bFound, bHasOnF, bHasInF;
  Standard_Integer i, aNbS2;
  TopTools_MapOfShape aMSX;
  TopTools_IndexedMapOfShape aMS2;
  //
  bHasOnF=myShapesOn.IsBound(theF1);
  const TopTools_MapOfShape& aMSOnF=(bHasOnF) ? myShapesOn.Find(theF1) : aMSX;
  //
  bHasInF=myShapesIn.IsBound(theF1);
  const TopTools_MapOfShape& aMSInF=(bHasInF) ? myShapesIn.Find(theF1) : aMSX;
  //
  MapBRepShapes(theF2, aMS2);
  //
  bFound=Standard_False;
  aNbS2=aMS2.Extent();
  for (i=1; i<=aNbS2; ++i) {
    const TopoDS_Shape& aS2=aMS2(i);
    if (aS2.IsSame(theF2)) {
      continue;
    }
    bFound=(aMSOnF.Contains(aS2) || aMSInF.Contains(aS2));
    if (!bFound) {
      break;
    }
  }
  if (!bFound) {
    return;
  }
  //
  bFound=CheckCoincidence(theF1, theF2);
  if (myErrorStatus) {
    return;
  }
  if (bFound) {
    FillShapesIn(theF1, theF2);
  }
}
//=======================================================================
//function : FillSolidsOn
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillSolidsOn(const TopoDS_Shape &theShape)
{
  Standard_Integer i, j, aNbS, aNbF;
  TopTools_IndexedMapOfShape aMS, aMF;
  TopTools_MapIteratorOfMapOfShape aItMS;
  //
  TopExp::MapShapes(theShape, TopAbs_SOLID, aMS);
  //
  aNbS=aMS.Extent();
  for (i=1; i<=aNbS; ++i) {
    const TopoDS_Shape& aSD1=aMS(i);
    //
    aMF.Clear();
    TopExp::MapShapes(aSD1, TopAbs_FACE, aMF);
    aNbF=aMF.Extent();
    for (j=1; j<=aNbF; ++j) {
      const TopoDS_Shape& aF1=aMF(j);
      //
      if (myShapesOn.IsBound(aF1)) {
        const TopTools_MapOfShape& aMSOn=myShapesOn.Find(aF1);
        aItMS.Initialize(aMSOn);
        for (; aItMS.More(); aItMS.Next()) {
          const TopoDS_Shape& aS2=aItMS.Key();
          FillShapesOn(aSD1, aS2);
        }
      }
      //
      if (myShapesIn.IsBound(aF1)) {
        const TopTools_MapOfShape& aMSIn=myShapesIn.Find(aF1);
        aItMS.Initialize(aMSIn);
        for (; aItMS.More(); aItMS.Next()) {
          const TopoDS_Shape& aS2=aItMS.Key();
          FillShapesOn(aSD1, aS2);
        }
      }
    }//for (j=1; j<=aNbF; ++j) {
  }//for (i=1; i<=aNbS; ++i) {
}
//=======================================================================
//function : PerformZF
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformZF()
{
  Standard_Boolean  bFound, bHasOnF;
  TopTools_MapOfShape aMSX;
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_SOLID, TopAbs_FACE);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aSo1=aCS.Shape1();
    const TopoDS_Shape& aF2=aCS.Shape2();
    //
    bHasOnF=myShapesOn.IsBound(aSo1);
    const TopTools_MapOfShape& aMSOnF=(bHasOnF) ? myShapesOn.Find(aSo1) : aMSX;
    bFound=aMSOnF.Contains(aF2);
    if (bFound) {
      continue;
    }
    //------------------------------
    bFound=CheckCoincidence(aSo1, aF2);
    if (myErrorStatus) {
      return;
    }
    if (bFound) {
      FillShapesIn(aSo1, aF2);
    }
  }
}
//=======================================================================
//function : PerformZZ
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformZZ()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  myIterator.Initialize(TopAbs_SOLID, TopAbs_SOLID);
  for (; myIterator.More(); myIterator.Next()) {
    const GEOMAlgo_CoupleOfShapes& aCS=myIterator.Value();
    const TopoDS_Shape& aSo1=aCS.Shape1();
    const TopoDS_Shape& aSo2=aCS.Shape2();

    PerformZZ(aSo1, aSo2);

    if (myErrorStatus) {
      return;
    }

    PerformZZ(aSo2, aSo1);

    if (myErrorStatus) {
      return;
    }
  }// for (; myIterator.More(); myIterator.Next()) {
}
//=======================================================================
//function : PerformZZ
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::PerformZZ(const TopoDS_Shape &theSo1,
                                    const TopoDS_Shape &theSo2)
{
  Standard_Boolean bFound, bHasOn, bHasIn;
  Standard_Integer i, aNbS2, iCntOn, iCntIn, iCntOut;
  TopTools_MapOfShape aMSX;
  TopTools_IndexedMapOfShape aMS2;
  //
  bHasOn=myShapesOn.IsBound(theSo1);
  const TopTools_MapOfShape& aMSOn=(bHasOn) ? myShapesOn.Find(theSo1) : aMSX;
  //
  bHasIn=myShapesIn.IsBound(theSo1);
  const TopTools_MapOfShape& aMSIn=(bHasIn) ? myShapesIn.Find(theSo1) : aMSX;
  //
  TopExp::MapShapes(theSo2, TopAbs_FACE, aMS2);
  //
  iCntIn=0;
  iCntOn=0;
  iCntOut=0;
  bFound=Standard_False;
  aNbS2=aMS2.Extent();
  for (i=1; i<=aNbS2; ++i) {
    const TopoDS_Shape& aF2=aMS2(i);
    //
    if (aMSIn.Contains(aF2)) {
      ++iCntIn;
      bFound=Standard_True;
      break;
    }
    else if (!aMSOn.Contains(aF2)) {
      ++iCntOut;
      bFound=Standard_False;// out
      break;
    }
    else {
      ++iCntOn; //on
    }
  }
  //
  if (!bFound && iCntOut) {
    return;
  }
  //
  if (!iCntIn) {
    bFound=CheckCoincidence(theSo1, theSo2);
    if (myErrorStatus) {
      return;
    }
  }
  if (bFound) {
    FillShapesIn(theSo1, theSo2);
  }
}
//=======================================================================
//function : FillImages
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillImages(const TopoDS_Shape &theShape,
                                     const Standard_Boolean IsWhere)
{
  //
  myErrorStatus=0;
  myWarningStatus=0;
  //
  // 1. Vertices
  FillImgSimple(theShape, TopAbs_VERTEX, IsWhere);
  //
  // 2. Edges
  FillImgSimple(theShape, TopAbs_EDGE, IsWhere);
  //
  // 3. Wires
  FillImgComplex(theShape, TopAbs_WIRE, IsWhere);
  //
  // 4. Faces
  FillImgSimple(theShape, TopAbs_FACE, IsWhere);
  //
  // 5. Shells
  FillImgComplex(theShape, TopAbs_SHELL, IsWhere);
  //
  // 6. Solids
  FillImgSimple(theShape, TopAbs_SOLID, IsWhere);
  //
  // 7. CompSolids
  FillImgComplex(theShape, TopAbs_COMPSOLID, IsWhere);
  //
  // 8. Compounds
  const TopAbs_ShapeEnum aType = theShape.ShapeType();

  if (aType == TopAbs_COMPOUND) {
    FillImgComplex(theShape, IsWhere);
  }
}

//=======================================================================
//function : FillImgSimple
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillImgSimple
                      (const TopoDS_Shape     &theShape,
                       const TopAbs_ShapeEnum  theSubShapeType,
                       const Standard_Boolean  IsWhere)
{
  TopTools_IndexedMapOfShape aMS;

  TopExp::MapShapes(theShape, theSubShapeType, aMS);

  Standard_Integer i;
  const Standard_Integer aNbS = aMS.Extent();
  const GEOMAlgo_DataMapOfShapeMapOfShape &aShapesInOn =
    (theSubShapeType == TopAbs_VERTEX ? myShapesOn : myShapesIn);

  for (i = 1; i <= aNbS; i++) {
    const TopoDS_Shape& aS = aMS(i);

    if (aShapesInOn.IsBound(aS)) {
      const TopTools_MapOfShape& aMSx = aShapesInOn.Find(aS);
      TopTools_ListOfShape aLSx;
      TopTools_MapIteratorOfMapOfShape aItMS(aMSx);

      for (; aItMS.More(); aItMS.Next()) {
        const TopoDS_Shape& aSx = aItMS.Key();
        TopAbs_ShapeEnum aType = aSx.ShapeType();

        if (aType == theSubShapeType){
          aLSx.Append(aSx);

          if (IsWhere) {
            myShapesInclusive.Bind(aSx, aS);
          }
        }
      }

      myImages.Bind(aS, aLSx);
    }
  }
}

//=======================================================================
//function : FillImgComplex
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillImgComplex(const TopoDS_Shape     &theShape,
                                         const Standard_Boolean  IsWhere)
{
  TopTools_MapOfShape  aMapRemaining;
  TopoDS_Iterator      aIt(theShape);
  TopTools_ListOfShape aLSx;
  TopTools_MapOfShape  aMSx;

  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape &aSubS = aIt.Value();
    TopAbs_ShapeEnum    aType = aSubS.ShapeType();

    if (aType == TopAbs_COMPOUND) {
      // Recursively treat compounds.
      FillImgComplex(aSubS, IsWhere);
    }

    if (myImages.IsBound(aSubS)) {
      const TopTools_ListOfShape& aLSi = myImages.Find(aSubS);
      TopTools_ListIteratorOfListOfShape aItLS(aLSi);

      for (; aItLS.More(); aItLS.Next()) {
        const TopoDS_Shape &aSubSi = aItLS.Value();

        if (aMSx.Add(aSubSi)) {
          aLSx.Append(aSubSi);
        }
      }
    } else if (!IsWhere) {
      aMapRemaining.Add(aSubS);
    }
  }

  if (!(IsWhere || aMapRemaining.IsEmpty())) {
    // Find the whole from parts.
    while (!aMapRemaining.IsEmpty()) {
      TopTools_MapIteratorOfMapOfShape anIter(aMapRemaining);
      const TopoDS_Shape &aShape = anIter.Key();

      if (myShapesInclusive.IsBound(aShape)) {
        // This "what" shape is inclusive to the "where" shape.
        // Get the other inclusive shapes.
        const TopoDS_Shape &aSWhere = myShapesInclusive.Find(aShape);

        if (myImages.IsBound(aSWhere)) {
          // Remove inclusive shapes from aMapRemaining.
          const TopTools_ListOfShape& aLWhat = myImages.Find(aSWhere);
          TopTools_ListIteratorOfListOfShape aItLS(aLWhat);

          for (; aItLS.More(); aItLS.Next()) {
            const TopoDS_Shape &aSWhat = aItLS.Value();

            aMapRemaining.Remove(aSWhat);
          }

          // Add "whole" shape to the list of images.
          if (aMSx.Add(aSWhere)) {
            aLSx.Append(aSWhere);
          }
        }
      } else {
        // This "what" shape is not inclusive to the "where" shape. Skip it.
        aMapRemaining.Remove(aShape);
      }
    }
  }

  myImages.Bind(theShape, aLSx);
}

//=======================================================================
//function : FillImgComplex
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillImgComplex
                           (const TopoDS_Shape     &theShape,
                            const TopAbs_ShapeEnum  theSubShapeType,
                            const Standard_Boolean  IsWhere)
{
  TopTools_IndexedMapOfShape aMS;

  TopExp::MapShapes(theShape, theSubShapeType, aMS);

  Standard_Integer i;
  const Standard_Integer aNbS = aMS.Extent();

  for (i=1; i<=aNbS; ++i) {
    const TopoDS_Shape &aS = aMS(i);

    FillImgComplex(aS, IsWhere);
  }
}

//=======================================================================
//function : FillShapesIn
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillShapesIn(const TopoDS_Shape& aS1,
                                       const TopoDS_Shape& aS2)
{
  if (myShapesIn.IsBound(aS1)) {
    TopTools_MapOfShape& aMS=myShapesIn.ChangeFind(aS1);
    aMS.Add(aS2);
  }
  else {
    TopTools_MapOfShape aMS;
    //
    aMS.Add(aS2);
    myShapesIn.Bind(aS1, aMS);
  }
}
//=======================================================================
//function : FillShapesOn
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::FillShapesOn(const TopoDS_Shape& aS1,
                                       const TopoDS_Shape& aS2)
{
  if (myShapesOn.IsBound(aS1)) {
    TopTools_MapOfShape& aMS=myShapesOn.ChangeFind(aS1);
    aMS.Add(aS2);
  }
  else {
    TopTools_MapOfShape aMS;
    //
    aMS.Add(aS2);
    myShapesOn.Bind(aS1, aMS);
  }
}

//=======================================================================
//function : MapBRepShapes
//purpose  :
//=======================================================================
void MapBRepShapes(const TopoDS_Shape& aS,
                   TopTools_IndexedMapOfShape& aM)
{
  Standard_Boolean bDegenerated;
  TopAbs_ShapeEnum aType;
  TopoDS_Iterator aIt;
  //
  aType=aS.ShapeType();
  if (aType==TopAbs_VERTEX || aType==TopAbs_EDGE ||
      aType==TopAbs_FACE   || aType==TopAbs_SOLID) {
    bDegenerated=Standard_False;
    if (aType==TopAbs_EDGE) {
      TopoDS_Edge *pE=(TopoDS_Edge*)&aS;
      bDegenerated=BRep_Tool::Degenerated(*pE);
    }
    if (!bDegenerated) {
      aM.Add(aS);
    }
  }
  //
  aIt.Initialize(aS);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    aType=aSx.ShapeType();
    MapBRepShapes(aSx, aM);
  }
}
//=======================================================================
//function : Result
//purpose  : 
//=======================================================================
const TopoDS_Shape &GEOMAlgo_GetInPlace::Result()
{
  TopoDS_Shape aDummy;
  //
  myResult = aDummy;
  //
  Standard_Boolean bFound;
  TopoDS_Compound aC;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aItLS;
  //
  bFound=myImages.IsBound(myArgument);
  if (!bFound) {
    return myResult;
  }
  //
  aBB.MakeCompound(aC);
  //
  const TopTools_ListOfShape& aLS=myImages.Find(myArgument);
  aItLS.Initialize(aLS);
  for (; aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aSD=aItLS.Value();
    aBB.Add(aC, aSD);
  }
  //
  myResult = aC;
  return myResult;
}
