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

#include "BasicGUI_ParamCurveWidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QGroupBox>

#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_IntSpinBox.h>


BasicGUI_ParamCurveWidget::BasicGUI_ParamCurveWidget(QWidget* parent):
  QWidget(parent) {

  QGridLayout* l = new QGridLayout( this );
  l->setSpacing( 0 );
  l->setMargin( 0 );

  QGroupBox* groupBox = new QGroupBox( tr("GEOM_PCURVE_TITLE"), this );

  QGridLayout* gridLayout = new QGridLayout( groupBox );
  gridLayout->setSpacing( 6 );
  gridLayout->setMargin( 11 );
  
  //X Equation
  QLabel* textLabel1 = new QLabel( tr("GEOM_PCURVE_X"), groupBox );
  myXExpr = new QLineEdit( groupBox );

  //Y Equation
  QLabel* textLabel2 = new QLabel(tr("GEOM_PCURVE_Y"), groupBox);
  myYExpr = new QLineEdit( groupBox );

  //Z Equation
  QLabel* textLabel3 = new QLabel( tr("GEOM_PCURVE_Z"), groupBox );
  myZExpr = new QLineEdit( groupBox );

  // Min and Max
  QLabel* textLabel4 = new QLabel( tr("GEOM_PCURVE_MIN"), groupBox );
  myPMin = new SalomeApp_DoubleSpinBox( groupBox );

  // Min and Max
  QLabel* textLabel5 = new QLabel( tr("GEOM_PCURVE_MAX"), groupBox );
  myPMax = new SalomeApp_DoubleSpinBox( groupBox );    

  // Step
  QLabel* textLabel6 = new QLabel( tr("GEOM_PCURVE_NBSTEP"), groupBox );
  myPStep = new SalomeApp_IntSpinBox( groupBox );

  //Layout
  gridLayout->addWidget(textLabel1, 0, 0, 1, 1);
  gridLayout->addWidget(myXExpr,    0, 1, 1, 1);

  gridLayout->addWidget(textLabel2, 1, 0, 1, 1);
  gridLayout->addWidget(myYExpr,    1, 1, 1, 1);

  gridLayout->addWidget(textLabel3, 2, 0, 1, 1);
  gridLayout->addWidget(myZExpr,    2, 1, 1, 1);

  gridLayout->addWidget(textLabel4, 3, 0, 1, 1);
  gridLayout->addWidget(myPMin,     3, 1, 1, 1);

  gridLayout->addWidget(textLabel5, 4, 0, 1, 1);
  gridLayout->addWidget(myPMax,     4, 1, 1, 1);

  gridLayout->addWidget(textLabel6, 5, 0, 1, 1);
  gridLayout->addWidget(myPStep,    5, 1, 1, 1);

  l->addWidget(groupBox, 0, 0, 1, 1);
}


BasicGUI_ParamCurveWidget::~BasicGUI_ParamCurveWidget(){
}
