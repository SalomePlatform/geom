// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GEOMToolsGUI.h
//  Author : Damien COQUERET, Open CASCADE S.A.S.
//
#ifndef GEOMTOOLSGUI_H
#define GEOMTOOLSGUI_H

#include "GEOM_ToolsGUI.hxx"

#include <GEOMGUI.h>
#include <SALOMEDSClient.hxx>

class GEOM_Displayer;
class SALOME_View;
class SALOME_ListIO;
class SalomeApp_Study;
class SVTK_View;
class QColor;
class SALOME_InteractiveObject;
class AIS_InteractiveContext;

#include <QList>

//=================================================================================
// class    : GEOMToolsGUI
// purpose  :
//=================================================================================
class GEOMTOOLSGUI_EXPORT GEOMToolsGUI : public GEOMGUI
{
public:
  GEOMToolsGUI( GeometryGUI* ); // hide constructor to avoid direct creation
  ~GEOMToolsGUI();

  bool         OnGUIEvent( int, SUIT_Desktop* );
  bool         OnGUIEvent( int theCommandID, SUIT_Desktop*, const QVariant& );

  virtual void deactivate();

  enum ActionType { SHOWDLG, INCR, DECR };

private:

  void         OnEditDelete();
  void         OnCheckGeometry();

  // Popup commands
  void         OnAutoColor();
  void         OnDisableAutoColor();
  void         OnColor();
  void         OnTexture();
  void         OnTransparency();
  void         OnNbIsos( ActionType actionType = SHOWDLG );
  void         OnDeflection();
  void         OnSelectOnly(int mode);
  void         OnDiscloseConcealChildren( bool );
  void         OnUnpublishObject();
  void         OnPublishObject() ;
  void         OnPointMarker();
  void         OnMaterialProperties();
  void         OnMaterialsLibrary();
  void         OnSetMaterial(const QVariant& );
  void         OnEdgeWidth();
  void         OnIsosWidth();
  void         OnBringToFront();
  void         OnClsBringToFront();
  void         OnCreateFolder();
  void         OnSortChildren();
#ifndef DISABLE_GRAPHICSVIEW
  void         OnShowDependencyTree();
#endif
  void         OnReduceStudy();

  // Shortcut commands
  void         OnChangeTransparency( bool );

  // Recursive deletion of object with children
  void         removeObjectWithChildren( _PTR(SObject),
                                         _PTR(Study),
                                         QList<SALOME_View*>,
                                         GEOM_Displayer* );
};

#endif // GEOMTOOLSGUI_H
