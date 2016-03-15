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

#ifndef _AdvancedEngine_ISmoothingSurface_HXX_
#define _AdvancedEngine_ISmoothingSurface_HXX_

#include "GEOM_Function.hxx"

#define SMOOTHINGSURFACE_ARG_LENG        1
#define SMOOTHINGSURFACE_ARG_LAST        2
#define SMOOTHINGSURFACE_ARG_NB_MAX      3
#define SMOOTHINGSURFACE_ARG_DEG_MAX     4
#define SMOOTHINGSURFACE_ARG_D_MAX       5

class AdvancedEngine_ISmoothingSurface
{
public:
  AdvancedEngine_ISmoothingSurface(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetLength(int theLen) { _func->SetInteger(SMOOTHINGSURFACE_ARG_LENG, theLen); }
  int GetLength() { return _func->GetInteger(SMOOTHINGSURFACE_ARG_LENG); }

  void SetPntOrComp(int theId, Handle(GEOM_Function) theP) { _func->SetReference(SMOOTHINGSURFACE_ARG_LAST + theId, theP); }
  Handle(GEOM_Function) GetPntOrComp(int theId) { return _func->GetReference(SMOOTHINGSURFACE_ARG_LAST + theId); }

  void SetNbMax(int theNbMax) { _func->SetInteger(SMOOTHINGSURFACE_ARG_NB_MAX, theNbMax); }
  int GetNbMax() { return _func->GetInteger(SMOOTHINGSURFACE_ARG_NB_MAX); }

  void SetDegMax(int theDegMax) { _func->SetInteger(SMOOTHINGSURFACE_ARG_DEG_MAX, theDegMax); }
  int GetDegMax() { return _func->GetInteger(SMOOTHINGSURFACE_ARG_DEG_MAX); }

  void SetDMax(double theDMax) { _func->SetReal(SMOOTHINGSURFACE_ARG_D_MAX, theDMax); }
  double GetDMax() { return _func->GetReal(SMOOTHINGSURFACE_ARG_D_MAX); }

private:
  Handle(GEOM_Function) _func;
};

#endif // _AdvancedEngine_ISmoothingSurface_HXX_
