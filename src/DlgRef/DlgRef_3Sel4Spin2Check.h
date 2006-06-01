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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : DlgRef_3Sel4Spin2Check.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#ifndef DLGREF_3SEL4SPIN2CHECK_H
#define DLGREF_3SEL4SPIN2CHECK_H

#include "DlgRef_3Sel4Spin2Check_QTD.h"
#include "DlgRef_SpinBox.h"

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define DLGREF_WNT_EXPORT __declspec( dllexport )
#else
#define DLGREF_WNT_EXPORT
#endif

class DLGREF_WNT_EXPORT DlgRef_3Sel4Spin2Check : public DlgRef_3Sel4Spin2Check_QTD
{ 
    Q_OBJECT

public:
    DlgRef_3Sel4Spin2Check(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~DlgRef_3Sel4Spin2Check();

    DlgRef_SpinBox* SpinBox_DX1;
    DlgRef_SpinBox* SpinBox_DY1;
    DlgRef_SpinBox* SpinBox_DX2;
    DlgRef_SpinBox* SpinBox_DY2;

};

#endif // DLGREF_3SEL4SPIN2CHECK_H
