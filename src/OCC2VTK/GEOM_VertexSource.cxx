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

#include "GEOM_VertexSource.h" 
 
#include <vtkObjectFactory.h> 

#include <vtkPoints.h> 
#include <vtkCellArray.h> 
#include <vtkPolyData.h> 
#include <vtkPolyDataMapper.h> 
#include <vtkInformation.h>
#include <vtkInformationVector.h>
 
#include <gp_Pnt.hxx>
#include <BRep_Tool.hxx>
 
vtkStandardNewMacro(GEOM_VertexSource)
 
GEOM_VertexSource::GEOM_VertexSource() 
{ 
  this->SetNumberOfInputPorts(0);
} 
 
GEOM_VertexSource::~GEOM_VertexSource() 
{ 
} 
 
void  
GEOM_VertexSource:: 
AddVertex(const TopoDS_Vertex& theVertex) 
{ 
  myVertexSet.Add(theVertex); 
} 
 
int GEOM_VertexSource::RequestData(vtkInformation *vtkNotUsed(request),
                                   vtkInformationVector **vtkNotUsed(inputVector),
                                   vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkPolyData *aPolyData = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  aPolyData->Allocate();
  vtkPoints* aPts = vtkPoints::New();
  aPolyData->SetPoints(aPts);
  aPts->Delete();

  TVertexSet::Iterator anIter(myVertexSet);
  for(; anIter.More(); anIter.Next()){
    const TopoDS_Vertex& aVertex = anIter.Value();
    OCC2VTK(aVertex,aPolyData,aPts);
  }
  return 1;
}

void  
GEOM_VertexSource:: 
OCC2VTK(const TopoDS_Vertex& theVertex,  
        vtkPolyData* thePolyData, 
        vtkPoints* thePts) 
{ 
  gp_Pnt aPnt = BRep_Tool::Pnt(theVertex);
  vtkIdType anId = thePts->InsertNextPoint(aPnt.X(),aPnt.Y(),aPnt.Z());
  thePolyData->InsertNextCell(VTK_VERTEX,1,&anId);
}
