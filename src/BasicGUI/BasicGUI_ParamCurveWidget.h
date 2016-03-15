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
// File   : BasicGUI_ParamCurveWidget.h
// Author : Roman NIKOLAEV (roman.nikolaev@opencascade.com)

#ifndef BASICGUI_PARAM_CURVE_WIDGET_H
#define BASICGUI_PARAM_CURVE_WIDGET_H
#include <QWidget>

class QLineEdit;
class SalomeApp_DoubleSpinBox;
class SalomeApp_IntSpinBox;

class BasicGUI_ParamCurveWidget: public QWidget {
  Q_OBJECT

 public:  
  BasicGUI_ParamCurveWidget(QWidget * parent = 0);
  ~BasicGUI_ParamCurveWidget();
  
  public:
  QLineEdit* myXExpr;
  QLineEdit* myYExpr;
  QLineEdit* myZExpr;

  SalomeApp_DoubleSpinBox*           myPMin;
  SalomeApp_DoubleSpinBox*           myPMax;
  SalomeApp_IntSpinBox*              myPStep;
  
};

#endif //BASICGUI_PARAM_CURVE_WIDGET_H
