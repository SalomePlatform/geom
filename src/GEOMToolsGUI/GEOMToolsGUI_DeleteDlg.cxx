// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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
 
#include <iostream>

namespace
{
  struct NaturalCompare
  {
    bool operator () (const std::string& s1, const std::string& s2) const
    {
      // hope the entries are never empty and never equal
      int diff21 = 0;
      const char* p1 = s1.c_str();
      const char* p2 = s2.c_str();
      for ( ; ; ++p1, ++p2 )
      {
        if ( *p1 == *p2 )
        {
          if ( diff21 && !*p1 ) // both numbers ends
            return diff21 > 0;
          continue;
        }
        // different chars
        bool d1 = isdigit( *p1 );
        bool d2 = isdigit( *p2 );
        if ( d1 != d2 ) // one number is shorter then another
          return d2; // is s1 shorter?
        if ( !d1 && diff21 ) // both numbers ends
          return diff21 > 0;
        if ( !diff21 ) // remember the first difference
          diff21 = *p2 - *p1;
      }
      return diff21 > 0;
    }
  };
}

static QStringList objectsToNames( const QMap<QString, QString>& objects )
{
  typedef std::map< std::string, QString, NaturalCompare > TSortMap;
  TSortMap sortedByEntry;
  for ( QMap<QString, QString>::ConstIterator it = objects.begin(); it != objects.end(); ++it )
    sortedByEntry.insert( sortedByEntry.end(), std::make_pair( it.key().toStdString(), it.value() ));

  QStringList names;
  for ( TSortMap::iterator it = sortedByEntry.begin(); it != sortedByEntry.end(); ++it )
  {
    const std::string& entry = it->first;
    int level = std::count( entry.begin(), entry.end(), ':' ) - 3;
    names.append( QString( level * 2, ' ' ) + it->second );
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
