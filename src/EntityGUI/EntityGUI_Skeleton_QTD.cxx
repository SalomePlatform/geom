// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_Skeleton_QTD.ui'
**
** Created: Fri Mar 17 15:47:14 2006
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "EntityGUI_Skeleton_QTD.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a EntityGUI_Skeleton_QTD as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
EntityGUI_Skeleton_QTD::EntityGUI_Skeleton_QTD( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "EntityGUI_Skeleton_QTD" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    EntityGUI_Skeleton_QTDLayout = new QGridLayout( this, 1, 1, 11, 6, "EntityGUI_Skeleton_QTDLayout"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    GroupVal = new QGroupBox( this, "GroupVal" );
    GroupVal->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupVal->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( GroupVal, 2, 0 );

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, GroupConstructors->sizePolicy().hasHeightForWidth() ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 6 );
    GroupConstructors->layout()->setMargin( 11 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    RadioButton1 = new QRadioButton( GroupConstructors, "RadioButton1" );
    Layout2->addWidget( RadioButton1 );

    RadioButton2 = new QRadioButton( GroupConstructors, "RadioButton2" );
    Layout2->addWidget( RadioButton2 );

    GroupConstructorsLayout->addLayout( Layout2, 0, 0 );

    Layout1->addWidget( GroupConstructors, 0, 0 );

    GroupDest = new QGroupBox( this, "GroupDest" );
    GroupDest->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupDest->sizePolicy().hasHeightForWidth() ) );
    GroupDest->setColumnLayout(0, Qt::Vertical );
    GroupDest->layout()->setSpacing( 6 );
    GroupDest->layout()->setMargin( 11 );
    GroupDestLayout = new QGridLayout( GroupDest->layout() );
    GroupDestLayout->setAlignment( Qt::AlignTop );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    GroupDest1 = new QButtonGroup( GroupDest, "GroupDest1" );
    GroupDest1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupDest1->sizePolicy().hasHeightForWidth() ) );
    GroupDest1->setColumnLayout(0, Qt::Vertical );
    GroupDest1->layout()->setSpacing( 6 );
    GroupDest1->layout()->setMargin( 11 );
    GroupDest1Layout = new QGridLayout( GroupDest1->layout() );
    GroupDest1Layout->setAlignment( Qt::AlignTop );

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    RB_Dest2 = new QRadioButton( GroupDest1, "RB_Dest2" );

    Layout4->addWidget( RB_Dest2, 0, 1 );

    RB_Dest1 = new QRadioButton( GroupDest1, "RB_Dest1" );

    Layout4->addWidget( RB_Dest1, 0, 0 );

    GroupDest1Layout->addLayout( Layout4, 0, 0 );

    Layout5->addMultiCellWidget( GroupDest1, 0, 0, 0, 1 );

    GroupDest2 = new QButtonGroup( GroupDest, "GroupDest2" );
    GroupDest2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupDest2->sizePolicy().hasHeightForWidth() ) );
    GroupDest2->setColumnLayout(0, Qt::Vertical );
    GroupDest2->layout()->setSpacing( 6 );
    GroupDest2->layout()->setMargin( 11 );
    GroupDest2Layout = new QGridLayout( GroupDest2->layout() );
    GroupDest2Layout->setAlignment( Qt::AlignTop );

    Layout5->addWidget( GroupDest2, 1, 0 );

    GroupDest3 = new QButtonGroup( GroupDest, "GroupDest3" );
    GroupDest3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupDest3->sizePolicy().hasHeightForWidth() ) );
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
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 6 );
    GroupButtons->layout()->setMargin( 11 );
    GroupButtonsLayout = new QHBoxLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 

    buttonEnd = new QPushButton( GroupButtons, "buttonEnd" );
    Layout3->addWidget( buttonEnd );

    buttonClose = new QPushButton( GroupButtons, "buttonClose" );
    Layout3->addWidget( buttonClose );
    Spacer1 = new QSpacerItem( 91, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( Spacer1 );

    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    Layout3->addWidget( buttonCancel );

    buttonHelp = new QPushButton( GroupButtons, "buttonHelp" );
    Layout3->addWidget( buttonHelp );
    GroupButtonsLayout->addLayout( Layout3 );

    Layout1->addWidget( GroupButtons, 3, 0 );

    EntityGUI_Skeleton_QTDLayout->addLayout( Layout1, 0, 0 );
    languageChange();
    resize( QSize(317, 276).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

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

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EntityGUI_Skeleton_QTD::languageChange()
{
    setCaption( tr( "EntityGUI_Skeleton_QTD" ) );
    GroupVal->setTitle( QString::null );
    GroupConstructors->setTitle( tr( "Element Type" ) );
    RadioButton1->setText( tr( "Segment" ) );
    RadioButton2->setText( tr( "Arc" ) );
    GroupDest->setTitle( tr( "Destination" ) );
    GroupDest1->setTitle( tr( "Type" ) );
    RB_Dest2->setText( tr( "Direction" ) );
    RB_Dest1->setText( tr( "Point" ) );
    GroupDest2->setTitle( QString::null );
    GroupDest3->setTitle( QString::null );
    GroupButtons->setTitle( QString::null );
    buttonEnd->setText( tr( "End Sketch" ) );
    buttonClose->setText( tr( "Close Sketch" ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonHelp->setText( tr( "&Help" ) );
}

