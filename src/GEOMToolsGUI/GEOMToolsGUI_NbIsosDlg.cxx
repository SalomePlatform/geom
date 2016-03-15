// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMToolsGUI_NbIsosDlg.cxx
// Author : OCC Team
//
#include "GEOMToolsGUI_NbIsosDlg.h"
#include <GeometryGUI.h>
#include <LightApp_Application.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Tools.h>

#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QKeyEvent>

//=================================================================================
// class    : GEOMToolsGUI_NbIsosDlg()
// purpose  : Constructs a GEOMToolsGUI_NbIsosDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
GEOMToolsGUI_NbIsosDlg::GEOMToolsGUI_NbIsosDlg(QWidget* parent )
  :QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  setObjectName( "GEOMToolsGUI_NbIsosDlg" );
  setModal( true );

  setWindowTitle( tr( "GEOM_MEN_ISOS" ) );
  setSizeGripEnabled(true);
  QGridLayout* MyDialogLayout = new QGridLayout(this); 
  MyDialogLayout->setSpacing(6);
  MyDialogLayout->setMargin(11);
  
  /***************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  GroupC1->setObjectName( "GroupC1" );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1 );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  
  QLabel* TextLabel1 = new QLabel( GroupC1 );
  TextLabel1->setObjectName( "TextLabel1" );
  TextLabel1->setText( tr( "GEOM_MEN_ISOU") );
  GroupC1Layout->addWidget( TextLabel1, 0, 0 );
  
  SpinBoxU = new QSpinBox( GroupC1 );
  SpinBoxU->setObjectName( "SpinBoxU" );
  SpinBoxU->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  SpinBoxU->setMinimum( 0 );
  SpinBoxU->setValue( 1 );
  GroupC1Layout->addWidget( SpinBoxU, 0, 1 );

  QLabel* TextLabel2 = new QLabel( GroupC1 );
  TextLabel2->setObjectName( "TextLabel2" );
  TextLabel2->setText( tr( "GEOM_MEN_ISOV") ) ;
  GroupC1Layout->addWidget( TextLabel2, 0, 2 );

  SpinBoxV = new QSpinBox( GroupC1 );
  SpinBoxV->setObjectName( "SpinBoxV");
  SpinBoxV->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  SpinBoxV->setValue( 1 );
  SpinBoxV->setMinimum( 0 );
  GroupC1Layout->addWidget( SpinBoxV, 0, 3 );
  
  /***************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  GroupButtons->setObjectName( "GroupButtons" );
  QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  
  QPushButton* buttonOk = new QPushButton( GroupButtons );
  buttonOk->setObjectName( "buttonOk" );
  buttonOk->setText( tr( "GEOM_BUT_OK" ) ) ;
  buttonOk->setAutoDefault( true );
  buttonOk->setDefault( true );
  GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
  
  GroupButtonsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  
  QPushButton* buttonCancel = new QPushButton( GroupButtons );
  buttonCancel->setObjectName( "buttonCancel" );
  buttonCancel->setText( tr( "GEOM_BUT_CANCEL" ) ) ;
  buttonCancel->setAutoDefault( true );
  GroupButtonsLayout->addWidget( buttonCancel, 0, 1 );

  QPushButton* buttonHelp = new QPushButton( GroupButtons );
  buttonHelp->setObjectName( "buttonHelp" );
  buttonHelp->setText( tr( "GEOM_BUT_HELP" ) ) ;
  buttonHelp->setAutoDefault( true );
  GroupButtonsLayout->addWidget( buttonHelp, 0, 2 );
  /***************************************************************/
  
  MyDialogLayout->addWidget(GroupC1, 0, 0);
  MyDialogLayout->addWidget(GroupButtons, 1, 0);

  myHelpFileName = "isolines_page.html";
  
  // signals and slots connections
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(ClickOnHelp()));
  
  /* Move widget on the botton right corner of main widget */
  SUIT_Tools::centerWidget(this, parent);
}

//=================================================================================
// function : ~GEOMToolsGUI_NbIsosDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GEOMToolsGUI_NbIsosDlg::~GEOMToolsGUI_NbIsosDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

int GEOMToolsGUI_NbIsosDlg::getU() const
{
  return SpinBoxU->text().toInt();
}

int GEOMToolsGUI_NbIsosDlg::getV() const
{
  return SpinBoxV->text().toInt();
}

void GEOMToolsGUI_NbIsosDlg::setU( const int v )
{
  SpinBoxU->setValue( v );
}

void GEOMToolsGUI_NbIsosDlg::setV( const int v )
{
  SpinBoxV->setValue( v );
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void GEOMToolsGUI_NbIsosDlg::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    app->onHelpContextModule(aGeomGUI ? app->moduleName(aGeomGUI->moduleName()) : QString(""), myHelpFileName);
  }
  else {
                QString platform;
#ifdef WIN32
                platform = "winapplication";
#else
                platform = "application";
#endif
    SUIT_MessageBox::warning(0, QObject::tr("WRN_WARNING"),
                             QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                             arg(app->resourceMgr()->stringValue("ExternalBrowser", platform)).arg(myHelpFileName),
                             QObject::tr("BUT_OK"));
  }
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void GEOMToolsGUI_NbIsosDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 )
    {
      e->accept();
      ClickOnHelp();
    }
}
