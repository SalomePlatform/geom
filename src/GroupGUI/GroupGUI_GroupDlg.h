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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GroupGUI_GroupDlg.h
//  Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#ifndef GROUPGUI_GROUPDLG_H
#define GROUPGUI_GROUPDLG_H

#include <GEOMBase_Skeleton.h>

#include <TopAbs_ShapeEnum.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

class QCheckBox;
class QComboBox;
class QPushButton;
class QGroupBox;
class QLineEdit;
class QListWidget;
class QButtonGroup;
class SalomeApp_DoubleSpinBox;

//=================================================================================
// class    : GroupGUI_GroupDlg
// purpose  :
//=================================================================================
class GroupGUI_GroupDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  typedef enum {
    CreateGroup,
    EditGroup
  } Mode;

  GroupGUI_GroupDlg (Mode mode, GeometryGUI*, QWidget* parent = 0);
  ~GroupGUI_GroupDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid (QString&);
  virtual bool                        execute (ObjectList&);
  virtual GEOM::GEOM_Object_ptr       getFather (GEOM::GEOM_Object_ptr);
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

  void                                closeEvent (QCloseEvent*);

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                ConstructorsClicked(int);

  void                                selectAllSubShapes();
  void                                add();
  void                                remove();
  void                                showOnlySelected();
  void                                selectionChanged();
  void                                ClickOnOkFilter();
#ifndef DISABLE_PLOT2DVIEWER
  void                                ClickOnPlot();
#endif
  void                                MeasureToggled();

private:
  void                                Init();
  void                                enterEvent (QEvent*);

  int                                 subSelectionWay() const;
  TopAbs_ShapeEnum                    getShapeType() const;
  void                                setShapeType (const TopAbs_ShapeEnum);
  void                                activateSelection();
  void                                updateState (bool isAdd = false);
  void                                highlightSubShapes();
  void                                onGetInPlace();
  void                                setInPlaceObj (GEOM::GEOM_Object_var, const bool isVisible=1);
  int                                 getSelectedSubshapes (TColStd_IndexedMapOfInteger& theMapIndex);

private:
  Mode                                myMode;
  bool                                myBusy;
  bool                                myIsShapeType;
  bool                                myIsHiddenMain;
  bool                                myWasHiddenMain;
  GEOM::GEOM_Object_var               myMainObj;
  GEOM::GEOM_Object_var               myGroup;
  GEOM::GEOM_Object_var               myInPlaceObj;
  int                                 myInPlaceObjSelectState;
  TColStd_DataMapOfIntegerInteger     myMain2InPlaceIndices;
  QList<int>                          myGroupIdList;
  int                                 myDmMode;
  bool                                myIsAccept;

  QPushButton*                        mySelBtn;
  QLineEdit*                          myMainName;
  QPushButton*                        mySelBtn2;
  QLineEdit*                          myShape2Name;
  QGroupBox*                          myRestrictGroupBox;
  QButtonGroup*                       myRestrictGroup;
  QPushButton*                        mySelAllBtn;
  QPushButton*                        myAddBtn;
  QPushButton*                        myRemBtn;
  QPushButton*                        myShowOnlyBtn;
  QPushButton*                        myHideSelBtn;
  QPushButton*                        myShowAllBtn;
  QListWidget*                        myIdList;
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

#endif
