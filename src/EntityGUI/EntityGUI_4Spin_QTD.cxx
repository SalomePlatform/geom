/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_4Spin_QTD.ui'
**
** Created: Fri Apr 30 10:56:21 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_4Spin_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_4Spin_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_4Spin_QTD::EntityGUI_4Spin_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_4Spin_QTD" );
    resize( 176, 154 ); 
    setCaption( trUtf8( "EntityGUI_4Spin_QTD" ) );
    EntityGUI_4Spin_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_4Spin_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "Values" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    buttonUndo = new QPushButton( GroupBox1, "buttonUndo" );
    buttonUndo->setText( trUtf8( "Undo" ) );

    Layout2->addWidget( buttonUndo, 1, 0 );

    buttonApply = new QPushButton( GroupBox1, "buttonApply" );
    buttonApply->setText( trUtf8( "Create" ) );

    Layout2->addWidget( buttonApply, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 0, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout2->addItem( spacer, 2, 0 );

    Layout3->addLayout( Layout2, 0, 1 );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    SpinBox3 = new QSpinBox( GroupBox1, "SpinBox3" );
    SpinBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox3->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox3, 2, 1 );

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox2, 1, 1 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout1->addWidget( TextLabel2, 1, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer_2, 4, 1 );

    SpinBox4 = new QSpinBox( GroupBox1, "SpinBox4" );
    SpinBox4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox4->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox4, 3, 1 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout1->addWidget( TextLabel3, 2, 0 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox1, 0, 1 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );

    Layout1->addWidget( TextLabel4, 3, 0 );

    Layout3->addLayout( Layout1, 0, 0 );

    GroupBox1Layout->addLayout( Layout3, 0, 0 );

    EntityGUI_4Spin_QTDLayout->addWidget( GroupBox1, 0, 0 );

    // tab order
    setTabOrder( buttonUndo, buttonApply );
    setTabOrder( buttonApply, SpinBox1 );
    setTabOrder( SpinBox1, SpinBox2 );
    setTabOrder( SpinBox2, SpinBox3 );
    setTabOrder( SpinBox3, SpinBox4 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_4Spin_QTD::~EntityGUI_4Spin_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

