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
//  File   : MeasureGUI_PropertiesDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_PropertiesDlg.h"
#include "MeasureGUI_1Sel3LineEdit_QTD.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "GEOM_Displayer.h"
#include "utilities.h"
#include "QAD_Desktop.h"
#include "GEOMImpl_Types.hxx"
#include "GEOMBase.h"

#include <TColStd_MapOfInteger.hxx>

#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : MeasureGUI_PropertiesDlg()
// purpose  : Constructs a MeasureGUI_PropertiesDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_PropertiesDlg::MeasureGUI_PropertiesDlg( QWidget* parent, SALOME_Selection* Sel )
: MeasureGUI_Skeleton( parent, "MeasureGUI_PropertiesDlg", Sel )
{
  QPixmap image0( QAD_Desktop::getResourceManager()->loadPixmap(
    "GEOM",tr( "ICON_DLG_BASICPROPERTIES" ) ) );
  QPixmap image1( QAD_Desktop::getResourceManager()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_PROPERTIES_TITLE" ) );

  /***************************************************************/
  
  GroupConstructors->setTitle( tr( "GEOM_PROPERTIES" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_1Sel3LineEdit_QTD( this, "myGrp"  );
  myGrp->GroupBox1->setTitle( tr( "GEOM_PROPERTIES_CONSTR" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_LENGTH" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_PROPERTIES_SURFACE" ) );
  myGrp->TextLabel4->setText( tr( "GEOM_PROPERTIES_VOLUME" ) );
  myGrp->LineEdit2->setReadOnly( TRUE );
  myGrp->LineEdit3->setReadOnly( TRUE );
  myGrp->LineEdit4->setReadOnly( TRUE );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  Layout1->addWidget( myGrp, 1, 0 );
  
  /***************************************************************/

  /* Initialisation */
  Init( Sel );
}


//=================================================================================
// function : ~MeasureGUI_PropertiesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_PropertiesDlg::~MeasureGUI_PropertiesDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::Init( SALOME_Selection* Sel   )
{
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init( Sel );
}

//=================================================================================
// function : activateSelection
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::activateSelection()
{
  static TColStd_MapOfInteger aTypes;
  if ( aTypes.IsEmpty() )
  {
    aTypes.Add( GEOM_COMPOUND );
//    aTypes.Add( TopAbs_COMPSOLID );
    aTypes.Add( GEOM_SOLID );
    aTypes.Add( GEOM_SHELL );
    aTypes.Add( GEOM_FACE );
    aTypes.Add( GEOM_WIRE );
    aTypes.Add( GEOM_EDGE );
  }

  globalSelection( aTypes );
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::processObject()
{
  double aLength, anArea, aVolume;
  
  if ( !getParameters( aLength, anArea, aVolume ) )
  {
    mySelEdit->setText( "" );
    myGrp->LineEdit2->setText( "" );
    myGrp->LineEdit3->setText( "" );
    myGrp->LineEdit4->setText( "" );
  }
  else
  {
    myGrp->LineEdit2->setText( QString( "%1" ).arg( aLength ) );
    myGrp->LineEdit3->setText( QString( "%1" ).arg( anArea ) );
    myGrp->LineEdit4->setText( QString( "%1" ).arg( aVolume ) );
  }
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
bool MeasureGUI_PropertiesDlg::getParameters( double& theLength,
                                              double& theArea,
                                              double& theVolume )
{
  if ( myObj->_is_nil() )
    return false;
  else
  {
    try
    {
      GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->GetBasicProperties(
        myObj, theLength, theArea, theVolume );
    }
    catch( const SALOME::SALOME_Exception& e )
    {
      QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_PropertiesDlg::buildPrs()
{
  TopoDS_Shape aShape, aResult;
  
  if ( myObj->_is_nil() ||
       !GEOMBase::GetShape( myObj, aShape )||
       aShape.IsNull() ||
       aShape.ShapeType() != TopAbs_EDGE ||
       !GEOMBase::CreateArrowForLinearEdge( aShape, aResult ) ||
       aResult.IsNull() )
    return 0;

  return getDisplayer()->BuildPrs( aResult );
  
}





















