// File:	GEOMAlgo_FinderShapeOn.cxx
// Created:	Tue Jan 11 14:44:31 2005
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_FinderShapeOn.ixx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>

#include <TopoDS_Iterator.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

#include <BRep_Builder.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRepLib_MakeFace.hxx>
#include <BRepLib_FaceError.hxx>

#include <BOPTools_DSFiller.hxx>

#include <GEOMAlgo_WireSolid.hxx>
#include <GEOMAlgo_ShellSolid.hxx>
#include <GEOMAlgo_VertexSolid.hxx>
#include <GEOMAlgo_ShapeSolid.hxx>


//=======================================================================
//function : GEOMAlgo_FinderShapeOn
//purpose  : 
//=======================================================================
GEOMAlgo_FinderShapeOn::GEOMAlgo_FinderShapeOn()
:
  GEOMAlgo_ShapeAlgo()
{
  myTolerance=0.0001;
  myShapeType=TopAbs_VERTEX;
  myState=GEOMAlgo_ST_UNKNOWN; 
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
GEOMAlgo_FinderShapeOn::~GEOMAlgo_FinderShapeOn()
{
}
//=======================================================================
//function : SetSurface
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::SetSurface(const Handle(Geom_Surface)& aS)
{
  mySurface=aS;
}
//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================
const Handle(Geom_Surface)& GEOMAlgo_FinderShapeOn::Surface() const
{
  return mySurface;
}
//=======================================================================
//function : SetShapeType
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::SetShapeType(const TopAbs_ShapeEnum aType)
{
  myShapeType=aType;
}
//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum GEOMAlgo_FinderShapeOn::ShapeType()const
{
  return myShapeType;
}
//=======================================================================
//function : SetState
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::SetState(const GEOMAlgo_State aState)
{
  myState=aState;
}
//=======================================================================
//function : State
//purpose  : 
//=======================================================================
GEOMAlgo_State GEOMAlgo_FinderShapeOn::State() const
{
  return myState;
}
//=======================================================================
// function: Shapes
// purpose: 
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_FinderShapeOn::Shapes() const
{
  return myLS;
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  myLS.Clear();
  //
  if (!myResult.IsNull()){
    myResult.Nullify();
  }
  //
  CheckData();
  if(myErrorStatus) {
    return;
  }
  //
  MakeArguments();
  if(myErrorStatus || myWarningStatus) {
    return;
  }
  //
  Find();
  if(myErrorStatus) {
    return;
  }
  //
}
//=======================================================================
//function : Find
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::Find()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsDone;
  Standard_Integer iErr;
  TopTools_ListIteratorOfListOfShape aIt;
  BRep_Builder aBB;
  BOPTools_DSFiller aDF;
  GEOMAlgo_ShapeSolid* pSS;
  //
  // 1. Prepare DSFiller
  aDF.SetShapes (myArg1, myArg2);
  bIsDone=aDF.IsDone();
  if (!bIsDone) {
    myErrorStatus=30; // wrong args are used for DSFiller
    return;
  }
  aDF.Perform();
  bIsDone=aDF.IsDone();
  if (!bIsDone) {
    myErrorStatus=31; // DSFiller failed
    return;
  }
  // 
  // 2. Find shapes
  myLS.Clear();
  //
  if (myShapeType==TopAbs_VERTEX) {
    pSS=new GEOMAlgo_VertexSolid;
  }
  else if (myShapeType==TopAbs_EDGE) {
    pSS=new GEOMAlgo_WireSolid;
  }
  else if (myShapeType==TopAbs_FACE) {
    pSS=new GEOMAlgo_ShellSolid;
  }
  //
  pSS->SetFiller(aDF);
  pSS->Perform();
  iErr=pSS->ErrorStatus();
  if (iErr) {
    myErrorStatus=32; // builder ShapeSolid failed
    delete pSS;
    return;
  }
  //
  const TopTools_ListOfShape& aLS=pSS->Shapes(myState);
  //
  aIt.Initialize(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSImage=aIt.Value(); 
    if (myImages.IsBound(aSImage)) { 
      const TopoDS_Shape& aS=myImages.Find(aSImage); 
      myLS.Append(aS);
    }
    else {
      myErrorStatus=33;// can not find original shape
      return; 
    }
  }
  //
  delete pSS;
}
//=======================================================================
//function : MakeArguments
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::MakeArguments()
{
  myErrorStatus=0;
  //
  Standard_Integer i, aNb;
  BRepLib_FaceError aFErr;
  BRepLib_MakeFace aMF;
  TopTools_IndexedMapOfShape aM;
  BRep_Builder aBB;
  TopoDS_Compound aCmp;
  TopoDS_Shell aSh;
  TopoDS_Solid aSd;
  TopoDS_Shape aSC;
  TopTools_DataMapOfShapeShape aOriginals;
  TopExp_Explorer aExp;
  //
  // Argument 1
  aMF.Init(mySurface, Standard_True);
  aFErr=aMF.Error();
  if (aFErr!=BRepLib_FaceDone) {
    myErrorStatus=20; // can not build the face
    return;
  }
  //
  const TopoDS_Shape& aF=aMF.Shape();
  //
  // update tolerance
  aExp.Init(aF, TopAbs_VERTEX);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aExp.Current());
    aBB.UpdateVertex(aV, myTolerance);
  }
  aExp.Init(aF, TopAbs_EDGE);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Edge& aE=TopoDS::Edge(aExp.Current());
    aBB.UpdateEdge(aE, myTolerance);
  }
  const TopoDS_Face& aFace=TopoDS::Face(aF);
  aBB.UpdateFace(aFace, myTolerance);
  //
  // make solid
  aBB.MakeShell(aSh);
  aBB.Add(aSh, aFace);
  aBB.MakeSolid(aSd);
  aBB.Add(aSd, aSh);
  myArg1=aSd;
  //
  // Argument 2
  //
  myImages.Clear();
  //
  GEOMAlgo_FinderShapeOn::CopySource(myShape, myImages, aOriginals, aSC);
  //
  TopExp::MapShapes(aSC, myShapeType, aM);
  aNb=aM.Extent();
  if (!aNb) {
    myWarningStatus=10; // No found subshapes of type myShapeType
    return;
  }
  //
  aBB.MakeCompound(aCmp);
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aS=aM(i);
    aBB.Add(aCmp, aS);
  }
  myArg2=aCmp;
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::CheckData()
{
  myErrorStatus=0;
  //
  if(mySurface.IsNull()) {
    myErrorStatus=10; // mySurface=NULL
    return;
  }
  //
  if (myShape.IsNull()) {
    myErrorStatus=11; // myShape=NULL
    return;
  }
  //
  if (!(myShapeType==TopAbs_VERTEX ||
	myShapeType==TopAbs_EDGE ||
	myShapeType==TopAbs_FACE)) {
    myErrorStatus=12; // unallowed subshape type
    return;
  }
  //
  if (myState==GEOMAlgo_ST_UNKNOWN || 
      myState==GEOMAlgo_ST_INOUT) {
    myErrorStatus=13; // unallowed state type
    return;
  }
}
//
//=======================================================================
//function : CopySource
//purpose  : 
//=======================================================================
void GEOMAlgo_FinderShapeOn::CopySource(const TopoDS_Shape& aE,
					TopTools_DataMapOfShapeShape& aImages,
					TopTools_DataMapOfShapeShape& aOriginals,
					TopoDS_Shape& aEx)
{
  Standard_Boolean bFree;
  TopAbs_ShapeEnum aType;
  Standard_Integer aR;
  BRep_Builder BB;
  TopoDS_Iterator aIt;
  //
  aType=aE.ShapeType();
  //
  if (aOriginals.IsBound(aE)) {
    aEx=aOriginals.ChangeFind(aE);
    if (aType==TopAbs_EDGE) {
      return;
    }
  }
  else {
    aEx=aE.EmptyCopied();
    aOriginals.Bind(aE, aEx);
    aImages.Bind(aEx, aE);
  }
  //
  aR=(Standard_Integer)aType+1;
  if (aR>TopAbs_VERTEX) {
    return;
  }
  //
  bFree=aEx.Free();
  aEx.Free(Standard_True);
  //
  aType=(TopAbs_ShapeEnum) aR;
  //
  aIt.Initialize(aE);//, Standard_False);
  for (; aIt.More();  aIt.Next()) {
    const TopoDS_Shape& aV=aIt.Value();
    TopoDS_Shape aVx;
    //
    CopySource (aV, aImages, aOriginals, aVx);  
    //
    aVx.Orientation(aV.Orientation());
    BB.Add(aEx, aVx);
  }
  //
  aEx.Free(bFree);
}

//
// myErrorStatus :
//
// 10 -mySurface=NULL
// 11 -myShape=NULL
// 12 -unallowed type of subshapes 
// 13 -unallowed state  
// 20 -can not build the face
// 30 -wrong args are used for DSFiller
// 31 -DSFiller failed
// 32 -builder ShapeSolid failed
// 33 -can not find original shape
//
// myWarningStatus
//
// 10 -subshapes of type myShapeType can not be fond in myShape

