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
// File   : PrimitiveGUI_BoxDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#include "PrimitiveGUI_BoxDlg.h"

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
#include <ostream>

//=================================================================================
// class    : PrimitiveGUI_BoxDlg()
// purpose  : Constructs a PrimitiveGUI_BoxDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_BoxDlg::PrimitiveGUI_BoxDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
					  bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BOX_2P" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BOX_DXYZ" ) ));
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_BOX_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_BOX" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_DIAGONAL_POINTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );
  GroupPoints->PushButton1->setIcon( image2 );
  GroupPoints->PushButton2->setIcon( image2 );

  GroupDimensions = new DlgRef_3Spin( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_BOX_OBJ" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_DX" ) );
  GroupDimensions->TextLabel2->setText( tr( "GEOM_DY" ) );
  GroupDimensions->TextLabel3->setText( tr( "GEOM_DZ" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupDimensions );
  /***************************************************************/

  setHelpFileName( "create_box_page.html" );
  
  Init();
}


//=================================================================================
// function : ~DialogBox_Box()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_BoxDlg::~PrimitiveGUI_BoxDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  
  myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
    
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupDimensions->SpinBox_DX, COORD_MIN, COORD_MAX, step, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DY, COORD_MIN, COORD_MAX, step, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupDimensions->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY

  double initValue = 200.0;
  GroupDimensions->SpinBox_DX->setValue( initValue );
  GroupDimensions->SpinBox_DY->setValue( initValue );
  GroupDimensions->SpinBox_DZ->setValue( initValue );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupDimensions->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( GroupDimensions->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( GroupDimensions->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_BOX" ) );

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_BoxDlg::SetDoubleSpinBoxStep( double step )
{
  GroupDimensions->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DY->setSingleStep(step);
  GroupDimensions->SpinBox_DZ->setSingleStep(step);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_BoxDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  
  switch (  constructorId ) {
  case 0:
    {
      //globalSelection( GEOM_POINT );
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      
      GroupDimensions->hide();
      GroupPoints->show();
      
      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
      
      connect( myGeomGUI->getApp()->selectionMgr(),
	       SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
      break;
    }
  case 1:
    {
      GroupPoints->hide();
      GroupDimensions->show();
      globalSelection(); // close local contexts, if any
      break;
    }
  }
 
  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  displayPreview();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void PrimitiveGUI_BoxDlg::SelectionIntoArgument()
{
  if ( getConstructorId() != 0 )
    return;
  
  myEditCurrentArgument->setText( "" );
  
  if ( IObjectCount() != 1 ) {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
      myPoint1 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
      myPoint2 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
  
  if ( !testResult || CORBA::is_nil( aSelectedObject ) )
    return;
  
  TopoDS_Shape aShape;
  QString aName = GEOMBase::GetName( aSelectedObject );
  if (GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    TColStd_IndexedMapOfInteger aMap;
    aSelMgr->GetIndexes( firstIObject(), aMap );
    if ( aMap.Extent() == 1) { // Local Selection
      int anIndex = aMap( 1 );
      aName.append( ":vertex_" + QString::number( anIndex ) );	  

      //Find SubShape Object in Father
      GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );
      
      if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	GEOM::GEOM_IShapesOperations_var aShapesOp =
	  getGeomEngine()->GetIShapesOperations( getStudyId() );
	aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
      }
      else {
	aSelectedObject = aFindedObject; // get Object from study	
      }
    }
    else { // Global Selection
      if ( aShape.ShapeType() != TopAbs_VERTEX ) {
        aSelectedObject = GEOM::GEOM_Object::_nil();
        aName = "";
      }
    }
  }

  myEditCurrentArgument->setText( aName );

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
    myPoint1 = aSelectedObject;
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
    myPoint2 = aSelectedObject;
  
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if ( send == GroupPoints->PushButton1 )
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == GroupPoints->PushButton2 )
    myEditCurrentArgument = GroupPoints->LineEdit2;
  
  //  globalSelection( GEOM_POINT );
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );
  
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_BoxDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::isValid( QString& msg )
{
  return getConstructorId() == 0 ? !( myPoint1->_is_nil() || myPoint2->_is_nil() ) : true;
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() ) {
  case 0 :
    { 
      if ( !CORBA::is_nil( myPoint1 ) &&  !CORBA::is_nil( myPoint2 ) ) {
	anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeBoxTwoPnt( myPoint1, myPoint2 );
	res = true;
      }
      
      break;
    }
  case 1 :
    {
      double x = GroupDimensions->SpinBox_DX->value();
      double y = GroupDimensions->SpinBox_DY->value();
      double z = GroupDimensions->SpinBox_DZ->value();
      
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeBoxDXDYDZ( x, y, z );
      res = true;
      break;
    }
  }
  
  if ( !anObj->_is_nil() )
  objects.push_back( anObj._retn() );
  
  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void PrimitiveGUI_BoxDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  if ( getConstructorId() == 0 ) {
    objMap[GroupPoints->LineEdit1->text()] = myPoint1;
    objMap[GroupPoints->LineEdit2->text()] = myPoint2;
    addSubshapesToFather( objMap );
  }
}
