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
//  File   : GeometryGUI_Swig.hxx
//  Author : Nicolas REJNERI, Paul RASCLE
//  Module : GEOM
//  $Header$

#ifndef _GeometryGUI_SWIG_HXX_
#define _GeometryGUI_SWIG_HXX_

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define GEOMGUI_WNT_EXPORT __declspec( dllexport )
#else
#define GEOMGUI_WNT_EXPORT
#endif

class GEOM_Client;

class GEOMGUI_WNT_EXPORT GEOM_Swig
{
public:
  GEOM_Swig();
  ~GEOM_Swig();

  void createAndDisplayGO(const char* Entry);
  void eraseGO(const char* Entry, bool allWindows);
  void createAndDisplayFitAllGO(const char* Entry);
  void setDisplayMode(const char* Entry, int mode);
  void setColor(const char* Entry, int red, int green, int blue);
  void setTransparency(const char* Entry, float transp);
  void setDeflection(const char* Entry, float deflect);

  int  getIndexTopology(const char *SubEntry, const char *Entry);
  const char* getShapeTypeString(const char *Entry);
  const char* getShapeTypeIcon(const char *Ior);

  bool initGeomGen();
};


#endif
