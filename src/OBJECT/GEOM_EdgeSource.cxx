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
 
void  
GEOM_EdgeSource:: 
AddEdge(const TopoDS_Edge& theEdge) 
{ 
  myEdgeSet.Add(theEdge); 
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

  TEdgeSet::Iterator anIter(myEdgeSet);
  for(; anIter.More(); anIter.Next()){
    const TopoDS_Edge& anEdge = anIter.Value();
    OCC2VTK(anEdge,aPolyData,aPts);
  }
}

void  
GEOM_EdgeSource:: 
OCC2VTK(const TopoDS_Edge& theEdge,  
        vtkPolyData* thePolyData, 
        vtkPoints* thePts) 
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

  if (aEdgePoly.IsNull()) {
    Standard_Integer aNbNodes = P->NbNodes();
    const TColgp_Array1OfPnt& aNodesP = P->Nodes();

    for(int j = 1; j < aNbNodes; j++){
      gp_Pnt pt1 = aNodesP(j);
      gp_Pnt pt2 = aNodesP(j+1);
    
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
  } else {
    Standard_Integer aNbNodes = aEdgePoly->NbNodes();
    const TColStd_Array1OfInteger& aNodeIds = aEdgePoly->Nodes();
    const TColgp_Array1OfPnt& anId2Pnts = T->Nodes();

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
}