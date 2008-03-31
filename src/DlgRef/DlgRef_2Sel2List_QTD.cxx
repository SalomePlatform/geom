/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_2Sel2List_QTD.ui'
**
** Created: Thu Mar 27 16:38:25 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_2Sel2List_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_2Sel2List_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DlgRef_2Sel2List_QTD::DlgRef_2Sel2List_QTD( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DlgRef_2Sel2List_QTD" );
    DlgRef_2Sel2List_QTDLayout = new QVBoxLayout( this, 11, 6, "DlgRef_2Sel2List_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    LineEdit2 = new QLineEdit( GroupBox1, "LineEdit2" );

    GroupBox1Layout->addWidget( LineEdit2, 1, 2 );

    ComboBox2 = new QComboBox( FALSE, GroupBox1, "ComboBox2" );

    GroupBox1Layout->addMultiCellWidget( ComboBox2, 3, 3, 1, 2 );

    TextLabel4 = new QLabel( GroupBox1, "TextLabel4" );
    TextLabel4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel4->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel4, 3, 0 );

    ComboBox1 = new QComboBox( FALSE, GroupBox1, "ComboBox1" );

    GroupBox1Layout->addMultiCellWidget( ComboBox1, 2, 2, 1, 2 );

    TextLabel3 = new QLabel( GroupBox1, "TextLabel3" );
    TextLabel3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel3->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel3, 2, 0 );

    LineEdit1 = new QLineEdit( GroupBox1, "LineEdit1" );

    GroupBox1Layout->addWidget( LineEdit1, 0, 2 );

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel2, 1, 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( TextLabel1, 0, 0 );

    PushButton1 = new QPushButton( GroupBox1, "PushButton1" );
    PushButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton1->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( PushButton1, 0, 1 );

    PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
    PushButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PushButton2->sizePolicy().hasHeightForWidth() ) );

    GroupBox1Layout->addWidget( PushButton2, 1, 1 );
    DlgRef_2Sel2List_QTDLayout->addWidget( GroupBox1 );
    languageChange();
    resize( QSize(187, 182).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_2Sel2List_QTD::~DlgRef_2Sel2List_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_2Sel2List_QTD::languageChange()
{
    setCaption( tr( "DlgRef_2Sel2List_QTD" ) );
    GroupBox1->setTitle( QString::null );
    TextLabel4->setText( tr( "TL4" ) );
    TextLabel3->setText( tr( "TL3" ) );
    TextLabel2->setText( tr( "TL2" ) );
    TextLabel1->setText( tr( "TL1" ) );
    PushButton1->setText( QString::null );
    PushButton2->setText( QString::null );
}

