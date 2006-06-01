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
** Form implementation generated from reading ui file 'MeasureGUI_1Sel6LineEdit_QTD.ui'
**
** Created: mar oct 28 16:11:14 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "MeasureGUI_1Sel6LineEdit_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a MeasureGUI_1Sel6LineEdit_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
MeasureGUI_1Sel6LineEdit_QTD::MeasureGUI_1Sel6LineEdit_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MeasureGUI_1Sel6LineEdit_QTD" );
    resize( 129, 163 ); 
    setCaption( trUtf8( "MeasureGUI_1Sel6LineEdit_QTD" ) );
    MeasureGUI_1Sel6LineEdit_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "MeasureGUI_1Sel6LineEdit_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 
    QSpacerItem* spacer = new QSpacerItem( 0, 50, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 2, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    LineEdit22 = new QLineEdit( GroupBox1, "LineEdit22" );

    Layout2->addWidget( LineEdit22, 2, 2 );

    LineEdit32 = new QLineEdit( GroupBox1, "LineEdit32" );

    Layout2->addWidget( LineEdit32, 3, 2 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );

    Layout2->addWidget( TextLabel4, 1, 0 );

    LineEdit31 = new QLineEdit( GroupBox1, "LineEdit31" );

    Layout2->addWidget( LineEdit31, 3, 1 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );
    TextLabel5->setText( trUtf8( "TL5" ) );

    Layout2->addWidget( TextLabel5, 2, 0 );

    LineEdit11 = new QLineEdit( GroupBox1, "LineEdit11" );

    Layout2->addWidget( LineEdit11, 1, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );
    TextLabel6->setText( trUtf8( "TL6" ) );

    Layout2->addWidget( TextLabel6, 3, 0 );

    LineEdit12 = new QLineEdit( GroupBox1, "LineEdit12" );

    Layout2->addWidget( LineEdit12, 1, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout2->addWidget( TextLabel2, 0, 1 );

    LineEdit21 = new QLineEdit( GroupBox1, "LineEdit21" );

    Layout2->addWidget( LineEdit21, 2, 1 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout2->addWidget( TextLabel3, 0, 2 );

    Layout1->addMultiCellLayout( Layout2, 1, 1, 0, 2 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    MeasureGUI_1Sel6LineEdit_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_1Sel6LineEdit_QTD::~MeasureGUI_1Sel6LineEdit_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

