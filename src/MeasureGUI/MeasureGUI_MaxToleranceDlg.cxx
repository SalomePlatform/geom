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
// File   : MeasureGUI_MaxToleranceDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//

#include "MeasureGUI_MaxToleranceDlg.h"
#include "MeasureGUI_Widgets.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>

//=================================================================================
// class    : MeasureGUI_MaxToleranceDlg()
// purpose  : Constructs a MeasureGUI_MaxToleranceDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_MaxToleranceDlg::MeasureGUI_MaxToleranceDlg( GeometryGUI* GUI, QWidget* parent )
  : MeasureGUI_Skeleton( GUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_DLG_TOLERANCE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_TOLERANCE_TITLE" ) );

  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_TOLERANCE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  myGrp = new MeasureGUI_1Sel6LineEdit( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_TOLERANCE_CONSTR" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_MIN" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_MAX" ) );
  myGrp->TextLabel4->setText( tr( "GEOM_TOLERANCE_FACE" ) );
  myGrp->TextLabel5->setText( tr( "GEOM_TOLERANCE_EDGE" ) );
  myGrp->TextLabel6->setText( tr( "GEOM_TOLERANCE_VERTEX" ) );
  myGrp->LineEdit11->setReadOnly( true );
  myGrp->LineEdit12->setReadOnly( true );
  myGrp->LineEdit21->setReadOnly( true );
  myGrp->LineEdit22->setReadOnly( true );
  myGrp->LineEdit31->setReadOnly( true );
  myGrp->LineEdit32->setReadOnly( true );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );
  
  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#tolerance_anchor";
  
  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_MaxToleranceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_MaxToleranceDlg::~MeasureGUI_MaxToleranceDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::Init()
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_MaxToleranceDlg::processObject()
{
  double aMinFaceToler, aMaxFaceToler;
  double aMinEdgeToler, aMaxEdgeToler;
  double aMinVertexToler, aMaxVertexToler;
  
  if ( !getParameters( aMinFaceToler, aMaxFaceToler,
                       aMinEdgeToler, aMaxEdgeToler,
                       aMinVertexToler, aMaxVertexToler ) ) {
    myGrp->LineEdit11->setText( "" );
    myGrp->LineEdit12->setText( "" );

    myGrp->LineEdit21->setText( "" );
    myGrp->LineEdit22->setText( "" );

    myGrp->LineEdit31->setText( "" );
    myGrp->LineEdit32->setText( "" );

    return;
  }

  double invalidMin = RealLast();
  double invalidMax = -RealLast();

  myGrp->LineEdit11->setText( aMinFaceToler != invalidMin ? QString( "%1" ).arg( aMinFaceToler, 5, 'e', 8 ) : QString("") );
  myGrp->LineEdit12->setText( aMaxFaceToler != invalidMax ? QString( "%1" ).arg( aMaxFaceToler, 5, 'e', 8 ) : QString("") );

  myGrp->LineEdit21->setText( aMinEdgeToler != invalidMin ? QString( "%1" ).arg( aMinEdgeToler, 5, 'e', 8 ) : QString("") );
  myGrp->LineEdit22->setText( aMaxEdgeToler != invalidMax ? QString( "%1" ).arg( aMaxEdgeToler, 5, 'e', 8 ) : QString("") );

  myGrp->LineEdit31->setText( aMinVertexToler != invalidMin ? QString( "%1" ).arg( aMinVertexToler, 5, 'e', 8 ) : QString("") );
  myGrp->LineEdit32->setText( aMaxVertexToler != invalidMax ? QString( "%1" ).arg( aMaxVertexToler, 5, 'e', 8 ) : QString("") );
}

//=================================================================================
// function : getParameters
// purpose  : Get tolerances. Returns false is myObj is nill. If there is no a
//            type of entity ( face, edge or vertex ) in selected object then corresponding
//            tolerances is less than 0
//=================================================================================
bool MeasureGUI_MaxToleranceDlg::getParameters( double& theMinFaceToler,
                                                double& theMaxFaceToler,
                                                double& theMinEdgeToler,
                                                double& theMaxEdgeToler,
                                                double& theMinVertexToler,
                                                double& theMaxVertexToler )
{
  if ( myObj->_is_nil() )
    return false;
  else {
    try {
      GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->GetTolerance( myObj,
        theMinFaceToler, theMaxFaceToler,   theMinEdgeToler,
        theMaxEdgeToler, theMinVertexToler, theMaxVertexToler );
    }
    catch( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}
