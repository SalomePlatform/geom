// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//NOTE: This is an interface to a function for the Box creation.
//
#include "GEOM_Function.hxx"

class GEOMImpl_IWrap
{
 public:

  enum {
    WRAP_ARG_EDGES         = 1, // list of edges
    WRAP_ARG_VERTICES      = 2, // list of vertices
    WRAP_TOLER             = 3, // tolerance
  };

  GEOMImpl_IWrap(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetEdges(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(WRAP_ARG_EDGES, theShapes); }

  Handle(TColStd_HSequenceOfTransient) GetEdges()
  { return _func->GetReferenceList(WRAP_ARG_EDGES); }

  void SetVertices(const Handle(TColStd_HSequenceOfTransient)& theShapes)
  { _func->SetReferenceList(WRAP_ARG_VERTICES, theShapes); }

  Handle(TColStd_HSequenceOfTransient) GetVertices()
  { return _func->GetReferenceList(WRAP_ARG_VERTICES); }

  void SetTolerance(double theTol) { _func->SetReal(WRAP_TOLER, theTol); }

  double GetTolerance() { return _func->GetReal(WRAP_TOLER); }

 private:

  Handle(GEOM_Function) _func;
};