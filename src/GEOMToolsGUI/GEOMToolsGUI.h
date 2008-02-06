// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : GEOMToolsGUI.h
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#ifndef GEOMTOOLSGUI_H
#define GEOMTOOLSGUI_H

#include <GEOMGUI.h>

#include <SALOMEDSClient.hxx>

class GEOM_Displayer;
class SALOME_View;
class SALOME_ListIO;

#include <QList>

//=================================================================================
// class    : GEOMToolsGUI
// purpose  :
//=================================================================================
class GEOMToolsGUI : public GEOMGUI
{
public:
  GEOMToolsGUI( GeometryGUI* ); // hide constructor to avoid direct creation
  ~GEOMToolsGUI();

  bool         OnGUIEvent( int, SUIT_Desktop* );
  virtual void deactivate();

private:
  // Import and export topology methods
  bool         Import();
  bool         Export();

  void         OnEditCopy();
  void         OnEditDelete();

  void         OnSettingsColor();
  void         OnRename();
  void         OnCheckGeometry();

  // Popup commands
  void         OnAutoColor();
  void         OnDisableAutoColor();
  void         OnColor();
  void         OnTransparency();
  void         OnNbIsos();
  void         OnOpen();
  void         OnSelectOnly(int mode);
  
  // returns name of Module (Component) of given objects (usually selected objects)
  // if objects belong to different Components, a NULL string is returned.
  QString      getParentComponent( _PTR(Study), const SALOME_ListIO& );
  QString      getParentComponent( _PTR(SObject) );

  // Recursive deletion of object with children
  void         RemoveObjectWithChildren( _PTR(SObject),
					 _PTR(Study),
					 QList<SALOME_View*>,
					 GEOM_Displayer* );
  
  //checks if the object passed as the first argument depends on the second arguments
  bool         CheckSubObjectInUse( _PTR(SObject),
				    _PTR(SObject),
				    _PTR(Study) );
};

#endif // GEOMTOOLSGUI_H
