// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_OCCReader.h
//  Author : Christophe ATTANASIO
//  Module : GEOM

#include "GEOM_OCCReader.h"

#include <GEOMUtils_Hatcher.hxx>

// VTK Includes
#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

// OpenCASCADE Includes
#include <Adaptor3d_HCurve.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <Precision.hxx>
#include <BRep_Tool.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include "utilities.h"


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
// Function : RequestData
// Purpose  : 
//=======================================================================

int GEOM_OCCReader::RequestData(vtkInformation *vtkNotUsed(request),
                                vtkInformationVector **inputVector,
                                vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

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
        return 0;
      }

      nbpts = aPoly->NbNodes();
      nbpoly = aPoly->NbTriangles();

      Pts->SetNumberOfPoints(nbpts);
      Cells->Allocate(Cells->EstimateSize(nbpoly,3));
    }
    else { 
        Cells->Delete();
        Pts->Delete();
        return 0; 
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
  return 1;
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
  createISO(aCopyFace,1,Pts,Cells);
}

//=======================================================================
// Function : createISO
// Purpose  : Create ISO for Face Wireframe representation 
//=======================================================================

void GEOM_OCCReader::createISO (const TopoDS_Face      &TopologicalFace,
                                const Standard_Integer  NbIsos,
                                      vtkPoints        *Pts,
                                      vtkCellArray     *Cell)
{
  GEOMUtils::Hatcher aHatcher(TopologicalFace);

  aHatcher.Init(NbIsos);
  aHatcher.Perform();

  if (aHatcher.IsDone()) {
    // Push iso lines in vtk kernel
    Standard_Integer  pt_start_idx = 0;

    createIsos(aHatcher, Standard_True, pt_start_idx, Pts, Cell);
    createIsos(aHatcher, Standard_False, pt_start_idx, Pts, Cell);
  }
}

//=======================================================================
// Function : createIsos
// Purpose  : Create isolines obtained from hatcher.
//=======================================================================
void GEOM_OCCReader::createIsos(const GEOMUtils::Hatcher &theHatcher,
                                const Standard_Boolean   IsUIso,
                                      Standard_Integer  &pt_start_idx,
                                      vtkPoints         *Pts,
                                      vtkCellArray      *Cell)
{
  // Push iso lines in vtk kernel
  Handle(TColStd_HArray1OfInteger) anIndices;
  Handle(TColStd_HArray1OfReal)    aParams;

  if (IsUIso) {
    // U-isolines
    anIndices = theHatcher.GetUIndices();
    aParams   = theHatcher.GetUParams();
  } else {
    // V-isolines
    anIndices = theHatcher.GetVIndices();
    aParams   = theHatcher.GetVParams();
  }

  if (anIndices.IsNull() || aParams.IsNull()) {
    if (IsUIso) {
      MESSAGE("GEOMUtils_Hatcher: null U-isoline indices")
    } else {
      MESSAGE("GEOMUtils_Hatcher: null V-isoline indices")
    }
  } else {
    const GeomAbs_IsoType aType    = (IsUIso ? GeomAbs_IsoU : GeomAbs_IsoV);
    Standard_Integer      anIsoInd = anIndices->Lower();

    for (; anIsoInd <= anIndices->Upper(); anIsoInd++) {
      const Standard_Integer aHatchingIndex = anIndices->Value(anIsoInd);

      if (aHatchingIndex != 0) {
        const Standard_Real    aParam     = aParams->Value(anIsoInd);
        const Standard_Integer aNbDomains =
          theHatcher.GetNbDomains(aHatchingIndex);

        if (aNbDomains < 0) {
          if (IsUIso) {
            MESSAGE("GEOMUtils_Hatcher: U iso of parameter: "<<aParam)
          } else {
            MESSAGE("GEOMUtils_Hatcher: V iso of parameter: "<<aParam)
          }

          switch (theHatcher.GetHatcher().Status (aHatchingIndex)) {
          case HatchGen_NoProblem          :
            MESSAGE("No Problem")          ; break ;
          case HatchGen_TrimFailure        :
            MESSAGE("Trim Failure")        ; break ;
          case HatchGen_TransitionFailure  :
            MESSAGE("Transition Failure")  ; break ;
          case HatchGen_IncoherentParity   :
            MESSAGE("Incoherent Parity")   ; break ;
          case HatchGen_IncompatibleStates :
            MESSAGE("Incompatible States") ; break ;
          }
        } else {
          Standard_Integer anIDom = 1;
          Standard_Real    aV1;
          Standard_Real    aV2;

          for (; anIDom <= aNbDomains; anIDom++) {
            if (theHatcher.GetDomain(aHatchingIndex, anIDom, aV1, aV2)) {
              DrawIso(aType, aParam, aV1, aV2, Pts, Cell,pt_start_idx);
            }
          }
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

  vtkIdType pts[2];
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

  gp_Pnt aP1, aP2;

  Standard_Integer nbnodes;
  if (aEdgePoly.IsNull()) {
    nbnodes = P->NbNodes();
    const TColgp_Array1OfPnt& theNodesP = P->Nodes();

    aP1 = theNodesP(1);
    aP2 = theNodesP(nbnodes);

    float coord[3];
    vtkIdType pts[2];

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

    aP1 = theNodesPoly(1);
    aP2 = theNodesPoly(nbnodes);

    float coord[3];
    vtkIdType pts[2];
    
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

  // vector representation has an arrow on its end
  if (myIsVector)
  {
    if (!isidtrsf) {
      // apply edge transformation
      aP1.Transform(edgeTransf);
      aP2.Transform(edgeTransf);
    }

    // draw an arrow
    gp_Vec aDirVec (aP1, aP2);
    Standard_Real aDist = aDirVec.Magnitude();
    if (aDist < gp::Resolution()) return;
    gp_Dir aDirection (aDirVec);

    Standard_Real anAngle = M_PI/180. * 5.;
    Standard_Real aLength = aDist/10.;

    Standard_Real dx,dy,dz;
    aDirection.Coord(dx,dy,dz);

    // Pointe de la fleche
    Standard_Real xo,yo,zo;
    aP2.Coord(xo,yo,zo);

    // Centre du cercle base de la fleche
    gp_XYZ aPc = aP2.XYZ() - aDirection.XYZ() * aLength;

    // Construction d'un repere i,j pour le cercle
    gp_Dir aDirN;
    if      (Abs(dx) <= Abs(dy) && Abs(dx) <= Abs(dz)) aDirN = gp::DX();
    else if (Abs(dy) <= Abs(dz) && Abs(dy) <= Abs(dx)) aDirN = gp::DY();
    else aDirN = gp::DZ();

    gp_Dir aDirI = aDirection ^ aDirN;
    gp_Dir aDirJ = aDirection ^ aDirI;

    // Add points and segments, composing the arrow
    Standard_Real cosinus, sinus, Tg = tan(anAngle);

    float coord[3];
    coord[0] = xo; coord[1] = yo; coord[2] = zo;

    int ptLoc = Pts->InsertNextPoint(coord);
    int ptFirst = 0;
    int ptPrev = 0;
    int ptCur = 0;

    vtkIdType pts[2];

    int NbPoints = 15;
    for (int i = 1; i <= NbPoints; i++, ptPrev = ptCur)
    {
      cosinus = cos(2. * M_PI / NbPoints * (i-1));   
      sinus   = sin(2. * M_PI / NbPoints * (i-1));

      gp_XYZ aP = aPc + (aDirI.XYZ() * cosinus + aDirJ.XYZ() * sinus) * aLength * Tg;
      coord[0] = aP.X();
      coord[1] = aP.Y();
      coord[2] = aP.Z();

      // insert pts
      ptCur = Pts->InsertNextPoint(coord);
      pts[0] = ptCur;

      if (i == 1) {
        ptFirst = ptCur;
      }
      else {
        // insert line (ptCur,ptPrev)
        pts[1] = ptPrev;
        Cells->InsertNextCell(2,pts);
      }

      // insert line (ptCur,ptLoc)
      pts[1] = ptLoc;
      Cells->InsertNextCell(2,pts);
    }

    // insert line (ptCur,ptFirst)
    pts[0] = ptCur;
    pts[1] = ptFirst;
    Cells->InsertNextCell(2,pts);
  }
}

/*  Standard_Integer nbnodes = aEdgePoly->NbNodes();
  const TColStd_Array1OfInteger& Nodesidx = aEdgePoly->Nodes();
  const TColgp_Array1OfPnt& theNodes = T->Nodes();
    
  float coord[3];
  vtkIdType pts[2];
    

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
    
    vtkIdType pts[2];
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
  vtkIdType pts[2];
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
        
      vtkIdType pts[3];
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

void GEOM_OCCReader::setTopo(const TopoDS_Shape& aShape, bool isVector) {
  myShape = aShape;
  myIsVector = isVector;
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
