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
** Form implementation generated from reading ui file 'DlgRef_4Sel1List_QTD.ui'
**
** Created: lun oct 27 17:21:04 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_4Sel1List_QTD.h"

#include <qvariant.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DlgRef_4Sel1List_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_4Sel1List_QTD::DlgRef_4Sel1List_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_4Sel1List_QTD" );
    resize( 129, 175 ); 
    setCaption( trUtf8( "DlgRef_4Sel1List_QTD" ) );
    DlgRef_4Sel1List_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_4Sel1List_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );
    
    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );
   

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );
    PushButton2->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton2, 1, 1 );
    
    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout1->addWidget( LineEdit2, 1, 2 );

    
    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout1->addMultiCellWidget( TextLabel3, 2, 2, 0, 1 );
    
    ComboBox1 = new QComboBox( FALSE, GroupBox1, "ComboBox1" );
    ComboBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, ComboBox1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( ComboBox1, 2, 2 );
   

    GroupBox1Layout->addLayout( Layout1, 0, 0 );
    
    DlgRef_4Sel1List_QTDLayout->addWidget( GroupBox1, 0, 0 );

    
    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setTitle( trUtf8( "" ) );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 6 );
    GroupBox2->layout()->setMargin( 11 );
    GroupBox2Layout = new QGridLayout( GroupBox2->layout() );
    GroupBox2Layout->setAlignment( Qt::AlignTop );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 
    
    TextLabel4 = new QLabel( GroupBox2, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );
    
    Layout2->addWidget( TextLabel4, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 0, 80, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 5, 2 );

    PushButton3 = new QPushButton( GroupBox2, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );
    PushButton3->setText( trUtf8( "" ) );

    Layout2->addWidget( PushButton3, 0, 1 );

    LineEdit3 = new QLineEdit( GroupBox2, "LineEdit3" );

    Layout2->addWidget( LineEdit3, 0, 2 );


    TextLabel5 = new QLabel( GroupBox2, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );
    TextLabel5->setText( trUtf8( "TL5" ) );

    Layout2->addWidget( TextLabel5, 1, 0 );

    PushButton4 = new QPushButton( GroupBox2, "PushButton4" );
    PushButton4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton4->sizePolicy().hasHeightForWidth() ) );
    PushButton4->setText( trUtf8( "" ) );

    Layout2->addWidget( PushButton4, 1, 1 );

    LineEdit4 = new QLineEdit( GroupBox2, "LineEdit4" );

    Layout2->addWidget( LineEdit4, 1, 2 );

    GroupBox2Layout->addLayout( Layout2, 0, 0 );
    
    DlgRef_4Sel1List_QTDLayout->addWidget( GroupBox2, 1, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_4Sel1List_QTD::~DlgRef_4Sel1List_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

