//  File      : GeometryGUI_Swig.hxx
//  Created   : Tue Mar 26 15:08:38 2002
//  Author    : Nicolas REJNERI, Paul RASCLE
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE, EDF 2002
//  $Header$

#ifndef _GeometryGUI_SWIG_HXX_
#define _GeometryGUI_SWIG_HXX_

// IDL Headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

class GEOM_Client;

class GEOM_Swig
{
public:
  GEOM_Swig();
  ~GEOM_Swig();

  void createAndDisplayGO(const char* Entry);
  void setDisplayMode(const char* Entry, int mode);
  void setColor(const char* Entry, int red, int green, int blue);
  void setTransparency(const char* Entry, float transp);

  int  getIndexTopology(const char *SubEntry, const char *Entry);
  const char* getShapeTypeString(const char *Entry);
  const char* getShapeTypeIcon(const char *Ior);
};


#endif
