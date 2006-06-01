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
//  File   : TransformationGUI_ScaleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_ScaleDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>
#include <qcheckbox.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_ScaleDlg()
// purpose  : Constructs a TransformationGUI_ScaleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ScaleDlg::TransformationGUI_ScaleDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                                       const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SCALE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SCALE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SCALE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin2Check(this, "GroupPoints");
  GroupPoints->CheckButton2->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_CENTRAL_POINT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_SCALE_FACTOR"));
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));

  // san -- modification of an exisitng object by offset is not allowed
  GroupPoints->CheckButton1->hide();

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/
  double aFactor = 2.0;
  double SpecificStep = 0.5;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(aFactor);
  
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
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
   
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  setHelpFileName("scale_transform.htm");

  Init();
}


//=================================================================================
// function : ~TransformationGUI_ScaleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ScaleDlg::~TransformationGUI_ScaleDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::Init()
{
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit2->clear();
  
  myPoint = GEOM::GEOM_Object::_nil();
  
  initName( tr( "GEOM_SCALE" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::ClickOnApply()
{
  if ( !onAccept(GroupPoints->CheckButton1->isChecked()) )
    return false;

  Init();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_ScaleDlg::SelectionIntoArgument()
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
	  myPoint = GEOM::GEOM_Object::_nil();
	  return;
	}
      Standard_Boolean testResult = Standard_False;
      myPoint = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
      if(!testResult || CORBA::is_nil( myPoint ))
	return;
      aName = GEOMBase::GetName( myPoint );
    }
  myEditCurrentArgument->setText( aName );
  
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::LineEditReturnPressed()
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection();
  }
  else if(send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection( GEOM_POINT );
  }
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  globalSelection();
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::enterEvent(QEvent* e)
{
  if( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_ScaleDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::isValid( QString& msg )
{
  return !(myObjects.length() == 0 || myPoint->_is_nil() || fabs(GetFactor()) <= 0.00001);
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  if (GroupPoints->CheckButton1->isChecked() || IsPreview())
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->ScaleShapeCopy( myObjects[i], myPoint, GetFactor() );
	if ( !anObj->_is_nil() )
	  objects.push_back( anObj._retn() );
      }
  else
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->ScaleShape( myObjects[i], myPoint, GetFactor() );
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
void  TransformationGUI_ScaleDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : GetFactor()
// purpose  :
//=================================================================================
double TransformationGUI_ScaleDlg::GetFactor() const
{
  return GroupPoints->SpinBox_DX->GetValue();
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  this->GroupBoxName->setEnabled(isCreateCopy);
}
