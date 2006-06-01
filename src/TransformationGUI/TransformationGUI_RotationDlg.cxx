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
//  File   : TransformationGUI_RotationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_RotationDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qcheckbox.h>
#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_RotationDlg()
// purpose  : Constructs a TransformationGUI_RotationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_RotationDlg::TransformationGUI_RotationDlg
  (GeometryGUI* theGeometryGUI, QWidget* parent,  const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_ROTATION")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ROTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ROTATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin2Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  GroupPoints->CheckButton2->setText(tr("GEOM_REVERSE"));

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/
  double anAngle = 0;
  double SpecificStep = 5;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(anAngle);

  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged(true);
  
  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  
  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  connect(GroupPoints->CheckButton2, SIGNAL(toggled(bool)), this, SLOT(onReverse()));
  
  connect(myGeomGUI->getApp()->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  setHelpFileName("rotation.htm");

  Init();
}


//=================================================================================
// function : ~TransformationGUI_RotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_RotationDlg::~TransformationGUI_RotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit2->clear();
  
  myAxis = GEOM::GEOM_Object::_nil();
  
  initName( tr( "GEOM_ROTATION" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::ClickOnApply()
{
  if ( !onAccept( GroupPoints->CheckButton1->isChecked()) )
    return false;
  
  Init();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_RotationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1)
    {
      int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
      if(aNbSel < 1)
	{
	  myObjects.length(0);
	  return;
	}
      GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myObjects);
      if (!myObjects.length())
	return;
    }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2)
    {
      if(IObjectCount() != 1)
	{
	  myAxis = GEOM::GEOM_Object::_nil();
	  return;
	}
      Standard_Boolean testResult = Standard_False;
      myAxis = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
      if(!testResult || CORBA::is_nil( myAxis ))
	return;
      aName = GEOMBase::GetName( myAxis );
    }
  myEditCurrentArgument->setText( aName );
  
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection();
  }
  else if(send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection( GEOM_LINE );
  }
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::LineEditReturnPressed()
{  
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1 ||
     send == GroupPoints->LineEdit2)
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  globalSelection();
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit2->clear();
  myAxis = GEOM::GEOM_Object::_nil();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::enterEvent(QEvent* e)
{
  if (!GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_RotationDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::isValid( QString& msg )
{
  return !(myObjects.length() == 0 || myAxis->_is_nil());
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  if (GroupPoints->CheckButton1->isChecked() || IsPreview())
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->RotateCopy( myObjects[i], myAxis, GetAngle() * PI180 );
	if ( !anObj->_is_nil() )
	  objects.push_back( anObj._retn() );
      }
  else
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->Rotate( myObjects[i], myAxis, GetAngle() * PI180 );
	if ( !anObj->_is_nil() )
	  objects.push_back( anObj._retn() );
      }
  res = true;
  
  return res;
}


//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : GetAngle()
// purpose  :
//=================================================================================
double TransformationGUI_RotationDlg::GetAngle() const
{
  return GroupPoints->SpinBox_DX->GetValue();
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  this->GroupBoxName->setEnabled(isCreateCopy);
}


//=================================================================================
// function :  onReverse()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->GetValue();
  GroupPoints->SpinBox_DX->SetValue( -anOldValue );
}
