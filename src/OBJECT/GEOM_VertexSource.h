#ifndef GEOM_VERTEXSOURCE_H 
#define GEOM_VERTEXSOURCE_H 
 
#include "GEOM_DeviceActor.h" 
 
#include <TopoDS_Vertex.hxx> 
#include <NCollection_Set.hxx> 
 
typedef NCollection_Set<TopoDS_Vertex> TVertexSet; 
 
#include <vtkPolyDataSource.h> 
 
 
class VTK_EXPORT GEOM_VertexSource: public vtkPolyDataSource 
{ 
public: 
  vtkTypeMacro(GEOM_VertexSource,vtkPolyDataSource); 
  static GEOM_VertexSource* New(); 
 
  void AddVertex(const TopoDS_Vertex& theVertex); 
  void Clear(){ myVertexSet.Clear();} 
 
  static  
  void OCC2VTK(const TopoDS_Vertex& theVertex,  
               vtkPolyData* thePolyData, 
               vtkPoints* thePts); 
 
protected: 
  TVertexSet myVertexSet; 
 
  void Execute(); 
 
  GEOM_VertexSource(); 
  ~GEOM_VertexSource(); 
 
private: 
  // Not implememnted 
  GEOM_VertexSource(const GEOM_VertexSource&); 
  void operator=(const GEOM_VertexSource&); 
}; 
 
 
#endif //GEOM_VERTEXSOURCE_H 
