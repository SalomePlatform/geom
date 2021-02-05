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

// File:        CurveCreator_ShapeFilter.hxx
// Author:      Ilya Shchekin


#ifndef _CurveCreator_ShapeFilter_HeaderFile
#define _CurveCreator_ShapeFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Basics_OCCTVersion.hxx>
#include <Standard_DefineHandle.hxx>
#include <SelectMgr_Filter.hxx>
#include <TopAbs_ShapeEnum.hxx> 
#include <TopTools_MapOfShape.hxx>

class SelectMgr_EntityOwner;
class TopoDS_Shape;


class CurveCreator_ShapeFilter : public SelectMgr_Filter
{
                          	
public:

  
  Standard_EXPORT CurveCreator_ShapeFilter();

  Standard_EXPORT ~CurveCreator_ShapeFilter();
  
  Standard_EXPORT virtual Standard_Boolean ActsOn (const TopAbs_ShapeEnum aType) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& EO) const Standard_OVERRIDE;
  
  Standard_EXPORT void AddShape (const TopoDS_Shape& aShape);
  
  Standard_EXPORT void RemoveShape (const TopoDS_Shape& aShape);

  Standard_EXPORT void ClearShapes ();


private:

  TopTools_MapOfShape myShapes;

public:

  DEFINE_STANDARD_RTTIEXT(CurveCreator_ShapeFilter,SelectMgr_Filter)

};

DEFINE_STANDARD_HANDLE(CurveCreator_ShapeFilter, SelectMgr_Filter)

#endif 
                                             	