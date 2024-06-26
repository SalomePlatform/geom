// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// File:        GEOMAlgo_FinderShapeOn2.cxx
// Created:     Fri Mar  4 10:31:06 2005
// Author:      Peter KURNEV
//              <pkv@irinox>
//
#include <GEOMAlgo_FinderShapeOn2.hxx>
#include <GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger.hxx>
#include <GEOMAlgo_DataMapOfPassKeyInteger.hxx>
#include <GEOMAlgo_ListIteratorOfListOfPnt.hxx>
#include <GEOMAlgo_PassKey.hxx>
#include <GEOMAlgo_StateCollector.hxx>
#include <GEOMAlgo_SurfaceTools.hxx>

#include <GEOMUtils.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <HatchGen_Domain.hxx>
#include <IntTools_Tools.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopAbs_State.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <IntTools_Context.hxx>

//=======================================================================
//function : 
//purpose  :
//=======================================================================
GEOMAlgo_FinderShapeOn2::GEOMAlgo_FinderShapeOn2()
:
  GEOMAlgo_ShapeAlgo()
{
  myTolerance=0.0001;
  myShapeType=TopAbs_VERTEX;
  myState=GEOMAlgo_ST_UNKNOWN;
  myNbPntsMin=3;
  myNbPntsMax=0;
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_FinderShapeOn2::~GEOMAlgo_FinderShapeOn2()
{
}
//=======================================================================
//function : SetClsf
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::SetClsf(const Handle(GEOMAlgo_Clsf)& aClsf)
{
  myClsf=aClsf;
}
//=======================================================================
//function : Clsf
//purpose  :
//=======================================================================
const Handle(GEOMAlgo_Clsf)& GEOMAlgo_FinderShapeOn2::Clsf() const
{
  return myClsf;
}
//=======================================================================
//function : SetShapeType
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::SetShapeType(const TopAbs_ShapeEnum aType)
{
  myShapeType=aType;
}
//=======================================================================
//function : ShapeType
//purpose  :
//=======================================================================
TopAbs_ShapeEnum GEOMAlgo_FinderShapeOn2::ShapeType()const
{
  return myShapeType;
}
//=======================================================================
//function : SetState
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::SetState(const GEOMAlgo_State aState)
{
  myState=aState;
}
//=======================================================================
//function : State
//purpose  :
//=======================================================================
GEOMAlgo_State GEOMAlgo_FinderShapeOn2::State() const
{
  return myState;
}
//=======================================================================
//function : SetNbPntsMin
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::SetNbPntsMin(const Standard_Integer aNb)
{
  myNbPntsMin=aNb;
}
//=======================================================================
//function : NbPntsMin
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_FinderShapeOn2::NbPntsMin()const
{
  return myNbPntsMin;
}
//=======================================================================
//function : SetNbPntsMax
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::SetNbPntsMax(const Standard_Integer aNb)
{
  myNbPntsMax=aNb;
}
//=======================================================================
//function : NbPntsMax
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_FinderShapeOn2::NbPntsMax()const
{
  return myNbPntsMax;
}
//=======================================================================
// function: MSS
// purpose:
//=======================================================================
  const GEOMAlgo_IndexedDataMapOfShapeState& GEOMAlgo_FinderShapeOn2::MSS() const
{
  return myMSS;
}
//=======================================================================
// function: Shapes
// purpose:
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_FinderShapeOn2::Shapes() const
{
  Standard_Integer i, aNb;
  TopTools_ListOfShape* pL;
  //
  pL=(TopTools_ListOfShape*) &myLS;
  pL->Clear();
  //
  aNb=myMSS.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Shape& aS=myMSS.FindKey(i);
    if (aS.ShapeType()==myShapeType) {
      pL->Append(aS);
    }
  }
  return myLS;
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::Perform()
{
  myErrorStatus=0;
  myWarningStatus=0;
  myLS.Clear();
  myMSS.Clear();
  //
  CheckData();
  if(myErrorStatus) {
    return;
  }
  //
  // Initialize the context
  GEOMAlgo_ShapeAlgo::Perform();
  //
  myClsf->SetTolerance(myTolerance);
  //
  // 1
  ProcessVertices();
  if(myErrorStatus) {
    return;
  }
  if (myShapeType==TopAbs_VERTEX) {
    return;
  }
  //
  // 2
  ProcessEdges();
  if(myErrorStatus) {
    return;
  }
  if (myShapeType==TopAbs_EDGE) {
    return;
  }
  //
  // 3
  ProcessFaces();
  if(myErrorStatus) {
    return;
  }
  if (myShapeType==TopAbs_FACE) {
    return;
  }
  //
  // 4
  ProcessSolids();
  //
}
//=======================================================================
//function : CheckData
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::CheckData()
{
  Standard_Integer iErr;
  //
  myErrorStatus=0;
  //
  if(myClsf.IsNull()) {
    myErrorStatus=10; // myClsf=NULL
    return;
  }
  //
  myClsf->CheckData();
  iErr=myClsf->ErrorStatus();
  if (iErr) {
    myErrorStatus=41; // invalid data for classifier
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
        myShapeType==TopAbs_FACE ||
        myShapeType==TopAbs_SOLID)) {
    myErrorStatus=12; // unallowed sub-shape type
    return;
  }
  //
  if (myState==GEOMAlgo_ST_UNKNOWN ||
      myState==GEOMAlgo_ST_INOUT) {
    myErrorStatus=13; // unallowed state type
    return;
  }
}
//=======================================================================
//function : ProcessVertices
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::ProcessVertices()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsConformState;
  Standard_Integer i, aNb, iErr;
  gp_Pnt aP;
  TopTools_IndexedMapOfShape aM;
  TopAbs_State aSt;
  //
  TopExp::MapShapes(myShape, TopAbs_VERTEX, aM);
  aNb=aM.Extent();
  for (i=1; i<=aNb; ++i) {
    const TopoDS_Vertex& aV=TopoDS::Vertex(aM(i));
    aP=BRep_Tool::Pnt(aV);
    //
    myClsf->SetPnt(aP);
    myClsf->Perform();
    iErr=myClsf->ErrorStatus();
    if (iErr) {
      myErrorStatus=40; // point can not be classified
      return;
    }
    //
    aSt=myClsf->State();
    bIsConformState=GEOMAlgo_SurfaceTools::IsConformState(aSt, myState);
    //
    if (myShapeType==TopAbs_VERTEX){
      if (bIsConformState) {
        myMSS.Add(aV, aSt);
      }
    }
    else if (bIsConformState || aSt==TopAbs_ON) {
      myMSS.Add(aV, aSt);
    }
  }
}
//=======================================================================
//function : ProcessEdges
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::ProcessEdges()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsConformState, bIsToBreak;
  Standard_Integer i, aNb, iCnt, iErr;
  TopAbs_State aSt = TopAbs_UNKNOWN; // todo: aSt must be explicitly initilized to avoid warning (see below)
  TopTools_IndexedMapOfShape aM;
  TopExp_Explorer aExp;
  GEOMAlgo_ListIteratorOfListOfPnt aIt;
  //
  TopExp::MapShapes(myShape, TopAbs_EDGE, aM);
  aNb=aM.Extent();
  for (i=1; i<=aNb; ++i) {
    GEOMAlgo_ListOfPnt aLP;
    GEOMAlgo_StateCollector aSC;
    //
    const TopoDS_Edge& aE=TopoDS::Edge(aM(i));
    //
    bIsConformState=Standard_False;
    //
    aExp.Init(aE, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aV=aExp.Current();
      //
      bIsConformState=myMSS.Contains(aV);
      if (!bIsConformState) {
        break;// vertex has non-conformed state
      }
      else {
        aSt=myMSS.FindFromKey(aV);
        aSC.AppendState(aSt);
      }
    }
    //
    if (!bIsConformState) {
      continue; // vertex has non-conformed state,skip edge
    }
    //
    if (BRep_Tool::Degenerated(aE)) {
      myMSS.Add(aE, aSt); // todo: aSt must be explicitly initilized to avoid warning (see above)
      continue;
    }
    //
    if (myState==GEOMAlgo_ST_ON) {
      Standard_Boolean bCanBeON;
      Standard_Real aT1, aT2;
      Handle(Geom_Curve) aC;
      //
      aC=BRep_Tool::Curve(aE, aT1, aT2);
      bCanBeON=myClsf->CanBeON(aC);
      if(!bCanBeON) {
        continue;
      }
    }
    //
    InnerPoints(aE, aLP);
    if (myErrorStatus) {
      return;
    }
    //
    bIsConformState=Standard_True;
    aIt.Initialize(aLP);
    for (iCnt=0; aIt.More(); aIt.Next(), ++iCnt) {
      if (myNbPntsMax) {
        if (iCnt > myNbPntsMax) {
          break;
        }
      }
      //
      const gp_Pnt& aP=aIt.Value();
      //
      myClsf->SetPnt(aP);
      myClsf->Perform();
      iErr=myClsf->ErrorStatus();
      if (iErr) {
        myErrorStatus=40; // point can not be classified
        return;
      }
      //
      aSt=myClsf->State();
      //
      bIsToBreak=aSC.AppendState(aSt);
      if (bIsToBreak) {
        break;
      }
    }
    //
    aSt=aSC.State();
    //
    bIsConformState=GEOMAlgo_SurfaceTools::IsConformState(aSt, myState);
    if (myShapeType==TopAbs_EDGE) {
      if (bIsConformState) {
        myMSS.Add(aE, aSt);
      }
    }
    else if (bIsConformState || aSt==TopAbs_ON) {
      myMSS.Add(aE, aSt);
    }
  } //  for (i=1; i<=aNb; ++i) next edge
}
//=======================================================================
//function : ProcessFaces
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::ProcessFaces()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsConformState, bIsToBreak, bCanBeON;
  Standard_Integer i, aNbF, iCnt, iErr;
  TopAbs_State aSt;
  TopTools_IndexedMapOfShape aM;
  TopExp_Explorer aExp;
  GEOMAlgo_ListIteratorOfListOfPnt aIt;
  //
  TopExp::MapShapes(myShape, TopAbs_FACE, aM);
  aNbF=aM.Extent();
  for (i=1; i<=aNbF; ++i) {
    GEOMAlgo_StateCollector aSC;
    GEOMAlgo_ListOfPnt aLP;
    //
    const TopoDS_Face& aF=TopoDS::Face(aM(i));
    //
    if (myState==GEOMAlgo_ST_ON) {
      Handle(Geom_Surface) aS;
      //
      aS=BRep_Tool::Surface(aF);
      bCanBeON=myClsf->CanBeON(aS);
      if(!bCanBeON) {
        continue;
      }
    }
    //
    //
    bIsConformState=Standard_False;
    //
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE=aExp.Current();
      bIsConformState=myMSS.Contains(aE);
      if (!bIsConformState) {
        break;// edge has non-conformed state
      }
      else {
        aSt=myMSS.FindFromKey(aE);
        aSC.AppendState(aSt);
      }
    }
    //
    if (!bIsConformState) {
      continue; // edge has non-conformed state,skip face
    }
    //
    InnerPoints(aF, aLP);
    if (myErrorStatus) {
      return;
    }
    //
    bIsConformState=Standard_True;
    aIt.Initialize(aLP);
    for (iCnt=0; aIt.More(); aIt.Next(), ++iCnt) {
      if (myNbPntsMax) {
        if (iCnt > myNbPntsMax) {
          break;
        }
      }
      //
      const gp_Pnt& aP=aIt.Value();
      //
      myClsf->SetPnt(aP);
      myClsf->Perform();
      iErr=myClsf->ErrorStatus();
      if (iErr) {
        myErrorStatus=40; // point can not be classified
        return;
      }
      //
      aSt=myClsf->State();
      //
      bIsToBreak=aSC.AppendState(aSt);
      if (bIsToBreak) {
        break;
      }
    }
    //
    aSt=aSC.State();
    //
    bIsConformState=GEOMAlgo_SurfaceTools::IsConformState(aSt, myState);
    if (myShapeType==TopAbs_FACE) {
      if (bIsConformState) {
        myMSS.Add(aF, aSt);
      }
    }
    else if (bIsConformState || aSt==TopAbs_ON) {
      myMSS.Add(aF, aSt);
    }
  }//  for (i=1; i<=aNb; ++i) next face
}
//=======================================================================
//function : ProcessSolids
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::ProcessSolids()
{
  myErrorStatus=0;
  //
  Standard_Boolean bIsConformState;
  Standard_Integer i, aNbS, j, aNbF;
  TopTools_IndexedMapOfShape aM, aMF;
  TopAbs_State aSt;
  //
  TopExp::MapShapes(myShape, TopAbs_SOLID, aM);
  aNbS=aM.Extent();
  for (i=1; i<=aNbS; ++i) {
    GEOMAlgo_StateCollector aSC;
    //
    const TopoDS_Shape& aSd=aM(i);
    aMF.Clear();
    TopExp::MapShapes(aSd, TopAbs_FACE, aMF);
    //
    bIsConformState=Standard_False;
    //
    aNbF=aMF.Extent();
    for (j=1; j<=aNbF; ++j) {
      const TopoDS_Shape& aF=aMF(j);
      bIsConformState=myMSS.Contains(aF);
      if (!bIsConformState) {
        break;// face has non-conformed state
      }
      else {
        aSt=myMSS.FindFromKey(aF);
        aSC.AppendState(aSt);
      }
    }
    //
    if (!bIsConformState) {
      continue; // face has non-conformed state,skip solid
    }
    //
    aSt=aSC.State();
    //
    bIsConformState=GEOMAlgo_SurfaceTools::IsConformState(aSt, myState);
    if (bIsConformState) {
      myMSS.Add(aSd, aSt);
    }
  }
}
//
//=======================================================================
//function : InnerPoints
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::InnerPoints(const TopoDS_Face& aF,
                                          GEOMAlgo_ListOfPnt& aLP)
{
  Standard_Integer j, j1, j2, k, n[4], aNx, aNb, iCnt;//, aNbLinks, aNbMax, *pIds;
  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aTRF;
  TColStd_MapOfInteger aMBN;
  GEOMAlgo_DataMapOfPassKeyInteger aMPKI;
  GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger aIt;
  gp_Pnt aP, aP1, aP2;
  // 
  myErrorStatus=0;
  //
  aLP.Clear();
  //
  if (!GEOMUtils::MeshShape(aF, /*deflection*/0.001, /*forced*/false,
                            /*angle deflection*/0.349066, /*isRelative*/true,
                            /*doPostCheck*/true)) {
    myWarningStatus=20; // no triangulation found
    return;
  }
  aTRF=BRep_Tool::Triangulation(aF, aLoc);
  //
  const gp_Trsf& aTrsf=aLoc.Transformation();
  //
  // map link/nbtriangles
  j1 = 1;
  j2 = aTRF->NbTriangles();
  for (j=j1; j<=j2; ++j) {
    const Poly_Triangle& aTr = aTRF->Triangle(j);
    aTr.Get(n[0], n[1], n[2]);
    n[3]=n[0];
    for (k=0; k<3; ++k) {
      GEOMAlgo_PassKey aPK;
      //
      aPK.SetIds(n[k], n[k+1]);
      if (aMPKI.IsBound(aPK)) {
        Standard_Integer& iCntX=aMPKI.ChangeFind(aPK);
        ++iCntX;
      }
      else {
        aMPKI.Bind(aPK, 1);
      }
    }
  }
  //
  // boundary nodes aMBN
  aIt.Initialize(aMPKI);
  for (; aIt.More(); aIt.Next()) {
    iCnt=aIt.Value();
    if (iCnt==1) {
      const GEOMAlgo_PassKey& aPK=aIt.Key();
      aNx=(Standard_Integer)aPK.Id(1);
      aMBN.Add(aNx);
      aNx=(Standard_Integer)aPK.Id(2);
      aMBN.Add(aNx);
      
    }
  }
  //
  // inner nodes=all_nodes - boundary_nodes
  j1=1;
  j2=aTRF->NbNodes();
  for (j=j1; j<=j2; ++j) {
    if (!aMBN.Contains(j)) {
      aP=aTRF->Node(j).Transformed(aTrsf);
      aLP.Append(aP);
    }
  }
  //
  aNb=aLP.Extent();
  //
  //modified by NIZNHY-PKV Mon Sep 24 08:42:32 2012f
  if (!aNb && myNbPntsMin) {    // A
    Standard_Boolean bIsDone, bHasFirstPoint, bHasSecondPoint;
    Standard_Integer i, aNb, aIx, /*iErr, */aNbDomains;
    Standard_Real aUMin, aUMax, aVMin, aVMax, dU, aUx, aVx, aV1, aV2;
    gp_Pnt aPx;
    gp_Dir2d aD2D (0., 1.);
    gp_Pnt2d aP2D;
    Handle(Geom2d_Line) aL2D;
    Handle(Geom_Surface) aS;
    TopoDS_Face aFF;
    //
    aFF=aF;
    aFF.Orientation (TopAbs_FORWARD);
    //
    Geom2dHatch_Hatcher& aHatcher=myContext->Hatcher(aFF);
    //
    aS=BRep_Tool::Surface(aFF);
    BRepTools::UVBounds(aFF, aUMin, aUMax, aVMin, aVMax);
    //
    aNb=myNbPntsMin+1;
    dU=(aUMax-aUMin)/aNb;
    for (i=1; i<aNb; ++i) {
      aUx=aUMin+i*dU;
      aP2D.SetCoord(aUx, 0.);
      aL2D=new Geom2d_Line (aP2D, aD2D);
      Geom2dAdaptor_Curve aHCur(aL2D);
      //
      aHatcher.ClrHatchings();
      aIx=aHatcher.AddHatching(aHCur);
      //
      aHatcher.Trim(aIx);
      bIsDone=aHatcher.TrimDone(aIx);
      if (!bIsDone) {
        myErrorStatus=42;
        return;
      }
      //
      aHatcher.ComputeDomains(aIx);
      bIsDone=aHatcher.IsDone(aIx);
      if (!bIsDone) {
        continue;
      }
      //
      aNbDomains=aHatcher.NbDomains(aIx);
      for (j=1; j<=aNbDomains; ++j) {
        const HatchGen_Domain& aDomain=aHatcher.Domain (aIx, j) ; 
        // 1
        bHasFirstPoint=aDomain.HasFirstPoint();
        bHasSecondPoint=aDomain.HasSecondPoint();
        if (!bHasFirstPoint || !bHasSecondPoint) {
          continue;
        }
        // 2
        aV1=aDomain.FirstPoint().Parameter();
        aV2=aDomain.SecondPoint().Parameter();
        aVx=IntTools_Tools::IntermediatePoint(aV1, aV2);
        // 3
        aS->D0(aUx, aVx, aPx);
        aLP.Append(aPx);
        break;
      }
    }// for (i=1; i<aNb; ++i) {
  }// if (!aNb && myNbPntsMin) {
}
//=======================================================================
//function : InnerPoints
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::InnerPoints(const TopoDS_Edge& aE,
                                          GEOMAlgo_ListOfPnt& aLP)
{
  myErrorStatus=0;
  //
  Standard_Integer j, aNbNodes, aIndex, aNb;
  Handle(Poly_PolygonOnTriangulation) aPTE;
  Handle(Poly_Triangulation) aTRE;
  TopLoc_Location aLoc;
  gp_Pnt aP;
  //
  aLP.Clear();
  if (!GEOMUtils::MeshShape(aE, /*deflection*/0.001, /*forced*/false,
                            /*angle deflection*/0.349066, /*isRelative*/true,
                            /*doPostCheck*/true)) {
    myErrorStatus=20; // no triangulation found
    return;
  }
  BRep_Tool::PolygonOnTriangulation(aE, aPTE, aTRE, aLoc);
  if (aTRE.IsNull() || aPTE.IsNull()) {
    Handle(Poly_Polygon3D) aPE = BRep_Tool::Polygon3D(aE, aLoc);
    const gp_Trsf& aTrsf=aLoc.Transformation();
    const TColgp_Array1OfPnt& aNodes=aPE->Nodes();
    //
    aNbNodes=aPE->NbNodes();
    Standard_Integer low = aNodes.Lower(), up = aNodes.Upper();
    for (j=low+1; j<up; ++j) {
      aP=aNodes(j).Transformed(aTrsf);
      aLP.Append(aP);
    }
  }
  else {
    const gp_Trsf& aTrsf=aLoc.Transformation();
    //
    aNbNodes=aPTE->NbNodes();
    const TColStd_Array1OfInteger& aInds=aPTE->Nodes();
    for (j=2; j<aNbNodes; ++j) {
      aIndex=aInds(j);
      aP=aTRE->Node(aIndex).Transformed(aTrsf);
      aLP.Append(aP);
    }
  }
  //
  aNb=aLP.Extent();
  if (!aNb && myNbPntsMin) {
    // try to fill it yourself
    InnerPoints(aE, myNbPntsMin, aLP);
    aNb=aLP.Extent();
  }
}
//=======================================================================
//function : InnerPoints
//purpose  :
//=======================================================================
void GEOMAlgo_FinderShapeOn2::InnerPoints(const TopoDS_Edge& aE,
                                          const Standard_Integer aNbPntsMin,
                                          GEOMAlgo_ListOfPnt& aLP)
{
  // try to fill it yourself
  Standard_Boolean bInf1, bInf2;
  Standard_Integer j, aNbT;
  Standard_Real dT, aT, aT1, aT2;
  gp_Pnt aP;
  Handle(Geom_Curve) aC3D;
  //
  aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  if (aC3D.IsNull()) {
    return;
  }
  //
  bInf1=Precision::IsNegativeInfinite(aT1);
  bInf2=Precision::IsPositiveInfinite(aT2);
  if (bInf1 || bInf2) {
    return;
  }
  //
  aNbT=myNbPntsMin+1;
  dT=(aT2-aT1)/aNbT;
  for (j=1; j<=aNbPntsMin; ++j) {
    aT=aT1+j*dT;
    aC3D->D0(aT, aP);
    aLP.Append(aP);
  }
}

//
// myErrorStatus :
//
// 10 -myClsf=NULL
// 11 -myShape=NULL
// 12 -unallowed type of sub-shapes
// 13 -unallowed state
// 15 -unallowed surface type
// 20- no triangulation found
// 30- can not obtain the line from the link
// 40- point can not be classified
// 41- invalid data for classifier
// 42- can not compute hatching 
