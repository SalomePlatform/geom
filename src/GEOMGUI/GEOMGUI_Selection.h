//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File   : GEOMGUI_Selection.h
// Author : Alexander SOLOVYOV, Open CASCADE S.A.S. (alexander.solovyov@opencascade.com)
//
#ifndef GEOMGUI_SELECTION_H
#define GEOMGUI_SELECTION_H

#include "GEOM_GEOMGUI.hxx"

#include <LightApp_Selection.h>
#include <SALOMEDSClient.hxx>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

class LightApp_SelectionMgr;
class LightApp_DataOwner;

class GEOMGUI_EXPORT GEOMGUI_Selection : public LightApp_Selection
{
public:
  GEOMGUI_Selection();
  virtual ~GEOMGUI_Selection();

  virtual QVariant      parameter( const int, const QString& ) const;
  virtual QVariant      parameter( const QString& ) const;
  
  virtual bool          processOwner( const LightApp_DataOwner* );

  static bool           hasChildren( const _PTR(SObject)& );
  static bool           expandable( const _PTR(SObject)& );
  static bool           isCompoundOfVertices( GEOM::GEOM_Object_ptr );

private:
  bool                  isVisible( const int ) const;
  bool                  isAutoColor( const int ) const;
  QString               typeName( const int ) const;  
  int                   typeId( const int ) const;  
  QString               displayMode( const int ) const;
  QString               selectionMode() const;
  bool                  isVectorsMode( const int ) const;
  bool                  hasHiddenChildren( const int ) const;
  bool                  hasShownChildren( const int ) const;
  bool                  compoundOfVertices( const int ) const;

  bool                  isComponent( const int ) const;
  GEOM::GEOM_Object_ptr getObject( const int ) const;
};

#endif // GEOMGUI_SELECTION_H
