#ifndef GEOM_EDGESOURCE_H 
#define GEOM_EDGESOURCE_H 
 
#include "GEOM_DeviceActor.h" 
 
#include <TopoDS_Edge.hxx> 
#include <NCollection_Set.hxx> 
 
typedef NCollection_Set<TopoDS_Edge> TEdgeSet; 
 
#include <vtkPolyDataSource.h> 
 
 
class VTK_EXPORT GEOM_EdgeSource: public vtkPolyDataSource 
{ 
public: 
  vtkTypeMacro(GEOM_EdgeSource,vtkPolyDataSource); 
  static GEOM_EdgeSource* New(); 
 
  void AddEdge (const TopoDS_Edge& theEdge,
                bool theIsVector = false);
  void Clear(){ myEdgeSet.Clear();} 
 
  static  
  void OCC2VTK(const TopoDS_Edge& theEdge,  
               vtkPolyData* thePolyData, 
               vtkPoints* thePts,
               bool theIsVector = false); 
 
protected: 
  TEdgeSet myEdgeSet;
  // The <myIsVector> flag is common for all edges, because the shape,
  // representing a vector, can have only one edge.
  bool myIsVector;
 
  void Execute(); 
 
  GEOM_EdgeSource(); 
  ~GEOM_EdgeSource(); 
 
private: 
  // Not implememnted 
  GEOM_EdgeSource(const GEOM_EdgeSource&); 
  void operator=(const GEOM_EdgeSource&); 
}; 
 
 
#endif //GEOM_EDGESOURCE_H 
