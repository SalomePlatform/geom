// File:	NMTAlgo_Splitter1.cxx
// Created:	Wed Feb 11 14:26:27 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Splitter1.ixx>
#include <NMTAlgo_Splitter.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <NMTTools_IndexedDataMapOfShapeIndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_DataMapOfIntegerListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeInteger.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <TopTools_MapOfShape.hxx>
#include <NMTAlgo_Loop3d.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Solid.hxx>
#include <NMTAlgo_Tools.hxx>

//=======================================================================
//function : NMTAlgo_Spliter1::NMTAlgo_Splitter1
//purpose  : 
//=======================================================================
  NMTAlgo_Splitter1::NMTAlgo_Splitter1()
:
  NMTAlgo_Splitter()
{
  myRemoveWebs=Standard_False;
}
//=======================================================================
// function: ~NMTAlgo_Splitter1
// purpose: 
//=======================================================================
  NMTAlgo_Splitter1::~NMTAlgo_Splitter1()
{
  Clear();
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void NMTAlgo_Splitter1::Clear()
{
  NMTAlgo_Splitter::Clear();
  //
  myRemoveWebs=Standard_False;
  myMapSWM.Clear();
  myMapSWMOut.Clear();
}
//=======================================================================
//function : SetRemoveWebs
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::SetRemoveWebs(const Standard_Boolean bFlag)
{
  myRemoveWebs=bFlag;
}
//=======================================================================
//function : RemoveWebs
//purpose  :
//=======================================================================
  Standard_Boolean NMTAlgo_Splitter1::RemoveWebs()const
{
  return myRemoveWebs;
}
//=======================================================================
//function : GetMaterialTable
//purpose  :
//=======================================================================
  const TopTools_DataMapOfShapeInteger& NMTAlgo_Splitter1::GetMaterialTable()const
{
  return myMapSWMOut;
}
//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::SetMaterial(const TopoDS_Shape& aS,
				      const Standard_Integer iMt)
{
  TopExp_Explorer aExp;
  //
  aExp.Init(aS, TopAbs_SOLID);
  for(; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aSd=aExp.Current();
    myMapSWM.Bind(aSd, iMt);
  }
}
//=======================================================================
//function : AddShape
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::AddShape(const TopoDS_Shape& aS)
{
  SetMaterial(aS, 0);
  NMTAlgo_Splitter::AddShape(aS);
}
//=======================================================================
//function : AddToolWithMaterial
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::AddTool(const TopoDS_Shape& aS)
					    
{
  SetMaterial(aS, 0);
  NMTAlgo_Splitter::AddTool(aS);
}
//=======================================================================
//function : Build
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::Build(const TopAbs_ShapeEnum aLimit)
{
  NMTAlgo_Splitter::Build(aLimit);
  //
  myMapSWMOut.Clear();
  //
  TreatSolids();
  //
  if (myRemoveWebs && myMapSWMOut.Extent()) {
    RestParts();
    TreatWebs();
  }
}
//=======================================================================
//function : TreatWebs
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::TreatWebs()
{
  Standard_Integer nMt;
  TopTools_DataMapOfIntegerListOfShape aMMLS;
  TopoDS_Iterator aIt;
  TopTools_DataMapIteratorOfDataMapOfShapeInteger aIt2;
  TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape aIt1;
  TopTools_ListIteratorOfListOfShape aItS;
  TopoDS_Compound aCx;
  TopoDS_Shape aCSR;
  //
  aIt2.Initialize(myMapSWMOut);
  for (; aIt2.More(); aIt2.Next()) {
    const TopoDS_Shape& aS=aIt2.Key();
    nMt=aIt2.Value();
    //
    if (aMMLS.IsBound(nMt)){
      TopTools_ListOfShape& aLS=aMMLS.ChangeFind(nMt);
      aLS.Append(aS);
    }
    else {
      TopTools_ListOfShape aLS;
      aLS.Append(aS);
      aMMLS.Bind(nMt, aLS);
    }
  }
  //
  myMapSWMOut.Clear();
  myBuilder.MakeCompound(aCx);
  //
  aIt1.Initialize(aMMLS);
  for (; aIt1.More(); aIt1.Next()) {
    nMt=aIt1.Key();
    //
    TopoDS_Compound aCS;
    myBuilder.MakeCompound(aCS);
    const TopTools_ListOfShape& aLS=aIt1.Value();
    aItS.Initialize(aLS);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aS=aItS.Value();
      myBuilder.Add(aCS, aS);
    }
    //
    NMTAlgo_Tools::BreakWebs(aCS, aCSR);
    //
    aIt.Initialize(aCSR);
    for(; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Value();
      myBuilder.Add(aCx, aS);
      //
      myMapSWMOut.Bind(aS, nMt);
    }
  }
  //
  aIt.Initialize(myRestParts);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    myBuilder.Add(aCx, aS);
  }
  //
  myShape=aCx;
}

//=======================================================================
//function : RestParts
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::RestParts()
{
  TopoDS_Iterator aIt;
  TopoDS_Compound aR;
  //
  myBuilder.MakeCompound(aR);
  //
  aIt.Initialize(myShape);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    if (aS.ShapeType()==TopAbs_SOLID){
      if (myMapSWMOut.IsBound(aS)) {
	continue;
      }
    }
    myBuilder.Add(aR, aS);
  }
  myRestParts=aR;
}
//=======================================================================
//function : TreatSolids
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter1::TreatSolids()
{
  Standard_Integer i, j, aNbSd, aNbSdx, nMtMax, nMt;
  TopAbs_ShapeEnum aType;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aMSo, aMSd;
  TopTools_IndexedDataMapOfShapeListOfShape aMFS;
  TopTools_ListIteratorOfListOfShape aItS, aItLS;
  NMTTools_IndexedDataMapOfShapeIndexedMapOfShape aMCS;
  //
  TopExp::MapShapesAndAncestors(myShape, TopAbs_FACE, TopAbs_SOLID, aMFS);
  TopExp::MapShapes(myShape, TopAbs_SOLID, aMSo);
  //
  aNbSd=aMSo.Extent();
  if (!aNbSd) {
    return;
  }
  //
  aItS.Initialize(myListShapes);
  for ( ;aItS.More(); aItS.Next()) {
    const TopoDS_Shape& aS=aItS.Value();
    //
    aType=aS.ShapeType();
    if (aType != TopAbs_SOLID) {
      continue;
    }
    //
    if (myToolShapes.Contains(aS)) {
      continue;
    }
    //
    if (!myMapSIFC.IsBound(aS)){
      continue;
    }
    //
    aMSd.Clear();
    //
    const TopoDS_Shape& aIFC=myMapSIFC.Find(aS);
    //
    aIt.Initialize(aIFC);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aIF=aIt.Value();
      if (aMFS.Contains(aIF)) {
	const TopTools_ListOfShape& aLS=aMFS.FindFromKey(aIF);
	aItLS.Initialize(aLS);
	for ( ;aItLS.More(); aItLS.Next()) {
	  const TopoDS_Shape& aSd=aItLS.Value();
	  if (!aMSd.Contains(aSd)) {
	    aMSd.Add(aSd);
	  }
	}
      }
    }
    //
    aNbSd=aMSd.Extent();
    if (!aNbSd) {
      TopoDS_Shape aSd;
      //
      const TopoDS_Shape& aFC=myImageShape.Image(aS).First();
      NMTAlgo_Tools::FindImageSolid(aFC, aMSo, aSd);
      //
      aMSd.Add(aSd);
    }
    aMCS.Add(aS, aMSd); 
  } //for ( ;aItS.More(); aItS.Next())
  //
  aMFS.Clear();
  //
  aNbSd=aMCS.Extent();
  for (i=1; i<=aNbSd; ++i) {
    const TopoDS_Shape& aSd=aMCS.FindKey(i);
    const TopTools_IndexedMapOfShape& aMSdx=aMCS(i);
    aNbSdx=aMSdx.Extent();
    for (j=1; j<=aNbSdx; ++j) {
      const TopoDS_Shape& aSdx=aMSdx(j);
      //
      if (aMFS.Contains(aSdx)) {
	TopTools_ListOfShape& aLS=aMFS.ChangeFromKey(aSdx);
	aLS.Append(aSd);
      }
      else {
	TopTools_ListOfShape aLS;
	aLS.Append(aSd);
	aMFS.Add(aSdx, aLS);
      }
    }
  }
  //
  // Assign materials values to subsolids
  //
  myMapSWMOut.Clear();
  //
  aNbSdx=aMFS.Extent();
  for (i=1; i<=aNbSdx; ++i) {
    const TopoDS_Shape& aSdx=aMFS.FindKey(i);
    const TopTools_ListOfShape& aLS=aMFS(i);
    aItLS.Initialize(aLS);
    for (j=0; aItLS.More(); aItLS.Next(), ++j) {
      const TopoDS_Shape& aSd=aItLS.Value();
      //
      if (!myMapSWM.IsBound(aSd)){
	continue;
      }
      //
      nMt=myMapSWM.Find(aSd);
      if (!j) {
	nMtMax=nMt;
	continue;
      }
      if (nMt>nMtMax) {
	nMtMax=nMt;
      }
    }
    myMapSWMOut.Bind(aSdx, nMtMax);
  }
  //
}
