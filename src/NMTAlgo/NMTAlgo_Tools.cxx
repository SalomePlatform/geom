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
// File:	NMTAlgo_Tools.cxx
// Created:	Fri Jan 30 16:30:45 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Tools.ixx>

#include <gp_Dir.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BOPTools_Tools3D.hxx>

#include <NMTAlgo_Loop3d.hxx>
#include <TopoDS_Iterator.hxx>


//=======================================================================
// function: Sense
// purpose: 
//=======================================================================
  Standard_Integer NMTAlgo_Tools::Sense (const TopoDS_Face& aF1,
					 const TopoDS_Face& aF2)
{
  Standard_Integer iSense=0;
  gp_Dir aDNF1, aDNF2;
  TopoDS_Edge aE1, aE2;
  TopExp_Explorer anExp;
  //
  anExp.Init(aF1, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    aE1=TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(aE1)) {
      if (!BRep_Tool::IsClosed(aE1, aF1)) {
	break;
      }
    }
  }
  //
  anExp.Init(aF2, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    aE2=TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(aE2)) {
      if (!BRep_Tool::IsClosed(aE2, aF2)) {
	if (aE2.IsSame(aE1)) {
	  iSense=1;
	  break;
	}
      }
    }
  }
  //
  if (!iSense) {
    return iSense;
  }
  //
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE1, aF1, aDNF1);
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE2, aF2, aDNF2);
  //
  iSense=BOPTools_Tools3D::SenseFlag(aDNF1, aDNF2);
  //
  return iSense;
}

//=======================================================================
// function: OrientFacesOnShell
// purpose: 
//=======================================================================
  void NMTAlgo_Tools::OrientFacesOnShell (const TopoDS_Face& aF1, 
					  TopoDS_Shell& aSh)
{
  Standard_Integer aNbFSh, iSenseFlag;
  gp_Dir aDNF1, aDNF2;
  TopExp_Explorer anExp;
  TopoDS_Edge aE1, aESh;
  TopTools_IndexedDataMapOfShapeListOfShape aMEFSh;
  //
  anExp.Init(aF1, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    aE1=TopoDS::Edge(anExp.Current());
    if (!BRep_Tool::Degenerated(aE1)) {
      break;
    }
  }
  //
  BOPTools_Tools3D::GetNormalToFaceOnEdge(aE1, aF1, aDNF1);
  //
  TopExp::MapShapesAndAncestors(aSh, TopAbs_EDGE, TopAbs_FACE, aMEFSh);
  if (aMEFSh.Contains(aE1)) {
    const TopTools_ListOfShape& aLFSh=aMEFSh.FindFromKey(aE1);
    aNbFSh=aLFSh.Extent();
    if (aNbFSh==1) {
      const TopoDS_Face& aFSh=TopoDS::Face(aLFSh.First());
      //
      BOPTools_Tools3D::OrientEdgeOnFace(aE1, aFSh, aESh); 
      BOPTools_Tools3D::GetNormalToFaceOnEdge(aESh, aFSh, aDNF2);
      //
      iSenseFlag=BOPTools_Tools3D::SenseFlag(aDNF1, aDNF2);
      if (iSenseFlag==1) {
	return;
      }
      //
      anExp.Init(aSh, TopAbs_FACE);
      for (; anExp.More(); anExp.Next()) {
	const TopoDS_Shape& aFx=anExp.Current();
	TopoDS_Shape *pFx=(TopoDS_Shape *)&aFx;
	pFx->Reverse();
      }
    }
  }
}
//=======================================================================
// function: OrientFacesOnShell
// purpose: 
//=======================================================================
  void NMTAlgo_Tools::OrientFacesOnShell (const TopoDS_Shell& aShell, 
					  TopoDS_Shell& aShellNew)
{
  Standard_Boolean bIsProcessed1, bIsProcessed2;
  Standard_Integer i, aNbE, aNbF, j;
  TopAbs_Orientation anOrE1, anOrE2;

  TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
  TopTools_IndexedMapOfShape aProcessedFaces;
  BRep_Builder aBB;

  aBB.MakeShell(aShellNew);
  
  TopExp::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  
  aNbE=aEFMap.Extent();
  // 
  // One seam edge  in aEFMap contains  2 equivalent faces.
  for (i=1; i<=aNbE; i++) {
    TopTools_ListOfShape& aLF=aEFMap.ChangeFromIndex(i);

    if (aLF.Extent()>1) {
      TopTools_ListOfShape aLFTmp;
      TopTools_IndexedMapOfShape aFM;

      TopTools_ListIteratorOfListOfShape anIt(aLF);
      for (; anIt.More(); anIt.Next()) {
	const TopoDS_Shape& aF=anIt.Value();
	if (!aFM.Contains(aF)) {
	  aFM.Add(aF);
	  aLFTmp.Append(aF);
	}
      }
      aLF.Clear();
      aLF=aLFTmp;
    }
  }
  //
  // Do
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEFMap.FindKey(i));

    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }

    const TopTools_ListOfShape& aLF=aEFMap.FindFromIndex(i);
    aNbF=aLF.Extent();
    
    if (aNbF==2) {
      TopoDS_Face& aF1=TopoDS::Face(aLF.First());
      TopoDS_Face& aF2=TopoDS::Face(aLF.Last() );
      
      
      bIsProcessed1=aProcessedFaces.Contains(aF1);
      bIsProcessed2=aProcessedFaces.Contains(aF2);
     
      if (bIsProcessed1 && bIsProcessed2) {
	continue;
      }

      if (!bIsProcessed1 && !bIsProcessed2) {
	aProcessedFaces.Add(aF1);
	aBB.Add(aShellNew, aF1);

	bIsProcessed1=!bIsProcessed1;
      }

      //
      TopoDS_Face aF1x, aF2x;
      
      aF1x=aF1;
      if (bIsProcessed1) {
	j=aProcessedFaces.FindIndex(aF1);
	aF1x=TopoDS::Face(aProcessedFaces.FindKey(j));
      }
      
      aF2x=aF2;
      if (bIsProcessed2) {
	j=aProcessedFaces.FindIndex(aF2);
	aF2x=TopoDS::Face(aProcessedFaces.FindKey(j));
      }
      //

      anOrE1=NMTAlgo_Tools::Orientation(aE, aF1x); 
      anOrE2=NMTAlgo_Tools::Orientation(aE, aF2x);

      if (bIsProcessed1 && !bIsProcessed2) {
	
	if (anOrE1==anOrE2) {
	  if (!BRep_Tool::IsClosed(aE, aF1) &&
	      !BRep_Tool::IsClosed(aE, aF2)) {
	    aF2.Reverse();
	  }
	}
	aProcessedFaces.Add(aF2);
	aBB.Add(aShellNew, aF2);
      }
      
      else if (!bIsProcessed1 && bIsProcessed2) {
	if (anOrE1==anOrE2) {
	  if (!BRep_Tool::IsClosed(aE, aF1) &&
	      !BRep_Tool::IsClosed(aE, aF2)) {
	    aF1.Reverse();
	  }
	}
	aProcessedFaces.Add(aF1);
	aBB.Add(aShellNew, aF1);
      }
    }
  }
  //
  //
  for (i=1; i<=aNbE; i++) {
    const TopoDS_Edge& aE=TopoDS::Edge(aEFMap.FindKey(i));

    if (BRep_Tool::Degenerated(aE)) {
      continue;
    }

    const TopTools_ListOfShape& aLF=aEFMap.FindFromIndex(i);
    aNbF=aLF.Extent();
    if (aNbF!=2) {
      TopTools_ListIteratorOfListOfShape anIt(aLF);
      for(; anIt.More(); anIt.Next()) {
	const TopoDS_Face& aF=TopoDS::Face(anIt.Value());
	if (!aProcessedFaces.Contains(aF)) {
	  aProcessedFaces.Add(aF);
	  aBB.Add(aShellNew, aF);
	}
      }
    }
  }
}
//=======================================================================
//function : Orientation
//purpose  :
//=======================================================================
  TopAbs_Orientation NMTAlgo_Tools::Orientation(const TopoDS_Edge& anE,
						const TopoDS_Face& aF)
{
  TopAbs_Orientation anOr=TopAbs_INTERNAL;

  TopExp_Explorer anExp;
  anExp.Init(aF, TopAbs_EDGE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEF1=TopoDS::Edge(anExp.Current());
    if (anEF1.IsSame(anE)) {
      anOr=anEF1.Orientation();
      break;
    }
  }
  return anOr;
}
//=======================================================================
//function : IsInside
//purpose  : Return True if the first vertex of S1 inside S2.
//           If S1.IsNull(), check infinite point against S2.
//=======================================================================
  Standard_Boolean NMTAlgo_Tools::IsInside (const TopoDS_Shape& theS1,
					    const TopoDS_Shape& theS2)
{
  BRepClass3d_SolidClassifier aClassifier( theS2 );
  //
  TopExp_Explorer expl(theS1, TopAbs_VERTEX);
  //
  if (!expl.More()){
    aClassifier.PerformInfinitePoint( ::RealSmall());
  }
  else  {
    const TopoDS_Vertex & aVertex = TopoDS::Vertex( expl.Current() );
    aClassifier.Perform (BRep_Tool::Pnt( aVertex ),
                         BRep_Tool::Tolerance( aVertex ));
  }
  //
  return ( aClassifier.State() == TopAbs_IN );
}
//=======================================================================
//function : MakeShells
//purpose  : 
//=======================================================================
  void NMTAlgo_Tools::MakeShells (const TopoDS_Shape& aFC,
				  TopTools_ListOfShape& aLNS)
{
  NMTAlgo_Loop3d aShellMaker;
  TopTools_MapOfOrientedShape aMTmp;
  //
  aShellMaker.AddConstFaces(aFC);
  aLNS=aShellMaker.MakeShells(aMTmp);
}
//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================
  void NMTAlgo_Tools::MakeSolids(const TopoDS_Shape& aFC,
				 TopTools_ListOfShape& theShellList)
{
  NMTAlgo_Tools::MakeShells(aFC, theShellList);
  NMTAlgo_Tools::MakeSolids(theShellList);
}
//=======================================================================
//function : MakeSolids
//purpose  : 
//=======================================================================
  void NMTAlgo_Tools::MakeSolids(TopTools_ListOfShape& theShellList)
{
  TopTools_ListOfShape aHoleShells, aNewSolids; 
  TopoDS_Shape anInfinitePointShape;
  TopTools_DataMapOfShapeShape aInOutMap;
  TopTools_ListIteratorOfListOfShape aShellIt, aSolisIt;
  BRep_Builder aBB;
  //
  aShellIt.Initialize(theShellList);
  for ( ; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape & aShell = aShellIt.Value();
    // check if a shell is a hole of theSolid
    if (NMTAlgo_Tools::IsInside(anInfinitePointShape, aShell)){
      aHoleShells.Append(aShell);
    }
    else {
      // make a solid from a shell
      TopoDS_Solid Solid;
      aBB.MakeSolid( Solid );
      aBB.Add (Solid, aShell);
      aNewSolids.Append (Solid);
    }
  }
  //
  // find outer a shell most close to each hole shell
  aShellIt.Initialize(aHoleShells);
  for (; aShellIt.More(); aShellIt.Next()){
    const TopoDS_Shape & aHole = aShellIt.Value();
    //
    aSolisIt.Initialize(aNewSolids);
    for ( ; aSolisIt.More(); aSolisIt.Next())    {
      const TopoDS_Shape & aSolid = aSolisIt.Value();
      //
      if (! NMTAlgo_Tools::IsInside(aHole, aSolid)){
        continue;
      }
      //
      if ( aInOutMap.IsBound (aHole)){
        const TopoDS_Shape & aSolid2 = aInOutMap( aHole );
        if ( IsInside( aSolid, aSolid2 )) {
          aInOutMap.UnBind( aHole );
          aInOutMap.Bind ( aHole, aSolid );
        }
      }
      else{
        aInOutMap.Bind (aHole, aSolid);
      }
    }
    //
    // add aHole to a solid
    if (aInOutMap.IsBound( aHole )){
      TopoDS_Shape & aSolid=aInOutMap(aHole);
      aBB.Add (aSolid, aHole);
    }
  }
  theShellList.Clear();
  theShellList.Append( aNewSolids );
}
//=======================================================================
//function : BreakWebs
//purpose  :
//=======================================================================
  void NMTAlgo_Tools::BreakWebs(const TopoDS_Shape& aCS,
				TopoDS_Shape& aCSR)
{
  Standard_Integer i, aNbF, aNbS;
  TopTools_IndexedDataMapOfShapeListOfShape aMFS;
  TopTools_MapOfShape aMF;
  
  TopoDS_Compound aFC, aCR;
  BRep_Builder aBB;
  //
  aBB.MakeCompound(aFC);
  TopExp::MapShapesAndAncestors(aCS, TopAbs_FACE, TopAbs_SOLID, aMFS);
  //
  aNbF=aMFS.Extent();
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aF=aMFS.FindKey(i);
    const TopTools_ListOfShape& aLS=aMFS(i);
    aNbS=aLS.Extent();
    if (aNbS==2) {
      aMF.Add(aF);
    }
    else {
      aBB.Add(aFC, aF);
    }
  }
  //
  if (!aMF.Extent()) {
    aCSR=aCS;
    return;
  }
  //
  TopTools_ListOfShape aLNS;
  TopTools_ListIteratorOfListOfShape aItS;
  //
  NMTAlgo_Tools::MakeShells(aFC, aLNS);
  NMTAlgo_Tools::MakeSolids(aLNS);
  //
  aBB.MakeCompound(aCR);
  aItS.Initialize(aLNS);
  for ( ;aItS.More(); aItS.Next()) {
    const TopoDS_Shape& aS=aItS.Value();
    aBB.Add(aCR, aS);
  }
  aCSR=aCR;
}
//=======================================================================
//function : FindImageSolid
//purpose  :
//=======================================================================
  Standard_Boolean NMTAlgo_Tools::FindImageSolid(const TopoDS_Shape& aFC,
						 const TopTools_IndexedMapOfShape& aMSo,
						 TopoDS_Shape& aSox)
{
  Standard_Boolean bFound=Standard_False;
  Standard_Integer i, j, aNbSo, aNbF, aNbFSo;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aMFC, aMFSo;
  //
  aIt.Initialize(aFC);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    aMFC.Add(aF);
  }
  //
  aNbF=aMFC.Extent();
  aNbSo=aMSo.Extent();
  for (i=1; i<=aNbSo; ++i) {
    const TopoDS_Shape& aSo=aMSo(i);
    //
    bFound=Standard_True;
    aMFSo.Clear();
    TopExp::MapShapes(aSo, TopAbs_FACE, aMFSo);
    //
    aNbFSo=aMFSo.Extent();
    if (aNbFSo!=aNbF) {
      bFound=Standard_False;
      continue;
    }
    //
    for (j=1; j<=aNbFSo; ++j) {
      const TopoDS_Shape& aFSo=aMFSo(j);
      if (!aMFC.Contains(aFSo)) {
	bFound=Standard_False;
	break;
      }
    }
    if (bFound) {
      aSox=aSo;
      return bFound;
    }
  }
  return bFound;
}
