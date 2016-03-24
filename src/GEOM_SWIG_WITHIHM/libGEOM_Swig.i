// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File      : libGeometry_Swig.i
//  Created   : Tue Mar 26 15:04:48 2002
//  Author    : Nicolas REJNERI, Paul RASCLE
//  Project   : SALOME
//  Module    : GEOM
//
%module libGEOM_Swig

%{
#include "libGEOM_Swig.h"
%}

/*
  managing C++ exception in the Python API
*/
%exception
{
  class PyAllowThreadsGuard {
   public:
    // Py_BEGIN_ALLOW_THREADS
    PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
    // Py_END_ALLOW_THREADS
    ~PyAllowThreadsGuard() { PyEval_RestoreThread(_save); }
   private:
    PyThreadState *_save;
  };

  PyAllowThreadsGuard guard;

  $action
}

class GEOM_Swig
{
 public:
  GEOM_Swig();
  ~GEOM_Swig();

  void createAndDisplayGO( const char* theEntry, bool theUpdateViewer = true );
  void createAndDisplayFitAllGO( const char* theEntry );
  void eraseGO( const char* theEntry, bool theUpdateViewer = true );

  void UpdateViewer();

  void setDisplayMode( const char* theEntry, int theMode, bool theUpdateViewer = true );
  void setVectorsMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  void setVerticesMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  void setNameMode( const char* theEntry, bool theOn, bool theUpdateViewer = true );
  void setColor( const char* theEntry, int theRed, int theGreen, int theBlue, bool theUpdateViewer = true );
  void setTransparency( const char* theEntry, float theTransparency, bool theUpdateViewer = true );
  void setIsos( const char* theEntry, int theNbU, int theNbV, bool theUpdateViewer = true );
  void setDeflection( const char* theEntry, float theDeflection, bool theUpdateViewer = true );
  void setMaterial( const char* theEntry, const char* theMaterial, bool theUpdateViewer = true );
  void setMaterialProperty( const char* theEntry, const char* theMaterial, bool theUpdateViewer = true );

  int         getIndexTopology( const char* theSubIOR, const char* theMainIOR );
  const char* getShapeTypeString( const char* theIOR );
  const char* getShapeTypeIcon( const char* theIOR );

  bool initGeomGen();
};

