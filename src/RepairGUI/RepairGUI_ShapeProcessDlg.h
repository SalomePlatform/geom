// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License. 
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
// File   : RepairGUI_ShapeProcessDlg.h
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#ifndef REPAIRGUI_SHAPEPROCESSDLG_H
#define REPAIRGUI_SHAPEPROCESSDLG_H

#include <GEOMBase_Skeleton.h>

#include <QMap>

class DlgRef_1Sel;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QCheckBox;
class QListWidget;

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
  
  virtual void                       closeEvent( QCloseEvent* );        

private:
  void                               init();
  void                               reset();
  void                               loadDefaults(); // initialize all controls with default values (from resource file)
    
  GEOM::string_array*                getActiveOperators();
  GEOM::string_array*                getParameters( const GEOM::string_array& );
  GEOM::string_array*                getValues( const GEOM::string_array& );

  void                               enterEvent( QEvent* );

  QWidget*                           getControl( const QString&  );
  void                               setValue( QWidget*, const QString& ); // initialize the given control in the proper way
  // (analize its class and convert the value string)
  QString                            getValue( QWidget* ) const; // retrieve value of the control in the proper way

  //QDict<QString,QWidget*>            myCtrlMap;  // map of controls (values) of parameters
  void                               initParamsValues(); // initialize the data structures
  void                               initSelection();

private:
  QStringList                        myOpLst; // list of available Shape Healing Operators
  QMap<QString,QStringList>          myValMap; // map of parameters of operators

  GEOM::ListOfGO_var                 myObjects;  // selected objects
  
  DlgRef_1Sel*                       mySelectWdgt;
  QListWidget*                       myOpList;
  
  QDoubleSpinBox*                    myFixShapeTol3D;
  QDoubleSpinBox*                    myFixShapeMaxTol3D;

  QDoubleSpinBox*                    myFixFaceSizeTol;
  
  QDoubleSpinBox*                    myDropSmallEdgesTol3D;

  QDoubleSpinBox*                    mySplitAngleAngle;
  QDoubleSpinBox*                    mySplitAngleMaxTol;
  
  QSpinBox*                          mySplitClosedFacesNum;
  
  QDoubleSpinBox*                    mySplitContTol3D;
  QComboBox*                         mySplitContSurfCont;
  QComboBox*                         mySplitContCurvCont;
  
  QCheckBox*                         myBSplineSurfModeChk;
  QCheckBox*                         myBSpline3DCurveChk;
  QCheckBox*                         myBSpline2DCurveChk;
  QDoubleSpinBox*                    myBSplineTol3D;
  QDoubleSpinBox*                    myBSplineTol2D;
  QSpinBox*                          myBSplineDegree;
  QSpinBox*                          myBSplineSegments;
  QComboBox*                         myBSpline2DCont;
  QComboBox*                         myBSpline3DCont;
  
  QCheckBox*                         myToBezierSurfModeChk;
  QCheckBox*                         myToBezier3DCurveChk;
  QCheckBox*                         myToBezier2DCurveChk;
  QDoubleSpinBox*                    myToBezierMaxTol;
  
  QDoubleSpinBox*                    mySameParameterTol3D;
  
private slots:
  void                               onOk();
  bool                               onApply();

  void                               activate();
  
  void                               lineEditReturnPressed();
  void                               selectionChanged();
  void                               selectClicked();
  void                               advOptionToggled( bool );
};             

#endif // REPAIRGUI_SHAPEPROCESSDLG_H
