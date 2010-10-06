//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : BuildGUI_EdgeDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef BUILDGUI_EDGEDLG_H
#define BUILDGUI_EDGEDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1Sel2Spin;
class DlgRef_2Sel;

//=================================================================================
// class    : BuildGUI_EdgeDlg
// purpose  :
//=================================================================================
class BuildGUI_EdgeDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  BuildGUI_EdgeDlg( GeometryGUI*, QWidget* = 0 );
  ~BuildGUI_EdgeDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );    
  virtual void                       addSubshapesToStudy();

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  
private:
  GEOM::GEOM_Object_var              myPoint1, myPoint2;   /* Points containing the edge */
  GEOM::GEOM_Object_var              myWire;               /* Wire */
  
  DlgRef_2Sel*                       GroupPoints;
  DlgRef_1Sel2Spin*                  GroupWire;

private slots:
  void                               ConstructorsClicked( int );
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
};

#endif // BUILDGUI_EDGEDLG_H
