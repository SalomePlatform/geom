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

//NOTE: This is an intreface to a function for the Common, Cut and Fuse creation.
//
#include "GEOM_Function.hxx"
#include "TColStd_HSequenceOfTransient.hxx"

#define BOOL_ARG_SHAPE1                   1
#define BOOL_ARG_SHAPE2                   2
#define BOOL_ARG_SHAPES                   3
#define BOOL_ARG_CHECK_SELF_INTERSECTION  4
#define BOOL_ARG_RM_EXTRA_EDGES           5

class GEOMImpl_IBoolean
{
 public:

  GEOMImpl_IBoolean(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape1(Handle(GEOM_Function) theRef) { _func->SetReference(BOOL_ARG_SHAPE1, theRef); }
  void SetShape2(Handle(GEOM_Function) theRef) { _func->SetReference(BOOL_ARG_SHAPE2, theRef); }
  void SetShapes(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(BOOL_ARG_SHAPES, theShapes); }
  void SetCheckSelfIntersection (Standard_Boolean theFlag)
  { _func->SetInteger(BOOL_ARG_CHECK_SELF_INTERSECTION, theFlag ? 1 : 0); }
  void SetRmExtraEdges (Standard_Boolean theFlag)
  { _func->SetInteger(BOOL_ARG_RM_EXTRA_EDGES, theFlag ? 1 : 0); }

  Handle(GEOM_Function) GetShape1() { return _func->GetReference(BOOL_ARG_SHAPE1); }
  Handle(GEOM_Function) GetShape2() { return _func->GetReference(BOOL_ARG_SHAPE2); }
  Handle(TColStd_HSequenceOfTransient) GetShapes()
  { return _func->GetReferenceList(BOOL_ARG_SHAPES); }
  Standard_Boolean GetCheckSelfIntersection()
  { return (_func->GetInteger(BOOL_ARG_CHECK_SELF_INTERSECTION) != 0); }
  Standard_Boolean GetRmExtraEdges()
  { return (_func->GetInteger(BOOL_ARG_RM_EXTRA_EDGES) != 0); }

 private:

  Handle(GEOM_Function) _func;
};
