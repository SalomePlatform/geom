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
//  File   : GEOMGUI.h
//  Author : Vadim SANDLER
//  Module : GEOM
//  $Header$

#if !defined( __GEOMGUI_H )
#define __GEOMGUI_H

#include <qobject.h>

class SUIT_Desktop;
class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;
class GeometryGUI;

//#ifdef WNT
//#include <SALOME_WNT.hxx>
//#else
//#define SALOME_WNT_EXPORT
//#endif
#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define GEOMGUI_WNT_EXPORT __declspec( dllexport )
#else
#define GEOMGUI_WNT_EXPORT
#endif
//=================================================================================
// class    : GEOMGUI
// purpose  : Base class for all geometry GUI subclasses
//=================================================================================
class GEOMGUI_WNT_EXPORT GEOMGUI : public QObject
{
public :
  // Constructor
  GEOMGUI( GeometryGUI* parent );
  // Destructor
  virtual ~GEOMGUI();

  // Each of this methods can be redifined by descendants
  virtual bool OnGUIEvent( int theCommandID, SUIT_Desktop* parent );

  virtual bool OnMousePress( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );
  virtual bool OnMouseMove( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );
  virtual bool OnKeyPress( QKeyEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );

  virtual void activate( SUIT_Desktop* parent );
  virtual void deactivate();

  GeometryGUI*  getGeometryGUI();

private:
  GeometryGUI*  myGeometryGUI;
};

#endif // __GEOMGUI_H

