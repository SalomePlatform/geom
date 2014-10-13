// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : MeasureGUI_CheckSelfIntersectionsDlg.h

#ifndef MEASUREGUI_CHECKSELFINTERDLG_H
#define MEASUREGUI_CHECKSELFINTERDLG_H

#include <GEOMBase_Skeleton.h>

class QComboBox;
class QListWidget;
class QTextBrowser;


//=================================================================================
// class    : MeasureGUI_CheckSelfIntersectionsDlg
// purpose  :
//=================================================================================

class MeasureGUI_CheckSelfIntersectionsDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:

  MeasureGUI_CheckSelfIntersectionsDlg(GeometryGUI*, QWidget*);
  ~MeasureGUI_CheckSelfIntersectionsDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid(QString &);
  virtual bool                        execute(ObjectList &);
  virtual bool                        extractPrefix() const;
  virtual GEOM::GEOM_Object_ptr       getFather (GEOM::GEOM_Object_ptr);

private slots:

  void                                onInteListSelectionChanged();
  void                                onSubShapesListSelectionChanged();
  void                                clear();
  void                                onCompute();
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                DeactivateActiveDialog();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();

private:

  void                                Init();
  void                                activateSelection();
  void                                enterEvent(QEvent *);
  bool                                findSelfIntersections
                                                    (bool    &HasSelfInte,
                                                     QString &theErrMsg);

private:

  QTextBrowser                       *myTextView;
  QPushButton                        *mySelButton;
  QLineEdit                          *myEditObjName;
  QComboBox                          *myLevelBox;
  QPushButton                        *myComputeButton;
  QListWidget                        *myInteList;
  QListWidget                        *myShapeList;
  GEOM::GEOM_Object_var               myObj;
  GEOM::ListOfLong_var                myInters;
  GEOM::GEOM_IShapesOperations_var    myShapesOper;

};

#endif // MEASUREGUI_CHECKSELFINTERDLG_H
