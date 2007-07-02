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
//  File   : DlgRef_1Sel5Spin.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "DlgRef_1Sel5Spin.h"
#include "DlgRef_SpinBox.h"

/* 
 *  Constructs a DlgRef_1Sel5Spin which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgRef_1Sel5Spin::DlgRef_1Sel5Spin(QWidget* parent,  const char* name, Qt::WindowFlags fl)
  :QWidget(parent, fl)
{
  setupUi(this);

  setObjectName(name);

  SpinBox1->setAttribute( Qt::WA_DeleteOnClose );
  SpinBox1->close();

  SpinBox2->setAttribute( Qt::WA_DeleteOnClose );
  SpinBox2->close();

  SpinBox3->setAttribute( Qt::WA_DeleteOnClose );
  SpinBox3->close();

  SpinBox4->setAttribute( Qt::WA_DeleteOnClose );
  SpinBox4->close();

  SpinBox5->setAttribute( Qt::WA_DeleteOnClose );
  SpinBox5->close();

  SpinBox_1 = new DlgRef_SpinBox(GroupBox1, "SpinBox_1");
  gridLayout3->addWidget(SpinBox_1, 0, 1);

  SpinBox_2 = new DlgRef_SpinBox(GroupBox1, "SpinBox_2");
  gridLayout3->addWidget(SpinBox_2, 0, 3);

  SpinBox_3 = new DlgRef_SpinBox(GroupBox1, "SpinBox_3");
  gridLayout3->addWidget(SpinBox_3, 2, 1);

  SpinBox_4 = new DlgRef_SpinBox(GroupBox1, "SpinBox_4");
  gridLayout3->addWidget(SpinBox_4, 1, 1);

  SpinBox_5 = new DlgRef_SpinBox(GroupBox1, "SpinBox_5");
  gridLayout3->addWidget(SpinBox_5, 1, 3);
}


/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_1Sel5Spin::~DlgRef_1Sel5Spin()
{
    // no need to delete child widgets, Qt does it all for us
}
