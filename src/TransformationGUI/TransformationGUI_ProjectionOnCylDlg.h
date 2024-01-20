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

// GEOM GEOMGUI : GUI for Geometry component
// File   : TransformationGUI_ProjectionOnCylDlg.h
// Author : Sergey KHROMOV, Open CASCADE S.A.S.
//
#ifndef TRANSFORMATIONGUI_PROJECTIONONCYLDLG_H
#define TRANSFORMATIONGUI_PROJECTIONONCYLDLG_H


#include "GEOMBase_Skeleton.h"


//=================================================================================
// class    : TransformationGUI_ProjectionOnCylDlg
// purpose  :
//=================================================================================
class TransformationGUI_ProjectionOnCylDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  TransformationGUI_ProjectionOnCylDlg(GeometryGUI*, QWidget* = 0,
                                       bool = false, Qt::WindowFlags = 0);
  ~TransformationGUI_ProjectionOnCylDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid(QString &msg);
  virtual bool                       execute(ObjectList &);

private:
  void                               Init();
  void                               enterEvent( QEvent* );
  void                               activateSelection();
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               SetUseLengthAngle();

private:

  GEOM::GEOM_Object_var              myObj;
  QLineEdit                         *myObjectName;
  QPushButton                       *mySelButton;
  SalomeApp_DoubleSpinBox           *myRadiusSpin;
  SalomeApp_DoubleSpinBox           *myStartAngleSpin;
  QCheckBox                         *myUseAngleLen;
  SalomeApp_DoubleSpinBox           *myAngleLenSpin;
  SalomeApp_DoubleSpinBox           *myAngleRotSpin;

};

#endif // TRANSFORMATIONGUI_PROJECTIONONCYLDLG_H
