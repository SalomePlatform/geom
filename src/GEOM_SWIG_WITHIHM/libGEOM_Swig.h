// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : libGEOM_Swig.h
// Author : Nicolas REJNERI, Paul RASCLE
//

#ifndef GEOMETRYGUI_SWIG_HXX
#define GEOMETRYGUI_SWIG_HXX

#include "GEOM_GEOMGUI.hxx"

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class GEOMGUI_EXPORT GEOM_Swig
{
public:
  GEOM_Swig();
  ~GEOM_Swig();

  void createAndDisplayGO(const char* Entry, bool isUpdated = true);
  void eraseGO(const char* Entry, bool allWindows);
  void createAndDisplayFitAllGO(const char* Entry);
  void UpdateViewer();
  void setDisplayMode(const char* Entry, int mode, bool isUpdated = true);
  void setVectorsMode(const char* Entry, bool isSet, bool isUpdated = true);
  void setColor(const char* Entry, int red, int green, int blue, bool isUpdated = true);
  void setTransparency(const char* Entry, float transp, bool isUpdated = true);
  void setIsos(const char* Entry, int nbU, int nbV, bool isUpdated =true);
  void setDeflection(const char* Entry, float deflect);

  int  getIndexTopology(const char *SubEntry, const char *Entry);
  const char* getShapeTypeString(const char *Entry);
  const char* getShapeTypeIcon(const char *Ior);

  bool initGeomGen();
};

#endif // GEOMETRYGUI_SWIG_HXX
