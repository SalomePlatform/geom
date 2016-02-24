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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_DimensionFilter.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_DIMENSIONFILTER_H
#define MEASUREGUI_DIMENSIONFILTER_H

// KERNEL includes
#include <Basics_OCCTVersion.hxx>

// GEOM includes
#include <GEOM_GenericObjPtr.h>

// OCCT includes
#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_Filter.hxx>
#include <Standard_DefineHandle.hxx>

DEFINE_STANDARD_HANDLE(MeasureGUI_DimensionFilter, SelectMgr_Filter)

//=====================================================================================
// class    : MeasureGUI_DimensionFilter
// purpose  : OCC viewer filter for selecting dimensions displayed as part of
//            presentation of particular GEOM object by its entry (owner of dimension).
//=====================================================================================
class MeasureGUI_DimensionFilter : public SelectMgr_Filter
{
public:

  OCCT_DEFINE_STANDARD_RTTIEXT(MeasureGUI_DimensionFilter,SelectMgr_Filter)

public:
  Standard_EXPORT MeasureGUI_DimensionFilter( const Handle(AIS_InteractiveObject)& theIO );
  Standard_EXPORT MeasureGUI_DimensionFilter( const int theId );
  Standard_EXPORT MeasureGUI_DimensionFilter( const std::string& theEntry );
  Standard_EXPORT MeasureGUI_DimensionFilter( const std::string& theEntry, const int theId );

  Standard_EXPORT void SetFilterIO( const Handle(AIS_InteractiveObject)& theIO );
  Standard_EXPORT void SetFilterEntry( const std::string& theEntry );
  Standard_EXPORT void SetFilterId( const int theId );

  Standard_Boolean IsFilterIO() const { return myIsFilterIO; }
  Standard_Boolean IsFilterEntry() const { return myIsFilterEntry; }
  Standard_Boolean IsFilterId() const { return myIsFilterId; }

  void UnsetFilterIO() { myIsFilterIO = Standard_False; }
  void UnsetFilterEntry() { myIsFilterEntry = Standard_False; }
  void UnsetFilterId() { myIsFilterId = Standard_False; }

  Standard_EXPORT virtual Standard_Boolean IsOk( const Handle(SelectMgr_EntityOwner)& theEntity ) const;

private:
  Handle(AIS_InteractiveObject) myFilterIO;
  std::string                   myFilterEntry;
  int                           myFilterId;
  Standard_Boolean              myIsFilterIO;
  Standard_Boolean              myIsFilterEntry;
  Standard_Boolean              myIsFilterId;
};

#endif
