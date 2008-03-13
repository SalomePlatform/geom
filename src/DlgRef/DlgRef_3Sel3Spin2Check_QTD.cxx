/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_3Sel3Spin2Check_QTD.ui'
**
** Created: Wed Mar 12 11:57:09 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_3Sel3Spin2Check_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_3Sel3Spin2Check_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_3Sel3Spin2Check_QTD::DlgRef_3Sel3Spin2Check_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_3Sel3Spin2Check_QTD" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    DlgRef_3Sel3Spin2Check_QTDLayout = new QVBoxLayout( this, 0, 6, "DlgRef_3Sel3Spin2Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    layout1 = new QGridLayout( 0, 1, 1, 0, 6, "layout1"); 

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel2, 1, 0 );

    SpinBox_3 = new QSpinBox( GroupBox1, "SpinBox_3" );
    SpinBox_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_3->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( SpinBox_3, 5, 2 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel3, 2, 0 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    layout1->addWidget( LineEdit2, 1, 2 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton2, 1, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel6, 5, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton1, 0, 1 );

    CheckBox1 = new QCheckBox( GroupBox1, "CheckBox1" );

    layout1->addMultiCellWidget( CheckBox1, 6, 6, 0, 2 );

    SpinBox_2 = new QSpinBox( GroupBox1, "SpinBox_2" );
    SpinBox_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_2->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( SpinBox_2, 4, 2 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel5, 4, 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel1, 0, 0 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel4, 3, 0 );

    CheckBox2 = new QCheckBox( GroupBox1, "CheckBox2" );

    layout1->addMultiCellWidget( CheckBox2, 7, 7, 0, 2 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    layout1->addWidget( LineEdit1, 0, 2 );
    Spacer3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout1->addItem( Spacer3, 8, 2 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    layout1->addWidget( LineEdit3, 2, 2 );

    PushButton3 = new QPushButton( GroupBox1, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton3, 2, 1 );

    SpinBox_1 = new QSpinBox( GroupBox1, "SpinBox_1" );
    SpinBox_1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_1->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( SpinBox_1, 3, 2 );

    GroupBox1Layout->addLayout( layout1, 0, 0 );
    DlgRef_3Sel3Spin2Check_QTDLayout->addWidget( GroupBox1 );
    languageChange();
    resize( QSize(263, 254).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( PushButton1, LineEdit1 );
    setTabOrder( LineEdit1, PushButton2 );
    setTabOrder( PushButton2, LineEdit2 );
    setTabOrder( LineEdit2, PushButton3 );
    setTabOrder( PushButton3, LineEdit3 );
    setTabOrder( LineEdit3, SpinBox_1 );
    setTabOrder( SpinBox_1, SpinBox_2 );
    setTabOrder( SpinBox_2, SpinBox_3 );
    setTabOrder( SpinBox_3, CheckBox1 );
    setTabOrder( CheckBox1, CheckBox2 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_3Sel3Spin2Check_QTD::~DlgRef_3Sel3Spin2Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_3Sel3Spin2Check_QTD::languageChange()
{
    setCaption( tr( "DlgRef_3Sel3Spin2Check_QTD" ) );
    GroupBox1->setTitle( QString::null );
    TextLabel2->setText( tr( "TL2" ) );
    TextLabel3->setText( tr( "TL3" ) );
    PushButton2->setText( QString::null );
    TextLabel6->setText( tr( "TL6" ) );
    PushButton1->setText( QString::null );
    CheckBox1->setText( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel1->setText( tr( "TL1" ) );
    TextLabel4->setText( tr( "TL4" ) );
    CheckBox2->setText( QString::null );
    PushButton3->setText( QString::null );
}

