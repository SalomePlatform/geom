//  File      : GeometryGUI_SpinBox.h
//  Created   : 4 july 2002
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef  GEOMSPINBOX_H
#define  GEOMSPINBOX_H

#include "QAD_SpinBoxDbl.h" 

class GeometryGUI ;

//=================================================================================
// class    : GeometryGUI_SpinBox
// purpose  : Derivated from QSpinBox class and modified to accept floats
//=================================================================================
class GeometryGUI_SpinBox : public QAD_SpinBoxDbl
{
  Q_OBJECT

public :
  GeometryGUI_SpinBox( QWidget* parent, const char* name = 0 );
  ~GeometryGUI_SpinBox();
  
  void    RangeStepAndValidator( double         min = -1000000.0, 
				 double         max = +1000000.0, 
				 double         step = 100.0, 
				 unsigned short decimals = 3 );
  void    SetValue( double v );
  double  GetValue();
  QString GetString();
  
public slots:
  void    SetStep( double newStep );

};
#endif //  GEOMSPINBOX_H
