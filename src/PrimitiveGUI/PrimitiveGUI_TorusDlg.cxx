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
//  File   : PrimitiveGUI_TorusDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "PrimitiveGUI_TorusDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : PrimitiveGUI_TorusDlg()
// purpose  : Constructs a PrimitiveGUI_TorusDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_TorusDlg::PrimitiveGUI_TorusDlg(GeometryGUI* theGeometryGUI, QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu), myGeometryGUI(theGeometryGUI)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_TORUS_PV")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_TORUS_DXYZ")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
    
  setCaption(tr("GEOM_TORUS_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_TORUS"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_POINT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS_I").arg("1"));
  GroupPoints->TextLabel4->setText(tr("GEOM_RADIUS_I").arg("2"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_2Spin(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_RADIUS_I").arg("1"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_RADIUS_I").arg("2"));

  Layout1->addWidget(GroupPoints, 2, 0);
  Layout1->addWidget(GroupDimensions, 2, 0);
  /***************************************************************/
  
  Init();
}


//=================================================================================
// function : ~PrimitiveGUI_TorusDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_TorusDlg::~PrimitiveGUI_TorusDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_TorusDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
 
  myPoint = myDir = GEOM::GEOM_Object::_nil();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupDimensions->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  GroupDimensions->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, step, 3);

  GroupPoints->SpinBox_DX->SetValue(300.0);
  GroupPoints->SpinBox_DY->SetValue(100.0);
  GroupDimensions->SpinBox_DX->SetValue(300.0);
  GroupDimensions->SpinBox_DY->SetValue(100.0);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY, SLOT(SetStep(double)));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  
  initName( tr( "GEOM_TORUS" ) );
  ConstructorsClicked(0);
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_TorusDlg::ConstructorsClicked(int constructorId)
{
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  
  switch(constructorId)
    { 
    case 0 :
      {
	globalSelection( GEOM_LINE );

	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();
	
	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText(tr(""));
	GroupPoints->LineEdit2->setText(tr(""));
	myPoint = myDir = GEOM::GEOM_Object::_nil();
	
	connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
		SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break ;
      }
    case 1 :
      {
	GroupPoints->hide();
	resize( 0, 0 );
	GroupDimensions->show();
	
	break ;
      }
    }
  displayPreview();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_TorusDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_TorusDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void PrimitiveGUI_TorusDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void PrimitiveGUI_TorusDlg::SelectionIntoArgument()
{
  if ( getConstructorId() != 0 )
    return;
  
  myEditCurrentArgument->setText("");
  
  if(IObjectCount() != 1)
    {
      if(myEditCurrentArgument == GroupPoints->LineEdit1)
	myPoint = GEOM::GEOM_Object::_nil();
      else if(myEditCurrentArgument == GroupPoints->LineEdit2)
	myDir = GEOM::GEOM_Object::_nil();
      return;
    }
  
  
  /* nbSel == 1 */
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
  
  if(!testResult || CORBA::is_nil( aSelectedObject ))
    return;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1)
    myPoint = aSelectedObject;
  else if(myEditCurrentArgument == GroupPoints->LineEdit2)
    myDir = aSelectedObject;
    
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_TorusDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection( GEOM_POINT );
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
void PrimitiveGUI_TorusDlg::LineEditReturnPressed()
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
void PrimitiveGUI_TorusDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
 
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void PrimitiveGUI_TorusDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void PrimitiveGUI_TorusDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox
// purpose  :
//=================================================================================
void PrimitiveGUI_TorusDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_TorusDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_TorusDlg::isValid( QString& msg )
{
  return getConstructorId() == 0 ? !(myPoint->_is_nil() || myDir->_is_nil()) : true;
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_TorusDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) 
  {
  case 0 :
  {
    if (!CORBA::is_nil( myPoint ) && !CORBA::is_nil( myDir )){
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeTorusPntVecRR(myPoint, myDir, getRadius1(), getRadius2());
      res = true;
    }
    break;
  }
  case 1 :
    {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeTorusRR(getRadius1(), getRadius2());
      res = true;
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
void PrimitiveGUI_TorusDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : getRadius1()
// purpose  :
//=================================================================================
double PrimitiveGUI_TorusDlg::getRadius1() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DX->GetValue();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DX->GetValue();
  return 0;
}


//=================================================================================
// function : getRadius2()
// purpose  :
//=================================================================================
double PrimitiveGUI_TorusDlg::getRadius2() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DY->GetValue();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DY->GetValue();
  return 0;
}
