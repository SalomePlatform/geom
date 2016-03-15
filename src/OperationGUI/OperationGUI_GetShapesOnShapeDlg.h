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
// File   : OperationGUI_GetShapesOnShapeDlg.h
// Author : Sergey KUUL, Open CASCADE S.A.S. (sergey.kuul@opencascade.com)
//
#ifndef OPERATIONGUI_GETSHAPESONSHAPEDLG_H
#define OPERATIONGUI_GETSHAPESONSHAPEDLG_H

#include <GEOMBase_Skeleton.h>
#include <GEOMAlgo_State.hxx>

class DlgRef_2Sel2List;

//=================================================================================
// class    : OperationGUI_GetShapesOnShapeDlg
// purpose  :
//=================================================================================
class OperationGUI_GetShapesOnShapeDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  OperationGUI_GetShapesOnShapeDlg( GeometryGUI* theGeometryGUI, QWidget* parent = 0 );
  ~OperationGUI_GetShapesOnShapeDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent( QEvent* );

private:
  GEOM::GEOM_Object_var              myObject1;
  GEOM::GEOM_Object_var              myObject2;
  int                                myShapeType;
  GEOMAlgo_State                     myState;
  
  DlgRef_2Sel2List*                  GroupPoints;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               SetEditCurrentArgument();
  void                               SelectionIntoArgument();
  void                               LineEditReturnPressed();
  void                               ActivateThisDialog();
  void                               ComboTextChanged();
};

#endif // OPERATIONGUI_GETSHAPESONSHAPEDLG_H
