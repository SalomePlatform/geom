// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "GEOM_DeviceActor.h" 

#include <vtkObjectFactory.h> 
 
#include <vtkPoints.h> 
#include <vtkPolyData.h> 
#include <vtkStripper.h>  
#include <vtkPolyDataMapper.h>  
#include <vtkPolyDataNormals.h>  
#include <vtkActor.h> 
#include <VTKViewer_Actor.h>
#include <vtkRenderer.h> 
 
vtkStandardNewMacro(GEOM_DeviceActor);
 
GEOM_DeviceActor::GEOM_DeviceActor(): 
  myStripper(vtkStripper::New(),true), 
  myPolyDataMapper(vtkPolyDataMapper::New(),true), 
  myPolyDataNormals(vtkPolyDataNormals::New(),true), 
  myActor(VTKViewer_Actor::New(),true) 
{ 
  myStripper->SetInputConnection(myPolyDataNormals->GetOutputPort()); 
  myPolyDataMapper->SetInputConnection(myStripper->GetOutputPort()); 
 
  myActor->SetMapper(myPolyDataMapper.Get()); 
  myActor->PickableOff(); 
} 
 
GEOM_DeviceActor::~GEOM_DeviceActor() 
{ 
} 
 
void 
GEOM_DeviceActor:: 
SetInput(vtkAlgorithmOutput* thePolyData, bool theUseStripper)
{ 
  if(theUseStripper)
  {
    myPolyDataNormals->SetInputConnection(thePolyData); 
    myStripper->SetInputConnection(myPolyDataNormals->GetOutputPort()); 
    myPolyDataMapper->SetInputConnection(myStripper->GetOutputPort()); 
  }
  else 
    myPolyDataMapper->SetInputConnection(thePolyData); 
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
SetBackfaceProperty(vtkProperty* theProperty)
{
  myActor->SetBackfaceProperty(theProperty);
}
 
vtkProperty*
GEOM_DeviceActor::
GetBackfaceProperty()
{
  return myActor->GetBackfaceProperty();
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
