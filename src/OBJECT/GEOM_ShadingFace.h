#ifndef GEOM_SHADINGFACE_H 
#define GEOM_SHADINGFACE_H 
 
#include "GEOM_FaceSource.h" 
 
 
class VTK_EXPORT GEOM_ShadingFace: public GEOM_FaceSource 
{ 
public: 
  vtkTypeMacro(GEOM_ShadingFace,GEOM_FaceSource); 
  static GEOM_ShadingFace* New(); 
 
  static  
  void OCC2VTK(const TopoDS_Face& theFace,  
               vtkPolyData* theCells, 
               vtkPoints* thePts); 
 
protected: 
  void Execute(); 
 
  GEOM_ShadingFace(); 
  ~GEOM_ShadingFace(); 
 
private: 
  // Not implememnted 
  GEOM_ShadingFace(const GEOM_ShadingFace&); 
  void operator=(const GEOM_ShadingFace&); 
}; 
 
 
#endif //GEOM_SHADINGFACE_H 
