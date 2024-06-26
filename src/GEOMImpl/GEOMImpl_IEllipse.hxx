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

//NOTE: This is an interface to a function for the Ellipse creation.
//
#include "GEOM_Function.hxx"

#define ELLIPS_ARG_CC    1
#define ELLIPS_ARG_VV    2
#define ELLIPS_ARG_RMAJ  3
#define ELLIPS_ARG_RMIN  4
#define ELLIPS_ARG_VVMAJ 5

class GEOMImpl_IEllipse
{
 public:

  GEOMImpl_IEllipse(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetCenter(Handle(GEOM_Function) theP) { _func->SetReference(ELLIPS_ARG_CC, theP); }
  void SetVector(Handle(GEOM_Function) theV) { _func->SetReference(ELLIPS_ARG_VV, theV); }

  void SetRMajor(double theR) { _func->SetReal(ELLIPS_ARG_RMAJ, theR); }
  void SetRMinor(double theR) { _func->SetReal(ELLIPS_ARG_RMIN, theR); }

  void SetVectorMajor(Handle(GEOM_Function) theV) { _func->SetReference(ELLIPS_ARG_VVMAJ, theV); }

  Handle(GEOM_Function) GetCenter() { return _func->GetReference(ELLIPS_ARG_CC); }
  Handle(GEOM_Function) GetVector() { return _func->GetReference(ELLIPS_ARG_VV); }

  double GetRMajor() { return _func->GetReal(ELLIPS_ARG_RMAJ); }
  double GetRMinor() { return _func->GetReal(ELLIPS_ARG_RMIN); }

  Handle(GEOM_Function) GetVectorMajor() { return _func->GetReference(ELLIPS_ARG_VVMAJ); }

 private:

  Handle(GEOM_Function) _func;
};
