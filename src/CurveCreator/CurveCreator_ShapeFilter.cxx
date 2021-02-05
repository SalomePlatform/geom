// Copyright (C) 2013-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:        CurveCreator_ShapeFilter.cxx
// Author:      Ilya Shchekin


#include "CurveCreator_ShapeFilter.hxx"

#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CurveCreator_ShapeFilter,SelectMgr_Filter)

CurveCreator_ShapeFilter::CurveCreator_ShapeFilter()
: SelectMgr_Filter()
{
}

CurveCreator_ShapeFilter::~CurveCreator_ShapeFilter()
{
}

Standard_Boolean CurveCreator_ShapeFilter::ActsOn(const TopAbs_ShapeEnum aType) const
{
  return (aType == TopAbs_VERTEX);
}

Standard_Boolean CurveCreator_ShapeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO) const
{
  Handle(StdSelect_BRepOwner) aBO = Handle(StdSelect_BRepOwner)::DownCast(EO);
  if (aBO.IsNull())
    return Standard_False;

  const TopoDS_Shape& aShape = aBO->Shape();

  if(aShape.ShapeType()!= TopAbs_VERTEX)
    return Standard_False;

  return myShapes.Contains(aShape);
}

void CurveCreator_ShapeFilter::AddShape(const TopoDS_Shape& aShape)
{
  myShapes.Add(aShape);  
}

void CurveCreator_ShapeFilter::RemoveShape(const TopoDS_Shape& aShape)
{
  myShapes.Remove(aShape);
}

void CurveCreator_ShapeFilter::ClearShapes()
{
  myShapes.Clear();
}

