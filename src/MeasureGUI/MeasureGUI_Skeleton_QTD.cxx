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
** Form implementation generated from reading ui file 'MeasureGUI_Skeleton_QTD.ui'
**
** Created: lun oct 27 11:23:09 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "MeasureGUI_Skeleton_QTD.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a MeasureGUI_Skeleton_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MeasureGUI_Skeleton_QTD::MeasureGUI_Skeleton_QTD( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MeasureGUI_Skeleton_QTD" );
    resize( 228, 266 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setCaption( trUtf8( "MeasureGUI_Skeleton_QTD" ) );
    setSizeGripEnabled( TRUE );
    MeasureGUI_Skeleton_QTDLayout = new QGridLayout( this, 1, 1, 11, 6, "MeasureGUI_Skeleton_QTDLayout"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, GroupButtons->sizePolicy().hasHeightForWidth() ) );
    GroupButtons->setTitle( trUtf8( "" ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 6 );
    GroupButtons->layout()->setMargin( 11 );
    GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 
    QSpacerItem* spacer = new QSpacerItem( 120, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 120, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout3->addItem( spacer_2, 0, 0 );

    buttonClose = new QPushButton( GroupButtons, "buttonClose" );
    buttonClose->setText( trUtf8( "&Close" ) );

    Layout3->addWidget( buttonClose, 0, 1 );

    GroupButtonsLayout->addLayout( Layout3, 0, 0 );

    Layout1->addWidget( GroupButtons, 2, 0 );

    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, GroupConstructors->sizePolicy().hasHeightForWidth() ) );
    GroupConstructors->setTitle( trUtf8( "" ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 6 );
    GroupConstructors->layout()->setMargin( 11 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    RadioButton1 = new QRadioButton( GroupConstructors, "RadioButton1" );
    RadioButton1->setText( trUtf8( "" ) );
    Layout2->addWidget( RadioButton1 );

    GroupConstructorsLayout->addLayout( Layout2, 0, 0 );

    Layout1->addWidget( GroupConstructors, 0, 0 );

    GroupMedium = new QGroupBox( this, "GroupMedium" );
    GroupMedium->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, GroupMedium->sizePolicy().hasHeightForWidth() ) );
    GroupMedium->setTitle( trUtf8( "" ) );

    Layout1->addWidget( GroupMedium, 1, 0 );

    MeasureGUI_Skeleton_QTDLayout->addLayout( Layout1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
MeasureGUI_Skeleton_QTD::~MeasureGUI_Skeleton_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}

