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
//  File   : RepairGUI_RemoveIntWiresDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_RemoveIntWiresDlg.h"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>

using namespace std;

//=================================================================================
// class    : RepairGUI_RemoveIntWiresDlg()
// purpose  : Constructs a RepairGUI_RemoveIntWiresDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_RemoveIntWiresDlg::RepairGUI_RemoveIntWiresDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SUPPRESS_INT_WIRES")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_REMOVE_INTERNAL_WIRES_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_REMOVE_INTERNAL_WIRES_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_Ext(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_INTERNAL_WIRES"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_FACE"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  myAllChk = new QCheckBox( tr( "GEOM_REMOVE_ALL_INT_WIRES" ), GroupPoints->GroupBox1 );

  QGridLayout* aSelectWiresLay = new QGridLayout( 0, 1, 1, 0, 6, "aSelectWiresLay");

  mySelectWiresBtn = new QPushButton( GroupPoints->GroupBox1, "mySelectWiresBtn" );
  mySelectWiresBtn->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, mySelectWiresBtn->sizePolicy().hasHeightForWidth() ) );
  mySelectWiresBtn->setText( trUtf8( "" ) );
  mySelectWiresBtn->setPixmap(image1);
  
  mySelectWiresEdt = new QLineEdit( GroupPoints->GroupBox1, "mySelectWiresEdt" );
  mySelectWiresEdt->setReadOnly( true );

  mySelectWiresLbl = new QLabel( tr( "GEOM_WIRES_TO_REMOVE" ), GroupPoints->GroupBox1, "TextLabel1" );
  mySelectWiresLbl->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, mySelectWiresLbl->sizePolicy().hasHeightForWidth() ) );

  aSelectWiresLay->addWidget( mySelectWiresLbl, 0, 0 );
	aSelectWiresLay->addWidget( mySelectWiresBtn, 0, 1 );  
  aSelectWiresLay->addWidget( mySelectWiresEdt, 0, 2 );
  QSpacerItem* spacer = new QSpacerItem( 0, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  aSelectWiresLay->addItem( spacer, 1, 2 );

  GroupPoints->getGroupBoxLayout()->addMultiCellWidget( myAllChk, 1, 1, 0, 2 );
  GroupPoints->getGroupBoxLayout()->addLayout( aSelectWiresLay, 2, 0 );  
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_RemoveIntWiresDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_RemoveIntWiresDlg::~RepairGUI_RemoveIntWiresDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();
	myWiresInd = new GEOM::short_array();
	myWiresInd->length( 0 );

  //myGeomGUI->SetState( 0 );
  initSelection(); 

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(mySelectWiresBtn, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(mySelectWiresEdt, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect( myAllChk, SIGNAL( clicked() ), this, SLOT( onRemoveAllClicked() ) );

  initName( tr( "REMOVE_INT_WIRES_NEW_OBJ_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_RemoveIntWiresDlg::ClickOnApply()
{
  if ( !onAccept() )
  	return false;

  initName();

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText("");
  mySelectWiresEdt->setText("");
  myObject = GEOM::GEOM_Object::_nil();
	myWiresInd->length( 0 );

	initSelection();

  return true;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myObject = GEOM::GEOM_Object::_nil();
  else if ( myEditCurrentArgument == mySelectWiresEdt ) myWiresInd->length( 0 );

  if ( IObjectCount() == 1 )
    {
      Handle(SALOME_InteractiveObject) anIO = firstIObject();
      
      if ( myEditCurrentArgument == GroupPoints->LineEdit1 )	// face selection
	{
	  Standard_Boolean aRes;
	  myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
	  if ( aRes && GEOMBase::IsShape( myObject ) )
	    myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
	  else
	    myObject = GEOM::GEOM_Object::_nil();
	}
      else if ( myEditCurrentArgument == mySelectWiresEdt && !myAllChk->isChecked() )
	{
	  TColStd_IndexedMapOfInteger aMap;
	  ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr()->GetIndexes( anIO, aMap );
	  const int n = aMap.Extent();
	  myWiresInd->length( n );
	  for ( int i = 1; i <= n; i++ )
	    myWiresInd[i-1] = aMap( i );
	  if ( n )
	    myEditCurrentArgument->setText( QString::number( n ) + "_" + tr( "GEOM_WIRE" ) + tr( "_S_" ) );
	}
    }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::SetEditCurrentArgument()
{
	const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 )
  	myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == mySelectWiresBtn && !myObject->_is_nil() )
  	myEditCurrentArgument = mySelectWiresEdt;

  if ( myEditCurrentArgument )
  {
  	initSelection();
  	myEditCurrentArgument->setFocus();
  	SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::LineEditReturnPressed()
{
	const QObject* send = sender();
  if( send == GroupPoints->LineEdit1 || send == mySelectWiresEdt )
  {
    myEditCurrentArgument = (QLineEdit*)send;
	  GEOMBase_Skeleton::LineEditReturnPressed();
	}
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::DeactivateActiveDialog()
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText("");
  mySelectWiresEdt->setText("");
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd->length( 0 );

  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_RemoveIntWiresDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_RemoveIntWiresDlg::isValid( QString& msg )
{
	return !myObject->_is_nil() && ( myAllChk->isChecked() || myWiresInd->length() );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_RemoveIntWiresDlg::execute( ObjectList& objects )
{
	GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->RemoveIntWires( myObject, myWiresInd );
	bool aResult = !anObj->_is_nil();
	if ( aResult )
		objects.push_back( anObj._retn() );

	return aResult;
}

//=================================================================================
// function : onRemoveAllClicked
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::onRemoveAllClicked()
{
	bool b = myAllChk->isOn();
  mySelectWiresLbl->setEnabled( !b );
  mySelectWiresBtn->setEnabled( !b );
  mySelectWiresEdt->setEnabled( !b );
  if ( b )
  {
    mySelectWiresEdt->setText( "" );
    myWiresInd->length( 0 );
  }
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_RemoveIntWiresDlg::initSelection()
{
  if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
  {
  	TColStd_MapOfInteger aTypes;
	aTypes.Add( GEOM_COMPOUND );
	aTypes.Add( GEOM_SOLID );
	aTypes.Add( GEOM_SHELL );
	aTypes.Add( GEOM_FACE );
  	
	globalSelection( aTypes );
  }
  else if ( myEditCurrentArgument == mySelectWiresEdt )
  {
    //localSelection( myObject, TopAbs_EDGE );
    localSelection( myObject, TopAbs_WIRE );
  }
}


