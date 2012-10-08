// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//  $Header$
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

  void createAndDisplayGO(const char* Entry, bool isUpdated =true);
  void eraseGO(const char* Entry, bool allWindows);
  void createAndDisplayFitAllGO(const char* Entry);
  void UpdateViewer();
  int  getIndexTopology(const char *SubEntry, const char *Entry);
  const char* getShapeTypeString(const char *Entry);

  void setDisplayMode(const char* Entry, int mode, bool isUpdated =true);
  void setVectorsMode(const char* Entry, bool isSet, bool isUpdated =true);
  void setColor(const char* Entry, int red, int green, int blue, bool isUpdated =true);
  void setTransparency(const char* Entry, float transp, bool isUpdated =true);
  void setIsos(const char* Entry, int nbU, int nbV, bool isUpdated =true);
  void setDeflection(const char* Entry, float deflect);
  const char* getShapeTypeIcon(const char *Ior);

  bool initGeomGen();
};

