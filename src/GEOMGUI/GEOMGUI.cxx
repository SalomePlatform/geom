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
//  File   : GEOMGUI.cxx
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#include "GEOMGUI.h"

#include "QAD_Desktop.h"
#include "QAD_StudyFrame.h"
#include "QAD_Application.h"
#include <qevent.h>

//=================================================================================
// class    : GEOMGUI::GEOMGUI
// purpose  : Constructor
//=================================================================================
GEOMGUI::GEOMGUI( QObject* parent ) : 
  QObject ( parent )
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
bool GEOMGUI::OnGUIEvent( int /*theCommandID*/, QAD_Desktop* /*parent*/ )
{
  return TRUE;
}

//=================================================================================
// class    : GEOMGUI::OnMousePress
// purpose  : Mouse press event processing. Should return FALSE to let the event 
//            be processed further.
//=================================================================================
bool GEOMGUI::OnMousePress( QMouseEvent* /*pe*/, QAD_Desktop* /*parent*/, QAD_StudyFrame* /*studyFrame*/ )
{
  return FALSE;
}

//=================================================================================
// class    : GEOMGUI::OnMouseMove
// purpose  : Mouse move event processing
//=================================================================================
bool GEOMGUI::OnMouseMove( QMouseEvent* /*pe*/, QAD_Desktop* /*parent*/, QAD_StudyFrame* /*studyFrame*/ )
{
  return TRUE;
}

//=================================================================================
// class    : GEOMGUI::OnKeyPress
// purpose  : Key press event processing
//=================================================================================
bool GEOMGUI::OnKeyPress( QKeyEvent* /*pe*/, QAD_Desktop* /*parent*/, QAD_StudyFrame* /*studyFrame*/ )
{
  return TRUE;
}

//=================================================================================
// class    : GEOMGUI::SetSettings
// purpose  : Called when GEOM component is activated
//=================================================================================
bool GEOMGUI::SetSettings( QAD_Desktop* /*parent*/ )
{
  return TRUE;
}

//=================================================================================
// class    : GEOMGUI::Deactivate
// purpose  : Called when GEOM component is deactivated
//=================================================================================
void GEOMGUI::Deactivate()
{
}

//=================================================================================
// class    : GEOMGUI::ActiveStudyChanged
// purpose  : Called when active study changed
//=================================================================================
bool GEOMGUI::ActiveStudyChanged( QAD_Desktop* /*parent*/ )
{
  return TRUE;
}

