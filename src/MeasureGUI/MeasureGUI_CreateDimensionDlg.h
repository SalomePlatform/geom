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
// File   : MeasureGUI_CreateDimensionDlg.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_CREATEDIMENSIONDLG_H
#define MEASUREGUI_CREATEDIMENSIONDLG_H

#include "MeasureGUI_Widgets.h"
#include "MeasureGUI_DimensionInteractor.h"

// GEOM includes
#include <GEOMBase_Skeleton.h>
#include <GEOM_GenericObjPtr.h>

// SUIT includes
#include <SOCC_ViewModel.h>

// OCCT includes
#include <AIS_Dimension.hxx>
#include <gp_Pnt.hxx>

class MeasureGUI_1Sel_Frame;
class MeasureGUI_2Sel_Frame;
class MeasureGUI_3Sel_Frame;
class QAbstractButton;
class QTabWidget;
class Bnd_Box;
class SOCC_Prs;

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg
// purpose  : Dialog invoked from MeasureGUI_ManageDimensionDlg for
//            creating dimension presentations for the passed onwer object.
//            Does not use operation logics.
//=================================================================================
class MeasureGUI_CreateDimensionDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

  class BaseSelectorPane;
  class LengthPane;
  class DiameterPane;
  class AnglePane;

public:
  // Enumerate dimension type radio buttons
  enum TypeButtonID
  {
    TypeButtonID_Length   = 0,
    TypeButtonID_Diameter = 1,
    TypeButtonID_Angle    = 2
  };

public:
  MeasureGUI_CreateDimensionDlg( const GEOM::GeomObjPtr&, GeometryGUI*, QWidget* );
  ~MeasureGUI_CreateDimensionDlg();

public:
  Handle(AIS_Dimension)               GetCreatedDimension() { return myDimension; }

public:
  void                                Init();


signals:
  void                                applyClicked();

/* User event logics */
protected slots:
  void                                ConstructTypeChanged( int theType );
  void                                OnArgumentTabChanged();
  void                                OnStartSelection( const QList<TopAbs_ShapeEnum>& theModes );
  void                                OnStopSelection();
  void                                SelectionIntoArgument();
  void                                OnSelectionDone();
  void                                ClickOnOk();
  bool                                ClickOnApply();

/* Commands */
protected:
  void                                StartLocalEditing();
  void                                StopLocalEditing();

/* Construction of preview and interaction */
protected:
  Handle(AIS_Dimension)               CreateDimension();
  GEOM::GeomObjPtr                    GetTopLevel( const GEOM::GeomObjPtr& theObject );
  bool                                AddDimensionToOwner();

private:
  BaseSelectorPane*                   ActiveArgs();

private:
  QString                             GenerateName( const QString& thePrefix );

private:
  GEOM::GeomObjPtr                    myParentObj;
  LengthPane*                         myLengthArgs;
  DiameterPane*                       myDiameterArgs;
  AnglePane*                          myAngleArgs;
  QList<TopAbs_ShapeEnum>             mySelectionModes;
  MeasureGUI_DimensionInteractor*     myDimensionInteractor;
  Handle(AIS_Dimension)               myDimension;
  SOCC_Viewer*                        myEditingViewer;
  int                                 myEditingLayer;
};

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::BaseSelectorPane
// purpose  : Base selector pane handles events of switching the selector controls
//            and changing argument tabs
//=================================================================================
class MeasureGUI_CreateDimensionDlg::BaseSelectorPane : public QWidget
{
  Q_OBJECT

public:
  typedef QList<TopAbs_ShapeEnum> SelectionModes;

public:
  BaseSelectorPane( QWidget* theParent );

signals:
  void               StartSelection( const QList<TopAbs_ShapeEnum>& theModes );
  void               StopSelection();
  void               SelectionDone();
  void               TabChanged();

public:
  void               Reset( bool theOpenDefaultTab = false );
  int                ActiveTab() const;
  void               SelectionIntoArguments( const GEOM::GeomObjPtr& theSelected );
  GEOM::GeomObjPtr   GetSelection( QLineEdit* theSelector ) const;

public slots:
  void               OnSelectorClicked();
  void               OnTabChanged();

protected:
  void               SetTabWidget( QTabWidget* theTabs );

  /* Register geometry selector controls at base level to provide
     automatic handling of button clicks, selector switching and
     sending start selection events to dialog.
     - theSelectorEdit  : line edit control with selected geometry name.
     - theSelectorButton: selector activation button.
     - theSelectorModes : shape selection modes allowed for the selection.
     - theTab : the tab index on which the controls are located. */
  void               RegisterSelector( QLineEdit* theSelectorEdit,
                                       QPushButton* theSelectorButton,
                                       const SelectionModes& theSelectorModes,
                                       const int theTab = 0 );

protected:
  QTabWidget*                         myTabs;
  QMap< int, QList<QLineEdit*> >      mySelectors;
  QMap<QLineEdit*, QPushButton*>      mySelectionButtons;
  QMap<QLineEdit*, SelectionModes>    mySelectionModes;
  QMap<QLineEdit*, GEOM::GeomObjPtr>  mySelectedShapes;
  QPixmap                             mySelectorIcon;
  QLineEdit*                          myCurrentSelector;
};

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::LengthPane
// purpose  : Layout for length arguments pane
//=================================================================================
class MeasureGUI_CreateDimensionDlg::LengthPane : public BaseSelectorPane
{
  Q_OBJECT

public:
  enum TabID
  {
    TabID_SingleEdge,
    TabID_TwoPoints,
    TabID_ParallelEdges
  };

public:
  LengthPane( QWidget* theParent );
  GEOM::GeomObjPtr  GetSingleEdge() const { return GetSelection( mySingleEdgeSelectors->LineEdit1 ); }
  GEOM::GeomObjPtr  GetPoint1() const     { return GetSelection( myTwoPointsSelectors->LineEdit1 ); }
  GEOM::GeomObjPtr  GetPoint2() const     { return GetSelection( myTwoPointsSelectors->LineEdit2 ); }
  GEOM::GeomObjPtr  GetEdge1() const      { return GetSelection( myParallelEdgesSelectors->LineEdit1 ); }
  GEOM::GeomObjPtr  GetEdge2() const      { return GetSelection( myParallelEdgesSelectors->LineEdit2 ); }

private:
  QTabWidget* myTabs;
  MeasureGUI_1Sel_Frame*   mySingleEdgeSelectors;
  MeasureGUI_2Sel_Frame*   myTwoPointsSelectors;
  MeasureGUI_2Sel_Frame*   myParallelEdgesSelectors;
};

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::DiameterPane
// purpose  : Layout for diameter arguments pane
//=================================================================================
class MeasureGUI_CreateDimensionDlg::DiameterPane : public BaseSelectorPane
{
  Q_OBJECT

public:
  DiameterPane( QWidget* theParent );
  GEOM::GeomObjPtr  GetShape() const { return GetSelection( myShapeSelector->LineEdit1 ); }

private:
  MeasureGUI_1Sel_Frame*   myShapeSelector;
};

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::AnglePane
// purpose  : Layout for angle arguments pane
//=================================================================================
class MeasureGUI_CreateDimensionDlg::AnglePane : public BaseSelectorPane
{
  Q_OBJECT

public:
  enum TabID
  {
    TabID_TwoEdges,
    TabID_ThreePoints
  };

public:
  AnglePane( QWidget* theParent );
  GEOM::GeomObjPtr  GetEdge1() const  { return GetSelection( myTwoEdgesSelectors->LineEdit1 ); }
  GEOM::GeomObjPtr  GetEdge2() const  { return GetSelection( myTwoEdgesSelectors->LineEdit2 ); }
  GEOM::GeomObjPtr  GetPoint1() const { return GetSelection( myThreePointsSelectors->LineEdit1 ); }
  GEOM::GeomObjPtr  GetPoint2() const { return GetSelection( myThreePointsSelectors->LineEdit2 ); }
  GEOM::GeomObjPtr  GetPoint3() const { return GetSelection( myThreePointsSelectors->LineEdit3 ); }

private:
  QTabWidget* myTabs;
  MeasureGUI_2Sel_Frame*   myTwoEdgesSelectors;
  MeasureGUI_3Sel_Frame*   myThreePointsSelectors;
};

#endif
