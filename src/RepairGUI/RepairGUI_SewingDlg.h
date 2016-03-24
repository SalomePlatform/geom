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
// File   : RepairGUI_SewingDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef REPAIRGUI_SEWINGDLG_H
#define REPAIRGUI_SEWINGDLG_H

#include <GEOMBase_Skeleton.h>

class DlgRef_1SelExt;
class QCheckBox;
class SalomeApp_DoubleSpinBox;
class QPushButton;

//=================================================================================
// class    : RepairGUI_SewingDlg
// purpose  :
//=================================================================================
class RepairGUI_SewingDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  public:
  RepairGUI_SewingDlg( GeometryGUI*, QWidget* = 0, bool = false );
  ~RepairGUI_SewingDlg();

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
  QList<GEOM::GeomObjPtr>            myObjects;

  DlgRef_1SelExt*                    GroupPoints;
  QCheckBox*                         myAllowNonManifoldChk;
  SalomeApp_DoubleSpinBox*           myTolEdt;
  QPushButton*                       myFreeBoundBtn;

  int                                myClosed; // Number of free closed boundaries detected. Calculated in execute(), used in onDetect().
  int                                myOpen;   // Number of free open   boundaries detected. Calculated in execute(), used in onDetect().

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();

  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();

  void                               onDetect();
};

#endif // REPAIRGUI_SEWINGDLG_H
