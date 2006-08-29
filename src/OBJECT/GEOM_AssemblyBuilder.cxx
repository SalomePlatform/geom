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
//  File   : GEOM_AssemblyBuilder.cxx
//  Author : Christophe ATTANASIO
//  Module : GEOM
//  $Header$

/*!
  \class GEOM_AssemblyBuilder GEOM_AssemblyBuilder.h
  \brief ....
*/

#include "GEOM_AssemblyBuilder.h"
#include "GEOM_Actor.h"

#include <SUIT_Session.h>

#include <vtkProperty.h>

// Open CASCADE Includes
#include <TopExp_Explorer.hxx>
#include <Bnd_Box.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Tool.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Iterator.hxx>

#include "utilities.h"

using namespace std;
// SALOME

#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )





void GEOM_AssemblyBuilder::InitProperties(vtkProperty* IsoProp,
					  vtkProperty* FaceProp,
					  vtkProperty* EdgeFProp,
					  vtkProperty* EdgeSProp,
					  vtkProperty* EdgeIProp,
					  vtkProperty* VertexProp,
					  vtkProperty* IsoPVProp,
					  vtkProperty* EdgePVProp,
					  vtkProperty* VertexPVProp)
{
  // Shading like default OCC material
  FaceProp->SetRepresentationToSurface();
  FaceProp->SetInterpolationToGouraud();
  FaceProp->SetAmbient(1.0);
  FaceProp->SetDiffuse(1.0);
  FaceProp->SetSpecular(0.4);
  FaceProp->SetAmbientColor(0.329412, 0.223529, 0.027451);
  FaceProp->SetDiffuseColor(0.780392, 0.568627, 0.113725);
  FaceProp->SetSpecularColor(0.992157, 0.941176, 0.807843);

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QColor aColor;

  // Wireframe for iso
  aColor = aResMgr->colorValue( "Geometry", "isos_color", QColor( int(0.5*255), int(0.5*255), int(0.5*255) ) );
  float red = aColor.red()/255.0;
  float green = aColor.green()/255.0;
  float blue = aColor.blue()/255.0;
  IsoProp->SetRepresentationToWireframe();
  IsoProp->SetAmbientColor(red, green, blue);
  IsoProp->SetDiffuseColor(red, green, blue);
  IsoProp->SetSpecularColor(red, green, blue);
  
  // Wireframe for iso
  IsoPVProp->SetRepresentationToWireframe();
  IsoPVProp->SetAmbientColor(0, 1, 1);
  IsoPVProp->SetDiffuseColor(0, 1, 1);
  IsoPVProp->SetSpecularColor(0, 1, 1);

  // Wireframe for shared edge 
  aColor = aResMgr->colorValue( "Geometry", "wireframe_color", QColor( 255, 255, 0 ) );
  red = aColor.red()/255.0;
  green = aColor.green()/255.0;
  blue = aColor.blue()/255.0;
  EdgeSProp->SetRepresentationToWireframe();
  EdgeSProp->SetAmbientColor(red, green, blue);
  EdgeSProp->SetDiffuseColor(red, green, blue);
  EdgeSProp->SetSpecularColor(red, green, blue);
  
  // Wireframe for free edge 
  aColor = aResMgr->colorValue( "Geometry", "free_bound_color", QColor( 0, 255, 0 ) );
  red = aColor.red()/255.0;
  green = aColor.green()/255.0;
  blue = aColor.blue()/255.0;
  EdgeFProp->SetRepresentationToWireframe();
  EdgeFProp->SetAmbientColor(red, green, blue);
  EdgeFProp->SetDiffuseColor(red, green, blue);
  EdgeFProp->SetSpecularColor(red, green, blue);

  // Wireframe for isolated edge 
  aColor = aResMgr->colorValue( "Geometry", "line_color", QColor( 255, 0, 0 ) );
  red = aColor.red()/255.0;
  green = aColor.green()/255.0;
  blue = aColor.blue()/255.0;
  EdgeIProp->SetRepresentationToWireframe();
  EdgeIProp->SetAmbientColor(red, green, blue);
  EdgeIProp->SetDiffuseColor(red, green, blue);
  
  // Wireframe for Preview edge 
  EdgePVProp->SetRepresentationToWireframe();
  EdgePVProp->SetAmbientColor(1, 1, 0);
  EdgePVProp->SetDiffuseColor(1, 1, 0);
  EdgePVProp->SetSpecularColor(1, 1, 0);

  // Wireframe for vertex 
  aColor = aResMgr->colorValue( "Geometry", "point_color", QColor( 255, 255, 0 ) );
  red = aColor.red()/255.0;
  green = aColor.green()/255.0;
  blue = aColor.blue()/255.0;
  VertexProp->SetRepresentationToWireframe();
  VertexProp->SetAmbientColor(red, green, blue);
  VertexProp->SetDiffuseColor(red, green, blue);
  VertexProp->SetSpecularColor(red, green, blue);
  
  // Wireframe for vertex 
  VertexPVProp->SetRepresentationToWireframe();
  VertexPVProp->SetAmbientColor(0, 1, 1);
  VertexPVProp->SetDiffuseColor(0, 1, 1);
  VertexPVProp->SetSpecularColor(0, 1, 1);
}


void GEOM_AssemblyBuilder::MeshShape(const TopoDS_Shape myShape,
					 Standard_Real deflection,
					 Standard_Boolean forced)
{
  // Mesh the shape if necessary
  Standard_Boolean alreadymesh = Standard_True;
  TopExp_Explorer ex;
  TopLoc_Location aLoc;

  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
    const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
    if(aPoly.IsNull()) { alreadymesh = Standard_False; break; }
  }

  if(!alreadymesh || forced) {
    if(deflection<=0) {
      // Compute default deflection
      Bnd_Box B;
      BRepBndLib::Add(myShape, B);
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
      deflection = MAX3( aXmax-aXmin , aYmax-aYmin , aZmax-aZmin) * 0.001 *4;
    }
    BRepMesh_IncrementalMesh MESH(myShape,deflection);
  }
}



vtkActorCollection* GEOM_AssemblyBuilder::BuildActors(const TopoDS_Shape& myShape,
							  Standard_Real deflection,
							  Standard_Integer mode,
							  Standard_Boolean forced) {

  vtkActorCollection* AISActors = vtkActorCollection::New();

  if(myShape.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator anItr(myShape);
    for(; anItr.More(); anItr.Next()) {
      vtkActorCollection* theActors = GEOM_AssemblyBuilder::BuildActors(anItr.Value(), deflection, mode, forced);
      theActors->InitTraversal();
      vtkActor* anActor = (vtkActor*)theActors->GetNextActor();
      while(!(anActor==NULL)) {
	AISActors->AddItem(anActor);
	anActor = (vtkActor*)theActors->GetNextActor();
      }
    }
  }
  // Create graphics properties

  vtkProperty* IsoProp = vtkProperty::New();
  vtkProperty* FaceProp = vtkProperty::New();
  vtkProperty* EdgeFProp = vtkProperty::New();
  vtkProperty* EdgeSProp = vtkProperty::New();
  vtkProperty* EdgeIProp = vtkProperty::New();
  vtkProperty* VertexProp = vtkProperty::New();

  vtkProperty* IsoPVProp = vtkProperty::New();
  vtkProperty* EdgePVProp = vtkProperty::New();
  vtkProperty* VertexPVProp = vtkProperty::New();

  InitProperties(IsoProp,FaceProp,EdgeFProp,EdgeSProp,EdgeIProp,VertexProp,IsoPVProp,EdgePVProp,VertexPVProp);

  MeshShape(myShape,deflection,forced);

  if ( myShape.ShapeType() <= 4 && myShape.ShapeType() != TopAbs_COMPOUND) {
    
    // FACE Actor
    // look if edges are free or shared 
    TopTools_IndexedDataMapOfShapeListOfShape edgemap;
    TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,edgemap);
    
    TopExp_Explorer ex;
    
    for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
      
      GEOM_Actor* FaceActor = GEOM_Actor::New();
      FaceActor->SetShadingProperty(FaceProp);
      FaceActor->SetWireframeProperty(IsoProp);

      FaceActor->SetPreviewProperty(IsoPVProp);
      
      FaceActor->setInputShape(ex.Current(),deflection,mode);
      
      AISActors->AddItem(FaceActor);
      
      TopExp_Explorer ex2;
      for (ex2.Init(ex.Current(), TopAbs_EDGE); ex2.More(); ex2.Next()) {
	const TopoDS_Edge& aEdge = TopoDS::Edge(ex2.Current());
	
	if (BRep_Tool::Degenerated(aEdge)) {    
	  continue;
	}
	
	// compute the number of faces
	Standard_Integer nbf = edgemap.FindFromKey(ex2.Current()).Extent();
	GEOM_Actor* EdgeActor = GEOM_Actor::New();
	EdgeActor->SubShapeOn();
	EdgeActor->setInputShape(ex2.Current(),deflection,mode);
	
	switch (nbf) {
	  
	case 0 : // isolated edge
	  {
	    EdgeActor->SetShadingProperty(EdgeIProp);
	    EdgeActor->SetWireframeProperty(EdgeIProp);
	  }
	  break;
	  
	case 1 :// edge in only one face
	  {
	    EdgeActor->SetShadingProperty(EdgeFProp);
	    EdgeActor->SetWireframeProperty(EdgeFProp);
	  }
	  break;
	  
	default :   // edge shared by at least two faces      
	  {
	    EdgeActor->SetShadingProperty(EdgeSProp);
	    EdgeActor->SetWireframeProperty(EdgeSProp);
	  }
	}
	
	EdgeActor->SetPreviewProperty(EdgePVProp);
	AISActors->AddItem(EdgeActor);
      }
    }
  } else if ( myShape.ShapeType() == TopAbs_WIRE ) { // WIRE Actor
    TopExp_Explorer ex;
    for (ex.Init(myShape, TopAbs_EDGE); ex.More(); ex.Next()) {
      const TopoDS_Edge& aEdge = TopoDS::Edge(ex.Current());
	
      if (BRep_Tool::Degenerated(aEdge)) {    
	continue;
      }
	
      GEOM_Actor* EdgeActor = GEOM_Actor::New();
      EdgeActor->setInputShape(ex.Current(),deflection,mode);
      EdgeActor->SetShadingProperty(EdgeIProp);
      EdgeActor->SetWireframeProperty(EdgeIProp);
      EdgeActor->SetPreviewProperty(EdgePVProp);
      
      AISActors->AddItem(EdgeActor);
    }
  } else if ( myShape.ShapeType() == TopAbs_EDGE ) { // EDGE Actor
    GEOM_Actor* EdgeActor = GEOM_Actor::New();
    EdgeActor->setInputShape(myShape,deflection,mode);
    EdgeActor->SetShadingProperty(EdgeIProp);
    EdgeActor->SetWireframeProperty(EdgeIProp);
    EdgeActor->SetPreviewProperty(EdgePVProp);
    
    AISActors->AddItem(EdgeActor);
  } else if ( myShape.ShapeType() == TopAbs_VERTEX ) { // VERTEX Actor
    GEOM_Actor* VertexActor = GEOM_Actor::New();
    VertexActor->setInputShape(myShape,deflection,mode);
    VertexActor->SetShadingProperty(VertexProp);
    VertexActor->SetWireframeProperty(VertexProp);
    VertexActor->SetPreviewProperty(VertexPVProp);
    
    AISActors->AddItem(VertexActor);
  
  } 
  
  return AISActors;

}



//-------------------------------------------------------------
// BUILD ASSEMBLY
//-------------------------------------------------------------
vtkAssembly*  GEOM_AssemblyBuilder::BuildAssembly(const TopoDS_Shape& myShape,
						      Standard_Real deflection,
						      Standard_Integer mode,
						      Standard_Boolean forced)
{
  // Create a new vtkAssembly

  vtkAssembly* myVTKShape = vtkAssembly::New();


  // Create graphics properties

  vtkProperty* IsoProp = vtkProperty::New();
  vtkProperty* FaceProp = vtkProperty::New();
  vtkProperty* EdgeFProp = vtkProperty::New();
  vtkProperty* EdgeSProp = vtkProperty::New();
  vtkProperty* EdgeIProp = vtkProperty::New();
  vtkProperty* VertexProp = vtkProperty::New();
  vtkProperty* EdgePVProp = vtkProperty::New();
  vtkProperty* VertexPVProp = vtkProperty::New();
  vtkProperty* IsoPVProp = vtkProperty::New();

  InitProperties(IsoProp,FaceProp,EdgeFProp,EdgeSProp,EdgeIProp,VertexProp,IsoPVProp,EdgePVProp,VertexPVProp);

  MeshShape(myShape,deflection,forced);

  
  // FACE Actor
  
  // look if edges are free or shared 
  TopTools_IndexedDataMapOfShapeListOfShape edgemap;
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE,TopAbs_FACE,edgemap);
  
  TopExp_Explorer ex;

  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
    //const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
    
    GEOM_Actor* FaceActor = GEOM_Actor::New();
    FaceActor->SetShadingProperty(FaceProp);
    FaceActor->SetWireframeProperty(IsoProp);
    
    vtkAssembly* myFaceAssembly = vtkAssembly::New();

   
    FaceActor->setInputShape(ex.Current(),deflection,mode);
    myFaceAssembly->AddPart(FaceActor);
    
    TopExp_Explorer ex2;
    for (ex2.Init(ex.Current(), TopAbs_EDGE); ex2.More(); ex2.Next()) {
      const TopoDS_Edge& aEdge = TopoDS::Edge(ex2.Current());
 
      if (BRep_Tool::Degenerated(aEdge)) {    
	continue;
      }
      
    
      // compute the number of faces
      Standard_Integer nbf = edgemap.FindFromKey(ex2.Current()).Extent();
      GEOM_Actor* EdgeActor = GEOM_Actor::New();
      switch (nbf) {
	
      case 0 : // isolated edge
	{
	  EdgeActor->SetShadingProperty(EdgeIProp);
	  EdgeActor->SetWireframeProperty(EdgeIProp);
	}
	break;
	
      case 1 :// edge in only one face
	{
	  EdgeActor->SetShadingProperty(EdgeFProp);
	  EdgeActor->SetWireframeProperty(EdgeFProp);
	}
	break;
	
      default :   // edge shared by at least two faces      
	{
	  EdgeActor->SetShadingProperty(EdgeSProp);
	  EdgeActor->SetWireframeProperty(EdgeSProp);
	}
      }
  
      EdgeActor->setInputShape(ex2.Current(),deflection,mode);
      myFaceAssembly->AddPart(EdgeActor);
    }
    myVTKShape->AddPart(myFaceAssembly);
  }
  
  return myVTKShape;
 
}

//-------------------------------------------------------------
// CHANGE SPECIFIC DISPLAY MODE
//-------------------------------------------------------------
void  GEOM_AssemblyBuilder::SwitchDisplayMode(vtkAssembly* aOCCAssembly)
{
}

void  GEOM_AssemblyBuilder::SwitchDisplayMode(vtkActorCollection* aOCCAssembly)
{
}

//-------------------------------------------------------------
// DISPLAY/ERASE
//-------------------------------------------------------------

void GEOM_AssemblyBuilder::DisplayErase(vtkAssembly* mySALOMEAssembly)
{
}


void GEOM_AssemblyBuilder::DisplayErase(vtkActorCollection* mySALOMEActors)
{
}





