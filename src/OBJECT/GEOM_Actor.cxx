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
//  File   : GEOM_Actor.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//
/*!
  \class GEOM_Actor GEOM_Actor.h
  \brief This class allows to display an OpenCASCADE CAD model in a VTK viewer.
*/
#include "GEOM_Actor.h" 
 
#include "GEOM_DeviceActor.h" 
#include "GEOM_VertexSource.h" 
#include "GEOM_EdgeSource.h" 
#include "GEOM_WireframeFace.h" 
#include "GEOM_ShadingFace.h"
#include "GEOM_PainterPolyDataMapper.h"
#include "GEOMUtils.hxx" 
#include "SVTK_Actor.h"

#include <OCC2VTK_Tools.h>
#include <GEOMUtils.hxx>

#include <vtkObjectFactory.h> 
#include <vtkRenderer.h> 
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h> 
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
 
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
 
#include <vtkPolyDataWriter.h> 
 
#include <vtkAppendPolyData.h>  
#include <vtkPolyDataMapper.h>  
#include <vtkPolyData.h>  
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkMath.h>
#include <vtkCamera.h>

#include "utilities.h"

#include "SALOME_InteractiveObject.hxx"

//vtkStandardNewMacro(GEOM_Actor);

#ifndef MYDEBUG
//#define MYDEBUG
#endif

GEOM_Actor::GEOM_Actor(): 
  isOnlyVertex(false),

  myDeflection(-1),
  myIsForced(false),

  //  myDisplayMode(eWireframe), 
  myIsSelected(false), 
  myVectorMode(false),
  myVerticesMode(false),
  myNameMode(false),

  myVertexActor(GEOM_DeviceActor::New(),true), 
  myVertexSource(GEOM_VertexSource::New(),true), 
 
  myStandaloneVertexActor(GEOM_DeviceActor::New(),true), 
  myStandaloneVertexSource(GEOM_VertexSource::New(),true), 

  myIsolatedEdgeActor(GEOM_DeviceActor::New(),true), 
  myIsolatedEdgeSource(GEOM_EdgeSource::New(),true), 
 
  myOneFaceEdgeActor(GEOM_DeviceActor::New(),true), 
  myOneFaceEdgeSource(GEOM_EdgeSource::New(),true), 
 
  mySharedEdgeActor(GEOM_DeviceActor::New(),true), 
  mySharedEdgeSource(GEOM_EdgeSource::New(),true), 
 
  myWireframeFaceActor(GEOM_DeviceActor::New(),true), 
  myWireframeFaceSource(GEOM_WireframeFace::New(),true), 
 
  myShadingFaceActor(GEOM_DeviceActor::New(),true), 
  myShadingFaceSource(GEOM_ShadingFace::New(),true), 
 
  myHighlightActor(GEOM_DeviceActor::New(),true), 
  myAppendFilter(vtkAppendPolyData::New(),true), 
  // Use mapper as an instance of GEOM_PainterPolyDataMapper class
  // to prevent drawing of mappers' content (due to an empty definition
  // of GEOM_PainterPolyDataMapper::RenderPiece(...)).
  // !!! Presentation of GEOM_Actor is drawing only with help of actors
  // defined in this class !!!
  myPolyDataMapper(GEOM_PainterPolyDataMapper::New(),true),

  myTextActor( vtkTextActor::New() ),

  myHighlightProp(vtkProperty::New()),
  myPreHighlightProp(vtkProperty::New()),
  myShadingFaceProp(vtkProperty::New()),
  myShadingBackFaceProp(vtkProperty::New())
{ 
#ifdef MYDEBUG
  MESSAGE (this<< " GEOM_Actor::GEOM_Actor");
#endif

  myPolyDataMapper->SetInputConnection(myAppendFilter->GetOutputPort()); 
  vtkProperty* aProperty; 

  myHighlightProp->SetAmbient(0.5);
  myHighlightProp->SetDiffuse(0.3);
  myHighlightProp->SetSpecular(0.2);
  myHighlightProp->SetRepresentationToSurface();
  myHighlightProp->SetAmbientColor(1, 1, 1);
  myHighlightProp->SetDiffuseColor(1, 1, 1);
  myHighlightProp->SetSpecularColor(0.5, 0.5, 0.5);
  myHighlightProp->SetPointSize(SALOME_POINT_SIZE+2);
  myHighlightActor->SetProperty(myHighlightProp.GetPointer());

  this->myHighlightActor->SetInput(myAppendFilter->GetOutputPort(),false);

  myPreHighlightProp->SetColor(0,1,1);
  myPreHighlightProp->SetPointSize(SALOME_POINT_SIZE);
  myPreHighlightProp->SetLineWidth(SALOME_LINE_WIDTH+1);
  myPreHighlightProp->SetRepresentationToWireframe();

  myAppendFilter->AddInputConnection(myVertexSource->GetOutputPort()); 
  myVertexActor->SetInput(myVertexSource->GetOutputPort(),false); 
  aProperty = myVertexActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_POINTS); 
  aProperty->SetPointSize(3); 
  aProperty->SetColor(1, 1, 0);
 
  myAppendFilter->AddInputConnection(myStandaloneVertexSource->GetOutputPort()); 
  myStandaloneVertexActor->SetInput(myStandaloneVertexSource->GetOutputPort(),false); 
  aProperty = myStandaloneVertexActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_POINTS); 
  aProperty->SetPointSize(3); 
  aProperty->SetColor(1, 1, 0);
 
  myAppendFilter->AddInputConnection(myIsolatedEdgeSource->GetOutputPort());
  myIsolatedEdgeActor->SetInput(myIsolatedEdgeSource->GetOutputPort(),false); 
  aProperty = myIsolatedEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  myIsolatedEdgeColor[0] = 1; myIsolatedEdgeColor[1] = 0; myIsolatedEdgeColor[2] = 0;
  aProperty->SetColor(myIsolatedEdgeColor[0], myIsolatedEdgeColor[1], myIsolatedEdgeColor[2]);
 
  myAppendFilter->AddInputConnection(myOneFaceEdgeSource->GetOutputPort());
  myOneFaceEdgeActor->SetInput(myOneFaceEdgeSource->GetOutputPort(),false); 
  aProperty = myOneFaceEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  myOneFaceEdgeColor[0] = 0; myOneFaceEdgeColor[1] = 1; myOneFaceEdgeColor[2] = 0;
  aProperty->SetColor(myOneFaceEdgeColor[0], myOneFaceEdgeColor[1], myOneFaceEdgeColor[2]);
 
  myAppendFilter->AddInputConnection(mySharedEdgeSource->GetOutputPort()); 
  mySharedEdgeActor->SetInput(mySharedEdgeSource->GetOutputPort(),false); 
  aProperty = mySharedEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  mySharedEdgeColor[0] = 1; mySharedEdgeColor[1] = 1; mySharedEdgeColor[2] = 0;
  aProperty->SetColor(mySharedEdgeColor[0], mySharedEdgeColor[1], mySharedEdgeColor[2]);
 
  myAppendFilter->AddInputConnection(myWireframeFaceSource->GetOutputPort()); 
  myWireframeFaceActor->SetInput(myWireframeFaceSource->GetOutputPort(),false); 
  aProperty = myWireframeFaceActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  aProperty->SetColor(0.5, 0.5, 0.5);

  myShadingFaceActor->SetInput(myShadingFaceSource->GetOutputPort(),true); 

  myShadingFaceProp->SetRepresentation(VTKViewer::Representation::Surface); 
  myShadingFaceProp->SetInterpolationToGouraud(); 
  myShadingFaceProp->SetAmbient(1.0);
  myShadingFaceProp->SetDiffuse(1.0);
  myShadingFaceProp->SetSpecular(0.4);
  myShadingFaceProp->SetAmbientColor(0.329412, 0.223529, 0.027451);
  myShadingFaceProp->SetDiffuseColor(0.780392, 0.568627, 0.113725);
  myShadingFaceProp->SetSpecularColor(0.992157, 0.941176, 0.807843);

  myShadingFaceActor->SetProperty(myShadingFaceProp.GetPointer());

  // Toggle display mode 
  setDisplayMode(0); // WIRE FRAME
  SetVectorMode(0);  //
  SetVerticesMode(0);  //
  SetNameMode(0); 
} 
 
 
GEOM_Actor::~GEOM_Actor() 
{ 
#ifdef MYDEBUG
  MESSAGE (this<< " ~GEOM_Actor::GEOM_Actor");
#endif
  myTextActor->Delete();
  myHighlightProp->Delete();
  myPreHighlightProp->Delete();
  myShadingFaceProp->Delete();
  myShadingBackFaceProp->Delete();
} 
 
GEOM_Actor*  
GEOM_Actor:: 
New() 
{ 
  GEOM_Actor* anObject = new GEOM_Actor(); 
  anObject->SetMapper(anObject->myPolyDataMapper.Get()); 
  return anObject; 
} 
 
 
void Write(vtkPolyData* theDataSet, const char* theFileName){ 
  vtkPolyDataWriter* aWriter = vtkPolyDataWriter::New(); 
  MESSAGE ("Write - "<<theFileName<<"' : "<<theDataSet->GetNumberOfPoints()<<"; "<<theDataSet->GetNumberOfCells()); 
  aWriter->SetInputData(theDataSet); 
  aWriter->SetFileName(theFileName); 
  //aWriter->Write(); 
  aWriter->Delete(); 
} 
 
void 
GEOM_Actor:: 
SetModified() 
{ 
  this->myVertexSource->Modified(); 
  this->myStandaloneVertexSource->Modified(); 
  this->myIsolatedEdgeSource->Modified(); 
  this->myOneFaceEdgeSource->Modified(); 
  this->mySharedEdgeSource->Modified(); 
  this->myWireframeFaceSource->Modified(); 
  this->myShadingFaceSource->Modified(); 
  this->myTextActor->Modified();
} 

void  
GEOM_Actor:: 
SetMapper(vtkMapper* theMapper) 
{ 
  SALOME_Actor::SetMapper(theMapper); 
} 

void 
GEOM_Actor:: 
AddToRender(vtkRenderer* theRenderer)
{
  //SALOME_Actor::AddToRender(theRenderer);
  
  theRenderer->AddActor(this); 
 
  this->myHighlightActor->AddToRender(theRenderer); 
  

  myShadingFaceActor->AddToRender(theRenderer); 
  myWireframeFaceActor->AddToRender(theRenderer); 
 
  mySharedEdgeActor->AddToRender(theRenderer); 
  myOneFaceEdgeActor->AddToRender(theRenderer); 
  myIsolatedEdgeActor->AddToRender(theRenderer); 
 
  myVertexActor->AddToRender(theRenderer); 
  myStandaloneVertexActor->AddToRender(theRenderer); 

  theRenderer->AddActor( myTextActor );
}
 
void 
GEOM_Actor:: 
RemoveFromRender(vtkRenderer* theRenderer)
{
  //SALOME_Actor::RemoveFromRender(theRenderer);

  
  theRenderer->RemoveActor(this);

  myHighlightActor->RemoveFromRender(theRenderer); 
  myShadingFaceActor->RemoveFromRender(theRenderer); 
  myWireframeFaceActor->RemoveFromRender(theRenderer); 
 
  mySharedEdgeActor->RemoveFromRender(theRenderer); 
  myOneFaceEdgeActor->RemoveFromRender(theRenderer); 
  myIsolatedEdgeActor->RemoveFromRender(theRenderer); 
 
  myVertexActor->RemoveFromRender(theRenderer);
  myStandaloneVertexActor->RemoveFromRender(theRenderer);

  theRenderer->RemoveActor( myTextActor );
  
  SetSelected(false);
  SetVisibility(false);
}

void  
GEOM_Actor:: 
setDisplayMode(int theMode) 
{ 
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::setDisplayMode = "<<theMode );
#endif

  if ( theMode == (int)eShadingWithEdges ) {
    // Coloring edges
    myIsolatedEdgeActor->GetProperty()->SetColor(myIsolatedEdgeColor[0],
                                                 myIsolatedEdgeColor[1],
                                                 myIsolatedEdgeColor[2]);
    myOneFaceEdgeActor->GetProperty()->SetColor(myEdgesInShadingColor[0],
                                                myEdgesInShadingColor[1],
                                                myEdgesInShadingColor[2]);
    mySharedEdgeActor->GetProperty()->SetColor(myEdgesInShadingColor[0],
                                               myEdgesInShadingColor[1],
                                               myEdgesInShadingColor[2]);
  }
  else {
    // Coloring edges
    myIsolatedEdgeActor->GetProperty()->SetColor(myIsolatedEdgeColor[0],
                                                 myIsolatedEdgeColor[1],
                                                 myIsolatedEdgeColor[2]);
    mySharedEdgeActor->GetProperty()->SetColor(mySharedEdgeColor[0],
                                               mySharedEdgeColor[1],
                                               mySharedEdgeColor[2]);
    myOneFaceEdgeActor->GetProperty()->SetColor(myOneFaceEdgeColor[0],
                                               myOneFaceEdgeColor[1],
                                               myOneFaceEdgeColor[2]);
  }

  VTKViewer_Actor::setDisplayMode(theMode);
  SetVisibility(GetVisibility()); 
} 

void  
GEOM_Actor:: 
SetSelected(bool theIsSelected) 
{ 
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetSelected = "<<theIsSelected  );
#endif

  myIsSelected = theIsSelected; 
  SetVisibility(GetVisibility()); 
} 

void  
GEOM_Actor:: 
SetVisibility(int theVisibility) 
{ 
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetVisibility = "<<theVisibility <<"  myIsSelected="<< myIsSelected
            << " theVisibility="<<theVisibility<<" myIsPreselected="<<myIsPreselected );
#endif

  SALOME_Actor::SetVisibility(theVisibility);

  this->myHighlightActor->SetVisibility(theVisibility && (myIsSelected || myIsPreselected));
  
  myShadingFaceActor->SetVisibility(theVisibility && (myDisplayMode == (int)eShading || myDisplayMode == (int)eShadingWithEdges) && (!myIsSelected || !myIsPreselected)); 
  myWireframeFaceActor->SetVisibility(theVisibility && (myDisplayMode == (int)eWireframe) && !myIsSelected);

  mySharedEdgeActor->SetVisibility(theVisibility && (myDisplayMode == (int)eWireframe || myDisplayMode == (int)eShadingWithEdges) && !myIsSelected);
  myOneFaceEdgeActor->SetVisibility(theVisibility && (myDisplayMode == (int)eWireframe || myDisplayMode == (int)eShadingWithEdges) && !myIsSelected);
  myIsolatedEdgeActor->SetVisibility(theVisibility && !myIsSelected);

  myVertexActor->SetVisibility(theVisibility && (isOnlyVertex || (myVerticesMode && (!myIsSelected && !myIsPreselected))));// must be added new mode points

  myStandaloneVertexActor->SetVisibility(theVisibility);

  myTextActor->SetVisibility( theVisibility && myNameMode );
}
 

void
GEOM_Actor
::SetNbIsos(const int theNb[2])
{
  myWireframeFaceSource->SetNbIso(theNb);
}

void
GEOM_Actor
::GetNbIsos(int &theNbU,int &theNbV)
{
  myWireframeFaceSource->GetNbIso(theNbU, theNbV);
}

void
GEOM_Actor
::SetVectorMode(bool theMode)
{
  myVectorMode = theMode;
  myIsolatedEdgeSource->SetVectorMode(theMode);
  myOneFaceEdgeSource->SetVectorMode(theMode);
  mySharedEdgeSource->SetVectorMode(theMode);
  SetModified();
}

bool
GEOM_Actor
::GetVectorMode()
{
  return myVectorMode;
}

void
GEOM_Actor
::SetVerticesMode(bool theMode)
{
  myVerticesMode = theMode;
  if ( theMode || isOnlyVertex ) {
    myAppendFilter->AddInputConnection(myVertexSource->GetOutputPort()); 
  } else {
    myAppendFilter->RemoveInputConnection(0, myVertexSource->GetOutputPort()); 
  }
  SetModified();
}

bool
GEOM_Actor
::GetVerticesMode()
{
  return myVerticesMode;
}

void
GEOM_Actor
::SetShapeName()
{
  if( !getIO() || myShape.IsNull() )
    return;

  gp_Ax3 anAx3 = GEOMUtils::GetPosition(myShape);
  double center[3] = { anAx3.Location().X(),
                       anAx3.Location().Y(),
                       anAx3.Location().Z() };
  double* pos = center;
  myTextActor->GetTextProperty()->SetFontSize( 16 );
  myTextActor->GetTextProperty()->ShadowOn();
  myTextActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
  myTextActor->GetPositionCoordinate()->SetValue(pos);
  myTextActor->SetInput( getIO()->getName() );
}

void
GEOM_Actor
::SetNameMode(bool theMode)
{
  myNameMode = theMode;
  myTextActor->SetVisibility(theMode);
  SetModified();
}

bool
GEOM_Actor
::GetNameMode()
{
  return myNameMode;
}

void  
GEOM_Actor:: 
SetDeflection(double theDeflection) 
{ 
  double aDeflection = ( theDeflection <= 0 ) ? GEOMUtils::DefaultDeflection() : theDeflection;

  if ( myDeflection != aDeflection ) {
    myDeflection = aDeflection; 
    GEOMUtils::MeshShape( myShape, myDeflection );
    SetModified(); 
  }
}

void GEOM_Actor::SetShape (const TopoDS_Shape& theShape,
                           double theDeflection,
                           bool theIsVector)
{
  myShape = theShape;

  myVertexSource->Clear();
  myStandaloneVertexSource->Clear();
  myIsolatedEdgeSource->Clear();
  myOneFaceEdgeSource->Clear();
  mySharedEdgeSource->Clear();
  myWireframeFaceSource->Clear();
  myShadingFaceSource->Clear();

  TopExp_Explorer aVertexExp (theShape,TopAbs_VERTEX);
  for (; aVertexExp.More(); aVertexExp.Next())
  {
     const TopoDS_Vertex& aVertex = TopoDS::Vertex(aVertexExp.Current());
     myVertexSource->AddVertex(aVertex);
  }
  
  SetDeflection(theDeflection);

  // look if edges are free or shared
  TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
  TopExp::MapShapesAndAncestors(theShape,TopAbs_EDGE,TopAbs_FACE,anEdgeMap);
  
  GEOM::ShapeToVTK(theShape,anEdgeMap,theIsVector,
                   myStandaloneVertexSource.Get(),
                   myIsolatedEdgeSource.Get(),
                   myOneFaceEdgeSource.Get(),
                   mySharedEdgeSource.Get(),
                   myWireframeFaceSource.Get(),
                   myShadingFaceSource.Get());
  isOnlyVertex =  
    myIsolatedEdgeSource->IsEmpty() &&
    myOneFaceEdgeSource->IsEmpty() &&
    mySharedEdgeSource->IsEmpty() &&
    myWireframeFaceSource->IsEmpty() &&
    myShadingFaceSource->IsEmpty();

  
  if((bool)myShape.Infinite() || isOnlyVertex ){
    myVertexActor->GetDeviceActor()->SetInfinitive(true);
    myStandaloneVertexActor->GetDeviceActor()->SetInfinitive(true);
    myHighlightActor->GetDeviceActor()->SetInfinitive(true);
  }

  SetShapeName();

  // 0051777: TC7.2.0: Element could not be selected in Hypothesis Construction
  myAppendFilter->Update();
}

// warning! must be checked!
// SetHighlightProperty
// SetWireframeProperty
// SetShadingProperty

void GEOM_Actor::SetHighlightProperty(vtkProperty* Prop)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetHighlightProperty" );
#endif
  this->myHighlightActor->GetProperty()->DeepCopy(Prop);
  
}

void GEOM_Actor::SetWireframeProperty(vtkProperty* Prop)
{
#ifdef MYDEBUG
  MESSAGE ( this << " GEOM_Actor::SetWireframeProperty" );
#endif
  // must be filled
  myWireframeFaceActor->SetProperty(Prop);
}


vtkProperty* GEOM_Actor::GetWireframeProperty()
{
  return myWireframeFaceActor->GetProperty();
}

vtkProperty* GEOM_Actor::GetIsolatedEdgeProperty()
{
  return myIsolatedEdgeActor->GetProperty();
}

vtkProperty* GEOM_Actor::GetVertexProperty()
{
  return myVertexActor->GetProperty();
}

vtkProperty* GEOM_Actor::GetStandaloneVertexProperty()
{
  return myStandaloneVertexActor->GetProperty();
}

vtkProperty* GEOM_Actor::GetSharedEdgeProperty()
{
  return mySharedEdgeActor->GetProperty();
}

vtkProperty* GEOM_Actor::GetFaceEdgeProperty()
{
  return myOneFaceEdgeActor->GetProperty();
}

void GEOM_Actor::SetShadingProperty(vtkProperty* Prop)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetShadingProperty" );
#endif
  myShadingFaceProp->DeepCopy(Prop);
}


vtkProperty* GEOM_Actor::GetShadingProperty()
{
  return myShadingFaceProp;
}


void GEOM_Actor::Render(vtkRenderer *ren, vtkMapper *theMapper)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::Render" );
#endif

  if(!GetVisibility())
    return;

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

  switch(myDisplayMode){
  case eWireframe://wireframe
    myPreHighlightProp->SetRepresentationToWireframe();
    myHighlightProp->SetRepresentationToWireframe();
    break;
  case eShading://shading
  case eShadingWithEdges://shading with edges
    myPreHighlightProp->SetRepresentationToSurface();
    myHighlightProp->SetRepresentationToSurface();
    myShadingFaceProp->SetRepresentationToSurface();
    myShadingBackFaceProp->SetRepresentationToSurface();
    break;
  }

  if(!myIsSelected){
    if(myIsPreselected){
      this->myHighlightActor->SetProperty(myPreHighlightProp.GetPointer());
      myShadingFaceActor->SetProperty(myPreHighlightProp.GetPointer());
      myShadingFaceActor->SetBackfaceProperty(myPreHighlightProp.GetPointer());
    } else {
      this->myHighlightActor->SetProperty(myShadingFaceProp.GetPointer());
      myShadingFaceActor->SetProperty(myShadingFaceProp.GetPointer());
      myShadingFaceActor->SetBackfaceProperty(myShadingBackFaceProp.GetPointer());
    }
  }
  else{
    this->myHighlightActor->SetProperty(myHighlightProp.GetPointer());
    myShadingFaceActor->SetProperty(myHighlightProp.GetPointer());
    myShadingFaceActor->SetBackfaceProperty(myHighlightProp.GetPointer());
  }

  this->Property->Render(this, ren);
  if (this->BackfaceProperty) {
    this->BackfaceProperty->BackfaceRender(this, ren);
    this->Device->SetBackfaceProperty(this->BackfaceProperty);
  }
  this->Device->SetProperty(this->Property);
  /*  if(myShape.ShapeType() == TopAbs_VERTEX) {
    if(ren){
      //The parameter determine size of vertex actor relate to diagonal of RendererWindow
      static double delta = 0.01;
      double X1 = -1, Y1 = -1, Z1 = 0;
      ren->ViewToWorld(X1,Y1,Z1);
      double X2 = +1, Y2 = +1, Z2 = 0;
      ren->ViewToWorld(X2,Y2,Z2);
      Z2 = sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) + (Z2-Z1)*(Z2-Z1));
      this->SetScale(Z2*delta);
    }
    vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
    this->GetMatrix(ren->GetActiveCamera(), aMatrix);
    this->Device->SetUserMatrix(aMatrix);
    this->Device->Render(ren,theMapper);
    aMatrix->Delete();    
    } else*/
    this->Device->Render(ren, theMapper);
}

void GEOM_Actor::ReleaseGraphicsResources(vtkWindow *)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::ReleaseGraphicsResources" );
#endif  
}



void GEOM_Actor::ShallowCopy(vtkProp *prop)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::ShallowCopy" );
#endif
  GEOM_Actor *f = GEOM_Actor::SafeDownCast(prop);
  if ( f != NULL )
    {
      this->SetShape(f->getTopo(),f->GetDeflection());
    }

  // Now do superclass
  this->SALOME_Actor::ShallowCopy(prop);
}

const TopoDS_Shape& GEOM_Actor::getTopo() {
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::getTopo" );
#endif
  return myShape;
}

void GEOM_Actor::setInputShape(const TopoDS_Shape& ashape, double adef1,
                               int imode, bool isVector)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::setInputShape" );
#endif
}

double GEOM_Actor::isVector()
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::isVector" );
#endif  
  return 0;
}

void GEOM_Actor::SubShapeOn()
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SubShapeOn" );
#endif  
}

void GEOM_Actor::SubShapeOff()
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SubShapeOff" );
#endif
}

void GEOM_Actor::highlight(bool highlight)
{
#ifdef MYDEBUG
  MESSAGE ( this << " GEOM_Actor::highlight highlight="<<highlight );
#endif
  SALOME_Actor::highlight(highlight);  
}

void GEOM_Actor::SetOpacity(double opa)
{
  // enk:tested OK
  myShadingFaceProp->SetOpacity(opa);
  myShadingBackFaceProp->SetOpacity(opa);
  myHighlightProp->SetOpacity(opa);
  myPreHighlightProp->SetOpacity(opa);
  myVertexActor->GetProperty()->SetOpacity(opa);
  myStandaloneVertexActor->GetProperty()->SetOpacity(opa);
}

double GEOM_Actor::GetOpacity()
{
  // enk:tested OK
  return myShadingFaceProp->GetOpacity(); 
}

void GEOM_Actor::SetColor(double r,double g,double b)
{
  // set the same color to all sub-actors:
  // - points
  // - standalone edges
  // - shared edges
  // - free edges
  // - shading color (front and back faces)
  SetPointColor(r, g, b);
  SetIsolatedEdgeColor(r, g, b);
  SetSharedEdgeColor(r, g, b);
  SetFreeEdgeColor(r, g, b);
  myShadingFaceProp->SetColor(r,g,b);     // shading color
  myShadingBackFaceProp->SetColor(r,g,b); // back face shading color
}

void GEOM_Actor::GetColor(double& r,double& g,double& b)
{
  double aRGB[3];
  myShadingFaceProp->GetColor(aRGB);
  r = aRGB[0];
  g = aRGB[1];
  b = aRGB[2];
}

/*!
  \brief Set color of points
  Points actor is shown in Wireframe mode only, see SetVisibility()
*/
void GEOM_Actor::SetPointColor(double r,  double g,  double b)
{
  myVertexActor->GetProperty()->SetColor(r, g, b);
  myStandaloneVertexActor->GetProperty()->SetColor(r, g, b);
}

/*!
  \brief Set color of standalone edges, wires, vectors
  This actor is shown in all display mode, see SetVisibility()
  TODO: check - this color seems to be used not only for standalone edges
*/
void GEOM_Actor::SetIsolatedEdgeColor(double r, double g,  double b)
{
  myIsolatedEdgeColor[0] = r; myIsolatedEdgeColor[1] = g; myIsolatedEdgeColor[2] = b;
  myIsolatedEdgeActor->GetProperty()->SetColor(r, g, b);
}

/*!
  \brief Set color of shared edges
  This actor is shown only in wireframe and shading+edges display modes, see SetVisibility()
  TODO: check - this seems to be not working currently
*/
void GEOM_Actor::SetSharedEdgeColor(double r, double g, double b)
{
  mySharedEdgeColor[0] = r; mySharedEdgeColor[1] = g; mySharedEdgeColor[2] = b;
  if ( myDisplayMode != (int)eShadingWithEdges )
    mySharedEdgeActor->GetProperty()->SetColor(r, g, b);
}

/*!
  \brief Set color of edges for standalone faces
  This actor is shown only in wireframe and shading+edges display modes, see SetVisibility()
  TODO: this color should be used not only for faces
*/
void GEOM_Actor::SetFreeEdgeColor(double r, double g, double b)
{
  myOneFaceEdgeColor[0] = r; myOneFaceEdgeColor[1] = g; myOneFaceEdgeColor[2] = b;
  if ( myDisplayMode != (int)eShadingWithEdges )
    myOneFaceEdgeActor->GetProperty()->SetColor(r, g, b);
}

/*!
  \brief Set color of iso-lines
  This actor is shown only in wireframe display mode, see SetVisibility()
*/
void GEOM_Actor::SetIsosColor(double r, double g, double b)
{
  myWireframeFaceActor->GetProperty()->SetColor(r, g, b);
}

/*!
  \brief Set color of labels
  This actor is shown only if 'Show name' is switched-on, see SetVisibility()
*/
void GEOM_Actor::SetLabelColor(double r, double g, double b)
{
  myTextActor->GetTextProperty()->SetColor(r, g, b);
}

void GEOM_Actor::SetMaterial(std::vector<vtkProperty*> theProps)
{
  // we set material properties as back and front material
  int aSize = theProps.size();

  if ( aSize < 1 || aSize > 2)
    return;

  // theProps[0] -- front material properties
  // theProps[1] -- back material properties (if exist)

  double aCoefnt;

  // Set reflection coefficients
  aCoefnt = theProps[0]->GetAmbient();
  myShadingFaceProp->SetAmbient(aCoefnt);
  myVertexActor->GetProperty()->SetAmbient(aCoefnt);
  myStandaloneVertexActor->GetProperty()->SetAmbient(aCoefnt);
  if ( aSize == 2 )
    aCoefnt = theProps[1]->GetAmbient();
  myShadingBackFaceProp->SetAmbient(aCoefnt);

  // Set diffuse coefficients
  aCoefnt = theProps[0]->GetDiffuse();
  myShadingFaceProp->SetDiffuse(aCoefnt);
  myVertexActor->GetProperty()->SetDiffuse(aCoefnt);
  myStandaloneVertexActor->GetProperty()->SetDiffuse(aCoefnt);
  if ( aSize == 2 )
    aCoefnt = theProps[1]->GetDiffuse();
  myShadingBackFaceProp->SetDiffuse(aCoefnt);
  
  // Set specular coefficients
  aCoefnt = theProps[0]->GetSpecular();
  myShadingFaceProp->SetSpecular(aCoefnt);
  myVertexActor->GetProperty()->SetSpecular(aCoefnt);
  myStandaloneVertexActor->GetProperty()->SetSpecular(aCoefnt);
  if ( aSize == 2 )
    aCoefnt = theProps[1]->GetSpecular();
  myShadingBackFaceProp->SetSpecular(aCoefnt);


  double* aColor;

  // Set reflection colors
  aColor = theProps[0]->GetAmbientColor();
  myShadingFaceProp->SetAmbientColor(aColor[0], aColor[1], aColor[2]);
  myVertexActor->GetProperty()->SetAmbientColor(aColor[0], aColor[1], aColor[2]);
  myStandaloneVertexActor->GetProperty()->SetAmbientColor(aColor[0], aColor[1], aColor[2]);
  if ( aSize == 2 )
    aColor = theProps[1]->GetAmbientColor();
  myShadingBackFaceProp->SetAmbientColor(aColor[0], aColor[1], aColor[2]);

  // Set diffuse colors
  aColor = theProps[0]->GetDiffuseColor();
  myShadingFaceProp->SetDiffuseColor(aColor[0], aColor[1], aColor[2]);
  myVertexActor->GetProperty()->SetDiffuseColor(aColor[0], aColor[1], aColor[2]);
  myStandaloneVertexActor->GetProperty()->SetDiffuseColor(aColor[0], aColor[1], aColor[2]);
  if ( aSize == 2 )
    aColor = theProps[1]->GetDiffuseColor();
  myShadingBackFaceProp->SetDiffuseColor(aColor[0], aColor[1], aColor[2]);

  // Set specular colors
  aColor = theProps[0]->GetSpecularColor();
  myShadingFaceProp->SetSpecularColor(aColor[0], aColor[1], aColor[2]);
  myVertexActor->GetProperty()->SetSpecularColor(aColor[0], aColor[1], aColor[2]);
  myStandaloneVertexActor->GetProperty()->SetSpecularColor(aColor[0], aColor[1], aColor[2]);
  if ( aSize == 2 )
    aColor = theProps[1]->GetSpecularColor();
  myShadingBackFaceProp->SetSpecularColor(aColor[0], aColor[1], aColor[2]);

  // Set shininess
  aCoefnt = theProps[0]->GetSpecularPower();
  myShadingFaceProp->SetSpecularPower(aCoefnt);
  myVertexActor->GetProperty()->SetSpecularPower(aCoefnt);
  myStandaloneVertexActor->GetProperty()->SetSpecularPower(aCoefnt);
  if ( aSize == 2 )
    aCoefnt = theProps[1]->GetSpecularPower();
  myShadingBackFaceProp->SetSpecularPower(aCoefnt);

  // Set back face material property
  myShadingFaceActor->SetBackfaceProperty(myShadingBackFaceProp.GetPointer());
}

vtkProperty* GEOM_Actor::GetFrontMaterial()
{
  return myShadingFaceProp;
}

vtkProperty* GEOM_Actor::GetBackMaterial()
{
  return myShadingBackFaceProp;
}

bool GEOM_Actor::IsInfinitive()
{
  return ((bool)myShape.Infinite() || isOnlyVertex);
}

/*!
  To map current selection to VTK representation
*/
void
GEOM_Actor
::Highlight(bool theIsHighlight)
{
  myIsSelected = theIsHighlight;
#ifdef MYDEBUG
  MESSAGE ( this << " GEOM_Actor::Highlight myIsSelected="<<myIsSelected );
#endif

  SALOME_Actor::Highlight(theIsHighlight); // this method call ::highlight(theIsHighlight) in the end
  SetVisibility(GetVisibility());
}

/*!
  To process prehighlight (called from SVTK_InteractorStyle)
*/
bool
GEOM_Actor
::PreHighlight(vtkInteractorStyle *theInteractorStyle, 
               SVTK_SelectionEvent* theSelectionEvent,
               bool theIsHighlight)
{
#ifdef MYDEBUG
  MESSAGE ( this<<" GEOM_Actor::PreHighlight (3) theIsHighlight="<<theIsHighlight );
#endif

  if ( !GetPickable() )
    return false;  

  myPreHighlightActor->SetVisibility( false );
  bool anIsPreselected = myIsPreselected;
  
  Selection_Mode aSelectionMode = theSelectionEvent->mySelectionMode;
  bool anIsChanged = (mySelectionMode != aSelectionMode);

  if( !theIsHighlight ) {
    SetPreSelected( false );
  }else{
    switch(aSelectionMode){
    case ActorSelection : 
    {
//       cout << "=============== " << myIO->getEntry() << endl;
//       int nbio = mySelector->IObjectCount();
//       cout << " nbio = " << nbio << endl;

      if( !mySelector->IsSelected( myIO ) ) {
        //      printf ("!!!!!!!!!!!!!!!!\n");
        SetPreSelected( true );
      }
    }
    default:
      break;
    }
  }

  mySelectionMode = aSelectionMode;
  anIsChanged |= (anIsPreselected != myIsPreselected);

  SetVisibility(GetVisibility());
  return anIsChanged;
}

/*!
  To process highlight (called from SVTK_InteractorStyle)
*/
bool
GEOM_Actor
::Highlight(vtkInteractorStyle *theInteractorStyle, 
            SVTK_SelectionEvent* theSelectionEvent,
            bool theIsHighlight)
{
  // define the selection of object
#ifdef MYDEBUG
  MESSAGE ( std::endl << this << " GEOM_Actor::Highlight (3) myIsSelected="<<myIsSelected );
#endif
  bool aRet = SALOME_Actor::Highlight(theInteractorStyle,theSelectionEvent,theIsHighlight);
  SetSelected(theIsHighlight);
  if(theIsHighlight)
    SetPreSelected(false);
  
 
  return aRet;
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

void GEOM_Actor::SetEdgesInShadingColor(double r,double g,double b)
{
  myEdgesInShadingColor[0] = r;
  myEdgesInShadingColor[1] = g;
  myEdgesInShadingColor[2] = b;
}

void GEOM_Actor::SetIsosWidth(const int width) {
  myWireframeFaceActor->GetProperty()->SetLineWidth(width);
}

int GEOM_Actor::GetIsosWidth() const {
  return (int)myWireframeFaceActor->GetProperty()->GetLineWidth();
}

void GEOM_Actor::SetWidth(const int width) {
  myIsolatedEdgeActor->GetProperty()->SetLineWidth(width);
  myOneFaceEdgeActor->GetProperty()->SetLineWidth(width);
  mySharedEdgeActor->GetProperty()->SetLineWidth(width);
  myHighlightProp->SetLineWidth(width);
  myPreHighlightProp->SetLineWidth(width+1);

}

int GEOM_Actor::GetWidth() const {
  return (int)myIsolatedEdgeActor->GetProperty()->GetLineWidth();
}
