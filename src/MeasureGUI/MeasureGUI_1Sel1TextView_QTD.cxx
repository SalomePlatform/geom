/****************************************************************************
** Form implementation generated from reading ui file 'MeasureGUI_1Sel1TextView_QTD.ui'
**
** Created: mar oct 28 14:01:15 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "MeasureGUI_1Sel1TextView_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a MeasureGUI_1Sel1TextView_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
MeasureGUI_1Sel1TextView_QTD::MeasureGUI_1Sel1TextView_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MeasureGUI_1Sel1TextView_QTD" );
    resize( 130, 160 ); 
    setCaption( trUtf8( "MeasureGUI_1Sel1TextView_QTD" ) );
    MeasureGUI_1Sel1TextView_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "MeasureGUI_1Sel1TextView_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
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

    TextEdit1 = new QTextEdit( GroupBox1, "TextEdit1" );

    Layout1->addMultiCellWidget( TextEdit1, 1, 1, 0, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    MeasureGUI_1Sel1TextView_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_1Sel1TextView_QTD::~MeasureGUI_1Sel1TextView_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

