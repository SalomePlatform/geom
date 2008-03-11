/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_3Sel3Spin2Check_QTD.ui'
**
** Created: Tue Feb 12 16:21:38 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_3Sel3Spin2Check_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlineedit.h>
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
    GroupBox1Layout = new QHBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    layout1 = new QGridLayout( 0, 1, 1, 0, 6, "layout1"); 
    Spacer3 = new QSpacerItem( 20, 130, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout1->addItem( Spacer3, 8, 3 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton1, 0, 1 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel3, 2, 0 );

    CheckBox1 = new QCheckBox( GroupBox1, "CheckBox1" );

    layout1->addMultiCellWidget( CheckBox1, 6, 6, 0, 1 );

    TextLabel6 = new QLabel( GroupBox1, "TextLabel6" );
    TextLabel6->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel6->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel6, 5, 0 );

    SpinBox_2 = new QSpinBox( GroupBox1, "SpinBox_2" );
    SpinBox_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_2->sizePolicy().hasHeightForWidth() ) );

    layout1->addMultiCellWidget( SpinBox_2, 4, 4, 2, 3 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    layout1->addMultiCellWidget( LineEdit2, 1, 1, 2, 3 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    layout1->addMultiCellWidget( LineEdit1, 0, 0, 2, 3 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    layout1->addMultiCellWidget( LineEdit3, 2, 2, 2, 3 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton2, 1, 1 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel2, 1, 0 );

    CheckBox2 = new QCheckBox( GroupBox1, "CheckBox2" );

    layout1->addMultiCellWidget( CheckBox2, 7, 7, 0, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel1, 0, 0 );

    PushButton3 = new QPushButton( GroupBox1, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( PushButton3, 2, 1 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel5, 4, 0 );

    SpinBox_1 = new QSpinBox( GroupBox1, "SpinBox_1" );
    SpinBox_1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_1->sizePolicy().hasHeightForWidth() ) );

    layout1->addMultiCellWidget( SpinBox_1, 3, 3, 2, 3 );

    SpinBox_3 = new QSpinBox( GroupBox1, "SpinBox_3" );
    SpinBox_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox_3->sizePolicy().hasHeightForWidth() ) );

    layout1->addMultiCellWidget( SpinBox_3, 5, 5, 2, 3 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    layout1->addWidget( TextLabel4, 3, 0 );
    GroupBox1Layout->addLayout( layout1 );
    DlgRef_3Sel3Spin2Check_QTDLayout->addWidget( GroupBox1 );
    languageChange();
    resize( QSize(338, 274).expandedTo(minimumSizeHint()) );
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
    PushButton1->setText( QString::null );
    TextLabel3->setText( tr( "TL3" ) );
    CheckBox1->setText( QString::null );
    TextLabel6->setText( tr( "TL6" ) );
    PushButton2->setText( QString::null );
    TextLabel2->setText( tr( "TL2" ) );
    CheckBox2->setText( QString::null );
    TextLabel1->setText( tr( "TL1" ) );
    PushButton3->setText( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel4->setText( tr( "TL4" ) );
}

