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

#ifndef _GEOM_TopWireframeShape_HeaderFile
#define _GEOM_TopWireframeShape_HeaderFile

#include <SALOME_AISShape.hxx>

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

class GEOM_TopWireframeShape : public SALOME_AISShape
{
public:
  Standard_EXPORT GEOM_TopWireframeShape(const TopoDS_Shape& shape);
  Standard_EXPORT ~GEOM_TopWireframeShape();

  Standard_EXPORT virtual Standard_Boolean hasIO();
  Standard_EXPORT void setIO(const Handle(SALOME_InteractiveObject)& io);
  Standard_EXPORT virtual Handle(SALOME_InteractiveObject) getIO();

  Standard_EXPORT virtual void setName(const Standard_CString aName);
  Standard_EXPORT virtual Standard_CString getName();

  Standard_EXPORT virtual Standard_Boolean isTopLevel();
  Standard_EXPORT virtual void setTopLevel(Standard_Boolean);
  Standard_EXPORT virtual Standard_Boolean switchTopLevel();

  Standard_EXPORT virtual Standard_Boolean toActivate();

  Standard_EXPORT virtual void highlightSubShapes(const TColStd_IndexedMapOfInteger& aIndexMap, const Standard_Boolean aHighlight );

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(GEOM_TopWireframeShape,SALOME_AISShape)
};

DEFINE_STANDARD_HANDLE(GEOM_TopWireframeShape, SALOME_AISShape)

#endif
