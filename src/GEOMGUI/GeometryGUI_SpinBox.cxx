//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GeometryGUI_SpinBox.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_SpinBox.h"
#include "GeometryGUI.h"
#include <qvalidator.h>
//#include <qpushbutton.h>
//#include "utilities.h"

//=================================================================================
// class    : GeometryGUI_SpinBox()
// purpose  : constructor of specific widget accepting floats in double precision.
//=================================================================================
GeometryGUI_SpinBox::GeometryGUI_SpinBox( QWidget* parent, const char* name )
: QAD_SpinBoxDbl( parent, name)
{
  /* when step value is changed in myGeomGUI */
  connect( GeometryGUI::GetGeometryGUI(), SIGNAL( SignalDefaultStepValueChanged( double )), this, SLOT( SetStep( double ) ) );
}


//=================================================================================
// function : SetStep()  [SLOT]
// purpose  :
//=================================================================================
void GeometryGUI_SpinBox::SetStep( double newStep )
{
  setLineStep( newStep );
}

//=================================================================================
// function : ~GeometryGUI_SpinBox()
// purpose  : destructor
//=================================================================================
GeometryGUI_SpinBox::~GeometryGUI_SpinBox()
{
}

//=================================================================================
// function : SetValue()
// purpose  :
//=================================================================================
void GeometryGUI_SpinBox::SetValue( double v )
{
  setValue( v );
}

//=================================================================================
// function : GetValue()
// purpose  : returns a double
//=================================================================================
double GeometryGUI_SpinBox::GetValue( )
{
  return value();
}

//=================================================================================
// function : GetString()
// purpose  : returns a QString
//=================================================================================
QString GeometryGUI_SpinBox::GetString( )
{
  return cleanText();
}

//=================================================================================
// function : RangeStepAndValidator()
// purpose  :
//=================================================================================
void GeometryGUI_SpinBox::RangeStepAndValidator( double min, double max, double step, unsigned short decimals )
{
  setRange( min, max );
  setLineStep( step );
  ( ( QDoubleValidator* )validator() )->setRange( min, max, decimals ) ;
}
