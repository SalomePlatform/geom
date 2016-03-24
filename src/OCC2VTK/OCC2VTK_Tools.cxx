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

#include "OCC2VTK_Tools.h" 

#include "GEOM_VertexSource.h" 
#include "GEOM_EdgeSource.h" 
#include "GEOM_WireframeFace.h" 
#include "GEOM_ShadingFace.h"
#include "GEOMUtils.hxx"

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

namespace GEOM
{
  void ShapeToVTK( const TopoDS_Shape& theShape,
                   const TopTools_IndexedDataMapOfShapeListOfShape& theEdgeMap,
                   bool theIsVector,
                   GEOM_VertexSource* theStandaloneVertexSource,
                   GEOM_EdgeSource* theIsolatedEdgeSource,
                   GEOM_EdgeSource* theOneFaceEdgeSource,
                   GEOM_EdgeSource* theSharedEdgeSource,
                   GEOM_WireframeFace* theWireframeFaceSource,
                   GEOM_ShadingFace* theShadingFaceSource )
  {
    if (theShape.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator anItr(theShape);
      for (; anItr.More(); anItr.Next()) {
        ShapeToVTK( anItr.Value(),theEdgeMap,theIsVector,
                    theStandaloneVertexSource,
                    theIsolatedEdgeSource,
                    theOneFaceEdgeSource,
                    theSharedEdgeSource,
                    theWireframeFaceSource,
                    theShadingFaceSource );
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

  vtkPolyData* GetVTKData( const TopoDS_Shape& theShape, float theDeflection )
  {
    vtkPolyData* ret = 0;

    BRepBuilderAPI_Copy aCopy(theShape);
    if (aCopy.IsDone() ) {

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
        
        GEOMUtils::MeshShape( aShape, theDeflection );
        TopExp_Explorer aVertexExp( aShape, TopAbs_VERTEX );
        for( ; aVertexExp.More(); aVertexExp.Next() ) {
          const TopoDS_Vertex& aVertex = TopoDS::Vertex( aVertexExp.Current() );
          myVertexSource->AddVertex( aVertex );
        }
      
        TopTools_IndexedDataMapOfShapeListOfShape anEdgeMap;
        TopExp::MapShapesAndAncestors( aShape, TopAbs_EDGE, TopAbs_FACE, anEdgeMap );
        
        ShapeToVTK( aShape,
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
        
        ret = vtkPolyData::New();
        ret->ShallowCopy(myAppendFilter->GetOutput());
        myAppendFilter->Delete();
      }
      catch(Standard_Failure) {
      }
    }
    return ret;
  }
}
