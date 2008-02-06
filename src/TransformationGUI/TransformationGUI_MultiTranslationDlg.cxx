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
// File   : TransformationGUI_MultiTranslationDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#include "TransformationGUI_MultiTranslationDlg.h"

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
// class    : TransformationGUI_MultiTranslationDlg()
// purpose  : Constructs a TransformationGUI_MultiTranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiTranslationDlg::TransformationGUI_MultiTranslationDlg
( GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_MULTITRANSLATION_SIMPLE" ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_MULTITRANSLATION_DOUBLE" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_MULTITRANSLATION_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_MULTITRANSLATION" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel2Spin1Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_MULTITRANSLATION_SIMPLE" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_VECTOR_U" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_STEP_U" ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_NB_TIMES_U" ) );
  GroupPoints->CheckButton1->setText( tr( "GEOM_REVERSE_U" ) );
  GroupPoints->PushButton1->setIcon( image2 );
  GroupPoints->PushButton2->setIcon( image2 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  GroupDimensions = new DlgRef_3Sel4Spin2Check( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_MULTITRANSLATION_DOUBLE" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  GroupDimensions->TextLabel2->setText( tr( "GEOM_VECTOR_U" ) );
  GroupDimensions->TextLabel3->setText( tr( "GEOM_VECTOR_V" ) );
  GroupDimensions->TextLabel4->setText( tr( "GEOM_STEP_U" ) );
  GroupDimensions->TextLabel5->setText( tr( "GEOM_NB_TIMES_U" ) );
  GroupDimensions->TextLabel6->setText( tr( "GEOM_STEP_V" ) );
  GroupDimensions->TextLabel7->setText( tr( "GEOM_NB_TIMES_V" ) );
  GroupDimensions->CheckButton1->setText( tr( "GEOM_REVERSE_U" ) );
  GroupDimensions->CheckButton2->setText( tr( "GEOM_REVERSE_V" ) );
  GroupDimensions->PushButton1->setIcon( image2 );
  GroupDimensions->PushButton2->setIcon( image2 );
  GroupDimensions->PushButton3->setIcon( image2 );
  GroupDimensions->LineEdit1->setReadOnly( true );
  GroupDimensions->LineEdit2->setReadOnly( true );
  GroupDimensions->LineEdit3->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupDimensions );
  /***************************************************************/

  setHelpFileName( "multi_translation_operation_page.html" );

  Init();
}


//=================================================================================
// function : ~TransformationGUI_MultiTranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiTranslationDlg::~TransformationGUI_MultiTranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::Init()
{
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  double SpecificStep = 1;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, step, 10 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupPoints->SpinBox_DY, 1.0, MAX_NUMBER, SpecificStep, 10 );
  GroupPoints->SpinBox_DX->setValue( myStepU );
  GroupPoints->SpinBox_DY->setValue( myNbTimesU );

  initSpinBox( GroupDimensions->SpinBox_DX1, COORD_MIN, COORD_MAX, step, 10 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DY1, 1.0, MAX_NUMBER, SpecificStep, 10 );
  initSpinBox( GroupDimensions->SpinBox_DX2, COORD_MIN, COORD_MAX, step, 10 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DY2, 1.0, MAX_NUMBER, SpecificStep, 10 );
  GroupDimensions->SpinBox_DX1->setValue( myStepU );
  GroupDimensions->SpinBox_DY1->setValue( myNbTimesU );
  GroupDimensions->SpinBox_DX2->setValue( myStepV );
  GroupDimensions->SpinBox_DY2->setValue( myNbTimesV );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this, SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1,     SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2,     SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupDimensions->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupDimensions->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupDimensions->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1,     SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2,     SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupDimensions->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupDimensions->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupDimensions->LineEdit3, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox_DY,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DX1, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY1, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DX2, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY2, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  // VSR: TODO ->>
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupPoints->SpinBox_DX,      SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupPoints->SpinBox_DY,      SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupDimensions->SpinBox_DX1, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupDimensions->SpinBox_DY1, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupDimensions->SpinBox_DX2, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupDimensions->SpinBox_DY2, SLOT( SetStep( double ) ) );
  // <<-

  connect( GroupPoints->CheckButton1,     SIGNAL( toggled( bool ) ), this, SLOT( ReverseStepU() ) );
  connect( GroupDimensions->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( ReverseStepU() ) );
  connect( GroupDimensions->CheckButton2, SIGNAL( toggled( bool ) ), this, SLOT( ReverseStepV() ) );
  
  connect( myGeomGUI->getApp()->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
  initName( tr( "GEOM_MULTITRANSLATION" ) );

  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  
  myStepU = myStepV = 50.0;
  myNbTimesU = myNbTimesV = 2;

  globalSelection( GEOM_ALLSHAPES );

  switch ( constructorId ) {
  case 0: /* Translate simple */
    {
      GroupDimensions->hide();
      GroupPoints->show();
      
      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      
      GroupPoints->SpinBox_DX->setValue( myStepU );
      GroupPoints->SpinBox_DY->setValue( myNbTimesU );
	
      break;
    }
  case 1: /* Translate double */
    {
      GroupPoints->hide();
      GroupDimensions->show();
      
      myEditCurrentArgument = GroupDimensions->LineEdit1;
      GroupDimensions->LineEdit1->setText( "" );
      GroupDimensions->LineEdit2->setText( "" );
      GroupDimensions->LineEdit3->setText( "" );
      
      GroupDimensions->SpinBox_DX1->setValue( myStepU );
      GroupDimensions->SpinBox_DY1->setValue( myNbTimesU );
      GroupDimensions->SpinBox_DX2->setValue( myStepV );
      GroupDimensions->SpinBox_DY2->setValue( myNbTimesV );
	
      myVectorV = GEOM::GEOM_Object::_nil();
      break;
    }
  }
  
  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  myEditCurrentArgument->setFocus();
  myBase = myVectorU = GEOM::GEOM_Object::_nil();
  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MultiTranslationDlg::ClickOnApply()
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
void TransformationGUI_MultiTranslationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  
  if ( IObjectCount() != 1 ) {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 || 
	 myEditCurrentArgument == GroupDimensions->LineEdit1 )
      myBase = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 || 
	      myEditCurrentArgument == GroupDimensions->LineEdit2 )
      myVectorU = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupDimensions->LineEdit3 )
      myVectorV = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );

  if ( !testResult || CORBA::is_nil( aSelectedObject ) || !GEOMBase::IsShape( aSelectedObject ) )
    return;

  QString aName = GEOMBase::GetName( aSelectedObject );

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ||
       myEditCurrentArgument == GroupDimensions->LineEdit1 )
    myBase = aSelectedObject;
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ||
	    myEditCurrentArgument == GroupDimensions->LineEdit2 ||
	    myEditCurrentArgument == GroupDimensions->LineEdit3 ) {
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
	    if ( myEditCurrentArgument == GroupDimensions->LineEdit3 )
	      myVectorV = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	    else
	      myVectorU = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	  }
	  else {
	    if ( myEditCurrentArgument == GroupDimensions->LineEdit3 )
	      myVectorV = aFindedObject;
	    else
	      myVectorU = aFindedObject;
	  }
	}
	else {
	  if ( aShape.ShapeType() != TopAbs_EDGE ) {
	    aSelectedObject = GEOM::GEOM_Object::_nil();
	    aName = "";
	  }
	  if ( myEditCurrentArgument == GroupDimensions->LineEdit3 )
	    myVectorV = aSelectedObject;
	  else
	    myVectorU = aSelectedObject;
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
void TransformationGUI_MultiTranslationDlg::SetEditCurrentArgument()
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
  else if ( send == GroupDimensions->PushButton3 ) {
    myEditCurrentArgument = GroupDimensions->LineEdit3;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 || send == GroupDimensions->LineEdit1 ||
       send == GroupPoints->LineEdit2 || send == GroupDimensions->LineEdit2 ||
       send == GroupDimensions->LineEdit3 ) {
    myEditCurrentArgument = send; 
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ActivateThisDialog()
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
void TransformationGUI_MultiTranslationDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();

  switch ( getConstructorId() ) {
  case 0:
    if ( send == GroupPoints->SpinBox_DX )
      myStepU = newValue;
    else if ( send == GroupPoints->SpinBox_DY )
      myNbTimesU = (int)newValue;
    break;
  case 1 :
    if ( send == GroupDimensions->SpinBox_DX1 )
      myStepU = newValue;
    else if ( send == GroupDimensions->SpinBox_DY1)
      myNbTimesU = (int)newValue;
    else if ( send == GroupDimensions->SpinBox_DX2 )
      myStepV = newValue;
    else if ( send == GroupDimensions->SpinBox_DY2 )
      myNbTimesV = (int)newValue;
    break;
  }
  
  displayPreview();
}


//=================================================================================
// function : ReverseStepU()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseStepU()
{
  myStepU = -myStepU;
  
  int aConstructorId = getConstructorId();

  if ( aConstructorId == 0 )
    GroupPoints->SpinBox_DX->setValue( myStepU );
  else if ( aConstructorId == 1 )
    GroupDimensions->SpinBox_DX1->setValue( myStepU );
  
  displayPreview();
}


//=================================================================================
// function : ReverseStepV
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseStepV()
{
  myStepV = -myStepV;
  
  GroupDimensions->SpinBox_DX2->setValue( myStepV );
 
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_MultiTranslationDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MultiTranslationDlg::isValid( QString& /*msg*/ )
{
  int aConstructorId = getConstructorId();
  
  if ( aConstructorId == 0 )
    return !( myBase->_is_nil() || myVectorU->_is_nil() );
  else if ( aConstructorId == 1 )
    return !( myBase->_is_nil() || myVectorU->_is_nil() || myVectorV->_is_nil() );
  return 0;	    
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_MultiTranslationDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0 :
    if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVectorU ) ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MultiTranslate1D( myBase, myVectorU, myStepU, myNbTimesU );
      res = true;
    }
    break;
  case 1 :
    if ( !CORBA::is_nil( myBase ) && !CORBA::is_nil( myVectorU ) && 
	 !CORBA::is_nil( myVectorV ) ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MultiTranslate2D( myBase, myVectorU, myStepU, myNbTimesU,
											    myVectorV, myStepV, myNbTimesV );
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
void TransformationGUI_MultiTranslationDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  
  switch ( getConstructorId() ) {
  case 0:
    objMap[GroupPoints->LineEdit2->text()] = myVectorU;
    break;
  case 1:
    objMap[GroupDimensions->LineEdit2->text()] = myVectorU;
    objMap[GroupDimensions->LineEdit3->text()] = myVectorV;
    break;
  }
  addSubshapesToFather( objMap );
}
