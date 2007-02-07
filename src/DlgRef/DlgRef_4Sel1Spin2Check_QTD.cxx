/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_4Sel1Spin2Check_QTD.ui'
**
** Created: Wed Oct 25 20:32:42 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_4Sel1Spin2Check_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_4Sel1Spin2Check_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_4Sel1Spin2Check_QTD::DlgRef_4Sel1Spin2Check_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_4Sel1Spin2Check_QTD" );
    DlgRef_4Sel1Spin2Check_QTDLayout = new QVBoxLayout( this, 0, 6, "DlgRef_4Sel1Spin2Check_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QVBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 6, 6, "Layout1"); 

    CheckButton2 = new QCheckBox( GroupBox1, "CheckButton2" );

    Layout1->addWidget( CheckButton2, 4, 3 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton2, 1, 1 );

    LineEdit5 = new QLineEdit( GroupBox1, "LineEdit5" );

    Layout1->addMultiCellWidget( LineEdit5, 3, 3, 2, 3 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel5, 3, 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    Layout1->addMultiCellWidget( LineEdit1, 0, 0, 2, 3 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox1, 4, 2 );

    CheckButton1 = new QCheckBox( GroupBox1, "CheckButton1" );

    Layout1->addMultiCellWidget( CheckButton1, 5, 5, 0, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    PushButton5 = new QPushButton( GroupBox1, "PushButton5" );
    PushButton5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton5->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton5, 3, 1 );

    PushButton4 = new QPushButton( GroupBox1, "PushButton4" );
    PushButton4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton4->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton4, 2, 1 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel4, 2, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( PushButton1, 0, 1 );

    LineEdit4 = new QLineEdit( GroupBox1, "LineEdit4" );

    Layout1->addMultiCellWidget( LineEdit4, 2, 2, 2, 3 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    Layout1->addMultiCellWidget( LineEdit2, 1, 1, 2, 3 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( TextLabel3, 4, 0 );
    GroupBox1Layout->addLayout( Layout1 );
    DlgRef_4Sel1Spin2Check_QTDLayout->addWidget( GroupBox1 );
    languageChange();
    resize( QSize(261, 198).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( PushButton1, LineEdit1 );
    setTabOrder( LineEdit1, PushButton2 );
    setTabOrder( PushButton2, LineEdit2 );
    setTabOrder( LineEdit2, SpinBox1 );
    setTabOrder( SpinBox1, CheckButton2 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_4Sel1Spin2Check_QTD::~DlgRef_4Sel1Spin2Check_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_4Sel1Spin2Check_QTD::languageChange()
{
    setCaption( tr( "DlgRef_4Sel1Spin2Check_QTD" ) );
    GroupBox1->setTitle( QString::null );
    CheckButton2->setText( QString::null );
    PushButton2->setText( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel1->setText( tr( "TL1" ) );
    CheckButton1->setText( QString::null );
    TextLabel2->setText( tr( "TL2" ) );
    PushButton5->setText( QString::null );
    PushButton4->setText( QString::null );
    TextLabel4->setText( tr( "TL4" ) );
    PushButton1->setText( QString::null );
    TextLabel3->setText( tr( "TL3" ) );
}

