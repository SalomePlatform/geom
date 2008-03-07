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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GeometryGUI_Swig.i
//  Author : Nicolas REJNERI, Paul RASCLE
//  Module : GEOM
//  $Header$

%{
#include "GeometryGUI_Swig.hxx"
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
  void setColor(const char* Entry, int red, int green, int blue, bool isUpdated =true);
  void setTransparency(const char* Entry, float transp, bool isUpdated =true);
  void setDeflection(const char* Entry, float deflect);
  const char* getShapeTypeIcon(const char *Ior);

  bool initGeomGen();
};
