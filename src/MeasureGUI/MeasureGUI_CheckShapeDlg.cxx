// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_CheckShapeDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#include "MeasureGUI_CheckShapeDlg.h"
#include "MeasureGUI_Widgets.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckShapeDlg()
// purpose  : Constructs a MeasureGUI_CheckShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckShapeDlg::MeasureGUI_CheckShapeDlg( GeometryGUI* GUI, QWidget* parent )
  : MeasureGUI_Skeleton( GUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_DLG_CHECKSHAPE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CHECK_TITLE" ) );

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CHECK_SHAPE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  myGrp = new MeasureGUI_1Sel1TextView1Check( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_CHECK_INFOS" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextView1->setReadOnly( true );

  QFont aFont( TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE );
  aFont.setStyleHint( QFont::TypeWriter, QFont::PreferAntialias );
  myGrp->TextView1->setFont( aFont );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#check_anchor";

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

  connect( myGrp->CheckBox1, SIGNAL( toggled( bool) ), 
           this, SLOT( SelectionIntoArgument() ) );
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::getParameters ( bool& theIsValid, QString& theMsg )
{
  if ( myObj->_is_nil() )
    return false;
  else {
    GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
    try {
      char* aMsg;
      bool isCheckGeometry = myGrp->CheckBox1->isChecked();
      if ( isCheckGeometry )
        theIsValid = anOper->CheckShapeWithGeometry( myObj, aMsg );
      else
        theIsValid = anOper->CheckShape( myObj, aMsg );
      theMsg = aMsg;
    }
    catch( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return anOper->IsDone();
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
  if ( !getParameters( isShapeValid, aMsg ) ) {
    myGrp->TextView1->setText( "" );
    return;
  }

  if ( isShapeValid ) {
    myGrp->TextView1->setText( "This Shape seems to be valid." );
  } 
  else {
    QString aDescr ( "This Shape is not valid.\n" );
    aDescr += aMsg;
    myGrp->TextView1->setText( aDescr );
//    myGrp->TextView1->setText("This Shape is not valid.");
  }
}
