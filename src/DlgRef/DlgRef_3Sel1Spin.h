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
//  File   : DlgRef_3Sel1Spin.h
//  Author : Alexander SLADKOV
//  Module : GEOM
//  $Header$

#ifndef DlgRef_3SEL_1SPIN_H
#define DlgRef_3SEL_1SPIN_H

#include "DlgRef_3Sel_QTD.h"
#include "DlgRef_SpinBox.h"
#include <qlayout.h>
#include <qlabel.h>

//=================================================================================
// class    : DlgRef_3Sel1Spin
// purpose  : extension of a DlgRef_3Sel_QTD class, 1 additional spin box is added
//=================================================================================
class DlgRef_3Sel1Spin : public DlgRef_3Sel_QTD
{
  Q_OBJECT

  public:
    DlgRef_3Sel1Spin( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_3Sel1Spin();

    QLabel* TextLabel4;
    DlgRef_SpinBox* SpinBox_DX;

  protected:
    QGridLayout* Layout2;
};

#endif // DlgRef_3SEL_1SPIN_H
