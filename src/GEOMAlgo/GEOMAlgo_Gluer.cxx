// File:	GEOMAlgo_Gluer.cxx
// Created:	Sat Dec 04 12:45:53 2004
// Author:	Peter KURNEV
//		<peter@PREFEX>


#include <GEOMAlgo_Gluer.ixx>

#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <Bnd_BoundSortBox.hxx>

#include <TopLoc_Location.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepBndLib.hxx>

#include <IntTools_Context.hxx>
#include <BOPTools_Tools.hxx>
#include <BOPTools_Tools3D.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BOP_CorrectTolerances.hxx>

#include <GEOMAlgo_IndexedDataMapOfIntegerShape.hxx>
#include <GEOMAlgo_IndexedDataMapOfShapeBox.hxx>
#include <GEOMAlgo_IndexedDataMapOfPassKeyListOfShape.hxx>
#include <GEOMAlgo_PassKey.hxx>
#include <GEOMAlgo_Tools.hxx>

//=======================================================================
//function : GEOMAlgo_Gluer
//purpose  : 
//=======================================================================
GEOMAlgo_Gluer::GEOMAlgo_Gluer()
:
  GEOMAlgo_ShapeAlgo()
{
  myTolerance=0.0001;
  myTol=myTolerance;
  myCheckGeometry=Standard_True;
  myNbAlone=0;
}
//=======================================================================
//function : ~GEOMAlgo_Gluer
//purpose  : 
//=======================================================================
GEOMAlgo_Gluer::~GEOMAlgo_Gluer()
{
}
//=======================================================================
//function : SetCheckGeometry
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::SetCheckGeometry(const Standard_Boolean aFlag)
{
  myCheckGeometry=aFlag;
}
//=======================================================================
//function : CheckGeometry
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_Gluer::CheckGeometry() const
{
  return myCheckGeometry;
}
//=======================================================================
//function : AloneShapes
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Gluer::AloneShapes()const
{
  return myNbAlone;
}
//=======================================================================
//function : Images
//purpose  : 
//=======================================================================
const TopTools_DataMapOfShapeListOfShape& GEOMAlgo_Gluer::Images()const
{
  return myImages;
}
//=======================================================================
//function : Origins
//purpose  : 
//=======================================================================
const TopTools_DataMapOfShapeShape& GEOMAlgo_Gluer::Origins()const
{
  return myOrigins;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  //
  Standard_Integer i;
  const Standard_Integer aNb=8;
  void (GEOMAlgo_Gluer::* pF[aNb])()={
    &GEOMAlgo_Gluer::CheckData,       &GEOMAlgo_Gluer::InnerTolerance,
    &GEOMAlgo_Gluer::MakeVertices,    &GEOMAlgo_Gluer::MakeEdges,
    &GEOMAlgo_Gluer::MakeFaces,       &GEOMAlgo_Gluer::MakeShells,
    &GEOMAlgo_Gluer::MakeSolids,      &GEOMAlgo_Gluer::CheckResult
  };
  //
  for (i=0; i<aNb; ++i) {
    (this->*pF[i])();
    if (myErrorStatus) {
      return;
    }
  }
}
//=======================================================================
//function : MakeVertices
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeVertices()
{
  myErrorStatus=0;
  //
  Standard_Integer j, i, aNbV, aIndex, aNbVSD;
  TColStd_ListIteratorOfListOfInteger aIt;
  Handle(Bnd_HArray1OfBox) aHAB;
  Bnd_BoundSortBox aBSB;
  TopoDS_Shape aSTmp, aVF;
  TopoDS_Vertex aVnew;
  TopTools_IndexedMapOfShape aMV, aMVProcessed;
  TopTools_ListIteratorOfListOfShape aItS;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItIm;
  TopTools_DataMapOfShapeListOfShape aMVV;
  GEOMAlgo_IndexedDataMapOfIntegerShape aMIS;
  GEOMAlgo_IndexedDataMapOfShapeBox aMSB;
  //
  TopExp::MapShapes(myShape, TopAbs_VERTEX, aMV);
  aNbV=aMV.Extent();
  if (!aNbV) {
    myErrorStatus=2; // no vertices in source shape
    return;
  }
  //
  aHAB=new Bnd_HArray1OfBox(1, aNbV);
  //
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMV(i);
    Bnd_Box aBox;
    //
    aBox.SetGap(myTol); 
    BRepBndLib::Add(aV, aBox);
    aHAB->SetValue(i, aBox);
    aMIS.Add(i, aV);
    aMSB.Add(aV, aBox); 
  }
  //
  aBSB.Initialize(aHAB);
  //
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMV(i);
    //
    if (aMVProcessed.Contains(aV)) {
      continue;
    }
    //
    const Bnd_Box& aBoxV=aMSB.FindFromKey(aV);
    const TColStd_ListOfInteger& aLI=aBSB.Compare(aBoxV);
    aNbVSD=aLI.Extent();
    if (!aNbVSD) {
      myErrorStatus=3; // it must not be 
      return;
    }
    //
    // Images
    //
    TopTools_ListOfShape aLVSD;
    //
    aIt.Initialize(aLI);
    for (j=0; aIt.More(); aIt.Next(), ++j) {
      aIndex=aIt.Value();
      const TopoDS_Shape& aVx=aMIS.FindFromKey(aIndex);
      if(!j) {
	aVF=aVx;
      }
      aLVSD.Append(aVx);
      aMVProcessed.Add(aVx);
    }
    //
    myImages.Bind(aVF, aLVSD);
  }
  //
  // Make new vertices
  aMV.Clear();
  aItIm.Initialize(myImages);
  for (; aItIm.More(); aItIm.Next()) {
    const TopoDS_Shape& aV=aItIm.Key();
    const TopTools_ListOfShape& aLVSD=aItIm.Value();
    aNbVSD=aLVSD.Extent();
    if (aNbVSD>1) {
      aMV.Add(aV);
      MakeVertex(aLVSD, aVnew);
      aMVV.Bind(aVnew, aLVSD);
    }
  }
  //
  // UnBind old vertices
  aNbV=aMV.Extent();
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Shape& aV=aMV(i);
    myImages.UnBind(aV);
  }
  //
  // Bind new vertices
  aItIm.Initialize(aMVV);
  for (; aItIm.More(); aItIm.Next()) {
    const TopoDS_Shape& aV=aItIm.Key();
    const TopTools_ListOfShape& aLVSD=aItIm.Value();
    myImages.Bind(aV, aLVSD);
  }
  //
  // Origins
  aItIm.Initialize(myImages);
  for (; aItIm.More(); aItIm.Next()) {
    const TopoDS_Shape& aV=aItIm.Key();
    const TopTools_ListOfShape& aLVSD=aItIm.Value();
    //
    aItS.Initialize(aLVSD);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aVSD=aItS.Value();
      if (!myOrigins.IsBound(aVSD)) {
	myOrigins.Bind(aVSD, aV);
      }
    }
  }
}
//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeSolids()
{
  myErrorStatus=0;
  //
  Standard_Integer aNbS;
  TopAbs_Orientation anOr;
  TopoDS_Compound aCmp;
  TopoDS_Solid aNewSolid;
  TopTools_IndexedMapOfShape aMS;
  TopExp_Explorer aExpS, aExp;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aCmp);
  //
  aNbS=aMS.Extent();
  aExpS.Init(myShape, TopAbs_SOLID);
  for (; aExpS.More(); aExpS.Next()) {
    const TopoDS_Solid& aSolid=TopoDS::Solid(aExpS.Current());
    if (aMS.Contains(aSolid)) {
      continue;
    }
    aMS.Add(aSolid);
    //
    anOr=aSolid.Orientation();
    //
    aBB.MakeSolid(aNewSolid);
    aNewSolid.Orientation(anOr);
    //
    aExp.Init(aSolid, TopAbs_SHELL);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aShell=aExp.Current();
      const TopoDS_Shape& aShellR=myOrigins.Find(aShell);
      aBB.Add(aNewSolid, aShellR);
    }
    //
    TopTools_ListOfShape aLS;
    //
    aLS.Append(aSolid);
    myImages.Bind(aNewSolid, aLS);
    myOrigins.Bind(aSolid, aNewSolid);
    //
    aBB.Add(aCmp, aNewSolid);
  }
  //
  myResult=aCmp;
  //
  aNbS=aMS.Extent();
  if (aNbS) {
    Standard_Real aTol=1.e-7;
    BOP_CorrectTolerances::CorrectCurveOnSurface(myResult);
  }
}
//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeShells()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsToReverse;
  Standard_Integer i, aNbS;
  TopAbs_Orientation anOr;
  TopoDS_Shell aNewShell;
  TopoDS_Face aFR;
  TopTools_IndexedMapOfShape aMS;
  TopExp_Explorer aExp;
  BRep_Builder aBB;
  //
  TopExp::MapShapes(myShape, TopAbs_SHELL, aMS);
  //
  aNbS=aMS.Extent();
  for (i=1; i<=aNbS; ++i) {
    const TopoDS_Shell& aShell=TopoDS::Shell(aMS(i));
    anOr=aShell.Orientation();
    //
    aBB.MakeShell(aNewShell);
    aNewShell.Orientation(anOr);
    aExp.Init(aShell, TopAbs_FACE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Face& aF=TopoDS::Face(aExp.Current());
      aFR=TopoDS::Face(myOrigins.Find(aF));
      if (aFR.IsSame(aF)) {
	aBB.Add(aNewShell, aF);
	continue;
      }
      bIsToReverse=IsToReverse(aFR, aF);
      if (bIsToReverse) {
	aFR.Reverse();
      }
      aBB.Add(aNewShell, aFR);
    }
    //
    TopTools_ListOfShape aLS;
    //
    aLS.Append(aShell);
    myImages.Bind(aNewShell, aLS);
    myOrigins.Bind(aShell, aNewShell);
  }
}
//=======================================================================
//function : MakeFaces
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeFaces()
{
  MakeShapes(TopAbs_FACE);
}
//=======================================================================
//function : MakeEdges
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeEdges()
{
  MakeShapes(TopAbs_EDGE);
}
//=======================================================================
//function : MakeShapes
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeShapes(const TopAbs_ShapeEnum aType)
{
  myErrorStatus=0;
  //
  Standard_Integer i, aNbF, aNbSDF, iErr;
  TopoDS_Shape aNewShape;
  TopTools_IndexedMapOfShape aMF;
  TopTools_ListIteratorOfListOfShape aItS;
  GEOMAlgo_PassKey aPKF;
  GEOMAlgo_IndexedDataMapOfPassKeyListOfShape aMPKLF;
  //
  TopExp::MapShapes(myShape, aType, aMF);
  //
  aNbF=aMF.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aS=aMF(i);
    // 
    aPKF.Clear();
    if (aType==TopAbs_FACE) {
      const TopoDS_Face& aF=TopoDS::Face(aS);
      FacePassKey(aF, aPKF);
    }
    else if (aType==TopAbs_EDGE) {
      const TopoDS_Edge& aE=TopoDS::Edge(aS);
      EdgePassKey(aE, aPKF);
    }
    //
    if (myErrorStatus) {
      return;
    }
    //
    if (aMPKLF.Contains(aPKF)) {
      TopTools_ListOfShape& aLSDF=aMPKLF.ChangeFromKey(aPKF);
      aLSDF.Append(aS);
    }
    else {
      TopTools_ListOfShape aLSDF;
      //
      aLSDF.Append(aS);
      aMPKLF.Add(aPKF, aLSDF);
    }
  }
  // check geometric coincidence
  if (myCheckGeometry) {
    iErr=GEOMAlgo_Tools::RefineSDShapes(aMPKLF, myTol, myContext); 
    if (iErr) {
      myErrorStatus=200;
      return;
    }
  }
  //
  // Images/Origins
  aNbF=aMPKLF.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopTools_ListOfShape& aLSDF=aMPKLF(i);
    aNbSDF=aLSDF.Extent();
    if (!aNbSDF) {
      myErrorStatus=4; // it must not be
    }
    //
    const TopoDS_Shape& aS1=aLSDF.First();
    if (aType==TopAbs_FACE) {
      TopoDS_Face aNewFace;
      //
      const TopoDS_Face& aF1=TopoDS::Face(aS1);
      MakeFace(aF1, aNewFace);
      aNewShape=aNewFace;
    }
    else if (aType==TopAbs_EDGE) {
      TopoDS_Edge aNewEdge;
      //
      const TopoDS_Edge& aE1=TopoDS::Edge(aS1);
      MakeEdge(aE1, aNewEdge);
      aNewShape=aNewEdge;
    }
    //
    myImages.Bind(aNewShape, aLSDF);
    // origins
    aItS.Initialize(aLSDF);
    for (; aItS.More(); aItS.Next()) {
      const TopoDS_Shape& aFSD=aItS.Value();
      if (!myOrigins.IsBound(aFSD)) {
	myOrigins.Bind(aFSD, aNewShape);
      }
    }
  }
}
//=======================================================================
//function : CheckResult
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::CheckResult()
{
  myErrorStatus=0;
  //
  if (myResult.IsNull()) {
    myErrorStatus=6;
    return; 
  }
  // 
  Standard_Boolean bFound;
  Standard_Integer i, j, aNbS, aNbFS, aNbSx;
  TopTools_IndexedMapOfShape aMS, aMFS;
  TopTools_IndexedDataMapOfShapeListOfShape aMFR;
  //
  TopExp::MapShapesAndAncestors(myResult, TopAbs_FACE, TopAbs_SOLID, aMFR);
  TopExp::MapShapes(myResult, TopAbs_SOLID, aMS);
  //
  
  myNbAlone=0;
  aNbS=aMS.Extent();
  for (i=1; i<=aNbS; ++i) {
    const TopoDS_Shape& aSolid=aMS(i);
    //
    aMFS.Clear();
    TopExp::MapShapes(aSolid, TopAbs_FACE, aMFS);
    //
    bFound=Standard_False;
    aNbFS=aMFS.Extent();
    for (j=1; j<=aNbFS; ++j) {
      const TopoDS_Shape& aFS=aMFS(j);
      if (aMFR.Contains(aFS)) {
	const TopTools_ListOfShape& aLSx=aMFR.FindFromKey(aFS);
	aNbSx=aLSx.Extent();
	if (aNbSx==2) {
	  bFound=!bFound;
	  break;
	}
      }
    }
    //
    if (!bFound) {
      myWarningStatus=1;
      ++myNbAlone;
      //break;
    }
  }
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::CheckData()
{
  myErrorStatus=0;
  //
  if (myShape.IsNull()) {
    myErrorStatus=5;
    return; 
  }
}
//=======================================================================
//function : InnerTolerance
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::InnerTolerance()
{
  myErrorStatus=0;
  //
  /*
  Standard_Integer i;
  Standard_Real aX[3][2], dH, dHmin, aCoef, aTolTresh;
  Bnd_Box aBox;
  //
  BRepBndLib::Add(myShape, aBox);
  aBox.Get(aX[0][0], aX[1][0], aX[2][0], aX[0][1], aX[1][1], aX[2][1]);
  //
  dHmin=aX[0][1]-aX[0][0];
  for (i=1; i<3; ++i) {
    dH=aX[i][1]-aX[i][0];
    if (dH<dHmin) {
      dHmin=dH;
    }
  }
  //
  myTol=myTolerance;
  aCoef=0.01;
  aTolTresh=aCoef*dHmin;
  if (myTol>aTolTresh) {
    myTol=aTolTresh;
  }
  */
  myTol=myTolerance;
}
//=======================================================================
//function : FacePassKey
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::FacePassKey(const TopoDS_Face& aF, 
				 GEOMAlgo_PassKey& aPK)
{
  Standard_Integer i, aNbE, aNbMax;
  TopTools_ListOfShape aLE;
  TopTools_IndexedMapOfShape aME;
  //
  TopExp::MapShapes(aF, TopAbs_EDGE, aME);
  aNbE=aME.Extent();
  aNbMax=aPK.NbMax();
  if (!aNbE || aNbE>aNbMax) {
     myErrorStatus=101; // temprorary
     return;
  }
  //
  for (i=1; i<=aNbE; ++i) {
    const TopoDS_Shape& aE=aME(i);
    if (!myOrigins.IsBound(aE)) {
      myErrorStatus=102;
      return;
    }
    const TopoDS_Shape& aER=myOrigins.Find(aE);
    aLE.Append(aER);
  }
  aPK.SetIds(aLE);
}
//=======================================================================
//function : EdgePassKey
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::EdgePassKey(const TopoDS_Edge& aE, 
				 GEOMAlgo_PassKey& aPK)
{
  TopoDS_Vertex aV1, aV2;
  //
  TopExp::Vertices(aE, aV1, aV2);
  //
  if (!myOrigins.IsBound(aV1) || !myOrigins.IsBound(aV2) ) {
     myErrorStatus=100;
     return;
  }
  const TopoDS_Shape& aVR1=myOrigins.Find(aV1);
  const TopoDS_Shape& aVR2=myOrigins.Find(aV2);
  aPK.SetIds(aVR1, aVR2);
}
//=======================================================================
//function : MakeVertex
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeVertex(const TopTools_ListOfShape& aLV, 
				TopoDS_Vertex& aNewVertex)
{
  Standard_Integer aNbV;
  Standard_Real aTolV, aD, aDmax;
  gp_XYZ aGC;
  gp_Pnt aP3D, aPGC;
  TopoDS_Vertex aVx;
  BRep_Builder aBB;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aNbV=aLV.Extent();
  if (!aNbV) {
    return;
  }
  //
  // center of gravity
  aGC.SetCoord(0.,0.,0.);
  aIt.Initialize(aLV);
  for (; aIt.More(); aIt.Next()) {
    aVx=TopoDS::Vertex(aIt.Value());
    aP3D=BRep_Tool::Pnt(aVx);
    aGC+=aP3D.XYZ();
  }
  aGC/=(Standard_Real)aNbV;
  aPGC.SetXYZ(aGC);
  //
  // tolerance value
  aDmax=-1.;
  aIt.Initialize(aLV);
  for (; aIt.More(); aIt.Next()) {
    aVx=TopoDS::Vertex(aIt.Value());
    aP3D=BRep_Tool::Pnt(aVx);
    aTolV=BRep_Tool::Tolerance(aVx);
    aD=aPGC.Distance(aP3D)+aTolV;
    if (aD>aDmax) {
      aDmax=aD;
    }
  }
  //
  aBB.MakeVertex (aNewVertex, aPGC, aDmax);
}
//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeEdge(const TopoDS_Edge& aE, 
			      TopoDS_Edge& aNewEdge)
{
  //modified by NIZNHY-PKV Thu Dec 30 11:15:23 2004 f
  myErrorStatus=0;
  //
  Standard_Boolean bIsDE;
  Standard_Real aT1, aT2;
  TopoDS_Vertex aV1, aV2, aVR1, aVR2;
  TopoDS_Edge aEx;
  //
  bIsDE=BRep_Tool::Degenerated(aE);
  //
  aEx=aE;
  aEx.Orientation(TopAbs_FORWARD);
  //
  TopExp::Vertices(aEx, aV1, aV2);
  //
  aT1=BRep_Tool::Parameter(aV1, aEx);
  aT2=BRep_Tool::Parameter(aV2, aEx);
  //
  aVR1=TopoDS::Vertex(myOrigins.Find(aV1));
  aVR1.Orientation(TopAbs_FORWARD);
  aVR2=TopoDS::Vertex(myOrigins.Find(aV2));
  aVR2.Orientation(TopAbs_REVERSED);
  //
  if (bIsDE) {
    Standard_Real aTol;
    BRep_Builder aBB;
    TopoDS_Edge E;
    TopAbs_Orientation anOrE;
    //
    anOrE=aE.Orientation();
    aTol=BRep_Tool::Tolerance(aE);
    //
    E=aEx;
    E.EmptyCopy();
    //
    aBB.Add  (E, aVR1);
    aBB.Add  (E, aVR2);
    aBB.Range(E, aT1, aT2);
    aBB.Degenerated(E, Standard_True);
    aBB.UpdateEdge(E, aTol);
    //
    aNewEdge=E;
  }
  //
  else {
    BOPTools_Tools::MakeSplitEdge(aEx, aVR1, aT1, aVR2, aT2, aNewEdge); 
  }
  //modified by NIZNHY-PKV Thu Dec 30 11:15:28 2004 t
}
//
//=======================================================================
//function : MakeFace
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::MakeFace(const TopoDS_Face& aF, 
			      TopoDS_Face& aNewFace)
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsToReverse, bIsUPeriodic;
  Standard_Real aTol, aUMin, aUMax, aVMin, aVMax;
  TopoDS_Edge aER;
  TopoDS_Wire newWire;
  TopoDS_Face aFFWD, newFace;
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aS;
  Handle(Geom2d_Curve) aC2D;
  TopExp_Explorer aExpW, aExpE;
  BRep_Builder aBB;
  //
  aFFWD=aF;
  aFFWD.Orientation(TopAbs_FORWARD);
  //
  aS=BRep_Tool::Surface(aFFWD, aLoc);
  bIsUPeriodic=GEOMAlgo_Tools::IsUPeriodic(aS);
  aTol=BRep_Tool::Tolerance(aFFWD);
  BRepTools::UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
  //
  aBB.MakeFace (newFace, aS, aLoc, aTol);
  //
  aExpW.Init(aFFWD, TopAbs_WIRE);
  for (; aExpW.More(); aExpW.Next()) {
    aBB.MakeWire(newWire);
    const TopoDS_Wire& aW=TopoDS::Wire(aExpW.Current());
    aExpE.Init(aW, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(aExpE.Current());
      aER=TopoDS::Edge(myOrigins.Find(aE));
      //
      aER.Orientation(TopAbs_FORWARD);
      if (!BRep_Tool::Degenerated(aER)) {//modified by NIZNHY-PKV Thu Dec 30 11:31:37 2004 ft
	// build p-curve
	if (bIsUPeriodic) {
	  GEOMAlgo_Tools::RefinePCurveForEdgeOnFace(aER, aFFWD, aUMin, aUMax);
	}
	BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aER, aFFWD);
	
	// orient image 
	bIsToReverse=BOPTools_Tools3D::IsSplitToReverse1(aER, aE, myContext);
	if (bIsToReverse) {
	  aER.Reverse();
	}
      }
      else {
	aER.Orientation(aE.Orientation());
      }
      //
      aBB.Add(newWire, aER);
    }
    aBB.Add(newFace, newWire);
  }
  aNewFace=newFace;
}
//=======================================================================
//function : IsToReverse
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_Gluer::IsToReverse(const TopoDS_Face& aFR,
					     const TopoDS_Face& aF)
{
  Standard_Boolean bRet;
  Standard_Real aT, aT1, aT2, aTR, aScPr;
  TopExp_Explorer aExp;
  Handle(Geom_Curve)aC3D;
  gp_Pnt aP;
  gp_Dir aDNF, aDNFR;
  //
  bRet=Standard_False;
  //
  aExp.Init(aF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE=TopoDS::Edge(aExp.Current());
    //modified by NIZNHY-PKV Thu Dec 30 11:38:05 2004 f
    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }
    //modified by NIZNHY-PKV Thu Dec 30 11:38:08 2004 t
    const TopoDS_Edge& aER=TopoDS::Edge(myOrigins.Find(aE));
    //
    aC3D=BRep_Tool::Curve(aE, aT1, aT2);
    aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
    aC3D->D0(aT, aP);
    myContext.ProjectPointOnEdge(aP, aER, aTR);
    //
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aE, aF, aT, aDNF);
    if (aF.Orientation()==TopAbs_REVERSED) {
      aDNF.Reverse();
    }
    //
    BOPTools_Tools3D::GetNormalToFaceOnEdge (aER, aFR, aTR, aDNFR);
    if (aFR.Orientation()==TopAbs_REVERSED) {
      aDNFR.Reverse();
    }
    //
    aScPr=aDNF*aDNFR;
    return (aScPr<0.);
  }
  return bRet;
}
//

//
// ErrorStatus
//
// 1   - the object is just initialized 
// 2   - no vertices found in source shape
// 3   - nb same domain vertices for the vertex Vi =0
// 4   - nb same domain edges(faces) for the edge Ei(face Fi)  =0
// 5   - source shape is Null
// 6   - result shape is Null
// 101 - nb edges > PassKey.NbMax() in FacesPassKey()
// 102 - the edge Ei can not be found in myOrigins Map
// 100 - the vertex Vi can not be found in myOrigins Map
//
// WarningStatus
//
// 1   - some shapes can not be glued by faces
//
/*
//=======================================================================
//function : BuildResult
//purpose  : 
//=======================================================================
void GEOMAlgo_Gluer::BuildResult()
{
  Standard_Boolean bAdded;
  TopoDS_Compound aCmp;
  BRep_Builder aBB;
  TopAbs_ShapeEnum aType;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItIm;
  //
  aBB.MakeCompound(aCmp);
  //
  bAdded=Standard_False;
  aItIm.Initialize(myImages);
  for (; aItIm.More(); aItIm.Next()) {
    const TopoDS_Shape& aIm=aItIm.Key();
    aType=aIm.ShapeType();
    if(aType==TopAbs_SOLID) {
      bAdded=Standard_True;
      aBB.Add(aCmp, aIm);
    }
  }
  myResult=aCmp;
  //
  if (bAdded) {
    Standard_Real aTol=1.e-7;
    BOP_CorrectTolerances::CorrectCurveOnSurface(myResult);
    //BRepLib::SameParameter(myResult, aTol, bAdded);
  }
}
*/
