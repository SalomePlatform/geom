//  GEOM OBJECT : interactive object for Geometry entities visualization
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_OCCReader.h
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

#include "GEOM_OCCReader.h"

// VTK Includes
#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkMergePoints.h>

#include <vtkTransform.h>
#include <vtkMatrix4x4.h>

// OpenCASCADE Includes
#include <BRepAdaptor_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBndLib.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <Precision.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dHatch_Hatcher.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <HatchGen_Domain.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Precision.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Adaptor3d_HCurve.hxx>

#include "utilities.h"

using namespace std;

#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

// Constante for iso building
static Standard_Real IntersectorConfusion = 1.e-10 ; // -8 ;
static Standard_Real IntersectorTangency  = 1.e-10 ; // -8 ;
static Standard_Real HatcherConfusion2d   = 1.e-8 ;
static Standard_Real HatcherConfusion3d   = 1.e-8 ;

static Standard_Integer lastVTKpoint = 0;
static Standard_Integer PlotCount = 0;
static Standard_Real IsoRatio = 1.001;
static Standard_Integer MaxPlotCount = 5; 

//=======================================================================
// Function : New
// Purpose  : 
//=======================================================================

GEOM_OCCReader* GEOM_OCCReader::New()
{
  vtkObject* ret = vtkObjectFactory::CreateInstance("GEOM_OCCReader");
  if(ret) {
    return (GEOM_OCCReader*)ret;
  }
  return new GEOM_OCCReader;
}

//=======================================================================
// Function : GEOM_OCCReader
// Purpose  : 
//=======================================================================

GEOM_OCCReader::GEOM_OCCReader()
{
  //this->myShape = NULL;
  this->amode = 0;
  this->forced = Standard_False;
  this->discretiso = 15;
  this->nbisos = 1;
}
//=======================================================================
// Function : ~GEOM_OCCReader
// Purpose  : 
//=======================================================================

GEOM_OCCReader::~GEOM_OCCReader()
{
}


//=======================================================================
// Function : Execute
// Purpose  : 
//=======================================================================


void GEOM_OCCReader::Execute() {

  vtkPolyData* output = this->GetOutput();
  vtkPoints* Pts = NULL;
  vtkCellArray* Cells = NULL;
  TopLoc_Location aLoc;

  // Allocation
  Pts = vtkPoints::New();
  Cells = vtkCellArray::New();
	
  //Compute number of triangles and points
  Standard_Integer nbpoly=0,nbpts=0;

  if(amode==1) {
    //for shading
    
    if(myShape.ShapeType() == TopAbs_FACE) {
      // whole FACE 
      const TopoDS_Face& aFace = TopoDS::Face(myShape);
      Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
      if(aPoly.IsNull()) {
	Pts->Delete();
	Cells->Delete();
	return;
      }

      nbpts = aPoly->NbNodes();
      nbpoly = aPoly->NbTriangles();

      Pts->SetNumberOfPoints(nbpts);
      Cells->Allocate(Cells->EstimateSize(nbpoly,3));
    }
    else { 
	Cells->Delete();
	Pts->Delete();
	return; 
    }
  }

  // Start computation
  if(amode == 0) {
    ComputeWireframe(Pts,Cells);
    output->SetPoints(Pts);
    output->SetLines(Cells);
    output->Squeeze();
  }
  else {
    if(myShape.ShapeType() == TopAbs_FACE) {
      ComputeShading(Pts,Cells);

      output->SetPoints(Pts);
      output->SetPolys(Cells);
      output->Squeeze();
    }
  }
  Pts->Delete();
  Cells->Delete();
  
}

//=======================================================================
// Function : ComputeWireframe
// Purpose  : Compute the shape in CAD wireframe mode
//=======================================================================

void GEOM_OCCReader::ComputeWireframe(vtkPoints* Pts,vtkCellArray* Cells){

  // Check the type of the shape:
  if(myShape.ShapeType() == TopAbs_FACE) {
    // Face
    TransferFaceWData(TopoDS::Face(myShape),Pts,Cells);
  } else if(myShape.ShapeType() == TopAbs_EDGE) {
    // Edge
    TransferEdgeWData(TopoDS::Edge(myShape),Pts,Cells);
  } else {
    if(myShape.ShapeType() == TopAbs_VERTEX) {
      // Vertex
      TransferVertexWData(TopoDS::Vertex(myShape),Pts,Cells);
    }
  }
}

//=======================================================================
// Function : TransferFaceWData
// Purpose  : Transfert wireframe data for FACE
//=======================================================================

void GEOM_OCCReader::TransferFaceWData(const TopoDS_Face& aFace,
					 vtkPoints* Pts,
					 vtkCellArray* Cells) 
{
  TopoDS_Face aCopyFace = aFace; 
  aCopyFace.Orientation (TopAbs_FORWARD);
  createISO(aCopyFace,Precision::Infinite(),1,Pts,Cells);
}

//=======================================================================
// Function : createISO
// Purpose  : Create ISO for Face Wireframe representation 
//=======================================================================

void GEOM_OCCReader::createISO (const TopoDS_Face&     TopologicalFace,
				  const Standard_Real    Infinite,
				  const Standard_Integer NbIsos,
				  vtkPoints* Pts,
				  vtkCellArray* Cell)
{
  Geom2dHatch_Hatcher aHatcher (Geom2dHatch_Intersector (IntersectorConfusion,
							 IntersectorTangency),
				HatcherConfusion2d,
				HatcherConfusion3d,
				Standard_True,
				Standard_False);
  
  Standard_Real myInfinite,myUMin,myUMax,myVMin,myVMax;
  //myInfinite = Precision::Infinite();
  myInfinite = 1e38; // VTK uses float numbers - Precision::Infinite() is double and can not be accepted.

  Standard_Integer myNbDom;
  TColStd_Array1OfReal myUPrm(1, NbIsos),myVPrm(1, NbIsos);
  TColStd_Array1OfInteger myUInd(1, NbIsos),myVInd(1, NbIsos);

  myUInd.Init(0);
  myVInd.Init(0);

  //-----------------------------------------------------------------------
  // If the Min Max bounds are infinite, there are bounded to Infinite
  // value.
  //-----------------------------------------------------------------------

  BRepTools::UVBounds (TopologicalFace, myUMin, myUMax, myVMin, myVMax) ;
  Standard_Boolean InfiniteUMin = Precision::IsNegativeInfinite (myUMin) ;
  Standard_Boolean InfiniteUMax = Precision::IsPositiveInfinite (myUMax) ;
  Standard_Boolean InfiniteVMin = Precision::IsNegativeInfinite (myVMin) ;
  Standard_Boolean InfiniteVMax = Precision::IsPositiveInfinite (myVMax) ;
  if (InfiniteUMin && InfiniteUMax) {
    myUMin = - myInfinite ;
    myUMax =   myInfinite ;
  } else if (InfiniteUMin) {
    myUMin = myUMax - myInfinite ;
  } else if (InfiniteUMax) {
    myUMax = myUMin + myInfinite ;
  }
  if (InfiniteVMin && InfiniteVMax) {
    myVMin = - myInfinite ;
    myVMax =   myInfinite ;
  } else if (InfiniteVMin) {
    myVMin = myVMax - myInfinite ;
  } else if (InfiniteVMax) {
    myVMax = myVMin + myInfinite ;
  }

  //-----------------------------------------------------------------------
  // Retreiving the edges and loading them into the hatcher.
  //-----------------------------------------------------------------------

  TopExp_Explorer ExpEdges ;
  for (ExpEdges.Init (TopologicalFace, TopAbs_EDGE) ; ExpEdges.More() ; ExpEdges.Next()) {
    const TopoDS_Edge& TopologicalEdge = TopoDS::Edge (ExpEdges.Current()) ;
    Standard_Real U1, U2 ;
    const Handle(Geom2d_Curve) PCurve = BRep_Tool::CurveOnSurface (TopologicalEdge, TopologicalFace, U1, U2) ;

    if ( PCurve.IsNull() ) {
      return;
    }

    if ( U1==U2) {
      return;
    }

    //-- Test if a TrimmedCurve is necessary
    if(   Abs(PCurve->FirstParameter()-U1)<= Precision::PConfusion() 
	  && Abs(PCurve->LastParameter()-U2)<= Precision::PConfusion()) { 
      aHatcher.AddElement (PCurve, TopologicalEdge.Orientation()) ;      
    }
    else { 
      if (!PCurve->IsPeriodic()) {
	Handle (Geom2d_TrimmedCurve) TrimPCurve =Handle(Geom2d_TrimmedCurve)::DownCast(PCurve);
	if (!TrimPCurve.IsNull()) {
	  if (TrimPCurve->BasisCurve()->FirstParameter()-U1 > Precision::PConfusion() ||
	      U2-TrimPCurve->BasisCurve()->LastParameter()  > Precision::PConfusion()) {
	    aHatcher.AddElement (PCurve, TopologicalEdge.Orientation()) ;      
	    return;
	  }
	}
	else {
	  if (PCurve->FirstParameter()-U1 > Precision::PConfusion()){
	    U1=PCurve->FirstParameter();
	  }
	  if (U2-PCurve->LastParameter()  > Precision::PConfusion()){
	    U2=PCurve->LastParameter();
	  }
	}
      }
      Handle (Geom2d_TrimmedCurve) TrimPCurve = new Geom2d_TrimmedCurve (PCurve, U1, U2) ;
      aHatcher.AddElement (TrimPCurve, TopologicalEdge.Orientation()) ;
    }
  }


  //-----------------------------------------------------------------------
  // Loading and trimming the hatchings.
  //-----------------------------------------------------------------------

  Standard_Integer IIso ;
  Standard_Real DeltaU = Abs (myUMax - myUMin) ;
  Standard_Real DeltaV = Abs (myVMax - myVMin) ;
  Standard_Real confusion = Min (DeltaU, DeltaV) * HatcherConfusion3d ;
  aHatcher.Confusion3d (confusion) ;

  Standard_Real StepU = DeltaU / (Standard_Real) NbIsos ;
  if (StepU > confusion) {
    Standard_Real UPrm = myUMin + StepU / 2. ;
    gp_Dir2d Dir (0., 1.) ;
    for (IIso = 1 ; IIso <= NbIsos ; IIso++) {
      myUPrm(IIso) = UPrm ;
      gp_Pnt2d Ori (UPrm, 0.) ;
      Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir)) ;
      myUInd(IIso) = aHatcher.AddHatching (HCur) ;
      UPrm += StepU ;
    }
  }

  Standard_Real StepV = DeltaV / (Standard_Real) NbIsos ;
  if (StepV > confusion) {
    Standard_Real VPrm = myVMin + StepV / 2. ;
    gp_Dir2d Dir (1., 0.) ;
    for (IIso = 1 ; IIso <= NbIsos ; IIso++) {
      myVPrm(IIso) = VPrm ;
      gp_Pnt2d Ori (0., VPrm) ;
      Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir)) ;
      myVInd(IIso) = aHatcher.AddHatching (HCur) ;
      VPrm += StepV ;
    }
  }

  //-----------------------------------------------------------------------
  // Computation.
  //-----------------------------------------------------------------------

  aHatcher.Trim() ;

  myNbDom = 0 ;
  for (IIso = 1 ; IIso <= NbIsos ; IIso++) {
    Standard_Integer Index ;

    Index = myUInd(IIso) ;
    if (Index != 0) {
      if (aHatcher.TrimDone (Index) && !aHatcher.TrimFailed (Index)) {
	aHatcher.ComputeDomains (Index);
	if (aHatcher.IsDone (Index)) myNbDom = myNbDom + aHatcher.NbDomains (Index) ;
      }
    }

    Index = myVInd(IIso) ;
    if (Index != 0) {
      if (aHatcher.TrimDone (Index) && !aHatcher.TrimFailed (Index)) {
	aHatcher.ComputeDomains (Index);
	if (aHatcher.IsDone (Index)) myNbDom = myNbDom + aHatcher.NbDomains (Index) ;
      }
    }
  }

  //-----------------------------------------------------------------------
  // Push iso lines in vtk kernel
  //-----------------------------------------------------------------------


  Standard_Integer pt_start_idx = 0;

  for (Standard_Integer UIso = myUPrm.Lower() ; UIso <= myUPrm.Upper() ; UIso++) {
    Standard_Integer UInd = myUInd.Value (UIso) ;
    if (UInd != 0) {
      Standard_Real UPrm = myUPrm.Value (UIso) ;
      if (!aHatcher.IsDone (UInd)) {
	MESSAGE("DBRep_IsoBuilder:: U iso of parameter: "<<UPrm)
	switch (aHatcher.Status (UInd)) {
	case HatchGen_NoProblem          : MESSAGE("No Problem")          ; break ;
	case HatchGen_TrimFailure        : MESSAGE("Trim Failure")        ; break ;
	case HatchGen_TransitionFailure  : MESSAGE("Transition Failure")  ; break ;
	case HatchGen_IncoherentParity   : MESSAGE("Incoherent Parity")   ; break ;
	case HatchGen_IncompatibleStates : MESSAGE("Incompatible States") ; break ;
	}
      } else {
	Standard_Integer NbDom = aHatcher.NbDomains (UInd) ;
	for (Standard_Integer IDom = 1 ; IDom <= NbDom ; IDom++) {
	  const HatchGen_Domain& Dom = aHatcher.Domain (UInd, IDom) ;
	  Standard_Real V1 = Dom.HasFirstPoint()  ? Dom.FirstPoint().Parameter()  : myVMin - myInfinite ;
	  Standard_Real V2 = Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite ;
	  DrawIso(GeomAbs_IsoU, UPrm, V1, V2, Pts, Cell,pt_start_idx);
	}
      }
    }
  }

  for (Standard_Integer VIso = myVPrm.Lower() ; VIso <= myVPrm.Upper() ; VIso++) {
    Standard_Integer VInd = myVInd.Value (VIso) ;
    if (VInd != 0) {
      Standard_Real VPrm = myVPrm.Value (VIso) ;
      if (!aHatcher.IsDone (VInd)) {
	MESSAGE("DBRep_IsoBuilder:: V iso of parameter: "<<VPrm)
	switch (aHatcher.Status (VInd)) {
	case HatchGen_NoProblem          : MESSAGE("No Problem")          ; break ;
	case HatchGen_TrimFailure        : MESSAGE("Trim Failure")        ; break ;
	case HatchGen_TransitionFailure  : MESSAGE("Transition Failure")  ; break ;
	case HatchGen_IncoherentParity   : MESSAGE("Incoherent Parity")   ; break ;
	case HatchGen_IncompatibleStates : MESSAGE("Incompatible States") ; break ;
	}
      } else {
	Standard_Integer NbDom = aHatcher.NbDomains (VInd) ;
	for (Standard_Integer IDom = 1 ; IDom <= NbDom ; IDom++) {
	  const HatchGen_Domain& Dom = aHatcher.Domain (VInd, IDom) ;
	  Standard_Real U1 = Dom.HasFirstPoint()  ? Dom.FirstPoint().Parameter()  : myVMin - myInfinite ;
	  Standard_Real U2 = Dom.HasSecondPoint() ? Dom.SecondPoint().Parameter() : myVMax + myInfinite ;
	  DrawIso(GeomAbs_IsoV, VPrm, U1, U2, Pts, Cell,pt_start_idx) ;
	}
      }
    }
  }

}

//=======================================================================
// Function : MoveTo
// Purpose  : Init VTK ISO PLOT
//=======================================================================
void GEOM_OCCReader::MoveTo(gp_Pnt P,
			      vtkPoints* Pts)
{    
  float coord[3];

  coord[0] = P.X(); coord[1] = P.Y(); coord[2] = P.Z();
  lastVTKpoint = Pts->InsertNextPoint(coord);
    
} 

//=======================================================================
// Function : DrawTo
// Purpose  : Plot point in VTK
//=======================================================================
void GEOM_OCCReader::DrawTo(gp_Pnt P,
			      vtkPoints* Pts,
			      vtkCellArray* Cells)
{
  float coord[3];
  coord[0] = P.X(); coord[1] = P.Y(); coord[2] = P.Z();
  Standard_Integer NewVTKpoint =  Pts->InsertNextPoint(coord);

  int pts[2];
  pts[0] = lastVTKpoint;
  pts[1] = NewVTKpoint;

  Cells->InsertNextCell(2,pts);
    
  lastVTKpoint = NewVTKpoint;
}


//=======================================================================
// Function : DrawIso
// Purpose  : Draw an iso on vtk
//=======================================================================
void GEOM_OCCReader::DrawIso(GeomAbs_IsoType T, 
			       Standard_Real Par, 
			       Standard_Real T1,
			       Standard_Real T2,
			       vtkPoints* Pts,
			       vtkCellArray* Cells,
			       Standard_Integer& startidx)
{

  Standard_Boolean halt = Standard_False;
  Standard_Integer j,myDiscret = discretiso;
  Standard_Real U1,U2,V1,V2,stepU=0.,stepV=0.;
  gp_Pnt P;
  TopLoc_Location l;

  const Handle(Geom_Surface)& S = BRep_Tool::Surface(TopoDS::Face(myShape),l);
  if (!S.IsNull()) {
    BRepAdaptor_Surface S(TopoDS::Face(myShape),Standard_False);
      
    GeomAbs_SurfaceType SurfType = S.GetType();

    GeomAbs_CurveType CurvType = GeomAbs_OtherCurve;

    Standard_Integer Intrv, nbIntv;
    Standard_Integer nbUIntv = S.NbUIntervals(GeomAbs_CN);
    Standard_Integer nbVIntv = S.NbVIntervals(GeomAbs_CN);
    TColStd_Array1OfReal TI(1,Max(nbUIntv, nbVIntv)+1);


    if (T == GeomAbs_IsoU) {
      S.VIntervals(TI, GeomAbs_CN);
      V1 = Max(T1, TI(1));
      V2 = Min(T2, TI(2));
      U1 = Par;
      U2 = Par;
      stepU = 0;
      nbIntv = nbVIntv;
    }
    else {
      S.UIntervals(TI, GeomAbs_CN);
      U1 = Max(T1, TI(1));
      U2 = Min(T2, TI(2));
      V1 = Par;
      V2 = Par;
      stepV = 0;
      nbIntv = nbUIntv;
    }	
	
    S.D0(U1,V1,P);
    MoveTo(P,Pts);

    for (Intrv = 1; Intrv <= nbIntv; Intrv++) {

      if (TI(Intrv) <= T1 && TI(Intrv + 1) <= T1)
	continue;
      if (TI(Intrv) >= T2 && TI(Intrv + 1) >= T2)
	continue;
      if (T == GeomAbs_IsoU) {
	V1 = Max(T1, TI(Intrv));
	V2 = Min(T2, TI(Intrv + 1));
	stepV = (V2 - V1) / myDiscret;
      }
      else {
	U1 = Max(T1, TI(Intrv));
	U2 = Min(T2, TI(Intrv + 1));
	stepU = (U2 - U1) / myDiscret;
      }

      switch (SurfType) {
	//-------------GeomAbs_Plane---------------
      case GeomAbs_Plane :
	break;
	//----GeomAbs_Cylinder   GeomAbs_Cone------
      case GeomAbs_Cylinder :
      case GeomAbs_Cone :
	if (T == GeomAbs_IsoV) {
	  for (j = 1; j < myDiscret; j++) {
	    U1 += stepU;
	    V1 += stepV;
	    S.D0(U1,V1,P);
	    DrawTo(P,Pts,Cells);
	  }
	}
	break;
	//---GeomAbs_Sphere   GeomAbs_Torus--------
	//GeomAbs_BezierSurface GeomAbs_BezierSurface
      case GeomAbs_Sphere :
      case GeomAbs_Torus :
      case GeomAbs_OffsetSurface :
      case GeomAbs_OtherSurface :
	for (j = 1; j < myDiscret; j++) {
	  U1 += stepU;
	  V1 += stepV;
	  S.D0(U1,V1,P);
	  DrawTo(P,Pts,Cells);
	}
	break;
	//-------------GeomAbs_BSplineSurface------
      case GeomAbs_BezierSurface :
      case GeomAbs_BSplineSurface :
	for (j = 1; j <= myDiscret/2; j++) {

	  PlotCount = 0;

	  PlotIso ( S, T, U1, V1, (T == GeomAbs_IsoV) ? stepU*2. : stepV*2., halt, Pts, Cells);
	  U1 += stepU*2.;
	  V1 += stepV*2.;
	}
	break;
	//-------------GeomAbs_SurfaceOfExtrusion--
	//-------------GeomAbs_SurfaceOfRevolution-
      case GeomAbs_SurfaceOfExtrusion :
      case GeomAbs_SurfaceOfRevolution :
	if ((T == GeomAbs_IsoV && SurfType == GeomAbs_SurfaceOfRevolution) ||
	    (T == GeomAbs_IsoU && SurfType == GeomAbs_SurfaceOfExtrusion)) {
	  if (SurfType == GeomAbs_SurfaceOfExtrusion) break;
	  for (j = 1; j < myDiscret; j++) {
	    U1 += stepU;
	    V1 += stepV;
	    S.D0(U1,V1,P);
	    DrawTo(P,Pts,Cells);
	  }
	} else {
	  CurvType = (S.BasisCurve())->GetType();
	  switch (CurvType) {
	  case GeomAbs_Line :
	    break;
	  case GeomAbs_Circle :
	  case GeomAbs_Ellipse :
	    for (j = 1; j < myDiscret; j++) {
	      U1 += stepU;
	      V1 += stepV;
	      S.D0(U1,V1,P);
	      DrawTo(P,Pts,Cells);
	    }
	    break;
	  case GeomAbs_Parabola :
	  case GeomAbs_Hyperbola :
	  case GeomAbs_BezierCurve :
	  case GeomAbs_BSplineCurve :
	  case GeomAbs_OtherCurve :
	    for (j = 1; j <= myDiscret/2; j++) {

	      PlotCount = 0;

	      PlotIso ( S, T, U1, V1,(T == GeomAbs_IsoV) ? stepU*2. : stepV*2., halt, Pts, Cells);
	      U1 += stepU*2.;
	      V1 += stepV*2.;
	    }
	    break;
	  }
	}
      }
    }
    S.D0(U2,V2,P);
    DrawTo(P,Pts,Cells);
  }  
}

//=======================================================================
// Function : PlotIso
// Purpose  : Plot iso for other surface
//=======================================================================

void GEOM_OCCReader::PlotIso (BRepAdaptor_Surface& S, 
				GeomAbs_IsoType T,
				Standard_Real& U, 
				Standard_Real& V, 
				Standard_Real Step, 
				Standard_Boolean& halt,
				vtkPoints* Pts,
				vtkCellArray* Cells)
{

  ++PlotCount; 

  gp_Pnt Pl, Pr, Pm;

  if (T == GeomAbs_IsoU) {
    S.D0(U, V, Pl);
    S.D0(U, V + Step/2., Pm);
    S.D0(U, V + Step, Pr);
  } else {
    S.D0(U, V, Pl);
    S.D0(U + Step/2., V, Pm);
    S.D0(U + Step, V, Pr);
  }

  if (PlotCount > MaxPlotCount) {
    DrawTo(Pr,Pts,Cells);
    return;
  }

  if (Pm.Distance(Pl) + Pm.Distance(Pr) <= IsoRatio*Pl.Distance(Pr)) {
    DrawTo(Pr,Pts,Cells);
  } else 
    if (T == GeomAbs_IsoU) {
      PlotIso ( S, T, U, V, Step/2, halt, Pts, Cells);
      Standard_Real aLocalV = V + Step/2 ;
      PlotIso ( S, T, U, aLocalV , Step/2, halt, Pts, Cells);
    } else {
      PlotIso ( S, T, U, V, Step/2, halt, Pts, Cells);
      Standard_Real aLocalU = U + Step/2 ;
      PlotIso ( S, T, aLocalU , V, Step/2, halt, Pts, Cells);
    }
}

//=======================================================================
// Function : TransferEdgeWData
// Purpose  : Transfert wireframe data for EDGE
//=======================================================================

void GEOM_OCCReader::TransferEdgeWData(const TopoDS_Edge& aEdge,
					 vtkPoints* Pts,
					 vtkCellArray* Cells) {
  
  
  Handle(Poly_PolygonOnTriangulation) aEdgePoly;
  Standard_Integer i = 1;
  Handle(Poly_Triangulation) T;
  TopLoc_Location aEdgeLoc;
  BRep_Tool::PolygonOnTriangulation(aEdge, aEdgePoly, T, aEdgeLoc, i);
  
  Handle(Poly_Polygon3D) P;
  if(aEdgePoly.IsNull()) {
    P = BRep_Tool::Polygon3D(aEdge, aEdgeLoc);
  }

  if(P.IsNull() && aEdgePoly.IsNull())
    return;
  
  // Location edges
  //---------------
  
  gp_Trsf edgeTransf;
  Standard_Boolean isidtrsf = true;
  if(!aEdgeLoc.IsIdentity())  {
    isidtrsf = false;
    edgeTransf = aEdgeLoc.Transformation();
  }

  Standard_Integer nbnodes;
  if (aEdgePoly.IsNull()) {
    nbnodes = P->NbNodes();
    const TColgp_Array1OfPnt& theNodesP = P->Nodes();

    float coord[3];
    int pts[2];
  
    for(int j=1;j<nbnodes;j++) {
      gp_Pnt pt1 = theNodesP(j);
      gp_Pnt pt2 = theNodesP(j+1);
    
      if(!isidtrsf) {
	// apply edge transformation
	pt1.Transform(edgeTransf);
	pt2.Transform(edgeTransf);
      }
      
      // insert pt1
      coord[0] = pt1.X(); coord[1] = pt1.Y(); coord[2] = pt1.Z();
      pts[0] = Pts->InsertNextPoint(coord);
      
      // insert pt2
      coord[0] = pt2.X(); coord[1] = pt2.Y(); coord[2] = pt2.Z();
      pts[1] = Pts->InsertNextPoint(coord);
      
      // insert line (pt1,pt2)
      Cells->InsertNextCell(2,pts);
    }
  } else {
    nbnodes = aEdgePoly->NbNodes();
    const TColStd_Array1OfInteger& Nodesidx = aEdgePoly->Nodes();
    const TColgp_Array1OfPnt& theNodesPoly = T->Nodes();

    float coord[3];
    int pts[2];
    
    for(int j=1;j<nbnodes;j++) {
      Standard_Integer id1 = Nodesidx(j);
      Standard_Integer id2 = Nodesidx(j+1);
      
      gp_Pnt pt1 = theNodesPoly(id1);
      gp_Pnt pt2 = theNodesPoly(id2);
          
      if(!isidtrsf) {
	// apply edge transformation
	pt1.Transform(edgeTransf);
	pt2.Transform(edgeTransf);
      }
      
      // insert pt1
      coord[0] = pt1.X(); coord[1] = pt1.Y(); coord[2] = pt1.Z();
      pts[0] = Pts->InsertNextPoint(coord);
      
      // insert pt2
      coord[0] = pt2.X(); coord[1] = pt2.Y(); coord[2] = pt2.Z();
      pts[1] = Pts->InsertNextPoint(coord);
      
      // insert line (pt1,pt2)
      Cells->InsertNextCell(2,pts);
    }
  }
}

/*  Standard_Integer nbnodes = aEdgePoly->NbNodes();
  const TColStd_Array1OfInteger& Nodesidx = aEdgePoly->Nodes();
  const TColgp_Array1OfPnt& theNodes = T->Nodes();
    
  float coord[3];
  int pts[2];
    

  // PUSH NODES
  for(i=1;i<=nbnodes;i++) {
    Standard_Integer id = Nodesidx(i);
    gp_Pnt pt = theNodes(id);
 
    float coord[3];
    if(!isidtrsf) pt.Transform(edgeTransf);

    coord[0] = pt.X(); coord[1] = pt.Y(); coord[2] = pt.Z();

    Pts->SetPoint(id-1,coord);

  }

  // PUSH EDGES
  for(i=1;i<nbnodes;i++) {
      
    Standard_Integer id1 = Nodesidx(i);
    Standard_Integer id2 = Nodesidx(i+1);
    
    int pts[2];
    pts[0] = id1-1; pts[1] = id2-1;

    // insert line (pt1,pt2)
    Cells->InsertNextCell(2,pts);
  }
 
  
  }*/

//=======================================================================
// Function : TransferVertexWData
// Purpose  : Transfert wireframe data for VERTEX
//=======================================================================

void GEOM_OCCReader::TransferVertexWData(const TopoDS_Vertex& aVertex,
					 vtkPoints* Pts,
					 vtkCellArray* Cells) {
#define ZERO_COORD coord[0] = 0.0; coord[1] = 0.0; coord[2] = 0.0
  
  gp_Pnt P = BRep_Tool::Pnt( aVertex );
  float delta = 1, coord[3];
  int pts[2];
  // insert pt
  ZERO_COORD; coord[0] = +delta;
  pts[0] = Pts->InsertNextPoint(coord);
  coord[0] = -delta;
  pts[1] = Pts->InsertNextPoint(coord);
  // insert line (pt1,pt2)
  Cells->InsertNextCell(2,pts);

  ZERO_COORD; coord[1] = +delta;
  pts[0] = Pts->InsertNextPoint(coord);
  coord[1] = -delta;
  pts[1] = Pts->InsertNextPoint(coord);
  // insert line (pt1,pt2)
  Cells->InsertNextCell(2,pts);

  ZERO_COORD; coord[2] = +delta;
  pts[0] = Pts->InsertNextPoint(coord);
  coord[2] = -delta;
  pts[1] = Pts->InsertNextPoint(coord);
  // insert line (pt1,pt2)
  Cells->InsertNextCell(2,pts);

#undef ZERO_COORD
}	

//=======================================================================
// Function : TransferEdgeSData(
// Purpose  : Transfert shading data for EDGE
//=======================================================================

void GEOM_OCCReader::TransferEdgeSData(const TopoDS_Edge& aFace,
					 vtkPoints* Pts,
					 vtkCellArray* Cells) 
{
}


//=======================================================================
// Function : TransferFaceSData
// Purpose  : Transfert shading data for FACE
//=======================================================================
void GEOM_OCCReader::TransferFaceSData(const TopoDS_Face& aFace,
					 vtkPoints* Pts,
					 vtkCellArray* Cells) {

  TopLoc_Location aLoc;
  Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
  if(aPoly.IsNull()) return;
  else {
    
    gp_Trsf myTransf;
    Standard_Boolean identity = true;
    if(!aLoc.IsIdentity())  {
      identity = false;
      myTransf = aLoc.Transformation();
    }

    Standard_Integer nbNodesInFace = aPoly->NbNodes();
    Standard_Integer nbTriInFace = aPoly->NbTriangles();
		
    const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
    const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
      
    Standard_Integer i;
    for(i=1;i<=nbNodesInFace;i++) {
      gp_Pnt P = Nodes(i);
      float coord[3];
      if(!identity) P.Transform(myTransf);
      coord[0] = P.X(); coord[1] = P.Y(); coord[2] = P.Z();
      Pts->SetPoint(i-1,coord);
    }

    for(i=1;i<=nbTriInFace;i++) {
      // Get the triangle
	
      Standard_Integer N1,N2,N3;
      Triangles(i).Get(N1,N2,N3);
	
      int pts[3];
      pts[0] = N1-1; pts[1] = N2-1; pts[2] = N3-1;
      Cells->InsertNextCell(3,pts);

    }
  } 
}

//=======================================================================
// Function : ComputeShading
// Purpose  : Compute the shape in shading mode
//=======================================================================
void GEOM_OCCReader::ComputeShading(vtkPoints* Pts,vtkCellArray* Cells){

  // Check the type of the shape:
  if(myShape.ShapeType() == TopAbs_FACE) {
    // Face
    TransferFaceSData(TopoDS::Face(myShape),Pts,Cells);
  }
  else {
    if(myShape.ShapeType() == TopAbs_EDGE) {
      // Edge
      TransferEdgeSData(TopoDS::Edge(myShape),Pts,Cells);
    }
    else {
    }

  } 

}

//=======================================================================
// Function : 
// Purpose  : Set parameters
//=======================================================================
void GEOM_OCCReader::setDisplayMode(int thenewmode) {
  amode = thenewmode;
}

void GEOM_OCCReader::setTopo(const TopoDS_Shape& aShape) {
  myShape = aShape;
}

void GEOM_OCCReader::setForceUpdate(Standard_Boolean bol) {
  forced = bol;
}

//=======================================================================
// Function : 
// Purpose  : Get parameters
//=======================================================================
const TopoDS_Shape& GEOM_OCCReader::getTopo() {
  return myShape;
}

int GEOM_OCCReader::getDisplayMode() {
  return amode;
}


