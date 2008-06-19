#ifndef GEOM_WIREFRAME_FACE_H 
#define GEOM_WIREFRAME_FACE_H 
 
#include "GEOM_FaceSource.h" 

#include <GeomAbs_IsoType.hxx>
#include <BRepAdaptor_Surface.hxx>
 
#include <vtkPolyDataSource.h> 
 
 
class VTK_EXPORT GEOM_WireframeFace: public GEOM_FaceSource 
{ 
public: 
  vtkTypeMacro(GEOM_WireframeFace,GEOM_FaceSource); 
  static GEOM_WireframeFace* New(); 
 
  /*  vtkSetMacro(NbIso,int); 
      vtkGetMacro(NbIso,int);*/
 
  vtkSetMacro(Discret,int); 
  vtkGetMacro(Discret,int); 
 
  static  
  void OCC2VTK(const TopoDS_Face& theFace,  
               vtkPolyData* thePolyData, 
               vtkPoints* thePts, 
               const int theNbIso[2], 
               const int theDiscret = 15);

  //! IsoLines management
  // theNb[0] - number of U lines
  // theNb[1] - number of V lines
  virtual void SetNbIso(const int theNb[2]);
  virtual void GetNbIso(int &theNbU,int &theNbV);

protected: 
  int NbIso[2], Discret; 
 
  static  
  void 
  CreateIso(const TopoDS_Face& theFace,
	    const int theNbIso[2], 
            const int theDiscret, 
            vtkPolyData* thePolyData,
            vtkPoints* thePts);
 
  static  
  void 
  CreateIso_(const TopoDS_Face& theFace,
             GeomAbs_IsoType theIsoType, 
             Standard_Real Par, 
             Standard_Real T1,
             Standard_Real T2,
             const int theDiscret, 
             vtkPolyData* thePolyData,
             vtkPoints* thePts);
 
  static  
  void 
  CreateIso__(const BRepAdaptor_Surface& theSurface, 
              GeomAbs_IsoType theIsoType,
		  		    Standard_Real& theU, 
			  	    Standard_Real& theV, 
				      Standard_Real theStep, 
              vtkPolyData* thePolyData,
              vtkPoints* thePts);

  void Execute(); 
 
  GEOM_WireframeFace(); 
  ~GEOM_WireframeFace(); 
 
private: 
  // Not implememnted 
  GEOM_WireframeFace(const GEOM_WireframeFace&); 
  void operator=(const GEOM_WireframeFace&); 
}; 
 
 
#endif //GEOM_WFACEACTOR_H 
