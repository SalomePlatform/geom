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
//  File   : MeasureGUI_CheckShapeDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_CheckShapeDlg.h"
#include "MeasureGUI_1Sel1TextView_QTD.h"

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

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckShapeDlg()
// purpose  : Constructs a MeasureGUI_CheckShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckShapeDlg::MeasureGUI_CheckShapeDlg( GeometryGUI* GUI, QWidget* parent )
: MeasureGUI_Skeleton( GUI, parent, "MeasureGUI_CheckShapeDlg" )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_CHECKSHAPE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_CHECK_TITLE" ) );

  /***************************************************************/

  GroupConstructors->setTitle( tr( "GEOM_CHECK_SHAPE" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_1Sel1TextView_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_CHECK_INFOS" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextEdit1->setReadOnly( TRUE );
  
  QFont aFont( TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE );
  aFont.setStyleHint( QFont::TypeWriter, QFont::PreferAntialias );
  myGrp->TextEdit1->setFont( aFont );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  Layout1->addWidget( myGrp, 1, 0 );

  /***************************************************************/

  myHelpFileName = "files/salome2_sp3_measuregui_functions.htm#check";

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_CheckShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CheckShapeDlg::~MeasureGUI_CheckShapeDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::Init()
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::getParameters ( bool& theIsValid, QString& theMsg )
{
  if ( myObj->_is_nil() )
    return false;
  else
  {
    try
    {
      char* aMsg;
      theIsValid = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->CheckShape( myObj, aMsg );
      theMsg = aMsg;
    }
    catch( const SALOME::SALOME_Exception& e )
    {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}


//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::processObject()
{
  bool isShapeValid;
  QString aMsg;
  if ( !getParameters( isShapeValid, aMsg ) )
  {
    myGrp->TextEdit1->setText( "" );
    return;
  }

  if (isShapeValid) {
    myGrp->TextEdit1->setText("This Shape seems to be valid.");
  } else {
    QString aDescr ("This Shape is not valid.\n");
    aDescr += aMsg;
    myGrp->TextEdit1->setText(aDescr);
//    myGrp->TextEdit1->setText("This Shape is not valid.");
  }
}
