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
//  File   : DlgRef_2Sel1Spin.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef DLGREF_2SEL1SPIN_H
#define DLGREF_2SEL1SPIN_H

#include "DlgRef_2Sel1Spin_QTD.h"
#include "DlgRef_SpinBox.h"

class DlgRef_2Sel1Spin : public DlgRef_2Sel1Spin_QTD
{ 
    Q_OBJECT

public:
    DlgRef_2Sel1Spin( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgRef_2Sel1Spin();

    DlgRef_SpinBox* SpinBox_DX;

};

#endif // DLGREF_2SEL1SPIN_H
