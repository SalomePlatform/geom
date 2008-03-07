/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_2Sel3Spin2Rb_QTD.ui'
**
** Created: Tue Oct 9 15:16:54 2007
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_2Sel3Spin2Rb_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_2Sel3Spin2Rb_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_2Sel3Spin2Rb_QTD::DlgRef_2Sel3Spin2Rb_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_2Sel3Spin2Rb_QTD" );
    DlgRef_2Sel3Spin2Rb_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_2Sel3Spin2Rb_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addWidget( LineEdit1, 0, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel5, 1, 3 );

    RadioButton2 = new QRadioButton( GroupBox1, "RadioButton2" );

    Layout2->addWidget( RadioButton2, 1, 0 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel3, 0, 1 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout2->addMultiCellWidget( SpinBox1, 0, 0, 2, 4 );

    RadioButton1 = new QRadioButton( GroupBox1, "RadioButton1" );

    Layout2->addWidget( RadioButton1, 0, 0 );

    SpinBox3 = new QSpinBox( GroupBox1, "SpinBox3" );
    SpinBox3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox3->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox3, 1, 4 );

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox2, 1, 2 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel4, 1, 1 );

    Layout1->addMultiCellLayout( Layout2, 2, 2, 0, 2 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton2, 1, 1 );
    Spacer5 = new QSpacerItem( 0, 207, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( Spacer5, 3, 2 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout1->addWidget( LineEdit2, 1, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    GroupBox1Layout->addLayout( Layout1, 0, 0 );

    DlgRef_2Sel3Spin2Rb_QTDLayout->addWidget( GroupBox1, 0, 0 );
    resize( QSize(439, 340).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_2Sel3Spin2Rb_QTD::~DlgRef_2Sel3Spin2Rb_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

