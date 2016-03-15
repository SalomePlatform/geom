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
 
//class vtkActor;
class VTKViewer_Actor;
typedef GEOM_SmartPtr<VTKViewer_Actor> PActor;
 
class vtkProperty; 
class vtkRenderer; 
 
#include <vtkObject.h> 
 
class vtkAlgorithmOutput;
 
class VTK_EXPORT GEOM_DeviceActor: public vtkObject 
{  
public: 
  vtkTypeMacro(GEOM_DeviceActor,vtkObject); 
  static GEOM_DeviceActor* New(); 

  void SetProperty(vtkProperty* theProperty);
  vtkProperty* GetProperty();

  void SetBackfaceProperty(vtkProperty* theProperty);
  vtkProperty* GetBackfaceProperty();

  void SetVisibility(int theVisibility); 
  int GetVisibility(); 
 
  void SetInput(vtkAlgorithmOutput* thePolyData, bool theUseStripper); 

  void AddToRender(vtkRenderer* theRenderer); 
  void RemoveFromRender(vtkRenderer* theRenderer);
  
  PActor GetDeviceActor() {return myActor;}
 
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
