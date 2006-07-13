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
//  File   : RepairGUI_ChangeOrientationDlg.cxx
//  Author : Sergey KUUL
//  Module : GEOM

#include "RepairGUI_ChangeOrientationDlg.h"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"

#include <TopAbs.hxx>

#include <qlabel.h>

using namespace std;

//=================================================================================
// class    : RepairGUI_ChangeOrientationDlg()
// purpose  : Constructs a RepairGUI_ChangeOrientationDlg  which is a child of 'parent',
//            with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_ChangeOrientationDlg::RepairGUI_ChangeOrientationDlg(GeometryGUI* theGeometryGUI,
                                                               QWidget* parent, const char* name,
                                                               bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  //QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_FACE")));
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_CHANGE_ORIENTATION")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
    
  setCaption(tr("GEOM_CHANGE_ORIENTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CHANGE_ORIENTATION_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel1Check_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_CHANGE_ORIENTATION"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  GroupPoints->PushButton1->setPixmap(image1);
//  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);

  setHelpFileName("change_orientation.htm");

  Init();
}


//=================================================================================
// function : ~RepairGUI_ChangeOrientationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_ChangeOrientationDlg::~RepairGUI_ChangeOrientationDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->CheckButton1->setChecked(true);

  myOkObject = false;

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "CHANGE_ORIENTATION_NEW_OBJ_NAME" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_ChangeOrientationDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::ClickOnApply()
{
//  if ( !onAccept() )
  if ( !onAccept(GroupPoints->CheckButton1->isChecked()) )
    return false;

  initName();

  myEditCurrentArgument->setText("");
  
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void RepairGUI_ChangeOrientationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    if (IObjectCount() != 1) {
      if (myEditCurrentArgument == GroupPoints->LineEdit1)
        myOkObject = false;
      return;
    }
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );

  if (!testResult)
    return;

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    myObject = aSelectedObject;
    myOkObject = true;
  }

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );

}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if( send == GroupPoints->PushButton1 )
  {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if( send == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_ChangeOrientationDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_ChangeOrientationDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::isValid( QString& msg )
{
  return myOkObject;
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::execute( ObjectList& objects )
{
  bool toCreateCopy = GroupPoints->CheckButton1->isChecked();

  GEOM::GEOM_Object_var anObj;
  if(toCreateCopy) {
    anObj = GEOM::GEOM_IHealingOperations::_narrow(getOperation())->ChangeOrientationCopy(myObject);
  }
  else {
    anObj = GEOM::GEOM_IHealingOperations::_narrow(getOperation())->ChangeOrientation(myObject);
  }

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  GroupBoxName->setEnabled(isCreateCopy);
}
