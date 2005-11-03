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
//  File   : TransformationGUI_PositionDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "TransformationGUI_PositionDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>
#include <qcheckbox.h>

#include "TColStd_MapOfInteger.hxx"

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_PositionDlg()
// purpose  : Constructs a TransformationGUI_PositionDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_PositionDlg::TransformationGUI_PositionDlg(QWidget* parent,  const char* name, bool modal, WFlags fl)
    :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POSITION")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POSITION2")));
  QPixmap imageselect(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_POSITION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_POSITION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);
 
  Group1 = new DlgRef_3Sel3Spin1Check(this, "Group1");
  Group1->SpinBox1->hide();
  Group1->SpinBox2->hide();
  Group1->SpinBox3->hide();
  Group1->TextLabel4->hide();
  Group1->TextLabel5->hide();
  Group1->TextLabel6->hide();
  Group1->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group1->TextLabel1->setText(tr("GEOM_OBJECTS"));
  Group1->TextLabel2->setText(tr("GEOM_START_LCS"));
  Group1->TextLabel3->setText(tr("GEOM_END_LCS"));
  Group1->PushButton1->setPixmap(imageselect);
  Group1->PushButton2->setPixmap(imageselect);
  Group1->PushButton3->setPixmap(imageselect);
  Group1->CheckBox1->setText(tr("GEOM_CREATE_COPY"));

  Layout1->addWidget(Group1, 2, 0);
  
  /***************************************************************/
  
  Init();
}


//=================================================================================
// function : ~TransformationGUI_PositionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_PositionDlg::~TransformationGUI_PositionDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::Init()
{  
  /* init variables */
  Group1->LineEdit1->setReadOnly(true);
  Group1->LineEdit2->setReadOnly(true);
  Group1->LineEdit3->setReadOnly(true);
  
  myStartLCS = GEOM::GEOM_Object::_nil();
  myEndLCS = GEOM::GEOM_Object::_nil();
    
  // Activate Create a Copy mode
  Group1->CheckBox1->setChecked(true);
  CreateCopyModeChanged(true);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), SLOT(ConstructorsClicked(int)));

  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(Group1->CheckBox1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_POSITION" ) );
  ConstructorsClicked( 0 );
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_PositionDlg::ConstructorsClicked(int constructorId)
{
  disconnect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0 );
  
  globalSelection();
  myEditCurrentArgument = Group1->LineEdit1;
  Group1->LineEdit2->clear();
  Group1->LineEdit3->clear();
  myStartLCS = GEOM::GEOM_Object::_nil();
  myEndLCS = GEOM::GEOM_Object::_nil();
  
  switch (constructorId)
    {
    case 0:
      {	
	Group1->LineEdit2->hide();
	Group1->TextLabel2->hide();
	Group1->PushButton2->hide();
	break;
      }
    case 1:
      {
	Group1->LineEdit2->show();
	Group1->TextLabel2->show();
	Group1->PushButton2->show();
	break;
      }
    }
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  SelectionIntoArgument();
}



//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::ClickOnApply()
{
  if ( !onAccept(Group1->CheckBox1->isChecked()) )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void TransformationGUI_PositionDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void TransformationGUI_PositionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;

  if(myEditCurrentArgument == Group1->LineEdit1) {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
    if(aNbSel < 1) {
      myObjects.length(0);
      displayPreview();
      return;
    }
    GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myObjects);
    if (!myObjects.length()) {
      displayPreview();
      return;
    }
  }
  else if(myEditCurrentArgument == Group1->LineEdit2) {
    myStartLCS = GEOM::GEOM_Object::_nil();
    if(IObjectCount() != 1) {
      displayPreview();
      return;
    }

    Standard_Boolean testResult = Standard_False;
    myStartLCS = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
    if(!testResult || CORBA::is_nil( myStartLCS )) {
      displayPreview();
      return;
    }
    aName = GEOMBase::GetName( myStartLCS );
  }
  else if(myEditCurrentArgument == Group1->LineEdit3) {
    myEndLCS = GEOM::GEOM_Object::_nil();
    if(IObjectCount() != 1) {
      displayPreview();
      return;
    }
    
    Standard_Boolean testResult = Standard_False;
    myEndLCS = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
    if(!testResult || CORBA::is_nil( myEndLCS )) {
      displayPreview();
      return;
    }
    aName = GEOMBase::GetName( myEndLCS );
  }

  myEditCurrentArgument->setText( aName );
  displayPreview(); 
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == Group1->LineEdit1 || send == Group1->LineEdit2 || send == Group1->LineEdit3) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == Group1->PushButton1){
    myEditCurrentArgument = Group1->LineEdit1;
    globalSelection();
  }
  else if(send == Group1->PushButton2) {
    myEditCurrentArgument = Group1->LineEdit2;
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_PLANE );
    aMap.Add( GEOM_MARKER );
    globalSelection( aMap );
  }
  else if(send == Group1->PushButton3) {
    myEditCurrentArgument = Group1->LineEdit3;
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_PLANE );
    aMap.Add( GEOM_MARKER );
    globalSelection( aMap );
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void TransformationGUI_PositionDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_PositionDlg::enterEvent(QEvent * e)
{
  if(!GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_PositionDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool  TransformationGUI_PositionDlg::isValid( QString& msg )
{
  bool res;
  if (getConstructorId() == 0)
    res = !(myObjects.length() == 0 || myEndLCS->_is_nil());
  else
    res = !(myObjects.length() == 0 || myStartLCS->_is_nil() || myEndLCS->_is_nil());

  return res;
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool  TransformationGUI_PositionDlg::execute( ObjectList& objects )
{
  bool res = false;
  bool toCreateCopy = IsPreview() || Group1->CheckBox1->isChecked();
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() )
    {
    case 0 :
      {
	for (int i = 0; i < myObjects.length(); i++) {
	  if (toCreateCopy)
	    anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->PositionShapeCopy( myObjects[i], myObjects[i], myEndLCS );
	  else
	    anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->PositionShape( myObjects[i], myObjects[i], myEndLCS );

	  if ( !anObj->_is_nil() )
	    objects.push_back( anObj._retn() );
	}
	res = true;
	break;
      }
    case 1 :
      {
	for (int i = 0; i < myObjects.length(); i++) {
	  if (toCreateCopy)
	    anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->PositionShapeCopy( myObjects[i], myStartLCS, myEndLCS );
	  else
	    anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->PositionShape( myObjects[i], myStartLCS, myEndLCS );
	  if ( !anObj->_is_nil() )
	    objects.push_back( anObj._retn() );
	}
	res = true;
	break;
      }
    }
  
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void  TransformationGUI_PositionDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  this->GroupBoxName->setEnabled(isCreateCopy);
}
