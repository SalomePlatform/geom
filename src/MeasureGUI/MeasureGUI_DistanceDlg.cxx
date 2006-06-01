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
//  File   : MeasureGUI_DistanceDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_DistanceDlg.h"
#include "MeasureGUI_2Sel1LineEdit_QTD.h"
#include "GEOMBase.h"
#include "GEOM_Displayer.h"

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"
#include "SOCC_Prs.h"
#include "SOCC_ViewModel.h"
#include "SalomeApp_Tools.h"

#include <Geom_Plane.hxx>
#include <TopoDS_Edge.hxx>
#include <AIS_LengthDimension.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <gce_MakePln.hxx>
#include <Precision.hxx>

#include "utilities.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : MeasureGUI_DistanceDlg()
// purpose  : Constructs a MeasureGUI_DistanceDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_DistanceDlg::MeasureGUI_DistanceDlg( GeometryGUI* GUI, QWidget* parent )
: MeasureGUI_Skeleton( GUI, parent, "MeasureGUI_DistanceDlg" )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_MINDIST" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
  "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_MINDIST_TITLE" ) );

  /***************************************************************/
  
  GroupConstructors->setTitle( tr( "GEOM_DISTANCE" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_2Sel1LineEdit_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_MINDIST_OBJ" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT_I" ).arg( "1" ) );
  myGrp->TextLabel2->setText( tr( "GEOM_OBJECT_I" ).arg( "2" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_LENGTH" ) );
  myGrp->LineEdit3->setReadOnly( TRUE );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->PushButton2->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );
  myGrp->LineEdit2->setReadOnly( true );

  Layout1->addWidget( myGrp, 1, 0 );

  /***************************************************************/

  myHelpFileName = "files/salome2_sp3_measuregui_functions.htm#Min_distance";

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_DistanceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_DistanceDlg::~MeasureGUI_DistanceDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::Init()
{
  mySelBtn   = myGrp->PushButton1;
  mySelEdit  = myGrp->LineEdit1;
  mySelBtn2  = myGrp->PushButton2;
  mySelEdit2 = myGrp->LineEdit2;
  
  myEditCurrentArgument = mySelEdit;

  connect( mySelEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( mySelBtn2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  MeasureGUI_Skeleton::Init();

}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void MeasureGUI_DistanceDlg::SelectionIntoArgument()
{
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );

  if ( !testResult )
    aSelectedObject = GEOM::GEOM_Object::_nil();

  if ( myEditCurrentArgument == mySelEdit )
      myObj = aSelectedObject;
    else
      myObj2 = aSelectedObject;

  processObject();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialogs fileds in accordance with myObj and myObj2
//=================================================================================
void MeasureGUI_DistanceDlg::processObject()
{
  myGrp->LineEdit1->setText( !myObj->_is_nil()  ? GEOMBase::GetName( myObj  ) : "" );
  myGrp->LineEdit2->setText( !myObj2->_is_nil() ? GEOMBase::GetName( myObj2 ) : "" );

  gp_Pnt aPnt1, aPnt2;
  double aDist = 0.;
  if ( getParameters( aDist, aPnt1, aPnt2 ) )
  {
    myGrp->LineEdit3->setText( QString( "%1" ).arg( aDist ) );
    redisplayPreview();
  }
  else
  {
    myGrp->LineEdit3->setText( "" );
    erasePreview();
  }
    
}

//=================================================================================
// function : getParameters()
// purpose  : Get distance between objects
//=================================================================================
bool MeasureGUI_DistanceDlg::getParameters( double& theDistance,
                                            gp_Pnt& thePnt1,
                                            gp_Pnt& thePnt2 )
{
  QString msg;
  if ( !isValid( msg ) )
    return false;
  else
  {
    try
    {
      double x1, y1, z1, x2, y2, z2;
      theDistance = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() )->GetMinDistance(
        myObj, myObj2, x1, y1, z1, x2, y2, z2 );

      thePnt1.SetCoord( x1, y1, z1 );
      thePnt2.SetCoord( x2, y2, z2 );
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::SetEditCurrentArgument()
{
  QPushButton* send = ( QPushButton* )sender();

  if( send == mySelBtn )
  {
    mySelEdit->setFocus();
    myEditCurrentArgument = mySelEdit;
  }
  else
  {
    mySelEdit2->setFocus();
    myEditCurrentArgument = mySelEdit2;
  }

  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::LineEditReturnPressed()
{
  QLineEdit* send = ( QLineEdit* )sender();

  if( send == mySelEdit )
    myEditCurrentArgument = mySelEdit;
  else
    myEditCurrentArgument = mySelEdit2;

  if ( GEOMBase::SelectionByNameInDialogs( this, mySelEdit->text(), selectedIO() ) )
    mySelEdit->setText( mySelEdit->text() );
}


//=================================================================================
// function : buildPrs()
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_DistanceDlg::buildPrs()
{
  double aDist = 0.;
  gp_Pnt aPnt1( 0, 0, 0 ), aPnt2( 0, 0, 0 );
  
  if ( myObj->_is_nil() || myObj2->_is_nil() || !getParameters( aDist, aPnt1, aPnt2 ) ||
       SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
       != OCCViewer_Viewer::Type() )
    return 0;
  
  try
  {
    if( aDist <= 1.e-9 )
    {
      BRepBuilderAPI_MakeVertex aMaker( aPnt1 );
      return getDisplayer()->BuildPrs( aMaker.Vertex() );
    }
    else
    {
      BRepBuilderAPI_MakeEdge MakeEdge( aPnt1, aPnt2 );
      TopoDS_Vertex aVert1 = BRepBuilderAPI_MakeVertex( aPnt1 );
      TopoDS_Vertex aVert2 = BRepBuilderAPI_MakeVertex( aPnt2 );

      QString aLabel;
      aLabel.sprintf( "%.1f", aDist );

      gp_Pnt aPnt3( ( aPnt1.X() + aPnt2.X() ) / 2,
                    ( aPnt1.Y() + aPnt2.Y() ) / 2,
                    ( aPnt1.Z() + aPnt2.Z() ) / 2 + 100 );


      gp_Vec va( aPnt3, aPnt1 );
      gp_Vec vb( aPnt3, aPnt2 );

      if ( va.IsParallel( vb, Precision::Angular() ) )
      {
        aPnt3.SetY( ( aPnt1.Y() + aPnt2.Y() ) / 2 + 100 );
        aPnt3.SetZ( ( aPnt1.Z() + aPnt2.Z() ) / 2 );
      }
      
      gce_MakePln gce_MP( aPnt1, aPnt2, aPnt3 );
      Handle( Geom_Plane ) P = new Geom_Plane( gce_MP.Value() );

      Handle( AIS_LengthDimension ) anIO = new AIS_LengthDimension(
        aVert1, aVert2, P, aDist, TCollection_ExtendedString( (Standard_CString)aLabel.latin1() ) );

      SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
      SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>( ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs( 0 ) );
      
      //QAD_ViewFrame* vf = GEOM_Displayer::GetActiveView();
      //OCCViewer_Prs* aPrs = dynamic_cast<OCCViewer_Prs*>( vf->CreatePrs( 0 ) );

      if ( aPrs )
        aPrs->AddObject( anIO );

      return aPrs;
    }
  }
  catch( Standard_Failure )
  {
    return 0;
  }
}

//=================================================================================
// function : isValid()
// purpose  :
//=================================================================================
bool MeasureGUI_DistanceDlg::isValid( QString& msg )
{
  return MeasureGUI_Skeleton::isValid( msg ) && !myObj2->_is_nil();
}














