#include "GEOM_EdgeSource.h" 
 
#include <vtkObjectFactory.h> 

#include <vtkPoints.h> 
#include <vtkCellArray.h> 

#include <BRep_Tool.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
 
#include <vtkStripper.h>  
#include <vtkPolyData.h>  
 
vtkStandardNewMacro(GEOM_EdgeSource);
 
GEOM_EdgeSource::GEOM_EdgeSource() 
{ 
} 
 
GEOM_EdgeSource::~GEOM_EdgeSource() 
{ 
} 

void GEOM_EdgeSource::AddEdge (const TopoDS_Edge& theEdge,
                               bool theIsVector)
{
  myEdgeSet.Add(theEdge);
  myIsVector = theIsVector;
}

void
GEOM_EdgeSource:: 
Execute()
{
  vtkPolyData* aPolyData = GetOutput();
  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints(aPts);
  aPts->Delete();

  TEdgeSet::Iterator anIter (myEdgeSet);
  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Edge& anEdge = anIter.Value();
    OCC2VTK(anEdge,aPolyData,aPts,myIsVector);
  }
}

void GEOM_EdgeSource::OCC2VTK (const TopoDS_Edge& theEdge,  
                               vtkPolyData* thePolyData, 
                               vtkPoints* thePts,
                               bool theIsVector) 
{
  Handle(Poly_PolygonOnTriangulation) aEdgePoly;
  Standard_Integer i = 1;
  Handle(Poly_Triangulation) T;
  TopLoc_Location aEdgeLoc;
  BRep_Tool::PolygonOnTriangulation(theEdge, aEdgePoly, T, aEdgeLoc, i);

  Handle(Poly_Polygon3D) P;
  if(aEdgePoly.IsNull())
    P = BRep_Tool::Polygon3D(theEdge, aEdgeLoc);

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

  if (aEdgePoly.IsNull()) {
    Standard_Integer aNbNodes = P->NbNodes();
    const TColgp_Array1OfPnt& aNodesP = P->Nodes();

    aP1 = aNodesP(1);
    aP2 = aNodesP(aNbNodes);

    for (int j = 1; j < aNbNodes; j++) {
      gp_Pnt pt1 = aNodesP(j);
      gp_Pnt pt2 = aNodesP(j+1);

      if (!isidtrsf) {
        // apply edge transformation
        pt1.Transform(edgeTransf);
        pt2.Transform(edgeTransf);
      }

      float aCoord1[3] = {pt1.X(), pt1.Y(), pt1.Z()};
      vtkIdType anIds[2];
      anIds[0] = thePts->InsertNextPoint(aCoord1);

      float aCoord2[3] = {pt2.X(), pt2.Y(), pt2.Z()};
      anIds[1] = thePts->InsertNextPoint(aCoord2);

      thePolyData->InsertNextCell(VTK_LINE,2,anIds);
    }
  } else {
    Standard_Integer aNbNodes = aEdgePoly->NbNodes();
    const TColStd_Array1OfInteger& aNodeIds = aEdgePoly->Nodes();
    const TColgp_Array1OfPnt& anId2Pnts = T->Nodes();

    aP1 = anId2Pnts(1);
    aP2 = anId2Pnts(aNbNodes);

    for(int j = 1; j < aNbNodes; j++) {
      Standard_Integer id1 = aNodeIds(j);
      Standard_Integer id2 = aNodeIds(j+1);
      
      gp_Pnt pt1 = anId2Pnts(id1);
      gp_Pnt pt2 = anId2Pnts(id2);
          
      if(!isidtrsf) {
      	// apply edge transformation
        pt1.Transform(edgeTransf);
        pt2.Transform(edgeTransf);
      }
      
      float aCoord1[3] = {pt1.X(), pt1.Y(), pt1.Z()};
      vtkIdType anIds[2];
      anIds[0] = thePts->InsertNextPoint(aCoord1);

      float aCoord2[3] = {pt2.X(), pt2.Y(), pt2.Z()};
      anIds[1] = thePts->InsertNextPoint(aCoord2);

      thePolyData->InsertNextCell(VTK_LINE,2,anIds);
    }
  }

  // vector representation has an arrow on its end
  if (theIsVector)
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

    Standard_Real anAngle = PI/180.*5.;
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

    float coord[3] = {xo, yo, zo};

    vtkIdType ptLoc = thePts->InsertNextPoint(coord);
    vtkIdType ptFirst = 0;
    vtkIdType ptPrev = 0;
    vtkIdType ptCur = 0;

    vtkIdType pts[2];

    int NbPoints = 15;
    for (int i = 1; i <= NbPoints; i++, ptPrev = ptCur)
    {
      cosinus = cos(2. * PI / NbPoints * (i-1));   
      sinus   = sin(2. * PI / NbPoints * (i-1));

      gp_XYZ aP = aPc + (aDirI.XYZ() * cosinus + aDirJ.XYZ() * sinus) * aLength * Tg;
      coord[0] = aP.X();
      coord[1] = aP.Y();
      coord[2] = aP.Z();

      // insert pts
      ptCur = thePts->InsertNextPoint(coord);
      pts[0] = ptCur;

      if (i == 1) {
        ptFirst = ptCur;
      }
      else {
        // insert line (ptCur,ptPrev)
        pts[1] = ptPrev;
        thePolyData->InsertNextCell(VTK_LINE,2,pts);
      }

      // insert line (ptCur,ptLoc)
      pts[1] = ptLoc;
      thePolyData->InsertNextCell(VTK_LINE,2,pts);
    }

    // insert line (ptCur,ptFirst)
    pts[0] = ptCur;
    pts[1] = ptFirst;
    thePolyData->InsertNextCell(VTK_LINE,2,pts);
  }
}
