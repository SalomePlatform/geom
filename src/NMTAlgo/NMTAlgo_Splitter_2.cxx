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
//purpose  : remove shapes that are outside of S from resul
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
//purpose  : remove shapes that are inside S from resul
//=======================================================================
  void NMTAlgo_Splitter::RemoveShapesInside (const TopoDS_Shape& S)
{
  TopoDS_Iterator it;
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    for (it.Initialize( S ); it.More(); it.Next())
      RemoveShapesInside( it.Value());
    return;
  }
  Standard_Boolean isTool = Standard_False;
  if (!myImageShape.HasImage( S )) {
    //isTool = CheckTool( S );
    //if (!isTool) return;
    return;
  }

  TopoDS_Shape IntFacesComp = FindFacesInside( S, Standard_False, Standard_True);
  TopTools_IndexedMapOfShape MIF; // map of internal faces
  TopExp::MapShapes( IntFacesComp, TopAbs_FACE, MIF);

  if (MIF.IsEmpty()) return;

  // add to MIF split faces of S
  if (myImageShape.HasImage(S))
    TopExp::MapShapes( myImageShape.Image(S).First(), TopAbs_FACE, MIF);

  // leave in the result only those shapes not having all face in MIF
  
  TopoDS_Compound C;
  myBuilder.MakeCompound(C);

  // RMF : faces of removed shapes that encounter once
  TopTools_MapOfShape RFM;
  
  for (it.Initialize( myShape ); it.More(); it.Next()) {
    
    TopExp_Explorer expResF( it.Value(), TopAbs_FACE );
    for (; expResF.More(); expResF.Next())
      if (!MIF.Contains( expResF.Current()))
	break;

    if (expResF.More())
      // add shape to result
      myBuilder.Add( C, it.Value() );
    else 
      // add faces of a removed shape to RFM
      for (expResF.ReInit(); expResF.More(); expResF.Next()) {
	const TopoDS_Shape& F = expResF.Current();
	if ( ! RFM.Remove ( F ))
	  RFM.Add( F );
      }
  }

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
      for (i=1;  isClosed && i<=MEF.Extent();  ++i) 
        isClosed = ( MEF(i).Extent() != 1 );
      break;
    }
    default:
      isClosed = Standard_False;
    }
    if (isClosed) {

      // add to a new shape external faces of removed shapes, ie those in RFM

      TopoDS_Shell Shell;
      myBuilder.MakeShell( Shell );

      // exclude redundant internal face with edges encounterd only once
      TopTools_IndexedDataMapOfShapeListOfShape MEF;
      TopTools_MapIteratorOfMapOfShape itF (RFM);
      for ( ; itF.More(); itF.Next()) 
        TopExp::MapShapesAndAncestors(itF.Key(), TopAbs_EDGE, TopAbs_FACE, MEF);

      // add only faces forming a closed shell
      for (itF.Reset() ; itF.More(); itF.Next())
      {
        TopExp_Explorer expE (itF.Key(), TopAbs_EDGE);
        for (; expE.More(); expE.Next())
          if (MEF.FindFromKey(expE.Current()).Extent() == 1)
            break;
        if (!expE.More())
          myBuilder.Add( Shell, itF.Key());
      }

      if (S.ShapeType() == TopAbs_SOLID) {
        TopoDS_Solid Solid;
        myBuilder.MakeSolid( Solid );
        myBuilder.Add (Solid, Shell);
        myBuilder.Add (C, Solid);
      }
      else
        myBuilder.Add (C, Shell);
    }
    else {
      if (myImageShape.HasImage( S )) {
        for (it.Initialize( myImageShape.Image(S).First()); it.More(); it.Next())
          myBuilder.Add (C, it.Value());
      }
    }
  }
  
  myShape = C;
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& NMTAlgo_Splitter::Modified (const TopoDS_Shape& S) 

{
  myGenerated.Clear();
  TopTools_ListIteratorOfListOfShape it;
  TopTools_MapOfShape aMap;
  TopExp_Explorer anExp;

  if(S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_EDGE) {

    if(S.ShapeType() == TopAbs_FACE) { 
      if (myImagesFaces.HasImage( S )) {
	it.Initialize(myImagesFaces.Image(S));
	anExp.Init(myShape, TopAbs_FACE);
      }
    }
    else {
      if (myImagesEdges.HasImage( S )) {
	it.Initialize(myImagesEdges.Image(S));
	anExp.Init(myShape, TopAbs_EDGE);
      }
    }
  
    for(; anExp.More(); anExp.Next()) {
      aMap.Add(anExp.Current());
    }

    for (; it.More(); it.Next()) {
      if(aMap.Contains(it.Value())) {
	myGenerated.Append(it.Value());
      }
    }

    return myGenerated;

  }

  if(S.ShapeType() == TopAbs_VERTEX) {
    
    const NMTTools_DSFiller& aDSF = Filler();
    const NMTTools_PaveFiller& aPF = aDSF.PaveFiller();
    const NMTDS_ShapesDataStructure& aDS = aDSF.DS();

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

    Standard_Integer aSDVInd = aPF.FindSDVertex(anIndex);

    if(aSDVInd == 0) return myGenerated;

    const TopoDS_Shape aSDV = aDS.Shape(aSDVInd);

    anExp.Init(myShape, TopAbs_VERTEX);
    for(; anExp.More(); anExp.Next()) {

      if(aSDV.IsSame(anExp.Current())) {
	myGenerated.Append(aSDV);
	break;
      }

    }
    
  }

  return myGenerated;
}
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

