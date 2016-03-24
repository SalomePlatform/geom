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
// File   : EntityGUI_SubShapeDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#ifndef ENTITYGUI_SUBSHAPEDLG_H
#define ENTITYGUI_SUBSHAPEDLG_H

#include <GEOMBase_Skeleton.h>

#include <TColStd_IndexedMapOfInteger.hxx>

class QCheckBox;
class QComboBox;
class QGroupBox;
class QPushButton;
class SalomeApp_DoubleSpinBox;
class DlgRef_1Sel1List1Check3Btn;

//=================================================================================
// class    : EntityGUI_SubShapeDlg
// purpose  :
//=================================================================================
class EntityGUI_SubShapeDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  EntityGUI_SubShapeDlg (GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0);
  ~EntityGUI_SubShapeDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid (QString&);
  virtual bool                        execute (ObjectList&);
  virtual GEOM::GEOM_Object_ptr       getFather (GEOM::GEOM_Object_ptr);
  virtual QString                     getNewObjectName (int CurrObj = -1) const;
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

  void                                closeEvent (QCloseEvent*);

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                LineEditReturnPressed();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                SubShapeToggled();
  void                                ComboTextChanged();

  void                                showOnlySelected();
  void                                ClickOnOkFilter();
#ifndef DISABLE_PLOT2DVIEWER
  void                                ClickOnPlot();
#endif
  void                                MeasureToggled();

private:
  void                                Init();
  void                                enterEvent (QEvent*);

  void                                ResetStateOfDialog();

  void                                activateSelection();
  int                                 getSelectedSubshapes (TColStd_IndexedMapOfInteger& theMapIndex);
  void                                updateButtonState();
  bool                                isAllSubShapes() const;
  int                                 shapeType() const;

private:
  TopoDS_Shape                        myShape;
  GEOM::GEOM_Object_var               myObject;
  int                                 myDmMode;

  bool                                myIsHiddenMain;

  DlgRef_1Sel1List1Check3Btn*         GroupPoints;
  QCheckBox*                          myLessFilterCheck;
  QCheckBox*                          myGreaterFilterCheck;
  QComboBox*                          myLessFilterCombo;
  QComboBox*                          myGreaterFilterCombo;
  SalomeApp_DoubleSpinBox*            myLessFilterSpin;
  SalomeApp_DoubleSpinBox*            myGreaterFilterSpin;
  QPushButton*                        myApplyFilterButton;
  QPushButton*                        myPlotDistributionButton;
  QGroupBox*                          myFilterGrp;
};

#endif // ENTITYGUI_SUBSHAPEDLG_H
