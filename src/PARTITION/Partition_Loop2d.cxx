using namespace std;
//  File      : Partition_Loop2d.cxx
//  Created   : Thu Aug 02 16:25:17 2001
//  Author    : Benedicte MARTIN
//  Project   : SALOME
//  Module    : PARTITION
//  Copyright : Open CASCADE
//  $Header$


#include "Partition_Loop2d.ixx"

#include "utilities.h"
#include <stdio.h>

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_AsDes.hxx>
#include <BRepOffset_DataMapOfShapeReal.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom2d_Curve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

//=======================================================================
//function : Partition_Loop2d
//purpose  :
//=======================================================================

Partition_Loop2d::Partition_Loop2d()
{
}

//=======================================================================
//function : Init
//purpose  : Init with <F> the set of edges must have
//           pcurves on <F>.
//=======================================================================

void Partition_Loop2d::Init(const TopoDS_Face& F)
{
  myConstEdges.Clear();
  myNewWires  .Clear();
  myNewFaces  .Clear();
  myFace = F;
  myFaceOri = myFace.Orientation();
  myFace.Orientation( TopAbs_FORWARD );
}

//=======================================================================
//function : AddConstEdge
//purpose  : Add <E> as unique edge in the result.
//=======================================================================

void Partition_Loop2d::AddConstEdge (const TopoDS_Edge& E)
{
#ifdef DEB
  Standard_Real f,l;
  Handle(Geom2d_Curve) pc = BRep_Tool::CurveOnSurface( E, myFace, f,l);
  if (pc.IsNull()) {
    INFOS( "AddConstEdge(): EDGE W/O PCURVE on FACE");
  } else
#endif
  {
    myConstEdges.Append(E);
  }
}

void Partition_Loop2d::AddSectionEdge (const TopoDS_Edge& E)
{
#ifdef DEB
  Standard_Real f,l;
  Handle(Geom2d_Curve) pc = BRep_Tool::CurveOnSurface( E, myFace, f,l);
  if (pc.IsNull())
    pc = BRep_Tool::CurveOnSurface( E, myFace, f,l);
  gp_Vec2d Tg1;
  gp_Pnt2d PC;
  pc->D1(0.5*(f+l), PC, Tg1);
  if (Tg1.Magnitude()  <= gp::Resolution()) {
    MESSAGE ("");
  }
  if (pc.IsNull()) {
    INFOS( "AddConstEdge(): EDGE W/O PCURVE on FACE");
  } else
#endif
  {
    myConstEdges.Append(E);
    myConstEdges.Append(E.Reversed());
    mySectionEdges.Add( E );
  }
}

//=======================================================================
//function : SelectEdge
//purpose  : Find the edge <NE> connected <CE> by the vertex <CV> in the list <LE>.
//           <NE> Is erased  of the list. If <CE> is too in the list <LE>
//           with the same orientation, it's erased of the list
//=======================================================================

static Standard_Boolean  SelectEdge(const BRepAdaptor_Surface& Surf,
				    const TopoDS_Edge&    CE,
				    const TopoDS_Vertex&  CV,
				    TopoDS_Edge&          NE,
				    const TopTools_ListOfShape& LE)
{
  NE.Nullify();

  if (LE.Extent() > 1) {
    //--------------------------------------------------------------
    // Several possible edges.
    // - Test the edges differents of CE
    //--------------------------------------------------------------
    TopoDS_Face FForward = Surf.Face();

    Standard_Real   cf, cl, f, l;
    Handle(Geom2d_Curve) Cc, C;
    Cc = BRep_Tool::CurveOnSurface(CE,FForward,cf,cl);

//    Standard_Real tolV, tol2d2;
    Standard_Real tolV = BRep_Tool::Tolerance(CV);
//     tol2d2 = Max ( Surf.UResolution(tolV) , Surf.VResolution(tolV) );
//     tol2d2 = 2 * Max ( tol2d2, Precision::PConfusion() );
//     tol2d2 *= tol2d2;

    Standard_Real uc,u, du = Precision::PConfusion();
    if (CE.Orientation () == TopAbs_FORWARD) uc = cl + du;
    else                                     uc = cf - du;

    gp_Vec2d CTg1, Tg1;
    gp_Pnt2d PC, P;
    gp_Pnt P3d;

    Cc->D1(uc, PC, CTg1);
    if (CE.Orientation () == TopAbs_REVERSED) CTg1.Reverse();

    Standard_Real anglemin = 3 * PI;
//    Standard_Real sqdist, sqdistmin = 1.0e50;

    TopTools_ListIteratorOfListOfShape itl;
    for ( itl.Initialize(LE); itl.More(); itl.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(itl.Value());
      if (E.IsSame(CE))
	continue;
      if (! CV.IsSame( TopExp::FirstVertex( E, Standard_True )))
	continue;

      C = BRep_Tool::CurveOnSurface(E,FForward,f,l);
      if (E.Orientation () == TopAbs_FORWARD) u = f + du;
      else                                    u = l - du;

      C->D1(u, P, Tg1);
//       if (P.SquareDistance(PC); > tol2d2)
// 	  continue;

      if (E.Orientation () == TopAbs_REVERSED) Tg1.Reverse();

      Standard_Real angle = Tg1.Angle(CTg1);

      if (angle <= anglemin) {
	anglemin = angle ;
	NE = E;
#ifdef DEB
// 	sqdist = P.SquareDistance(PC);
// 	if (sqdist < sqdistmin)
// 	  sqdistmin = sqdist;
	P3d = Surf.Value (PC.X(), PC.Y());
#endif
      }
    }
#ifdef DEB
    if (!NE.IsNull() && P3d.Distance( BRep_Tool::Pnt(CV)) > tolV) {
      MESSAGE( "DISTANCE MORE THAN VERTEX TOL (" << tolV << ")" );
      cout << "point p " << P3d.X() << " " << P3d.Y() << " " << P3d.Z() << endl;
    }
#endif
  }
  else if (LE.Extent() == 1) {
    NE = TopoDS::Edge(LE.First());
  }
  else {
    return Standard_False;
  }
  return !NE.IsNull();
}

//=======================================================================
//function : SamePnt2d
//purpose  :
//=======================================================================

static Standard_Boolean  SamePnt2d(const TopoDS_Vertex& V1,
				   const TopoDS_Edge&   E1,
				   const TopoDS_Vertex& V2,
				   const TopoDS_Edge&   E2,
				   const TopoDS_Face&   F)
{
  Standard_Real   f1,f2,l1,l2;
  Handle(Geom2d_Curve) C1 = BRep_Tool::CurveOnSurface(E1,F,f1,l1);
  Handle(Geom2d_Curve) C2 = BRep_Tool::CurveOnSurface(E2,F,f2,l2);

  gp_Pnt2d P1 = C1->Value( BRep_Tool::Parameter(V1,E1));
  gp_Pnt2d P2 = C2->Value( BRep_Tool::Parameter(V2,E2));

  Standard_Real Tol  = 100 * BRep_Tool::Tolerance(V1);
  Standard_Real Dist = P1.Distance(P2);
  return Dist < Tol;
}


//=======================================================================
//function : StoreInMVE
//purpose  :
//=======================================================================

static void StoreInMVE (const TopoDS_Face& /*F*/,
			TopoDS_Edge& E,
			TopTools_DataMapOfShapeListOfShape& MVE )

{
  TopoDS_Vertex V1, V2;
  TopTools_ListOfShape Empty;

  TopExp::Vertices(E,V1,V2);
  if (!MVE.IsBound(V1)) {
    MVE.Bind(V1,Empty);
  }
  MVE(V1).Append(E);

  if (!MVE.IsBound(V2)) {
    MVE.Bind(V2,Empty);
  }
  MVE(V2).Append(E);
}

//=======================================================================
//function : RemoveFromMVE
//purpose  : 
//=======================================================================

static void RemoveFromMVE(const TopoDS_Edge& E,
			  TopTools_DataMapOfShapeListOfShape& MVE)
{
  TopTools_ListIteratorOfListOfShape itl;
  TopoDS_Vertex  V1,V2;
  TopExp::Vertices (E,V1,V2);
  if (MVE.IsBound(V1))
    for ( itl.Initialize(MVE(V1)); itl.More(); itl.Next()) {
      if (itl.Value().IsEqual(E)) {
	MVE(V1).Remove(itl);
	break;
      }
    }
  if (MVE.IsBound(V2))
    for ( itl.Initialize(MVE(V2)); itl.More(); itl.Next()) {
      if (itl.Value().IsEqual(E)) {
	MVE(V2).Remove(itl);
	break;
      }
    }
}
//=======================================================================
//function : addConnected
//purpose  : add to <EM> all edges reachable from <E>
//=======================================================================

static void addConnected(const TopoDS_Shape& E,
			 TopTools_MapOfShape& EM,
			 TopTools_MapOfShape& VM,
			 const TopTools_DataMapOfShapeListOfShape& MVE)
{
  // Loop on vertices of E
  TopoDS_Iterator itV ( E );
  for ( ; itV.More(); itV.Next()) {

    if ( ! VM.Add ( itV.Value() )) continue;
    
    // Loop on edges sharing V
    TopTools_ListIteratorOfListOfShape itE( MVE( itV.Value() ) );
    for (; itE.More(); itE.Next()) {
      if ( EM.Add( itE.Value() ))
	addConnected ( itE.Value(), EM, VM, MVE );
    }
  }
}
//=======================================================================
//function : canPassToOld
//purpose  :
//=======================================================================

static Standard_Boolean canPassToOld (const TopoDS_Shape& V,
				      TopTools_MapOfShape& UsedShapesMap,
				      const TopTools_DataMapOfShapeListOfShape& MVE,
				      const TopTools_MapOfShape& SectionEdgesMap)
{
  TopTools_ListIteratorOfListOfShape itE( MVE(V) );
  // Loop on edges sharing V
  for (; itE.More(); itE.Next()) {
    if ( !UsedShapesMap.Add( itE.Value() ))
      continue; // already checked

    if ( !SectionEdgesMap.Contains( itE.Value() ))
      return Standard_True; // WE PASSED

    TopoDS_Iterator itV( itE.Value() );
    // Loop on vertices of an edge
    for (; itV.More(); itV.Next()) {
      if ( !UsedShapesMap.Add( itV.Value() ))
	continue; // already checked
      else
	return canPassToOld( itV.Value(), UsedShapesMap, MVE, SectionEdgesMap);
    }
  }
  return Standard_False;
}

//=======================================================================
//function : MakeDegenAndSelect
//purpose  : Find parameter of intersection of <CE> with <DE> and
//           select an edge with its parameter closest to found one.
//           Return new degenerated edge trimming <DE> by found parameters
//=======================================================================

static TopoDS_Edge MakeDegenAndSelect(const TopoDS_Edge& CE,
				      const TopoDS_Vertex& CV,
				      TopoDS_Edge& NE,
				      TopTools_SequenceOfShape& EdgesSeq,
				      TColStd_SequenceOfReal& USeq,
				      const TopoDS_Edge& DE)
{
  if (EdgesSeq.Length() < 3) {
    if (CE == EdgesSeq.First())
      NE = TopoDS::Edge( EdgesSeq.Last() );
    else
      NE = TopoDS::Edge( EdgesSeq.First() );
    return DE;
  }

  // find parameter on DE where it intersects CE

  Standard_Real U1;
  Standard_Integer i, nb = EdgesSeq.Length();
  for (i=1; i<= nb; ++i) {
    if (CE == EdgesSeq(i)) {
      U1 = USeq(i);
      break;
    }
  }

  // select NE with param closest to U1 thus finding U2 for a new degen edge

  Standard_Real U2, dU, dUmin = 1.e100;
  Standard_Boolean isReversed = ( DE.Orientation() == TopAbs_REVERSED );
  for (i=1; i<= nb; ++i) {
    dU = USeq(i) - U1;
    if (isReversed ? (dU > 0) : (dU < 0))
	continue;
    dU = Abs( dU );
    if ( dU  > dUmin || IsEqual( dU, 0.))
      continue;
    const TopoDS_Edge& E = TopoDS::Edge ( EdgesSeq(i) );
    if ( ! CV.IsSame( TopExp::FirstVertex( E , Standard_True )))
      continue;
    NE = E;
    dUmin = dU + Epsilon(dU);
    U2 = USeq(i);
  }

  // make a new degenerated edge
  TopoDS_Edge NewDegen = TopoDS::Edge ( DE.EmptyCopied() );

  Standard_Real Tol = BRep_Tool::Tolerance( CV );
  TopoDS_Vertex V = CV;

  BRep_Builder B;
  V.Orientation( NewDegen.Orientation() );
  B.UpdateVertex( V, U1, NewDegen, Tol);
  B.Add ( NewDegen , V );

  V.Reverse();
  B.UpdateVertex( V, U2, NewDegen, Tol);
  B.Add ( NewDegen , V );

  return NewDegen;
}

//=======================================================================
//function : prepareDegen
//purpose  : Intersect <DegEdge> with edges bound to its vertex in <MVE>
//           and store intersection parameter on <DegEdge> in
//           <USeq> as well as the edges them-self in <EdgesSeq>.
//           Bind <DegEdgeIndex> to vertex of <DegEdge> in <MVDEI>
//=======================================================================

static void prepareDegen (const TopoDS_Edge&                        DegEdge,
			  const TopoDS_Face&                        F,
			  const TopTools_DataMapOfShapeListOfShape& MVE,
			  TopTools_SequenceOfShape&                 EdgesSeq,
			  TColStd_SequenceOfReal&                   USeq,
			  TopTools_DataMapOfShapeInteger&           MVDEI,
			  const Standard_Integer                    DegEdgeIndex)
{
  const TopoDS_Vertex& V = TopExp::FirstVertex ( DegEdge );
  MVDEI.Bind ( V, DegEdgeIndex );

  const TopTools_ListOfShape& EdgesList = MVE ( V );
  // if only 2 edges come to degenerated one, no pb in selection and
  // no need to intersect them, just simulate asked data
  Standard_Boolean doIntersect =  ( EdgesList.Extent() > 2 );

  BRepAdaptor_Curve2d DC, C;
  Geom2dInt_GInter InterCC;
  Standard_Real Tol = Precision::PConfusion();
  if ( doIntersect )
    DC.Initialize( DegEdge, F );

  // avoid intersecting twice the same edge
  BRepOffset_DataMapOfShapeReal EUMap ( EdgesList.Extent() );

  Standard_Real U, f, l;
  BRep_Tool::Range (DegEdge, f, l);

  TopTools_ListIteratorOfListOfShape itE (EdgesList);
  for (; itE.More(); itE.Next()) {

    const TopoDS_Edge& E = TopoDS::Edge ( itE.Value() );

    if ( !doIntersect) {
      U = 0.; // it won't be used
    }
    else if ( BRep_Tool::IsClosed( E, F )) {
      // seam edge: select U among f and l
      Standard_Boolean first = Standard_True;
      if ( V.IsSame ( TopExp::FirstVertex( E, Standard_True ) ))
	first = Standard_False;
      if ( DegEdge.Orientation() == TopAbs_REVERSED )
	first = !first;
      U = first ? f : l;
    }
    else if ( EUMap.IsBound( E ) ) {
      // same edge already bound
      U = EUMap( E );
    }
    else {
      // intersect 2d curves
      C.Initialize( E, F );
      InterCC.Perform ( DC, C , Tol, Tol );
      if (! InterCC.IsDone() || InterCC.NbPoints() == 0) {
	MESSAGE ( "NO 2d INTERSECTION ON DEGENERATED EDGE" );
	continue;
      }
      // hope there is only one point of intersection
      U = InterCC.Point( 1 ).ParamOnFirst();
    }
    USeq.Append ( U );
    EdgesSeq.Append ( E );
  }
}
//=======================================================================
//function : Perform
//purpose  : Make loops.
//=======================================================================

void Partition_Loop2d::Perform()
{

  Standard_Integer NbConstEdges = myConstEdges.Extent();
  TopTools_DataMapOfShapeListOfShape MVE(NbConstEdges) , MVE2(NbConstEdges);
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape Mapit;
  TopTools_ListIteratorOfListOfShape itl;
  TopoDS_Vertex V1,V2;
  BRepAdaptor_Surface Surface ( myFace, Standard_False );

  // degenerated edges and parameters of their 2d intersection with other edges
  TopoDS_Edge                    DE [2];
  TopTools_SequenceOfShape       SEID [2]; // seq of edges intersecting degenerated
  TColStd_SequenceOfReal         SeqU [2]; // n-th U corresponds to n-th edge in SEID
  TopTools_DataMapOfShapeInteger MVDEI(2); // map vertex - degenerated edge index
  Standard_Integer               iDeg = 0; // index of degenerated edge [0,1]

  //---------------------------------------------------------
  // Construction map vertex => edges, find degenerated edges
  //---------------------------------------------------------
  for (itl.Initialize(myConstEdges); itl.More(); itl.Next()) {
    TopoDS_Edge& E = TopoDS::Edge(itl.Value());
    if ( BRep_Tool::Degenerated( E )) {
      if (DE[0].IsNull()) DE[0] = E;
      else                DE[1] = E;
    }
    else
      StoreInMVE(myFace,E,MVE);
  }

  // fill data for degenerated edges
  if ( ! DE[0].IsNull() )
    prepareDegen ( DE[0], myFace, MVE, SEID[0], SeqU[0], MVDEI, 0);
  if ( ! DE[1].IsNull() )
    prepareDegen ( DE[1], myFace, MVE, SEID[1], SeqU[1], MVDEI, 1);


  // to detect internal wires
  Standard_Boolean isInternCW = 0;
  MVE2 = MVE;


  //------------------------------
  // Construction of all the wires
  //------------------------------
  // first, we collect wire edges in WEL list looking for same edges that
  // will be then removed possibly exploding a wire into parts;
  // second, build wire(s)

  while (!MVE.IsEmpty()) {

    TopoDS_Vertex    VF,CV;
    TopoDS_Edge      CE,NE,EF;
    TopoDS_Wire      NW;
    BRep_Builder     B;
    Standard_Boolean End = Standard_False;
    TopTools_ListOfShape WEL;

    Mapit.Initialize(MVE);
    if (Mapit.Value().IsEmpty()) {
      MVE.UnBind(Mapit.Key());
      continue;
    }

    // EF first edge.
    EF = CE = TopoDS::Edge(Mapit.Value().First());
    // VF first vertex
    VF = TopExp::FirstVertex( CE, Standard_True);

    isInternCW = Standard_True;

    TopTools_MapOfShape addedEM  (NbConstEdges); // map of edges added to WEL
    TopTools_MapOfShape doubleEM (NbConstEdges); // edges encountered twice in WEL

    //-------------------------------
    // Construction of a wire.
    //-------------------------------
    while (!End) {

      // only a seam is allowed twice in a wire, the others should be removed
      if (addedEM.Add ( CE ) || BRep_Tool::IsClosed( CE, myFace ) )
	WEL.Append( CE );
      else {
	doubleEM.Add( CE );
	RemoveFromMVE (CE,MVE2);
	TopoDS_Edge CERev = CE;
	CERev.Reverse();
	RemoveFromMVE (CERev,MVE2);
      }

      RemoveFromMVE (CE,MVE);

      CV = TopExp::LastVertex( CE, Standard_True);

      if (isInternCW && !mySectionEdges.Contains(CE))
	// wire is internal if all edges are section ones
	isInternCW = Standard_False;

      if (MVDEI.IsBound( CV )) { // CE comes to the degeneration
	iDeg = MVDEI( CV );  
	TopoDS_Edge NewDegen;
	NewDegen = MakeDegenAndSelect( CE, CV, NE, SEID[iDeg], SeqU[iDeg], DE[iDeg]);
	WEL.Append( NewDegen );
        CE = NE;
        End = CV.IsSame( VF );
	continue;
      }

      //--------------
      // stop test
      //--------------
      if (MVE(CV).IsEmpty()) {
	End=Standard_True;
	MVE.UnBind(CV);
      }
      else if (CV.IsSame(VF) && SamePnt2d(CV,CE, VF,EF, myFace) ) {
	End = Standard_True;
      }
      else {
	//----------------------------
	// select new current edge
	//----------------------------
	if (! SelectEdge (Surface,CE,CV,NE,MVE(CV))) {
	  MESSAGE ( " NOT CLOSED WIRE " );
	  End=Standard_True;
	}
	else 
	  CE = NE;
      }
    } // while ( !End ) 

    
    // WEL is built, built wire(s)

    
    itl.Initialize( WEL );
    if ( doubleEM.IsEmpty()) { // no double edges
      B.MakeWire( NW );
      for (; itl.More(); itl.Next())
	B.Add ( NW, itl.Value());
      if (isInternCW) myInternalWL.Append(NW);
      else            myNewWires.Append  (NW);
    }
    
    else {
      // remove double and degenerated edges from WEL
      while (itl.More()) {
	const TopoDS_Edge& E = TopoDS::Edge ( itl.Value() );
	if ( doubleEM.Contains( E ) || BRep_Tool::Degenerated( E ))
	  WEL.Remove( itl );
	else
	   itl.Next();
      }
      if ( WEL.IsEmpty())
	continue;
      // remove double edges from SEID and SeqU
      Standard_Integer i,j;
      for (j=0; j<2; ++j) {
	for (i=1; i<=SEID[j].Length(); ++i) {
	  if (doubleEM.Contains( SEID[j].Value(i))) {
	    SEID[j].Remove( i );
	    SeqU[j].Remove( i-- );
	  }
	}
      }
      // removal of doulbe edges can explode a wire into parts,
      // make new wires of them.
      // A Loop like previous one but without 2d check
      while ( !WEL.IsEmpty() ) {
	CE = TopoDS::Edge( WEL.First() );
	WEL.RemoveFirst();
	B.MakeWire( NW );
	VF = TopExp::FirstVertex ( EF, Standard_True);
	
	End = Standard_False;
	while ( !End) {
	  B.Add( NW, CE );
	  CV = TopExp::LastVertex  ( CE, Standard_True);

	  if (MVDEI.IsBound( CV )) {   // CE comes to the degeneration
	    iDeg = MVDEI( CV );
            TopoDS_Edge NewDegen;
            NewDegen = MakeDegenAndSelect( CE, CV, NE, SEID[iDeg], SeqU[iDeg], DE[iDeg]);
            B.Add( NW, NewDegen );
            End = CV.IsSame( VF );
	    CE = NE;
	    if (!NE.IsNull()) { // remove NE from WEL
	      for (itl.Initialize( WEL ); itl.More(); itl.Next())
		if ( NE == itl.Value()) {
		  WEL.Remove( itl );
		  break;
		}
	    }
	  }  // end degeneration
	  
	  else {
	    if (CV.IsSame( VF )) {
	      End = Standard_True;
	      continue;
	    }
	    // edges in WEL most often are well ordered
	    // so try to iterate until the End
	    Standard_Boolean add = Standard_False;
	    itl.Initialize(WEL);
	    while ( itl.More() && !End) {
              NE = TopoDS::Edge( itl.Value() );
              if ( CV.IsSame( TopExp::FirstVertex( NE, Standard_True ))) {
                WEL.Remove( itl );
		if (add)
		  B.Add( NW, CE );
		CE = NE;
                add = Standard_True;
                CV = TopExp::LastVertex( CE, Standard_True);
                if (MVDEI.IsBound( CV ) || CV.IsSame( VF ))
                  break;
              }
	      else
		itl.Next();
	    }
	    if (!add)
	      End = Standard_True;
	  }
	} // !End
	
	myInternalWL.Append( NW );
      }
    } // end building new wire(s) from WEL

  } // end Loop on MVE
  
  // all wires are built
  
  
  // ============================================================
  // select really internal wires i.e. those from which we can`t
  // pass to an old (not section) edge
  // ============================================================

  Standard_Integer nbIW = myInternalWL.Extent();
  if ( nbIW == 1 ) {
    TopTools_MapOfShape UsedShapes( 2*NbConstEdges );
    TopExp_Explorer expV (myInternalWL.First(), TopAbs_VERTEX);
    if (canPassToOld (expV.Current(), UsedShapes, MVE2, mySectionEdges)) 
      myNewWires.Append ( myInternalWL );
  }
  else if ( nbIW > 1 ) {
    TopTools_MapOfShape outerEM (NbConstEdges); // edges connected to non-section ones
    TopTools_MapOfShape visitedVM (NbConstEdges);
    for ( itl.Initialize( myConstEdges ); itl.More(); itl.Next()) {
      if ( ! mySectionEdges.Contains( itl.Value() )) 
        addConnected (itl.Value(), outerEM, visitedVM, MVE2);
    }
    // if an edge of a wire is in <outerEM>, the wire is not internal
    TopExp_Explorer expIWE;
    TopTools_ListIteratorOfListOfShape itIW ( myInternalWL );
    while (itIW.More()) {
      expIWE.Init ( itIW.Value() , TopAbs_EDGE );
      if ( outerEM.Contains( expIWE.Current() )) {
        myNewWires.Append ( itIW.Value() );
        myInternalWL.Remove( itIW ); // == itIW.Next()
      }
      else
        itIW.Next();
    }
  }
}
//=======================================================================
//function : isHole
//purpose  :
//=======================================================================

static Standard_Boolean isHole (const TopoDS_Wire& W,
				const TopoDS_Face& F)
{
  BRep_Builder B;
  TopoDS_Shape newFace = F.EmptyCopied();
  B.Add(newFace,W.Oriented(TopAbs_FORWARD));
  BRepTopAdaptor_FClass2d classif (TopoDS::Face(newFace),
				   Precision::PConfusion());
  return (classif.PerformInfinitePoint() == TopAbs_IN);
}

//=======================================================================
//function : IsInside
//purpose  : check if W1 is inside W2. Suppose W2 is not a hole !!!!
//=======================================================================

static Standard_Boolean isInside(const TopoDS_Face& F,
				 const TopoDS_Wire& W1,
				 const TopoDS_Wire& W2)
{
  // make a face with wire W2
  BRep_Builder B;
  TopoDS_Shape aLocalShape = F.EmptyCopied();
  TopoDS_Face newFace = TopoDS::Face(aLocalShape);
  B.Add(newFace,W2);

  // get any 2d point of W1
  TopExp_Explorer exp(W1,TopAbs_EDGE);
  const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
  Standard_Real f,l;
  Handle(Geom2d_Curve) C2d = BRep_Tool::CurveOnSurface(edg,F,f,l);
  gp_Pnt2d pt2d(C2d->Value(f));

  BRepTopAdaptor_FClass2d classif(newFace,Precision::PConfusion());
  return (classif.Perform(pt2d) == TopAbs_IN);
}

//=======================================================================
//function : NewWires
//purpose  : Returns the list of wires performed.
//           can be an empty list.
//=======================================================================

const TopTools_ListOfShape&  Partition_Loop2d::NewWires() const
{
  return myNewWires;
}

//=======================================================================
//function : NewFaces
//purpose  : Returns the list of faces.
//Warning  : The method <WiresToFaces> as to be called before.
//           can be an empty list.
//=======================================================================

const TopTools_ListOfShape&  Partition_Loop2d::NewFaces() const
{
  return myNewFaces;
}

//=======================================================================
//function : findEqual
//purpose  : move wires form <WL> to <EqWL> pairs of wires build of the same edges
//=======================================================================

static void findEqual (TopTools_ListOfShape& WL,
		       TopTools_DataMapOfShapeShape& EqWM,
		       const TopoDS_Face& F)
{
  TopTools_ListIteratorOfListOfShape it1, it2;
  Standard_Integer i,j;
  TColStd_MapOfInteger IndMap;
  for (it1.Initialize(WL), i=1;  it1.More();  it1.Next(), i++) {

    if (IndMap.Contains(i)) continue;
    const TopoDS_Wire& Wire1 = TopoDS::Wire( it1.Value());
    
    for (it2.Initialize(WL), j=1;  it2.More();  it2.Next(), j++) {

      if (j <= i || IndMap.Contains(j)) continue;

      TopTools_IndexedMapOfShape EdgesMap;
      TopExp::MapShapes (Wire1, TopAbs_EDGE, EdgesMap);

      const TopoDS_Shape& Wire2 = it2.Value();
      TopoDS_Iterator itE ( Wire2);
      for (; itE.More(); itE.Next()) {
	if ( !EdgesMap.Contains( itE.Value()) )
	  break;
      }
      if (!itE.More()) { // all edges are same
	if (isHole( Wire1, F)) {
	  EqWM.Bind ( Wire1, Wire2 );
	}
	else {
	  EqWM.Bind ( Wire2, Wire1 );
	}
	IndMap.Add(i);
	IndMap.Add(j);
	break;
      }
    }
  }
  // clear WL
  it1.Initialize(WL);
  i=1;
  while (it1.More()) {
    if (IndMap.Contains(i))
      WL.Remove(it1); // next node becomes current and with Next() we would miss it
    else
      it1.Next();
    i++;
  }
}

//=======================================================================
//function : classify
//purpose  : bind to a wire a list of internal wires
//=======================================================================

static void classify(const TopTools_DataMapOfShapeShape& EqWM,
		     BRepAlgo_AsDes& OuterInner,
		     const TopoDS_Face& F)
{
  TopTools_DataMapIteratorOfDataMapOfShapeShape it1, it2;

  for (it1.Initialize(EqWM);  it1.More();  it1.Next()) {
    for (it2.Initialize(EqWM);  it2.More();  it2.Next()) {
      if (it1.Value().IsSame( it2.Value() )) continue;
      const TopoDS_Wire& Wire1 = TopoDS::Wire( it1.Value() );
      const TopoDS_Wire& Wire2 = TopoDS::Wire( it2.Value() );
      if (isInside(F, Wire1, Wire2))
	OuterInner.Add (Wire2, Wire1);
      else if (isInside(F, Wire2, Wire1))
	OuterInner.Add (Wire1, Wire2);
    }
  }
}
//=======================================================================
//function : WiresToFaces
//purpose  : Build faces from the wires result.
//           <EdgeImage> serves to  find  original edge by new
//           one. <Section> contains edges resulting from face
//           intersections
//=======================================================================

//#define USE_BREPFEAT_SPLITSHAPE

#ifdef USE_BREPFEAT_SPLITSHAPE

# include <BRepFeat_SplitShape.hxx>
void  Partition_Loop2d::WiresToFaces(const BRepAlgo_Image& EdgeImage)
#else
     
# include <BRepAlgo_FaceRestrictor.hxx>
void  Partition_Loop2d::WiresToFaces(const BRepAlgo_Image& )
#endif
{
  Standard_Integer nbW = myNewWires.Extent() + myInternalWL.Extent();
  if (nbW==0)
    return;

#ifndef USE_BREPFEAT_SPLITSHAPE

  // ============================================================
  // use BRepAlgo_FaceRestrictor to make faces
  // ============================================================

  BRepAlgo_FaceRestrictor FR;
  FR.Init (myFace,Standard_False);

  // FaceRestrictor is instable in rather simple cases
  // (ex. a single face of bellecoque.brep splited by 10 planes:
  // sometimes 1-2 faces are missing ).
  // So we use it as less as possible: no holes -> make faces by hands

  
  // are there holes in myFace ?
  Standard_Boolean hasOldHoles = Standard_False;
  TopoDS_Iterator itOldW (myFace);
  if ( itOldW.More()) {
    const TopoDS_Wire& FirstOldWire = TopoDS::Wire( itOldW.Value() );
    itOldW.Next();
    hasOldHoles = itOldW.More() || isHole( FirstOldWire, myFace);
  }
  if (myInternalWL.IsEmpty() && !hasOldHoles) {
    // each wire bounds one face
    BRep_Builder B;
    TopTools_ListIteratorOfListOfShape itNW (myNewWires);
    for (; itNW.More(); itNW.Next()) {
      TopoDS_Face NF = TopoDS::Face ( myFace.EmptyCopied() );
      B.Add ( NF, itNW.Value() );
      NF.Orientation( myFaceOri);
      myNewFaces.Append ( NF );
    }
    return;
  }
  
  // FaceRestrictor can't classify wires build on all the same edges
  // and gives incorrect result in such cases (ex. a plane cut into 2 parts by cylinder)
  // We must make faces of equal wires separately. One of equal wires makes a
  // hole in a face and should come together with outer wires of face.
  // The other of a wires pair bounds a face that may have holes in turn.

  // Find equal wires among internal wires
  TopTools_DataMapOfShapeShape EqWM; // key is a hole part of a pair of equal wires
  findEqual (myInternalWL, EqWM, myFace);

  if (!EqWM.IsEmpty()) { // there are equal wires

    if (hasOldHoles)
      myInternalWL.Append( myNewWires ); // an old wire can be inside an equal wire
    
    // classify equal wire pairs
    BRepAlgo_AsDes OuterInner;
    classify (EqWM,OuterInner,myFace);

    // make face of most internal of equal wires and its inner wires
    while ( !EqWM.IsEmpty()) {

      TopTools_ListOfShape prevHolesL; // list of hole-part of previous most internal equal wires

      // find most internal wires among pairs (key - hole, value - outer part)
      TopTools_DataMapIteratorOfDataMapOfShapeShape it(EqWM);
      for ( ; it.More(); it.Next()) {

	TopoDS_Wire outerW = TopoDS::Wire ( it.Value() );
	if (  OuterInner.HasDescendant( outerW ) && // has internal
	     ! OuterInner.Descendant( outerW ).IsEmpty() )
	  continue;

	FR.Add( outerW );

        // add internal wires that are inside of outerW
        TopTools_ListIteratorOfListOfShape itIW (myInternalWL);
        while ( itIW.More()) {
          TopoDS_Wire IW = TopoDS::Wire ( itIW.Value() );
          if ( isInside (myFace, IW, outerW)) {
	    FR.Add (IW);
	    myInternalWL.Remove( itIW ); // == itIW.Next() !!!
	  }
	  else
	    itIW.Next();
	}

	// the hole-part of current pair of equal wires will be in the next new face
	prevHolesL.Append ( it.Key() );

      } // Loop on map of equal pairs searching for innermost wires

      // make faces
      FR.Perform();
      if (FR.IsDone()) {
	for (; FR.More(); FR.Next())
	  myNewFaces.Append(FR.Current());
      }

      FR.Clear();

      // add hole-parts to FaceRestrictor,
      // remove themfrom the EqWM,
      // remove found wires as internal of resting classified wires
      Standard_Boolean clearOuterInner =  ( prevHolesL.Extent() < EqWM.Extent() );
      TopTools_ListIteratorOfListOfShape itPrev (prevHolesL);
      for (; itPrev.More(); itPrev.Next()) {
	TopoDS_Wire& Hole = TopoDS::Wire ( itPrev.Value() );
	FR.Add ( Hole );
	if (clearOuterInner) {
	  const TopoDS_Wire& outerW = TopoDS::Wire ( EqWM.Find( Hole ) );
	  // Loop on wires including outerW
	  TopTools_ListIteratorOfListOfShape itO( OuterInner.Ascendant( outerW ));
	  for (; itO.More(); itO.Next()) {
	    TopTools_ListOfShape& innerL = OuterInner.ChangeDescendant( itO.Value() );
	    TopTools_ListIteratorOfListOfShape itI (innerL);
	    // Loop on internal wires of current including wire
	    for (; itI.More(); itI.Next())
	      if ( outerW.IsSame( itI.Value() )) {
		innerL.Remove( itI );   break;
	      }
	  }
	}
	EqWM.UnBind ( Hole );
      }

    } // while (!EqWM.IsEmpty)

  } //  !EqWM.IsEmpty()

  myNewWires.Append ( myInternalWL );

  TopTools_ListIteratorOfListOfShape itW (myNewWires);
  for (; itW.More(); itW.Next()) {
    TopoDS_Wire& W = TopoDS::Wire ( itW.Value() );
    FR.Add(W);
  }
  FR.Perform();
  for (; FR.IsDone() && FR.More(); FR.Next())
    myNewFaces.Append(FR.Current());


  
#else // ifndef USE_BREPFEAT_SPLITSHAPE

  // ============================================================
  // use BRepFeat_SplitShape to make faces
  // ============================================================
  
  BRepFeat_SplitShape Split(myFace);
  TopTools_MapOfShape AddedSectionEdgesMap;

  myNewWires.Append(myInternalWL);

  TopTools_ListIteratorOfListOfShape it(myNewWires);
  for (; it.More(); it.Next()) {
    TopoDS_Iterator itE(it.Value());
    for (; itE.More(); itE.Next()) {
      const TopoDS_Edge& newE = TopoDS::Edge( itE.Value() );
      if (AddedSectionEdgesMap.Add(newE)) {
	if (mySectionEdges.Contains(newE))
          Split.Add(newE,F); // new edge on face
	else {
	  const TopoDS_Edge& oldE = TopoDS::Edge( EdgeImage.ImageFrom(newE) );
	  Split.Add(newE, oldE); // splited edge
	}
      }
    }
  }
  Split.Build();

  if (Split.IsDone())
    myNewFaces = Split.Modified(F);

#endif  // ifndef USE_BREPFEAT_SPLITSHAPE


  
#ifdef DEB
  Standard_Integer nbF = myNewFaces.Extent();
  if (nbW != nbF)
    cout << "WiresToFaces(): " << nbW << " wires --> " << myNewFaces.Extent() << " faces "
      << endl;
#endif

  TopTools_ListIteratorOfListOfShape itNF (myNewFaces);
  for (; itNF.More(); itNF.Next()) 
    itNF.Value().Orientation( myFaceOri );
}
