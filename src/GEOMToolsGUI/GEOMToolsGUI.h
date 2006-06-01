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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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

#include <SALOMEDSClient.hxx>
#include <SALOME_ListIO.hxx>

#include <CORBA.h>
#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define GEOMTOOLSGUI_WNT_EXPORT __declspec( dllexport )
#else
#define GEOMTOOLSGUI_WNT_EXPORT
#endif
//=================================================================================
// class    : GEOMToolsGUI
// purpose  :
//=================================================================================
class GEOMTOOLSGUI_WNT_EXPORT GEOMToolsGUI : public GEOMGUI
{
public :
  GEOMToolsGUI( GeometryGUI* ); // hide constructor to avoid direct creation
  ~GEOMToolsGUI();

  bool OnGUIEvent( int theCommandID, SUIT_Desktop* parent );

private:
  // Import and export topology methods
  bool Import();
  bool Export();

  void OnEditCopy();
  void OnEditDelete();

  void OnSettingsColor();
  void OnSettingsIsos();
  void OnSettingsStep();
  void OnRename();
  void OnCheckGeometry();

  // Popup commands
  void OnColor();
  void OnTransparency();
  void OnNbIsos();
  void OnOpen();

  // returns name of Module (Component) of given objects (usually selected objects)
  // if objects belong to different Components, a NULL string is returned.
  QString getParentComponent( _PTR( Study ), const SALOME_ListIO& );
  QString getParentComponent( _PTR(SObject) );
};

#endif
