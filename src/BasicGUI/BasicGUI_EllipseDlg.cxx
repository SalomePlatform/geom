// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE 
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
// File   : BasicGUI_EllipseDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//

#include "BasicGUI_EllipseDlg.h"

#include <GEOM_DlgRef.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_EllipseDlg()
// purpose  : Constructs a BasicGUI_EllipseDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_EllipseDlg::BasicGUI_EllipseDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
					  const char* name, bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, name, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_ELLIPSE_PV" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_ELLIPSE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_ELLIPSE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel2Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_CENTER" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_VECTOR" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_RADIUS_MAJOR" ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_RADIUS_MINOR" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );

  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  /***************************************************************/

  setHelpFileName( "ellipse.htm" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_EllipseDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_EllipseDlg::~BasicGUI_EllipseDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  globalSelection( GEOM_POINT );

  myPoint = myDir = GEOM::GEOM_Object::_nil();

  // myGeomGUI->SetState( 0 );

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  double aMajorR( 200. ), aMinorR( 100. );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, 0.001, COORD_MAX, step, 3 );
  initSpinBox( GroupPoints->SpinBox_DY, 0.001, COORD_MAX, step, 3 );
  GroupPoints->SpinBox_DX->setValue( aMajorR );
  GroupPoints->SpinBox_DY->setValue( aMinorR );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonCancel(), SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) );
  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  // VSR: TODO ->>
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupPoints->SpinBox_DX, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupPoints->SpinBox_DY, SLOT( SetStep( double ) ) );
  // <<-
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_ELLIPSE" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  // reset
  myPoint = myDir = GEOM::GEOM_Object::_nil();
  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  myEditCurrentArgument = GroupPoints->LineEdit1;
  globalSelection( GEOM_POINT );
  
  return true;
}

//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void BasicGUI_EllipseDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_EllipseDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  if ( IObjectCount() != 1 ) {
    if      ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myPoint = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) myDir   = GEOM::GEOM_Object::_nil();
    return;
  }

  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {  
    myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
    if      ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myPoint = aSelectedObject;
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) myDir   = aSelectedObject;
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      ( send == GroupPoints->PushButton1 ) myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == GroupPoints->PushButton2 ) myEditCurrentArgument = GroupPoints->LineEdit2;
  
  myEditCurrentArgument->setFocus();
  if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
    globalSelection( GEOM_LINE );
  else
    globalSelection( GEOM_POINT );
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );

  myPoint = myDir = GEOM::GEOM_Object::_nil();
  globalSelection( GEOM_POINT );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_EllipseDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_EllipseDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::isValid( QString& msg )
{
  double aMajorR = GroupPoints->SpinBox_DX->value();
  double aMinorR = GroupPoints->SpinBox_DY->value();
  if ( aMajorR < aMinorR ) {
    msg = tr( "GEOM_ELLIPSE_ERROR_1" );
    return false;
  }
  return !myPoint->_is_nil() && !myDir->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::execute( ObjectList& objects )
{
  double aMajorR = GroupPoints->SpinBox_DX->value();
  double aMinorR = GroupPoints->SpinBox_DY->value();
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeEllipse( myPoint, myDir, aMajorR, aMinorR );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}
