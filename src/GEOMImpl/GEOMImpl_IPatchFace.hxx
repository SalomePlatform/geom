// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

//NOTE: This is an interface to a function for the patch face.
//
#include "GEOM_Function.hxx"

#include <TColStd_HSequenceOfTransient.hxx>

#define PATCHFACE_ARG_FACE 1

class GEOMImpl_IPatchFace
{
 public:

  GEOMImpl_IPatchFace(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theRef) 
  {
    _func->SetReference(PATCHFACE_ARG_FACE, theRef);
  }

  Handle(GEOM_Function) GetShape()
  {
    return _func->GetReference(PATCHFACE_ARG_FACE);
  }

 private:

  Handle(GEOM_Function) _func;
};
