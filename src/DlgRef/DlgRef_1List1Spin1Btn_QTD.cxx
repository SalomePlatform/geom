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
** Form implementation generated from reading ui file 'DlgRef_1List1Spin1Btn_QTD.ui'
**
** Created: Wed Mar 17 11:29:24 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_1List1Spin1Btn_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DlgRef_1List1Spin1Btn_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_1List1Spin1Btn_QTD::DlgRef_1List1Spin1Btn_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_1List1Spin1Btn_QTD" );
    resize( 204, 148 ); 
    setCaption( trUtf8( "DlgRef_3Sel4Spin2Check_QTD" ) );
    DlgRef_1List1Spin1Btn_QTDLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgRef_1List1Spin1Btn_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer = new QSpacerItem( 0, 23, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( spacer, 3, 1 );

    ListView1 = new QListView( GroupBox1, "ListView1" );

    GroupBox1Layout->addMultiCellWidget( ListView1, 0, 3, 0, 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setText( trUtf8( "TL1" ) );

    GroupBox1Layout->addWidget( TextLabel1, 0, 1 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );

    GroupBox1Layout->addWidget( SpinBox1, 1, 1 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setText( trUtf8( "" ) );

    GroupBox1Layout->addWidget( PushButton1, 2, 1 );

    DlgRef_1List1Spin1Btn_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_1List1Spin1Btn_QTD::~DlgRef_1List1Spin1Btn_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

