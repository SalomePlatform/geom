/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_Point_QTD.ui'
**
** Created: ven déc 12 11:17:08 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_Point_QTD.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_Point_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_Point_QTD::EntityGUI_Point_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_Point_QTD" );
    resize( 124, 106 ); 
    setCaption( trUtf8( "EntityGUI_Point_QTD" ) );
    EntityGUI_Point_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_Point_QTDLayout"); 

    GroupPoint = new QButtonGroup( this, "GroupPoint" );
    GroupPoint->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupPoint->sizePolicy().hasHeightForWidth() ) );
    GroupPoint->setTitle( trUtf8( "Point" ) );
    GroupPoint->setColumnLayout(0, Qt::Vertical );
    GroupPoint->layout()->setSpacing( 6 );
    GroupPoint->layout()->setMargin( 11 );
    GroupPointLayout = new QGridLayout( GroupPoint->layout() );
    GroupPointLayout->setAlignment( Qt::AlignTop );

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    RB_Point2 = new QRadioButton( GroupPoint, "RB_Point2" );
    RB_Point2->setText( trUtf8( "Relative" ) );

    Layout1->addWidget( RB_Point2, 1, 0 );

    RB_Point1 = new QRadioButton( GroupPoint, "RB_Point1" );
    RB_Point1->setText( trUtf8( "Absolute" ) );

    Layout1->addWidget( RB_Point1, 0, 0 );

    RB_Point3 = new QRadioButton( GroupPoint, "RB_Point3" );
    RB_Point3->setText( trUtf8( "Selection" ) );

    Layout1->addWidget( RB_Point3, 2, 0 );

    GroupPointLayout->addLayout( Layout1, 0, 0 );

    EntityGUI_Point_QTDLayout->addWidget( GroupPoint, 0, 0 );

    // tab order
    setTabOrder( RB_Point1, RB_Point2 );
    setTabOrder( RB_Point2, RB_Point3 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_Point_QTD::~EntityGUI_Point_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

