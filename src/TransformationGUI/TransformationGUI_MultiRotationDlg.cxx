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
//  File   : TransformationGUI_MultiRotationDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "TransformationGUI_MultiRotationDlg.h"

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
// class    : TransformationGUI_MultiRotationDlg()
// purpose  : Constructs a TransformationGUI_MultiRotationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiRotationDlg::TransformationGUI_MultiRotationDlg
  (GeometryGUI* theGeometryGUI, QWidget* parent,  const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0(aResMgr->loadPixmap("GEOM",tr("ICON_DLG_MULTIROTATION_SIMPLE")));
  QPixmap image1(aResMgr->loadPixmap("GEOM",tr("ICON_DLG_MULTIROTATION_DOUBLE")));
  QPixmap image2(aResMgr->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_MULTIROTATION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_MULTIROTATION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_SIMPLE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_NB_TIMES"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupDimensions = new DlgRef_2Sel4Spin1Check(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_DOUBLE"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupDimensions->TextLabel4->setText(tr("GEOM_NB_TIMES"));
  GroupDimensions->TextLabel5->setText(tr("GEOM_STEP"));
  GroupDimensions->TextLabel6->setText(tr("GEOM_NB_TIMES"));
  GroupDimensions->CheckButton1->setText(tr("GEOM_REVERSE"));
  GroupDimensions->PushButton1->setPixmap(image2);
  GroupDimensions->PushButton2->setPixmap(image2);
  GroupDimensions->LineEdit1->setReadOnly(true);
  GroupDimensions->LineEdit2->setReadOnly(true);

  Layout1->addWidget(GroupPoints, 2, 0);
  Layout1->addWidget(GroupDimensions, 2, 0);
  /***************************************************************/

  setHelpFileName("multi_rotation.htm");

  Init();
}


//=================================================================================
// function : ~TransformationGUI_MultiRotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiRotationDlg::~TransformationGUI_MultiRotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::Init()
{
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  double SpecificStep1 = 5;
  double SpecificStep2 = 1;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupPoints->SpinBox_DX->SetValue(myNbTimes1);

  GroupDimensions->SpinBox_DX1->RangeStepAndValidator(-999.999, 999.999, SpecificStep1, 3);
  GroupDimensions->SpinBox_DY1->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupDimensions->SpinBox_DX2->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DY2->RangeStepAndValidator(1.0, 999.999, SpecificStep2, 3);
  GroupDimensions->SpinBox_DX1->SetValue(myAng);
  GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
  GroupDimensions->SpinBox_DX2->SetValue(myStep);
  GroupDimensions->SpinBox_DY2->SetValue(myNbTimes2);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupDimensions->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DX2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX2, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY2, SLOT(SetStep(double)));

  connect(GroupDimensions->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(ReverseAngle()));
  
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_MULTIROTATION" ) );
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiRotationDlg::ConstructorsClicked(int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  myAng = 45.0;
  myStep = 50.0;
  myNbTimes1 = myNbTimes2 = 2;

  globalSelection( GEOM_ALLSHAPES );

  switch (constructorId)
    {
    case 0: /* Rotate simple */
      {
	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();

	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText("");
 	GroupPoints->LineEdit2->setText("");

	GroupPoints->SpinBox_DX->SetValue(myNbTimes1);

	break;
      }
    case 1: /* Rotate double */
      {
	GroupPoints->hide();
	resize(0, 0);
	GroupDimensions->show();

	myEditCurrentArgument = GroupDimensions->LineEdit1;
	GroupDimensions->LineEdit1->setText("");
 	GroupDimensions->LineEdit2->setText("");

	GroupDimensions->SpinBox_DX1->SetValue(myAng);
	GroupDimensions->SpinBox_DY1->SetValue(myNbTimes1);
	GroupDimensions->SpinBox_DX2->SetValue(myStep);
	GroupDimensions->SpinBox_DY2->SetValue(myNbTimes2);

	break;
      }      
    }

  myEditCurrentArgument->setFocus();
  myBase = myVector = GEOM::GEOM_Object::_nil();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_MultiRotationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if (IObjectCount() != 1) {
    if (myEditCurrentArgument == GroupPoints->LineEdit1 ||
        myEditCurrentArgument == GroupDimensions->LineEdit1)
      myBase = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2 ||
             myEditCurrentArgument == GroupDimensions->LineEdit2)
      myVector = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );

  if (!testResult || CORBA::is_nil(aSelectedObject) || !GEOMBase::IsShape(aSelectedObject))
    return;

  if (myEditCurrentArgument == GroupPoints->LineEdit1 ||
      myEditCurrentArgument == GroupDimensions->LineEdit1)
    myBase = aSelectedObject;
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 ||
           myEditCurrentArgument == GroupDimensions->LineEdit2)
    myVector = aSelectedObject;

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection( GEOM_ALLSHAPES );
  }
  else if(send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection( GEOM_LINE  );
  }
  else if(send == GroupDimensions->PushButton1) {
    myEditCurrentArgument = GroupDimensions->LineEdit1;
    globalSelection( GEOM_ALLSHAPES );
  }
  else if(send == GroupDimensions->PushButton2) {
    myEditCurrentArgument = GroupDimensions->LineEdit2;
    globalSelection( GEOM_LINE  );
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1 || send == GroupDimensions->LineEdit1 || 
     send == GroupPoints->LineEdit2 || send == GroupDimensions->LineEdit2)
    {
      myEditCurrentArgument = send; 
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ActivateThisDialog()
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
void TransformationGUI_MultiRotationDlg::enterEvent(QEvent* e)
{
  if(!GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_DX || send == GroupDimensions->SpinBox_DY1)
    myNbTimes1 = (int)newValue;
  else if(send == GroupDimensions->SpinBox_DX1)
    myAng = newValue;
  else if(send == GroupDimensions->SpinBox_DX2)
    myStep = newValue;
  else if(send == GroupDimensions->SpinBox_DY2)
    myNbTimes2 = (int)newValue;
  
  displayPreview();
}


//=================================================================================
// function : ReverseAngle()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiRotationDlg::ReverseAngle()
{
  myAng = -myAng;

  int aConstructorId = getConstructorId();

  if(aConstructorId == 0)
    GroupPoints->SpinBox_DX->SetValue(myAng);
  else if(aConstructorId == 1)
    GroupDimensions->SpinBox_DX1->SetValue(myAng);

  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_MultiRotationDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::isValid( QString& msg )
{
  return !(myBase->_is_nil() || myVector->_is_nil());
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) 
    {
    case 0 :
      {
	if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVector ) ) {
	  anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->
            MultiRotate1D( myBase, myVector, myNbTimes1 );
	  res = true;
	}
	break;
      }
    case 1 :
      {
	if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVector ) )
	  {
	    anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->
              MultiRotate2D( myBase, myVector, myAng, myNbTimes1, myStep, myNbTimes2 );
	    res = true;
	  }
	break;
      }
    }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}


//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void  TransformationGUI_MultiRotationDlg::closeEvent( QCloseEvent* e )
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}
