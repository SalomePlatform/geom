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
//  File   : PrimitiveGUI_CylinderDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "PrimitiveGUI_CylinderDlg.h"

#include "QAD_Config.h"
#include "QAD_Desktop.h"

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : PrimitiveGUI_CylinderDlg()
// purpose  : Constructs a PrimitiveGUI_CylinderDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_CylinderDlg::PrimitiveGUI_CylinderDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CYLINDER_PV")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CYLINDER_DXYZ")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CYLINDER_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CYLINDER"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_POINT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupPoints->TextLabel4->setText(tr("GEOM_HEIGHT"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);

  GroupDimensions = new DlgRef_2Spin(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_RADIUS"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_HEIGHT"));

  Layout1->addWidget(GroupPoints, 2, 0);
  Layout1->addWidget(GroupDimensions, 2, 0);
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~PrimitiveGUI_CylinderDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_CylinderDlg::~PrimitiveGUI_CylinderDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_CylinderDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  myPoint = myDir = GEOM::GEOM_Object::_nil();
  
  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  double step = St.toDouble();

  /* min, max, step and decimals for spin boxes & initial values */
  /* First constructor : radius */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  /* First constructor : algebric height */
  GroupPoints->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  /* Second constructor : radius */
  GroupDimensions->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, step, 3);
  /* Second constructor : algebric height */
  GroupDimensions->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);

  GroupPoints->SpinBox_DX->SetValue(100.0);
  GroupPoints->SpinBox_DY->SetValue(300.0);
  GroupDimensions->SpinBox_DX->SetValue(100.0);  
  GroupDimensions->SpinBox_DY->SetValue(300.0);  

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

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DY, SLOT(SetStep(double)));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  
  initName( tr( "GEOM_CYLINDER" ) );
  ConstructorsClicked(0);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_CylinderDlg::ConstructorsClicked(int constructorId)
{
  disconnect(mySelection, 0, this, 0);
    
  switch(constructorId)
    { 
    case 0 :
      {
	globalSelection( GEOM_POINT );

	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();
	
	myEditCurrentArgument = GroupPoints->LineEdit1;
	GroupPoints->LineEdit1->setText(tr(""));
	GroupPoints->LineEdit2->setText(tr(""));
	myPoint = myDir = GEOM::GEOM_Object::_nil();

	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    case 1 :
      { 
	GroupPoints->hide();
	resize( 0, 0 );
	GroupDimensions->show();
	
	break;
      }
    }
  displayPreview();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_CylinderDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_CylinderDlg::ClickOnApply()
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
void PrimitiveGUI_CylinderDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void PrimitiveGUI_CylinderDlg::SelectionIntoArgument()
{
  if ( getConstructorId() != 0 )
    return;

  myEditCurrentArgument->setText("");
  
  if(mySelection->IObjectCount() != 1) 
    {
      if(myEditCurrentArgument == GroupPoints->LineEdit1)
	myPoint = GEOM::GEOM_Object::_nil();
      else if (myEditCurrentArgument == GroupPoints->LineEdit2)
	myDir = GEOM::GEOM_Object::_nil();
      return;
    }
  
  /* nbSel == 1 */
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), testResult );
    
  if(!testResult || CORBA::is_nil( aSelectedObject ))
    return;
  
  if (myEditCurrentArgument == GroupPoints->LineEdit1)
    myPoint = aSelectedObject;
  else if (myEditCurrentArgument == GroupPoints->LineEdit2)
    myDir = aSelectedObject;
    
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
 
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_CylinderDlg::SetEditCurrentArgument()
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
void PrimitiveGUI_CylinderDlg::LineEditReturnPressed()
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
void PrimitiveGUI_CylinderDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void PrimitiveGUI_CylinderDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void PrimitiveGUI_CylinderDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox
// purpose  :
//=================================================================================
void PrimitiveGUI_CylinderDlg::ValueChangedInSpinBox( )
{  
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_CylinderDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_CylinderDlg::isValid( QString& msg )
{
  return getConstructorId() == 0 ? !(myPoint->_is_nil() || myDir->_is_nil() ) : true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool  PrimitiveGUI_CylinderDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() ) 
    {
    case 0 :
      {
	if ( !CORBA::is_nil( myPoint ) && !CORBA::is_nil( myDir )) {
	  anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeCylinderPntVecRH(myPoint, myDir, getRadius(), getHeight());
	  res = true;
	}
	break;
      }
    case 1 :
      {
	anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeCylinderRH(getRadius(), getHeight());
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
void  PrimitiveGUI_CylinderDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : getRadius()
// purpose  :
//=================================================================================
double PrimitiveGUI_CylinderDlg::getRadius() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DX->GetValue();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DX->GetValue();
  return 0;
}


//=================================================================================
// function : getHeight()
// purpose  :
//=================================================================================
double PrimitiveGUI_CylinderDlg::getHeight() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DY->GetValue();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DY->GetValue();
  return 0;
}
