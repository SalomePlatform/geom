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
// File:	DlgRef_3Sel1Spin.cxx
// Created:	Thu May 13 12:19:31 2004
// Author:	Alexander SLADKOV
//		<asv@yandex.nnov.opencascade.com>


#include "DlgRef_3Sel1Spin.h"

//=================================================================================
// class    : DlgRef_3Sel1Spin
// purpose  : 
//=================================================================================
DlgRef_3Sel1Spin::DlgRef_3Sel1Spin( QWidget* parent, const char* name, WFlags fl )
  :DlgRef_3Sel_QTD( parent, name, fl )
{
  Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

  TextLabel4 = new QLabel( (QWidget*)GroupBox1, "TextLabel4" );
  TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
  Layout2->addWidget( TextLabel4, 0, 0 );

  SpinBox_DX = new DlgRef_SpinBox( (QWidget*)GroupBox1, "SpinBox_DX" );
  Layout2->addWidget( SpinBox_DX, 0, 1 );

  Layout1->addMultiCellLayout( Layout2, 3, 3, 0, 2 );    
  QSpacerItem* spacer = new QSpacerItem( 0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  Layout1->addItem( spacer, 4, 2 );
}

DlgRef_3Sel1Spin::~DlgRef_3Sel1Spin()
{
}
