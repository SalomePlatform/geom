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
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
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
//  File   : BasicGUI.h
//  Author : Damien COQUERET
//  Module : GEOM

#ifndef BASICGUI_H
#define BASICGUI_H

#include "GEOMGUI.h"
#include "GEOMBase.h"
#include <V3d_View.hxx>

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define BASICGUI_WNT_EXPORT __declspec( dllexport )
#else
#define BASICGUI_WNT_EXPORT
#endif

//=================================================================================
// class    : BasicGUI
// purpose  :
//=================================================================================
class BasicGUI : public GEOMGUI
{
protected:
  BasicGUI( GeometryGUI* parent ); // hide constructor to avoid direct creation

public:
  ~BasicGUI();

  // Get the only BasicGUI object
  static BasicGUI* GetBasicGUI( GeometryGUI* parent );

  bool OnGUIEvent(int theCommandID, SUIT_Desktop* parent);
  bool OnMousePress(QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* theViewWindow);

  gp_Pnt ConvertClickToPoint( int x, int y, Handle(V3d_View) aView );
};

#endif
