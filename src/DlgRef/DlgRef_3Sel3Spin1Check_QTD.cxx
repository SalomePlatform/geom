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
** Form implementation generated from reading ui file 'DlgRef_3Sel3Spin1Check_QTD.ui'
**
** Created: Mon Apr 19 16:36:48 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_3Sel3Spin1Check_QTD.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DlgRef_3Sel3Spin1Check_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_3Sel3Spin1Check_QTD::DlgRef_3Sel3Spin1Check_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_3Sel3Spin1Check_QTD" );
    resize( 380, 229 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "DlgRef_3Sel3Spin1Check_QTD" ) );
    DlgRef_3Sel3Spin1Check_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_3Sel3Spin1Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    SpinBox_2 = new QSpinBox( GroupBox1, "SpinBox_2" );
    SpinBox_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox_2, 4, 2 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    Layout1->addWidget( LineEdit3, 2, 2 );

    SpinBox_3 = new QSpinBox( GroupBox1, "SpinBox_3" );
    SpinBox_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_3->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox_3, 5, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout1->addWidget( TextLabel3, 2, 0 );

    CheckBox1 = new QCheckBox( GroupBox1, "CheckBox1" );
    CheckBox1->setText( trUtf8( "" ) );

    Layout1->addMultiCellWidget( CheckBox1, 6, 6, 0, 1 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );

    Layout1->addWidget( TextLabel4, 3, 0 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout1->addWidget( LineEdit2, 1, 2 );

    PushButton3 = new QPushButton( GroupBox1, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );
    PushButton3->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton3, 2, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );
    TextLabel6->setText( trUtf8( "TL6" ) );

    Layout1->addWidget( TextLabel6, 5, 0 );

    SpinBox_1 = new QSpinBox( GroupBox1, "SpinBox_1" );
    SpinBox_1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox_1, 3, 2 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );
    TextLabel5->setText( trUtf8( "TL5" ) );

    Layout1->addWidget( TextLabel5, 4, 0 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );
    PushButton2->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton2, 1, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 250, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 7, 2 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    DlgRef_3Sel3Spin1Check_QTDLayout->addWidget( GroupBox1, 0, 0 );

    // tab order
    setTabOrder( PushButton1, LineEdit1 );
    setTabOrder( LineEdit1, PushButton2 );
    setTabOrder( PushButton2, LineEdit2 );
    setTabOrder( LineEdit2, PushButton3 );
    setTabOrder( PushButton3, LineEdit3 );
    setTabOrder( LineEdit3, SpinBox_1 );
    setTabOrder( SpinBox_1, SpinBox_2 );
    setTabOrder( SpinBox_2, SpinBox_3 );
    setTabOrder( SpinBox_3, CheckBox1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_3Sel3Spin1Check_QTD::~DlgRef_3Sel3Spin1Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

