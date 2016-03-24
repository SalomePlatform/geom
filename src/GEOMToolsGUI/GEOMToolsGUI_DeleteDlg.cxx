// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : GEOMToolsGUI_DeleteDlg.cxx
// Author : Dmitry Matveitchev, Open CASCADE S.A.S.
//
#include "GEOMToolsGUI_DeleteDlg.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QStringList>
#include <QGridLayout>
#include <SUIT_MessageBox.h>
 
static bool isEntryLess( const QString& e1, const QString& e2 )
{
  QStringList el1 = e1.split(":");
  QStringList el2 = e2.split(":");
  int e1c = el1.count(), e2c = el2.count();
  for ( int i = 0; i < e1c && i < e2c; i++ ) {
    int id1 = el1[i].toInt();
    int id2 = el2[i].toInt();
    if ( id1 < id2 ) return true;
    else if ( id2 < id1 ) return false;
  }
  return el1.count() < el2.count();
}

static QStringList objectsToNames( const QMap<QString, QString>& objects )
{
  QStringList entries;
  for ( QMap<QString, QString>::ConstIterator it = objects.begin(); it != objects.end(); ++it ) {
    QString entry = it.key();
    QStringList::Iterator iter;
    bool added = false;
    for ( iter = entries.begin(); iter != entries.end() && !added; ++iter ) {
      if ( isEntryLess( entry, *iter ) ) {
        entries.insert( iter, entry );
        added = true;
      }
    }
    if ( !added ) 
      entries.append( entry );
  }
  QStringList names;
  for ( int i = 0; i < entries.count(); i++ ) {
    int level = entries[i].count(":")-3;
    names.append( QString( level*2, ' ' ) + objects[ entries[i] ] );
  }
  return names;
}

/*!
  \brief Constructor.
  \param parent parent widget
  \param objects map of objects names/objects
  \param deleteAll if True, delete all objects
*/
GEOMToolsGUI_DeleteDlg::GEOMToolsGUI_DeleteDlg( QWidget* parent, 
                                                const QMap<QString, QString>& objects, 
                                                bool deleteAll )
: QDialog( parent )
{
  setModal( true );
  setObjectName( "GEOMToolsGUI_DeleteDlg" );

  setWindowTitle( tr( "GEOM_DELETE_OBJECTS" ) );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );

  topLayout->setSpacing( 6 );
  topLayout->setMargin( 11 );

  QLabel* pix = new QLabel( this );
  pix->setPixmap( SUIT_MessageBox::standardIcon( QMessageBox::Question ) );
  pix->setScaledContents( false );
  pix->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  topLayout->addWidget( pix, 0, 0, 1, 1 );

  QLabel* lab = new QLabel( this );
  lab->setAlignment( Qt::AlignCenter );
  topLayout->addWidget( lab, 0, 1, 1, 1 );

  if ( !deleteAll ) {
    lab->setText( tr( "GEOM_REALLY_DELETE" ).arg( objects.count() ) );
    QTextBrowser* viewer = new QTextBrowser( this );
    viewer->setText( QString( " - %1" ).arg( objectsToNames( objects ).join( "\n - " ) ) );
    topLayout->addWidget( viewer,    1, 0, 1, 2 );
  }
  else {
    lab->setText( tr( "GEOM_REALLY_DELETE_ALL" ) );
  }

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Yes
                                 | QDialogButtonBox::No);
  int rc = topLayout->rowCount();
  topLayout->addWidget( buttonBox, rc, 1, 1, 1 );

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

GEOMToolsGUI_DeleteDlg::~GEOMToolsGUI_DeleteDlg()
{
}
