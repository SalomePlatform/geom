//  GEOM OBJECT : interactive object for Geometry entities visualization
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOM_Actor.h
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

#ifndef GEOM_ACTOR_H
#define GEOM_ACTOR_H

#include "GEOM_OBJECT_defs.hxx"

#include "SALOME_Actor.h"

//OpenCASCADE
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>

class vtkCamera;
class TopoDS_Shape;

class GEOM_OBJECT_EXPORT GEOM_Actor : public SALOME_Actor {
 public:
  vtkTypeMacro(GEOM_Actor,SALOME_Actor);

  static GEOM_Actor* New();

  // Description:
  // This causes the actor to be rendered. It, in turn, will render the actor's
  // property and then mapper.
  virtual void Render(vtkRenderer *, vtkMapper *);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  const TopoDS_Shape& getTopo();
  void setInputShape(const TopoDS_Shape& ashape,double adef1,int imode);

  double getDeflection();
  void setDeflection(double adefl);

  // SubShape
  void SubShapeOn();
  void SubShapeOff();
  
  // Display Mode
  virtual void setDisplayMode(int);

  // Highlight
  virtual void highlight(bool theHighlight);
  virtual bool hasHighlight() { return true; }

  void ShallowCopy(vtkProp *prop);

  // Properties
  void SetHighlightProperty(vtkProperty* Prop);
  void SetWireframeProperty(vtkProperty* Prop);
  void SetShadingProperty(vtkProperty* Prop);

  // Opacity
  void SetOpacity(vtkFloatingPointType opa);
  vtkFloatingPointType GetOpacity();

  // Color
  void SetColor(vtkFloatingPointType r,vtkFloatingPointType g,vtkFloatingPointType b);
  void GetColor(vtkFloatingPointType& r,vtkFloatingPointType& g,vtkFloatingPointType& b);

  virtual bool IsInfinite() {return myIsInfinite;}  
 protected:

  GEOM_Actor();
  ~GEOM_Actor();
  GEOM_Actor(const GEOM_Actor&) {};
  void operator=(const GEOM_Actor&) {};

  void CreateWireframeMapper();
  void CreateShadingMapper();
  void CreateMapper(int theMode);
  void GetMatrix(vtkCamera* theCam, vtkMatrix4x4 *result);


 private:

  bool subshape;
  bool myIsInfinite;

  TopoDS_Shape myShape;
  double deflection;

  vtkMapper* ShadingMapper;
  vtkMapper* WireframeMapper;

  vtkProperty* ShadingProperty;
  vtkProperty* WireframeProperty;
  vtkProperty* HighlightProperty;
};


#endif //GEOM_ACTOR_H

