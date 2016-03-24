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

//NOTE: This is an intreface to a function for the Offset creation.
//
#include "GEOM_Function.hxx"
#include <TColStd_HArray1OfInteger.hxx>

#define OFF_ARG_SHAPE 1
#define OFF_ARG_VALUE 2
#define OFF_ARG_IDS   3
#define OFF_ARG_PARAM 4

class GEOMImpl_IOffset
{
 public:

  GEOMImpl_IOffset(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape) { _func->SetReference(OFF_ARG_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape() { return _func->GetReference(OFF_ARG_SHAPE); }

  void SetValue(double theValue) { _func->SetReal(OFF_ARG_VALUE, theValue); }

  double GetValue() { return _func->GetReal(OFF_ARG_VALUE); }

  void SetParam(Standard_Boolean theParam) { _func->SetInteger(OFF_ARG_PARAM, theParam ? 1 : 0); }

  Standard_Boolean GetParam() { return _func->GetInteger(OFF_ARG_PARAM); }

  void SetFaceIDs(const Handle(TColStd_HArray1OfInteger)& theFaceIDs)
    { _func->SetIntegerArray(OFF_ARG_IDS, theFaceIDs); }

  Handle(TColStd_HArray1OfInteger) GetFaceIDs()
    { return _func->GetIntegerArray(OFF_ARG_IDS); }

 private:

  Handle(GEOM_Function) _func;
};
