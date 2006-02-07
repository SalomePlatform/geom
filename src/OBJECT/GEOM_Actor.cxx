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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_Actor.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

/*!
  \class GEOM_Actor GEOM_Actor.h
  \brief This class allows to display an OpenCASCADE CAD model in a VTK viewer.
*/

#include "GEOM_Actor.h"

#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>

#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

// OpenCASCADE Includes
#include "GEOM_OCCReader.h"
#include <BRep_Tool.hxx>

using namespace std;

//-------------------------------------------------------------
// Main methods
//-------------------------------------------------------------


GEOM_Actor* GEOM_Actor::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("GEOM_Actor");
  if(ret)
    {
      return (GEOM_Actor*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new GEOM_Actor;
}


GEOM_Actor::GEOM_Actor()
{
  this->Device = vtkActor::New();

  this->WireframeMapper = NULL;
  this->ShadingMapper = NULL;

  this->ShadingProperty = NULL;
  this->WireframeProperty = NULL;

  this->deflection = 0;
  myDisplayMode = 0; 

  this->myIO = NULL;
  this->myName = "";

  this->HighlightProperty = NULL;
  this->myIsHighlighted = false;

  this->subshape = false;
  this->myIsInfinite = false;
}

GEOM_Actor::~GEOM_Actor()
{
  if (WireframeMapper != NULL)
    WireframeMapper->Delete();
  if (ShadingMapper != NULL)
    ShadingMapper->Delete();
  if (ShadingProperty != NULL)
    ShadingProperty->Delete();
  if (WireframeProperty != NULL)
    WireframeProperty->Delete();
  if (HighlightProperty != NULL)
    HighlightProperty->Delete();
}


void GEOM_Actor::ShallowCopy(vtkProp *prop)
{
  GEOM_Actor *f = GEOM_Actor::SafeDownCast(prop);
  if ( f != NULL )
    {
      this->setInputShape(f->getTopo(),f->getDeflection(),f->getDisplayMode());
      this->setName( f->getName() );
      if ( f->hasIO() )
	this->setIO( f->getIO() );
      this->ShadingMapper = NULL;
      this->WireframeMapper = NULL;
    } else {
      this->myIO = NULL;
      this->myName = "";
      this->ShadingMapper = NULL;
      this->WireframeMapper = NULL;
    }

  // Now do superclass
  this->SALOME_Actor::ShallowCopy(prop);
}

//-------------------------------------------------------------
// Set parameters
//-------------------------------------------------------------


void GEOM_Actor::setDisplayMode(int thenewmode) {
  myDisplayMode = thenewmode;
  if ( thenewmode >=1 ) {
    if ((myShape.ShapeType() == TopAbs_WIRE) || 
	(myShape.ShapeType() == TopAbs_EDGE) || 
	(myShape.ShapeType() == TopAbs_VERTEX)) {
      if ( !subshape )
	CreateWireframeMapper();
      else
	return;
    } else
      CreateShadingMapper();
  } else
    CreateWireframeMapper();
}

void GEOM_Actor::setDeflection(double adef) {
  deflection = adef;
}

void GEOM_Actor::setInputShape(const TopoDS_Shape& aShape,double adef,int imode) {
  myShape = aShape;
  deflection = adef;
  setDisplayMode(imode);
}

//-------------------------------------------------------------
// Get parameters
//-------------------------------------------------------------

const TopoDS_Shape& GEOM_Actor::getTopo() {
  return myShape;
}

double GEOM_Actor::getDeflection() {
  return deflection;
}

void GEOM_Actor::SetWireframeProperty(vtkProperty* Prop) {
  this->WireframeProperty = Prop;
}

void GEOM_Actor::SetShadingProperty(vtkProperty* Prop) {
  this->ShadingProperty = Prop;
}


//-------------------------------------------------------------
// Mapper creating function
//-------------------------------------------------------------
void GEOM_Actor::CreateMapper(int theMode) {
  this->myIsInfinite = (bool)myShape.Infinite();  
  if(myShape.ShapeType() == TopAbs_VERTEX) {
    gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(myShape));
    this->SetPosition(aPnt.X(),aPnt.Y(),aPnt.Z());
  }
  GEOM_OCCReader* aread = GEOM_OCCReader::New();
  aread->setTopo(myShape);
  aread->setDisplayMode(theMode);
  aread->GetOutput()->ReleaseDataFlagOn(); 
    
  vtkPolyDataMapper* aMapper = vtkPolyDataMapper::New();
  if (theMode == 0) { 
    aMapper->SetInput(aread->GetOutput());
  } else {
    vtkPolyDataNormals *normals = vtkPolyDataNormals::New();
    normals->SetInput(aread->GetOutput());
    aMapper->SetInput(normals->GetOutput());
  }
  aread->Delete();
  this->SetMapper(theMode == 0? WireframeMapper = aMapper : ShadingMapper = aMapper);
}

void GEOM_Actor::CreateShadingMapper() {
  CreateMapper(1);
}


void GEOM_Actor::CreateWireframeMapper() {
  CreateMapper(0);
}

//-------------------------------------------------------------
// Render function
//-------------------------------------------------------------

void GEOM_Actor::Render(vtkRenderer *ren, vtkMapper *Mapper)
{
  /* render the property */
  if (!this->Property) {
    // force creation of a property
    this->GetProperty();
    this->Property->SetInterpolation(1);
    this->Property->SetRepresentationToSurface();
    this->Property->SetAmbient(0.3);
    this->Property->SetAmbientColor(0.88,0.86,0.2);
    this->Property->SetDiffuseColor(0.99,0.7,0.21);
    this->Property->SetSpecularColor(0.99,0.98,0.83);
  }

  if(!myIsHighlighted) {
    if ( myIsPreselected ) 
      this->Property = PreviewProperty;
    else if(myDisplayMode >= 1) {
      // SHADING
      this->Property = ShadingProperty;
    }
    else {
      this->Property = WireframeProperty;     
    }

  }

  this->Property->Render(this, ren);
  if (this->BackfaceProperty) {
    this->BackfaceProperty->BackfaceRender(this, ren);
    this->Device->SetBackfaceProperty(this->BackfaceProperty);
  }
  this->Device->SetProperty(this->Property);
  // Store information on time it takes to render.
  // We might want to estimate time from the number of polygons in mapper.
  if(myDisplayMode >= 1) {
    if((myShape.ShapeType() == TopAbs_WIRE) || 
       (myShape.ShapeType() == TopAbs_EDGE) || 
       (myShape.ShapeType() == TopAbs_VERTEX)) {
      if ( !subshape ) {
	if(WireframeMapper==NULL) CreateWireframeMapper();
      } else
	return;
    }
    else {
      if(ShadingMapper==NULL) CreateShadingMapper();
    }
  }
  else {
    if(WireframeMapper==NULL) CreateWireframeMapper();
  }
  if(myShape.ShapeType() == TopAbs_VERTEX) {
    if(ren){
      //The parameter determine size of vertex actor relate to diagonal of RendererWindow
      static float delta = 0.01;
      float X1 = -1, Y1 = -1, Z1 = 0;
      ren->ViewToWorld(X1,Y1,Z1);
      float X2 = +1, Y2 = +1, Z2 = 0;
      ren->ViewToWorld(X2,Y2,Z2);
      Z2 = sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) + (Z2-Z1)*(Z2-Z1));
      this->SetScale(Z2*delta);
    }
    vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
    this->GetMatrix(ren->GetActiveCamera(), aMatrix);
    this->Device->SetUserMatrix(aMatrix);
    this->Device->Render(ren,this->Mapper);
    aMatrix->Delete();    
  } else
    this->Device->Render(ren, this->Mapper);
 if(WireframeMapper!=NULL) this->EstimatedRenderTime = WireframeMapper->GetTimeToDraw(); 
 else if(ShadingMapper!=NULL) this->EstimatedRenderTime = ShadingMapper->GetTimeToDraw();
}

// SubShape
void GEOM_Actor::SubShapeOn()
{
  subshape = true;
}
void GEOM_Actor::SubShapeOff()
{
  subshape = false;
}

//-------------------------------------------------------------
// Opacity methods
//-------------------------------------------------------------

void GEOM_Actor::SetOpacity(float opa)
{
  //HighlightProperty->SetOpacity(opa);
  SALOME_Actor::SetOpacity(opa);
  ShadingProperty->SetOpacity(opa);
}

float GEOM_Actor::GetOpacity() {
  return ShadingProperty->GetOpacity();
}

//-------------------------------------------------------------
// Color methods
//-------------------------------------------------------------
void GEOM_Actor::SetColor(float r,float g,float b) {
  ShadingProperty->SetColor(r,g,b);  
}

void GEOM_Actor::GetColor(float& r,float& g,float& b) {
  float color[3];
  ShadingProperty->GetColor(color);
  r = color[0];
  g = color[1];
  b = color[2];
}

//-------------------------------------------------------------
// Highlight methods
//-------------------------------------------------------------

void GEOM_Actor::highlight(bool highlight) {

  if(highlight && !myIsHighlighted) {
    myIsHighlighted=true;
    // build highlight property is necessary
    if(HighlightProperty==NULL) {
      HighlightProperty = vtkProperty::New();
      HighlightProperty->SetAmbient(0.5);
      HighlightProperty->SetDiffuse(0.3);
      HighlightProperty->SetSpecular(0.2);
      HighlightProperty->SetRepresentationToSurface();
      HighlightProperty->SetAmbientColor(1, 1, 1);
      HighlightProperty->SetDiffuseColor(1, 1, 1);
      HighlightProperty->SetSpecularColor(0.5, 0.5, 0.5); 
    }
      
    this->Property = HighlightProperty;
 
  }
  else if (!highlight) {
    if(myIsHighlighted) {
      myIsHighlighted=false;
      if(myDisplayMode==1) {
	//unhilight in shading
	this->Property = ShadingProperty;
      }
      else {
	//unhilight in wireframe
	this->Property = WireframeProperty;
      }
    }
  }
}

void GEOM_Actor::SetHighlightProperty(vtkProperty* Prop) {
  this->HighlightProperty = Prop;
}


void GEOM_Actor::ReleaseGraphicsResources(vtkWindow *renWin) 
{
  vtkActor::ReleaseGraphicsResources(renWin);
  
  // broadcast the message down to the individual LOD mappers

  if(WireframeMapper) this->WireframeMapper->ReleaseGraphicsResources(renWin);
  if(ShadingMapper) this->ShadingMapper->ReleaseGraphicsResources(renWin);
}


// Copy the follower's composite 4x4 matrix into the matrix provided.
void GEOM_Actor::GetMatrix(vtkCamera* theCam, vtkMatrix4x4 *result)
{
  double *pos, *vup;
  double Rx[3], Ry[3], Rz[3], p1[3];
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  int i;
  double distance;
  
  this->GetOrientation();
  this->Transform->Push();  
  this->Transform->PostMultiply();  
  this->Transform->Identity();

  // apply user defined matrix last if there is one 
  if (this->UserMatrix)
    {
    this->Transform->Concatenate(this->UserMatrix);
    }

  this->Transform->Translate(-this->Origin[0],
                             -this->Origin[1],
                             -this->Origin[2]);
  // scale
  this->Transform->Scale(this->Scale[0],
                         this->Scale[1],
                         this->Scale[2]);
  
  // rotate
  this->Transform->RotateY(this->Orientation[1]);
  this->Transform->RotateX(this->Orientation[0]);
  this->Transform->RotateZ(this->Orientation[2]);

  if (theCam)
    {
    // do the rotation
    // first rotate y 
    pos = theCam->GetPosition();
    vup = theCam->GetViewUp();

    if (theCam->GetParallelProjection())
      {
      theCam->GetDirectionOfProjection(Rz);
      }
    else
      {
      distance = sqrt(
	(pos[0] - this->Position[0])*(pos[0] - this->Position[0]) +
        (pos[1] - this->Position[1])*(pos[1] - this->Position[1]) +
        (pos[2] - this->Position[2])*(pos[2] - this->Position[2]));
      for (i = 0; i < 3; i++)
	{
        Rz[i] = (pos[i] - this->Position[i])/distance;
	}
      }
  
    vtkMath::Cross(vup,Rz,Rx);
    vtkMath::Normalize(Rx);
    vtkMath::Cross(Rz,Rx,Ry);
    
    matrix->Element[0][0] = Rx[0];
    matrix->Element[1][0] = Rx[1];
    matrix->Element[2][0] = Rx[2];
    matrix->Element[0][1] = Ry[0];
    matrix->Element[1][1] = Ry[1];
    matrix->Element[2][1] = Ry[2];
    matrix->Element[0][2] = Rz[0];
    matrix->Element[1][2] = Rz[1];
    matrix->Element[2][2] = Rz[2];
    
    this->Transform->Concatenate(matrix);
    }
  
  // translate to projection reference point PRP
  // this is the camera's position blasted through
  // the current matrix
  p1[0] = this->Origin[0] + this->Position[0];
  p1[1] = this->Origin[1] + this->Position[1];
  p1[2] = this->Origin[2] + this->Position[2];

  this->Transform->Translate(p1[0],p1[1],p1[2]);
  this->Transform->GetMatrix(result);
  
  matrix->Delete();
  this->Transform->Pop();  
}  
