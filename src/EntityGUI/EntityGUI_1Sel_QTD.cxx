/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_1Sel_QTD.ui'
**
** Created: Fri Apr 30 11:23:36 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_1Sel_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_1Sel_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_1Sel_QTD::EntityGUI_1Sel_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_1Sel_QTD" );
    resize( 245, 110 ); 
    setCaption( trUtf8( "EntityGUI_1Sel_QTD" ) );
    EntityGUI_1Sel_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_1Sel_QTDLayout"); 

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

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 180, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer_2, 1, 2 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );
    LineEdit1->setFocusPolicy( QLineEdit::NoFocus );

    Layout1->addWidget( LineEdit1, 0, 2 );

    Layout3->addLayout( Layout1, 0, 0 );

    GroupBox1Layout->addLayout( Layout3, 0, 0 );

    EntityGUI_1Sel_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_1Sel_QTD::~EntityGUI_1Sel_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

