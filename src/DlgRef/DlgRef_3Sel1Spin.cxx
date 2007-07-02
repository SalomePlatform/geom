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
// File:	DlgRef_3Sel1Spin.cxx
// Created:	Thu May 13 12:19:31 2004
// Author:	Alexander SLADKOV
//		<asv@yandex.nnov.opencascade.com>


#include "DlgRef_3Sel1Spin.h"
#include "DlgRef_SpinBox.h"

#include <QGridLayout>
#include <QLabel>


//=================================================================================
// class    : DlgRef_3Sel1Spin
// purpose  : 
//=================================================================================
DlgRef_3Sel1Spin::DlgRef_3Sel1Spin( QWidget* parent, const char* name, Qt::WindowFlags fl )
  :QWidget( parent, fl )
{
  setupUi(this);

  setObjectName(name);

  Layout2 = new QGridLayout(0); 
  Layout2->setMargin(0);
  Layout2->setSpacing(6);
  Layout2->setObjectName("Layout2");

  TextLabel4 = new QLabel( (QWidget*)GroupBox1 );
  TextLabel4->setObjectName( "TextLabel4" );
  QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(TextLabel3->sizePolicy().hasHeightForWidth());
  TextLabel4->setSizePolicy( sizePolicy );
  Layout2->addWidget( TextLabel4, 0, 0 );

  SpinBox_DX = new DlgRef_SpinBox( (QWidget*)GroupBox1, "SpinBox_DX" );
  Layout2->addWidget( SpinBox_DX, 0, 1 );

  gridLayout2->addLayout( Layout2, 3, 0, 1, 3 );    
  QSpacerItem* spacer = new QSpacerItem( 0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  gridLayout2->addItem( spacer, 4, 2 );
}

DlgRef_3Sel1Spin::~DlgRef_3Sel1Spin()
{
}
