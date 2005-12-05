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
** Form implementation generated from reading ui file 'DlgRef_3Check_QTD.ui'
**
** Created: Tue Aug 24 11:27:47 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_3Check_QTD.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DlgRef_3Check_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_3Check_QTD::DlgRef_3Check_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_3Check_QTD" );
    resize( 135, 108 ); 
    setCaption( trUtf8( "DlgRef_3Check_QTD" ) );
    DlgRef_3Check_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_3Check_QTDLayout"); 

    GroupBox1 = new QButtonGroup( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    RadioButton3 = new QRadioButton( GroupBox1, "RadioButton3" );
    RadioButton3->setText( trUtf8( "RadioButton3" ) );

    Layout1->addWidget( RadioButton3, 2, 0 );

    RadioButton1 = new QRadioButton( GroupBox1, "RadioButton1" );
    RadioButton1->setText( trUtf8( "RadioButton1" ) );

    Layout1->addWidget( RadioButton1, 0, 0 );

    RadioButton2 = new QRadioButton( GroupBox1, "RadioButton2" );
    RadioButton2->setText( trUtf8( "RadioButton2" ) );

    Layout1->addWidget( RadioButton2, 1, 0 );
    QSpacerItem* spacer = new QSpacerItem( 0, 91, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 3, 0 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    DlgRef_3Check_QTDLayout->addWidget( GroupBox1, 0, 0 );

    // tab order
    setTabOrder( RadioButton1, RadioButton2 );
    setTabOrder( RadioButton2, RadioButton3 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_3Check_QTD::~DlgRef_3Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

