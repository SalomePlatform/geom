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
// File   : MeasureGUI_FastCheckIntersectionsDlg.h

#ifndef MEASUREGUI_FASTCHECKINTERDLG_H
#define MEASUREGUI_FASTCHECKINTERDLG_H

#include <GEOMBase_Skeleton.h>

class QListWidget;

//=================================================================================
// class    : MeasureGUI_FastCheckIntersectionsDlg
// purpose  :
//=================================================================================

class MeasureGUI_FastCheckIntersectionsDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:

  MeasureGUI_FastCheckIntersectionsDlg(GeometryGUI*, QWidget*);
  ~MeasureGUI_FastCheckIntersectionsDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid(QString &);
  virtual bool                        execute(ObjectList &);
  virtual bool                        extractPrefix() const;
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

private slots:

  void                                onSubShapesListSelectionChanged();
  void                                clear();
  void                                onCompute();
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                DeactivateActiveDialog();  
  void                                SelectionIntoArgument(); 
  void                                SetEditCurrentArgument();
  void                                OnGaps(bool);

private:

  void                                Init();
  void                                activateSelection();
  void                                enterEvent(QEvent *);
  bool                                findIntersections();
  float                               getDeflection();
  double                              getTolerance();
  void                                previewSubShapesListSelection(QListWidget*);
  GEOM::ListOfGO_var                  getLstObjFromListSelection(QListWidget*);

private:

  QPushButton                        *mySelButton1;
  QPushButton                        *mySelButton2;
  QLineEdit                          *myEditObjName1;
  QLineEdit                          *myEditObjName2;
  QLineEdit                          *myEditCurrentArgument;
  QCheckBox                          *myDetGaps;
  SalomeApp_DoubleSpinBox            *myTolerance;
  SalomeApp_DoubleSpinBox            *myDeflection;
  QPushButton                        *myComputeButton;
  QListWidget                        *myShapeList1;
  QListWidget                        *myShapeList2;
  GEOM::GeomObjPtr                    myObj1;
  GEOM::GeomObjPtr                    myObj2;
  GEOM::ListOfLong_var                myInters1;
  GEOM::ListOfLong_var                myInters2;
  GEOM::GEOM_IShapesOperations_var    myShapesOper;
};

#endif // MEASUREGUI_FASTCHECKINTERDLG_H
