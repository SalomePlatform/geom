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
//  File   : DlgRef_1Sel2Spin.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "DlgRef_1Sel2Spin.h"

#include <qlayout.h>
#include <qspinbox.h>
#include <qgroupbox.h>

/* 
 *  Constructs a DlgRef_1Sel2Spin which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgRef_1Sel2Spin::DlgRef_1Sel2Spin(QWidget* parent,  const char* name, WFlags fl)
  :DlgRef_1Sel2Spin_QTD(parent, name, fl)
{
  SpinBox1->close(TRUE);
  SpinBox2->close(TRUE);

  SpinBox_DX = new DlgRef_SpinBox(GroupBox1, "SpinBox_DX");
  Layout2->addWidget(SpinBox_DX, 0, 1);

  SpinBox_DY = new DlgRef_SpinBox( GroupBox1, "SpinBox_DY");
  Layout2->addWidget(SpinBox_DY, 1, 1);

}


/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_1Sel2Spin::~DlgRef_1Sel2Spin()
{
    // no need to delete child widgets, Qt does it all for us
}
