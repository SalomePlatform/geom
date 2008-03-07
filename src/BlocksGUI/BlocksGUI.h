//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  CEA
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
//  File   : BuildGUI.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM

#ifndef BLOCKSGUI_H
#define BLOCKSGUI_H

#include "GEOM_BlocksGUI.hxx"

#include "GEOMGUI.h"

//=================================================================================
// class    : BlocksGUI
// purpose  :
//=================================================================================
class GEOM_BLOCKSGUI_EXPORT BlocksGUI : public GEOMGUI
{
 public:
  BlocksGUI( GeometryGUI* parent );
  ~BlocksGUI();

  bool OnGUIEvent (int theCommandID, SUIT_Desktop* parent);
};

#endif
