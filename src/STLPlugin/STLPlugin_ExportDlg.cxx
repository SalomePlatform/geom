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
#include "STLPlugin_ExportDlg.h"

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Study.h>

// GEOM includes
#include "GEOMBase.h"
#include "GEOM_Constants.h"

// OCC includes
#include <BRepBndLib.hxx>

// QT includes
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QComboBox>
#include <QCheckBox>

//=================================================================================
// Constructor
//=================================================================================
STLPlugin_ExportDlg::STLPlugin_ExportDlg( const Handle(SALOME_InteractiveObject)& io, QWidget* parent )
: SUIT_FileDlg( parent, false, true, true )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QLabel* deflectionLabel = new QLabel( tr( "DEFLECTION" ), this );

  QWidget* options = new QWidget( this );
  QHBoxLayout* optionsLayout = new QHBoxLayout( options );
  optionsLayout->setMargin( 0 );
  optionsLayout->setSpacing( 5 );

  myDeflectionSB = new SalomeApp_DoubleSpinBox( options );
  int aPrecision = resMgr->integerValue( "Geometry", "parametric_precision", 6 );
  myDeflectionSB->setAcceptNames( false );
  myDeflectionSB->setPrecision( aPrecision );
  myDeflectionSB->setDecimals( aPrecision );
  myDeflectionSB->setRange( GEOM::minDeflection(), 1.0 );
  myDeflectionSB->setSingleStep( 1.0e-04 );

  myModeCB = new QCheckBox( tr( "RELATIVE" ), options );

  myFormatCB = new QCheckBox( tr( "ASCII" ) );

  optionsLayout->addWidget( myDeflectionSB );
  optionsLayout->addWidget( myModeCB );
  optionsLayout->addWidget( myFormatCB );

  layout()->addWidget( deflectionLabel );
  layout()->addWidget( options );

  myShapeSize = getShapeSize( io );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* study = dynamic_cast< SalomeApp_Study* >( app->activeStudy() );
  int mgrId = app->activeViewManager()->getGlobalId();
  QVariant v = study->getObjectProperty( mgrId, io->getEntry(), GEOM::propertyName( GEOM::Deflection ), QVariant() );
  double deflection =  v.isValid() ? v.toDouble() : SUIT_Session::session()->resourceMgr()->doubleValue( "Geometry", "deflection_coef", 0.001 );

  myDeflectionSB->setValue( deflection );
  myModeCB->setChecked( true );
  myFormatCB->setChecked( true );

  connect( myModeCB, SIGNAL( clicked ( bool ) ), this, SLOT( modeChanged() ) );
}

//=================================================================================
// Destructor
//=================================================================================
STLPlugin_ExportDlg::~STLPlugin_ExportDlg()
{
}

//=================================================================================
// modeChanged
//=================================================================================
void STLPlugin_ExportDlg::modeChanged()
{
  if ( myModeCB->isChecked() ) {
    double deflection = myDeflectionSB->value() / myShapeSize;
    deflection = ( deflection > 1.0 ) ? 1.0 : deflection;
    myDeflectionSB->setRange( GEOM::minDeflection(), 1.0 );
    myDeflectionSB->setValue( deflection );
  }
  else {
    double deflection = myDeflectionSB->value() * myShapeSize;
    myDeflectionSB->setRange( GEOM::minDeflection(), 10000.0 );
    myDeflectionSB->setValue( deflection );
  }
}

//=================================================================================
// getShapeSize
//=================================================================================
double STLPlugin_ExportDlg::getShapeSize( const Handle(SALOME_InteractiveObject)& io )
{
  TopoDS_Shape shape;
  GEOM::GEOM_Object_var obj = GEOMBase::ConvertIOinGEOMObject( io );
  GEOMBase::GetShape( obj, shape, TopAbs_SHAPE );
  Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
  Bnd_Box bnd;
  BRepBndLib::Add( shape, bnd );
  bnd.Get( xmin, ymin, zmin, xmax, ymax, zmax );
  double xd = fabs( xmax-xmin );
  double yd = fabs( ymax-ymin );
  double zd = fabs( zmax-zmin ); 
  return std::max( std::max ( xd, yd ), zd );
}

//=================================================================================
// isAscii
//=================================================================================
bool STLPlugin_ExportDlg::isAscii() const
{
  return myFormatCB->isChecked();
}

//=================================================================================
// getDeflection
//=================================================================================
double STLPlugin_ExportDlg::getDeflection() const
{
  return myDeflectionSB->value();
}
 
//=================================================================================
// isDeflectionRelative
//=================================================================================
bool STLPlugin_ExportDlg::isDeflectionRelative() const
{
  return myModeCB->isChecked();
}

//=================================================================================
// getFileName
//=================================================================================
QString STLPlugin_ExportDlg::getFileName( const Handle(SALOME_InteractiveObject)& io,
					  const QString& filters, const QString& caption,
					  QWidget* parent, bool& isAscii,
					  double& deflection, bool& isRelative )
{
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );

  QString tmpfilename = io->getName();
  tmpfilename = tmpfilename.simplified();
  tmpfilename = tmpfilename.replace( QRegExp( "\\*" ), "" ).replace( QRegExp( "\\?" ), "" );

  STLPlugin_ExportDlg fd( io, parent );
  fd.setFileMode( AnyFile );
  fd.setNameFilters( fls );
  fd.setWindowTitle( caption );
  if ( !tmpfilename.isEmpty() )
    fd.processPath( tmpfilename );

  QString filename;

  if ( fd.exec() == QDialog::Accepted ) {
    filename = fd.selectedFile();
    isAscii = fd.isAscii();
    deflection = fd.getDeflection();
    isRelative = fd.isDeflectionRelative();
  }

  QApplication::processEvents();

  return filename;
}
  
