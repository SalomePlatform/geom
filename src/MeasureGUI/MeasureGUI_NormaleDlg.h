// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
// File   : MeasureGUI_NormaleDlg.h
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_NORMALEDLG_H
#define MEASUREGUI_NORMALEDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_2Sel;

//=================================================================================
// class    : MeasureGUI_NormaleDlg
// purpose  :
//=================================================================================
class MeasureGUI_NormaleDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_NormaleDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_NormaleDlg();

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
  GEOM::GEOM_Object_var              myFace;
  GEOM::GEOM_Object_var              myPoint;

  DlgRef_2Sel*                       GroupArgs;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();
  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
};

#endif // MEASUREGUI_NORMALEDLG_H
