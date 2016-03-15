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
// File   : DisplayGUI.h
// Author : Damien COQUERET, Open CASCADE S.A.S.

#ifndef DISPLAYGUI_H
#define DISPLAYGUI_H

#include "GEOMGUI.h"

//=================================================================================
// class    : DisplayGUI
// purpose  :
//=================================================================================

class SUIT_ViewWindow;

class DisplayGUI : public GEOMGUI
{
public:
  DisplayGUI( GeometryGUI* );
  ~DisplayGUI();

  // Dispatch menu command
  bool OnGUIEvent( int, SUIT_Desktop* );

  // Display all GEOM objects
  void DisplayAll();
  // Erase all GEOM objects
  void EraseAll();
  // Display selected GEOM objects
  void Display();
  // Display selected GEOM objects and erase other
  void DisplayOnly();
  // Display only children of selected GEOM objects and erase other
  void DisplayOnlyChildren();
  // Erase selected GEOM objects
  void Erase();

  // DISPLAY MODE methods : 0 - wireframe, 1 - shading
  // Set display mode for the viewer (current viewer if <viewWindow> - 0 )
  void SetDisplayMode( const int, SUIT_ViewWindow* = 0 );

  // VECTOR MODE methods
  // Set vectror mode for the viewer
  void SetVectorMode( const bool, SUIT_ViewWindow* = 0 );
  // Get vector mode of the viewer
  int  GetVectorMode( SUIT_ViewWindow* = 0 );
  // Invert vector mode ( shadin <-> wireframe ) for the viewer 

  // VERTICES MODE methods
  // Set vertices mode for the viewer
  void SetVerticesMode( const bool, SUIT_ViewWindow* = 0 );
  // Get vertices mode of the viewer
  int  GetVerticesMode( SUIT_ViewWindow* = 0 );
  
  // NAME MODE methods
  // Set name mode for the viewer
  void SetNameMode( const bool, SUIT_ViewWindow* = 0 );
  // Get name mode of the viewer
  int  GetNameMode( SUIT_ViewWindow* = 0 );
  
  // Set display mode for selected objects in the viewer given
  // (current viewer if <viewWindow> = 0 )
  void ChangeDisplayMode( const int, SUIT_ViewWindow* = 0 );
};

#endif // DISPLAYGUI_H
