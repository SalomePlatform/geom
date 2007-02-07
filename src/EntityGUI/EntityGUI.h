//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : EntityGUI.h
//  Author : Damien COQUERET
//  Module : GEOM

#ifndef ENTITYGUI_H
#define ENTITYGUI_H

#include "GEOM_EntityGUI.hxx"

#include "GEOMGUI.h"

#include "SALOMEDSClient.hxx"

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

//=================================================================================
// class    : EntityGUI
// purpose  :
//=================================================================================
class GEOM_ENTITYGUI_EXPORT EntityGUI : public GEOMGUI
{
public :
  EntityGUI( GeometryGUI* parent ); // hide constructor to avoid direct creation
  ~EntityGUI();

  bool OnGUIEvent(int theCommandID, SUIT_Desktop* parent);

  void DisplaySimulationShape(const TopoDS_Shape& S1, const TopoDS_Shape& S2); 
  void EraseSimulationShape();

  // Methods for sub shapes explode
  bool SObjectExist(const _PTR(SObject)& theFatherObject, const char* IOR);

  // AIS shape used only during topo/geom simulations
  Handle(AIS_Shape) mySimulationShape1;
  Handle(AIS_Shape) mySimulationShape2;
};

#endif
