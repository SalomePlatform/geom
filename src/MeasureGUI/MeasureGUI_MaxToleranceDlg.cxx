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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : MeasureGUI_MaxToleranceDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_MaxToleranceDlg.h"
#include "MeasureGUI_1Sel6LineEdit_QTD.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"

#include "utilities.h"
#include "QAD_Desktop.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : MeasureGUI_MaxToleranceDlg()
// purpose  : Constructs a MeasureGUI_MaxToleranceDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_MaxToleranceDlg::MeasureGUI_MaxToleranceDlg( QWidget* parent, SALOME_Selection* Sel )
: MeasureGUI_Skeleton( parent, "MeasureGUI_MaxToleranceDlg", Sel )
{
  QPixmap image0( QAD_Desktop::getResourceManager()->loadPixmap(
    "GEOM",tr( "ICON_DLG_TOLERANCE" ) ) );
  QPixmap image1( QAD_Desktop::getResourceManager()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_TOLERANCE_TITLE" ) );

  /***************************************************************/
  
  GroupConstructors->setTitle( tr( "GEOM_TOLERANCE" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_1Sel6LineEdit_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_TOLERANCE_CONSTR" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_MIN" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_MAX" ) );
  myGrp->TextLabel4->setText( tr( "GEOM_TOLERANCE_FACE" ) );
  myGrp->TextLabel5->setText( tr( "GEOM_TOLERANCE_EDGE" ) );
  myGrp->TextLabel6->setText( tr( "GEOM_TOLERANCE_VERTEX" ) );
  myGrp->LineEdit11->setReadOnly( TRUE );
  myGrp->LineEdit12->setReadOnly( TRUE );
  myGrp->LineEdit21->setReadOnly( TRUE );
  myGrp->LineEdit22->setReadOnly( TRUE );
  myGrp->LineEdit31->setReadOnly( TRUE );
  myGrp->LineEdit32->setReadOnly( TRUE );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  Layout1->addWidget( myGrp, 1, 0 );
  
  /***************************************************************/

  /* Initialisation */
  Init( Sel );
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
void MeasureGUI_MaxToleranceDlg::Init( SALOME_Selection* Sel )
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init( Sel );
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
                       aMinVertexToler, aMaxVertexToler ) )
  {
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
  else
  {
    try
    {
      GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->GetTolerance( myObj,
        theMinFaceToler, theMaxFaceToler, theMinEdgeToler,
        theMaxEdgeToler, theMinVertexToler, theMaxVertexToler  );
    }
    catch( const SALOME::SALOME_Exception& e )
    {
      QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}





















