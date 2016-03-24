// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : OperationGUI_ArchimedeDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#include "OperationGUI_ArchimedeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : OperationGUI_ArchimedeDlg()
// purpose  : Constructs a OperationGUI_ArchimedeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ArchimedeDlg::OperationGUI_ArchimedeDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_ARCHIMEDE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  
  setWindowTitle( tr( "GEOM_ARCHIMEDE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_ARCHIMEDE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel3Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_WEIGHT" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_WATER_DENSITY" ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_MESHING_DEFLECTION" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/

  setHelpFileName( "archimede_page.html" );

  /* Initialisations */
  Init();
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
void OperationGUI_ArchimedeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  /* Get setting of myStep value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double myStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  double SpecificStep1 = 0.1;
  double SpecificStep2 = 0.01;
  /* min, max, myStep and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, 0.001, COORD_MAX, myStep, "weight_precision" );
  initSpinBox( GroupPoints->SpinBox_DY, 0.001, COORD_MAX, SpecificStep1, "density_precision" );
  initSpinBox( GroupPoints->SpinBox_DZ, 0.001, COORD_MAX, SpecificStep2, "parametric_precision" );

  showOnlyPreviewControl();

  GroupPoints->SpinBox_DX->setValue( 100.0 );
  GroupPoints->SpinBox_DY->setValue( 1.0 );
  GroupPoints->SpinBox_DZ->setValue( 0.01 );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );
  
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );


  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
  
  initName( tr( "GEOM_ARCHIMEDE" ) );
  
  globalSelection( GEOM_ALLSHAPES );

  SelectionIntoArgument();
  resize(100,100);
}


//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void OperationGUI_ArchimedeDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  
  // san: Commented so as not to override specific step settings
  //GroupPoints->SpinBox_DY->setSingleStep(step);
  //GroupPoints->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
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
  myEditCurrentArgument->setText("");
  myShape = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  myShape = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( !GEOMBase::IsShape(myShape) ) {
    myShape = GEOM::GEOM_Object::_nil();
    return;
  }
  myEditCurrentArgument->setText(GEOMBase::GetName(myShape));
  processPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 )
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  processPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::enterEvent( QEvent* )
{
  if ( mainFrame()->GroupConstructors->isEnabled() )
    return;
  this->ActivateThisDialog();
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
bool OperationGUI_ArchimedeDlg::isValid( QString& msg )
{
  bool ok = true;
  ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
  return !myShape->_is_nil() && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ArchimedeDlg::execute( ObjectList& objects )
{
  double aWeight         = GroupPoints->SpinBox_DX->value();
  double aWaterDensity   = GroupPoints->SpinBox_DY->value();
  double aMeshDeflection = GroupPoints->SpinBox_DZ->value();
  
  GEOM::GEOM_ILocalOperations_var anOper = GEOM::GEOM_ILocalOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->MakeArchimede( myShape, aWeight, aWaterDensity, aMeshDeflection );

  if ( !anObj->_is_nil() )
  {
    if ( !IsPreview() )
    {
      QStringList aParameters;
      aParameters << GroupPoints->SpinBox_DX->text();
      aParameters << GroupPoints->SpinBox_DY->text();
      aParameters << GroupPoints->SpinBox_DZ->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    objects.push_back( anObj._retn() );
  }

  return true;
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ArchimedeDlg::ValueChangedInSpinBox(double newValue)
{
  processPreview();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_ArchimedeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
