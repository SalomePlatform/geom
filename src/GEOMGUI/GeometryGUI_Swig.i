//  File      : GeometryGUI_Swig.i
//  Created   : Tue Mar 26 15:08:38 2002
//  Author    : Nicolas REJNERI, Paul RASCLE
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE, EDF 2002
//  $Header$

%{
#include "GeometryGUI_Swig.hxx"
%}

class GeometryGUI_Swig
{
 public:
  GeometryGUI_Swig();
  ~GeometryGUI_Swig();

  void createAndDisplayGO(const char* Entry);
  int  getIndexTopology(const char *SubEntry, const char *Entry);
  const char* getShapeTypeString(const char *Entry);

  void setDisplayMode(const char* Entry, int mode);
  void setColor(const char* Entry, int red, int green, int blue);
  void setTransparency(const char* Entry, float transp);
  const char* getShapeTypeIcon(const char *Ior);
};
