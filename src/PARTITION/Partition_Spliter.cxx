using namespace std;
//  File      : Partition_Spliter.cxx
//  Created   : Thu Aug 02 16:07:39 2001
//  Author    : Benedicte MARTIN
//  Project   : SALOME
//  Module    : SALOMEGUI
//  Copyright : Open CASCADE
//  $Header$

#include "Partition_Spliter.ixx"
#include "Partition_Inter2d.hxx"
#include "Partition_Inter3d.hxx"
#include "Partition_Loop2d.hxx"
#include "Partition_Loop3d.hxx"

#include "utilities.h"

#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>
#include <TopAbs_Orientation.hxx>

#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepBndLib.hxx>

#include <stdio.h>
#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <TopOpeBRepTool_CurveTool.hxx>

#ifdef DEB
#define DRAW 0
#endif

#ifdef DRAW
#include <DBRep.hxx>
Standard_IMPORT Standard_Boolean AffichInter3d ;
Standard_IMPORT Standard_Boolean AffichInter2d ;
Standard_IMPORT Standard_Boolean AffichVertex;
Standard_IMPORT Standard_Boolean AffichFace;
Standard_IMPORT Standard_Boolean AffichWire;
Standard_IMPORT Standard_Boolean SelectFace;

static char* names = new char[100];

#endif

//=======================================================================
//function : Partition_Spliter
//purpose  : constructor
//=======================================================================

Partition_Spliter::Partition_Spliter()
{
  myAsDes = new BRepAlgo_AsDes;
  Clear();
}

//=======================================================================
//function : AddTool
//purpose  : add cutting tool that will _NOT_ be in result
//=======================================================================

void Partition_Spliter::AddTool(const TopoDS_Shape& S)
{
  for (TopExp_Explorer exp(S,TopAbs_FACE); exp.More(); exp.Next())
    myMapTools.Add(exp.Current());
}

//=======================================================================
//function : AddShape
//purpose  : add object Shape to be splited
//=======================================================================

void Partition_Spliter::AddShape(const TopoDS_Shape& S)
{
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    TopoDS_Iterator it (S);
    for (; it.More(); it.Next())
      AddShape( it.Value());
    return;
  }

  TopExp_Explorer exp(S,TopAbs_FACE);
  if (!exp.More()) { // do not split edges and vertices
    //myBuilder.Add( myShape, S );
    return;
  }
  
  myListShapes.Append(S);

  for (; exp.More(); exp.Next()) {
    myFaceShapeMap.Bind( exp.Current(), S );
    myMapFaces.Add(exp.Current());
    myImagesFaces.SetRoot(exp.Current());
  }
}

//=======================================================================
//function : Shape
//purpose  : return resulting compound
//=======================================================================

TopoDS_Shape Partition_Spliter::Shape() const
{
  return myShape;
}

//=======================================================================
//function : Clear
//purpose  : clear fields
//=======================================================================

void Partition_Spliter::Clear()
{
  myDoneStep = TopAbs_SHAPE;
  
  myListShapes.Clear();
  myMapFaces.Clear();
  myMapTools.Clear();
  myFaceShapeMap.Clear();
  
  myNewSection.Clear();
  
  myAsDes->Clear();
  myImagesFaces.Clear();
  myImagesEdges.Clear();
  myImageShape.Clear();
  
  myInter3d = Partition_Inter3d(myAsDes);
  
  myAddedFacesMap.Clear();

  myInternalFaces.Clear();
}

//=======================================================================
//function : Compute
//purpose  : produce a result
//=======================================================================

void Partition_Spliter::Compute(const TopAbs_ShapeEnum Limit)
{
  if ((Limit != TopAbs_SHAPE && myDoneStep == Limit) ||
      (Limit == TopAbs_SHAPE && myDoneStep == TopAbs_SOLID))
    return;
  
  myBuilder.MakeCompound( myShape );
  
  TopTools_MapIteratorOfMapOfShape it;
  TopTools_ListIteratorOfListOfShape itl;
  TopExp_Explorer exp;

  if (myDoneStep > TopAbs_VERTEX) {

    TopTools_ListOfShape aListFaces;
    aListFaces = myImagesFaces.Roots();
    for (it.Initialize(myMapTools); it.More(); it.Next())
      aListFaces.Append(it.Key());

    //-----------------------------------------------
    // Intersection between faces
    //-----------------------------------------------
    // result is in myAsDes as a map Face - list of new edges;
    // special care is done for section edges, same domain faces and vertices:
    // data about them is inside myInter3d

    myInter3d.CompletPart3d(aListFaces, myFaceShapeMap);

    TopTools_MapOfShape& Modif = myInter3d.TouchedFaces();
    TopTools_MapOfShape& NewEdges = myInter3d.NewEdges();
    Handle(BRepAlgo_AsDes) SectionEdgesAD = myInter3d.SectionEdgesAD();

#ifdef DRAW
    if (AffichInter3d) {
      Standard_Integer i=0;
      for (it.Initialize(NewEdges); it.More(); it.Next(), i++) {
        sprintf(names,"e_%d",i);
        cout << "donly " << names << endl;
        DBRep::Set(names,it.Key());
      }
    }
#endif
    //if (Modif.IsEmpty()) return;

    // -----------------------------------------------
    // store tools intersecting solids as object shapes,
    // they must split into faces too
    // -----------------------------------------------

    // build edge - face map for tool faces
    TopTools_IndexedDataMapOfShapeListOfShape EFM;
    for (it.Initialize(myMapTools); it.More(); it.Next())
      TopExp::MapShapesAndAncestors( it.Key(), TopAbs_EDGE, TopAbs_FACE, EFM);

    TopTools_MapOfShape checkedEdgeMap;
    for (itl.Initialize( myListShapes ); itl.More(); itl.Next()) {
      TopExp_Explorer expSo (itl.Value(), TopAbs_SOLID);
      for (; expSo.More(); expSo.Next()) {

        TopTools_ListOfShape checkFL;  // faces to check
        for ( exp.Init( expSo.Current(), TopAbs_FACE); exp.More(); exp.Next())
          checkFL.Append ( exp.Current());

        // iterate a list while appending new items
        TopTools_ListIteratorOfListOfShape itF, itCF;
        for (itCF.Initialize (checkFL) ; itCF.More(); itCF.Next()) {
          const TopoDS_Shape& F = itCF.Value();
          if ( myAsDes->HasDescendant( F )) {
            // new edges on face to check
            const TopTools_ListOfShape& NEL = myAsDes->Descendant( F );
            TopTools_ListIteratorOfListOfShape itE (NEL);
            for (; itE.More(); itE.Next()) {
              if (checkedEdgeMap.Add( itE.Value() )) {
                // intersected faces originating an edge
                itF.Initialize (myAsDes->Ascendant( itE.Value() ));
                for (; itF.More(); itF.Next()) {
                  if (!myMapFaces.Contains( itF.Value())) {
                    AddShape( itF.Value() );
                    checkFL.Append( itF.Value() );
                  }
                }
                // faces having section edges on F
                if (EFM.Contains( itE.Value())) 
                  itF.Initialize ( EFM.FindFromKey (itE.Value()));
                for (; itF.More(); itF.Next()) {
                  if (!myMapFaces.Contains( itF.Value())) {
                    AddShape( itF.Value() );
                    checkFL.Append( itF.Value() );
                  }
                }
              }
            }
          }
          // find faces cut by edges of F
          TopExp_Explorer expE(F, TopAbs_EDGE);
          for (; expE.More();expE.Next()) {
            if ( SectionEdgesAD->HasAscendant( expE.Current() )
                && checkedEdgeMap.Add( expE.Current() )) {
              itF.Initialize( SectionEdgesAD->Ascendant( expE.Current()) );
              for (; itF.More(); itF.Next()) {
                if (!myMapFaces.Contains( itF.Value())) {
                  AddShape( itF.Value() );
                  checkFL.Append( itF.Value() );
                }
              }
            }
          }
        }
      }
    }

    //-----------------------------------------------
    // Intersection of edges
    //-----------------------------------------------

    // add existing vertices to edges of object faces in myAsDes
    TopTools_MapOfShape DoneEM;
    for ( it.Initialize(myMapFaces); it.More(); it.Next()) {
      const TopoDS_Shape& F  = it.Key();
      TopoDS_Face FForward = TopoDS::Face(F.Oriented(TopAbs_FORWARD));
      for (exp.Init(FForward,TopAbs_EDGE); exp.More(); exp.Next()) {
        const TopoDS_Edge& E = TopoDS::Edge( exp.Current() );
        myAsDes->Add(FForward,E);
        if (DoneEM.Add(E)) {
          TopoDS_Iterator itV(E);
          for (; itV.More(); itV.Next()) {
            const TopoDS_Vertex& V = TopoDS::Vertex( itV.Value());
            myAsDes->Add(E, myInter3d.ReplaceSameDomainV( V, E ));
          }
        }
      }
    }

    // intersect edges that are descendants of a face in myAsDes
    for ( it.Initialize(Modif); it.More(); it.Next()) {
      const TopoDS_Face& F  = TopoDS::Face(it.Key());
      Partition_Inter2d::CompletPart2d (myAsDes, F, NewEdges);
    }
    // now myAsDes contains also new vertices made at edge intersection as
    // descendant of edges both new and old

    myDoneStep = TopAbs_VERTEX;
    
  } //   if (myDoneStep > TopAbs_VERTEX)
  
  if (Limit == TopAbs_VERTEX) {
    // add new vertices to myShape
    for ( it.Initialize( myInter3d.NewEdges() ); it.More(); it.Next()) {
      if (! myAsDes->HasDescendant( it.Key() ))
        continue;
      itl.Initialize( myAsDes->Descendant( it.Key() ));
      for (; itl.More(); itl.Next()) 
        myBuilder.Add ( myShape, itl.Value() );
    }
    return;
  }
  
  if (myDoneStep > TopAbs_EDGE) {

    //-----------------------------------------------
    // Reconstruction of all the edges.
    //-----------------------------------------------

    // Add to myAsDes end vertices of new edges and cut new edges
    int j=1;
    TopTools_MapOfShape& NewEdges = myInter3d.NewEdges();
    TopTools_ListOfShape LSE; // all edge splits
    for ( it.Initialize(NewEdges); it.More(); it.Next()) {

      TopoDS_Vertex V1,V2;
      TopoDS_Edge EE = TopoDS::Edge(it.Key());

      TopTools_ListOfShape aListV, aListF;
      aListV = myAsDes->Descendant(EE); // intersection vertices
      aListF = myAsDes->Ascendant(EE);  // intersected faces

      if (aListV.IsEmpty())
        continue;  // new edge does not intersect any other edge

      // one face is Tool, the other is Shape:
      if ( (myMapTools.Contains(aListF.First()) && myMapFaces.Contains(aListF.Last()) ) ||
          ( myMapFaces.Contains(aListF.First()) && myMapTools.Contains(aListF.Last()) ) )
      {
        TopExp::Vertices(EE,V1,V2);
	Standard_Real Tol = Max (BRep_Tool::Tolerance( V1 ),
				 BRep_Tool::Tolerance( V2 ));

        gp_Pnt P1 = BRep_Tool::Pnt(V1);
        gp_Pnt P2 = BRep_Tool::Pnt(V2);
        Standard_Boolean AddV1 = Standard_True;
        Standard_Boolean AddV2 = Standard_True;

        // add only if there is no intersection at end vertex
        for (itl.Initialize(aListV); itl.More(); itl.Next()) {
          const TopoDS_Vertex& Ve = TopoDS::Vertex(itl.Value()) ;
          Standard_Real Tol2 = Max ( Tol, BRep_Tool::Tolerance( Ve ));
          Tol2 *= Tol2;
          gp_Pnt P = BRep_Tool::Pnt(Ve);
          if (AddV1 && P.SquareDistance(P1) <= Tol2)
            AddV1 = Standard_False;

          if (AddV2 && P.SquareDistance(P2) <= Tol2) 
            AddV2 = Standard_False;
        }

        if (AddV1) {
          aListV.Append(V1);
          myAsDes->Add(EE,V1);
        }

        if (AddV2) {
          aListV.Append(V2);
          myAsDes->Add(EE,V2);
        }
      }
#ifdef DRAW
      if (AffichVertex) {
        for(itl.Initialize(aListV);itl.More();itl.Next(), j++) {
          sprintf(names,"v_%d",j);
          cout << "donly " << names << endl;
          DBRep::Set(names,itl.Value());
        }
      }
#endif

      Standard_Integer NbV=aListV.Extent() ;
      if (NbV>1 || (NbV==1 && V1.IsSame(V2)) ) {
        // cut new edges
        TopTools_ListOfShape LNE;
        MakeEdges (EE,aListV, LNE);
        myImagesEdges.Bind(EE,LNE);
	LSE.Append( LNE );
      }
    }

    // cut old edges
    for ( it.Initialize(myMapFaces); it.More(); it.Next()) {
      for (exp.Init( it.Key(), TopAbs_EDGE); exp.More(); exp.Next()) {
        const TopoDS_Edge& EE = TopoDS::Edge( exp.Current() );
        if ( myImagesEdges.HasImage( EE ))
          continue;
        TopTools_ListOfShape  LNE;
        const TopTools_ListOfShape& aListVV = myAsDes->Descendant(EE);
        MakeEdges (EE, aListVV, LNE);
        myImagesEdges.Bind(EE,LNE);
	LSE.Append( LNE );
      }
    }

    MergeEqualEdges( LSE );
    
    myDoneStep = TopAbs_EDGE;
    
  }  //   if (myDoneStep > TopAbs_EDGE) 

  if (Limit == TopAbs_EDGE) {
    // add splits of old edges
    TopTools_ListIteratorOfListOfShape itNE;
    for (itl.Initialize( myListShapes );itl.More();itl.Next()) {
      for ( exp.Init( itl.Value(), TopAbs_EDGE ); exp.More(); exp.Next()) {
	itNE.Initialize( myImagesEdges.Image( exp.Current() ));
	for ( ; itNE.More(); itNE.Next())
	  myBuilder.Add ( myShape, itNE.Value() );
      }
    }
    // add splits of new edges
    for ( it.Initialize( myInter3d.NewEdges() ); it.More(); it.Next()) {
      itNE.Initialize( myImagesEdges.Image( it.Key() ));
      for (; itNE.More(); itNE.Next())
        myBuilder.Add ( myShape, itNE.Value() );
    }
    return;
  }
  
  // make faces interfering by section edges share the same splits
  //ProcessSectionEdges( SectionEdgesAD );

  
  //-----------------------------------------------
  // split faces
  //-----------------------------------------------

  if (myDoneStep > TopAbs_FACE) {
    
    for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
      TopoDS_Shape FacesComp = MakeFaces ( itl.Value());
      // there is a cunning here: myImagesFaces keeps faces made by Loop2d
      // but some of them may be replaced with splits of same domain face
      // and myImageShape keeps ultimate result
      myImageShape.Bind( itl.Value(), FacesComp );
    }
    
    myDoneStep = TopAbs_FACE;
  }
  
  if (Limit == TopAbs_WIRE ||
      Limit == TopAbs_FACE)   {
    for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
      if ( myMapTools.Contains( itl.Value() ))
	continue; // no result needed for a tool face
      const TopoDS_Shape& FacesComp = myImageShape.Image( itl.Value() ).First();
      for ( exp.Init( FacesComp, Limit); exp.More(); exp.Next())
	myBuilder.Add ( myShape, exp.Current());
    }
    return;
  }

  
  //-----------------------------------------------
  // split solids
  //-----------------------------------------------
  
  // solids must remains closed, so process them first
  Standard_Boolean makeSolids = (Limit == TopAbs_SHAPE ||
				 Limit < TopAbs_SHELL);

  for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
    if (itl.Value().ShapeType() == TopAbs_SOLID) {
      TopTools_ListOfShape NSL;
      MakeShells (itl.Value() , NSL);
      TopTools_ListIteratorOfListOfShape itS(NSL);
      for ( ; itS.More(); itS.Next()) 
        if (makeSolids) {
          // make a solid from a shell
          TopoDS_Solid Solid;
          myBuilder.MakeSolid( Solid );
          myBuilder.Add (Solid, itS.Value());
          myBuilder.Add (myShape, Solid);
        }
	else 
          myBuilder.Add (myShape, itS.Value());
    }
  }
      
  //-----------------------------------------------
  // split shells
  //-----------------------------------------------

  for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
    if (itl.Value().ShapeType() == TopAbs_SHELL) {
      TopTools_ListOfShape NSL;
      MakeShells (itl.Value() , NSL);
      TopTools_ListIteratorOfListOfShape itS(NSL);
      for ( ; itS.More(); itS.Next())
	myBuilder.Add (myShape, itS.Value());
    }
  }
      
  //-----------------------------------------------
  // add split faces
  //-----------------------------------------------

  for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
    const TopoDS_Shape& S = itl.Value();
    if (S.ShapeType() != TopAbs_FACE ||
	myMapTools.Contains( S ))
      continue; 
    TopoDS_Iterator itS( myImageShape.Image(S).First() );
    for (; itS.More(); itS.Next())
      if (! myAddedFacesMap.Contains( itS.Value() ))
	myBuilder.Add (myShape, itS.Value());
  }
    
  myDoneStep = makeSolids ? TopAbs_SOLID : TopAbs_SHELL;
  
}


//=======================================================================
//function : Tri
//purpose  : 
//=======================================================================

static void Tri(const TopoDS_Edge&        E,
		TopTools_SequenceOfShape& Seq)
{
  Standard_Boolean Invert   = Standard_True;
  Standard_Integer NbPoints = Seq.Length();
  Standard_Real    U1,U2;
  TopoDS_Vertex    V1,V2;

  while (Invert) {
    Invert = Standard_False;
    for ( Standard_Integer i = 1; i < Seq.Length(); i++) {
      
      V1 = TopoDS::Vertex(Seq.Value(i));
      V2 = TopoDS::Vertex(Seq.Value(i+1));
      
      V1.Orientation(TopAbs_INTERNAL);
      V2.Orientation(TopAbs_INTERNAL);
      
      U1 = BRep_Tool::Parameter(V1,E);
      U2 = BRep_Tool::Parameter(V2,E);
      
      if (IsEqual(U1,U2)) {
	Seq.Remove(i);
	i--;
	continue;
      }
      if (U2 < U1) {
	Seq.Exchange(i,i+1);
	Invert = Standard_True;
      }
    }
  }
}

//=======================================================================
//function : MakeEdges
//purpose  : cut E by vertices VOnE, return list of new edges NE
//=======================================================================

void Partition_Spliter::MakeEdges (const TopoDS_Edge& E,
                                   const TopTools_ListOfShape& VOnE,
                                   TopTools_ListOfShape& NE   ) const
{
  TopoDS_Edge WE = E;
  WE.Orientation(TopAbs_FORWARD);

  TopTools_ListIteratorOfListOfShape itv(VOnE);
  TopTools_SequenceOfShape SV;

  Standard_Real    U1,U2, f, l;
  TopoDS_Vertex    V1,V2,VF,VL;

  BRep_Tool::Range(WE,f,l);
  TopExp::Vertices(WE,VF,VL);

  if (VOnE.Extent() < 3) { // do not rebuild not cut edge
    if (( VF.IsSame( VOnE.First() ) && VL.IsSame( VOnE.Last() )) ||
	VL.IsSame( VOnE.First() ) && VF.IsSame( VOnE.Last() )  ) {
      NE.Append( E );
      return;
    }
  }

  for (; itv.More(); itv.Next()) 
    SV.Append(itv.Value());

  Tri( WE, SV);

  Standard_Integer iVer, NbVer = SV.Length();


  //----------------------------------------------------------------
  // Construction of the new edges .
  //----------------------------------------------------------------

  if (VF.IsSame(VL)) { // closed edge
    if (NbVer==1) 
      SV.Append( SV.First() );
    else if (!SV.First().IsSame(SV.Last())) {
      Standard_Boolean isFirst=0;
      Standard_Real    minDU = 1.e10;
      TopoDS_Vertex endV = Partition_Inter2d::FindEndVertex(VOnE, f,l, E, isFirst,minDU);
      if (endV.IsSame(SV.First()))
	SV.Append(endV);
      else if (endV.IsSame(SV.Last()))
	SV.Prepend(endV);
      else
	MESSAGE ("END VERTEX IS IN SEQUNCE MIDDLE");
    }
    NbVer = SV.Length();
  }

  for (iVer=1; iVer < NbVer; iVer++) {
    V1  = TopoDS::Vertex(SV(iVer));
    V2  = TopoDS::Vertex(SV(iVer+1));
    
    TopoDS_Shape NewEdge = WE.EmptyCopied();
    V1.Orientation(TopAbs_FORWARD);
    myBuilder.Add  (NewEdge,V1);
    V2.Orientation(TopAbs_REVERSED);
    myBuilder.Add  (NewEdge,V2);
    
    if (iVer==1)
      U1 = f;
    else 	{
      V1.Orientation(TopAbs_INTERNAL);
      U1=BRep_Tool::Parameter(V1,WE);
    }
    if (iVer+1 == NbVer)
      U2 = l;
    else	{
      V2.Orientation(TopAbs_INTERNAL);
      U2=BRep_Tool::Parameter(V2,WE);
    }
    if (Abs(U1-U2) <= Precision::PConfusion()) {
      MESSAGE( "MakeEdges(), EQUAL PARAMETERS OF DIFFERENT VERTICES");
      continue;
    }
    TopoDS_Edge EE=TopoDS::Edge(NewEdge);
    myBuilder.Range (EE,U1,U2);

    TopoDS_Edge NEdge = TopoDS::Edge(NewEdge);
    myBuilder.SameParameter(NEdge,Standard_False);

    Standard_Real tol = 1.0e-2;
    Standard_Boolean flag = BRep_Tool::SameParameter(NEdge);
    if (!flag) {
      BRepLib::SameParameter(NEdge,tol);
    }
    NE.Append(NEdge.Oriented(E.Orientation()));
  }
}

//=======================================================================
//function : FindFacesInside
//purpose  : return compound of faces  of other shapes that are
//           inside <S>. 
//           <S> is an object shape.
//           <CheckClosed> makes avoid faces that do not form a
//           closed shell
//           <All> makes return already added faces
//=======================================================================

TopoDS_Shape Partition_Spliter::FindFacesInside(const TopoDS_Shape& S,
						const Standard_Boolean CheckClosed,
						const Standard_Boolean All)
{
  if (myInternalFaces.IsBound( S ))
    return myInternalFaces.Find ( S );
  
  TopoDS_Compound C;
  myBuilder.MakeCompound(C);
  
  const TopoDS_Shape& SS = myImageShape.Image(S).First(); // compound of split faces of S 

  TopTools_MapOfShape MSE, MPF;
  TopTools_DataMapOfShapeListOfShape MEF;
  TopTools_MapIteratorOfMapOfShape itm;
  TopExp_Explorer expl;
  TopTools_ListOfShape Empty;

  // MSE filling: map of section edges of SS
  for (expl.Init(SS,TopAbs_EDGE); expl.More(); expl.Next()) {
    TopoDS_Shape resE = expl.Current() ;
    if (myNewSection.Contains( resE ))
      MSE.Add(resE);
  }

  // 1. MPF filling: map of resulting faces except those of S,
  // we`ll add to C those of them which are inside SS
  // 2. MEF filling: edge of MSE => faces of MPF
  TopTools_ListIteratorOfListOfShape itl;
  for (itl.Initialize(myListShapes);itl.More();itl.Next()) {
    const TopoDS_Shape& SL = itl.Value();
    if ( S.IsSame( SL )) continue;
    // fill maps
    TopoDS_Iterator itF ( myImageShape.Image(SL).First() );
    for ( ; itF.More(); itF.Next()) {
      const TopoDS_Shape& snf = itF.Value();
      MPF.Add(snf);
      for (expl.Init( snf, TopAbs_EDGE ); expl.More(); expl.Next()) {
	TopoDS_Shape se = expl.Current();
	if ( MSE.Contains(se)) {// section edge
	  if (!MEF.IsBound(se)) 
	    MEF.Bind(se,Empty);
	  MEF(se).Append(snf);
	}
      }
    }
  }
  
  // MSEF: map edge of SS - faces of SS
  TopTools_IndexedDataMapOfShapeListOfShape MSEF;
  TopExp::MapShapesAndAncestors(SS, TopAbs_EDGE, TopAbs_FACE, MSEF);

  // find faces inside S

  Standard_Boolean skipAlreadyAdded = Standard_False;
  Standard_Boolean GoodOri, inside;
  Standard_Real dot;
  TopTools_ListOfShape KeepFaces;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape Mapit;

  for (Mapit.Initialize(MEF); Mapit.More() ; Mapit.Next() ) {
    TopoDS_Edge E = TopoDS::Edge (Mapit.Key());
    TopoDS_Edge OrigE = TopoDS::Edge ( myImagesEdges.Root( E ));
    Standard_Boolean isSectionE = myInter3d.IsSectionEdge ( OrigE );

    TopTools_ListOfShape& EF = MEF.ChangeFind(E);
    itl.Initialize( EF );
    while (itl.More()) {
      TopoDS_Face face1 = TopoDS::Face(itl.Value());
      EF.Remove( itl ); // == itl.Next();
      if (!MPF.Remove( face1 ))
	continue; // was not is MPF ( i.e already checked)
      if (!All &&
	  myAddedFacesMap.Contains( face1 ) &&
	  myAddedFacesMap.Contains( face1.Reversed() )) {
	skipAlreadyAdded = Standard_True;
	continue; // already added to 2 shells
      }

      // find another face which originates from the same face as <face1>
      const TopoDS_Shape& origS = myImagesFaces.Root(face1);
      TopoDS_Shape face2;
      if ( !isSectionE ) {
        while (itl.More()) {
          face2 = itl.Value();
          if (!MPF.Contains( face2 )) {
            EF.Remove( itl );
            continue;
          }
          if (origS.IsSame( myImagesFaces.Root( face2 )))
            break;
          itl.Next();
        }
        if (itl.More()) { // face2 found
          EF.Remove( itl );
          MPF.Remove(face2);
        }
        else
          face2.Nullify();
        itl.Initialize( EF );
      }

      // check that origS is not same domain with SS
      Standard_Boolean sameDom1 = 0, sameDom2 = 0;
      const TopTools_ListOfShape& FL = MSEF.FindFromKey(E); //faces of SS sharing E
      const TopoDS_Shape& origF1 = myImagesFaces.Root(FL.First());
      const TopoDS_Shape& origF2 = myImagesFaces.Root(FL.Last());
      if (origS.IsSame( origF1 ))
	sameDom1 = Standard_True;
      if ( origS.IsSame( origF2 ))
	sameDom2 = Standard_True;
      if (!(sameDom1 || sameDom2) && myInter3d.HasSameDomainF( origS )) {
	sameDom1 = myInter3d.IsSameDomainF( origS, origF1);
	sameDom2 = (origF1 == origF2) ? sameDom1 : myInter3d.IsSameDomainF( origS, origF2);
      }
      if (sameDom1 && sameDom2)
	continue;
      if ((sameDom1 || sameDom2)) {
	inside = Partition_Loop3d::IsInside (E,
					     TopoDS::Face(FL.First()),
					     TopoDS::Face(FL.Last()),
					     1, dot, GoodOri);
	if (inside || (dot + Precision::Angular() >= 1.0))
	  continue; // E is convex between origF1 and origF2 or they are tangent
      }
      
	
      // keep one of found faces
      const TopoDS_Shape& SFace = sameDom1 ? FL.Last() : FL.First(); //face of SS sharing E
      inside = Partition_Loop3d::IsInside (E, TopoDS::Face(SFace), face1,
					   1, dot, GoodOri);
      if ((dot + Precision::Angular() >= 1.0) &&
	  !face2.IsNull()) { // face2 position is not clear, it will be analysed alone
	MPF.Add( face2 );
	EF.Append( face2 );
	face2.Nullify();
      }
      if (inside) 
	KeepFaces.Append(face1);
      else
	if (!face2.IsNull())
	  KeepFaces.Append(face2);
    }
  }

  TopTools_ListOfShape KeptFaces;
  if (MPF.IsEmpty())
    KeptFaces.Append (KeepFaces);

  while (!KeepFaces.IsEmpty()) {
    // add to KeepFaces not distributed faces connected with KeepFaces

    // KeepEdges : map of edges of kept faces
    TopTools_IndexedMapOfShape KeepEdges;
    for (itl.Initialize(KeepFaces);itl.More();itl.Next() ) 
      TopExp::MapShapes( itl.Value(), TopAbs_EDGE, KeepEdges);

    KeptFaces.Append (KeepFaces); // == KeepFaces.Clear()
    
    // keep faces connected with already kept faces
    for (itm.Initialize(MPF);itm.More();itm.Next() ) {
      const TopoDS_Shape& PF = itm.Key();
      for (expl.Init(PF,TopAbs_EDGE); expl.More(); expl.Next()) {
	const TopoDS_Shape& se = expl.Current();
	if (!MSE.Contains(se) && KeepEdges.Contains(se) ) {
	  KeepFaces.Append(PF);
	  MPF.Remove(PF);
	  break;
	}
      }
    }
  }

  // check if kept faces make shell without free edges
  MSEF.Clear();  // edge - kept faces
  MPF.Clear(); // wrong faces
  if (CheckClosed) {
    for (itl.Initialize(KeptFaces); itl.More(); itl.Next() ) 
      TopExp::MapShapesAndAncestors(itl.Value(), TopAbs_EDGE, TopAbs_FACE, MSEF);

    Standard_Integer i, nb = MSEF.Extent();
    Standard_Boolean isClosed = Standard_False;
    while (!isClosed) {
      isClosed = Standard_True;
      for (i=1;  isClosed && i<=nb;  ++i) {
	const TopoDS_Shape& E = MSEF.FindKey( i );
	if (! MSE.Contains( E ))
	  isClosed = ( MSEF(i).Extent() != 1 );
      }
      if (!isClosed) {
	const TopoDS_Shape& F = MSEF.FindFromIndex( i-1 ).First(); // bad face
	MPF.Add( F ); 
	// remove bad face from MSEF
	for (expl.Init( F, TopAbs_EDGE); expl.More(); expl.Next()) {
	  const TopoDS_Shape& E = expl.Current();
	  TopTools_ListOfShape& FL = MSEF.ChangeFromKey( E );
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

  // add to the compound kept faces except bad ones in MPF
  if (KeptFaces.Extent() > MPF.Extent()) {
    for (itl.Initialize(KeptFaces); itl.More(); itl.Next() ) 
      if (! MPF.Contains( itl.Value() )) {
	myBuilder.Add( C, itl.Value());
	//myBuilder.Add( C, itl.Value().Reversed());
      }
  }

  if (!skipAlreadyAdded)
    myInternalFaces.Bind( S, C );
  
  return C;
}

//=======================================================================
//function : MakeShell
//purpose  : split S into compound of shells
//=======================================================================

void Partition_Spliter::MakeShells(const TopoDS_Shape& S,
				   TopTools_ListOfShape& NS)
{
  // check if S is closed shape
  Standard_Boolean isClosed = Standard_True;
  
  TopTools_IndexedDataMapOfShapeListOfShape MEF;
  Standard_Integer i;
  if (S.ShapeType() != TopAbs_SOLID) {
    TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, MEF);
    for (i=1;  isClosed && i<=MEF.Extent();  ++i) 
      isClosed = ( MEF(i).Extent() != 1 );
  }
  Partition_Loop3d ShellMaker;
  // get compound of split faces of S
  const TopoDS_Shape& FacesComp = myImageShape.Image(S).First();
  ShellMaker.AddConstFaces( FacesComp );
  // split faces inside S
  if (isClosed) {
    TopoDS_Shape InternalFacesComp = FindFacesInside(S, Standard_True);
    ShellMaker.AddSectionFaces( InternalFacesComp );
// } else { // a shell may become closed
//     ShellMaker.AddConstFaces( InternalFacesComp );
  }
  
  NS = ShellMaker.MakeShells( myAddedFacesMap );

  // 1. Add faces added to new shell to myAddedFacesMap:
  // avoid rebuilding twice commont part of 2 solids.
  // 2. Check shell closeness (DEBUG)
  TopTools_ListIteratorOfListOfShape itS(NS);
  while ( itS.More()) {
#ifdef DEB
    Standard_Boolean checkCloseness = Standard_True;
#endif
    TopExp_Explorer expF (itS.Value(), TopAbs_FACE);
    for (; expF.More(); expF.Next()) {
      
      myAddedFacesMap.Add (expF.Current());
      
#ifdef DEB
      if (checkCloseness &&
	  ! myInter3d.HasSameDomainF( myImagesFaces.Root(expF.Current()) ))
	checkCloseness = Standard_False;
#endif
    }
    
#ifdef DEB
    if (checkCloseness) {
      // if S is closed, a new shell must be closed too;
      if (isClosed) {
	// check that a new shell is closed
	MEF.Clear();
	TopExp::MapShapesAndAncestors(itS.Value(), TopAbs_EDGE, TopAbs_FACE, MEF);
	for (i=1;  isClosed && i<=MEF.Extent();  ++i) 
	  isClosed = ( MEF(i).Extent() != 1 );
	if (!isClosed) { // remove not closed shell
	  MESSAGE (" NOT CLOSED SHELL " );
	  //NS.Remove( itS );
	  itS.Next();
	  continue;
	}
      }
    }
#endif
    itS.Next();
  } // loop on new shells
}

//=======================================================================
//function : findEqual
//purpose  : compare edges form EL1 against edges from EL2,
//           Result is in EMM binding edge form EL1 to list of equal edges
//           Edges are considered equall only if they have same vertices
//           <addSame>==True makes consider same edges as equal
//           Put in <AllEqMap> all equal edges
//=======================================================================

static void findEqual (const TopTools_ListOfShape& EL1,
		       const TopTools_ListOfShape& EL2,
		       const Standard_Boolean addSame,
		       TopTools_DataMapOfShapeListOfShape& EEM,
		       TopTools_MapOfShape& AllEqMap)
{
  // map vertices to edges for EL2
  TopTools_DataMapOfShapeListOfShape VEM;
  TopTools_ListIteratorOfListOfShape itE1, itE2(EL2);
  TopoDS_Iterator itV;
  TopTools_ListOfShape emptyL;
  for (; itE2.More(); itE2.Next()) {
    for (itV.Initialize( itE2.Value() ); itV.More(); itV.Next()) {
      const TopoDS_Shape& V = itV.Value(); 
      if (! VEM.IsBound( V ) )
	VEM.Bind( V, emptyL);
      VEM( V ).Append( itE2.Value());
    }
  }

  gp_Vec D1, D2;
  gp_Pnt P;
  Standard_Real f,l,u,tol;
  Handle(Geom_Curve) C1, C2;
  Extrema_ExtPC Extrema;
  TopoDS_Vertex V1, V2, V3, V4;

  AllEqMap.Clear();
  
  for (itE1.Initialize(EL1); itE1.More(); itE1.Next()) {
    const TopoDS_Edge& E1 = TopoDS::Edge( itE1.Value());
    if (BRep_Tool::Degenerated( E1 ) || AllEqMap.Contains (E1))
      continue;
    TopExp::Vertices( E1, V1, V2 );

    if (VEM.IsBound(V1))
      itE2.Initialize( VEM(V1) );
    for (; itE2.More(); itE2.Next()) {
      const TopoDS_Edge& E2 = TopoDS::Edge( itE2.Value());
      if (BRep_Tool::Degenerated( E2 ))
        continue;

      if (E1.IsSame(E2)) {
	if (!addSame)
	  continue;
      }
      else {
	TopExp::Vertices( E2, V3, V4);
	if (!V2.IsSame(V4) && !V2.IsSame(V3))
	  continue;
	// E1 and E2 have same vertices
	// check D1 at end points.
        C2 = BRep_Tool::Curve( E2, f,l);
        C1 = BRep_Tool::Curve( E1, f,l);
	u = BRep_Tool::Parameter(V1,E1);
        C1->D1(u, P, D1);
	u = BRep_Tool::Parameter(V1.IsSame(V3) ? V3 : V4, E2);
	C2->D1(u, P, D2);
        D1.Normalize(); D2.Normalize();
        if (Abs(D1*D2) + Precision::Angular() < 1.0)
          continue;
	if (! V1.IsSame(V2)) {
	  u = BRep_Tool::Parameter(V2,E1);
	  C1->D1(u, P, D1);
	  u = BRep_Tool::Parameter(V2.IsSame(V3) ? V3 : V4, E2);
	  C2->D1(u, P, D2);
	  D1.Normalize(); D2.Normalize();
	  if (Abs(D1*D2) + Precision::Angular() < 1.0)
	    continue;
	}
        // check distance at a couple of internal points
        tol = Max(BRep_Tool::Tolerance(E1),
                  BRep_Tool::Tolerance(E2));
        GeomAdaptor_Curve AC1(C1);
        Extrema.Initialize(AC1,f,l);
	Standard_Boolean ok = Standard_True, hasMin = Standard_False;
	BRep_Tool::Range( E2, f, l);
        Standard_Integer i=1, nbi=3;
        for (; i<nbi && ok; ++i) {
          Extrema.Perform( C2->Value( f+(l-f)*i/nbi ));
          Standard_Integer j=1, nbj=Extrema.NbExt();
          for (; j<=nbj && ok; ++j) {
            if (Extrema.IsMin(j)) {
	      hasMin = Standard_True;
              ok = Extrema.Value(j) <= tol;
	    }
          }
        }
        if ( !hasMin || !ok)
          continue;
      }
      // bind E2 to E1 in EEM
      if (!EEM.IsBound(E1)) {
        EEM.Bind (E1, emptyL);
	AllEqMap.Add (E1);
      }
      EEM(E1).Append(E2);
      AllEqMap.Add (E2);
    }
  }
}

//=======================================================================
//function : MakeFaces
//purpose  : split faces of S, return compound of new faces
//=======================================================================

TopoDS_Shape Partition_Spliter::MakeFaces (const TopoDS_Shape& S)
{
  TopoDS_Compound C;
  myBuilder.MakeCompound(C);
  
  TopTools_ListIteratorOfListOfShape itl, itNE;
  
  TopExp_Explorer exp(S,TopAbs_FACE);
  for (; exp.More(); exp.Next()) {

    const TopoDS_Face& F = TopoDS::Face(exp.Current());

    TopTools_ListOfShape LNF;
    
    if (myImagesFaces.HasImage( F )) {
      myImagesFaces.LastImage( F, LNF );
      TopAbs_Orientation oriF = F.Orientation();
      for ( itl.Initialize( LNF ); itl.More(); itl.Next())
	itl.Value().Orientation( oriF );
    }
    else {

      Partition_Loop2d loops;
      loops.Init(F);

      TopTools_IndexedMapOfShape EM;
      TopExp::MapShapes( F, TopAbs_EDGE, EM);

      TopTools_MapOfShape AddedEqualM;
      Standard_Boolean needRebuild = Standard_False;

      // add splits to loops

      // LE: old edges + new not splitted edges
      const TopTools_ListOfShape& LE = myAsDes->Descendant(F);
      for (itl.Initialize(LE); itl.More(); itl.Next()) {
	const TopoDS_Edge& E = TopoDS::Edge( itl.Value() );

	Standard_Boolean isSectionE = myInter3d.IsSectionEdge(E);
	Standard_Boolean isNewE = !EM.Contains( E );

	// LSE: list of split edges
	TopTools_ListOfShape LSE;
	myImagesEdges.LastImage(E,LSE); // splits of E or E itself

	for (itNE.Initialize(LSE); itNE.More(); itNE.Next()) {

	  TopoDS_Edge NE = TopoDS::Edge( itNE.Value() );
	  Standard_Boolean isSameE = NE.IsSame ( E );
	  
	  if ( isNewE || isSectionE || !isSameE) {
	    if (AddedEqualM.Contains( NE ))
	      continue;

	    if (isNewE) {
	      if (isSectionE) {
		if ( ! myInter3d.IsSplitOn( NE, E, F) )
		  continue;
	      }
	      else {
		TopoDS_Vertex V1,V2;
		TopExp::Vertices(NE,V1,V2);
		const TopTools_ListOfShape& EL1 = myAsDes->Ascendant(V1);
		const TopTools_ListOfShape& EL2 = myAsDes->Ascendant(V2);
		if ( EL1.Extent() < 2 && EL2.Extent() < 2 )
		  continue;
	      }
	    }
	    else {
	      NE.Orientation( E.Orientation());
	      if (!isSameE) {
		// orient NE because it may be a split of other edge
		Standard_Real f,l,u;
		Handle(Geom_Curve) C3d  = BRep_Tool::Curve( E,f,l );
		Handle(Geom_Curve) NC3d = BRep_Tool::Curve( NE,f,l);
		if ( C3d != NC3d) {
		  gp_Vec D1, ND1;  gp_Pnt P;
		  TopoDS_Vertex V = TopExp::FirstVertex(NE);
		  u = BRep_Tool::Parameter(V,NE);
		  NC3d->D1 (u, P, ND1);
		  u = BRep_Tool::Parameter(V,E);
		  C3d ->D1 (u, P, D1);
		  if (ND1.Dot(D1) < 0)
		    NE.Reverse();
		}
	      }
	    }
	    if (myEqualEdges.Contains( NE ) && !AddedEqualM.Add( NE ))
	      continue;

	    needRebuild = Standard_True;
	  }

	  if (isNewE || isSectionE)
	    myNewSection.Add( NE );

	  if (isNewE) 
	    loops.AddSectionEdge(NE);
	  else
	    loops.AddConstEdge(NE);
	}
      }

      //-------------------
      // Build the faces.
      //-------------------
      
      if (needRebuild) {
	
        loops.Perform();
        loops.WiresToFaces(myImagesEdges);

        LNF = loops.NewFaces();

        myImagesFaces.Bind(F,LNF);

        // replace the result faces that have already been built
        // during same domain faces reconstruction
        if (myInter3d.HasSameDomainF( F )) {
          // build map edge to same domain faces
          TopTools_IndexedDataMapOfShapeListOfShape EFM;
          TopTools_MapOfShape SDFM; // avoid doubling
          itl.Initialize( myInter3d.SameDomain( F ));
          for (; itl.More(); itl.Next()) {
            if ( !myImagesFaces.HasImage( itl.Value() ))
              continue;
            TopTools_ListIteratorOfListOfShape itNF;
            itNF.Initialize (myImagesFaces.Image( itl.Value() ));
            for ( ; itNF.More(); itNF.Next()) {
              TopoDS_Shape SDF = itNF.Value();
              if (myImagesFaces.HasImage( SDF )) // already replaced
                SDF = myImagesFaces.Image( SDF ).First();
              if (SDFM.Add (SDF))
                TopExp::MapShapesAndAncestors(SDF, TopAbs_EDGE, TopAbs_FACE, EFM);
            }
          }
          // do replace
          TopTools_ListOfShape LOF;
          if ( !EFM.IsEmpty() )
            itl.Initialize( LNF );
          while (itl.More()) {
            const TopoDS_Shape& NF = itl.Value();
            TopExp_Explorer expE ( NF, TopAbs_EDGE );
            const TopoDS_Edge& E  = TopoDS::Edge (expE.Current());
            if (EFM.Contains( E )) {
              const TopTools_ListOfShape& SDFL = EFM.FindFromKey( E );
              TopoDS_Shape SDF = SDFL.First();
              Standard_Boolean GoodOri;
              Standard_Real dot;
              Partition_Loop3d::IsInside (E, TopoDS::Face(NF), TopoDS::Face(SDF),
                                          1, dot, GoodOri);
              if (dot < 0) {
                if (SDFL.Extent() == 1) {
                  itl.Next();
                  continue;
                }
                else
                  SDF = SDFL.Last();
              }
              gp_Vec V1 = Partition_Loop3d::Normal( E, TopoDS::Face( NF ));
              gp_Vec V2 = Partition_Loop3d::Normal( E, TopoDS::Face( SDF ));
              if (V1*V2 < 0)
                SDF.Reverse();

              if (!myImagesFaces.HasImage( NF ))
                myImagesFaces.Bind( NF, SDF );

              LOF.Prepend ( SDF );
              LNF.Remove (itl);
            }
            else
              itl.Next();
          }

          LNF.Append (LOF);
        }
      } // if (needRebuild)
      
      else {
	LNF.Append( F );
	myImagesFaces.Bind(F,LNF);
      }
    } // if (myImagesFaces.HasImage( F ))
    
    for (itl.Initialize(LNF); itl.More(); itl.Next())
      myBuilder.Add ( C, itl.Value());
  } // loop on faces of S

  return C;
}

//=======================================================================
//function : MergeEqualEdges
//purpose  : find equal edges,  choose  ones  to  keep and make
//           them have pcurves on all faces they are shared by
//=======================================================================

void Partition_Spliter::MergeEqualEdges (const TopTools_ListOfShape& LSE)
{
  // find equal edges
  // map: edge - equal edges
  TopTools_DataMapOfShapeListOfShape EEM( LSE.Extent() );
  findEqual (LSE, LSE, 0, EEM, myEqualEdges);

  TopTools_ListOfShape EEL; // list of equal edges
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itM (EEM);
  for ( ; itM.More(); itM.Next()) {
    EEL = itM.Value();
    EEL.Append( itM.Key() );
    // choose an edge to keep, section edges have priority
    TopoDS_Edge EKeep;
    TopTools_ListIteratorOfListOfShape itEE (EEL);
    for (; itEE.More(); itEE.Next()) {
      EKeep = TopoDS::Edge( itEE.Value() );
      const TopoDS_Edge& EKeepOrig = TopoDS::Edge( myImagesEdges.Root( EKeep ));
      if (myInter3d.IsSectionEdge( EKeepOrig ))
        break;
    }

    Standard_Real f,l, tol;
    for (itEE.Initialize (EEL); itEE.More(); itEE.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge( itEE.Value() );
      if ( E.IsSame( EKeep )) 
        continue;
      const TopoDS_Edge& EReplOrig = TopoDS::Edge( myImagesEdges.Root( E ));
      TopTools_ListOfShape FL;
      if (myInter3d.IsSectionEdge( EReplOrig ))
	FL = myInter3d.SectionEdgeFaces ( EReplOrig );
      else
	FL = myAsDes->Ascendant( EReplOrig );
	
      TopTools_ListIteratorOfListOfShape itF (FL);
      for ( ; itF.More(); itF.Next()) {
        const TopoDS_Face& F = TopoDS::Face( itF.Value());
        // build pcurves
        Handle(Geom2d_Curve) pc = BRep_Tool::CurveOnSurface( EKeep, F, f,l);
        if (pc.IsNull()) {
          Handle(Geom_Curve) C3d = BRep_Tool::Curve( EKeep, f, l);
          C3d = new Geom_TrimmedCurve( C3d, f,l);
          pc = TopOpeBRepTool_CurveTool::MakePCurveOnFace (F,C3d,tol);
          if (pc.IsNull()) {
            MESSAGE (" CANT BUILD PCURVE ");
          }
          myBuilder.UpdateEdge( EKeep, pc, F, tol);
        }
      }
      // replace edges in faces
      if (!myImagesEdges.HasImage( E ))
	myImagesEdges.Bind( E, EKeep );
    }
  }
}

//=======================================================================
//function : KeepShapesInside
//purpose  : remove shapes that are outside of S from resul
//=======================================================================

void Partition_Spliter::KeepShapesInside (const TopoDS_Shape& S)
{
  TopoDS_Iterator it;
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    for (it.Initialize( S ); it.More(); it.Next())
      KeepShapesInside( it.Value());
    return;
  }
  if (!myImageShape.HasImage( S ) && ! CheckTool( S ))
    return;
  
  TopoDS_Shape InsFacesComp = FindFacesInside( S, Standard_False, Standard_True);
  TopTools_IndexedMapOfShape MIF; // map of internal faces;
  TopExp::MapShapes( InsFacesComp, TopAbs_FACE, MIF );

  if (MIF.IsEmpty()) return;
  
  // leave in the result only those shapes having a face in MIF
  TopoDS_Compound C;
  myBuilder.MakeCompound(C);
  
  for (it.Initialize( myShape ); it.More(); it.Next()) {

    TopExp_Explorer expResF( it.Value(), TopAbs_FACE );
    for (; expResF.More(); expResF.Next()) {
      if ( MIF.Contains( expResF.Current())) {
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

void Partition_Spliter::RemoveShapesInside (const TopoDS_Shape& S)
{
  TopoDS_Iterator it;
  if (S.ShapeType() < TopAbs_SOLID) { // compound or compsolid
    for (it.Initialize( S ); it.More(); it.Next())
      RemoveShapesInside( it.Value());
    return;
  }
  Standard_Boolean isTool = Standard_False;
  if (!myImageShape.HasImage( S )) {
    isTool = CheckTool( S );
    if (!isTool) return;
  }
  
  TopoDS_Shape InsFacesComp = FindFacesInside( S, Standard_False, Standard_True);
  TopTools_IndexedMapOfShape MIF; // map of internal faces
  TopExp::MapShapes( InsFacesComp, TopAbs_FACE, MIF);

  if (MIF.IsEmpty()) return;

  // add to MIF split faces of S
  if (myImageShape.HasImage( S ))
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
      Standard_Integer i;
      TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, MEF);
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

      TopTools_MapIteratorOfMapOfShape itF (RFM);
      for ( ; itF.More(); itF.Next()) 
	myBuilder.Add( Shell, itF.Key());

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
//function : CheckTool
//purpose  : Return True if <S>  is  a tool shape. Prepare tool
//           faces of <S> for the search of internal faces.
//=======================================================================

Standard_Boolean Partition_Spliter::CheckTool(const TopoDS_Shape& S)
{
  // suppose S has not an image
  
  Standard_Boolean isTool = Standard_False;
  TopoDS_Compound C;
  myBuilder.MakeCompound( C );

  TopExp_Explorer expF( S, TopAbs_FACE);
  for (; expF.More(); expF.Next()) {

    const TopoDS_Face& F = TopoDS::Face( expF.Current() );
    if (myMapTools.Contains( F ))
      isTool = Standard_True;
    else
      continue;

    if (myImagesFaces.HasImage( F )) {
      // F has been reconstructed
      TopAbs_Orientation Fori = F.Orientation();
      TopTools_ListOfShape LNF;
      myImagesFaces.LastImage( F, LNF);
      TopTools_ListIteratorOfListOfShape itF (LNF);
      for ( ; itF.More(); itF.Next())
	myBuilder.Add( C, itF.Value().Oriented(Fori) );
      continue;
    }
    
    Standard_Boolean hasSectionE = myInter3d.HasSectionEdge( F );
    Standard_Boolean hasNewE     = myAsDes->HasDescendant( F );
    if (!hasSectionE && !hasNewE)
      continue; // F intersects nothing
    
    // make an image for F
    
    TopoDS_Face NF = F;
    NF.Orientation(TopAbs_FORWARD);
    NF = TopoDS::Face( NF.EmptyCopied() ); // make a copy
    TopoDS_Wire NW;
    myBuilder.MakeWire( NW );

    // add edges, as less as possible
    TopTools_ListOfShape NEL;
    TopTools_ListIteratorOfListOfShape itNE;
    if (hasSectionE) {
      // add section edges
      TopExp_Explorer expE;
      for ( ; expE.More(); expE.Next()) {
	if (! myImagesEdges.HasImage( expE.Current() ))
	  continue;
	myImagesEdges.LastImage( expE.Current(), NEL );
	for ( itNE.Initialize( NEL ); itNE.More(); itNE.Next())
	  myBuilder.Add ( NW, itNE.Value());
      }
    }
    if (hasNewE) {
      // add new adges
      NEL = myAsDes->Descendant( F );
      for ( itNE.Initialize( NEL ); itNE.More(); itNE.Next()) {
	TopTools_ListOfShape SEL; // splits
	myImagesEdges.LastImage( itNE.Value(), SEL );
	TopTools_ListIteratorOfListOfShape itSE (SEL);
	for ( ; itSE.More(); itSE.Next()) 
	  myBuilder.Add ( NW, itSE.Value());
      }
    }
    myBuilder.Add( NF, NW );
    myBuilder.Add (C, NF);
    
    NF.Orientation( F.Orientation() ); // NF is most probably invalid
    myImagesFaces.Bind (F, NF);
  }
  if (isTool)
    myImageShape.Bind (S, C);

  return isTool;
}
