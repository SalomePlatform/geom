// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : OperationGUI_Fillet1d2dDlg.h
// Author : DMV, OCN

#ifndef OPERATIONGUI_Fillet1d2dDLG_H
#define OPERATIONGUI_Fillet1d2dDLG_H

#include <GEOMBase_Skeleton.h>

#include <TColStd_IndexedMapOfInteger.hxx>

class DlgRef_2Sel1Spin2Check;

//=================================================================================
// class    : OperationGUI_Fillet1d2dDlg
// purpose  :
//=================================================================================
class OperationGUI_Fillet1d2dDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT
      
public:
  OperationGUI_Fillet1d2dDlg( GeometryGUI*, QWidget*, bool theIs1D );
  ~OperationGUI_Fillet1d2dDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual                             GEOM::GEOM_IOperations_ptr createOperation();
  virtual                             bool isValid( QString& );
  virtual                             bool execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

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
  bool                                myIs1D;
  GEOM::GEOM_Object_var               myShape;
  TColStd_IndexedMapOfInteger         myVertexes;

  DlgRef_2Sel1Spin2Check*             GroupVertexes;
};

#endif // OPERATIONGUI_Fillet1d2dDLG_H
