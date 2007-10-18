#ifndef GEOM_SMARTPTR_H 
#define GEOM_SMARTPTR_H 
 
#include <vtkSmartPointer.h> 
 
template <class T>
class GEOM_SmartPtr: public vtkSmartPointer<T>
{
public:
  GEOM_SmartPtr() {}
  GEOM_SmartPtr(T* r, bool theIsOwner = false): vtkSmartPointer<T>(r) { 
    if(r && theIsOwner) 
      r->Delete();
  }
  GEOM_SmartPtr& operator()(T* r, bool theIsOwner = false){ 
    vtkSmartPointer<T>::operator=(r); 
    if(r && theIsOwner) 
      r->Delete();
    return *this;
  }
  GEOM_SmartPtr& operator=(T* r){ vtkSmartPointer<T>::operator=(r); return *this;}
  T* Get() const { return this->GetPointer();} 
}; 
 
 
class GEOM_DeviceActor; 
typedef GEOM_SmartPtr<GEOM_DeviceActor> PDeviceActor; 
 
class vtkPolyDataMapper; 
typedef GEOM_SmartPtr<vtkPolyDataMapper> PPolyDataMapper; 
 
 
#endif //GEOM_SMARTPTR_H 
