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
// File   : TransformationGUI_ScaleDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "TransformationGUI_ScaleDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_ScaleDlg()
// purpose  : Constructs a TransformationGUI_ScaleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ScaleDlg::TransformationGUI_ScaleDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
							bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SCALE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SCALE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SCALE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel1Spin2Check( centralWidget() );
  GroupPoints->CheckButton2->hide();
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_CENTRAL_POINT" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_SCALE_FACTOR" ) );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );
  GroupPoints->CheckButton1->setText( tr( "GEOM_CREATE_COPY" ) );

  // san -- modification of an exisitng object by offset is not allowed
  GroupPoints->CheckButton1->hide();

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/
  double aFactor = 2.0;
  double SpecificStep = 0.5;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, SpecificStep, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupPoints->SpinBox_DX->setValue( aFactor );
  
  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked( true );
  CreateCopyModeChanged( true );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  
  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  
  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );
  connect( GroupPoints->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( CreateCopyModeChanged( bool ) ) );
   
  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  setHelpFileName( "scale_operation_page.html" );

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
  if ( !onAccept( GroupPoints->CheckButton1->isChecked() ) )
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
  myEditCurrentArgument->setText( "" );
  QString aName;

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects( selectedIO(), aName );
    if ( aNbSel < 1 ) {
      myObjects.length( 0 );
      return;
    }
    GEOMBase::ConvertListOfIOInListOfGO( selectedIO(), myObjects );
    if ( !myObjects.length() )
      return;
  }
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
    if ( IObjectCount() != 1 ) {
      myPoint = GEOM::GEOM_Object::_nil();
      return;
    }
    Standard_Boolean testResult = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
    aName = GEOMBase::GetName( aSelectedObject );
    
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes( firstIObject(), aMap );
      if ( aMap.Extent() == 1 ) {
	int anIndex = aMap( 1 );
	aName += QString( ":vertex_%1" ).arg( anIndex );
	
	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = findObjectInFather( aSelectedObject, aName );
	
	if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	  GEOM::GEOM_IShapesOperations_var aShapesOp =
	    getGeomEngine()->GetIShapesOperations( getStudyId() );
	  aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	  aSelMgr->clearSelected();
	}
	else {
	  aSelectedObject = aFindedObject; // get Object from study
	}
      }
      else {
	if ( aShape.ShapeType() != TopAbs_VERTEX ) {
	  aSelectedObject = GEOM::GEOM_Object::_nil();
	  aName = "";
	}
      }
    }
    
    myPoint = aSelectedObject;

    if ( !testResult || CORBA::is_nil( myPoint ) )
      return;
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
  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 ) {
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
  globalSelection();
  
  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
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
  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  globalSelection();
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
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
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_ScaleDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ScaleDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::isValid( QString& /*msg*/)
{
  return !( myObjects.length() == 0 || myPoint->_is_nil() || fabs( GetFactor()) <= 0.00001 );
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  if ( GroupPoints->CheckButton1->isChecked() || IsPreview() ) {
    for ( int i = 0; i < myObjects.length(); i++ ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->ScaleShapeCopy( myObjects[i], myPoint, GetFactor() );
      if ( !anObj->_is_nil() )
	objects.push_back( anObj._retn() );
    }
  }
  else {
    for ( int i = 0; i < myObjects.length(); i++ ) {
      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->ScaleShape( myObjects[i], myPoint, GetFactor() );
      if ( !anObj->_is_nil() )
	objects.push_back( anObj._retn() );
    }
  }
  return true;
}


//=================================================================================
// function : GetFactor()
// purpose  :
//=================================================================================
double TransformationGUI_ScaleDlg::GetFactor() const
{
  return GroupPoints->SpinBox_DX->value();
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::CreateCopyModeChanged( bool isCreateCopy )
{
  mainFrame()->GroupBoxName->setEnabled(isCreateCopy);
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_ScaleDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();
  if ( toCreateCopy ) {
    QMap<QString, GEOM::GEOM_Object_var> objMap;

    objMap[GroupPoints->LineEdit2->text()] = myPoint;
    
    addSubshapesToFather( objMap );
  }
}
