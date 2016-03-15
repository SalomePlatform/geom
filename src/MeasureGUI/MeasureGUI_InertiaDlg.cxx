// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_InertiaDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "MeasureGUI_InertiaDlg.h"
#include "MeasureGUI_Widgets.h"

#include <GEOMBase.h>
#include <DlgRef.h>

#include <gp_XYZ.hxx>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Tools.h>

//=================================================================================
// class    : MeasureGUI_InertiaDlg()
// purpose  : Constructs a MeasureGUI_InertiaDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================

MeasureGUI_InertiaDlg::MeasureGUI_InertiaDlg( GeometryGUI* GUI, QWidget* parent )
  : MeasureGUI_Skeleton( GUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_INERTIA" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_INERTIA_TITLE" ) );

  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_INERTIA_CONSTR" ) );
  mainFrame()->RadioButton1->setIcon( image0 );

  myGrp = new MeasureGUI_1Sel12LineEdit( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_MATRIX" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2_1->setText( "1:1" );
  myGrp->TextLabel2_2->setText( "1:2" );
  myGrp->TextLabel2_3->setText( "1:3" );
  myGrp->TextLabel3_1->setText( "2:1" );
  myGrp->TextLabel3_2->setText( "2:2" );
  myGrp->TextLabel3_3->setText( "2:3" );
  myGrp->TextLabel4_1->setText( "3:1" );
  myGrp->TextLabel4_2->setText( "3:2" );
  myGrp->TextLabel4_3->setText( "3:3" );
  myGrp->TextLabel5_1->setText( "IX" );
  myGrp->TextLabel5_2->setText( "IY" );
  myGrp->TextLabel5_3->setText( "IZ" );
  myGrp->LineEdit11->setReadOnly( true );
  myGrp->LineEdit12->setReadOnly( true );
  myGrp->LineEdit13->setReadOnly( true );
  myGrp->LineEdit21->setReadOnly( true );
  myGrp->LineEdit22->setReadOnly( true );
  myGrp->LineEdit23->setReadOnly( true );
  myGrp->LineEdit31->setReadOnly( true );
  myGrp->LineEdit32->setReadOnly( true );
  myGrp->LineEdit33->setReadOnly( true );
  myGrp->LineEdit41->setReadOnly( true );
  myGrp->LineEdit42->setReadOnly( true );
  myGrp->LineEdit43->setReadOnly( true );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );
  
  /***************************************************************/

  myHelpFileName = "inertia_page.html";

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_InertiaDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_InertiaDlg::~MeasureGUI_InertiaDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::Init()
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : activateSelection()
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::activateSelection()
{
  MeasureGUI_Skeleton::activateSelection();
  localSelection( TopAbs_SHAPE );
}


//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_InertiaDlg::processObject()
{
  gp_XYZ anIXYZ;
  gp_Mat aMat;

  if ( !getParameters( aMat, anIXYZ ) ) {
    mySelEdit->setText( "" );
    
    myGrp->LineEdit11->setText( "" );
    myGrp->LineEdit12->setText( "" );
    myGrp->LineEdit13->setText( "" );
    
    myGrp->LineEdit21->setText( "" );
    myGrp->LineEdit22->setText( "" );
    myGrp->LineEdit23->setText( "" );
    
    myGrp->LineEdit31->setText( "" );
    myGrp->LineEdit32->setText( "" );
    myGrp->LineEdit33->setText( "" );

    myGrp->LineEdit41->setText( "" );
    myGrp->LineEdit42->setText( "" );
    myGrp->LineEdit43->setText( "" );
  }
  else {
    myGrp->LineEdit11->setText( DlgRef::PrintDoubleValue( aMat( 1, 1 ), -6 ) );
    myGrp->LineEdit12->setText( DlgRef::PrintDoubleValue( aMat( 1, 2 ), -6 ) );
    myGrp->LineEdit13->setText( DlgRef::PrintDoubleValue( aMat( 1, 3 ), -6 ) );
    
    myGrp->LineEdit21->setText( DlgRef::PrintDoubleValue( aMat( 2, 1 ), -6 ) );
    myGrp->LineEdit22->setText( DlgRef::PrintDoubleValue( aMat( 2, 2 ), -6 ) );
    myGrp->LineEdit23->setText( DlgRef::PrintDoubleValue( aMat( 2, 3 ), -6 ) );
    
    myGrp->LineEdit31->setText( DlgRef::PrintDoubleValue( aMat( 3, 1 ), -6 ) );
    myGrp->LineEdit32->setText( DlgRef::PrintDoubleValue( aMat( 3, 2 ), -6 ) );
    myGrp->LineEdit33->setText( DlgRef::PrintDoubleValue( aMat( 3, 3 ), -6 ) );
    
    myGrp->LineEdit41->setText( DlgRef::PrintDoubleValue( anIXYZ.X(), -6 ) );
    myGrp->LineEdit42->setText( DlgRef::PrintDoubleValue( anIXYZ.Y(), -6 ) );
    myGrp->LineEdit43->setText( DlgRef::PrintDoubleValue( anIXYZ.Z(), -6 ) );
  }
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_InertiaDlg::getParameters( gp_Mat& I,
                                           gp_XYZ& theIXYZ )
{
  if ( !myObj )
    return false;
  else {
    GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
    try {
      double x, y, z;
      anOper->GetInertia( myObj.get(),
                          I( 1, 1 ), I( 1, 2 ), I( 1, 3 ),
                          I( 2, 1 ), I( 2, 2 ), I( 2, 3 ),
                          I( 3, 1 ), I( 3, 2 ), I( 3, 3 ),
                          x, y, z );

        theIXYZ.SetCoord( x, y, z );
    }
    catch ( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return anOper->IsDone();
  }
}

void MeasureGUI_InertiaDlg::SelectionIntoArgument()
{
  myObj.nullify();
  QList<TopAbs_ShapeEnum> aTypes;
  aTypes << TopAbs_VERTEX << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPSOLID << TopAbs_COMPOUND << TopAbs_SHAPE;
  myObj = getSelected( aTypes );
 
  if (!myObj) {
    mySelEdit->setText("");
    processObject();
    erasePreview();
    return;
  }

  mySelEdit->setText(GEOMBase::GetName(myObj.get()));
  processObject();
  redisplayPreview();
}
