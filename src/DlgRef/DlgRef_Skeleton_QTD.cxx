/****************************************************************************
** Form implementation generated from reading ui file 'DlgRef_Skeleton_QTD.ui'
**
** Created: Wed May 7 11:21:32 2008
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgRef_Skeleton_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a DlgRef_Skeleton_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgRef_Skeleton_QTD::DlgRef_Skeleton_QTD( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "DlgRef_Skeleton_QTD" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    DlgRef_Skeleton_QTDLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgRef_Skeleton_QTDLayout"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, GroupConstructors->sizePolicy().hasHeightForWidth() ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 6 );
    GroupConstructors->layout()->setMargin( 11 );
    GroupConstructorsLayout = new QHBoxLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    RadioButton1 = new QRadioButton( GroupConstructors, "RadioButton1" );
    Layout2->addWidget( RadioButton1 );

    RadioButton2 = new QRadioButton( GroupConstructors, "RadioButton2" );
    Layout2->addWidget( RadioButton2 );

    RadioButton3 = new QRadioButton( GroupConstructors, "RadioButton3" );
    Layout2->addWidget( RadioButton3 );

    RadioButton4 = new QRadioButton( GroupConstructors, "RadioButton4" );
    Layout2->addWidget( RadioButton4 );

    RadioButton5 = new QRadioButton( GroupConstructors, "RadioButton5" );
    Layout2->addWidget( RadioButton5 );
    GroupConstructorsLayout->addLayout( Layout2 );

    Layout1->addWidget( GroupConstructors, 0, 0 );

    GroupBoxName = new QGroupBox( this, "GroupBoxName" );
    GroupBoxName->setColumnLayout(0, Qt::Vertical );
    GroupBoxName->layout()->setSpacing( 6 );
    GroupBoxName->layout()->setMargin( 11 );
    GroupBoxNameLayout = new QGridLayout( GroupBoxName->layout() );
    GroupBoxNameLayout->setAlignment( Qt::AlignTop );

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    NameLabel = new QLabel( GroupBoxName, "NameLabel" );

    Layout4->addWidget( NameLabel, 0, 0 );

    ResultName = new QLineEdit( GroupBoxName, "ResultName" );

    Layout4->addWidget( ResultName, 0, 1 );

    GroupBoxNameLayout->addLayout( Layout4, 0, 0 );

    Layout1->addWidget( GroupBoxName, 1, 0 );

    GroupBoxPublish = new QGroupBox( this, "GroupBoxPublish" );
    GroupBoxPublish->setColumnLayout(0, Qt::Vertical );
    GroupBoxPublish->layout()->setSpacing( 6 );
    GroupBoxPublish->layout()->setMargin( 11 );
    GroupBoxPublishLayout = new QGridLayout( GroupBoxPublish->layout() );
    GroupBoxPublishLayout->setAlignment( Qt::AlignTop );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    CheckBoxRestoreSS = new QCheckBox( GroupBoxPublish, "CheckBoxRestoreSS" );

    Layout5->addWidget( CheckBoxRestoreSS, 0, 0 );

    GroupBoxPublishLayout->addLayout( Layout5, 0, 0 );

    Layout1->addWidget( GroupBoxPublish, 3, 0 );

    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupButtons->sizePolicy().hasHeightForWidth() ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 6 );
    GroupButtons->layout()->setMargin( 11 );
    GroupButtonsLayout = new QVBoxLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 

    buttonOk = new QPushButton( GroupButtons, "buttonOk" );

    Layout3->addWidget( buttonOk, 0, 0 );

    buttonApply = new QPushButton( GroupButtons, "buttonApply" );

    Layout3->addWidget( buttonApply, 0, 1 );

    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );

    Layout3->addWidget( buttonCancel, 0, 3 );
    Spacer1 = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( Spacer1, 0, 2 );

    buttonHelp = new QPushButton( GroupButtons, "buttonHelp" );

    Layout3->addWidget( buttonHelp, 0, 4 );
    GroupButtonsLayout->addLayout( Layout3 );

    Layout1->addWidget( GroupButtons, 4, 0 );

    DlgRef_Skeleton_QTDLayout->addLayout( Layout1, 0, 0 );
    languageChange();
    resize( QSize(615, 682).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgRef_Skeleton_QTD::~DlgRef_Skeleton_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgRef_Skeleton_QTD::languageChange()
{
}

