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
//  File   : GEOMGUI.h
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#if !defined( __GEOMGUI_H )
#define __GEOMGUI_H

#include <qobject.h>

class QAD_Desktop;
class QAD_StudyFrame;
class QMouseEvent;
class QKeyEvent;

//=================================================================================
// class    : GEOMGUI
// purpose  : Base class for all geometry GUI subclasses
//=================================================================================
class GEOMGUI : public QObject
{
public :
  // Constructor
  GEOMGUI( QObject* parent = 0 );
  // Destructor
  virtual ~GEOMGUI();

  // Each of this methods can be redifined by descendants
  virtual bool OnGUIEvent( int theCommandID, QAD_Desktop* parent );
  virtual bool OnMousePress( QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame );
  virtual bool OnMouseMove( QMouseEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame );
  virtual bool OnKeyPress( QKeyEvent* pe, QAD_Desktop* parent, QAD_StudyFrame* studyFrame );
  virtual bool SetSettings( QAD_Desktop* parent );
  virtual void Deactivate();
  virtual bool ActiveStudyChanged( QAD_Desktop* parent );
};

#endif // __GEOMGUI_H

