#ifndef GEOM_DEVICEACTOR_H 
#define GEOM_DEVICEACTOR_H 
 
#include "GEOM_SmartPtr.h" 
 
class vtkPoints; 
typedef GEOM_SmartPtr<vtkPoints> PPoints; 
 
class vtkPolyData; 
typedef GEOM_SmartPtr<vtkPolyData> PPolyData; 
 
class vtkStripper; 
typedef GEOM_SmartPtr<vtkStripper> PStripper; 
 
class vtkPolyDataNormals; 
typedef GEOM_SmartPtr<vtkPolyDataNormals> PPolyDataNormals; 
 
class vtkActor; 
typedef GEOM_SmartPtr<vtkActor> PActor; 
 
class vtkProperty; 
class vtkRenderer; 
 
#include <vtkObject.h> 
 
 
class VTK_EXPORT GEOM_DeviceActor: public vtkObject 
{  
public: 
  vtkTypeMacro(GEOM_DeviceActor,vtkObject); 
  static GEOM_DeviceActor* New(); 

  void SetProperty(vtkProperty* theProperty);
  vtkProperty* GetProperty();
 
  void SetVisibility(int theVisibility); 
  int GetVisibility(); 
 
  void SetInput(vtkPolyData* thePolyData, bool theUseStripper); 

  void AddToRender(vtkRenderer* theRenderer); 
  void RemoveFromRender(vtkRenderer* theRenderer);
 
protected: 
  PPolyDataNormals myPolyDataNormals; 
  PPolyDataMapper myPolyDataMapper; 
  PStripper myStripper; 
  PActor myActor; 
 
  GEOM_DeviceActor(); 
  ~GEOM_DeviceActor(); 
 
private: 
  // Not implememnted 
  GEOM_DeviceActor(const GEOM_DeviceActor&); 
  void operator=(const GEOM_DeviceActor&); 
}; 
 
 
#endif //GEOM_DEVICEACTOR_H 
