//  File   : DlgRef_2Sel2Spin.cxx
//  Author : Dmitry MATVEITCHEV
//  Module : GEOM
//  $Header: 

#include "DlgRef_2Sel3Spin2Rb.h"
#include <iostream>

#include <qlayout.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>

/* 
 *  Constructs a DlgRef_2Sel3Spin2Rb which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgRef_2Sel3Spin2Rb::DlgRef_2Sel3Spin2Rb(QWidget* parent,  const char* name, WFlags fl)
  :DlgRef_2Sel3Spin2Rb_QTD(parent, name, fl)
{
  SpinBox1->close(TRUE);
  SpinBox2->close(TRUE);
  SpinBox3->close(TRUE);
  RadioButton1->setChecked(true);
  SpinBox_DX = new DlgRef_SpinBox(GroupBox1, "SpinBox_DX");
  Layout2->addMultiCellWidget(SpinBox_DX, 0, 0, 2, 4);
  SpinBox_DY = new DlgRef_SpinBox(GroupBox1, "SpinBox_DY");
  Layout2->addWidget(SpinBox_DY, 1, 2);
  SpinBox_DZ = new DlgRef_SpinBox(GroupBox1, "SpinBox_DZ");
  Layout2->addWidget(SpinBox_DZ, 1, 4);
  SpinBox_DY->setEnabled(false);
  SpinBox_DZ->setEnabled(false);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_2Sel3Spin2Rb::~DlgRef_2Sel3Spin2Rb()
{
    // no need to delete child widgets, Qt does it all for us
}
