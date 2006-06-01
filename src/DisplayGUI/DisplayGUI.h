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
//  File   : DisplayGUI.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef DISPLAYGUI_H
#define DISPLAYGUI_H

#include "GEOMGUI.h"
#include "GEOMBase.h"
//#ifdef WNT
//#include <SALOME_WNT.hxx>
//#else
//#define SALOME_WNT_EXPORT
//#endif
#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define DISPLAYGUI_WNT_EXPORT __declspec( dllexport )
#else
#define DISPLAYGUI_WNT_EXPORT
#endif
//=================================================================================
// class    : GEOMBase_Display
// purpose  :
//=================================================================================
//class QAD_ViewFrame;
class SUIT_ViewWindow;
class DisplayGUI : public GEOMGUI
{
protected:
  DisplayGUI( GeometryGUI* parent ); // hide constructor to avoid direct creation

public :
  ~DisplayGUI();

  // Get the only DisplayGUI object
  static DisplayGUI* GetDisplayGUI( GeometryGUI* parent );

  // Dispatch menu command
  bool OnGUIEvent(int theCommandID, SUIT_Desktop* parent);

  // Display all GEOM objects
  void DisplayAll();
  // Erase all GEOM objects
  void EraseAll();
  // Display selected GEOM objects
  void Display();
  // Display selected GEOM objects and erase other
  void DisplayOnly();
  // Erase selected GEOM objects
  void Erase();

  // DISPLAY MODE methods : 0 - wireframe, 1 - shading
  // Set display mode for the viewer (current viewer if <viewWindow> - 0 )
  void SetDisplayMode( const int mode, SUIT_ViewWindow* viewWindow = 0 );
  // Get display mode of the viewer (current viewer if <viewWindow> - 0 )
  int  GetDisplayMode( SUIT_ViewWindow* viewWindow = 0 );
  // Invert display mode ( shadin <-> wireframe ) for the viewer 
  // (current viewer if <viewWindow> = 0 )
  void InvertDisplayMode( SUIT_ViewWindow* viewWindow = 0 );

  // Set display mode for selected objects in the viewer given
  // (current viewer if <viewWindow> = 0 )
  void ChangeDisplayMode( const int mode, SUIT_ViewWindow* viewWindo = 0 );

private:
  static DisplayGUI* myGUIObject;        // the only DisplayGUI object
};

#endif
