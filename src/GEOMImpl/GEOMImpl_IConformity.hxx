// Copyright (C) 2013-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMImpl_IConformity_HXX_
#define _GEOMImpl_IConformity_HXX_

#include "GEOM_Function.hxx"
#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfTransient.hxx>

#define CHECKCONFORMITY_ARG_SHAPE 1
#define CHECKCONFORMITY_ARG_SHAPETYPE_1 2
#define CHECKCONFORMITY_ARG_SHAPETYPE_2 3
#define CHECKCONFORMITY_ARG_TOLERANCE 4

#define CHECKCONFORMITY_RET_TOLERANCE 5
#define CHECKCONFORMITY_RET_ISVALID 6
#define CHECKCONFORMITY_RET_SHAPES_IDS 7
#define CHECKCONFORMITY_RET_TYPES_CHECKS 8
#define CHECKCONFORMITY_RET_LIST_OF_INDEXES 9
#define CHECKCONFORMITY_RET_LIST_SHAPES 10

class GEOMImpl_IConformity
{
public:
  GEOMImpl_IConformity(Handle(GEOM_Function) theFunction)
    : _func(theFunction)
  {
  }

  void SetShape(Handle(GEOM_Function) theRef)
  {
    _func->SetReference(CHECKCONFORMITY_ARG_SHAPE, theRef);
  }
  Handle(GEOM_Function) GetShape()
  {
    return _func->GetReference(CHECKCONFORMITY_ARG_SHAPE);
  }

  void SetShapeType1(int theShapeType) { _func->SetInteger(CHECKCONFORMITY_ARG_SHAPETYPE_1, theShapeType); }
  Standard_Integer GetShapeType1() { return _func->GetInteger(CHECKCONFORMITY_ARG_SHAPETYPE_1); }
  void SetShapeType2(int theShapeType) { _func->SetInteger(CHECKCONFORMITY_ARG_SHAPETYPE_2, theShapeType); }
  Standard_Integer GetShapeType2() { return _func->GetInteger(CHECKCONFORMITY_ARG_SHAPETYPE_2); }
  void SetTolerance(double theTolerance) { _func->SetReal(CHECKCONFORMITY_ARG_TOLERANCE, theTolerance); }
  Standard_Real GetTolerance() { return _func->GetReal(CHECKCONFORMITY_ARG_TOLERANCE); }

  void SetListOfShapesIndices(const Handle(TColStd_HArray2OfInteger)& theArrayOfIndexes);
  Handle(TColStd_HArray2OfInteger) GetListOfShapesIndices();

private:
  Handle(GEOM_Function) _func;
};

#endif // _GEOMImpl_IConformity_HXX_
