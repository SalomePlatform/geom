// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License. 
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
// File   : TransformationGUI_MultiRotationDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#include "TransformationGUI_MultiRotationDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_MultiRotationDlg()
// purpose  : Constructs a TransformationGUI_MultiRotationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiRotationDlg::TransformationGUI_MultiRotationDlg
( GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_MULTIROTATION_SIMPLE" ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_MULTIROTATION_DOUBLE" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_MULTIROTATION_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_MULTIROTATION" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel1Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_MULTIROTATION_SIMPLE" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_VECTOR" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_NB_TIMES" ) );
  GroupPoints->PushButton1->setIcon( image2 );
  GroupPoints->PushButton2->setIcon( image2 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  GroupDimensions = new DlgRef_2Sel4Spin1Check( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_MULTIROTATION_DOUBLE" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  GroupDimensions->TextLabel2->setText( tr( "GEOM_VECTOR" ) );
  GroupDimensions->TextLabel3->setText( tr( "GEOM_ANGLE" ) );
  GroupDimensions->TextLabel4->setText( tr( "GEOM_NB_TIMES" ) );
  GroupDimensions->TextLabel5->setText( tr( "GEOM_STEP" ) );
  GroupDimensions->TextLabel6->setText( tr( "GEOM_NB_TIMES" ) );
  GroupDimensions->CheckButton1->setText( tr( "GEOM_REVERSE" ) );
  GroupDimensions->PushButton1->setIcon( image2 );
  GroupDimensions->PushButton2->setIcon( image2 );
  GroupDimensions->LineEdit1->setReadOnly( true );
  GroupDimensions->LineEdit2->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupDimensions );
  /***************************************************************/

  setHelpFileName( "multi_rotation_operation_page.html" );

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
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  double SpecificStep1 = 5;
  double SpecificStep2 = 1;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, 1.0, MAX_NUMBER, SpecificStep2, 10 );
  GroupPoints->SpinBox_DX->setValue( myNbTimes1 );

  initSpinBox( GroupDimensions->SpinBox_DX1, COORD_MIN, COORD_MAX, SpecificStep1, 10 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DY1, 1.0, MAX_NUMBER, SpecificStep2, 10 );
  initSpinBox( GroupDimensions->SpinBox_DX2, COORD_MIN, COORD_MAX, step, 10 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DY2, 1.0, MAX_NUMBER, SpecificStep2, 10 );
  GroupDimensions->SpinBox_DX1->setValue( myAng );
  GroupDimensions->SpinBox_DY1->setValue( myNbTimes1 );
  GroupDimensions->SpinBox_DX2->setValue( myStep );
  GroupDimensions->SpinBox_DY2->setValue( myNbTimes2 );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1,     SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2,     SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupDimensions->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupDimensions->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1,     SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2,     SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupDimensions->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupDimensions->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DX1, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY1, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DX2, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY2, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( GroupDimensions->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( ReverseAngle() ) );
  
  connect( myGeomGUI->getApp()->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_MULTIROTATION" ) );

  ConstructorsClicked( 0 );
}



//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DX1->setSingleStep(step);
  GroupDimensions->SpinBox_DY1->setSingleStep(step);
  GroupDimensions->SpinBox_DX2->setSingleStep(step);
  GroupDimensions->SpinBox_DY2->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiRotationDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );

  myAng = 45.0;
  myStep = 50.0;
  myNbTimes1 = myNbTimes2 = 2;

  globalSelection( GEOM_ALLSHAPES );

  switch ( constructorId ) {
  case 0: /* Rotate simple */
    {
      GroupDimensions->hide();
      GroupPoints->show();

      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      
      GroupPoints->SpinBox_DX->setValue( myNbTimes1 );
      
      break;
    }
  case 1: /* Rotate double */
    {
      GroupPoints->hide();
      GroupDimensions->show();
      
      myEditCurrentArgument = GroupDimensions->LineEdit1;
      GroupDimensions->LineEdit1->setText( "" );
      GroupDimensions->LineEdit2->setText( "" );
      
      GroupDimensions->SpinBox_DX1->setValue( myAng );
      GroupDimensions->SpinBox_DY1->setValue( myNbTimes1 );
      GroupDimensions->SpinBox_DX2->setValue( myStep );
      GroupDimensions->SpinBox_DY2->setValue( myNbTimes2 );
      
      break;
    }      
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  myEditCurrentArgument->setFocus();
  myBase = myVector = GEOM::GEOM_Object::_nil();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );
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
  myEditCurrentArgument->setText( "" );

  if ( IObjectCount() != 1 ) {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ||
	 myEditCurrentArgument == GroupDimensions->LineEdit1 )
      myBase = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ||
	      myEditCurrentArgument == GroupDimensions->LineEdit2 )
      myVector = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );

  if ( !testResult || CORBA::is_nil( aSelectedObject) || !GEOMBase::IsShape( aSelectedObject ) )
    return;

  QString aName = GEOMBase::GetName( aSelectedObject );

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ||
       myEditCurrentArgument == GroupDimensions->LineEdit1 )
    myBase = aSelectedObject;
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ||
            myEditCurrentArgument == GroupDimensions->LineEdit2 ) {
    if ( testResult && !aSelectedObject->_is_nil() ) {
      TopoDS_Shape aShape;
	  
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
	LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
	TColStd_IndexedMapOfInteger aMap;
	aSelMgr->GetIndexes( firstIObject(), aMap );
	if ( aMap.Extent() == 1 ) {
	  int anIndex = aMap( 1 );
	  aName += QString( ":edge_%1" ).arg( anIndex );
	  
	  //Find SubShape Object in Father
	  GEOM::GEOM_Object_var aFindedObject = findObjectInFather( aSelectedObject, aName );
		  
	  if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	    GEOM::GEOM_IShapesOperations_var aShapesOp =
	      getGeomEngine()->GetIShapesOperations( getStudyId() );
	    myVector = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	  }
	  else {
	    myVector = aFindedObject; // get existing object
	  }
	}
	else {
	  if ( aShape.ShapeType() != TopAbs_EDGE ) {
	    aSelectedObject = GEOM::GEOM_Object::_nil();
	    aName = "";
	  }
	  myVector = aSelectedObject;
	}
      }
    }
  }
  myEditCurrentArgument->setText( aName );

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_ALLSHAPES );
  
  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else if ( send == GroupDimensions->PushButton1 ) {
    myEditCurrentArgument = GroupDimensions->LineEdit1;
  }
  else if ( send == GroupDimensions->PushButton2 ) {
    myEditCurrentArgument = GroupDimensions->LineEdit2;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
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
  if ( send == GroupPoints->LineEdit1 || send == GroupDimensions->LineEdit1 || 
       send == GroupPoints->LineEdit2 || send == GroupDimensions->LineEdit2 ) {
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
  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();

  if ( send == GroupPoints->SpinBox_DX || send == GroupDimensions->SpinBox_DY1 )
    myNbTimes1 = (int)newValue;
  else if ( send == GroupDimensions->SpinBox_DX1 )
    myAng = newValue;
  else if ( send == GroupDimensions->SpinBox_DX2 )
    myStep = newValue;
  else if ( send == GroupDimensions->SpinBox_DY2 )
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

  if ( aConstructorId == 0 )
    GroupPoints->SpinBox_DX->setValue( myAng );
  else if ( aConstructorId == 1 )
    GroupDimensions->SpinBox_DX1->setValue( myAng );

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
bool TransformationGUI_MultiRotationDlg::isValid( QString& /*msg*/ )
{
  return !( myBase->_is_nil() || myVector->_is_nil() );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0 :
    if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVector ) ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->
	MultiRotate1D( myBase, myVector, myNbTimes1 );
      res = true;
    }
    break;
  case 1 :
    if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVector ) ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->
	MultiRotate2D( myBase, myVector, myAng, myNbTimes1, myStep, myNbTimes2 );
      res = true;
    }
    break;
  }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_MultiRotationDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  
  switch ( getConstructorId() ) {
  case 0:
    objMap[GroupPoints->LineEdit2->text()] = myVector;
    break;
  case 1:
    objMap[GroupDimensions->LineEdit2->text()] = myVector;
    break;
  }
  addSubshapesToFather( objMap );
}
