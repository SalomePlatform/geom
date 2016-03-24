// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "IGESPlugin_ExportDlg.h"

#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QComboBox>

//=================================================================================
// Constructor
//=================================================================================
IGESPlugin_ExportDlg::IGESPlugin_ExportDlg( QWidget* parent )
  : SUIT_FileDlg( parent, false, true, true )
{
  QLabel* versionLabel = new QLabel( tr( "IGES_VERSION" ), this );

  myVersionCB = new QComboBox( this );
  myVersionCB->addItem( tr( "5.1" ) );
  myVersionCB->addItem( tr( "5.3" ) );

  layout()->addWidget( versionLabel );
  layout()->addWidget( myVersionCB );
}

//=================================================================================
// Destructor
//=================================================================================
IGESPlugin_ExportDlg::~IGESPlugin_ExportDlg()
{
}

//=================================================================================
// getVersion
//=================================================================================
QString IGESPlugin_ExportDlg::getVersion() const
{
  return myVersionCB->currentText();
}

//=================================================================================
// getFileName
//=================================================================================
QString IGESPlugin_ExportDlg::getFileName( const QString& initial, const QString& filters,
					   const QString& caption, QWidget* parent, QString& version )
{
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );

  QString tmpfilename = initial;
  tmpfilename = tmpfilename.simplified();
  tmpfilename = tmpfilename.replace( QRegExp( "\\*" ), "" ).replace( QRegExp( "\\?" ), "" );

  IGESPlugin_ExportDlg fd( parent );
  fd.setFileMode( AnyFile );
  fd.setNameFilters( fls );
  fd.setWindowTitle( caption );
  if ( !tmpfilename.isEmpty() )
    fd.processPath( tmpfilename );

  QString filename;

  if ( fd.exec() == QDialog::Accepted ) {
    filename = fd.selectedFile();
    version = fd.getVersion();
  }

  QApplication::processEvents();

  return filename;
}
  
