// Copyright (C) 2007-2023  CEA, EDF, OPEN CASCADE
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
// File   : libGEOM_Swig.h
// Author : Nicolas REJNERI, Paul RASCLE

#ifndef LIBGEOM_SWIG_H
#define LIBGEOM_SWIG_H

#include "GEOM_GEOMGUI.hxx"
#include <vector>

class GEOM_Swig_LocalSelector;
class GEOMGUI_OCCSelector;

class GEOM_Swig
{
public:
  GEOMGUI_EXPORT GEOM_Swig( bool updateOB = true );
  GEOMGUI_EXPORT ~GEOM_Swig();

  GEOMGUI_EXPORT void createAndDisplayGO( const char* theEntry, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void createAndDisplayFitAllGO( const char* theEntry );
  GEOMGUI_EXPORT void eraseGO( const char* theEntry, bool theUpdateViewer = true );

  GEOMGUI_EXPORT void UpdateViewer();

  GEOMGUI_EXPORT void setDisplayMode( const char* theEntry, int theMode, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setVectorsMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setVerticesMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setNameMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setColor( const char* theEntry, int theRed, int theGreen, int theBlue, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setTransparency( const char* theEntry, float theTransparency, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setIsos( const char* theEntry, int theNbU, int theNbV, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setDeflection( const char* theEntry, float theDeflection, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setMaterial( const char* theEntry, const char* theMaterial, bool theUpdateViewer = true );
  GEOMGUI_EXPORT void setMaterialProperty( const char* theEntry, const char* theMaterial, bool theUpdateViewer = true );

  GEOMGUI_EXPORT void initLocalSelection( const char* theEntry, int theMode);
  GEOMGUI_EXPORT std::vector<int> getLocalSelection();
  GEOMGUI_EXPORT void closeLocalSelection();

  GEOMGUI_EXPORT int         getIndexTopology( const char* theSubIOR, const char* theMainIOR );
  GEOMGUI_EXPORT const char* getShapeTypeString( const char* theIOR );
  GEOMGUI_EXPORT const char* getShapeTypeIcon( const char* theIOR );

  GEOMGUI_EXPORT bool initGeomGen();

  static GEOM_Swig_LocalSelector* myLocalSelector;
  static GEOMGUI_OCCSelector* myOCCSelector;

private:
  void init( bool updateOB );
};

#endif // LIBGEOM_SWIG_H
