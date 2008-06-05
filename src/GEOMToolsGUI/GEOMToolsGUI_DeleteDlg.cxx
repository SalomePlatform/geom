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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <SUIT_MessageBox.h>
 
static bool isEntryLess( const QString& e1, const QString& e2 )
{
  QStringList el1 = QStringList::split(":", e1);
  QStringList el2 = QStringList::split(":", e2);
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
    QStringList::Iterator it;
    bool added = false;
    for ( it = entries.begin(); it != entries.end() && !added; ++it ) {
      if ( isEntryLess( entry, *it ) ) {
	entries.insert( it, entry );
	added = true;
      }
    }
    if ( !added ) 
      entries.append( entry );
  }
  QStringList names;
  for ( int i = 0; i < entries.count(); i++ ) {
    int level = entries[i].contains(":")-3;
    QString prefix; prefix.fill( ' ', level*2 );
    names.append( prefix + objects[ entries[i] ] );
  }
  return names;
}

/*!
  \brief Constructor.
  \param parent parent widget
*/
GEOMToolsGUI_DeleteDlg::GEOMToolsGUI_DeleteDlg( QWidget* parent, 
						const QMap<QString, QString>& objects, 
						bool deleteAll )
: QDialog( parent, "GEOMToolsGUI_DeleteDlg", true )
{
  setCaption( tr( "GEOM_DELETE_OBJECTS" ) );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );

  topLayout->setSpacing( 6 );
  topLayout->setMargin( 11 );

  QLabel* pix = new QLabel( this );
  pix->setPixmap( QMessageBox::standardIcon( QMessageBox::Question ) );
  pix->setScaledContents( false );
  pix->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  topLayout->addWidget( pix, 0, 0 );

  QLabel* lab = new QLabel( this );
  lab->setAlignment( Qt::AlignCenter );
  topLayout->addWidget( lab, 0, 1 );

  if ( !deleteAll ) {
    lab->setText( tr( "GEOM_REALLY_DELETE" ).arg( objects.count() ) );
    QTextBrowser* viewer = new QTextBrowser( this );
    viewer->setText( QString( " - %1" ).arg( objectsToNames( objects ).join( "\n - " ) ) );
    topLayout->addMultiCellWidget( viewer,    1, 1, 0, 1 );
  }
  else {
    lab->setText( tr( "GEOM_REALLY_DELETE_ALL" ) );
  }

  QPushButton* buttonYes = new QPushButton( tr( "GEOM_BUT_YES" ), this );
  QPushButton* buttonNo  = new QPushButton( tr( "GEOM_BUT_NO" ),  this );
  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->setMargin( 0 );
  btnLayout->setSpacing( 6 );
  btnLayout->addWidget( buttonYes );
  btnLayout->addSpacing( 10 );
  btnLayout->addStretch();
  btnLayout->addWidget( buttonNo );
  int rc = topLayout->numRows();
  topLayout->addMultiCellLayout( btnLayout, rc, rc, 0, 1 );

  /* signals and slots connections */
  connect( buttonYes, SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( buttonNo,  SIGNAL( clicked() ), this, SLOT( reject() ) );
}

GEOMToolsGUI_DeleteDlg::~GEOMToolsGUI_DeleteDlg()
{
}
