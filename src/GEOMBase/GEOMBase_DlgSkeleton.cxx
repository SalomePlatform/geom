// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMBase_DlgSkeleton.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S.

#include "GEOMBase_DlgSkeleton.h"

//Qt includes
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>


/*!
  Constructor.
*/
GEOMBase_DlgSkeleton::GEOMBase_DlgSkeleton( QWidget* parent, Qt::WindowFlags flags ) :
  QWidget(parent,flags)
{
  init();
}

/*!
  Destructor.
*/
GEOMBase_DlgSkeleton::~GEOMBase_DlgSkeleton( ) {

}

/*!
  Initialize the widget
*/
void GEOMBase_DlgSkeleton::init( ) {
  // Main layout
  gridLayout = new QGridLayout(this);

  //Constructors group
  GroupConstructors = new QGroupBox(this);
  GroupConstructors->setObjectName(QString::fromUtf8("GroupConstructors"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(GroupConstructors->sizePolicy().hasHeightForWidth());
  GroupConstructors->setSizePolicy(sizePolicy);
  hboxLayout = new QHBoxLayout(GroupConstructors);

  RadioButton1 = new QRadioButton(GroupConstructors);
  hboxLayout->addWidget(RadioButton1);

  RadioButton2 = new QRadioButton(GroupConstructors);
  hboxLayout->addWidget(RadioButton2);
  
  RadioButton3 = new QRadioButton(GroupConstructors);
  hboxLayout->addWidget(RadioButton3);

  RadioButton4 = new QRadioButton(GroupConstructors);
  hboxLayout->addWidget(RadioButton4);

  RadioButton5 = new QRadioButton(GroupConstructors);
  hboxLayout->addWidget(RadioButton5);
  gridLayout->addWidget(GroupConstructors, 0, 0, 1, 1);

  //Name group
  GroupBoxName = new QGroupBox(this);
  sizePolicy.setHeightForWidth(GroupBoxName->sizePolicy().hasHeightForWidth());
  GroupBoxName->setSizePolicy(sizePolicy);
  hboxLayout1 = new QHBoxLayout(GroupBoxName);
  hboxLayout1->setContentsMargins(9, 9, 9, 9);

  NameLabel = new QLabel(GroupBoxName);
  NameLabel->setWordWrap(false);
  hboxLayout1->addWidget(NameLabel);

  ResultName = new QLineEdit(GroupBoxName);
  hboxLayout1->addWidget(ResultName);
  gridLayout->addWidget(GroupBoxName, 1, 0, 1, 1);

  //Group box with the check-boxes
  GroupMedium = new QWidget(this);
  QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(GroupMedium->sizePolicy().hasHeightForWidth());
  GroupMedium->setSizePolicy(sizePolicy1);

  gridLayout->addWidget(GroupMedium, 2, 0, 1, 1);
  GroupBoxPublish = new QGroupBox(this);
  vboxLayout = new QVBoxLayout(GroupBoxPublish);

  CheckBoxRestoreSS = new QCheckBox(GroupBoxPublish);
  vboxLayout->addWidget(CheckBoxRestoreSS);

  CheckBoxAddPrefix = new QCheckBox(GroupBoxPublish);
  vboxLayout->addWidget(CheckBoxAddPrefix);

  CheckBoxPreview = new QCheckBox(GroupBoxPublish);
  vboxLayout->addWidget(CheckBoxPreview);
  gridLayout->addWidget(GroupBoxPublish, 3, 0, 1, 1);

  //Group with buttons
  GroupButtons = new QGroupBox(this);

  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(GroupButtons->sizePolicy().hasHeightForWidth());
  GroupButtons->setSizePolicy(sizePolicy2);

  hboxLayout2 = new QHBoxLayout(GroupButtons);
  hboxLayout2->setContentsMargins(9, 9, 9, 9);
  buttonOk = new QPushButton(GroupButtons);
  hboxLayout2->addWidget(buttonOk);

  buttonApply = new QPushButton(GroupButtons);
  hboxLayout2->addWidget(buttonApply);
  
  QSpacerItem* spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
  hboxLayout2->addItem(spacerItem);
  
  buttonCancel = new QPushButton(GroupButtons);
  hboxLayout2->addWidget(buttonCancel);
  
  buttonHelp = new QPushButton(GroupButtons);
  
  hboxLayout2->addWidget(buttonHelp);
  
  gridLayout->addWidget(GroupButtons, 4, 0, 1, 1);
  
  setTabOrder(CheckBoxRestoreSS, CheckBoxAddPrefix);
  setTabOrder(CheckBoxAddPrefix, CheckBoxPreview);
  setTabOrder(CheckBoxPreview, buttonOk);
  setTabOrder(buttonOk, buttonApply);
  setTabOrder(buttonApply, buttonCancel);
  setTabOrder(buttonCancel, buttonHelp);
  setTabOrder(buttonHelp, RadioButton1);
  setTabOrder(RadioButton1, RadioButton2);
  setTabOrder(RadioButton2, RadioButton3);
  setTabOrder(RadioButton3, RadioButton4);
  setTabOrder(RadioButton4, RadioButton5);
  setTabOrder(RadioButton5, ResultName);

  buttonOk->setText(tr("A&pply and Close"));
  buttonApply->setText("&Apply");
  buttonCancel->setText("&Close");
  buttonHelp->setText("&Help");
}
