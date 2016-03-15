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
// File   : MeasureGUI.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_H
#define MEASUREGUI_H

#include <GEOMGUI.h>

class QDialog;

//=================================================================================
// class    : MeasureGUI
// purpose  :
//=================================================================================

#define DISPLAY_PREVIEW_MACRO \
SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr(); \
int w = resMgr->integerValue("Geometry", "measures_line_width", 1); \
displayPreview(true, false, true, true, w, -1, -1);

class MeasureGUI : public GEOMGUI
{
  Q_OBJECT
public:
  MeasureGUI( GeometryGUI* ); 
  ~MeasureGUI();

  bool OnGUIEvent( int, SUIT_Desktop* );

  // Show/hide all dimension created for object
  void ChangeDimensionsVisibility( const bool theIsVisible );
private:
  QDialog* myManageDimensionDlg;
private slots:
  void onFinished(int);
};

#endif // MEASUREGUI_H
