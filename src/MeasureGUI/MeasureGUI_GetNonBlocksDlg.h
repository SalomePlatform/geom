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
// File   : MeasureGUI_GetNonBlocksDlg.h

#ifndef MEASUREGUI_GETNONBLOCKSDLG_H
#define MEASUREGUI_GETNONBLOCKSDLG_H

#include <GEOMBase_Skeleton.h>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class SalomeApp_DoubleSpinBox;

//=================================================================================
// class    : MeasureGUI_GetNonBlocksDlg
// purpose  :
//=================================================================================
class MeasureGUI_GetNonBlocksDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  MeasureGUI_GetNonBlocksDlg (GeometryGUI*, QWidget*);
  ~MeasureGUI_GetNonBlocksDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr  createOperation();
  virtual bool                        isValid (QString &msg);
  virtual bool                        execute (ObjectList&);
  virtual GEOM::GEOM_Object_ptr       getFather (GEOM::GEOM_Object_ptr);
  virtual QList<GEOM::GeomObjPtr>     getSourceObjects();

private slots:
  void                                ClickOnOk();
  bool                                ClickOnApply();
  void                                ActivateThisDialog();
  void                                LineEditReturnPressed();
  void                                SelectionIntoArgument();
  void                                SetEditCurrentArgument();
  void                                SetUseC1Tolerance();
  
private:
  void                                Init();
  void                                enterEvent (QEvent*);
  void                                processObject();

private:
  GEOM::GEOM_Object_var               myObj;
  QLineEdit                          *myObjectName;
  QPushButton                        *mySelButton;
  QCheckBox                          *myUseC1Check;
  QLabel                             *myTolLbl;
  SalomeApp_DoubleSpinBox            *mySpinTol;
};

#endif // MEASUREGUI_GETNONBLOCKSDLG_H
