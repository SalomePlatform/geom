// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_RemoveIntWiresDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef REPAIRGUI_REMOVEINTWIRESDLG_H
#define REPAIRGUI_REMOVEINTWIRESDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1Sel1Check1Sel;

//=================================================================================
// class    : RepairGUI_RemoveIntWiresDlg
// purpose  :
//=================================================================================
class RepairGUI_RemoveIntWiresDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  RepairGUI_RemoveIntWiresDlg( GeometryGUI*, QWidget* = 0, bool = false );
  ~RepairGUI_RemoveIntWiresDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();
  
private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               initSelection();
  
private:
  GEOM::GEOM_Object_var              myObject;
  GEOM::short_array_var              myWiresInd;
  
  DlgRef_1Sel1Check1Sel*             GroupPoints;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  
  void                               ActivateThisDialog();
  
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  
  void                               onRemoveAllClicked();
};

#endif // REPAIRGUI_REMOVEINTWIRESDLG_H
