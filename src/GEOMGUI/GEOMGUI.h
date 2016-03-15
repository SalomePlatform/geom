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
// File   : GEOMGUI.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef GEOMGUI_H
#define GEOMGUI_H

#include "GEOM_GEOMGUI.hxx"

#include <QObject>

class SUIT_Desktop;
class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;
class GeometryGUI;

//=================================================================================
// class    : GEOMGUI
// purpose  : Base class for all geometry GUI subclasses
//=================================================================================
class GEOMGUI_EXPORT GEOMGUI : public QObject
{
public :
  // Constructor
  GEOMGUI( GeometryGUI* parent );
  // Destructor
  virtual ~GEOMGUI();

  // Each of this methods can be redifined by descendants
  virtual bool OnGUIEvent( int theCommandID, SUIT_Desktop* parent );
  virtual bool OnGUIEvent( int theCommandID, SUIT_Desktop* parent, const QVariant& theParam  );

  virtual bool OnMousePress( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );
  virtual bool OnMouseMove( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );
  virtual bool OnMouseRelease( QMouseEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );
  virtual bool OnKeyPress( QKeyEvent* pe, SUIT_Desktop* parent, SUIT_ViewWindow* );

  virtual void activate( SUIT_Desktop* parent );
  virtual void deactivate();

  GeometryGUI*  getGeometryGUI();

private:
  GeometryGUI*  myGeometryGUI;
};

#endif // GEOMGUI_H

