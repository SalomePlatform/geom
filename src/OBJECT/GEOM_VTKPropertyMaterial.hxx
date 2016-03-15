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

//  SALOME GEOM : 
//  File   : GEOM_VTKPropertyMaterial.hxx

#ifndef GEOM_VTKPropertyMaterial_H
#define GEOM_VTKPropertyMaterial_H

#include "GEOM_OBJECT_defs.hxx"

#include <vtkProperty.h>

class GEOM_OBJECT_EXPORT GEOM_VTKPropertyMaterial : public vtkProperty
{

protected:
                                    GEOM_VTKPropertyMaterial();
  virtual                           ~GEOM_VTKPropertyMaterial();

public:
                                    vtkTypeMacro(GEOM_VTKPropertyMaterial, vtkProperty);

                                    vtkSetMacro(Physical, bool);
                                    vtkGetMacro(Physical, bool);

  static GEOM_VTKPropertyMaterial*  New();

protected:
  bool                              Physical;
};

#endif
