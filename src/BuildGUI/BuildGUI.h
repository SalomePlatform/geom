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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : BuildGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef BUILDGUI_H
#define BUILDGUI_H

#include "GEOMGUI.h"

//=================================================================================
// class    : BuildGUI
// purpose  :
//=================================================================================
class BuildGUI : public GEOMGUI
{
protected:
  BuildGUI(); // hide constructor to avoid direct creation

public :
  ~BuildGUI();

  // Get the only BuildGUI object
  static BuildGUI* GetBuildGUI();

  bool OnGUIEvent( int theCommandID, QAD_Desktop* parent );

private:
  static BuildGUI* myGUIObject;        // the only BuildGUI object
};

#endif
