/****************************************************************************
** Form implementation generated from reading ui file 'MeasureGUI_2Sel1LineEdit_QTD.ui'
**
** Created: Tue Oct 9 14:45:59 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MeasureGUI_2Sel1LineEdit_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MeasureGUI_2Sel1LineEdit_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
MeasureGUI_2Sel1LineEdit_QTD::MeasureGUI_2Sel1LineEdit_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MeasureGUI_2Sel1LineEdit_QTD" );
    MeasureGUI_2Sel1LineEdit_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "MeasureGUI_2Sel1LineEdit_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton2, 1, 1 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout1->addWidget( LineEdit2, 1, 2 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel3, 2, 0 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    Layout1->addMultiCellWidget( LineEdit3, 2, 2, 1, 2 );
    Spacer8 = new QSpacerItem( 0, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( Spacer8, 3, 2 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    MeasureGUI_2Sel1LineEdit_QTDLayout->addWidget( GroupBox1, 0, 0 );
    languageChange();
    resize( QSize(129, 115).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_2Sel1LineEdit_QTD::~MeasureGUI_2Sel1LineEdit_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MeasureGUI_2Sel1LineEdit_QTD::languageChange()
{
}

