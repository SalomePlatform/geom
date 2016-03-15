// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : EntityGUI_Widgets.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "EntityGUI_Widgets.h"
#include <QButtonGroup>

//////////////////////////////////////////
// EntityGUI_1Sel
//////////////////////////////////////////

EntityGUI_1Sel::EntityGUI_1Sel( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_1Sel::~EntityGUI_1Sel()
{
}

//////////////////////////////////////////
// EntityGUI_2Sel1Check
//////////////////////////////////////////

EntityGUI_2Sel1Check::EntityGUI_2Sel1Check( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_2Sel1Check::~EntityGUI_2Sel1Check()
{
}


//////////////////////////////////////////
// EntityGUI_1Sel1Spin1Check
//////////////////////////////////////////

EntityGUI_1Sel1Spin1Check::EntityGUI_1Sel1Spin1Check( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_1Sel1Spin1Check::~EntityGUI_1Sel1Spin1Check()
{
}

//////////////////////////////////////////
// EntityGUI_1Spin
//////////////////////////////////////////

EntityGUI_1Spin::EntityGUI_1Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_1Spin::~EntityGUI_1Spin()
{
}

//////////////////////////////////////////
// EntityGUI_2Spin
//////////////////////////////////////////

EntityGUI_2Spin::EntityGUI_2Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_2Spin::~EntityGUI_2Spin()
{
}

//////////////////////////////////////////
// EntityGUI_3Spin
//////////////////////////////////////////

EntityGUI_3Spin::EntityGUI_3Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_3Spin::~EntityGUI_3Spin()
{
}


//////////////////////////////////////////
// EntityGUI_3Spin1Check
//////////////////////////////////////////

EntityGUI_3Spin1Check::EntityGUI_3Spin1Check( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_3Spin1Check::~EntityGUI_3Spin1Check()
{
}

//////////////////////////////////////////
// EntityGUI_4Spin
//////////////////////////////////////////

EntityGUI_4Spin::EntityGUI_4Spin( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_4Spin::~EntityGUI_4Spin()
{
}


//////////////////////////////////////////
// EntityGUI_4Spin1Check
//////////////////////////////////////////

EntityGUI_4Spin1Check::EntityGUI_4Spin1Check( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_4Spin1Check::~EntityGUI_4Spin1Check()
{
}

//////////////////////////////////////////
// EntityGUI_Dir1
//////////////////////////////////////////

EntityGUI_Dir1::EntityGUI_Dir1( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );

  ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( RB_Dir11, 2 );
  ButtonGroup->addButton( RB_Dir12, 0 );
  ButtonGroup->addButton( RB_Dir13, 1 );
  ButtonGroup->addButton( RB_Dir14, 3 );
}

EntityGUI_Dir1::~EntityGUI_Dir1()
{
}

//////////////////////////////////////////
// EntityGUI_Dir2
//////////////////////////////////////////

EntityGUI_Dir2::EntityGUI_Dir2( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );

  ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( RB_Dir21, 2 );
  ButtonGroup->addButton( RB_Dir22, 0 );
  ButtonGroup->addButton( RB_Dir23, 1 );
}

EntityGUI_Dir2::~EntityGUI_Dir2()
{
}

//////////////////////////////////////////
// EntityGUI_Point
//////////////////////////////////////////

EntityGUI_Point::EntityGUI_Point( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );

  ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( RB_Point1, 1 );
  ButtonGroup->addButton( RB_Point2, 0 );
  ButtonGroup->addButton( RB_Point3, 2 );
}

EntityGUI_Point::~EntityGUI_Point()
{
}

//////////////////////////////////////////
// EntityGUI_Type
//////////////////////////////////////////

EntityGUI_Type::EntityGUI_Type( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_Type::~EntityGUI_Type()
{
}

//////////////////////////////////////////
// EntityGUI_Angles
//////////////////////////////////////////

EntityGUI_Angles::EntityGUI_Angles( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_Angles::~EntityGUI_Angles()
{
}

//////////////////////////////////////////
// EntityGUI_Controls
//////////////////////////////////////////

EntityGUI_Controls::EntityGUI_Controls( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );
}

EntityGUI_Controls::~EntityGUI_Controls()
{
}

//////////////////////////////////////////
// EntityGUI_Skeleton
//////////////////////////////////////////

EntityGUI_Skeleton::EntityGUI_Skeleton( QWidget* parent, Qt::WindowFlags f )
: QWidget( parent, f )
{
  setupUi( this );

  ButtonGroup = new QButtonGroup( this );
  ButtonGroup->addButton( RadioButton1, 0 );
  ButtonGroup->addButton( RadioButton2, 1 );
  ButtonGroup->addButton( RadioButton3, 2 );
}

EntityGUI_Skeleton::~EntityGUI_Skeleton()
{
}

