#include "GEOM_FaceSource.h" 
 
#include <vtkObjectFactory.h> 

#include <vtkPoints.h>
#include <vtkCellArray.h>
 
#include <vtkPolyDataMapper.h>  
#include <vtkPolyData.h>  
 
GEOM_FaceSource::GEOM_FaceSource() 
{ 
} 
 
GEOM_FaceSource::~GEOM_FaceSource() 
{ 
} 
 
void  
GEOM_FaceSource:: 
AddFace(const TopoDS_Face& theFace) 
{ 
  myFaceSet.Add(theFace); 
} 
 
void 
GEOM_FaceSource::
MoveTo(gp_Pnt thePnt, 
       vtkPoints* thePts)
{    
  thePts->InsertNextPoint(thePnt.X(), thePnt.Y(), thePnt.Z());
} 

void 
GEOM_FaceSource::
DrawTo(gp_Pnt thePnt,
       vtkPolyData* thePolyData,
       vtkPoints* thePts)
{
  vtkIdType anId = 
    thePts->InsertNextPoint(thePnt.X(), thePnt.Y(), thePnt.Z());
  vtkIdType anIds[2] = {anId-1, anId};
  thePolyData->InsertNextCell(VTK_LINE,2,anIds);
}
