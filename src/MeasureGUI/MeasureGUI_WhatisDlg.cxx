//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_WhatisDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#include "MeasureGUI_WhatisDlg.h"
#include "MeasureGUI_Widgets.h"

#include <DlgRef.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>

#include <QGroupBox>
#include <QScrollBar>
#include <QMessageBox>

class MeasureGUI_WhatisDlg::Whatis : public QGroupBox
{ 
public:
  Whatis( QWidget* parent );
  ~Whatis();

  QLabel*      TextLabel1;
  QPushButton* PushButton1;
  QLineEdit*   LineEdit1;
  
  QTextEdit*   TextEdit1;
  
  QLabel*      TextLabel2;
  QLineEdit*   LineEdit2;
  QPushButton* PushButton2;
};

//=================================================================================
// Constructs a MeasureGUI_Whatis_QTD which is a child of 'parent', with the
// name 'name' and widget flags set to 'f'
//=================================================================================
MeasureGUI_WhatisDlg::Whatis::Whatis( QWidget* parent )
  : QGroupBox( parent )
{
  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setMargin( 11 );
  topLayout->setSpacing( 6 );

  TextLabel1  = new QLabel( this );
  PushButton1 = new QPushButton( this );
  LineEdit1   = new QLineEdit( this );
  TextEdit1   = new QTextEdit( this );
  TextLabel2  = new QLabel( this );
  LineEdit2   = new QLineEdit( this);
  PushButton2 = new QPushButton( this );

  topLayout->addWidget( TextLabel1,  0, 0 );
  topLayout->addWidget( PushButton1, 0, 1 );
  topLayout->addWidget( LineEdit1,   0, 2, 1, 2 );
  topLayout->addWidget( TextEdit1,   1, 0, 1, 4 );
  topLayout->addWidget( TextLabel2 , 2, 0 );
  topLayout->addWidget( LineEdit2  , 2, 1, 1, 2 );
  topLayout->addWidget( PushButton2, 2, 3 );
}

//=================================================================================
// Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_WhatisDlg::Whatis::~Whatis()
{
}

//=================================================================================
// class    : MeasureGUI_WhatisDlg( )
// purpose  : Constructs a MeasureGUI_WhatisDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_WhatisDlg::MeasureGUI_WhatisDlg( GeometryGUI* GUI, QWidget* parent )
  : MeasureGUI_Skeleton( GUI, parent )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0( resMgr->loadPixmap( "GEOM", tr( "ICON_DLG_WHATIS" ) ) );
  QPixmap image1( resMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_WHATIS_TITLE" ) );

  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_WHATIS" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  myGrp = new Whatis( this );

  myGrp->setTitle( tr( "GEOM_WHATIS_OBJECT" ) );

  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->PushButton1->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  myGrp->LineEdit1->setReadOnly( true );

  myGrp->TextEdit1->setReadOnly( true );
  // fix height to fit all text
  myGrp->TextEdit1->setLineWrapMode( QTextEdit::NoWrap );
  // myGrp->TextEdit1->setTextFormat( Qt::PlainText ); // VSR : TODO
  QString allLines ("\n\n\n\n\n\n\n\n\n"); // 10 lines
  myGrp->TextEdit1->setText( allLines );
  int sbHeight = myGrp->TextEdit1->horizontalScrollBar()->height();
  myGrp->TextEdit1->setFixedHeight( myGrp->TextEdit1->document()->size().height() + sbHeight );

  myGrp->TextLabel2->setText( tr( "GEOM_KIND_OF_SHAPE" ) );
  myGrp->LineEdit2->setReadOnly( true );
  myGrp->LineEdit2->setMinimumWidth( 150 );
  myGrp->PushButton2->setText( tr( "GEOM_PROPERTIES" ) );
  myGrp->PushButton2->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  // Properties dialog
  myParamsDlg = new QMessageBox(QMessageBox::Information, "", "", QMessageBox::Ok, this);
  myParamsDlg->setModal( false );

  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#whatis_anchor";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_WhatisDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_WhatisDlg::~MeasureGUI_WhatisDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::Init()
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;

  connect( myGrp->PushButton2, SIGNAL( clicked() ), this, SLOT( ClickOnProperties() ) );

  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::processObject()
{
  QString aText;
  myGrp->TextEdit1->setText( getParameters( aText ) ? aText : QString( "" ) );

  QString aParameters;
  QString aKind = getKindOfShape( aParameters );
  myGrp->LineEdit2->setText( aKind );
  myGrp->PushButton2->setEnabled( !aParameters.isEmpty());
  myParamsDlg->setWindowTitle(aKind + " " + tr("GEOM_PROPERTIES"));
  myParamsDlg->setText(aParameters);
  myParamsDlg->adjustSize();
}

//=================================================================================
// function : ClickOnProperties()
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::ClickOnProperties()
{
  myParamsDlg->show();
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_WhatisDlg::getParameters( QString& theText )
{
  if ( myObj->_is_nil() )
    return false;

  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
  try
  {
    theText = anOper->WhatIs( myObj );
  }
  catch( const SALOME::SALOME_Exception& e )
  {
    SalomeApp_Tools::QtCatchCorbaException( e );
    return false;
  }

  return anOper->IsDone();
}

//=================================================================================
// function : getKindOfShape
// purpose  :
//=================================================================================
QString MeasureGUI_WhatisDlg::getKindOfShape( QString& theParameters )
{
  QString aKindStr("");
  theParameters = "";

  if ( myObj->_is_nil() )
    return aKindStr;

  GEOM::GEOM_IKindOfShape::shape_kind aKind;
  GEOM::ListOfLong_var anInts;
  GEOM::ListOfDouble_var aDbls;

  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );

  try
  {
    aKind = anOper->KindOfShape( myObj, anInts, aDbls );
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
    return aKindStr;
  }

  if ( !anOper->IsDone() )
    return aKindStr;

#define PRINT_DOUBLE(val) QString(" %1").arg( DlgRef::PrintDoubleValue( val ) )
  switch ( aKind )
  {
  case GEOM::GEOM_IKindOfShape::COMPOUND:
    aKindStr = tr( "GEOM_COMPOUND" );
    break;
  case GEOM::GEOM_IKindOfShape::COMPSOLID:
    aKindStr = tr( "GEOM_COMPOUNDSOLID" );
    break;
  case GEOM::GEOM_IKindOfShape::SHELL:
    aKindStr = tr( "GEOM_SHELL" );
    if ( anInts[0] == 1 )
      theParameters = tr( "GEOM_CLOSED" );
    else if ( anInts[0] == 2 )
      theParameters = tr( "GEOM_UNCLOSED" );
    else
      theParameters = tr( "GEOM_CLOSEDUNCLOSED" );
    break;
  case GEOM::GEOM_IKindOfShape::WIRE:
    aKindStr = tr( "GEOM_WIRE" );
    if ( anInts[0] == 1 )
      theParameters = tr( "GEOM_CLOSED" );
    else if ( anInts[0] == 2 )
      theParameters = tr( "GEOM_UNCLOSED" );
    else
      theParameters = tr( "GEOM_CLOSEDUNCLOSED" );
    break;
  // SOLIDs
  case GEOM::GEOM_IKindOfShape::SPHERE:
    aKindStr = tr( "GEOM_SPHERE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )       + PRINT_DOUBLE( aDbls[3] );
    break;
  case GEOM::GEOM_IKindOfShape::CYLINDER:
    aKindStr = tr( "GEOM_CYLINDER" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )         +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )       + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_HEIGHT" )       + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::BOX:
    aKindStr = tr( "GEOM_BOX" );
    theParameters = tr( "GEOM_CENTER") +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        "Ax :"                    + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        "Ay :"                    + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        "Az :"                    + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::ROTATED_BOX:
    aKindStr = tr( "GEOM_BOX" );
    theParameters = tr( "GEOM_CENTER" )         +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[2] ) +
      "\nZ Axis:"                               +
      "\n" +        "Zx :"                      + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        "Zy :"                      + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        "Zz :"                      + PRINT_DOUBLE( aDbls[5] ) +
      "\nX Axis:"                               +
      "\n" +        tr( "GEOM_X_I" ).arg( "x" ) + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_X_I" ).arg( "y" ) + PRINT_DOUBLE( aDbls[7] ) +
      "\n" +        tr( "GEOM_X_I" ).arg( "z" ) + PRINT_DOUBLE( aDbls[8] ) +
      "\nDimensions along local axes:"          +
      "\n" +        "Ax :"                      + PRINT_DOUBLE( aDbls[9] ) +
      "\n" +        "Ay :"                      + PRINT_DOUBLE( aDbls[10] ) +
      "\n" +        "Az :"                      + PRINT_DOUBLE( aDbls[11] );
    break;
  case GEOM::GEOM_IKindOfShape::TORUS:
    aKindStr = tr( "GEOM_TORUS" );
    theParameters = tr( "GEOM_CENTER" )            +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )              +
      "\n" +        tr( "GEOM_DX" )                + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )                + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )                + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )        +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 1 ) + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 2 ) + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::CONE:
    aKindStr = tr( "GEOM_CONE" );
    theParameters = tr( "GEOM_CENTER" )            +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )              +
      "\n" +        tr( "GEOM_DX" )                + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )                + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )                + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )        +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 1 ) + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 2 ) + PRINT_DOUBLE( aDbls[7] ) +
      "\n" +        tr( "GEOM_HEIGHT" )            + PRINT_DOUBLE( aDbls[8] );
    break;
  case GEOM::GEOM_IKindOfShape::POLYHEDRON:
    aKindStr = tr( "GEOM_POLYHEDRON" );
    break;
  case GEOM::GEOM_IKindOfShape::SOLID:
    aKindStr = tr( "GEOM_SOLID" );
    break;
  // FACEs
  case GEOM::GEOM_IKindOfShape::SPHERE2D:
    aKindStr = tr( "GEOM_SURFSPHERE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )       + PRINT_DOUBLE( aDbls[3] );
    break;
  case GEOM::GEOM_IKindOfShape::CYLINDER2D:
    aKindStr = tr( "GEOM_SURFCYLINDER" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )         +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )       + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_HEIGHT" )       + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::TORUS2D:
    aKindStr = tr( "GEOM_SURFTORUS" );
    theParameters = tr( "GEOM_CENTER" )            +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )              +
      "\n" +        tr( "GEOM_DX" )                + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )                + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )                + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )        +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 1 ) + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 2 ) + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::CONE2D:
    aKindStr = tr( "GEOM_SURFCONE" );
    theParameters = tr( "GEOM_CENTER" )            +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )      + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_AXIS" )              +
      "\n" +        tr( "GEOM_DX" )                + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )                + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )                + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )        +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 1 ) + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_I" ).arg( 2 ) + PRINT_DOUBLE( aDbls[7] ) +
      "\n" +        tr( "GEOM_HEIGHT" )            + PRINT_DOUBLE( aDbls[8] );
    break;
  case GEOM::GEOM_IKindOfShape::DISK_CIRCLE:
    aKindStr = tr( "GEOM_DISK_CIRCLE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )       +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )      + PRINT_DOUBLE( aDbls[6] );
    break;
  case GEOM::GEOM_IKindOfShape::DISK_ELLIPSE:
    aKindStr = tr( "GEOM_DISK_ELLIPSE" );
    theParameters = tr( "GEOM_CENTER" )         +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )         +
      "\n" +        tr( "GEOM_DX" )             + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )             + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )             + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )     +
      "\n" +        tr( "GEOM_RADIUS_MAJOR" )   + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_MINOR" )   + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::POLYGON:
    aKindStr = tr( "GEOM_POLYGON" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )       +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::PLANE:
    aKindStr = tr( "GEOM_PLANE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )       +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::PLANAR:
    aKindStr = tr( "GEOM_PLANAR_FACE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )       +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::FACE:
    aKindStr = tr( "GEOM_FACE" );
    break;
  // EDGEs
  case GEOM::GEOM_IKindOfShape::CIRCLE:
    aKindStr = tr( "GEOM_CIRCLE" );
    theParameters = tr( "GEOM_CENTER" )       +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )       +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )   +
      "\n" +        tr( "GEOM_RADIUS" )       + PRINT_DOUBLE( aDbls[6] );
    break;
  case GEOM::GEOM_IKindOfShape::ARC_CIRCLE:
    aKindStr = tr( "GEOM_ARC" );
    theParameters = tr( "GEOM_CENTER" )           +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )           +
      "\n" +        tr( "GEOM_DX" )               + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )               + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )               + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )       +
      "\n" +        tr( "GEOM_RADIUS" )           + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_POINT_I" ).arg( 1 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[7] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[8] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[9] ) +
      "\n" +        tr( "GEOM_POINT_I" ).arg( 2 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[10] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[11] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[12] );
    break;
  case GEOM::GEOM_IKindOfShape::ELLIPSE:
    aKindStr = tr( "GEOM_ELLIPSE" );
    theParameters = tr( "GEOM_CENTER" )         +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )   + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )         +
      "\n" +        tr( "GEOM_DX" )             + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )             + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )             + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )     +
      "\n" +        tr( "GEOM_RADIUS_MAJOR" )   + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_MINOR" )   + PRINT_DOUBLE( aDbls[7] );
    break;
  case GEOM::GEOM_IKindOfShape::ARC_ELLIPSE:
    aKindStr = tr( "GEOM_ARC_ELLIPSE" );
    theParameters = tr( "GEOM_CENTER" )           +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 )     + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_NORMAL" )           +
      "\n" +        tr( "GEOM_DX" )               + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )               + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )               + PRINT_DOUBLE( aDbls[5] ) +
      "\n" +        tr( "GEOM_DIMENSIONS" )       +
      "\n" +        tr( "GEOM_RADIUS_MAJOR" )     + PRINT_DOUBLE( aDbls[6] ) +
      "\n" +        tr( "GEOM_RADIUS_MINOR" )     + PRINT_DOUBLE( aDbls[7] ) +
      "\n" +        tr( "GEOM_POINT_I" ).arg( 1 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[8] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[9] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[10] ) +
      "\n" +        tr( "GEOM_POINT_I" ).arg( 2 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[11] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[12] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[13] );
    break;
  case GEOM::GEOM_IKindOfShape::LINE:
    aKindStr = tr( "GEOM_LINE" );
    theParameters = tr( "GEOM_POSITION" )     +
      "\n" +        tr( "GEOM_X_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 0 ) + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_DIRECTION" )    +
      "\n" +        tr( "GEOM_DX" )           + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_DY" )           + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_DZ" )           + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::SEGMENT:
    aKindStr = tr( "GEOM_SEGMENT" );
    theParameters = tr( "GEOM_POINT_I" ).arg( 1 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 1 )     + PRINT_DOUBLE( aDbls[2] ) +
      "\n" +        tr( "GEOM_POINT_I" ).arg( 2 ) +
      "\n" +        tr( "GEOM_X_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[3] ) +
      "\n" +        tr( "GEOM_Y_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[4] ) +
      "\n" +        tr( "GEOM_Z_I" ).arg( 2 )     + PRINT_DOUBLE( aDbls[5] );
    break;
  case GEOM::GEOM_IKindOfShape::EDGE:
    aKindStr = tr( "GEOM_EDGE" );
    break;
  case GEOM::GEOM_IKindOfShape::VERTEX:
    aKindStr = tr( "GEOM_VERTEX" );
    theParameters = tr( "GEOM_COORDINATES" ) +
      "\n" +        tr( "GEOM_X" )           + PRINT_DOUBLE( aDbls[0] ) +
      "\n" +        tr( "GEOM_Y" )           + PRINT_DOUBLE( aDbls[1] ) +
      "\n" +        tr( "GEOM_Z" )           + PRINT_DOUBLE( aDbls[2] );
    break;
  case GEOM::GEOM_IKindOfShape::ADVANCED:
    {
      QString strid = QString( "GEOM_ADVANCED_%1" ).arg( myObj->GetType() ); 
      aKindStr = tr( strid.toLatin1().constData() ) == strid ? 
	tr( "GEOM_ADVANCED" ).arg( myObj->GetType() ) :
	tr( strid.toLatin1().constData() );
      break;
    }
  default:
    break;
  }

  return aKindStr;
}
