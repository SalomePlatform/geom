using namespace std;
//  File      : GeometryGUI_SpinBox.cxx
//  Created   : 5 june 2002
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

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
