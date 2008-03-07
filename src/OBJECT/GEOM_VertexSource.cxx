#include "GEOM_VertexSource.h" 
 
#include <vtkObjectFactory.h> 

#include <vtkPoints.h> 
#include <vtkCellArray.h> 
#include <vtkPolyData.h> 
#include <vtkPolyDataMapper.h> 
 
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
 
vtkStandardNewMacro(GEOM_VertexSource);
 
GEOM_VertexSource::GEOM_VertexSource() 
{ 
} 
 
GEOM_VertexSource::~GEOM_VertexSource() 
{ 
} 
 
void  
GEOM_VertexSource:: 
AddVertex(const TopoDS_Vertex& theVertex) 
{ 
  myVertexSet.Add(theVertex); 
} 
 
void
GEOM_VertexSource:: 
Execute()
{
  vtkPolyData* aPolyData = GetOutput();
  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints(aPts);
  aPts->Delete();

  TVertexSet::Iterator anIter(myVertexSet);
  for(; anIter.More(); anIter.Next()){
    const TopoDS_Vertex& aVertex = anIter.Value();
    OCC2VTK(aVertex,aPolyData,aPts);
  }
}

void  
GEOM_VertexSource:: 
OCC2VTK(const TopoDS_Vertex& theVertex,  
        vtkPolyData* thePolyData, 
        vtkPoints* thePts) 
{ 
  gp_Pnt aPnt = BRep_Tool::Pnt(theVertex);
  vtkIdType anId = thePts->InsertNextPoint(aPnt.X(),aPnt.Y(),aPnt.Z());
  thePolyData->InsertNextCell(VTK_VERTEX,1,&anId);
}
