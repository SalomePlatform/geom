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

#define EXTRACT_SHAPE         1
#define EXTRACT_IDS           2
#define EXTRACT_REMOVED_IDS   3
#define EXTRACT_MODIFIED_IDS  4
#define EXTRACT_ADDED_IDS     5

class GEOMImpl_IExtract
{
 public:

  GEOMImpl_IExtract(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetShape(Handle(GEOM_Function) theShape)
    { _func->SetReference(EXTRACT_SHAPE, theShape); }

  Handle(GEOM_Function) GetShape()
    { return _func->GetReference(EXTRACT_SHAPE); }

  void SetSubShapeIDs(const Handle(TColStd_HArray1OfInteger)& theSubShapeIDs)
    { _func->SetIntegerArray(EXTRACT_IDS, theSubShapeIDs); }

  Handle(TColStd_HArray1OfInteger) GetSubShapeIDs()
    { return _func->GetIntegerArray(EXTRACT_IDS); }

  void SetRemovedIDs(const Handle(TColStd_HArray1OfInteger)& theRemovedIDs)
    { _func->SetIntegerArray(EXTRACT_REMOVED_IDS, theRemovedIDs); }

  Handle(TColStd_HArray1OfInteger) GetRemovedIDs()
    { return _func->GetIntegerArray(EXTRACT_REMOVED_IDS); }

  void SetModifiedIDs(const Handle(TColStd_HArray1OfInteger)& theModifiedIDs)
    { _func->SetIntegerArray(EXTRACT_MODIFIED_IDS, theModifiedIDs); }

  Handle(TColStd_HArray1OfInteger) GetModifiedIDs()
    { return _func->GetIntegerArray(EXTRACT_MODIFIED_IDS); }

  void SetAddedIDs(const Handle(TColStd_HArray1OfInteger)& theAddedIDs)
    { _func->SetIntegerArray(EXTRACT_ADDED_IDS, theAddedIDs); }

  Handle(TColStd_HArray1OfInteger) GetAddedIDs()
    { return _func->GetIntegerArray(EXTRACT_ADDED_IDS); }

 private:

  Handle(GEOM_Function) _func;
};
