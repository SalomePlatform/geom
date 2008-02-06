// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File   : GEOMToolsGUI_DeleteDlg.cxx
// Author : Dmitry Matveitchev, Open CASCADE S.A.S.
//

#include "GEOMToolsGUI_DeleteDlg.h"

#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QStringList>
#include <QGridLayout>

/*!
  \brief Constructor.
  \param parent parent widget
*/
GEOMToolsGUI_DeleteDlg::GEOMToolsGUI_DeleteDlg( QWidget* parent, const QStringList& objects )
: QDialog( parent )
{
  setModal( true );
  setObjectName( "GEOMToolsGUI_DeleteDlg" );

  setWindowTitle( tr( "GEOM_DELETE_OBJECTS" ) );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );

  topLayout->setSpacing( 6 );
  topLayout->setMargin( 11 );

  QLabel* lab = new QLabel( tr( "GEOM_REALLY_DELETE" ).arg( objects.count() ), this );

  QTextBrowser* viewer = new QTextBrowser( this );
  viewer->setText( QString( "- " ).arg( objects.join( "<br> -" ) ) );

  QPushButton* buttonYes = new QPushButton( tr( "GEOM_BUT_YES" ), this );
  QPushButton* buttonNo  = new QPushButton( tr( "GEOM_BUT_NO" ),  this );

  topLayout->addWidget( lab,       0, 0, 1, 3 );
  topLayout->addWidget( viewer,    1, 0, 1, 3 );
  topLayout->addWidget( buttonYes, 2, 0 );
  topLayout->addWidget( buttonNo,  2, 2 );

  /* signals and slots connections */
  connect( buttonYes, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonNo,  SIGNAL( clicked() ), this, SLOT( reject() ) );
}

GEOMToolsGUI_DeleteDlg::~GEOMToolsGUI_DeleteDlg()
{
}



