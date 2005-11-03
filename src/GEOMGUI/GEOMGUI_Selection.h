//  GEOMGUI_Selection
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
//  File   : GEOMGUI_Selection.h
//  Author : Alexander SOLOVYOV
//  Module : GUI
//  $Header$

#ifndef GEOMGUI_SELECTION_HeaderFile
#define GEOMGUI_SELECTION_HeaderFile

#include <LightApp_Selection.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class GEOMGUI_Selection : public LightApp_Selection
{
public:
  GEOMGUI_Selection();
  virtual ~GEOMGUI_Selection();

  virtual QtxValue      param( const int, const QString& ) const;
  virtual QtxValue      globalParam( const QString& ) const;

private:
  bool                  isVisible( const int ) const;
  QString               typeName( const int ) const;  
  QString               displayMode( const int ) const;

  bool                  isComponent( const int ) const;
  GEOM::GEOM_Object_ptr getObject( const int ) const;
};

#endif
