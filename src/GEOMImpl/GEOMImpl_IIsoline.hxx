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

//NOTE: This is an interface to a function for the isoline creation algorithm.

#include "GEOM_Function.hxx"

#define ISOLINE_ARG_FACE       1
#define ISOLINE_ARG_U_OR_V     2
#define ISOLINE_ARG_PARAMETER  3

class GEOMImpl_IIsoline
{
 public:

  GEOMImpl_IIsoline(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetFace (Handle(GEOM_Function) theFace)
      { _func->SetReference(ISOLINE_ARG_FACE, theFace); }
  void SetIsUIso (bool IsUIso)
      { _func->SetInteger(ISOLINE_ARG_U_OR_V, IsUIso ? 1 : 0); }
  void SetParameter (double theParameter)
      { _func->SetReal(ISOLINE_ARG_PARAMETER,  theParameter); }


  Handle(GEOM_Function) GetFace()
      { return _func->GetReference(ISOLINE_ARG_FACE); }
  bool GetIsUIso()
    { return (_func->GetInteger(ISOLINE_ARG_U_OR_V) != 0); }
  double GetParameter() { return _func->GetReal(ISOLINE_ARG_PARAMETER ); }

 private:

  Handle(GEOM_Function) _func;
};
