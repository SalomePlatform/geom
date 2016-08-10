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

#ifndef GEOM_PAINTERPOLYDATAMAPPER_H 
#define GEOM_PAINTERPOLYDATAMAPPER_H 

#ifndef VTK_OPENGL2
#include <vtkPainterPolyDataMapper.h>
#else
#include <vtkPolyDataMapper.h>
#endif

/*
 * This class can be used to prevent drawing of mappers' content (due to an
 * empty definition of GEOM_PainterPolyDataMapper::RenderPiece(...) method).
 * It is used as poly data mapper in GEOM_Actor class.
 */
class GEOM_PainterPolyDataMapper:
#ifndef VTK_OPENGL2
public vtkPainterPolyDataMapper
#else
public vtkPolyDataMapper
#endif
{
 public:
#ifndef VTK_OPENGL2
  vtkTypeMacro(GEOM_PainterPolyDataMapper,vtkPainterPolyDataMapper);
#else
  vtkTypeMacro(GEOM_PainterPolyDataMapper,vtkPolyDataMapper);
#endif


  static GEOM_PainterPolyDataMapper* New();

  virtual void RenderPiece(vtkRenderer *ren, vtkActor *act) {}

 protected:
  GEOM_PainterPolyDataMapper() {}
  ~GEOM_PainterPolyDataMapper() {}

};

#endif //GEOM_PAINTERPOLYDATAMAPPER_H
