#ifndef GEOM_FACESOURCE_H 
#define GEOM_FACESOURCE_H 
 
#include "GEOM_DeviceActor.h" 
 
#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx> 
#include <NCollection_Set.hxx> 
 
typedef NCollection_Set<TopoDS_Face> TFaceSet; 
 
#include <vtkPolyDataSource.h> 
 
class GEOM_FaceSource; 
typedef GEOM_SmartPtr<GEOM_FaceSource> PFaceSource; 
 
 
class VTK_EXPORT GEOM_FaceSource: public vtkPolyDataSource 
{ 
public: 
  vtkTypeMacro(GEOM_FaceSource,vtkPolyDataSource); 
 
  void AddFace(const TopoDS_Face& theFace); 
  void Clear(){ myFaceSet.Clear();} 
 
protected: 
  TFaceSet myFaceSet; 
 
  static 
  void MoveTo(gp_Pnt thePnt, 
              vtkPoints* thePts);
  static 
  void DrawTo(gp_Pnt thePnt,
              vtkPolyData* thePolyData,
              vtkPoints* thePts); 
 
  GEOM_FaceSource(); 
  ~GEOM_FaceSource(); 
 
private: 
  // Not implememnted 
  GEOM_FaceSource(const GEOM_FaceSource&); 
  void operator=(const GEOM_FaceSource&); 
}; 
 
 
#endif //GEOM_FACESOURCE_H 
