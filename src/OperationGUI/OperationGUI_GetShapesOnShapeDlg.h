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
//  File   : OperationGUI_GetShapesOnShapeDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_GETSHAPESONSHAPE_H
#define DIALOGBOX_GETSHAPESONSHAPE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2List_QTD.h"
#include "GEOMAlgo_State.hxx"


//=================================================================================
// class    : OperationGUI_GetShapesOnShapeDlg
// purpose  :
//=================================================================================
class OperationGUI_GetShapesOnShapeDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  OperationGUI_GetShapesOnShapeDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0);
  ~OperationGUI_GetShapesOnShapeDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool isValid( QString& );
  virtual bool execute( ObjectList& objects );

  virtual void closeEvent( QCloseEvent* e );

private:
  void Init();
  void enterEvent(QEvent * e);

  GEOM::GEOM_Object_var myObject1;
  GEOM::GEOM_Object_var myObject2;
  int myShapeType;
  GEOMAlgo_State myState;
  
  DlgRef_2Sel2List_QTD* GroupPoints;

private slots:
  void ClickOnOk();
  bool ClickOnApply();
  void SetEditCurrentArgument();
  void SelectionIntoArgument();
  void LineEditReturnPressed();
  void ActivateThisDialog();
  void ComboTextChanged();
};

#endif // DIALOGBOX_GETSHAPESONSHAPE_H
