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
//  File   : TransformationGUI_TranslationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_TranslationDlg.h"

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
// class    : TransformationGUI_TranslationDlg()
// purpose  : Constructs a TransformationGUI_TranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_TranslationDlg::TransformationGUI_TranslationDlg
  (GeometryGUI* theGeometryGUI, QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_DXYZ")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_POINTS")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_VECTOR")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_TRANSLATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_TRANSLATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->setPixmap(image2);

  RadioButton1->setChecked(true);

  GroupPoints = new DlgRef_3Sel3Spin1Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->TextLabel4->setText(tr("GEOM_DX"));
  GroupPoints->TextLabel5->setText(tr("GEOM_DY"));
  GroupPoints->TextLabel6->setText(tr("GEOM_DZ"));
  GroupPoints->PushButton1->setPixmap(image3);
  GroupPoints->PushButton2->setPixmap(image3);
  GroupPoints->PushButton3->setPixmap(image3);
  GroupPoints->CheckBox1->setText(tr("GEOM_CREATE_COPY"));

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/
  
  setHelpFileName("translation.htm");
  
  Init();
}


//=================================================================================
// function : ~TransformationGUI_TranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_TranslationDlg::~TransformationGUI_TranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);
  
  myVector = myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
  
  // Activate Create a Copy mode
  GroupPoints->CheckBox1->setChecked(true);
  CreateCopyModeChanged(true);
  
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);
  
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox1->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, 3);
  GroupPoints->SpinBox2->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, 3);
  GroupPoints->SpinBox3->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, 3);
  
  GroupPoints->SpinBox1->SetValue(0.0);
  GroupPoints->SpinBox2->SetValue(0.0);
  GroupPoints->SpinBox3->SetValue(0.0);
  
  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed())); //@ Delete ?

  connect(GroupPoints->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox2, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox3, SLOT(SetStep(double)));
  
  connect(GroupPoints->CheckBox1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_TRANSLATION" ) );
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_TranslationDlg::ConstructorsClicked(int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  
  myEditCurrentArgument = GroupPoints->LineEdit1;
  globalSelection();

  switch (constructorId)
    {
    case 0: /* translation an object by dx, dy, dz */
      {	 
	GroupPoints->ShowRows(1,2,false);
	resize(0,0);
	GroupPoints->ShowRows(3,5,true);
	break;
      }
    case 1: /* translation an object by 2 points */
      {
	GroupPoints->ShowRows(3,5,false);
	resize(0,0);
	GroupPoints->ShowRows(0,2,true);
	GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("1"));
	GroupPoints->LineEdit2->clear();
	GroupPoints->LineEdit3->clear();
	myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
	break;
      } 
    case 2: /* translation an object by vector */
      {
	GroupPoints->ShowRows(2,5,false);
	resize(0,0);
	GroupPoints->ShowRows(0,1,true);
	GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
	GroupPoints->LineEdit2->clear();
	myVector = GEOM::GEOM_Object::_nil();
	break;
      }
    }
  
  myEditCurrentArgument->setFocus();
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_TranslationDlg::ClickOnApply()
{
  if ( !onAccept(GroupPoints->CheckBox1->isChecked()) )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_TranslationDlg::SelectionIntoArgument()
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
  else
    {
      if (IObjectCount() != 1) {
	if(myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 1)
	  myPoint1 = GEOM::GEOM_Object::_nil();
	else if(myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 2)
	  myVector = GEOM::GEOM_Object::_nil();
	else if(myEditCurrentArgument == GroupPoints->LineEdit3)
	  myPoint2 = GEOM::GEOM_Object::_nil();
	return;
      }
      
      Standard_Boolean testResult = Standard_False;;
      GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
      
      if (!testResult || CORBA::is_nil( aSelectedObject ))
	return;
      
      if(myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 1)
	myPoint1 = aSelectedObject;
      else if(myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 2)
	myVector = aSelectedObject;
      else if(myEditCurrentArgument == GroupPoints->LineEdit3)
	myPoint2 = aSelectedObject; 
      
      aName = GEOMBase::GetName( aSelectedObject );
    }
  
  myEditCurrentArgument->setText( aName );
  
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::SetEditCurrentArgument()
{    
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) 
    {
      myEditCurrentArgument = GroupPoints->LineEdit1;
      globalSelection();
    }
  else if (send == GroupPoints->PushButton2)
    {
      myEditCurrentArgument = GroupPoints->LineEdit2;
      getConstructorId() == 1 ? globalSelection( GEOM_POINT ) :
	                        globalSelection( GEOM_LINE  );
    }
  else if (send == GroupPoints->PushButton3)
    {
      myEditCurrentArgument = GroupPoints->LineEdit3;
      globalSelection( GEOM_POINT );
    }
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::enterEvent(QEvent* e)
{
  if(!GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_TranslationDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool  TransformationGUI_TranslationDlg::isValid( QString& msg )
{
  int aConstructorId = getConstructorId();
  
  switch (aConstructorId)
    {
    case 0: 
      {
	return !(myObjects.length() == 0 );
	break;
      }
    case 1: 
      {
	return !(myObjects.length() == 0 || myPoint1->_is_nil() || myPoint2->_is_nil() );
	break;
      }
    case 2: 
      {
	return !(myObjects.length() == 0 || myVector->_is_nil());
	break;
      } 
    default: return false;
    }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_TranslationDlg::execute( ObjectList& objects )
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckBox1->isChecked();
  
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) 
    {
    case 0 :
      {
	double dx = GroupPoints->SpinBox1->GetValue();
	double dy = GroupPoints->SpinBox2->GetValue();
	double dz = GroupPoints->SpinBox3->GetValue();
	
	if (toCreateCopy)
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateDXDYDZCopy( myObjects[i], dx, dy, dz );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateDXDYDZ( myObjects[i], dx, dy, dz );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	res = true;
	break;
      }
    case 1 :
      {
	if (toCreateCopy)
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateTwoPointsCopy( myObjects[i], myPoint1, myPoint2 );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateTwoPoints( myObjects[i], myPoint1, myPoint2 );	
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	res = true;
	break;
      }
    case 2:
      {
	if (toCreateCopy)
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateVectorCopy( myObjects[i], myVector );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->TranslateVector( myObjects[i], myVector );
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
void  TransformationGUI_TranslationDlg::closeEvent( QCloseEvent* e )
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  GroupBoxName->setEnabled(isCreateCopy);
}
