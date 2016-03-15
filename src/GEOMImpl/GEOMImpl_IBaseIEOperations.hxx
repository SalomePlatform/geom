// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef _GEOMImpl_IBaseIEOperations_HXX_
#define _GEOMImpl_IBaseIEOperations_HXX_

// GEOM includes
#include <GEOM_IOperations.hxx>

// OCC includes
#include <NCollection_List.hxx>

class GEOMImpl_IGroupOperations;
class GEOMImpl_IFieldOperations;
class GEOMImpl_IShapesOperations;

class GEOMImpl_IBaseIEOperations : public GEOM_IOperations
{
public:
  Standard_EXPORT GEOMImpl_IBaseIEOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IBaseIEOperations();
  
protected:
  Standard_EXPORT void MakeMaterialGroups(const Handle(GEOM_Object)                  &theObject,
                          const Handle(TColStd_HSequenceOfTransient) &theSeq);

  Handle(GEOM_Object) MakeGroup
                         (const Handle(GEOM_Object)            &theObject,
                          const TCollection_ExtendedString     &theName,
                          const NCollection_List<TopoDS_Shape> &theShapes);
protected:
  GEOMImpl_IGroupOperations*  myGroupOperations;
  GEOMImpl_IFieldOperations*  myFieldOperations;
  GEOMImpl_IShapesOperations* myShapesOperations;
};

#endif
