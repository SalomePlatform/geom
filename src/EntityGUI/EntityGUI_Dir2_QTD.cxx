// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_Dir2_QTD.ui'
**
** Created: ven déc 12 11:17:10 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_Dir2_QTD.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_Dir2_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_Dir2_QTD::EntityGUI_Dir2_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_Dir2_QTD" );
    resize( 124, 106 ); 
    setCaption( trUtf8( "EntityGUI_Dir2_QTD" ) );
    EntityGUI_Dir2_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_Dir2_QTDLayout"); 

    GroupDir2 = new QButtonGroup( this, "GroupDir2" );
    GroupDir2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupDir2->sizePolicy().hasHeightForWidth() ) );
    GroupDir2->setTitle( trUtf8( "Direction" ) );
    GroupDir2->setColumnLayout(0, Qt::Vertical );
    GroupDir2->layout()->setSpacing( 6 );
    GroupDir2->layout()->setMargin( 11 );
    GroupDir2Layout = new QGridLayout( GroupDir2->layout() );
    GroupDir2Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    RB_Dir22 = new QRadioButton( GroupDir2, "RB_Dir22" );
    RB_Dir22->setText( trUtf8( "X" ) );

    Layout1->addWidget( RB_Dir22, 1, 0 );

    RB_Dir23 = new QRadioButton( GroupDir2, "RB_Dir23" );
    RB_Dir23->setText( trUtf8( "Y" ) );

    Layout1->addWidget( RB_Dir23, 2, 0 );
    QSpacerItem* spacer = new QSpacerItem( 0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 3, 0 );

    RB_Dir21 = new QRadioButton( GroupDir2, "RB_Dir21" );
    RB_Dir21->setText( trUtf8( "Length" ) );

    Layout1->addWidget( RB_Dir21, 0, 0 );

    GroupDir2Layout->addLayout( Layout1, 0, 0 );

    EntityGUI_Dir2_QTDLayout->addWidget( GroupDir2, 0, 0 );

    // tab order
    setTabOrder( RB_Dir21, RB_Dir22 );
    setTabOrder( RB_Dir22, RB_Dir23 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_Dir2_QTD::~EntityGUI_Dir2_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

