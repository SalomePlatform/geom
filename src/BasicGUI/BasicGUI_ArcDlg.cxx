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
// File   : BasicGUI_ArcDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#include "BasicGUI_ArcDlg.h"

#include <GEOM_DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_ArcDlg()
// purpose  : Constructs a BasicGUI_ArcDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_ArcDlg::BasicGUI_ArcDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
				  bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_ARC" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_ARC_CENTER" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_ARC_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_ARC" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  Group3Pnts = new DlgRef_3Sel( centralWidget() );

  Group3Pnts->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  Group3Pnts->TextLabel1->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  Group3Pnts->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );
  Group3Pnts->TextLabel3->setText( tr( "GEOM_POINT_I" ).arg( 3 ) );

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );

  Group3Pnts->PushButton1->setIcon(image2);
  Group3Pnts->PushButton2->setIcon(image2);
  Group3Pnts->PushButton3->setIcon(image2);


  Group3Pnts2 = new DlgRef_3Sel1Check( centralWidget() );

  Group3Pnts2->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  Group3Pnts2->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) );
  Group3Pnts2->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( "Start" ) );
  Group3Pnts2->TextLabel3->setText( tr( "GEOM_POINT_I" ).arg( "End" ) );

  Group3Pnts2->LineEdit1->setReadOnly( true );
  Group3Pnts2->LineEdit2->setReadOnly( true );
  Group3Pnts2->LineEdit3->setReadOnly( true );

  Group3Pnts2->PushButton1->setIcon( image2 );
  Group3Pnts2->PushButton2->setIcon( image2 );
  Group3Pnts2->PushButton3->setIcon( image2 );
  
  Group3Pnts2->CheckButton1->setText( tr( "GEOM_REVERSE" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( Group3Pnts );
  layout->addWidget( Group3Pnts2 );

  setHelpFileName( "arc.htm" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_ArcDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_ArcDlg::~BasicGUI_ArcDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::Init()
{
  /* init variables */
  // myGeomGUI->SetState( 0 );
  globalSelection( GEOM_POINT );
  
  myConstructorId = -1;
  myEditCurrentArgument = Group3Pnts->LineEdit1;
  myEditCurrentArgument->setFocus();

  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
  Group3Pnts2->CheckButton1->setChecked( false );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
 
  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );
 
  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( Group3Pnts->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( Group3Pnts->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts->LineEdit3, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  
  connect( Group3Pnts2->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts2->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts2->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( Group3Pnts2->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts2->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts2->LineEdit3, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  
  connect( Group3Pnts2->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( ReverseSense() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_ARC" ) ); 
  

  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  // reset
  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
  Group3Pnts->LineEdit1->setText( "" );
  Group3Pnts->LineEdit2->setText( "" );
  Group3Pnts->LineEdit3->setText( "" );
  Group3Pnts2->LineEdit1->setText( "" );
  Group3Pnts2->LineEdit2->setText( "" );
  Group3Pnts2->LineEdit3->setText( "" );
  if ( getConstructorId() == 0 )  myEditCurrentArgument = Group3Pnts->LineEdit1;
  if ( getConstructorId() == 1 )  myEditCurrentArgument = Group3Pnts2->LineEdit1;

  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_ArcDlg::SelectionIntoArgument()
{
  if ( getConstructorId() != 0 && getConstructorId() != 1 )
    return;
  
  myEditCurrentArgument->setText( "" );
  if ( IObjectCount() != 1 ) {
    switch ( getConstructorId() ) {
    case 0:
      if      ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = GEOM::GEOM_Object::_nil();
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2 = GEOM::GEOM_Object::_nil();
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3 = GEOM::GEOM_Object::_nil();
      return;
      break;
    case 1:
      if      ( myEditCurrentArgument == Group3Pnts2->LineEdit1 )   myPoint1 = GEOM::GEOM_Object::_nil();
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit2 )   myPoint2 = GEOM::GEOM_Object::_nil();
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit3 )   myPoint3 = GEOM::GEOM_Object::_nil();
      return;
      break;
    }
  }   
  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
    switch ( getConstructorId() ) {
    case 0:
      myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
      if      ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = aSelectedObject;
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2 = aSelectedObject;
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3 = aSelectedObject;
      break;
    case 1:
      myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
      if      ( myEditCurrentArgument == Group3Pnts2->LineEdit1 )   myPoint1 = aSelectedObject;
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit2 )   myPoint2 = aSelectedObject;
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit3 )   myPoint3 = aSelectedObject;
      break;
    }
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch ( getConstructorId() ) {
  case 0:
    if      ( send == Group3Pnts->PushButton1 )   myEditCurrentArgument = Group3Pnts->LineEdit1;
    else if ( send == Group3Pnts->PushButton2 )   myEditCurrentArgument = Group3Pnts->LineEdit2;
    else if ( send == Group3Pnts->PushButton3 )   myEditCurrentArgument = Group3Pnts->LineEdit3;
    break;
  case 1:
    if      ( send == Group3Pnts2->PushButton1 )   myEditCurrentArgument = Group3Pnts2->LineEdit1;
    else if ( send == Group3Pnts2->PushButton2 )   myEditCurrentArgument = Group3Pnts2->LineEdit2;
    else if ( send == Group3Pnts2->PushButton3 )   myEditCurrentArgument = Group3Pnts2->LineEdit3;
    break;
  }
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == Group3Pnts->LineEdit1 || send == Group3Pnts->LineEdit2 || send == Group3Pnts->LineEdit3 ||
       send == Group3Pnts2->LineEdit1 || send == Group3Pnts2->LineEdit2 || send == Group3Pnts2->LineEdit3 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  // myGeomGUI->SetState( 0 );
  globalSelection( GEOM_POINT );

  myEditCurrentArgument = Group3Pnts->LineEdit1;
  myEditCurrentArgument->setFocus();

  Group3Pnts->LineEdit1->setText( "" );
  Group3Pnts->LineEdit2->setText( "" );
  Group3Pnts->LineEdit3->setText( "" );
  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  ConstructorsClicked( getConstructorId() );
  
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_ArcDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_ArcDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isEqual
// purpose  : it may also be needed to check for min distance between gp_Pnt-s...
//=================================================================================
static bool isEqual( const GEOM::GEOM_Object_var& thePnt1, const GEOM::GEOM_Object_var& thePnt2 )
{
  return thePnt1->_is_equivalent( thePnt2 );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::isValid( QString& msg )
{
  return !myPoint1->_is_nil() && !myPoint2->_is_nil() && !myPoint3->_is_nil() &&
    !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0:
    {
      if ( !CORBA::is_nil( myPoint1 ) && !CORBA::is_nil( myPoint2 ) && !CORBA::is_nil( myPoint3 ) ) {
	anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeArc( myPoint1, myPoint2, myPoint3 );
	res = true;
      }
      break;
    }
  case 1:
    {
      bool Sense = Group3Pnts2->CheckButton1->isChecked();
      if ( !CORBA::is_nil( myPoint1 ) && !CORBA::is_nil( myPoint2 ) && !CORBA::is_nil( myPoint3 ) ) {
	anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeArcCenter( myPoint1, myPoint2, myPoint3, Sense );
	res = true;
      }
      break;
    }
  }
  if ( !anObj->_is_nil() ) {
    objects.push_back( anObj._retn() );
  }
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_ArcDlg::ConstructorsClicked( int constructorId )
{
  disconnect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 0, this, 0 );

  switch ( constructorId ) {
  case 0:
    {
      Group3Pnts2->hide();
      Group3Pnts->show();

      connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	       SIGNAL(currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
      break;
    }
  case 1:
    {
      Group3Pnts->hide();
      Group3Pnts2->show();
      connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),          
	       SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
      
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  displayPreview();
}

//=================================================================================
// function : ReverseSense()
// purpose  : Orientation of the arc
//=================================================================================
void BasicGUI_ArcDlg::ReverseSense()
{
  displayPreview();
}
