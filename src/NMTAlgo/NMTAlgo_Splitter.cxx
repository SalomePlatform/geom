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
// File:	NMTAlgo_Splitter.cxx
// Created:	Thu Jan 29 17:13:03 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Splitter.ixx>

#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>

#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>
//
#include <NMTTools_DSFiller.hxx>
#include <NMTAlgo_Tools.hxx>
#include <NMTAlgo_Loop3d.hxx>
#include <BOP_CorrectTolerances.hxx>


static 
  Standard_Boolean isClosed(const TopoDS_Shape& theShape);

//=======================================================================
//function : NMTAlgo_Spliter::NMTAlgo_Splitter
//purpose  : 
//=======================================================================
  NMTAlgo_Splitter::NMTAlgo_Splitter()
:
  NMTAlgo_Builder()
{
  myErrorStatus=0;
}
//=======================================================================
// function: ~NMTAlgo_Splitter
// purpose: 
//=======================================================================
  NMTAlgo_Splitter::~NMTAlgo_Splitter()
{
  Clear();
  
  if (myDSFiller){
    delete myDSFiller;
  }
}
//=======================================================================
// function: SourceShapes
// purpose: 
//=======================================================================
  const TopTools_ListOfShape& NMTAlgo_Splitter::SourceShapes()const
{
  return mySourceShapes;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void NMTAlgo_Splitter::Clear()
{
  NMTAlgo_Builder::Clear();
  //
  myListShapes.Clear();
  myMapFaces.Clear();
  myMapTools.Clear();
  myClosedShapes.Clear();
  myEqualEdges.Clear();
  myNewSection.Clear();
  myWrappingSolid.Clear();
  myFaceShapeMap.Clear();
  myInternalFaces.Clear();
  myIntNotClFaces.Clear();
  //
  myImageShape.Clear();
  myAddedFacesMap.Clear();
  //
  myDoneStep = TopAbs_SHAPE;
  myIsComputed=Standard_False;
  mySourceShapes.Clear();
  myObjShapes.Clear();
  myToolShapes.Clear();
  myMapSIFC.Clear();
  //modified by NIZNHY-PKV Mon Jan 24 09:47:37 2005f
  myModifiedFaces.Clear();
  //modified by NIZNHY-PKV Mon Jan 24 09:47:41 2005t
  myErrorStatus=0;
}
//=======================================================================
//function : AddShape
//purpose  : add object Shape to be splited
//=======================================================================
  void NMTAlgo_Splitter::AddShape(const TopoDS_Shape& aS)
{
  myErrorStatus=0;
  if (myIsComputed) {
    // DS is already computed
    myErrorStatus=100;
    return;
  }
  //
  if (aS.IsNull()){
    // Null shape is not allowed here
    myErrorStatus=101;
    return;
  }
  //
  TopAbs_ShapeEnum aType=aS.ShapeType(); 
  //
  if (aType < TopAbs_SOLID) { 
    // compound or compsolid
    TopoDS_Iterator it (aS);
    for (; it.More(); it.Next()) {
      const TopoDS_Shape& aSS=it.Value();
      AddShape(aSS);
      // to know compound by shape
      myFaceShapeMap.Bind(aSS, aS); 
    }
    return;
  }
  //---
  myObjShapes.Add(aS);
  mySourceShapes.Append(aS);  
  //---
  //
  TopExp_Explorer exp(aS, TopAbs_FACE);
  if (!exp.More()) { 
    // do not split edges and vertices
    return;
  }
  // not to add twice the same S
  Standard_Integer nbFacesBefore = myMapFaces.Extent(); 
  //
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& aFace = exp.Current();
    if (!myFaceShapeMap.IsBound(aFace)) {
      // keep shape of tool face added as object
      myFaceShapeMap.Bind(aFace, aS);
    }
    if (myMapFaces.Add(aFace)){
      myImagesFaces.SetRoot(aFace);
    }
  }
  //
  if (nbFacesBefore == myMapFaces.Extent()){
    return;
  }
  // solids must be processed before all
  if (aType==TopAbs_SOLID){
    myListShapes.Prepend(aS);
  }
  else{
    myListShapes.Append(aS);
  }
  if (isClosed(aS)){
    myClosedShapes.Add(aS);
  }
}
//=======================================================================
//function : AddTool
//purpose  : add cutting tool that will _NOT_ be in result
//=======================================================================
  void NMTAlgo_Splitter::AddTool(const TopoDS_Shape& aS)
{
  myErrorStatus=0;
  if (myIsComputed) {
    // DS is already computed
    myErrorStatus=100;
    return;
  }
  //
  if (aS.IsNull()){
    // Null shape is not allowed here
    myErrorStatus=101;
    return;
  }
  //
  TopAbs_ShapeEnum aType=aS.ShapeType(); 
  
  if (aType < TopAbs_SOLID) { // compound or compsolid
    TopoDS_Iterator it (aS);
    for (; it.More(); it.Next()) {
      const TopoDS_Shape& aSS=it.Value();
      AddTool(aSS);
      myFaceShapeMap.Bind(aSS, aS); // to know compound by shape
    }
    return;
  }
  //---
  myToolShapes.Add(aS);
  mySourceShapes.Append(aS);  
  //---
  //
  TopExp_Explorer exp(aS, TopAbs_FACE);
  for (; exp.More(); exp.Next()) {
    const TopoDS_Shape& aFace = exp.Current();
    myMapTools.Add(aFace);
    myFaceShapeMap.Bind(aFace, aS);
  }
  //
  // solids must be processed before all
  if (aType==TopAbs_SOLID){
    myListShapes.Prepend(aS);
  }
  else{
    myListShapes.Append(aS);
  }
  //
  if (isClosed(aS)) { 
    myClosedShapes.Add(aS);
  }
}
//=======================================================================
// function: Compute
// purpose: 
//=======================================================================
  void NMTAlgo_Splitter::Compute()
{
  if (!mySourceShapes.Extent()){
    // No source shapes to treat
    myErrorStatus=103; 
    return;
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aCS;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aBB.MakeCompound(aCS);
  //
  aIt.Initialize(mySourceShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aBB.Add(aCS, aS);
  }
  //
  NMTTools_DSFiller* pDSF=new NMTTools_DSFiller;
  //
  pDSF->SetCompositeShape(aCS);
  pDSF->Perform();
  //
  myIsComputed=Standard_False;
  //
  NMTAlgo_Splitter::ComputeWithFiller(*pDSF);
}
//=======================================================================
// function: ComputeWithFiller
// purpose: 
//=======================================================================
  void NMTAlgo_Splitter::ComputeWithFiller(const NMTTools_DSFiller& aDSF)
{
  myErrorStatus=0;
  //
  if (myIsComputed) {
    // DS is already computed
    myErrorStatus=100; 
    return;
  }
  //
  if (!mySourceShapes.Extent()){
    // No source shapes to treat
    myErrorStatus=103; 
    return;
  }
  //
  NMTAlgo_Builder::ComputeWithFiller(aDSF);
  //
  myIsComputed=Standard_True;
}
//=======================================================================
//function : Build
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter::Build(const TopAbs_ShapeEnum aLimit)
{
  myErrorStatus=0;
  myLimit=aLimit;
  //
  if (!myIsComputed){
    myErrorStatus=102;// DS is not computed
    return;
  }
  //
  TopoDS_Compound aCShape;
  //
  myBuilder.MakeCompound(aCShape);
  myShape=aCShape;
  //
  // 1. VERTEX
  //
  if (myLimit==TopAbs_VERTEX) {
    SplitVertices();
    FillResult();
    return;
  }
  //
  // 2. EDGE
  if (myLimit==TopAbs_EDGE) {
    SplitsAndSections();
    FillResult();
    return;
  }
  //
  // 3. WIRE
  FillImageShape();
  //
  if (myLimit==TopAbs_WIRE) {
    SplittedWires();
    FillResult();
    return;
  }
  //
  // 4. FACE
  //
  if (myLimit==TopAbs_FACE) {
    SplittedFaces();
    FillResult();
    return;
  }
  //
  // 5.6. SHELL / SOLID
  ShellsAndSolids();
  BOP_CorrectTolerances::CorrectTolerances(myShape, 0.01);
}
//=======================================================================
// function: SplitsAndSections
// purpose: 
//=======================================================================
  void NMTAlgo_Splitter::SplitsAndSections()
{
  Standard_Integer i, aNbE, aNbF;
  TopTools_ListIteratorOfListOfShape aItLS, aIt;
  TopTools_IndexedMapOfShape aME, aMF;
  //
  myQueryShapes.Clear();
  //
  //  1. Splits / no splits
  aItLS.Initialize(myListShapes);
  for ( ;aItLS.More(); aItLS.Next()) {
    const TopoDS_Shape& aS=aItLS.Value();
    //
    if (myToolShapes.Contains(aS)) {
      continue; // skip tool Shapes
    }
    //
    TopExp::MapShapes(aS, TopAbs_EDGE, aME);
    TopExp::MapShapes(aS, TopAbs_FACE, aMF);
  }    
  //
  //  1. Splits / no splits
  aNbE=aME.Extent();
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Shape& aE=aME(i);
    //
    if (!myImagesEdges.HasImage(aE)) {
      myQueryShapes.Add(aE);
    }
    else {
      const TopTools_ListOfShape& aLSp=myImagesEdges.Image(aE);
      //
      aIt.Initialize(aLSp);
      for (; aIt.More(); aIt.Next()) { 
	const TopoDS_Shape& aSp=aIt.Value();
	myQueryShapes.Add(aSp);
      }
    }
  }
  //
  // 2. Sections
  aNbF=aMF.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aF=aMF(i);
    if (mySectionParts.Contains(aF)) {
      const TopTools_ListOfShape& aLSc=mySectionParts.FindFromKey(aF);
      aIt.Initialize(aLSc);
      for (; aIt.More(); aIt.Next()) { 
	const TopoDS_Shape& aSc=aIt.Value();
	myQueryShapes.Add(aSc);
      }
    }
  }
}
//=======================================================================
// function: SplittedWires
// purpose: 
//=======================================================================
  void NMTAlgo_Splitter::SplittedWires()
{
  Standard_Integer i, aNbF;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aMF;
  //
  SplittedFaces();
  //
  aMF=myQueryShapes;
  myQueryShapes.Clear();
  //
  aNbF=aMF.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aF=aMF(i);
    aIt.Initialize(aF);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aW=aIt.Value();
      myQueryShapes.Add(aW);
    }
  }
}
//=======================================================================
// function: SplittedFaces
// purpose: 
//=======================================================================
  void NMTAlgo_Splitter::SplittedFaces()
{
  TopTools_ListIteratorOfListOfShape aIt;
  TopoDS_Iterator aItF;
  //
  myQueryShapes.Clear();
  //
  aIt.Initialize(myListShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    //
    if (myToolShapes.Contains(aS)) {
      continue;
    }
    //
    const TopoDS_Shape& aFC = myImageShape.Image(aS).First();
    aItF.Initialize(aFC);
    for (; aItF.More(); aItF.Next()) {
      const TopoDS_Shape& aF=aItF.Value();
      myQueryShapes.Add(aF);
    }
  }
}
//=======================================================================
//function : FillImageShape
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter::FillImageShape()
{
  Standard_Integer i, aNbF, iSense;
  TopTools_ListIteratorOfListOfShape aItS, aItFI;
  TopExp_Explorer aExp;
  TopAbs_Orientation aOriFS; 
  TopoDS_Face aFIx, aFIy;
  BRep_Builder aBB;
  //
  myImageShape.Clear();
  //modified by NIZNHY-PKV Mon Jan 24 09:48:15 2005f
  myModifiedFaces.Clear();
  //modified by NIZNHY-PKV Mon Jan 24 09:48:18 2005t
  //
  aItS.Initialize(myListShapes);
  for ( ;aItS.More(); aItS.Next()) {
    const TopoDS_Shape& aS=aItS.Value();
    //
    myQueryShapes.Clear();
    //
    aExp.Init(aS, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Face& aFS=TopoDS::Face(aExp.Current());
      aOriFS= aFS.Orientation();
      //
      if (!myImagesFaces.HasImage(aFS)) {
	myQueryShapes.Add(aFS);
	//modified by NIZNHY-PKV Mon Jan 24 09:50:42 2005 f
	if (!myModifiedFaces.IsBound(aFS)) {
	  TopTools_ListOfShape aLS;
	  //
	  aLS.Append(aFS);
	  myModifiedFaces.Bind(aFS, aLS);
	}
	//modified by NIZNHY-PKV Mon Jan 24 09:50:44 2005 t
	continue;
      }
      //
      const TopTools_ListOfShape& aLFI=myImagesFaces.Image(aFS);
      aItFI.Initialize(aLFI);
      for (; aItFI.More(); aItFI.Next()) {
	const TopoDS_Face& aFI=TopoDS::Face(aItFI.Value());
	aFIx=aFI;
	aFIx.Orientation(aOriFS);
	//
	if (mySDFaces.Contains(aFIx)) {
	  const TopoDS_Face& aFSDI=TopoDS::Face(mySDFaces.FindFromKey(aFIx));
	  TopoDS_Face aFSDIx=aFSDI;
	  //
	  iSense=NMTAlgo_Tools::Sense(aFIx, aFSDIx);
	  //
	  if (iSense < 0) {
	    aFSDIx.Reverse();
	  }
	  myQueryShapes.Add(aFSDIx);
	  //modified by NIZNHY-PKV Mon Jan 24 09:56:06 2005f
	  aFIy=aFSDIx;
	  //modified by NIZNHY-PKV Mon Jan 24 09:56:09 2005t
	}
	else {
	  myQueryShapes.Add(aFIx);
	  //modified by NIZNHY-PKV Mon Jan 24 09:56:06 2005f
	  aFIy=aFIx;
	  //modified by NIZNHY-PKV Mon Jan 24 09:56:09 2005t
	}
	//modified by NIZNHY-PKV Mon Jan 24 09:53:38 2005f
	if (!myModifiedFaces.IsBound(aFS)) {
	  TopTools_ListOfShape aLS;
	  //
	  aLS.Append(aFIy);
	  myModifiedFaces.Bind(aFS, aLS);
	}
	else {
	  TopTools_ListOfShape& aLS=myModifiedFaces.ChangeFind(aFS);
	  aLS.Append(aFIy);
	}
	//modified by NIZNHY-PKV Mon Jan 24 09:53:43 2005t
      }
    }//for (; aExp.More(); aExp.Next()) {
    //
    TopoDS_Compound aCompound;
    //
    aBB.MakeCompound(aCompound);
    //
    aNbF=myQueryShapes.Extent();
    for (i=1; i<=aNbF; ++i) {
      const TopoDS_Shape& aF=myQueryShapes(i);
      aBB.Add(aCompound, aF);
    }
    //
    myImageShape.Bind(aS, aCompound);
  }// for ( ;aItS.More(); aItS.Next())
  //
  myQueryShapes.Clear();
}
//=======================================================================
//function : FillResult
//purpose  :
//=======================================================================
  void NMTAlgo_Splitter::FillResult()
{
  Standard_Integer i, aNb;
  //
  aNb=myQueryShapes.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aS=myQueryShapes(i); 
    myBuilder.Add (myShape, aS);
  }
  BOP_CorrectTolerances::CorrectTolerances(myShape, 0.01);
}
//=======================================================================
//function : isClosed
//purpose  : check id a shape is closed, ie is a solid or a closed shell
//=======================================================================
 Standard_Boolean isClosed(const TopoDS_Shape& theShape)
{
  Standard_Boolean isClosed = (theShape.ShapeType() == TopAbs_SOLID);
  //
  if (!isClosed && theShape.ShapeType() == TopAbs_SHELL) {
    TopTools_IndexedDataMapOfShapeListOfShape MEF;
    TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, MEF);
    for (Standard_Integer i=1;  isClosed && i<=MEF.Extent();  ++i)
      isClosed = ( MEF(i).Extent() != 1 );
  }
  //
  return isClosed;
}
//
//  myErrorStatus     
//
// 100 - DS is already computed
// 101 - Null shape is not allowed here
// 102 - DS is not computed
// 103 - No source shapes to treat
