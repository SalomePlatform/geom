/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_Skeleton_QTD.ui'
**
** Created: ven déc 12 11:17:08 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_Skeleton_QTD.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_Skeleton_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EntityGUI_Skeleton_QTD::EntityGUI_Skeleton_QTD( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "EntityGUI_Skeleton_QTD" );
    resize( 317, 276 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "EntityGUI_Skeleton_QTD" ) );
    EntityGUI_Skeleton_QTDLayout = new QGridLayout( this, 1, 1, 11, 6, "EntityGUI_Skeleton_QTDLayout"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    GroupVal = new QGroupBox( this, "GroupVal" );
    GroupVal->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupVal->sizePolicy().hasHeightForWidth() ) );
    GroupVal->setTitle( trUtf8( "" ) );

    Layout1->addWidget( GroupVal, 2, 0 );

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, GroupConstructors->sizePolicy().hasHeightForWidth() ) );
    GroupConstructors->setTitle( trUtf8( "Element Type" ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 6 );
    GroupConstructors->layout()->setMargin( 11 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    RadioButton1 = new QRadioButton( GroupConstructors, "RadioButton1" );
    RadioButton1->setText( trUtf8( "Segment" ) );
    Layout2->addWidget( RadioButton1 );

    RadioButton2 = new QRadioButton( GroupConstructors, "RadioButton2" );
    RadioButton2->setText( trUtf8( "Arc" ) );
    Layout2->addWidget( RadioButton2 );

    GroupConstructorsLayout->addLayout( Layout2, 0, 0 );

    Layout1->addWidget( GroupConstructors, 0, 0 );

    GroupDest = new QGroupBox( this, "GroupDest" );
    GroupDest->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupDest->sizePolicy().hasHeightForWidth() ) );
    GroupDest->setTitle( trUtf8( "Destination" ) );
    GroupDest->setColumnLayout(0, Qt::Vertical );
    GroupDest->layout()->setSpacing( 6 );
    GroupDest->layout()->setMargin( 11 );
    GroupDestLayout = new QGridLayout( GroupDest->layout() );
    GroupDestLayout->setAlignment( Qt::AlignTop );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    GroupDest1 = new QButtonGroup( GroupDest, "GroupDest1" );
    GroupDest1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupDest1->sizePolicy().hasHeightForWidth() ) );
    GroupDest1->setTitle( trUtf8( "Type" ) );
    GroupDest1->setColumnLayout(0, Qt::Vertical );
    GroupDest1->layout()->setSpacing( 6 );
    GroupDest1->layout()->setMargin( 11 );
    GroupDest1Layout = new QGridLayout( GroupDest1->layout() );
    GroupDest1Layout->setAlignment( Qt::AlignTop );

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    RB_Dest2 = new QRadioButton( GroupDest1, "RB_Dest2" );
    RB_Dest2->setText( trUtf8( "Direction" ) );

    Layout4->addWidget( RB_Dest2, 0, 1 );

    RB_Dest1 = new QRadioButton( GroupDest1, "RB_Dest1" );
    RB_Dest1->setText( trUtf8( "Point" ) );

    Layout4->addWidget( RB_Dest1, 0, 0 );

    GroupDest1Layout->addLayout( Layout4, 0, 0 );

    Layout5->addMultiCellWidget( GroupDest1, 0, 0, 0, 1 );

    GroupDest2 = new QButtonGroup( GroupDest, "GroupDest2" );
    GroupDest2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupDest2->sizePolicy().hasHeightForWidth() ) );
    GroupDest2->setTitle( trUtf8( "" ) );
    GroupDest2->setColumnLayout(0, Qt::Vertical );
    GroupDest2->layout()->setSpacing( 6 );
    GroupDest2->layout()->setMargin( 11 );
    GroupDest2Layout = new QGridLayout( GroupDest2->layout() );
    GroupDest2Layout->setAlignment( Qt::AlignTop );

    Layout5->addWidget( GroupDest2, 1, 0 );

    GroupDest3 = new QButtonGroup( GroupDest, "GroupDest3" );
    GroupDest3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupDest3->sizePolicy().hasHeightForWidth() ) );
    GroupDest3->setTitle( trUtf8( "" ) );
    GroupDest3->setColumnLayout(0, Qt::Vertical );
    GroupDest3->layout()->setSpacing( 6 );
    GroupDest3->layout()->setMargin( 11 );
    GroupDest3Layout = new QGridLayout( GroupDest3->layout() );
    GroupDest3Layout->setAlignment( Qt::AlignTop );

    Layout5->addWidget( GroupDest3, 1, 1 );

    GroupDestLayout->addLayout( Layout5, 0, 0 );

    Layout1->addWidget( GroupDest, 1, 0 );

    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupButtons->sizePolicy().hasHeightForWidth() ) );
    GroupButtons->setTitle( trUtf8( "" ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 6 );
    GroupButtons->layout()->setMargin( 11 );
    GroupButtonsLayout = new QHBoxLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 

    buttonEnd = new QPushButton( GroupButtons, "buttonEnd" );
    buttonEnd->setText( trUtf8( "End Sketch" ) );
    Layout3->addWidget( buttonEnd );

    buttonClose = new QPushButton( GroupButtons, "buttonClose" );
    buttonClose->setText( trUtf8( "Close Sketch" ) );
    Layout3->addWidget( buttonClose );
    QSpacerItem* spacer = new QSpacerItem( 91, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer );

    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( trUtf8( "&Cancel" ) );
    Layout3->addWidget( buttonCancel );
    GroupButtonsLayout->addLayout( Layout3 );

    Layout1->addWidget( GroupButtons, 3, 0 );

    EntityGUI_Skeleton_QTDLayout->addLayout( Layout1, 0, 0 );

    // tab order
    setTabOrder( RadioButton1, RadioButton2 );
    setTabOrder( RadioButton2, RB_Dest1 );
    setTabOrder( RB_Dest1, RB_Dest2 );
    setTabOrder( RB_Dest2, buttonEnd );
    setTabOrder( buttonEnd, buttonClose );
    setTabOrder( buttonClose, buttonCancel );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_Skeleton_QTD::~EntityGUI_Skeleton_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

