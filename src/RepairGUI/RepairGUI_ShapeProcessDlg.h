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
// File   : RepairGUI_ShapeProcessDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#ifndef REPAIRGUI_SHAPEPROCESSDLG_H
#define REPAIRGUI_SHAPEPROCESSDLG_H

#include <GEOMBase_Skeleton.h>

#include <QMap>

class DlgRef_1Sel;
class SalomeApp_IntSpinBox;
class SalomeApp_DoubleSpinBox;
class QCheckBox;
class QComboBox;
class QListWidget;
class QListWidgetItem;
class QStackedLayout;

//=================================================================================
// class    : RepairGUI_ShapeProcessDlg
// purpose  :
//=================================================================================
class RepairGUI_ShapeProcessDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  RepairGUI_ShapeProcessDlg( GeometryGUI*, QWidget* = 0, bool = false );
  ~RepairGUI_ShapeProcessDlg();
  
protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList&  );
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();
  
private:
  void                               init();
  void                               reset();
  void                               loadDefaults(); // initialize all controls with default values (from resource file)
    
  GEOM::string_array*                getActiveOperators();
  GEOM::string_array*                getParameters( const GEOM::string_array& );
  GEOM::string_array*                getValues( const GEOM::string_array& );

  QStringList                        getTexts( const GEOM::string_array& );

  void                               enterEvent( QEvent* );

  QWidget*                           getControl( const QString&  );
  void                               setValue( QWidget*, const QString& ); // initialize the given control in the proper way
  // (analize its class and convert the value string)
  QString                            getValue( QWidget* ) const; // retrieve value of the control in the proper way
  QString                            getText( QWidget* ) const; // retrieve text of the control (for spin-boxes only)

  //QDict<QString,QWidget*>            myCtrlMap;  // map of controls (values) of parameters
  void                               initParamsValues(); // initialize the data structures
  void                               initSelection();
  void                               updateSelectAll();

private:
  QStringList                        myOpLst; // list of available Shape Healing Operators
  QMap<QString,QStringList>          myValMap; // map of parameters of operators

  GEOM::ListOfGO_var                 myObjects;  // selected objects
  
  DlgRef_1Sel*                       mySelectWdgt;
  QCheckBox*                         mySelectAll;
  QListWidget*                       myOpList;
  QStackedLayout*                    myStack;
  
  SalomeApp_DoubleSpinBox*           myFixShapeTol3D;
  SalomeApp_DoubleSpinBox*           myFixShapeMaxTol3D;

  SalomeApp_DoubleSpinBox*           myFixFaceSizeTol;
  
  SalomeApp_DoubleSpinBox*           myDropSmallEdgesTol3D;

  SalomeApp_DoubleSpinBox*           mySplitAngleAngle;
  SalomeApp_DoubleSpinBox*           mySplitAngleMaxTol;
  
  SalomeApp_IntSpinBox*              mySplitClosedFacesNum;
  
  SalomeApp_DoubleSpinBox*           mySplitContTol3D;
  QComboBox*                         mySplitContSurfCont;
  QComboBox*                         mySplitContCurvCont;
  
  QCheckBox*                         myBSplineSurfModeChk;
  QCheckBox*                         myBSpline3DCurveChk;
  QCheckBox*                         myBSpline2DCurveChk;
  SalomeApp_DoubleSpinBox*           myBSplineTol3D;
  SalomeApp_DoubleSpinBox*           myBSplineTol2D;
  SalomeApp_IntSpinBox*              myBSplineDegree;
  SalomeApp_IntSpinBox*              myBSplineSegments;
  QComboBox*                         myBSpline2DCont;
  QComboBox*                         myBSpline3DCont;
  
  QCheckBox*                         myToBezierSurfModeChk;
  QCheckBox*                         myToBezier3DCurveChk;
  QCheckBox*                         myToBezier2DCurveChk;
  SalomeApp_DoubleSpinBox*           myToBezierMaxTol;
  
  SalomeApp_DoubleSpinBox*           mySameParameterTol3D;

  QCheckBox*                         myDropSmallSolidsWidChk;
  QCheckBox*                         myDropSmallSolidsVolChk;
  SalomeApp_DoubleSpinBox*           myDropSmallSolidsWidTol;
  SalomeApp_DoubleSpinBox*           myDropSmallSolidsVolTol;
  QCheckBox*                         myDropSmallSolidsMergeChk;

private slots:
  void                               onOk();
  bool                               onApply();

  void                               activate();
  
  void                               lineEditReturnPressed();
  void                               selectionChanged();
  void                               selectClicked();
  void                               advOptionToggled( bool );
  void                               operatorChecked( QListWidgetItem * item );
  void                               onSelectAll( int );
};

#endif // REPAIRGUI_SHAPEPROCESSDLG_H
