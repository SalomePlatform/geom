// File:	GEOMAlgo_Tools.cxx
// Created:	Mon Dec  6 11:35:29 2004
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <GEOMAlgo_Tools.ixx>

#include <gp_Pnt.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Curve.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BRepTools.hxx>

#include <BOPTools_Tools2D.hxx>
#include <IntTools_Context.hxx>

#include <GEOMAlgo_PassKey.hxx>
#include <GEOMAlgo_IndexedDataMapOfPassKeyListOfShape.hxx>

//=======================================================================
//function : RefineSDShapes
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Tools::RefineSDShapes(GEOMAlgo_IndexedDataMapOfPassKeyListOfShape &aMPKLE,
						const Standard_Real aTol,
						IntTools_Context& aCtx)
{
  Standard_Integer i, aNbE, iErr, j, aNbEE, aNbToAdd;
  TopTools_IndexedDataMapOfShapeListOfShape aMEE, aMSDE, aMEToAdd;
  //
  iErr=1;
  //
  aNbE=aMPKLE.Extent();
  for (i=1; i<=aNbE; ++i) {
    TopTools_ListOfShape& aLSDE=aMPKLE.ChangeFromIndex(i);
    aMEE.Clear();
    //
    iErr=GEOMAlgo_Tools::FindSDShapes(aLSDE, aTol, aMEE, aCtx);
    if (iErr) {
      return iErr;
    }
    //
    aNbEE=aMEE.Extent();
    if (aNbEE==1) {
      continue;  // nothing to do 
    }
    //
    for (j=1; j<=aNbEE; ++j) {
      TopTools_ListOfShape& aLEE=aMEE.ChangeFromIndex(j);
      //
      if (j==1) {
	aLSDE.Clear();
	aLSDE.Append(aLEE);
      }
      else {
	const TopoDS_Shape& aE1=aLEE.First();
	aMEToAdd.Add(aE1, aLEE);
      }
    }
  }
  //
  aNbToAdd=aMEToAdd.Extent();
  if (!aNbToAdd) {
    return aNbToAdd;
  }
  //
  for (i=1; i<=aNbToAdd; ++i) {
    GEOMAlgo_PassKey aPKE1;
    //
    const TopoDS_Shape& aE1=aMEToAdd.FindKey(i);
    const TopTools_ListOfShape& aLE=aMEToAdd(i);
    //
    aPKE1.SetIds(aE1);
    aMPKLE.Add(aPKE1, aLE);
  }
  //
  return 0;
}
//=======================================================================
//function : FindSDShapes
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Tools::FindSDShapes(const TopTools_ListOfShape& aLE,
					      const Standard_Real aTol,
					      TopTools_IndexedDataMapOfShapeListOfShape& aMEE,
					      IntTools_Context& aCtx)
{
  Standard_Integer aNbE, aNbEProcessed, aNbESD, iErr;
  TopTools_ListOfShape aLESD;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  TopTools_IndexedMapOfShape aMProcessed;
  //
  aNbE=aLE.Extent();
  if (!aNbE) {
    return 3; // Err
  }
  //
  while(1) {
    aNbEProcessed=aMProcessed.Extent();
    if (aNbEProcessed==aNbE) {
      break;
    }
    //
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Value();
      if (aMProcessed.Contains(aS)) {
	continue;
      }
      //
      aLESD.Clear();
      iErr=GEOMAlgo_Tools::FindSDShapes(aS, aLE, aTol, aLESD, aCtx);
      if (iErr) {
	return 2; // Err
      }
      //
      aNbESD=aLESD.Extent();
      if (!aNbESD) {
	return 1; // Err
      }
      //
      aMEE.Add(aS, aLESD);
      //
      aIt1.Initialize(aLESD);
      for (; aIt1.More(); aIt1.Next()) {
	const TopoDS_Shape& aE1=aIt1.Value();
	aMProcessed.Add(aE1);
      }
    }
  }
  return 0;
}
//=======================================================================
//function : FindSDShapes
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Tools::FindSDShapes(const TopoDS_Shape& aE1,
					      const TopTools_ListOfShape& aLE,
					      const Standard_Real aTol,
					      TopTools_ListOfShape& aLESD,
					      IntTools_Context& aCtx)
{
  Standard_Boolean bIsDone;
  Standard_Real aTol2, aD2;
  gp_Pnt aP1, aP2;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aTol2=aTol*aTol;
  GEOMAlgo_Tools::PointOnShape(aE1, aP1);
  //
  aIt.Initialize(aLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE2=aIt.Value();
    if (aE2.IsSame(aE1)) {
       aLESD.Append(aE2);
    }
    else {
      bIsDone=GEOMAlgo_Tools::ProjectPointOnShape(aP1, aE2, aP2, aCtx);
      if (!bIsDone) {
	return 1; 
      }
      aD2=aP1.SquareDistance(aP2);
      if(aD2<aTol2) {
	aLESD.Append(aE2);
      }
    }
  }
  return 0;
}

//=======================================================================
//function : ProjectPointOnShape
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_Tools::ProjectPointOnShape(const gp_Pnt& aP1,
						     const TopoDS_Shape& aS,
						     gp_Pnt& aP2,
						     IntTools_Context& aCtx)
{
  Standard_Boolean bIsDone=Standard_False;
  Standard_Real aT2;
  TopAbs_ShapeEnum aType;
  //
  aType=aS.ShapeType();
  switch(aType) {
    case TopAbs_EDGE: {
      const TopoDS_Edge& aE2=TopoDS::Edge(aS);
      //
      bIsDone=aCtx.ProjectPointOnEdge(aP1, aE2, aT2);
      if (!bIsDone) {
	return bIsDone;
      }
      //
      GEOMAlgo_Tools::PointOnEdge(aE2, aT2, aP2);
    }
      break;
      //
    case TopAbs_FACE: {
      const TopoDS_Face& aF2=TopoDS::Face(aS);
      GeomAPI_ProjectPointOnSurf& aProj=aCtx.ProjPS(aF2);
      //
      aProj.Perform(aP1);
      bIsDone=aProj.IsDone();
      if (!bIsDone) {
	return bIsDone;
      }
      //
      aP2=aProj.NearestPoint(); 
    }
      break;
      //  
    default:
      break; // Err
  }
  return bIsDone;
}
//=======================================================================
//function : PointOnShape
//purpose  : 
//=======================================================================
void GEOMAlgo_Tools::PointOnShape(const TopoDS_Shape& aS,
				  gp_Pnt& aP3D)
{
  TopAbs_ShapeEnum aType;
  //
  aP3D.SetCoord(99.,99.,99.);
  aType=aS.ShapeType();
  switch(aType) {
    case TopAbs_EDGE: {
      const TopoDS_Edge& aE=TopoDS::Edge(aS);
      GEOMAlgo_Tools::PointOnEdge(aE, aP3D);
      }
      break;
      //
    case TopAbs_FACE: {
      const TopoDS_Face& aF=TopoDS::Face(aS);
      GEOMAlgo_Tools::PointOnFace(aF, aP3D);
      }
      break;
      //  
    default:
      break; // Err
  }
}
//=======================================================================
//function : PointOnFace
//purpose  : 
//=======================================================================
void GEOMAlgo_Tools::PointOnFace(const TopoDS_Face& aF,
				 gp_Pnt& aP3D)
{
  Standard_Real aU, aV, aUMin, aUMax, aVMin, aVMax;
  //
  BRepTools::UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
  //
  aU=BOPTools_Tools2D::IntermediatePoint(aUMin, aUMax); 
  aV=BOPTools_Tools2D::IntermediatePoint(aVMin, aVMax); 
  //
  GEOMAlgo_Tools::PointOnFace(aF, aU, aV, aP3D);
}
//=======================================================================
//function : PointOnFace
//purpose  : 
//=======================================================================
void GEOMAlgo_Tools::PointOnFace(const TopoDS_Face& aF,
				 const Standard_Real aU,
				 const Standard_Real aV,
				 gp_Pnt& aP3D)
{
  Handle(Geom_Surface) aS;
  //
  aS=BRep_Tool::Surface(aF);
  aS->D0(aU, aV, aP3D);
}
//=======================================================================
//function : PointOnEdge
//purpose  : 
//=======================================================================
void GEOMAlgo_Tools::PointOnEdge(const TopoDS_Edge& aE,
				 gp_Pnt& aP3D)
{
  Standard_Real aTx, aT1, aT2;
  //
  BRep_Tool::Curve(aE, aT1, aT2);
  aTx=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  GEOMAlgo_Tools::PointOnEdge(aE, aTx, aP3D);
}
//=======================================================================
//function : PointOnEdge
//purpose  : 
//=======================================================================
void GEOMAlgo_Tools::PointOnEdge(const TopoDS_Edge& aE,
				 const Standard_Real aT,
				 gp_Pnt& aP3D)
{
  Standard_Real aT1, aT2;
  Handle(Geom_Curve) aC3D;
  //
  aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  aC3D->D0(aT, aP3D);
}
/*
//=======================================================================
//function : FindSDEdges
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Tools::FindSDEdges(const TopoDS_Edge& aE1,
					     const TopTools_ListOfShape& aLE,
					     const Standard_Real aTol,
					     TopTools_ListOfShape& aLESD,
					     IntTools_Context& aCtx)
{
  Standard_Boolean bIsDone;
  Standard_Real aT2, aTol2, aD2;
  gp_Pnt aP1, aP2;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aTol2=aTol*aTol;
  //
  GEOMAlgo_Tools::PointOnEdge(aE1, aP1);
  aIt.Initialize(aLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Edge& aE2=TopoDS::Edge(aIt.Value());
    if (aE2.IsSame(aE1)) {
       aLESD.Append(aE2);
    }
    else {
      bIsDone=aCtx.ProjectPointOnEdge(aP1, aE2, aT2);
      if (!bIsDone) {
	return 1; 
      }
      GEOMAlgo_Tools::PointOnEdge(aE2, aT2, aP2);
      aD2=aP1.SquareDistance(aP2);
      if(aD2<aTol2) {
	aLESD.Append(aE2);
      }
    }
  }
  return 0;
}
//=======================================================================
//function : FindSDFaces
//purpose  : 
//=======================================================================
Standard_Integer GEOMAlgo_Tools::FindSDFaces(const TopoDS_Face& aF1,
					     const TopTools_ListOfShape& aLF,
					     const Standard_Real aTol,
					     TopTools_ListOfShape& aLFSD,
					     IntTools_Context& aCtx)
{
  Standard_Boolean bIsDone;
  Standard_Real aTol2, aD2;
  gp_Pnt aP1, aP2;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aTol2=aTol*aTol;
  //
  GEOMAlgo_Tools::PointOnFace(aF1, aP1);
  aIt.Initialize(aLF);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Face& aF2=TopoDS::Face(aIt.Value());
    if (aF2.IsSame(aF1)) {
       aLFSD.Append(aF2);
    }
    else {
      GeomAPI_ProjectPointOnSurf& aProj=aCtx.ProjPS(aF2);
      aProj.Perform(aP1);
      bIsDone=aProj.IsDone();
      if (!bIsDone) {
	return 1; //??
      }
      //
      aP2=aProj.NearestPoint();
      aD2=aP1.SquareDistance(aP2);
      if(aD2<aTol2) {
	aLFSD.Append(aF2);
      }
    }
  }
  return 0;
}
*/
