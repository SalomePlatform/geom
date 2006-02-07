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
// See http://www.salome-platform.org/
//
// File:	NMTAlgo_Splitter_1.cxx
// Created:	Mon Feb  2 14:58:54 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTAlgo_Splitter.ixx>

#include <Precision.hxx>

#include <gp_Pnt.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <NMTAlgo_Loop3d.hxx>
//
#include <BOPTools_Tools2D.hxx>
#include <Geom_Curve.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <BOPTools_Tools3D.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>
#include <TopAbs_State.hxx>

//
static 
  void RefineShells(const TopoDS_Shape& ,
		    TopTools_ListOfShape&);
static 
  void RefineSolids(const TopoDS_Shape& ,
		    TopTools_ListOfShape&);

static
  void GetPlanes (const TopoDS_Edge& anEx,
		const TopTools_IndexedDataMapOfShapeListOfShape& anEFMapx,
		const TopoDS_Face& aF1,
		TopAbs_State& aStPF1);

//=======================================================================
//function : ShellsAndSolids
//purpose  : 
//=======================================================================
 void NMTAlgo_Splitter::ShellsAndSolids()
{
  Standard_Boolean bMakeSolids;
  TopAbs_ShapeEnum aType;
  TopTools_ListIteratorOfListOfShape aItS;
  TopTools_ListOfShape aLNS;
  //
  myAddedFacesMap.Clear();
  bMakeSolids=(myLimit==TopAbs_SHAPE || myLimit<TopAbs_SHELL);
  //
  myInternalFaces.Clear(); // remove it after all modifs
  //
  aItS.Initialize(myListShapes);
  for ( ;aItS.More(); aItS.Next()) {
    const TopoDS_Shape& aS=aItS.Value();
    //
    if (myToolShapes.Contains(aS)) {
      continue;
    }
    //
    aType=aS.ShapeType();
    if (aType > TopAbs_SHELL) {
      continue;//face,wire,...
    }
    //
    aLNS.Clear();
    //
    MakeShells (aS, aLNS);
    //
    if (bMakeSolids && aType==TopAbs_SOLID) {
      MakeSolids(aS, aLNS);
    }
    //
    TopTools_ListIteratorOfListOfShape it (aLNS);
    for (; it.More(); it.Next()) {
      myBuilder.Add (myShape, it.Value());
    }
  }
  //
  // add split faces
  aItS.Initialize(myListShapes);
  for ( ;aItS.More(); aItS.Next()) {
    const TopoDS_Shape& aS=aItS.Value();
    //
    aType=aS.ShapeType();
    if (aType!=TopAbs_FACE || myMapTools.Contains(aS)) {
      continue; 
    }
    //
    const TopoDS_Shape& aCSF=myImageShape.Image(aS).First();
    TopoDS_Iterator itS(aCSF);
    for (; itS.More(); itS.Next()){
      const TopoDS_Shape& aF=itS.Value();
      if (!myAddedFacesMap.Contains(aF)){
        myBuilder.Add (myShape, aF);
      }
    }
  }
}
//=======================================================================
//function : MakeShells
//purpose  : split S into compound of shells
//=======================================================================
void NMTAlgo_Splitter::MakeShells(const TopoDS_Shape& aS,
				  TopTools_ListOfShape& aLNS)
{
  NMTAlgo_Loop3d aShellMaker;
  //
  // get compound of split faces of aS
  const TopoDS_Shape& aFC=myImageShape.Image(aS).First();
  aShellMaker.AddConstFaces(aFC);
  //
  // add split faces inside aS
  TopoDS_Shape aIFC;
  if (myClosedShapes.Contains(aS)) {
    //
    // internal faces compound  
    aIFC=FindFacesInside(aS, Standard_True);
    aShellMaker.AddSectionFaces(aIFC);
  }
  //
  aLNS=aShellMaker.MakeShells(myAddedFacesMap);
  //
  RefineShells(aS, aLNS);
  //
  // Add faces added to new shell to myAddedFacesMap:
  // avoid rebuilding twice common part of 2 solids.
  TopTools_ListIteratorOfListOfShape itS(aLNS);
  TopExp_Explorer expF;
  for (; itS.More(); itS.Next()) {
    const TopoDS_Shape& aSh=itS.Value();
    expF.Init (aSh, TopAbs_FACE);
    for (; expF.More(); expF.Next()){
      const TopoDS_Shape& aFx=expF.Current();
      myAddedFacesMap.Add (aFx);
    }
  }
}
//=======================================================================
//function : MakeSolids
//purpose  : make solids out of Shells
//=======================================================================
void NMTAlgo_Splitter::MakeSolids(const TopoDS_Shape&   theSolid,
				  TopTools_ListOfShape& theShellList)
{
  // for a solid wrapping other shells or solids without intersection,
  // it is necessary to find shells making holes in it
  Standard_Boolean isWrapping;
  TopTools_ListOfShape aNewSolids; // result
  TopTools_ListOfShape aHoleShells;
  TopoDS_Shape anInfinitePointShape;
  TopTools_DataMapOfShapeShape aInOutMap;
  TopTools_ListIteratorOfListOfShape aShellIt, aSolisIt;
  //
  isWrapping = myWrappingSolid.Contains(theSolid);
  if (!isWrapping && !theShellList.IsEmpty())  {
    // check if theSolid initially has internal shells
    TopoDS_Iterator aShellExp (theSolid);
    aShellExp.Next();
    isWrapping = aShellExp.More();
  }
  //
  aShellIt.Initialize(theShellList);
  for ( ; aShellIt.More(); aShellIt.Next()) {
    const TopoDS_Shape & aShell = aShellIt.Value();
    // check if a shell is a hole of theSolid
    if (isWrapping && IsInside(anInfinitePointShape, aShell)){
      aHoleShells.Append(aShell);
    }
    else {
      // make a solid from a shell
      TopoDS_Solid Solid;
      myBuilder.MakeSolid( Solid );
      myBuilder.Add (Solid, aShell);
      
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
      if (! IsInside(aHole, aSolid)){
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
      myBuilder.Add (aSolid, aHole);
    }
  }
  //
  theShellList.Clear();
  //
  RefineSolids(theSolid, aNewSolids);
  //
  theShellList.Append(aNewSolids);
}
 
//=======================================================================
//function : FindFacesInside
//purpose  : return compound of faces  of other shapes that are
//           inside <theShape>. 
//           <theShape> is an object shape.
//           <CheckClosed> makes avoid faces that do not form a
//           closed shell
//           <All> makes return already added faces
//=======================================================================
  TopoDS_Shape NMTAlgo_Splitter::FindFacesInside(const TopoDS_Shape& theShape,
						const Standard_Boolean CheckClosed,
						const Standard_Boolean All)
{
  TopExp_Explorer expl;
  TopAbs_State aState;
  //
  // ================================================
  // check if internal faces have been already found
  // ================================================
  if (myInternalFaces.IsBound(theShape)) {
    TopoDS_Shape aIntFComp = myInternalFaces.Find (theShape);
    TopoDS_Shape aIntRemFComp = myIntNotClFaces.Find (theShape);

    expl.Init( aIntRemFComp, TopAbs_FACE);
    if (CheckClosed || !expl.More()){
      return aIntFComp;
    }
    //
    TopoDS_Compound C;
    myBuilder.MakeCompound( C );
    // add removed faces
    for (; expl.More(); expl.Next()){
      myBuilder.Add( C, expl.Current() );
    }
    // add good internal faces
    expl.Init( aIntFComp, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {
      myBuilder.Add( C, expl.Current() );
    }
    //
    return C;
  }

  // ===================================
  // get data for internal faces search
  // ===================================
  //
  // compound of split faces of theShape 
  const TopoDS_Shape& CSF = myImageShape.Image(theShape).First();
  //
  TopTools_MapOfShape MSE, MFP;
  //xf
  TopTools_IndexedMapOfShape  aMFCSF;
  //xt
  TopTools_DataMapOfShapeListOfShape DMSEFP;
  TopTools_MapIteratorOfMapOfShape itm;
  TopTools_ListOfShape EmptyL;
  TopTools_ListIteratorOfListOfShape itl;
  TopTools_IndexedDataMapOfShapeListOfShape DMEF;
  //
  // MSE filling: map of new section edges of CSF
  expl.Init(CSF, TopAbs_EDGE);
  for (; expl.More(); expl.Next()) {
    const TopoDS_Shape& aE = expl.Current() ;
    MSE.Add(aE);
  }
  //
  // DMEF: map edge of CSF - faces of CSF
  TopExp::MapShapesAndAncestors(CSF, TopAbs_EDGE, TopAbs_FACE, DMEF);
  //xf
  TopExp::MapShapes(CSF, TopAbs_FACE, aMFCSF);
  //xt
  //
  // Fill
  // 1.  MFP - a map of faces to process: map of resulting faces except
  // those of theShape; we`ll add to C those of them which are inside CSF
  // 2.  DMSEFP - edge of MSE => faces of MFP
  //
  itl.Initialize(myListShapes);
  for (;itl.More(); itl.Next()) {
    const TopoDS_Shape& aShape = itl.Value();
    //
    if ( theShape.IsSame(aShape)) {
      continue;
    }
    // fill maps
    // iterate on split faces of aShape
    const TopoDS_Shape& CSF1 = myImageShape.Image(aShape).First();
    TopoDS_Iterator itF (CSF1);
    for ( ; itF.More(); itF.Next()) {
      const TopoDS_Shape& aF1 = itF.Value();
      MFP.Add(aF1);
      // iterate on edges of split faces of aShape,
      // add to DMSEFP edges that are new
      expl.Init(aF1, TopAbs_EDGE);
      for (; expl.More(); expl.Next()) {
	TopoDS_Shape aE1 = expl.Current();
	if ( MSE.Contains(aE1)) {// section edge
	  if (!DMSEFP.IsBound(aE1)) {
	    DMSEFP.Bind(aE1, EmptyL);
	  }
	  DMSEFP(aE1).Append(aF1);
	}
      }
    }
  }//for (;itl.More(); itl.Next()) 
  //
  // add tool faces... (is absent)
  //
  // ===========================
  // find faces inside theShape
  // ===========================
  Standard_Boolean sameDom1, sameDom2;
  Standard_Boolean skipAlreadyAdded = Standard_False;
  Standard_Boolean GoodOri, inside;
  Standard_Real dot;
  TopTools_ListOfShape KeepFaces;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape Mapit;

  // iterate on section edges, check faces of other shapes
  // sharing section edges and put internal faces to KeepFaces
  Mapit.Initialize(DMSEFP);
  for (; Mapit.More() ; Mapit.Next()) {
    // a new edge of theShape
    const TopoDS_Edge& E = TopoDS::Edge (Mapit.Key());
    //
    //Standard_Boolean isSectionE=IsSectionEdge(E);//(OrigE);  
    //
    // split faces of other shapes sharing E
    TopTools_ListOfShape& LSF = DMSEFP.ChangeFind(E);
    //
    itl.Initialize( LSF );
    while (itl.More()) {
      // a split faces of other shape
      TopoDS_Face aFace1 = TopoDS::Face(itl.Value());
      // remove aFace1 form DMSEFP and MFP
      LSF.Remove( itl ); // == itl.Next();
      if (!MFP.Remove(aFace1)) {
	continue; // was not is MFP (i.e already checked)
      }      
      //
      // check if aFace1 was already added to 2 shells
      if (!All &&
	  myAddedFacesMap.Contains(aFace1) &&
	  myAddedFacesMap.Contains(aFace1.Reversed())) {
	skipAlreadyAdded = Standard_True;
      }
      //
      //xf 
      if (aMFCSF.Contains(aFace1)) {
	// the face aFace1 can not be inside CSF 
	// if CSF contains the aFace1
	continue;
      }
      //xt
      //
      TopoDS_Shape anOrigFace = aFace1;
      if (myImagesFaces.IsImage(aFace1)){
        anOrigFace = myImagesFaces.Root(aFace1);
      }
      //
      // <- A was here
      //
      // check that anOrigFace is not same domain with CSF faces it intersects
      //
      const TopTools_ListOfShape& FL = DMEF.FindFromKey(E); //faces of CSF sharing E
      //
      const TopoDS_Shape& origF1 = myImagesFaces.IsImage(FL.First()) ?
	myImagesFaces.Root(FL.First()) : FL.First();
      //
      const TopoDS_Shape& origF2 = myImagesFaces.IsImage(FL.Last()) ?
	myImagesFaces.Root(FL.Last()) : FL.Last();
      //
      sameDom1 = anOrigFace.IsSame( origF1 );
      sameDom2 = anOrigFace.IsSame( origF2 );
      //
      if (!(sameDom1 || sameDom2) && HasSameDomainF( TopoDS::Face(anOrigFace) )) {       
	sameDom1 = IsSameDomainF( TopoDS::Face(anOrigFace), TopoDS::Face(origF1));
        if (origF1 == origF2) {
          sameDom2 = sameDom1;
	}
      }
      if (sameDom1 && sameDom2){
	continue;
      }
      //
      if (sameDom1 || sameDom2) {
	inside = NMTAlgo_Loop3d::IsInside (E,
					   TopoDS::Face(FL.First()),
					   TopoDS::Face(FL.Last()),
					   1, dot, GoodOri);
	if (inside || (dot + Precision::Angular() >= 1.0)) {
	  continue; // E is convex between origF1 and origF2 or they are tangent
	}
      }
      //
      GetPlanes(E, DMEF, aFace1, aState);
      if (aState==TopAbs_IN) {
	KeepFaces.Append(aFace1);
      }
    } //while (itl.More()) {
  } //for (; Mapit.More() ; Mapit.Next() )

  // ===================================================
  // add not distributed faces connected with KeepFaces
  // ===================================================

  // ultimate list of internal faces
  TopTools_ListOfShape KeptFaces;
  //
  // add to MFP not split tool faces as well, they may be connected with
  // tool faces interfering with theShape
  /*
  itm.Initialize(myMapTools);
  for (; itm.More(); itm.Next() ) {
    const TopoDS_Shape& aToolFace = itm.Key();
    if (!myImageShape.HasImage(aToolFace)){
      MFP.Add (aToolFace);
    }
  }
  */
  //
  if (MFP.IsEmpty())
    KeptFaces.Append (KeepFaces);
  //
  while (!KeepFaces.IsEmpty()) {
    // KeepEdges : map of edges of faces kept last time
    TopTools_IndexedMapOfShape KeepEdges;
    for ( itl.Initialize(KeepFaces); itl.More(); itl.Next() ) {
      TopExp::MapShapes( itl.Value(), TopAbs_EDGE, KeepEdges);
      KeptFaces.Append( itl.Value() );
    }
    //
    KeepFaces.Clear();
    //
    // keep faces connected with already kept faces by KeepEdges
    for ( itm.Initialize(MFP); itm.More(); itm.Next() ) {
      const TopoDS_Shape& FP = itm.Key();
      for (expl.Init(FP,TopAbs_EDGE); expl.More(); expl.Next()) {
        const TopoDS_Shape& se = expl.Current();
        if (!MSE.Contains(se) && KeepEdges.Contains(se) ) {
          KeepFaces.Append(FP);
          MFP.Remove(FP);
          break;
        }
      }
    }
  }

  // ===============================================================
  // here MFP contains faces outer of theShape and those of shapes
  // which do not interfere with theShape at all and between which
  // there may be those wrapped by theShape and whose faces may be
  // needed to be returned as well
  // ===============================================================

  Standard_Boolean isSolid = (theShape.ShapeType() == TopAbs_SOLID);
  if (All || isSolid)  // All is for sub-result removal
  {
    for ( itm.Initialize( MFP ); itm.More(); itm.Next() ) {
      TopoDS_Shape aFace = itm.Key();

      // find a shape aFace originates from
      TopoDS_Shape anOrigShape = GetOriginalShape( aFace );

      // find out if all faces of anOrigShape are not in MFP
      // and by the way remove them from MFP
      Standard_Boolean isAllOut = Standard_True;
      TopoDS_Shape aSplitFaces = anOrigShape;
      if (myImageShape.HasImage(anOrigShape))
        aSplitFaces = myImageShape.Image(anOrigShape).First();

      TopTools_ListOfShape aSplitFaceL;
      for (expl.Init( aSplitFaces, TopAbs_FACE ); expl.More(); expl.Next())
      {
        const TopoDS_Shape & aSpFace = expl.Current();
        // a tool face which become object has image but the whole tool shape has not
        if (myImageShape.HasImage( aSpFace ))
        {
          TopExp_Explorer exF (myImageShape.Image( aSpFace ).First(), TopAbs_FACE );
          for ( ; exF.More(); exF.Next() )
          {
            aSplitFaceL.Append( exF.Current() );
            if ( ! MFP.Remove( exF.Current() ))
              isAllOut = Standard_False;
          }
        }
        else
        {
          aSplitFaceL.Append( aSpFace );
          if ( ! MFP.Remove( aSpFace ))
            isAllOut = Standard_False;
        }
      }
      itm.Initialize( MFP );
      if ( !isAllOut )
        continue;

      // classify anOrigShape against theShape
      if (IsInside (anOrigShape, theShape)) {
        if (isSolid && myClosedShapes.Contains(anOrigShape)) {
          // to make a special care at solid reconstruction
          myWrappingSolid.Add ( theShape );
	}
        // keep faces of an internal shape anOrigShape
        KeptFaces.Append( aSplitFaceL );
      }
    }
  }

  // ====================================================
  // check if kept faces form a shell without free edges
  // ====================================================

  DMEF.Clear();  // edge - kept faces
  MFP.Clear(); // reuse it for wrong faces
  if (CheckClosed) {
    for (itl.Initialize(KeptFaces); itl.More(); itl.Next() ) 
      TopExp::MapShapesAndAncestors(itl.Value(), TopAbs_EDGE, TopAbs_FACE, DMEF);

    Standard_Integer i, nb = DMEF.Extent();
    Standard_Boolean isClosed = Standard_False;
    while (!isClosed) {
      isClosed = Standard_True;
      for (i=1;  isClosed && i<=nb;  ++i) {
        const TopoDS_Shape& E = DMEF.FindKey( i );
        if (! BRep_Tool::Degenerated( TopoDS::Edge( E )) &&
            ! MSE.Contains( E ))
          isClosed = ( DMEF(i).Extent() != 1 );
      }
      if (!isClosed) {
        const TopoDS_Shape& F = DMEF.FindFromIndex( i-1 ).First(); // bad face
        MFP.Add( F ); 
        // remove bad face from DMEF
        for (expl.Init( F, TopAbs_EDGE); expl.More(); expl.Next()) {
	  const TopoDS_Shape& E = expl.Current();
          TopTools_ListOfShape& FL = DMEF.ChangeFromKey( E );
          for (itl.Initialize( FL ); itl.More(); itl.Next() ) {
            if ( F.IsSame( itl.Value() )) {
              FL.Remove( itl );
              break;
            }
          }
        }
      }
    }
  }

  // ==============
  // make a result
  // ==============

  TopoDS_Compound C;
  // compound of removed internal faces
  TopoDS_Compound CNotCl;

  myBuilder.MakeCompound(C);
  myBuilder.MakeCompound(CNotCl);

  // add to compounds
  itl.Initialize(KeptFaces);
  for (; itl.More(); itl.Next() ) {
    TopoDS_Shape & aIntFace = itl.Value();
    //
    if (!All &&
	myAddedFacesMap.Contains(aIntFace) &&
	myAddedFacesMap.Contains(aIntFace.Reversed())) {
      continue;
    }
    //
    if (! MFP.Contains( aIntFace )){
      myBuilder.Add(C, aIntFace);
    }
    else{
      myBuilder.Add(CNotCl, aIntFace);
    }
  }
  //
  if (!skipAlreadyAdded && CheckClosed) {
    myInternalFaces.Bind(theShape, C);
    myIntNotClFaces.Bind(theShape, CNotCl);
  }
  //
  //
  if (!myMapSIFC.IsBound(theShape)) {
    TopoDS_Compound aCIF;
    myBuilder.MakeCompound(aCIF);
    //
    itl.Initialize(KeptFaces);
    for (; itl.More(); itl.Next() ) {
      TopoDS_Shape & aIntFace = itl.Value();
      if (! MFP.Contains(aIntFace )){
	myBuilder.Add(aCIF, aIntFace);
      }
    }
    myMapSIFC.Bind(theShape, aCIF);
  }
  //
  return C;
}
//=======================================================================
//function : IsInside
//purpose  : Return True if the first vertex of S1 inside S2.
//           If S1.IsNull(), check infinite point against S2.
//=======================================================================
Standard_Boolean NMTAlgo_Splitter::IsInside (const TopoDS_Shape& theS1,
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
//function : GetOriginalShape
//purpose  : Return the  shape  aShape  originates from. aShape
//           should be a face or more complex result shape
//=======================================================================
TopoDS_Shape NMTAlgo_Splitter::GetOriginalShape(const TopoDS_Shape& theShape) const
{
  TopoDS_Shape anOrigShape;

  TopExp_Explorer expl( theShape, TopAbs_FACE);
  if (expl.More()) {
    TopoDS_Shape aFace = expl.Current();
    if (myImagesFaces.IsImage( aFace ))
      aFace = myImagesFaces.Root( aFace );
    anOrigShape = myFaceShapeMap.Find( aFace );
  }
  return anOrigShape;
}
//=======================================================================
//function :RefineShells 
//purpose  : 
//=======================================================================
void RefineShells(const TopoDS_Shape& aS,
		  TopTools_ListOfShape& aLNS)
{
  Standard_Boolean bFound;
  Standard_Integer iS, jS, aNbSOrs, aNbSIms, aNbFOrs, aNbFIms, kFOrs, aNb;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_IndexedMapOfShape aMSOrs, aMSIms, aMFOrs, aMFIms;
  TopTools_IndexedDataMapOfShapeShape aMImOr;
  TopTools_ListOfShape aLS;
  //
  TopExp::MapShapes(aS, TopAbs_SHELL, aMSOrs);
  aIt.Initialize(aLNS);
  for (;aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSh=aIt.Value();
    aMSIms.Add(aSh);
  }
  //
  aNbSOrs=aMSOrs.Extent();
  aNbSIms=aMSIms.Extent();
  //
  for (iS=1; iS<=aNbSOrs; ++iS) {
    const TopoDS_Shape& aSOr=aMSOrs(iS);
    aMFOrs.Clear();
    TopExp::MapShapes(aSOr, TopAbs_FACE, aMFOrs);
    aNbFOrs=aMFOrs.Extent();
    //
    for (jS=1; jS<=aNbSIms; ++jS) {
      const TopoDS_Shape& aSIm=aMSIms(jS);
      if (aMImOr.Contains(aSIm)) {
	continue;
      }
      //
      aMFIms.Clear();
      TopExp::MapShapes(aSIm, TopAbs_FACE, aMFIms);
      aNbFIms=aMFIms.Extent();
      //
      if (aNbFIms==aNbFOrs) {
	bFound=Standard_True;
	for (kFOrs=1; kFOrs<=aNbFOrs; ++kFOrs) {
	  const TopoDS_Shape& aFOr=aMFOrs(kFOrs);
	  if (!aMFIms.Contains(aFOr)) {
	    bFound=Standard_False;
	    break; //next aSIm
	  }
	}
	if (bFound){
	  aMImOr.Add(aSIm, aSOr);
	  break; //next aSOr
	}
      } //if (aNbFIms==aNbFOrs)
    }
  }
  //
  aNb=aMImOr.Extent();
  aIt.Initialize(aLNS);
  for (;aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSh=aIt.Value();
    if (aMImOr.Contains(aSh)) {
      const TopoDS_Shape& aSOr=aMImOr.FindFromKey(aSh);
      aLS.Append(aSOr);
    }
    else {
      aLS.Append(aSh);
    }
  }
  //
  aLNS.Clear();
  aLNS.Append(aLS);
}

//=======================================================================
//function :RefineSolids 
//purpose  : 
//=======================================================================
void RefineSolids(const TopoDS_Shape& aSolidOr,
		  TopTools_ListOfShape& aLNS)
{
  Standard_Integer aNb, iS,  aNbSOrs, aNbSIms;
  TopoDS_Shape aSolidIm;
  TopTools_IndexedMapOfShape aMSOrs, aMSIms;
  //
  aNb=aLNS.Extent();
  if (aNb!=1) {
    return;
  }
  //
  aSolidIm=aLNS.First();
  
  TopExp::MapShapes(aSolidOr, TopAbs_SHELL, aMSOrs);
  TopExp::MapShapes(aSolidIm, TopAbs_SHELL, aMSIms);
  aNbSOrs=aMSOrs.Extent();
  aNbSIms=aMSIms.Extent();
  if (aNbSOrs!=aNbSIms) {
    return;
  }
  //
  for (iS=1; iS<=aNbSOrs; ++iS) {
    const TopoDS_Shape& aSOr=aMSOrs(iS);
    if (!aMSIms.Contains(aSOr)) {
      return;
    }
  }
  //
  aLNS.Clear();
  aLNS.Append(aSolidOr);
}
//=======================================================================
//function : GetPlanes
//purpose  :
//=======================================================================
void GetPlanes (const TopoDS_Edge& anEx,
		const TopTools_IndexedDataMapOfShapeListOfShape& anEFMapx,
		const TopoDS_Face& aF1,
		TopAbs_State& aStPF1)
		
{
  Standard_Boolean bIsAdjExists;
  Standard_Real aT, aT1, aT2;
  TopAbs_Orientation anOrEx, anOr;
  gp_Dir aDNFx1, aDNFx2, aDNF1; 
  gp_Pnt aPx, aPx1, aPx2, aPF1;
  TopoDS_Edge aERight, aSpxSimm;
  TopoDS_Face aFx1, aFx2, aFF1;
  TopTools_ListIteratorOfListOfShape anIt;
  //
  // Point on Edge
  Handle(Geom_Curve)aC3D =BRep_Tool::Curve(anEx, aT1, aT2);
  aT=BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
  
  aC3D->D0(aT, aPx);
  //
  anOrEx=anEx.Orientation();
  
  aSpxSimm=anEx;
  if (anOrEx==TopAbs_FORWARD) {
    aSpxSimm.Orientation(TopAbs_REVERSED);
  }
  else if (anOrEx==TopAbs_REVERSED){
    aSpxSimm.Orientation(TopAbs_FORWARD);
  }
  //
  const TopTools_ListOfShape& aLF=anEFMapx.FindFromKey(anEx);
  anIt.Initialize(aLF);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aFE=anIt.Value();
    aFx1=TopoDS::Face(aFE);
    anOr=BOPTools_Tools3D::Orientation(anEx, aFx1);
    if (anOr==anOrEx){
      break;
    }
  }
  //
  BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (anEx, aFx1, aT, aPx1, aDNFx1);
  //
  bIsAdjExists=BOPTools_Tools3D::GetAdjacentFace (aFx1, anEx, anEFMapx, aFx2);
  if (!bIsAdjExists) {
    BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (aSpxSimm, aFx1, aT, aPx2, aDNFx2); 
  }
  else {
    BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (aSpxSimm, aFx2, aT, aPx2, aDNFx2);
  }
  //
  aFF1=aF1;
  aFF1.Orientation(TopAbs_FORWARD);
  BOPTools_Tools3D::OrientEdgeOnFace (anEx, aFF1, aERight);
  BOPTools_Tools3D::GetApproxNormalToFaceOnEdge (aERight, aFF1, aT, aPF1, aDNF1);
  //
  {
    Standard_Real d12, d1, anAlfa12, anAlfa1, aTwoPI;
    
    aTwoPI=Standard_PI+Standard_PI;
    
    gp_Vec aVx1(aPx, aPx1);
    gp_Dir aDBx1 (aVx1);
    gp_Pln aPlnToCompare (aPx, aDNFx1);
    
    gp_Vec aVx2(aPx, aPx2);
    gp_Dir aDBx2 (aVx2);
    
    anAlfa12=aDBx1.Angle(aDBx2);
    d12=BOPTools_Tools3D::SignDistance(aPx2, aPlnToCompare);
    if (d12 < 0.) {
      anAlfa12=aTwoPI-anAlfa12;
    }
    
    gp_Vec aVF1(aPx, aPF1);
    gp_Dir aDBF1 (aVF1);
    anAlfa1=aDBx1.Angle(aDBF1);
    d1=BOPTools_Tools3D::SignDistance(aPF1, aPlnToCompare);
    if (d1 < 0.) {
      anAlfa1=aTwoPI-anAlfa1;
    }
    
    aStPF1=TopAbs_OUT;
    if (anAlfa1 > anAlfa12) {
      aStPF1=TopAbs_IN;
    }
  }
}

//modified by NIZNHY-PKV Fri Feb 25 17:00:03 2005t XX 
/*
	 A
      //
      TopoDS_Shape aFace2;
      if ( !isSectionE ) {
        while (itl.More()) {
          aFace2 = itl.Value();
	  //
	   TopoDS_Shape anOrigFace2 = aFace2;
	  if (myImagesFaces.IsImage(aFace2)) {
	    anOrigFace2 = myImagesFaces.Root(aFace2);
	  }
	  //
          if (!MFP.Contains( aFace2 )) {
            LSF.Remove( itl );
            continue;
          }
          //if (anOrigFace.IsSame( myImagesFaces.Root( aFace2 )))
          if (anOrigFace.IsSame(anOrigFace2)) {
            break;
	  }
          itl.Next();
        }
        if (itl.More()) { // aFace2 found, remove it from maps
          LSF.Remove( itl );
          MFP.Remove(aFace2);
        }
        else{
          aFace2.Nullify();
	}
        itl.Initialize( LSF );
      } 
      */
