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
// File   : MeasureGUI_ManageDimensionsDlg.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_MANAGEDIMENSIONSDLG_H
#define MEASUREGUI_MANAGEDIMENSIONSDLG_H

#include <GEOMBase_Skeleton.h>
#include "MeasureGUI_DimensionInteractor.h"
#include "MeasureGUI_Widgets.h"
#include <GEOMGUI_DimensionProperty.h>

class DlgRef_1Sel;
class QTreeWidget;
class QPushButton;
class SOCC_Viewer;
class MeasureGUI_CreateDimensionDlg;

//=================================================================================
// class    : MeasureGUI_ManageDimensionsDlg
// purpose  : The dialog provides interactive management of measurement
//            dimension presentations and the "dimension" property state of
//            GEOM shape objects.
//=================================================================================
class MeasureGUI_ManageDimensionsDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_ManageDimensionsDlg( GeometryGUI*, QWidget* );
  ~MeasureGUI_ManageDimensionsDlg();

protected:
  enum Selection
  {
    Selection_None,
    Selection_Object,
    Selection_Dimension
  };

/* User event logics */
protected slots:
  void                            StartObjectSelection() { StartSelection( Selection_Object ); }
  void                            StartSelection( const Selection theSelection );
  void                            StopSelection();
  void                            OnSelection();
  void                            SelectionIntoArgument( const Selection theSelection );
  void                            OnAdd();
  void                            OnRemove();
  void                            OnChangeItem( QTreeWidgetItem* theItem );
  void                            OnSelectItem();
  void                            OnShowAll();
  void                            OnHideAll();
  void                            OnInteractionFinished( Handle_AIS_InteractiveObject theIO );
  void                            Suspend();
  void                            Resume();
  void                            ClickOnOk();
  virtual void                    ClickOnCancel();
  bool                            ClickOnApply();
  void                            OnFinish();
  void                            OnActivateThisDialog();
  void                            OnDeactivateThisDialog();
  void                            OnDimensionAdded();


/* Utils */
private:
  void                            SetEditObject( const GEOM::GeomObjPtr& );
  void                            RestoreState();
  void                            PopulateList();
  bool                            HasUnsavedChanges();
  bool                            AllowedToCancelChanges();
  bool                            AllowedToSaveChanges();
  bool                            WarnUnsaved();
  int                             IdFromItem( QTreeWidgetItem* theItem );
  int                             IdFromPrs( const Handle(AIS_InteractiveObject)& theAIS );
  void                            SelectInList( const Handle(AIS_InteractiveObject)& theIO );
  void                            SelectInList( const int theId );
  void                            SelectInViewer( SOCC_Viewer* theViewer, const int theId );
  void                            RedisplayObject();

private:
  void                            enterEvent(QEvent*);

private:
  enum GroupItems
  {
    GroupItem_Distance = 0,
    GroupItem_Diameter = 1,
    GroupItem_Angle    = 2
  };

private:
  GEOM::GeomObjPtr                myEditObject;
  bool                            myIsNeedRedisplay;
private:
  DlgRef_1Sel*                    myObjectSelector;
  MeasureGUI_1TreeWidget_4Button* myDimensionView;
  MeasureGUI_DimensionInteractor* myDimensionInteractor;
  GEOMGUI_DimensionProperty       mySavedPropertyState;
  SOCC_Viewer*                    myOperatedViewer;
  Selection                       myCurrentSelection;
  MeasureGUI_CreateDimensionDlg*  myCreateDialog;
};

#endif
