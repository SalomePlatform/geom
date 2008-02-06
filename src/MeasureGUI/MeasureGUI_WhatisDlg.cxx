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
// File   : MeasureGUI_WhatisDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//

#include "MeasureGUI_WhatisDlg.h"
#include "MeasureGUI_Widgets.h"

#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>

#include <QScrollBar>

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
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_DLG_WHATIS" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_WHATIS_TITLE" ) );

  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_WHATIS" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  myGrp = new MeasureGUI_1Sel1TextView( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_WHATIS_OBJECT" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextView1->setReadOnly( true );

  // fix height to fit all text
  myGrp->TextView1->setLineWrapMode( QTextEdit::NoWrap );
  // myGrp->TextView1->setTextFormat( Qt::PlainText ); // VSR : TODO
  QString allLines ("\n\n\n\n\n\n\n\n\n"); // 10 lines
  myGrp->TextView1->setText( allLines );
  int sbHeight = myGrp->TextView1->horizontalScrollBar()->height();
  myGrp->TextView1->setFixedHeight( myGrp->TextView1->document()->size().height() + sbHeight );

  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

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
  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_WhatisDlg::processObject()
{
  QString aText;
  myGrp->TextView1->setText( getParameters( aText ) ? aText : QString( "" ) );
}


//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_WhatisDlg::getParameters( QString& theText )
{
  if ( myObj->_is_nil() )
    return false;
  else {
    try {
      theText = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->WhatIs( myObj );
    }
    catch( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}
