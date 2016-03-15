// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_Actor.h
//  Author : Christophe ATTANASIO
//  Module : GEOM
//
#ifndef GEOM_ACTOR_H
#define GEOM_ACTOR_H

#include "GEOM_OBJECT_defs.hxx"
#include "GEOM_SmartPtr.h"

#include <SALOME_Actor.h>

#include <TopoDS_Shape.hxx>
#include <vtkSmartPointer.h>

class vtkCamera;

class GEOM_VertexSource;
typedef GEOM_SmartPtr<GEOM_VertexSource> PVertexSource;

class GEOM_EdgeSource;
typedef GEOM_SmartPtr<GEOM_EdgeSource> PEdgeSource;

class GEOM_WireframeFace;
typedef GEOM_SmartPtr<GEOM_WireframeFace> PWFaceSource;

class GEOM_ShadingFace;
typedef GEOM_SmartPtr<GEOM_ShadingFace> PSFaceSource;

class vtkRenderer;
class vtkTextActor;

class vtkAppendPolyData;
typedef GEOM_SmartPtr<vtkAppendPolyData> PAppendFilter;

class GEOM_OBJECT_EXPORT GEOM_Actor: public SALOME_Actor
{
public:
  vtkTypeMacro(GEOM_Actor,SALOME_Actor);
  static GEOM_Actor* New();

  void SetShape(const TopoDS_Shape& theShape,
                double theDeflection,
                bool theIsVector = false);

  void SetDeflection(double theDeflection);
  double GetDeflection() const{ return myDeflection;}

  void AddToRender(vtkRenderer* theRenderer);
  void RemoveFromRender(vtkRenderer* theRenderer);

  enum EDisplayMode{ eWireframe, eShading, eShadingWithEdges = eShading + 2 };

/*   void SetDisplayMode(EDisplayMode theMode);  */
/*   EDisplayMode GetDisplayMode() const { return myDisplayMode;}  */

  void SetSelected(bool theIsSelected);
  bool IsSelected() const { return myIsSelected;}

  // OLD METHODS
  // Properties
  void SetHighlightProperty(vtkProperty* Prop);
  void SetWireframeProperty(vtkProperty* Prop);
  void SetShadingProperty(vtkProperty* Prop);

  vtkProperty* GetWireframeProperty();
  vtkProperty* GetShadingProperty();
  vtkProperty* GetIsolatedEdgeProperty();
  vtkProperty* GetVertexProperty();
  vtkProperty* GetStandaloneVertexProperty();
  vtkProperty* GetSharedEdgeProperty();
  vtkProperty* GetFaceEdgeProperty();

  virtual void setDisplayMode(int thenewmode);

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
  void setInputShape(const TopoDS_Shape& ashape, double adef1,
                     int imode, bool isVector = false);
  double isVector();

  // SubShape
  void SubShapeOn();
  void SubShapeOff();

  // Highlight
  virtual void highlight(bool theHighlight);
  virtual bool hasHighlight() { return true; }

  void ShallowCopy(vtkProp *prop);

  // Opacity
  void SetOpacity(double opa);
  double GetOpacity();

  // Color (same to all sub-actors/display modes)
  void SetColor(double r,  double g,  double b);
  void GetColor(double& r, double& g, double& b);
  // Color of points
  void SetPointColor(double r, double g, double b);
  // Color of standalone edges, wires, vectors
  void SetIsolatedEdgeColor(double r, double g, double b);
  // Color of shared edges
  void SetSharedEdgeColor(double r, double g, double b);
  // Color of free edges
  void SetFreeEdgeColor(double r, double g, double b);
  // Color of edges in shading+edges display mode
  void SetEdgesInShadingColor(double r, double g, double b);
  // Color of iso-lines
  void SetIsosColor(double r, double g, double b);
  // Color of labels
  void SetLabelColor(double r, double g, double b);


  // Material
  void SetMaterial(std::vector<vtkProperty*> theProps);
  vtkProperty* GetFrontMaterial();
  vtkProperty* GetBackMaterial();
  virtual bool IsInfinitive();

  // overloaded functions
  //! To map current selection to VTK representation
  virtual
  void
  Highlight(bool theHighlight);

  //----------------------------------------------------------------------------
  //! To process prehighlight (called from #SVTK_InteractorStyle)
  virtual
  bool
  PreHighlight(vtkInteractorStyle* theInteractorStyle,
               SVTK_SelectionEvent* theSelectionEvent,
               bool theIsHighlight);

  //! To process highlight (called from #SVTK_InteractorStyle)
  virtual
  bool
  Highlight(vtkInteractorStyle* theInteractorStyle,
            SVTK_SelectionEvent* theSelectionEvent,
            bool theIsHighlight);

  //! Visibility management
  virtual
  void
  SetVisibility( int );

  //! IsoLines management
  // theNb[0] - number of U lines
  // theNb[1] - number of V lines
  virtual
  void
  SetNbIsos(const int theNb[2]);

  virtual
  void
  GetNbIsos(int &theNbU,int &theNbV);
  
  virtual 
  void SetIsosWidth(const int width);

  int GetIsosWidth() const;

  virtual void SetWidth(const int width);

  int GetWidth() const;
  
  //! Vector mode management
  virtual
  void
  SetVectorMode(const bool theMode);

  virtual
  bool
  GetVectorMode();

  //! Vertices mode management
  virtual
  void
  SetVerticesMode(const bool theMode);

  virtual
  bool
  GetVerticesMode();

  //! Name mode management
  virtual
  void
  SetNameMode(const bool theMode);

  virtual
  bool
  GetNameMode();

protected:
  void SetModified();

  void GetMatrix(vtkCamera* theCam, vtkMatrix4x4 *result);
  void SetShapeName();

  GEOM_Actor();
  ~GEOM_Actor();

private:
  TopoDS_Shape myShape;
  bool isOnlyVertex;

  double myDeflection;
  bool myIsForced;

  //  EDisplayMode myDisplayMode;
  bool myIsSelected;
  bool myVectorMode;
  bool myVerticesMode;
  bool myNameMode;

  PDeviceActor myVertexActor;
  PVertexSource myVertexSource;

  PDeviceActor myStandaloneVertexActor;
  PVertexSource myStandaloneVertexSource;

  PDeviceActor myIsolatedEdgeActor;
  PEdgeSource myIsolatedEdgeSource;

  PDeviceActor myOneFaceEdgeActor;
  PEdgeSource myOneFaceEdgeSource;

  PDeviceActor mySharedEdgeActor;
  PEdgeSource mySharedEdgeSource;

  PDeviceActor myWireframeFaceActor;
  PWFaceSource myWireframeFaceSource;

  PDeviceActor myShadingFaceActor;
  PSFaceSource myShadingFaceSource;

  PDeviceActor myHighlightActor;

  vtkTextActor* myTextActor;

  vtkSmartPointer<vtkProperty>  myHighlightProp;
  vtkSmartPointer<vtkProperty>  myPreHighlightProp;
  vtkSmartPointer<vtkProperty>  myShadingFaceProp;
  vtkSmartPointer<vtkProperty>  myShadingBackFaceProp;

  PAppendFilter myAppendFilter;
  PPolyGeomPainterDataMapper myPolyDataMapper;

  virtual void SetMapper(vtkMapper*);

  GEOM_Actor(const GEOM_Actor&);
  void operator=(const GEOM_Actor&);

  double myEdgesInWireframeColor[3];
  double myEdgesInShadingColor[3];
  double myIsolatedEdgeColor[3];
  double mySharedEdgeColor[3];
  double myOneFaceEdgeColor[3];
};

#endif //GEOM_ACTOR_H


