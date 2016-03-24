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

//NOTE: This is an intreface to a function for the extended shape creation.
//
#include "GEOM_Function.hxx"

#define SHAPE_EXTEND_SHAPE  1
#define SHAPE_EXTEND_UMIN   2
#define SHAPE_EXTEND_UMAX   3
#define SHAPE_EXTEND_VMIN   4
#define SHAPE_EXTEND_VMAX   5

class GEOMImpl_IShapeExtend
{
 public:

  GEOMImpl_IShapeExtend(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape)
  { _func->SetReference(SHAPE_EXTEND_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape()
  { return _func->GetReference(SHAPE_EXTEND_SHAPE); }

  void SetUMin(const Standard_Real theUMin)
  { _func->SetReal(SHAPE_EXTEND_UMIN, theUMin); }

  double GetUMin()
  { return _func->GetReal(SHAPE_EXTEND_UMIN); }

  void SetUMax(const Standard_Real theUMax)
  { _func->SetReal(SHAPE_EXTEND_UMAX, theUMax); }

  double GetUMax()
  { return _func->GetReal(SHAPE_EXTEND_UMAX); }

  void SetVMin(const Standard_Real theVMin)
  { _func->SetReal(SHAPE_EXTEND_VMIN, theVMin); }

  double GetVMin()
  { return _func->GetReal(SHAPE_EXTEND_VMIN); }

  void SetVMax(const Standard_Real theVMax)
  { _func->SetReal(SHAPE_EXTEND_VMAX, theVMax); }

  double GetVMax()
  { return _func->GetReal(SHAPE_EXTEND_VMAX); }

 private:

  Handle(GEOM_Function) _func;
};
