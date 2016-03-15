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

// internal includes
#include "VTKPlugin_ExportDlg.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(VTKPlugin)

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Study.h>

// GEOM includes
#include "GEOM_Constants.h"

// QT includes
#include <QApplication>
#include <QLabel>
#include <QLayout>

//=================================================================================
// Constructor
//=================================================================================
VTKPlugin_ExportDlg::VTKPlugin_ExportDlg( const Handle(SALOME_InteractiveObject)& io, QWidget* parent )
: SUIT_FileDlg( parent, false, true, true )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QLabel* deflectionLabel = new QLabel( tr( "DEFLECTION" ), this );

  myDeflectionSB = new SalomeApp_DoubleSpinBox( this );
  int aPrecision = resMgr->integerValue( "Geometry", "parametric_precision", 6 );
  myDeflectionSB->setAcceptNames( false );
  myDeflectionSB->setPrecision( aPrecision );
  myDeflectionSB->setDecimals( aPrecision );
  myDeflectionSB->setRange( GEOM::minDeflection(), 1.0 );
  myDeflectionSB->setSingleStep( 1.0e-04 );

  layout()->addWidget( deflectionLabel );
  layout()->addWidget( myDeflectionSB );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* study = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  int mgrId = app->activeViewManager()->getGlobalId();
  QVariant v = study->getObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::Deflection ), QVariant() );
  double deflection =  v.isValid() ? v.toDouble() : SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "deflection_coef", 0.001 );
  myDeflectionSB->setValue( deflection );
}

//=================================================================================
// Destructor
//=================================================================================
VTKPlugin_ExportDlg::~VTKPlugin_ExportDlg()
{
}

//=================================================================================
// getDeflection
//=================================================================================
double VTKPlugin_ExportDlg::getDeflection() const
{
  return myDeflectionSB->value();
}
 
//=================================================================================
// getFileName
//=================================================================================
QString VTKPlugin_ExportDlg::getFileName( const Handle(SALOME_InteractiveObject)& io,
					  const QString& filters, const QString& caption,
					  QWidget* parent, double& deflection )
{
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );

  QString tmpfilename = io->getName();
  tmpfilename = tmpfilename.simplified();
  tmpfilename = tmpfilename.replace( QRegExp( "\\*" ), "" ).replace( QRegExp( "\\?" ), "" );

  VTKPlugin_ExportDlg fd( io, parent );
  fd.setFileMode( AnyFile );
  fd.setNameFilters( fls );
  fd.setWindowTitle( caption );
  if ( !tmpfilename.isEmpty() )
    fd.processPath( tmpfilename );

  QString filename;

  if ( fd.exec() == QDialog::Accepted ) {
    filename = fd.selectedFile();
    deflection = fd.getDeflection();
  }

  QApplication::processEvents();

  return filename;
}
  
