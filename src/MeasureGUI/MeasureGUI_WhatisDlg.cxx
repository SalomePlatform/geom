//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MeasureGUI_WhatisDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_WhatisDlg.h"
#include "MeasureGUI_1Sel1TextView_QTD.h"
#include "GEOMBase.h"

#include <TopTools_MapOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Tool.hxx>

#include "utilities.h"
#include "SUIT_Session.h"
#include "SalomeApp_Tools.h"

#include <qtextedit.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : MeasureGUI_WhatisDlg( )
// purpose  : Constructs a MeasureGUI_WhatisDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_WhatisDlg::MeasureGUI_WhatisDlg( GeometryGUI* GUI, QWidget* parent )
: MeasureGUI_Skeleton( GUI, parent, "MeasureGUI_WhatisDlg" )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_WHATIS" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_WHATIS_TITLE" ) );

  /***************************************************************/
  
  GroupConstructors->setTitle( tr( "GEOM_WHATIS" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_1Sel1TextView_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_WHATIS_OBJECT" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextEdit1->setReadOnly( TRUE );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  Layout1->addWidget( myGrp, 1, 0 );

  /***************************************************************/

  myHelpFileName = "files/salome2_sp3_measuregui_functions.htm#Whatis";

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
  myGrp->TextEdit1->setText( getParameters( aText ) ? aText : QString("") );
}


//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_WhatisDlg::getParameters( QString& theText )
{
  if ( myObj->_is_nil() )
    return false;
  else
  {
    try
    {
      theText = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->WhatIs( myObj );
    }
    catch( const SALOME::SALOME_Exception& e )
    {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}



