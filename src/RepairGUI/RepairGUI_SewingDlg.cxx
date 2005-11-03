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
//  File   : RepairGUI_SewingDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_SewingDlg.h"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>

#define DEFAULT_TOLERANCE_VALUE 1e-07

using namespace std;

//=================================================================================
// class    : RepairGUI_SewingDlg()
// purpose  : Constructs a RepairGUI_SewingDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SewingDlg::RepairGUI_SewingDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SEWING")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SEWING_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SEWING_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_SEWING"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  QGridLayout* aLay = new QGridLayout( 0, 2, 2, 0, 6, "aLay" );
  myTolEdt = new QtxDblSpinBox( 0, 100, 1e-7, GroupPoints->GroupBox1 );//QAD_SpinBoxDbl( GroupPoints->GroupBox1, 0, 100, 1e-7, 10, 1e-10 );
  myTolEdt->setPrecision( 10 );
  myTolEdt->setValue( DEFAULT_TOLERANCE_VALUE );
  QLabel* aLbl1 = new QLabel( tr( "GEOM_TOLERANCE" ), GroupPoints->GroupBox1 );
	myFreeBoundBtn = new QPushButton( tr( "GEOM_DETECT" ), GroupPoints->GroupBox1 );
  QLabel* aLbl2 = new QLabel( tr( "GEOM_FREE_BOUNDARIES" ), GroupPoints->GroupBox1 );
  aLay->addWidget( aLbl1, 0, 0 );
  aLay->addWidget( myTolEdt, 0, 1 );
  aLay->addWidget( aLbl2, 1, 0 );
  aLay->addWidget( myFreeBoundBtn, 1, 1 );

  GroupPoints->getGroupBoxLayout()->addLayout( aLay, 3, 0 );
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_SewingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SewingDlg::~RepairGUI_SewingDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  initSelection();

  myClosed = -1;
  myOpen = -1;
	
  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(myFreeBoundBtn, SIGNAL(clicked()), this, SLOT(onDetect()));

  initName( tr( "SEWING_NEW_OBJ_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SewingDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::ClickOnApply()
{
  if ( !onAccept() )
  	return false;

  initName();

  GroupPoints->LineEdit1->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  initSelection();

  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_SewingDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  if ( IObjectCount() == 1 )
  {
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    Standard_Boolean aRes;
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( aRes )
      myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 )
  {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if( send == GroupPoints->LineEdit1 )
  {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::DeactivateActiveDialog()
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  GroupPoints->LineEdit1->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  myClosed = -1;
  myOpen = -1;

  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SewingDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_SewingDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::isValid( QString& msg )
{
  myClosed = -1;
  return !myObject->_is_nil() && ( IsPreview() || myTolEdt->value() > 0. );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  if ( IsPreview() ) // called from onDetect(): detect free boundary edges, highlight them (add to objects), display message dialog
  {
    GEOM::ListOfGO_var aClosed, anOpen;

    aResult = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->GetFreeBoundary( myObject, aClosed, anOpen );

    if ( aResult )
    {
      myClosed = aClosed->length();
      myOpen = anOpen->length();
      int i;
      for ( i = 0; i < myClosed; i++ )
	objects.push_back( aClosed[i]._retn() );
      for ( i = 0; i < myOpen; i++ )
	objects.push_back( anOpen[i]._retn() );
    }
    else
      myClosed = -1;
  }
  else
  {
    GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->Sew( myObject, myTolEdt->value() );
    aResult = !anObj->_is_nil();
    if ( aResult )
      objects.push_back( anObj._retn() );
  }

  return aResult;
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::initSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add( GEOM_SHELL );
  aTypes.Add( GEOM_SOLID );
  aTypes.Add( GEOM_COMPOUND );
  globalSelection( aTypes );
}

//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::onDetect()
{
  displayPreview( false, true, true, 3 );

  // field myClosed,myOpen is initialized in execute() method, called by displayPreview().
  QString msg;
  if ( myClosed != -1 )
    msg = tr( "GEOM_FREE_BOUNDS_MSG" ).arg( myClosed + myOpen ).arg( myClosed ).arg( myOpen );
  else
    msg = tr( "GEOM_FREE_BOUNDS_ERROR" );
  SUIT_MessageBox::info1( this, tr( "GEOM_FREE_BOUNDS_TLT" ), msg, "Close" );
}
