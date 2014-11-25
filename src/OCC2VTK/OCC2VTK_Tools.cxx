// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "OCC2VTK_Tools.h" 

#include "GEOM_VertexSource.h" 
#include "GEOM_EdgeSource.h" 
#include "GEOM_WireframeFace.h" 
#include "GEOM_ShadingFace.h"

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListOfShape.hxx>


#include <TopExp.hxx>
#include <vtkAppendPolyData.h>
#include <vtkPolyData.h>
#include <BRepBuilderAPI_Copy.hxx>

#define MAX2(X, Y)    (Abs(X) > Abs(Y) ? Abs(X) : Abs(Y))
#define MAX3(X, Y, Z) (MAX2(MAX2(X,Y), Z))


#define DEFAULT_DEFLECTION 0.001

namespace GEOM
{
  void MeshShape(const TopoDS_Shape theShape,
                 float& theDeflection,
                 bool theForced ) {
    
    Standard_Real aDeflection = theDeflection <= 0 ? DEFAULT_DEFLECTION : theDeflection;
    
    //If deflection <= 0, than return default deflection
    if(theDeflection <= 0)
      theDeflection = aDeflection;  
    
    // Is shape triangulated?
    Standard_Boolean alreadymeshed = Standard_True;
    TopExp_Explorer ex;
    TopLoc_Location aLoc;
    for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next()) {
      const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
      Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
      if(aPoly.IsNull()) { 
        alreadymeshed = Standard_False; 
        break; 
      }
    }

    if(!alreadymeshed || theForced) {
      Bnd_Box B;
      BRepBndLib::Add(theShape, B);
      if ( B.IsVoid() )
        return; // NPAL15983 (Bug when displaying empty groups) 
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

      // This magic line comes from Prs3d_ShadedShape.gxx in OCCT
      aDeflection = MAX3(aXmax-aXmin, aYmax-aYmin, aZmax-aZmin) * aDeflection * 4;
      
      //Clean triangulation before compute incremental mesh
      BRepTools::Clean(theShape);
      
      //Compute triangulation
      BRepMesh_IncrementalMesh MESH(theShape,aDeflection); 
    }
  }

  void SetShape(const TopoDS_Shape& theShape,
                const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeMap,
                bool theIsVector,
                GEOM_VertexSource* theStandaloneVertexSource,
                GEOM_EdgeSource* theIsolatedEdgeSource,
                GEOM_EdgeSource* theOneFaceEdgeSource,
                GEOM_EdgeSource* theSharedEdgeSource,
                GEOM_WireframeFace* theWireframeFaceSource,
                GEOM_ShadingFace* theShadingFaceSource)
  {
    if (theShape.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator anItr(theShape);
      for (; anItr.More(); anItr.Next()) {
        SetShape(anItr.Value(),theEdgeMap,theIsVector,
                 theStandaloneVertexSource,
                 theIsolatedEdgeSource,
                 theOneFaceEdgeSource,
                 theSharedEdgeSource,
                 theWireframeFaceSource,
                 theShadingFaceSource);
      }
    }

    switch (theShape.ShapeType()) {
      case TopAbs_WIRE: {
        TopExp_Explorer anEdgeExp(theShape,TopAbs_EDGE);
        for (; anEdgeExp.More(); anEdgeExp.Next()){
          const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
          if (!BRep_Tool::Degenerated(anEdge))
            theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
        }
        break;
      }
      case TopAbs_EDGE: {
        const TopoDS_Edge& anEdge = TopoDS::Edge(theShape);
        if (!BRep_Tool::Degenerated(anEdge))
          theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
        break;
      }
      case TopAbs_VERTEX: {
        if ( theStandaloneVertexSource ) {
          const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
          theStandaloneVertexSource->AddVertex(aVertex);
        }
        break;
      }
      default: {
        TopExp_Explorer aFaceExp (theShape,TopAbs_FACE);
        for(; aFaceExp.More(); aFaceExp.Next()) {
          const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
          theWireframeFaceSource->AddFace(aFace);
          theShadingFaceSource->AddFace(aFace);
          TopExp_Explorer anEdgeExp(aFaceExp.Current(), TopAbs_EDGE);
          for(; anEdgeExp.More(); anEdgeExp.Next()) {
            const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeExp.Current());
            if(!BRep_Tool::Degenerated(anEdge)){
              // compute the number of faces
              int aNbOfFaces = theEdgeMap.FindFromKey(anEdge).Extent();
              switch(aNbOfFaces){
              case 0:  // isolated edge
                theIsolatedEdgeSource->AddEdge(anEdge,theIsVector);
                break;
              case 1:  // edge in only one face
                theOneFaceEdgeSource->AddEdge(anEdge,theIsVector);
                break;
              default: // edge shared by at least two faces
                theSharedEdgeSource->AddEdge(anEdge,theIsVector);
              }
            }
          }
        }
      }
    }
  }

  vtkPolyData* GetData(const TopoDS_Shape& theShape, float theDeflection) {
    BRepBuilderAPI_Copy aCopy(theShape);
    if(!aCopy.IsDone()) {
      return 0;
    }

    TopoDS_Shape aShape = aCopy.Shape();
    
    try {
      GEOM_VertexSource* myVertexSource = GEOM_VertexSource::New();
      GEOM_EdgeSource* myIsolatedEdgeSource = GEOM_EdgeSource::New();
      GEOM_EdgeSource* myOneFaceEdgeSource = GEOM_EdgeSource::New();
      GEOM_EdgeSource* mySharedEdgeSource = GEOM_EdgeSource::New();
      GEOM_WireframeFace* myWireframeFaceSource = GEOM_WireframeFace::New();
      GEOM_ShadingFace* myShadingFaceSource = GEOM_ShadingFace::New();
      
      vtkAppendPolyData* myAppendFilter = vtkAppendPolyData::New();
      myAppendFilter->AddInputConnection( myVertexSource->GetOutputPort() );
      myAppendFilter->AddInputConnection( myIsolatedEdgeSource->GetOutputPort() );
      myAppendFilter->AddInputConnection( myOneFaceEdgeSource->GetOutputPort() );
      myAppendFilter->AddInputConnection( mySharedEdgeSource->GetOutputPort() );      
      myAppendFilter->AddInputConnection( myShadingFaceSource->GetOutputPort() );
      
      bool anIsVector = false;
      
      GEOM::MeshShape( aShape, theDeflection );
      TopExp_Explorer aVertexExp( aShape, TopAbs_VERTEX );
      for( ; aVertexExp.More(); aVertexExp.Next() ) {
	const TopoDS_Vertex& aVertex = TopoDS::Vertex( aVertexExp.Current() );
	myVertexSource->AddVertex( aVertex );
      }
      
      TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
      TopExp::MapShapesAndAncestors( aShape, TopAbs_EDGE, TopAbs_FACE, anEdgeMap );
      
      GEOM::SetShape( aShape,
		      anEdgeMap,
		      anIsVector,
		      0,
		      myIsolatedEdgeSource,
		      myOneFaceEdgeSource,
		      mySharedEdgeSource,
		      myWireframeFaceSource,
		      myShadingFaceSource );
      
      myAppendFilter->Update();

      myVertexSource->Delete();
      myIsolatedEdgeSource->Delete();
      myOneFaceEdgeSource->Delete();
      mySharedEdgeSource->Delete();
      myWireframeFaceSource->Delete();
      myShadingFaceSource->Delete();

      vtkPolyData* ret = vtkPolyData::New();
      ret->ShallowCopy(myAppendFilter->GetOutput());
      myAppendFilter->Delete();
      return ret;
    }
    catch(Standard_Failure) {
      return 0;
    }
  }  
}
