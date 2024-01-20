// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#ifndef GEOM_VERTEXSOURCE_H 
#define GEOM_VERTEXSOURCE_H 
 
#include "OCC2VTK.h" 
 
#include <TopoDS_Vertex.hxx> 
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx> 
 
typedef NCollection_Map<TopoDS_Vertex, TopTools_ShapeMapHasher> TVertexSet; 
 
#include <vtkPoints.h> 
#include <vtkPolyDataAlgorithm.h> 

class vtkPolyData;

class OCC2VTK_EXPORT GEOM_VertexSource: public vtkPolyDataAlgorithm
{ 
public: 
  vtkTypeMacro(GEOM_VertexSource,vtkPolyDataAlgorithm)
  static GEOM_VertexSource* New(); 
 
  void AddVertex(const TopoDS_Vertex& theVertex); 
  void Clear(){ myVertexSet.Clear();} 
 
  static  
  void OCC2VTK(const TopoDS_Vertex& theVertex,  
               vtkPolyData* thePolyData, 
               vtkPoints* thePts); 
 
protected: 
  TVertexSet myVertexSet; 
 
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
 
  GEOM_VertexSource(); 
  ~GEOM_VertexSource(); 
 
private: 
  // Not implememnted 
  GEOM_VertexSource(const GEOM_VertexSource&); 
  void operator=(const GEOM_VertexSource&); 
}; 
 
 
#endif //GEOM_VERTEXSOURCE_H 
