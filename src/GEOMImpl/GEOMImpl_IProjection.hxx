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

//NOTE: This is an interface to a function for the Projection creation.

#include "GEOM_Function.hxx"

#define PROJECTION_ARG_POINT      1
#define PROJECTION_ARG_SHAPE      2
#define PROJECTION_ARG_PARAMETER  3
#define PROJECTION_ARG_INDEX      4

class GEOMImpl_IProjection
{
 public:

  GEOMImpl_IProjection(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetPoint (Handle(GEOM_Function) thePoint) { _func->SetReference(PROJECTION_ARG_POINT, thePoint); }
  void SetShape (Handle(GEOM_Function) theShape) { _func->SetReference(PROJECTION_ARG_SHAPE, theShape); }
  void SetU     (double                theU)     { _func->SetReal(PROJECTION_ARG_PARAMETER,  theU); }
  void SetIndex (int                   theIndex) { _func->SetInteger(PROJECTION_ARG_INDEX,   theIndex); }

  Handle(GEOM_Function) GetPoint() { return _func->GetReference(PROJECTION_ARG_POINT); }
  Handle(GEOM_Function) GetShape() { return _func->GetReference(PROJECTION_ARG_SHAPE); }
  double                GetU()     { return _func->GetReal(PROJECTION_ARG_PARAMETER ); }
  int                   GetIndex() { return _func->GetInteger(PROJECTION_ARG_INDEX); }
  
 private:

  Handle(GEOM_Function) _func;
};
