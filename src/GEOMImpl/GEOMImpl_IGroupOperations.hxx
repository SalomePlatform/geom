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

#ifndef _GEOMImpl_IGroupOperations_HXX_
#define _GEOMImpl_IGroupOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TopAbs.hxx>

class GEOMImpl_IGroupOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IGroupOperations(GEOM_Engine* theEngine);
  Standard_EXPORT ~GEOMImpl_IGroupOperations();

  Standard_EXPORT Handle(GEOM_Object) CreateGroup(Handle(GEOM_Object) theMainShape, TopAbs_ShapeEnum  theShapeType);

  Standard_EXPORT void AddObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  Standard_EXPORT void RemoveObject(Handle(GEOM_Object) theGroup, int theSubShapeID);

  Standard_EXPORT void UnionList (Handle(GEOM_Object) theGroup,
                  const Handle(TColStd_HSequenceOfTransient)& theSubShapes);

  Standard_EXPORT void DifferenceList (Handle(GEOM_Object) theGroup,
                       const Handle(TColStd_HSequenceOfTransient)& theSubShapes);

  Standard_EXPORT void UnionIDs (Handle(GEOM_Object) theGroup,
                 const Handle(TColStd_HSequenceOfInteger)& theSubShapes);

  Standard_EXPORT void DifferenceIDs (Handle(GEOM_Object) theGroup,
                      const Handle(TColStd_HSequenceOfInteger)& theSubShapes);

  Standard_EXPORT Handle(GEOM_Object) UnionGroups (Handle(GEOM_Object) theGroup1,
                                                   Handle(GEOM_Object) theGroup2);
  Standard_EXPORT Handle(GEOM_Object) IntersectGroups (Handle(GEOM_Object) theGroup1,
                                                       Handle(GEOM_Object) theGroup2);
  Standard_EXPORT Handle(GEOM_Object) CutGroups (Handle(GEOM_Object) theGroup1,
                                                 Handle(GEOM_Object) theGroup2);

  Standard_EXPORT Handle(GEOM_Object) UnionListOfGroups
                                                      (const Handle(TColStd_HSequenceOfTransient)& theGList);
  Standard_EXPORT Handle(GEOM_Object) IntersectListOfGroups
                                                      (const Handle(TColStd_HSequenceOfTransient)& theGList);
  Standard_EXPORT Handle(GEOM_Object) CutListOfGroups (const Handle(TColStd_HSequenceOfTransient)& theGList1,
                                                       const Handle(TColStd_HSequenceOfTransient)& theGList2);

  Standard_EXPORT TopAbs_ShapeEnum GetType(Handle(GEOM_Object) theGroup);

  Standard_EXPORT Handle(GEOM_Object) GetMainShape(Handle(GEOM_Object) theGroup);

  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetObjects(Handle(GEOM_Object) theGroup);

};

#endif
