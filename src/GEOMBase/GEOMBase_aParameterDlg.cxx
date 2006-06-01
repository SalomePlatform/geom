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
//  File   : GEOMBase_aParameterDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GEOMBase_aParameterDlg.h"
#include "QtxDblSpinBox.h"

#include <stdio.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qvalidator.h>

#ifndef WNT
using namespace std;
#endif


//====================================================================================== 
// function : GEOMBase_aParameterDlg()
// purpose  : Constructs a GEOMBase_aParametertDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'
//
//  avalue1    : is a float or integer used as default value in edit line
//  aTitle1    : is the prompt for aValue1
//  aTitle     : is the title for the user in dialog box
//
//  bottom     : the minimal value to be entered
//  top        : the maximum value to be entered
//  decimals   : number of decimals to be entered
//
//  The dialog will by default be modeless, unless you set 'modal' to
//  TRUE to construct a modal dialog.
// 
//====================================================================================== 
GEOMBase_aParameterDlg::GEOMBase_aParameterDlg(const char *aValue1, const char *aTitle1, QWidget* parent, const char* name, bool modal, WFlags fl, const double bottom, const double top, const int decimals)
  :QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  if(!name)
    setName( "MyParameterDialog" );
  resize(288, 81); 
  setCaption(name); /* appears on the title bar */
  setSizeGripEnabled(TRUE);

  QGridLayout* topLayout = new QGridLayout(this); 
  topLayout->setSpacing(6);
  topLayout->setMargin(11);

  QGroupBox* mainGrp = new QGroupBox(this, "mainGrp");
  mainGrp->setColumnLayout(0, Qt::Vertical);
  mainGrp->layout()->setSpacing(0);
  mainGrp->layout()->setMargin(0);
  QGridLayout* mainGrpLayout = new QGridLayout(mainGrp->layout());
  mainGrpLayout->setAlignment(Qt::AlignTop);
  mainGrpLayout ->setSpacing(6);
  mainGrpLayout->setMargin(11);
  topLayout->addWidget(mainGrp, 0, 0);

  /* aTitle1 : text prompt on left of edit line */
  QLabel* TextLabel1 = new QLabel(mainGrp, "TextLabel1");
  TextLabel1->setText(tr(aTitle1));  
  mainGrpLayout->addWidget(TextLabel1, 0, 0);

  mySpinBox = new QtxDblSpinBox(mainGrp, "mySpinBox");
  mySpinBox->setPrecision(12);
  mySpinBox->setRange(bottom, top);
  ((QDoubleValidator*)(mySpinBox->validator()))->setRange(bottom, top, decimals);
  mySpinBox->setValue(QString(aValue1).toDouble());
  mainGrpLayout->addWidget(mySpinBox, 0, 1);
  
  QGroupBox* btnGrp = new QGroupBox(this, "btnGrp");
  btnGrp->setColumnLayout(0, Qt::Vertical);
  btnGrp->layout()->setSpacing(0);
  btnGrp->layout()->setMargin(0);
  QGridLayout* btnGrpLayout = new QGridLayout(btnGrp->layout());
  btnGrpLayout->setAlignment(Qt::AlignTop);
  btnGrpLayout->setSpacing(6);
  btnGrpLayout->setMargin(11);
  topLayout->addWidget(btnGrp, 1, 0);

  /* Ok button */
  myButtonOk = new QPushButton(btnGrp, "buttonOk");
  myButtonOk->setText(tr("GEOM_BUT_OK"));
  myButtonOk->setAutoDefault(TRUE);
  myButtonOk->setDefault(TRUE);
  btnGrpLayout->addWidget(myButtonOk, 0, 0);

  btnGrpLayout->addItem(new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1);

  /* Cancel button */
  myButtonCancel = new QPushButton(btnGrp, "buttonCancel");
  myButtonCancel->setText(tr("GEOM_BUT_CANCEL"));
  myButtonCancel->setAutoDefault(TRUE);
  btnGrpLayout->addWidget(myButtonCancel, 0, 2);

  /* signals and slots connections */
  connect(myButtonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(myButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  
  /* Move widget on the botton right corner of main widget */
  //mzn: QAD_Tools::centerWidget(this, parent);
}


//====================================================================================== 
// function : ~GEOMBase_aParameterDlg() destructor
// purpose  : Destroys the object and frees any allocated resources
//====================================================================================== 
GEOMBase_aParameterDlg::~GEOMBase_aParameterDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//====================================================================================== 
// function : GEOMBase_aParameterDlg::setValue
// purpose  : sets value
//====================================================================================== 
void GEOMBase_aParameterDlg::setValue(double val)
{
  mySpinBox->setValue(val);
}


//====================================================================================== 
// function : GEOMBase_aParameterDlg::getValue
// purpose  : gets value
//====================================================================================== 
double GEOMBase_aParameterDlg::getValue()
{
  return mySpinBox->value();
}
