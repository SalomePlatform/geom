/****************************************************************************
** Form implementation generated from reading ui file 'MeasureGUI_1Sel3LineEdit_QTD.ui'
**
** Created: mar oct 28 16:11:13 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "MeasureGUI_1Sel3LineEdit_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a MeasureGUI_1Sel3LineEdit_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
MeasureGUI_1Sel3LineEdit_QTD::MeasureGUI_1Sel3LineEdit_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MeasureGUI_1Sel3LineEdit_QTD" );
    resize( 129, 141 ); 
    setCaption( trUtf8( "MeasureGUI_1Sel3LineEdit_QTD" ) );
    MeasureGUI_1Sel3LineEdit_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "MeasureGUI_1Sel3LineEdit_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout2->addWidget( TextLabel1, 0, 0 );

    LineEdit4 = new QLineEdit( GroupBox1, "LineEdit4" );

    Layout2->addMultiCellWidget( LineEdit4, 3, 3, 1, 2 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout2->addWidget( TextLabel3, 2, 0 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout2->addMultiCellWidget( LineEdit2, 1, 1, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 0, 80, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout2->addItem( spacer, 4, 2 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    Layout2->addMultiCellWidget( LineEdit3, 2, 2, 1, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout2->addWidget( LineEdit1, 0, 2 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );

    Layout2->addWidget( TextLabel4, 3, 0 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout2->addWidget( TextLabel2, 1, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout2->addWidget( PushButton1, 0, 1 );

    GroupBox1Layout->addLayout( Layout2, 0, 0 );

    MeasureGUI_1Sel3LineEdit_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_1Sel3LineEdit_QTD::~MeasureGUI_1Sel3LineEdit_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

