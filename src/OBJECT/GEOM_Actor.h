//  File      : GEOM_Actor.h
//  Created   : Wed Feb 20 17:24:59 2002
//  Author    : Christophe ATTANASIO
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$


#ifndef GEOM_ACTOR_H
#define GEOM_ACTOR_H

#include "SALOME_Actor.h"

//VTK
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkMatrix4x4.h>
#include <vtkCamera.h>

//OpenCASCADE
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>


class TopoDS_Shape;

#ifdef _WIN_32
#define VTKOCC_EXPORT __declspec (dllexport)
#else
#define VTKOCC_EXPORT
#endif
class VTKOCC_EXPORT GEOM_Actor : public SALOME_Actor {


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
  void setDisplayMode(int);

  // Highlight
  void highlight(Standard_Boolean highlight);
  bool hasHighlight();

  void ShallowCopy(vtkProp *prop);

  // Properties
  void SetHighlightProperty(vtkProperty* Prop);
  void SetWireframeProperty(vtkProperty* Prop);
  void SetShadingProperty(vtkProperty* Prop);

  // Opacity
  void SetOpacity(float opa);
  float GetOpacity();

  // Color
  void SetColor(float r,float g,float b);
  void GetColor(float& r,float& g,float& b);
  
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

  TopoDS_Shape myShape;
  double deflection;

  vtkMapper* ShadingMapper;
  vtkMapper* WireframeMapper;

  vtkProperty* ShadingProperty;
  vtkProperty* WireframeProperty;
  vtkProperty* HighlightProperty;

  int myDisplayMode;

};
#endif //GEOM_ACTOR_H
