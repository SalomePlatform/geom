/****************************************************************************
** Form implementation generated from reading ui file 'MeasureGUI_1Sel12LineEdit_QTD.ui'
**
** Created: Tue Feb 17 11:08:48 2009
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "MeasureGUI_1Sel12LineEdit_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MeasureGUI_1Sel12LineEdit_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
MeasureGUI_1Sel12LineEdit_QTD::MeasureGUI_1Sel12LineEdit_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MeasureGUI_1Sel12LineEdit_QTD" );
    MeasureGUI_1Sel12LineEdit_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "MeasureGUI_1Sel12LineEdit_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    LineEdit12 = new QLineEdit( GroupBox1, "LineEdit12" );

    Layout2->addWidget( LineEdit12, 0, 2 );

    LineEdit11 = new QLineEdit( GroupBox1, "LineEdit11" );

    Layout2->addWidget( LineEdit11, 0, 1 );

    LineEdit42 = new QLineEdit( GroupBox1, "LineEdit42" );

    Layout2->addWidget( LineEdit42, 3, 2 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel5, 3, 0 );

    LineEdit31 = new QLineEdit( GroupBox1, "LineEdit31" );

    Layout2->addWidget( LineEdit31, 2, 1 );

    LineEdit41 = new QLineEdit( GroupBox1, "LineEdit41" );

    Layout2->addWidget( LineEdit41, 3, 1 );

    LineEdit23 = new QLineEdit( GroupBox1, "LineEdit23" );

    Layout2->addWidget( LineEdit23, 1, 3 );

    LineEdit33 = new QLineEdit( GroupBox1, "LineEdit33" );

    Layout2->addWidget( LineEdit33, 2, 3 );

    LineEdit43 = new QLineEdit( GroupBox1, "LineEdit43" );

    Layout2->addWidget( LineEdit43, 3, 3 );

    LineEdit22 = new QLineEdit( GroupBox1, "LineEdit22" );

    Layout2->addWidget( LineEdit22, 1, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel2, 0, 0 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel4, 2, 0 );

    LineEdit13 = new QLineEdit( GroupBox1, "LineEdit13" );

    Layout2->addWidget( LineEdit13, 0, 3 );

    LineEdit32 = new QLineEdit( GroupBox1, "LineEdit32" );

    Layout2->addWidget( LineEdit32, 2, 2 );

    LineEdit21 = new QLineEdit( GroupBox1, "LineEdit21" );

    Layout2->addWidget( LineEdit21, 1, 1 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel3, 1, 0 );

    Layout1->addMultiCellLayout( Layout2, 1, 1, 0, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );
    LineEdit1->setMinimumSize( QSize( 250, 0 ) );

    Layout1->addWidget( LineEdit1, 0, 2 );
    Spacer8 = new QSpacerItem( 0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( Spacer8, 2, 2 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    MeasureGUI_1Sel12LineEdit_QTDLayout->addWidget( GroupBox1, 0, 0 );
    languageChange();
    resize( QSize(131, 171).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_1Sel12LineEdit_QTD::~MeasureGUI_1Sel12LineEdit_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MeasureGUI_1Sel12LineEdit_QTD::languageChange()
{
    setCaption( tr( "MeasureGUI_1Sel12LineEdit_QTD" ) );
    GroupBox1->setTitle( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel2->setText( tr( "TL2" ) );
    TextLabel4->setText( tr( "TL4" ) );
    TextLabel3->setText( tr( "TL3" ) );
    PushButton1->setText( QString::null );
    TextLabel1->setText( tr( "TL1" ) );
}

