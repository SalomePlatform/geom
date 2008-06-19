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
//  File   : GEOM_Actor.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

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
#include "SVTK_Actor.h"

#include <vtkObjectFactory.h> 
#include <vtkRenderer.h> 
#include <vtkProperty.h> 
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
 
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Bnd_Box.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Iterator.hxx>
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

//vtkStandardNewMacro(GEOM_Actor);

#ifndef MYDEBUG
//#define MYDEBUG
#endif

GEOM_Actor::GEOM_Actor(): 
  //  myDisplayMode(eWireframe), 
  myIsSelected(false), 
 
  myVertexActor(GEOM_DeviceActor::New(),true), 
  myVertexSource(GEOM_VertexSource::New(),true), 
 
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
  myPolyDataMapper(vtkPolyDataMapper::New(),true),

  myHighlightProp(vtkProperty::New()),
  myPreHighlightProp(vtkProperty::New()),
  myShadingFaceProp(vtkProperty::New())
  
{ 
#ifdef MYDEBUG
  MESSAGE (this<< " GEOM_Actor::GEOM_Actor");
#endif

  myPolyDataMapper->SetInput(myAppendFilter->GetOutput()); 
  vtkProperty* aProperty; 

  myHighlightProp->SetAmbient(0.5);
  myHighlightProp->SetDiffuse(0.3);
  myHighlightProp->SetSpecular(0.2);
  myHighlightProp->SetRepresentationToSurface();
  myHighlightProp->SetAmbientColor(1, 1, 1);
  myHighlightProp->SetDiffuseColor(1, 1, 1);
  myHighlightProp->SetSpecularColor(0.5, 0.5, 0.5);
  myHighlightActor->SetProperty(myHighlightProp.GetPointer());

  this->myHighlightActor->SetInput(myAppendFilter->GetOutput(),false);

  myPreHighlightProp->SetColor(0,1,1);
  myPreHighlightProp->SetPointSize(SALOME_POINT_SIZE+2);
  myPreHighlightProp->SetLineWidth(SALOME_LINE_WIDTH+1);
  myPreHighlightProp->SetRepresentationToWireframe();

  myAppendFilter->AddInput(myVertexSource->GetOutput()); 
  myVertexActor->SetInput(myVertexSource->GetOutput(),false); 
  aProperty = myVertexActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_POINTS); 
  aProperty->SetPointSize(3); 
  aProperty->SetColor(1, 1, 0);
 
  myAppendFilter->AddInput(myIsolatedEdgeSource->GetOutput()); 
  myIsolatedEdgeActor->SetInput(myIsolatedEdgeSource->GetOutput(),false); 
  aProperty = myIsolatedEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  aProperty->SetColor(1, 0, 0);
 
  myAppendFilter->AddInput(myOneFaceEdgeSource->GetOutput()); 
  myOneFaceEdgeActor->SetInput(myOneFaceEdgeSource->GetOutput(),false); 
  aProperty = myOneFaceEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  aProperty->SetColor(0, 1, 0);
 
  myAppendFilter->AddInput(mySharedEdgeSource->GetOutput()); 
  mySharedEdgeActor->SetInput(mySharedEdgeSource->GetOutput(),false); 
  aProperty = mySharedEdgeActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  aProperty->SetColor(1, 1, 0);
 
  myAppendFilter->AddInput(myWireframeFaceSource->GetOutput()); 
  myWireframeFaceActor->SetInput(myWireframeFaceSource->GetOutput(),false); 
  aProperty = myWireframeFaceActor->GetProperty(); 
  aProperty->SetRepresentation(VTK_WIREFRAME); 
  aProperty->SetColor(0.5, 0.5, 0.5);

  myShadingFaceActor->SetInput(myShadingFaceSource->GetOutput(),true); 

  myShadingFaceProp->SetRepresentation(VTK_SURFACE); 
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

} 
 
 
GEOM_Actor::~GEOM_Actor() 
{ 
#ifdef MYDEBUG
  MESSAGE (this<< " ~GEOM_Actor::GEOM_Actor");
#endif
  myHighlightProp->Delete();
  myPreHighlightProp->Delete();
  myShadingFaceProp->Delete();
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
  aWriter->SetInput(theDataSet); 
  aWriter->SetFileName(theFileName); 
  //aWriter->Write(); 
  aWriter->Delete(); 
} 
 
void 
GEOM_Actor:: 
SetModified() 
{ 
  this->myVertexSource->Modified(); 
  this->myIsolatedEdgeSource->Modified(); 
  this->myOneFaceEdgeSource->Modified(); 
  this->mySharedEdgeSource->Modified(); 
  this->myWireframeFaceSource->Modified(); 
  this->myShadingFaceSource->Modified(); 
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

  
  SetSelected(false);
  SetVisibility(false);
}

void  
GEOM_Actor:: 
setDisplayMode(int theMode) 
{ 
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetDisplayMode = "<<theMode );
#endif
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
  
  myShadingFaceActor->SetVisibility(theVisibility && (myDisplayMode == (int)eShading) && (!myIsSelected || !myIsPreselected)); 
  myWireframeFaceActor->SetVisibility(theVisibility && (myDisplayMode ==(int)eWireframe) && !myIsSelected);

  mySharedEdgeActor->SetVisibility(theVisibility && myDisplayMode == (int)eWireframe && !myIsSelected);
  myOneFaceEdgeActor->SetVisibility(theVisibility && myDisplayMode == (int)eWireframe && !myIsSelected);
  myIsolatedEdgeActor->SetVisibility(theVisibility && !myIsSelected);

//  myVertexActor->SetVisibility(false);// must be added new mode points 
  myVertexActor->SetVisibility(theVisibility);
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

static 
void 
MeshShape(const TopoDS_Shape& theShape,
          float& theDeflection, 
          bool theIsRelative)
{ 
  static Standard_Real RELATIVE_DEFLECTION = 0.0001; 
  Standard_Real aDeflection = theDeflection; 

  if(theDeflection <= 0) { // Compute default theDeflection
    Bnd_Box B;
    BRepBndLib::Add(theShape, B);
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    Standard_Real aDiagonal = (aXmax-aXmin)*(aXmax-aXmin) +
                              (aYmax-aYmin)*(aYmax-aYmin) +
                              (aZmax-aZmin)*(aZmax-aZmin);
    aDiagonal = sqrt(aDiagonal); 
    aDeflection = aDiagonal*RELATIVE_DEFLECTION; 
 
    if(theIsRelative) 
      theDeflection = RELATIVE_DEFLECTION; 
    else 
      theDeflection = aDeflection; 
  }
  
  BRepMesh_IncrementalMesh aMesh(theShape,aDeflection);
}

void  
GEOM_Actor:: 
SetDeflection(float theDeflection, bool theIsRelative) 
{ 
  myDeflection = theDeflection; 
  myIsRelative = theIsRelative; 
 
  MeshShape(myShape,myDeflection,myIsRelative); 
 
  SetModified(); 
} 

void GEOM_Actor::SetShape (const TopoDS_Shape& theShape,
                           float theDeflection,
                           bool theIsRelative,
                           bool theIsVector)
{
  myShape = theShape;

  myVertexSource->Clear();
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
  SetDeflection(theDeflection, theIsRelative);

  // look if edges are free or shared
  TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
  TopExp::MapShapesAndAncestors(theShape,TopAbs_EDGE,TopAbs_FACE,anEdgeMap);

  SetShape(theShape,anEdgeMap,theIsVector);
}

void GEOM_Actor::SetShape (const TopoDS_Shape& theShape,
                           const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeMap,
                           bool theIsVector)
{
  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anItr(theShape);
    for (; anItr.More(); anItr.Next()) {
      SetShape(anItr.Value(),theEdgeMap,theIsVector);
    }
  }

  switch (theShape.ShapeType()) {
    case TopAbs_WIRE: {
      TopExp_Explorer anEdgeExp(theShape,TopAbs_EDGE);
      for (; anEdgeExp.More(); anEdgeExp.Next()){
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
        if (!BRep_Tool::Degenerated(anEdge))
          myIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
      }
      break;
    }
    case TopAbs_EDGE: {
      const TopoDS_Edge& anEdge = TopoDS::Edge(theShape);
      if (!BRep_Tool::Degenerated(anEdge))
        myIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
      break;
    }
    case TopAbs_VERTEX: {
      break;
    }
    default: {
      TopExp_Explorer aFaceExp (theShape,TopAbs_FACE);
      for(; aFaceExp.More(); aFaceExp.Next()) {
        const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
        myWireframeFaceSource->AddFace(aFace);
        myShadingFaceSource->AddFace(aFace);
        TopExp_Explorer anEdgeExp(aFaceExp.Current(), TopAbs_EDGE);
	for(; anEdgeExp.More(); anEdgeExp.Next()) {
	  const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
          if(!BRep_Tool::Degenerated(anEdge)){
	    // compute the number of faces
	    int aNbOfFaces = theEdgeMap.FindFromKey(anEdge).Extent();
	    switch(aNbOfFaces){
	    case 0:  // isolated edge
	      myIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
	      break;
	    case 1:  // edge in only one face
	      myOneFaceEdgeSource->AddEdge(anEdge,theIsVector);
	      break;
	    default: // edge shared by at least two faces
	      mySharedEdgeSource->AddEdge(anEdge,theIsVector);
	    }
          }
        }
      }
    }
  }
}

// OLD METHODS
void GEOM_Actor::setDeflection(double adef) {
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::setDeflection" );
#endif
  SetDeflection((float)adef,GetIsRelative());
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

void GEOM_Actor::SetShadingProperty(vtkProperty* Prop)
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::SetShadingProperty" );
#endif
  myShadingFaceProp->DeepCopy(Prop);
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
  case 0://wireframe
    myPreHighlightProp->SetRepresentationToWireframe();
    myHighlightProp->SetRepresentationToWireframe();
    break;
  case 1://shading
    myPreHighlightProp->SetRepresentationToSurface();
    myHighlightProp->SetRepresentationToSurface();
    break;
  }

  if(!myIsSelected){
    if(myIsPreselected){
      this->myHighlightActor->SetProperty(myPreHighlightProp.GetPointer());
      myShadingFaceActor->SetProperty(myPreHighlightProp.GetPointer());
    } else {
      this->myHighlightActor->SetProperty(myShadingFaceProp.GetPointer());
      myShadingFaceActor->SetProperty(myShadingFaceProp.GetPointer());
    }
  }
  else{
    this->myHighlightActor->SetProperty(myHighlightProp.GetPointer());
    myShadingFaceActor->SetProperty(myHighlightProp.GetPointer());
  }

  this->Property->Render(this, ren);
  if (this->BackfaceProperty) {
    this->BackfaceProperty->BackfaceRender(this, ren);
    this->Device->SetBackfaceProperty(this->BackfaceProperty);
  }
  this->Device->SetProperty(this->Property);
  if(myShape.ShapeType() == TopAbs_VERTEX) {
    if(ren){
      //The parameter determine size of vertex actor relate to diagonal of RendererWindow
      static vtkFloatingPointType delta = 0.01;
      vtkFloatingPointType X1 = -1, Y1 = -1, Z1 = 0;
      ren->ViewToWorld(X1,Y1,Z1);
      vtkFloatingPointType X2 = +1, Y2 = +1, Z2 = 0;
      ren->ViewToWorld(X2,Y2,Z2);
      Z2 = sqrt((X2-X1)*(X2-X1) + (Y2-Y1)*(Y2-Y1) + (Z2-Z1)*(Z2-Z1));
      this->SetScale(Z2*delta);
    }
    vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
    this->GetMatrix(ren->GetActiveCamera(), aMatrix);
    this->Device->SetUserMatrix(aMatrix);
    this->Device->Render(ren,theMapper);
    aMatrix->Delete();    
  } else
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
      this->SetShape(f->getTopo(),f->GetDeflection(),f->GetIsRelative());
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

double GEOM_Actor::getDeflection()
{
#ifdef MYDEBUG
  MESSAGE ( "GEOM_Actor::getDeflection" );
#endif
  return (double) GetDeflection();
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

void GEOM_Actor::SetOpacity(vtkFloatingPointType opa)
{
  // enk:tested OK
  myShadingFaceProp->SetOpacity(opa);
  myHighlightProp->SetOpacity(opa);
  myPreHighlightProp->SetOpacity(opa);
}

vtkFloatingPointType GEOM_Actor::GetOpacity()
{
  // enk:tested OK
  return myShadingFaceProp->GetOpacity(); 
}

void GEOM_Actor::SetColor(vtkFloatingPointType r,vtkFloatingPointType g,vtkFloatingPointType b)
{
  // enk:tested OK
  myShadingFaceProp->SetColor(r,g,b);                          // shading color (Shading)
  myIsolatedEdgeActor->GetProperty()->SetColor(r,g,b);         // standalone edge color (Wireframe)
  myVertexActor->GetProperty()->SetColor(r,g,b);               // vertex actor (Shading/Wireframe)
  myOneFaceEdgeActor->GetProperty()->SetColor(r,g,b);          // standalone face edge color (Wireframe)
  mySharedEdgeActor->GetProperty()->SetColor(r,g,b);           // share edge color (Wireframe)
}

void GEOM_Actor::GetColor(vtkFloatingPointType& r,vtkFloatingPointType& g,vtkFloatingPointType& b)
{
  // enk:tested OK
  vtkFloatingPointType aRGB[3];
  myShadingFaceProp->GetColor(aRGB);
  r = aRGB[0];
  g = aRGB[1];
  b = aRGB[2];
}

bool GEOM_Actor::IsInfinitive()
{
  return (bool)(myShape.Infinite());
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
      if( !mySelector->IsSelected( myIO ) ) {
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
