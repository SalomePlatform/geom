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
//  File   : GEOMToolsGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef GEOMTOOLSGUI_H
#define GEOMTOOLSGUI_H

#include "GEOMGUI.h"

//=================================================================================
// class    : GEOMToolsGUI
// purpose  :
//=================================================================================
class GEOMToolsGUI : public GEOMGUI
{
protected:
  GEOMToolsGUI(); // hide constructor to avoid direct creation

public :
  ~GEOMToolsGUI();

  // Get the only GEOMToolsGUI object
  static GEOMToolsGUI* GetGEOMToolsGUI();

  bool OnGUIEvent(int theCommandID, QAD_Desktop* parent);

private:
  /* Import and export topology methods */
  bool Import();
  bool Export(); 
  
  void OnEditCopy();
  void OnEditDelete();

  void OnSettingsColor();
  void OnSettingsIsos();
  void OnSettingsStep();
  void OnRename();
  void OnCheckGeometry();
  void OnLoadScript();

  // Popup commands
  void OnColor();
  void OnTransparency();
  void OnNbIsos();
  void OnOpen();

private:
  static GEOMToolsGUI* myGUIObject;        // the only GEOMToolsGUI object
};

#endif
