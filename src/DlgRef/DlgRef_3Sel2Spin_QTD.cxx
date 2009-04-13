/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_3Sel2Spin_QTD.ui'
**
** Created: Mon Apr 13 11:59:12 2009
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_3Sel2Spin_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_3Sel2Spin_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_3Sel2Spin_QTD::DlgRef_3Sel2Spin_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_3Sel2Spin_QTD" );
    DlgRef_3Sel2Spin_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "DlgRef_3Sel2Spin_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    GroupBox1Layout->addWidget( LineEdit1, 0, 2 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel1, 0, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( PushButton1, 0, 1 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel2, 1, 0 );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    GroupBox1Layout->addWidget( LineEdit2, 1, 2 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( PushButton2, 1, 1 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel3, 2, 0 );

    PushButton3 = new QPushButton( GroupBox1, "PushButton3" );
    PushButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton3->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( PushButton3, 2, 1 );

    LineEdit3 = new QLineEdit( GroupBox1, "LineEdit3" );

    GroupBox1Layout->addWidget( LineEdit3, 2, 2 );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox2, 1, 1 );

    TextLabel5 = new QLabel( GroupBox1, "TextLabel5" );
    TextLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel5->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel5, 1, 0 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( TextLabel4, 0, 0 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout2->addWidget( SpinBox1, 0, 1 );

    GroupBox1Layout->addMultiCellLayout( Layout2, 3, 3, 0, 2 );
    Spacer5 = new QSpacerItem( 16, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GroupBox1Layout->addItem( Spacer5, 4, 1 );

    DlgRef_3Sel2Spin_QTDLayout->addWidget( GroupBox1, 0, 0 );
    languageChange();
    resize( QSize(124, 201).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // tab order
    setTabOrder( PushButton1, LineEdit1 );
    setTabOrder( LineEdit1, PushButton2 );
    setTabOrder( PushButton2, LineEdit2 );
    setTabOrder( LineEdit2, PushButton3 );
    setTabOrder( PushButton3, LineEdit3 );
    setTabOrder( LineEdit3, SpinBox1 );
    setTabOrder( SpinBox1, SpinBox2 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_3Sel2Spin_QTD::~DlgRef_3Sel2Spin_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_3Sel2Spin_QTD::languageChange()
{
    setCaption( tr( "DlgRef_2Sel2Spin_QTD" ) );
    GroupBox1->setTitle( QString::null );
    TextLabel1->setText( tr( "TL1" ) );
    PushButton1->setText( QString::null );
    TextLabel2->setText( tr( "TL2" ) );
    PushButton2->setText( QString::null );
    TextLabel3->setText( tr( "TL3" ) );
    PushButton3->setText( QString::null );
    TextLabel5->setText( tr( "TL5" ) );
    TextLabel4->setText( tr( "TL4" ) );
}

