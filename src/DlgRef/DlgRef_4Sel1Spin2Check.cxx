//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : DlgRef_4Sel1Spin2Check.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 
//
#include "DlgRef_4Sel1Spin2Check.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qgroupbox.h>

/* 
 *  Constructs a DlgRef_4Sel1Spin2Check which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
DlgRef_4Sel1Spin2Check::DlgRef_4Sel1Spin2Check(QWidget* parent, const char* name, WFlags fl)
  :DlgRef_4Sel1Spin2Check_QTD(parent, name, fl)
{
  SpinBox1->close(TRUE);
  SpinBox_DX = new DlgRef_SpinBox(GroupBox1, "SpinBox_DX");
  Layout1->addWidget(SpinBox_DX, 4, 2);
}


/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_4Sel1Spin2Check::~DlgRef_4Sel1Spin2Check()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgRef_4Sel1Spin2Check::ShowRows(int fromRow, int toRow, bool toShow)
{
  typedef void (QWidget::* ShowFun)(); 
  ShowFun pShowFun;    

  if (toShow)
    	pShowFun = &QWidget::show;
  else
        pShowFun = &QWidget::hide;

  if (fromRow >= 0 && toRow >= 0 && toRow >= fromRow)
  {
    if ( fromRow == 0)
    {
      (TextLabel1->*pShowFun)();
      (PushButton1->*pShowFun)();
      (LineEdit1->*pShowFun)();
    }
    if ( fromRow <= 1 && 1 <= toRow)
    {
      (TextLabel2->*pShowFun)();
      (PushButton2->*pShowFun)();
      (LineEdit2->*pShowFun)(); 
    }
    if ( fromRow <= 2 && 2 <= toRow)
    {
      (TextLabel4->*pShowFun)();
      (PushButton4->*pShowFun)();
      (LineEdit4->*pShowFun)(); 
    }
    if ( fromRow <= 3 && 3 <= toRow)
    {
      (TextLabel5->*pShowFun)();
      (PushButton5->*pShowFun)();
      (LineEdit5->*pShowFun)(); 
    }
    if ( fromRow <= 4 && 4 <= toRow)
    {
      (TextLabel3->*pShowFun)();
      (SpinBox_DX->*pShowFun)();
      (CheckButton2->*pShowFun)();
    }
    if ( fromRow <= 5 && 5 <= toRow)
    (CheckButton1->*pShowFun)();
  }
}

