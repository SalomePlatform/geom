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

#ifndef REPAIRGUI_FuseEdgesDLG_H
#define REPAIRGUI_FuseEdgesDLG_H

#include <GEOMBase_Skeleton.h>

#include <TColStd_IndexedMapOfInteger.hxx>

class DlgRef_2Sel1Spin;

//=================================================================================
// class    : RepairGUI_FuseEdgesDlg
// purpose  :
//=================================================================================
class RepairGUI_FuseEdgesDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  RepairGUI_FuseEdgesDlg (GeometryGUI*, QWidget*);
  ~RepairGUI_FuseEdgesDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();

private:
  void                               Init();
  void                               enterEvent (QEvent*);
  void                               activateSelection();

private:
  GEOM::GEOM_Object_var              myShape;
  QList<GEOM::GeomObjPtr>            myPoints;

  DlgRef_2Sel1Spin*                  GroupVertexes;
};

#endif // REPAIRGUI_FuseEdgesDLG_H
