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
// File   : BasicGUI_CurveDlg.h
// Author : Alexander SLADKOV, Open CASCADE S.A.S. (alexander.sladkov@opencascade.com)

#ifndef BASICGUI_CURVEDLG_H
#define BASICGUI_CURVEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

#include <list>

class DlgRef_1Sel3Check;
class QRadioButton;
class QPushButton;
class QLineEdit;
class QGroupBox;
class BasicGUI_ParamCurveWidget;

//=================================================================================
// class    : BasicGUI_CurveDlg
// purpose  :
//=================================================================================
class BasicGUI_CurveDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  BasicGUI_CurveDlg (GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0);
  ~BasicGUI_CurveDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid (QString&);
  virtual bool                       execute (ObjectList&);
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent (QEvent*);

private:
  QRadioButton*                      myAnaliticalBtn;
  QRadioButton*                      myBySelectionBtn;
  DlgRef_1Sel3Check*                 myGroupPoints;
  BasicGUI_ParamCurveWidget*         myGroupParams;
  QGroupBox*                         myGroupTangents;

  QPushButton*                       myPushBtnV1;
  QPushButton*                       myPushBtnV2;
  QLineEdit*                         myLineEditV1;
  QLineEdit*                         myLineEditV2;

  QList<GEOM::GeomObjPtr>            myPoints;
  GEOM::GeomObjPtr                   myVec1, myVec2;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();

  void                               ConstructorsClicked (int);
  void                               CheckButtonToggled();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               CreationModeChanged();
  void                               ValueChangedInSpinBox (double/*theValue*/);
  void                               ValueChangedInSpinBox (int   /*theValue*/);
  void                               OnEditingFinished();
};

#endif // BASICGUI_CURVEDLG_H
