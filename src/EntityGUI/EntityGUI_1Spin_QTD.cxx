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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_1Spin_QTD.ui'
**
** Created: Fri Jul 30 16:05:59 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_1Spin_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_1Spin_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_1Spin_QTD::EntityGUI_1Spin_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_1Spin_QTD" );
    resize( 255, 112 ); 
    setCaption( trUtf8( "EntityGUI_1Spin_QTD" ) );
    EntityGUI_1Spin_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_1Spin_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "Values" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox1, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 82, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 3, 1 );

    Layout4->addLayout( Layout1, 0, 0 );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    buttonRedo = new QPushButton( GroupBox1, "buttonRedo" );
    buttonRedo->setText( trUtf8( "Redo" ) );

    Layout2->addWidget( buttonRedo, 0, 1 );

    buttonUndo = new QPushButton( GroupBox1, "buttonUndo" );
    buttonUndo->setText( trUtf8( "Undo" ) );

    Layout2->addWidget( buttonUndo, 0, 0 );

    Layout3->addLayout( Layout2, 1, 0 );

    buttonApply = new QPushButton( GroupBox1, "buttonApply" );
    buttonApply->setText( trUtf8( "Create" ) );

    Layout3->addWidget( buttonApply, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout3->addItem( spacer_2, 2, 0 );

    Layout4->addLayout( Layout3, 0, 1 );

    GroupBox1Layout->addLayout( Layout4, 0, 0 );

    EntityGUI_1Spin_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_1Spin_QTD::~EntityGUI_1Spin_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

