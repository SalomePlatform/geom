#include "GEOM_DeviceActor.h" 

#include <vtkObjectFactory.h> 
 
#include <vtkPoints.h> 
#include <vtkPolyData.h> 
#include <vtkStripper.h>  
#include <vtkPolyDataMapper.h>  
#include <vtkPolyDataNormals.h>  
#include <vtkActor.h> 
 
#include <vtkRenderer.h> 
 
vtkStandardNewMacro(GEOM_DeviceActor);
 
GEOM_DeviceActor::GEOM_DeviceActor(): 
  myStripper(vtkStripper::New(),true), 
  myPolyDataMapper(vtkPolyDataMapper::New(),true), 
  myPolyDataNormals(vtkPolyDataNormals::New(),true), 
  myActor(vtkActor::New(),true) 
{ 
  myStripper->SetInput(myPolyDataNormals->GetOutput()); 
  myPolyDataMapper->SetInput(myStripper->GetOutput()); 
 
  myActor->SetMapper(myPolyDataMapper.Get()); 
  myActor->PickableOff(); 
} 
 
GEOM_DeviceActor::~GEOM_DeviceActor() 
{ 
} 
 
void 
GEOM_DeviceActor:: 
SetInput(vtkPolyData* thePolyData, bool theUseStripper)
{ 
  if(theUseStripper)
    myPolyDataNormals->SetInput(thePolyData); 
  else 
    myPolyDataMapper->SetInput(thePolyData); 
}
 
void 
GEOM_DeviceActor:: 
SetProperty(vtkProperty* theProperty)
{
  myActor->SetProperty(theProperty);
}
 
vtkProperty* 
GEOM_DeviceActor:: 
GetProperty()
{
  return myActor->GetProperty();
}
 
void 
GEOM_DeviceActor:: 
SetVisibility(int theVisibility)
{
  myActor->SetVisibility(theVisibility);
}
 
int 
GEOM_DeviceActor:: 
GetVisibility()
{
  return myActor->GetVisibility();
}
 
void 
GEOM_DeviceActor:: 
AddToRender(vtkRenderer* theRenderer)
{
  theRenderer->AddActor(myActor.GetPointer());
}

void 
GEOM_DeviceActor:: 
RemoveFromRender(vtkRenderer* theRenderer)
{
  theRenderer->RemoveActor(myActor.GetPointer());
}
