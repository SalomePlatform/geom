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
//  File   : OperationGUI_ArchimedeDlg.cxx
//  Author : Nicolas REJNERI 
//  Module : GEOM
//  $Header$

#include "OperationGUI_ArchimedeDlg.h"
#include "DlgRef_1Sel3Spin.h"

#include "QAD_Config.h"
#include "QAD_Desktop.h"

//=================================================================================
// class    : OperationGUI_ArchimedeDlg()
// purpose  : Constructs a OperationGUI_ArchimedeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ArchimedeDlg::OperationGUI_ArchimedeDlg( QWidget* parent, SALOME_Selection* Sel )
: GEOMBase_Skeleton( parent, "ArchimedeDlg", Sel, false,
    WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  mySelection = Sel;

  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ARCHIMEDE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));
  
  setCaption(tr("GEOM_ARCHIMEDE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ARCHIMEDE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel3Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_WEIGHT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_WATER_DENSITY"));
  GroupPoints->TextLabel4->setText(tr("GEOM_MESHING_DEFLECTION"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init( mySelection );
}


//=================================================================================
// function : ~OperationGUI_ArchimedeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ArchimedeDlg::~OperationGUI_ArchimedeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::Init( SALOME_Selection* Sel )
{
  mySelection = Sel;

  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  /* Get setting of myStep value from file configuration */
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" );
  myStep = !St.isEmpty() ? St.toDouble() : 100;

  double SpecificStep1 = 0.1;
  double SpecificStep2 = 0.01;
  /* min, max, myStep and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, myStep, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_DZ->RangeStepAndValidator(0.001, 999.999, SpecificStep2, 3);

  GroupPoints->SpinBox_DX->SetValue( 100.0 );
  GroupPoints->SpinBox_DY->SetValue( 1.0 );
  GroupPoints->SpinBox_DZ->SetValue( 0.01 );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DZ, SLOT(SetStep(double)));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_ARCHIMEDE" ) );

  globalSelection( GEOM_ALLSHAPES );

  SelectionIntoArgument();
  
  /* displays Dialog */
  GroupPoints->show();
  this->show();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ArchimedeDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_ArchimedeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  myShape = GEOM::GEOM_Object::_nil();
  
  if ( mySelection->IObjectCount() != 1 )
    return;

  Standard_Boolean testResult = Standard_False;
  myShape = GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), testResult );

  if ( !testResult || myShape->_is_nil() || !GEOMBase::IsShape( myShape ) )
  {
    myShape = GEOM::GEOM_Object::_nil();
    return;
  }

  myEditCurrentArgument->setText( GEOMBase::GetName( myShape ) );
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ArchimedeDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_ArchimedeDlg::isValid( QString& )
{
  return !myShape->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ArchimedeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  double aWeight         = GroupPoints->SpinBox_DX->GetValue();
  double aWaterDensity   = GroupPoints->SpinBox_DY->GetValue();
  double aMeshDeflection = GroupPoints->SpinBox_DZ->GetValue();
  
  anObj = GEOM::GEOM_ILocalOperations::_narrow(
    getOperation() )->MakeArchimede( myShape, aWeight, aWaterDensity, aMeshDeflection );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
