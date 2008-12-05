//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//NOTE: This is an intreface to a function for the Prism creation.
//
#include "GEOM_Function.hxx"

#define PRISM_ARG_H     1
#define PRISM_ARG_VEC   2
#define PRISM_ARG_BASE  3
#define PRISM_ARG_PNT_F 4
#define PRISM_ARG_PNT_L 5
#define PRISM_ARG_DX    6
#define PRISM_ARG_DY    7
#define PRISM_ARG_DZ    8

class GEOMImpl_IPrism
{
 public:

  GEOMImpl_IPrism(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetBase  (Handle(GEOM_Function) theRefBase  ) { _func->SetReference(PRISM_ARG_BASE, theRefBase); }
  void SetVector(Handle(GEOM_Function) theRefVector) { _func->SetReference(PRISM_ARG_VEC , theRefVector); }
  void SetFirstPoint(Handle(GEOM_Function) thePoint) { _func->SetReference(PRISM_ARG_PNT_F, thePoint); }
  void SetLastPoint (Handle(GEOM_Function) thePoint) { _func->SetReference(PRISM_ARG_PNT_L, thePoint); }

  void SetDX(double theDX) { _func->SetReal(PRISM_ARG_DX, theDX); }
  void SetDY(double theDY) { _func->SetReal(PRISM_ARG_DY, theDY); }
  void SetDZ(double theDZ) { _func->SetReal(PRISM_ARG_DZ, theDZ); }

  double GetDX() { return _func->GetReal(PRISM_ARG_DX); }
  double GetDY() { return _func->GetReal(PRISM_ARG_DY); }
  double GetDZ() { return _func->GetReal(PRISM_ARG_DZ); }

  Handle(GEOM_Function) GetBase  () { return _func->GetReference(PRISM_ARG_BASE); }
  Handle(GEOM_Function) GetVector() { return _func->GetReference(PRISM_ARG_VEC ); }
  Handle(GEOM_Function) GetFirstPoint() { return _func->GetReference(PRISM_ARG_PNT_F ); }
  Handle(GEOM_Function) GetLastPoint () { return _func->GetReference(PRISM_ARG_PNT_L ); }

  void SetH(double theH) { _func->SetReal(PRISM_ARG_H, theH); }

  double GetH() { return _func->GetReal(PRISM_ARG_H); }

 private:

  Handle(GEOM_Function) _func;
};
