/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_1Sel5Spin1Check_QTD.ui'
**
** Created: Fri Feb 1 14:04:55 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_1Sel5Spin1Check_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_1Sel5Spin1Check_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_1Sel5Spin1Check_QTD::DlgRef_1Sel5Spin1Check_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_1Sel5Spin1Check_QTD" );
    DlgRef_1Sel5Spin1Check_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_1Sel5Spin1Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
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

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox2, 0, 3 );

    SpinBox3 = new QSpinBox( GroupBox1, "SpinBox3" );
    SpinBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox3->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox3, 2, 1 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel5, 1, 0 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel3, 0, 2 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel4, 2, 0 );

    SpinBox4 = new QSpinBox( GroupBox1, "SpinBox4" );
    SpinBox4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox4->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox4, 1, 1 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox1, 0, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel6, 1, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel2, 0, 0 );

    SpinBox5 = new QSpinBox( GroupBox1, "SpinBox5" );
    SpinBox5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox5->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox5, 1, 3 );

    CheckBox1 = new QCheckBox( GroupBox1, "CheckBox1" );

    Layout2->addMultiCellWidget( CheckBox1, 2, 2, 2, 3 );

    Layout1->addMultiCellLayout( Layout2, 1, 1, 0, 2 );
    Spacer6 = new QSpacerItem( 0, 120, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( Spacer6, 2, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    DlgRef_1Sel5Spin1Check_QTDLayout->addWidget( GroupBox1, 0, 0 );
    languageChange();
    resize( QSize(162, 143).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_1Sel5Spin1Check_QTD::~DlgRef_1Sel5Spin1Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_1Sel5Spin1Check_QTD::languageChange()
{
    setCaption( tr( "DlgRef_1Sel5Spin1Check_QTD" ) );
    GroupBox1->setTitle( QString::null );
    TextLabel1->setText( tr( "TL1" ) );
    PushButton1->setText( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel3->setText( tr( "TL3" ) );
    TextLabel4->setText( tr( "TL4" ) );
    TextLabel6->setText( tr( "TL6" ) );
    TextLabel2->setText( tr( "TL2" ) );
    CheckBox1->setText( QString::null );
}

