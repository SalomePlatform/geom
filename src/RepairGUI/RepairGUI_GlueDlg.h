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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : RepairGUI_GlueDlg.h
//  Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#ifndef REPAIRGUI_GLUEDLG_H
#define REPAIRGUI_GLUEDLG_H

#include <GEOMBase_Skeleton.h>

#include <TopAbs.hxx>

#include <QList>

class DlgRef_1SelExt;
class SalomeApp_DoubleSpinBox;
class QPushButton;
class QCheckBox;

//=================================================================================
// class    : RepairGUI_GlueDlg
// purpose  :
//=================================================================================
class RepairGUI_GlueDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  RepairGUI_GlueDlg (GeometryGUI*, QWidget* = 0, bool = false, TopAbs_ShapeEnum theGlueMode = TopAbs_FACE);
  ~RepairGUI_GlueDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       restoreSubShapes (SALOMEDS::SObject_ptr);
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent (QEvent*);
  void                               initSelection();

  void                               clearTemporary();

  // Reimplementation of onAccept for local case of this class.
  bool                               onAcceptLocal();
  void                               clearShapeBufferLocal (GEOM::GEOM_Object_ptr);

  void                               activateSelection();
  void                               updateButtonState();
  void                               selectTmpInViewer();

private:
  QList<GEOM::GeomObjPtr>            myObjects;
  QList<GEOM::GeomObjPtr>            myTmpObjs;

  DlgRef_1SelExt*                    GroupPoints;
  DlgRef_1SelExt*                    GroupPoints2;
  SalomeApp_DoubleSpinBox*           myTolEdt;
  SalomeApp_DoubleSpinBox*           myTolEdt2;
  QPushButton*                       myDetectBtn;
  QCheckBox*                         mySubShapesChk;
  QCheckBox*                         myGlueAllEdgesChk;

  int                                myCurrConstrId;

  TopAbs_ShapeEnum                   myGlueMode;

protected slots:
  virtual void                       ClickOnCancel();

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();

  void                               LineEditReturnPressed();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();

  void                               ConstructorsClicked (int);

  void                               onDetect();
  void                               onTolerChanged (double);
  void                               onSubShapesChk();
};

#endif // REPAIRGUI_GLUEDLG_H
