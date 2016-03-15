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
// File   : GEOMGUI.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "GEOMGUI.h"
#include "GeometryGUI.h"

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>

//=================================================================================
// class    : GEOMGUI::GEOMGUI
// purpose  : Constructor
//=================================================================================
GEOMGUI::GEOMGUI( GeometryGUI* parent ) : 
QObject ( parent ),
myGeometryGUI( parent )
{
}

//=================================================================================
// class    : GEOMGUI::GEOMGUI
// purpose  : Destructor
//=================================================================================
GEOMGUI::~GEOMGUI()
{
}

//=================================================================================
// class    : GEOMGUI::OnGUIEvent
// purpose  : Main/popup menu events processing
//=================================================================================
bool GEOMGUI::OnGUIEvent( int /*theCommandID*/, SUIT_Desktop* /*parent*/, const QVariant& /*theParam*/  )
{
  return true;
}

//=================================================================================
// class    : GEOMGUI::OnGUIEvent
// purpose  : Main/popup menu events processing
//=================================================================================
bool GEOMGUI::OnGUIEvent( int /*theCommandID*/, SUIT_Desktop* /*parent*/ )
{
  return true;
}

//=================================================================================
// class    : GEOMGUI::OnMousePress
// purpose  : Mouse press event processing. Should return false to let the event 
//            be processed further.
//=================================================================================
bool GEOMGUI::OnMousePress( QMouseEvent* /*pe*/, SUIT_Desktop* /*parent*/, SUIT_ViewWindow* /*studyFrame*/ )
{
  return false;
}

//=================================================================================
// class    : GEOMGUI::OnMouseMove
// purpose  : Mouse move event processing
//=================================================================================
bool GEOMGUI::OnMouseMove( QMouseEvent* /*pe*/, SUIT_Desktop* /*parent*/, SUIT_ViewWindow* /*studyFrame*/ )
{
  return true;
}

//=================================================================================
// class    : GEOMGUI::OnMouseMove
// purpose  : Mouse move event processing
//=================================================================================
bool GEOMGUI::OnMouseRelease( QMouseEvent* /*pe*/, SUIT_Desktop* /*parent*/, SUIT_ViewWindow* /*studyFrame*/ )
{
  return true;
}

//=================================================================================
// class    : GEOMGUI::OnKeyPress
// purpose  : Key press event processing
//=================================================================================
bool GEOMGUI::OnKeyPress( QKeyEvent* /*pe*/, SUIT_Desktop* /*parent*/, SUIT_ViewWindow* /*studyFrame*/ )
{
  return true;
}

//=================================================================================
// class    : GEOMGUI::SetSettings
// purpose  : Called when GEOM component is activated
//=================================================================================
void GEOMGUI::activate( SUIT_Desktop* /*parent*/ )
{
}

//=================================================================================
// class    : GEOMGUI::Deactivate
// purpose  : Called when GEOM component is deactivated
//=================================================================================
void GEOMGUI::deactivate()
{
}

//=================================================================================
// class    : GEOMGUI::Deactivate
// purpose  : Called when GEOM component is deactivated
//=================================================================================
GeometryGUI* GEOMGUI::getGeometryGUI()
{
  return myGeometryGUI;
}
