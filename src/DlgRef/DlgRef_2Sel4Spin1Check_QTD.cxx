/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_2Sel4Spin1Check_QTD.ui'
**
** Created: Tue Jun 1 12:38:04 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "DlgRef_2Sel4Spin1Check_QTD.h"

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
 *  Constructs a DlgRef_2Sel4Spin1Check_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_2Sel4Spin1Check_QTD::DlgRef_2Sel4Spin1Check_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_2Sel4Spin1Check_QTD" );
    resize( 262, 230 ); 
    setCaption( trUtf8( "DlgRef_2Sel4Spin1Check_QTD" ) );
    DlgRef_2Sel4Spin1Check_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_2Sel4Spin1Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );
    PushButton1->setText( trUtf8( "" ) );

    Layout5->addWidget( PushButton1, 0, 1 );

    CheckButton1 = new QCheckBox( GroupBox1, "CheckButton1" );
    CheckButton1->setText( trUtf8( "" ) );

    Layout5->addMultiCellWidget( CheckButton1, 3, 3, 0, 3 );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );
    TextLabel3->setText( trUtf8( "TL3" ) );

    Layout2->addWidget( TextLabel3, 0, 0 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox1, 0, 1 );

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox2, 1, 1 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );
    TextLabel4->setText( trUtf8( "TL4" ) );

    Layout2->addWidget( TextLabel4, 1, 0 );

    Layout5->addMultiCellLayout( Layout2, 2, 2, 0, 2 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout5->addWidget( LineEdit2, 1, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout5->addWidget( TextLabel1, 0, 0 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout5->addWidget( LineEdit1, 0, 2 );
    QSpacerItem* spacer = new QSpacerItem( 0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout5->addItem( spacer, 5, 3 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );
    PushButton2->setText( trUtf8( "" ) );

    Layout5->addWidget( PushButton2, 1, 1 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout5->addWidget( TextLabel2, 1, 0 );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 

    SpinBox4 = new QSpinBox( GroupBox1, "SpinBox4" );
    SpinBox4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox4->sizePolicy().hasHeightForWidth() ) );

    Layout3->addWidget( SpinBox4, 1, 1 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );
    TextLabel5->setText( trUtf8( "TL5" ) );

    Layout3->addWidget( TextLabel5, 0, 0 );

    SpinBox3 = new QSpinBox( GroupBox1, "SpinBox3" );
    SpinBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox3->sizePolicy().hasHeightForWidth() ) );

    Layout3->addWidget( SpinBox3, 0, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );
    TextLabel6->setText( trUtf8( "TL6" ) );

    Layout3->addWidget( TextLabel6, 1, 0 );

    Layout5->addMultiCellLayout( Layout3, 4, 4, 0, 2 );

    GroupBox1Layout->addLayout( Layout5, 0, 0 );

    DlgRef_2Sel4Spin1Check_QTDLayout->addWidget( GroupBox1, 0, 0 );

    // tab order
    setTabOrder( PushButton1, LineEdit1 );
    setTabOrder( LineEdit1, PushButton2 );
    setTabOrder( PushButton2, LineEdit2 );
    setTabOrder( LineEdit2, SpinBox1 );
    setTabOrder( SpinBox1, SpinBox2 );
    setTabOrder( SpinBox2, CheckButton1 );
    setTabOrder( CheckButton1, SpinBox3 );
    setTabOrder( SpinBox3, SpinBox4 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgRef_2Sel4Spin1Check_QTD::~DlgRef_2Sel4Spin1Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

