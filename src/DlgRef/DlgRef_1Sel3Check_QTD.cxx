/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_1Sel3Check_QTD.ui'
**
** Created: lun nov 17 11:32:21 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_1Sel3Check_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a DlgRef_1Sel3Check_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_1Sel3Check_QTD::DlgRef_1Sel3Check_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_1Sel3Check_QTD" );
    resize( 129, 114 ); 
    setCaption( trUtf8( "DlgRef_1Sel3Check_QTD" ) );
    DlgRef_1Sel3Check_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_1Sel3Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    CheckButton2 = new QRadioButton( GroupBox1, "CheckButton2" );
    CheckButton2->setText( trUtf8( "" ) );

    Layout1->addMultiCellWidget( CheckButton2, 2, 2, 0, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 4, 2 );

    CheckButton3 = new QRadioButton( GroupBox1, "CheckButton3" );
    CheckButton3->setText( trUtf8( "" ) );

    Layout1->addMultiCellWidget( CheckButton3, 3, 3, 0, 2 );

    CheckButton1 = new QRadioButton( GroupBox1, "CheckButton1" );
    CheckButton1->setText( trUtf8( "" ) );

    Layout1->addMultiCellWidget( CheckButton1, 1, 1, 0, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    DlgRef_1Sel3Check_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_1Sel3Check_QTD::~DlgRef_1Sel3Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

