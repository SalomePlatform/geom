// File:	NMTAlgo_Splitter_2.cxx
// Created:	Mon Feb  9 15:07:51 2004
// Author:	Igor FEOKTISTOV
//		<ifv@philipox.nnov.matra-dtv.fr>


#include <NMTAlgo_Splitter.ixx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <NMTTools_DSFiller.hxx>
#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_PaveFiller.hxx>
#include <BOPTools_PInterferencePool.hxx>
#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfEEInterference.hxx>
#include <BOPTools_EEInterference.hxx>
#include <BOPTools_CArray1OfESInterference.hxx>
#include <BOPTools_ESInterference.hxx>

//=======================================================================
//function : KeepShapesInside
//purpose  : remove shapes that are outside of S from result
//=======================================================================
  void NMTAlgo_Splitter::KeepShapesInside (const TopoDS_Shape& S)
{
  TopoDS_Iterator it;
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    for (it.Initialize( S ); it.More(); it.Next())
      KeepShapesInside( it.Value());
    return;
  }

  Standard_Boolean isTool = Standard_False;
  if (!myImageShape.HasImage( S )) {
    //isTool = CheckTool( S );
    //if (!isTool) return;
    return;
  }

  // build map of internal faces
  TopTools_IndexedMapOfShape MIF;
  TopoDS_Shape IntFacesComp = FindFacesInside( S, Standard_False, Standard_True);
  TopExp::MapShapes( IntFacesComp, TopAbs_FACE, MIF );

  TopoDS_Compound C;
  myBuilder.MakeCompound(C);

  TopAbs_ShapeEnum anInternalShapeType = TopAbs_SHAPE;
  if (!MIF.IsEmpty())
  {
    // leave in the result only those shapes having a face in MIF
    for (it.Initialize( myShape ); it.More(); it.Next()) {
      const TopoDS_Shape & aResShape = it.Value();
      TopExp_Explorer expResF( aResShape, TopAbs_FACE );
      for (; expResF.More(); expResF.Next()) {
        if ( MIF.Contains( expResF.Current())) {
          myBuilder.Add( C, aResShape );
          if (aResShape.ShapeType() < anInternalShapeType)
            anInternalShapeType = aResShape.ShapeType();
          break;
        }
      }
    }
  }

  // may be S was not split by internal faces then it is missing
  // in myShape, add it
  if (!isTool &&
      (anInternalShapeType > TopAbs_SOLID || S.ShapeType() > TopAbs_SOLID))
  {
    TopTools_IndexedMapOfShape MSF; // map of split faces of S
    TopExp::MapShapes( myImageShape.Image(S).First(), TopAbs_FACE, MSF);

    // find a shape having all faces in MSF
    for (it.Initialize( myShape ); it.More(); it.Next()) {
      TopExp_Explorer expResF( it.Value(), TopAbs_FACE );
      for (; expResF.More(); expResF.Next()) {
        if (! MSF.Contains( expResF.Current())) 
          break;
      }
      if (! expResF.More()) {
        myBuilder.Add( C, it.Value() );
        break;
      }
    }
  }

  myShape = C;
}

//=======================================================================
//function : RemoveShapesInside
//purpose  : remove shapes that are inside S from result
//=======================================================================
  void NMTAlgo_Splitter::RemoveShapesInside (const TopoDS_Shape& S)
{
  TopoDS_Iterator it;
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    it.Initialize( S );
    for (; it.More(); it.Next()) {
      RemoveShapesInside( it.Value());
    }
    return;
  }
  //
  Standard_Boolean isTool;
  TopoDS_Shape IntFacesComp;
  TopoDS_Compound C;
  TopTools_IndexedMapOfShape MIF; // map of internal faces
  TopTools_MapOfShape RFM;
  TopTools_MapIteratorOfMapOfShape itF;
  //
  //modified by NIZNHY-PKV Wed Dec 22 18:56:27 2004 f
  isTool=myToolShapes.Contains(S);
  //modified by NIZNHY-PKV Wed Dec 22 18:56:31 2004 t
  //
  //isTool = Standard_False;
  if (!myImageShape.HasImage( S )) {
    return;
  }
  //
  IntFacesComp = FindFacesInside( S, Standard_False, Standard_True);
  //
  TopExp::MapShapes( IntFacesComp, TopAbs_FACE, MIF);
  if (MIF.IsEmpty()) {
    return;
  }
  // add to MIF split faces of S
  const TopoDS_Shape& aSIm=myImageShape.Image(S).First();
  TopExp::MapShapes(aSIm, TopAbs_FACE, MIF);
  //
  // leave in the result only those shapes not having all face in MIF
  myBuilder.MakeCompound(C);
  //
  // RFM : faces of removed shapes that encounter once
  it.Initialize(myShape);
  for (; it.More(); it.Next()) {
    TopExp_Explorer expResF;
    //
    const TopoDS_Shape& aSR=it.Value();
    //
    expResF.Init(aSR, TopAbs_FACE);
    for (; expResF.More(); expResF.Next()) {
      const TopoDS_Shape& aFR=expResF.Current();
      if (!MIF.Contains(aFR)) {
	break;
      }
    }
    //
    if (expResF.More()) {
      // add shape to result
      myBuilder.Add(C, aSR);
    }
    else {
      // add faces of a removed shape to RFM
      for (expResF.ReInit(); expResF.More(); expResF.Next()) {
	const TopoDS_Shape& aF = expResF.Current();
	if (!RFM.Remove(aF)) {
	  RFM.Add(aF);
	}
      }
    }
  }// for (; it.More(); it.Next())
  //
  //modified by NIZNHY-PKV Wed Dec 22 18:59:46 2004 f
  TopoDS_Compound aCx;
  //
  myBuilder.MakeCompound(aCx);
  itF.Initialize (RFM);
  for (; itF.More(); itF.Next()) {
    const TopoDS_Shape& aF=itF.Key();
    myBuilder.Add(aCx, aF);
  }
  //modified by NIZNHY-PKV Wed Dec 22 18:59:48 2004 t
  //
  if (!isTool) {
    // rebuild S, it must remain in the result
    Standard_Boolean isClosed = Standard_False;
    switch (S.ShapeType()) {
    case TopAbs_SOLID :
      isClosed = Standard_True; break;
    case TopAbs_SHELL: {
      TopTools_IndexedDataMapOfShapeListOfShape MEF;
      TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, MEF);
      Standard_Integer i;
      for (i=1;  isClosed && i<=MEF.Extent();  ++i) {
        isClosed = ( MEF(i).Extent() != 1 );
      }
      break;
    }
    default:
      isClosed = Standard_False;
    }
    //
    if (isClosed) {
      // add to a new shape external faces of removed shapes, ie those in RFM
      TopoDS_Shell Shell;
      myBuilder.MakeShell(Shell);
      // exclude redundant internal face with edges encounterd only once
      TopTools_IndexedDataMapOfShapeListOfShape MEF;
      //
      itF.Initialize (RFM);
      for ( ; itF.More(); itF.Next()) {
	const TopoDS_Shape& aF=itF.Key();
        TopExp::MapShapesAndAncestors(aF, TopAbs_EDGE, TopAbs_FACE, MEF);
      }
      // add only faces forming a closed shell
      for (itF.Reset() ; itF.More(); itF.Next())  {
	const TopoDS_Shape& aF=itF.Key();
        TopExp_Explorer expE (aF, TopAbs_EDGE);
        for (; expE.More(); expE.Next()) {
          if (MEF.FindFromKey(expE.Current()).Extent() == 1) {
            break;
	  }
	}
        if (!expE.More()) {
          myBuilder.Add( Shell, aF);
	}
	else {
	  //int a=0;
	}
      }
      
      if (S.ShapeType() == TopAbs_SOLID) {
        TopoDS_Solid Solid;
        myBuilder.MakeSolid( Solid );
        myBuilder.Add (Solid, Shell);
        myBuilder.Add (C, Solid);
      }
      else {
        myBuilder.Add (C, Shell);
      }
    } // if (isClosed) {
    else {
      it.Initialize(aSIm);
      for (; it.More(); it.Next()) {
	myBuilder.Add (C, it.Value());
      }
    }
  }
  //
  myShape = C;
}
 //modified by NIZNHY-PKV Mon Jan 24 10:19:30 2005 f
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& NMTAlgo_Splitter::Modified (const TopoDS_Shape& S) 

{
  TopAbs_ShapeEnum aType;
  TopTools_ListIteratorOfListOfShape it;
  TopTools_MapOfShape aMap;
  TopExp_Explorer anExp;
  //
  myGenerated.Clear();
  aType=S.ShapeType();
  //
  switch (aType) {
    case TopAbs_FACE: {
      if (myModifiedFaces.IsBound(S)) {
	anExp.Init(myShape, aType);
	for(; anExp.More(); anExp.Next()) {
	  aMap.Add(anExp.Current());
	}
	//
	const TopTools_ListOfShape& aLS=myModifiedFaces.Find(S);
	it.Initialize(aLS);
	for (; it.More(); it.Next()) {
	  const TopoDS_Shape& aFx=it.Value();
	  if (!aFx.IsSame(S)) {
	    if (aMap.Contains(aFx)) {
	      myGenerated.Append(aFx);
	    }
	  }
	}
      }
    }
      break;
      //
    case TopAbs_EDGE: {
      if (myImagesEdges.HasImage(S)) {
	anExp.Init(myShape, aType);
	for(; anExp.More(); anExp.Next()) {
	  aMap.Add(anExp.Current());
	}
	//
	const TopTools_ListOfShape& aLE=myImagesEdges.Image(S);
	it.Initialize(aLE);
	for (; it.More(); it.Next()) {
	  const TopoDS_Shape& aEx=it.Value();
	  if (!aEx.IsSame(S)) {
	    if(aMap.Contains(aEx)) {
	      myGenerated.Append(aEx);
	    }
	  }
	}
      }
    }
      break;
      //
    case TopAbs_VERTEX: {
      Standard_Integer aNbS, anIndex, i, aSDVInd;
      //
      const NMTTools_DSFiller& aDSF = Filler();
      const NMTTools_PaveFiller& aPF = aDSF.PaveFiller();
      const NMTDS_ShapesDataStructure& aDS = aDSF.DS();
      //
      aNbS = aDS.NumberOfSourceShapes();
      anIndex = 0;
      //
      for(i=1; i<=aNbS; ++i) {
	const TopoDS_Shape& aS = aDS.Shape(i);
	if(S.IsSame(aS)) {
	  anIndex = i;
	  break;
	}
      }
      //
      if(!anIndex) {
	break;//return myGenerated;
      }
      //
      aSDVInd=aPF.FindSDVertex(anIndex);
      if(!aSDVInd) {
	break;//return myGenerated;
      }
      //
      const TopoDS_Shape& aSDV=aDS.Shape(aSDVInd);
      //
      anExp.Init(myShape, aType);
      for(; anExp.More(); anExp.Next()) {
	const TopoDS_Shape& aVx=anExp.Current();
	if(aSDV.IsSame(aVx)) {
	  myGenerated.Append(aSDV);
	  break;
	}
      }
    } 
      break;
      //
    default:
      break;
  } // switch (aType) {
  //
  return myGenerated;
}
//modified by NIZNHY-PKV Mon Jan 24 10:28:40 2005 t
//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& NMTAlgo_Splitter::Generated(const TopoDS_Shape& S) 
{
  myGenerated.Clear();
  TopTools_ListIteratorOfListOfShape it;
  TopTools_MapOfShape aMap;
  TopExp_Explorer anExp;
  Standard_Boolean bCheckVert = Standard_False;

  if(S.ShapeType() == TopAbs_FACE) {
    if (mySectionParts.Contains(S)) {
      it.Initialize(mySectionParts.FindFromKey(S));
      anExp.Init(myShape, TopAbs_EDGE);
  
      for(; anExp.More(); anExp.Next()) {
	aMap.Add(anExp.Current());
      }

      for (; it.More(); it.Next()) {
	if(aMap.Contains(it.Value())) {
	  myGenerated.Append(it.Value());
	}
      }
    }

    NMTTools_PaveFiller& aPF = myDSFiller->ChangePaveFiller();
    const NMTDS_ShapesDataStructure& aDS = myDSFiller->DS();
    const BOPTools_PInterferencePool& anIP = aPF.InterfPool();

    Standard_Integer aNbS = aDS.NumberOfSourceShapes();
    Standard_Integer anIndex = 0, i;

    for(i = 1; i <= aNbS; ++i) {

      const TopoDS_Shape& aS = aDS.Shape(i);
      if(S.IsSame(aS)) {
	anIndex = i;
	break;
      }

    }

    if(anIndex == 0) return myGenerated;
    if(!anIP->HasInterference(anIndex)) return myGenerated;

    const BOPTools_CArray1OfESInterference& aESs = anIP->ESInterferences();
    Standard_Integer aNbI = aESs.Extent();

    if(aNbI == 0) return myGenerated;

    for(i = 1; i <= aNbI; ++i) {

      const BOPTools_ESInterference& aES = aESs(i);
      Standard_Integer ind1, ind2;
      aES.Indices(ind1, ind2);

      if(ind1 == anIndex || ind2 == anIndex) {

	Standard_Integer aNSI = aES.NewShape();
	if(aDS.GetShapeType(aNSI) == TopAbs_VERTEX) {
	  myGenerated.Append(aDS.Shape(aNSI));
	  bCheckVert = Standard_True;
	}
     
      }

    }
	  
    if(bCheckVert) {
      aMap.Clear();
      anExp.Init(myShape, TopAbs_VERTEX);
  
      for(; anExp.More(); anExp.Next()) {
	aMap.Add(anExp.Current());
      }

      it.Initialize(myGenerated);
      for (; it.More(); it.Next()) {

	if(it.Value().ShapeType() != TopAbs_VERTEX) continue;
	
	if(!aMap.Contains(it.Value())) {
	  myGenerated.Remove(it);
	}

      }
    }
   
    return myGenerated;
  }

  if(S.ShapeType() == TopAbs_EDGE) {

    NMTTools_PaveFiller& aPF = myDSFiller->ChangePaveFiller();
    const NMTDS_ShapesDataStructure& aDS = myDSFiller->DS();
    const BOPTools_PInterferencePool& anIP = aPF.InterfPool();

    Standard_Integer aNbS = aDS.NumberOfSourceShapes();
    Standard_Integer anIndex = 0, i;

    for(i = 1; i <= aNbS; ++i) {

      const TopoDS_Shape& aS = aDS.Shape(i);
      if(S.IsSame(aS)) {
	anIndex = i;
	break;
      }

    }

    if(anIndex == 0) return myGenerated;
    if(!anIP->HasInterference(anIndex)) return myGenerated;

    const BOPTools_CArray1OfEEInterference& aEEs = anIP->EEInterferences();
    Standard_Integer aNbI = aEEs.Extent();

    for(i = 1; i <= aNbI; ++i) {

      const BOPTools_EEInterference& aEE = aEEs(i);
      Standard_Integer ind1, ind2;
      aEE.Indices(ind1, ind2);

      if(ind1 == anIndex || ind2 == anIndex) {

	Standard_Integer aNSI = aEE.NewShape();
	if(aDS.GetShapeType(aNSI) == TopAbs_VERTEX) {
	  myGenerated.Append(aDS.Shape(aNSI));
	  bCheckVert = Standard_True;
	}
     
      }
	  
    }    

    const BOPTools_CArray1OfESInterference& aESs = anIP->ESInterferences();
    aNbI = aESs.Extent();

    for(i = 1; i <= aNbI; ++i) {

      const BOPTools_ESInterference& aES = aESs(i);
      Standard_Integer ind1, ind2;
      aES.Indices(ind1, ind2);

      if(ind1 == anIndex || ind2 == anIndex) {

	Standard_Integer aNSI = aES.NewShape();
	if(aDS.GetShapeType(aNSI) == TopAbs_VERTEX) {
	  myGenerated.Append(aDS.Shape(aNSI));
	  bCheckVert = Standard_True;
	}
     
      }
	  
    }    

    if(bCheckVert) {
      aMap.Clear();
      anExp.Init(myShape, TopAbs_VERTEX);
  
      for(; anExp.More(); anExp.Next()) {
	aMap.Add(anExp.Current());
      }

      it.Initialize(myGenerated);
      for (; it.More(); it.Next()) {

	if(!aMap.Contains(it.Value())) {
	  myGenerated.Remove(it);
	}

      }
    }

    return myGenerated;
  
  }

  return myGenerated;
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean NMTAlgo_Splitter::IsDeleted (const TopoDS_Shape& S) 

{
  const TopTools_ListOfShape& aL = Modified(S);
  if(aL.Extent() != 0) return Standard_False;

  TopTools_MapOfShape aMap;
  TopExp_Explorer anExp;

  TopAbs_ShapeEnum aType = S.ShapeType();

  if(aType == TopAbs_VERTEX || 
     aType == TopAbs_EDGE   || 
     aType == TopAbs_FACE     ) {

    anExp.Init(myShape, aType);
    for(; anExp.More(); anExp.Next()) {
      if(S.IsSame(anExp.Current())) return Standard_False;
    }

  }
  
  return Standard_True;
}

