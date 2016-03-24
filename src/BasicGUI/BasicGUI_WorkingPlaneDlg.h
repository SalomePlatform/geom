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

// GEOM GEOMGUI : GUI for Geometry component
// File   : BasicGUI_WorkingPlaneDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef BASICGUI_WORKINGPLANEDLG_H
#define BASICGUI_WORKINGPLANEDLG_H

#include <GEOMBase_Skeleton.h>

#include <gp_Ax3.hxx>

class DlgRef_1Sel;
class DlgRef_2Sel;
class DlgRef_3Check;
class QCheckBox;

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg
// purpose  :
//=================================================================================
class BasicGUI_WorkingPlaneDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  BasicGUI_WorkingPlaneDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~BasicGUI_WorkingPlaneDlg();

private:
  void                  Init();
  void                  enterEvent( QEvent* );
  
  bool                  updateWPlane( const bool = true );

  GEOM::GEOM_Object_var myFace;
  GEOM::GEOM_Object_var myVectX;
  GEOM::GEOM_Object_var myVectZ;
  
  int                   aOriginType;
  
  DlgRef_1Sel*          Group1;
  DlgRef_2Sel*          Group2;
  DlgRef_3Check*        Group3;
  
  QCheckBox*            myReverseCB;
  
  gp_Ax3                myWPlane;
  
private slots:
  void                  ClickOnOk();
  bool                  ClickOnApply();
  
  void                  ConstructorsClicked( int );
  void                  GroupClicked( int );
  void                  SetEditCurrentArgument();
  void                  SelectionIntoArgument();
  void                  LineEditReturnPressed();
  void                  onReverse();
  
  void                  ActivateThisDialog();
  void                  DeactivateActiveDialog();
};

#endif // BASICGUI_WORKINGPLANEDLG_H
