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
// File   : OperationGUI_Fillet2dDlg.h
// Author : DMV, OCN
//

#ifndef OPERATIONGUI_FILLET2DDLG_H
#define OPERATIONGUI_FILLET2DDLG_H

#include <GEOMBase_Skeleton.h>

#include <TColStd_IndexedMapOfInteger.hxx>

class DlgRef_2Sel1Spin;

//=================================================================================
// class    : OperationGUI_Fillet2dDlg
// purpose  :
//=================================================================================
class OperationGUI_Fillet2dDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT
      
public:
  OperationGUI_Fillet2dDlg( GeometryGUI*, QWidget* );
  ~OperationGUI_Fillet2dDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual                             GEOM::GEOM_IOperations_ptr createOperation();
  virtual                             bool isValid( QString& );
  virtual                             bool execute( ObjectList& );

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                LineEditReturnPressed();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                ValueChangedInSpinBox( double );
  
private:
  void                                Init();
  void                                enterEvent( QEvent* );
  void                                activateSelection();
  double                              getRadius() const;

private:
  GEOM::GEOM_Object_var               myShape;
  TColStd_IndexedMapOfInteger         myVertexes;

  DlgRef_2Sel1Spin*                   GroupVertexes;
};

#endif // OPERATIONGUI_FILLET2DDLG_H
