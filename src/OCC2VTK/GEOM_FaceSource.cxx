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

#include "GEOM_FaceSource.h" 
 
#include <vtkObjectFactory.h> 

#include <vtkPoints.h>
#include <vtkCellArray.h>
 
#include <vtkPolyDataMapper.h>  
#include <vtkPolyData.h>  
 
GEOM_FaceSource::GEOM_FaceSource() 
{ 
  this->SetNumberOfInputPorts(0);
} 
 
GEOM_FaceSource::~GEOM_FaceSource() 
{ 
} 
 
void  
GEOM_FaceSource:: 
AddFace(const TopoDS_Face& theFace) 
{ 
  myFaceSet.Add(theFace); 
} 
 
void 
GEOM_FaceSource::
MoveTo(gp_Pnt thePnt, 
       vtkPoints* thePts)
{    
  thePts->InsertNextPoint(thePnt.X(), thePnt.Y(), thePnt.Z());
} 

void 
GEOM_FaceSource::
DrawTo(gp_Pnt thePnt,
       vtkPolyData* thePolyData,
       vtkPoints* thePts)
{
  vtkIdType anId = 
    thePts->InsertNextPoint(thePnt.X(), thePnt.Y(), thePnt.Z());
  vtkIdType anIds[2] = {anId-1, anId};
  thePolyData->InsertNextCell(VTK_LINE,2,anIds);
}
