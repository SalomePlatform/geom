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
// File   : PrimitiveGUI_FaceDlg.cxx
// Author : Dmitry Matveitchev, OCN.
//

#include "PrimitiveGUI_FaceDlg.h"

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
// class    : PrimitiveGUI_FaceDlg()
// purpose  : Constructs a PrimitiveGUI_FaceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_FaceDlg::PrimitiveGUI_FaceDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
				    bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_FACE_OBJ_HW" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_FACE_HW" ) ) );

  setWindowTitle( tr( "GEOM_FACE_TITLE" ) );
 
  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_FACE" ) );
  mainFrame()->RadioButton1->setIcon( image2 );
  mainFrame()->RadioButton2->setIcon( image0 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPlane = new DlgRef_1Sel2Spin( centralWidget() );
  GroupPlane->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPlane->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  GroupPlane->TextLabel2->setText( tr( "GEOM_HEIGHT" ) );
  GroupPlane->TextLabel3->setText( tr( "GEOM_WIDTH" ) );
  GroupPlane->PushButton1->setIcon( image1 );
  GroupPlane->LineEdit1->setReadOnly( true );

  GroupType = new DlgRef_3Radio( centralWidget() );
  GroupType->GroupBox1->setTitle( tr( "GEOM_OBJECT_TYPE" ) );
  GroupType->RadioButton1->setText( tr( "GEOM_EDGE" ) );
  GroupType->RadioButton2->setText( tr( "GEOM_FACE" ) );
  GroupType->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  GroupType->RadioButton3->close();

  GroupDimensions = new DlgRef_2Spin( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_BOX_OBJ" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_HEIGHT" ) );
  GroupDimensions->TextLabel2->setText( tr( "GEOM_WIDTH" ) );

  GroupOrientation = new DlgRef_3Radio( centralWidget() );

  GroupOrientation->GroupBox1->setTitle( tr( "GEOM_ORIENTATION" ) );
  GroupOrientation->RadioButton1->setText( tr( "GEOM_WPLANE_OXY" ) );
  GroupOrientation->RadioButton2->setText( tr( "GEOM_WPLANE_OYZ" ) );
  GroupOrientation->RadioButton3->setText( tr( "GEOM_WPLANE_OZX" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupType );
  layout->addWidget( GroupPlane );
  layout->addWidget( GroupDimensions );
  layout->addWidget( GroupOrientation );

  /***************************************************************/

  setHelpFileName( "create_squareface_page.html" );

  Init();
}


//=================================================================================
// function : ~PrimitiveGUI_FaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_FaceDlg::~PrimitiveGUI_FaceDlg()
{  
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPlane->LineEdit1;
  GroupType->RadioButton1->setChecked(true);
  myEdge = GEOM::GEOM_Object::_nil();
  myFace = GEOM::GEOM_Object::_nil();
  globalSelection(); // close local contexts, if any
  //  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );

  myOrientationType = 1;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  double aDefaultSize = 100.0;
  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupPlane->SpinBox_DX, 0.001, COORD_MAX, aStep, 3 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupPlane->SpinBox_DX->setValue( aDefaultSize );
  initSpinBox( GroupPlane->SpinBox_DY, 0.001, COORD_MAX, aStep, 3 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupPlane->SpinBox_DY->setValue( aDefaultSize );

  initSpinBox( GroupDimensions->SpinBox_DX, 0.001, COORD_MAX, aStep, 3 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupDimensions->SpinBox_DX->setValue( aDefaultSize );
  initSpinBox( GroupDimensions->SpinBox_DY, 0.001, COORD_MAX, aStep, 3 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupDimensions->SpinBox_DY->setValue( aDefaultSize );

	
  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPlane->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPlane->LineEdit1,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPlane->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPlane->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( GroupDimensions->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( GroupOrientation->RadioButton1, SIGNAL( clicked() ), this, SLOT( RadioButtonClicked() ) );
  connect( GroupOrientation->RadioButton2, SIGNAL( clicked() ), this, SLOT( RadioButtonClicked() ) );
  connect( GroupOrientation->RadioButton3, SIGNAL( clicked() ), this, SLOT( RadioButtonClicked() ) );

  connect( GroupType->RadioButton1, SIGNAL( clicked() ), this, SLOT( TypeButtonClicked() ) );
  connect( GroupType->RadioButton2, SIGNAL( clicked() ), this, SLOT( TypeButtonClicked() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );
  
  initName( tr( "GEOM_FACE" ) );

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_FaceDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPlane->SpinBox_DX->setSingleStep(step);
  GroupPlane->SpinBox_DY->setSingleStep(step);
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}

//=================================================================================
// function : RadioBittonClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_FaceDlg::RadioButtonClicked()
{
  if ( GroupOrientation->RadioButton1->isChecked() )
    myOrientationType = 1;
  else if ( GroupOrientation->RadioButton2->isChecked() )
    myOrientationType = 2;
  else if ( GroupOrientation->RadioButton3->isChecked() )
    myOrientationType = 3;
  displayPreview();
}

//=================================================================================
// function : TypeBittonClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_FaceDlg::TypeButtonClicked()
{
  if ( GroupType->RadioButton1->isChecked() ) {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    GroupPlane->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  }
  else if ( GroupType->RadioButton2->isChecked() ) {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
    GroupPlane->TextLabel1->setText( tr( "GEOM_FACE" ) );
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_FaceDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();

  myEditCurrentArgument->setText( "" );
  ConstructorsClicked( getConstructorId() );

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_FaceDlg::ConstructorsClicked( int constructorId )
{  
  switch ( constructorId ) {
  case 0:
    {
      globalSelection(); // close local contexts, if any
      GroupPlane->hide();
      GroupDimensions->show();
      GroupOrientation->show();
      GroupType->hide();
      GroupOrientation->RadioButton1->setChecked( true );
      myOrientationType = 1;
      break;
    }
  case 1:
    {
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
      myEditCurrentArgument = GroupPlane->LineEdit1;
      myEditCurrentArgument->setText("");
      myEdge = GEOM::GEOM_Object::_nil();
      myFace = GEOM::GEOM_Object::_nil();
      GroupType->RadioButton1->setChecked( true );
      TypeButtonClicked();
      GroupDimensions->hide();
      GroupOrientation->hide();
      GroupType->show();
      GroupPlane->show();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );
  SelectionIntoArgument();
  displayPreview();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void PrimitiveGUI_FaceDlg::SelectionIntoArgument()
{
  if (getConstructorId() == 0) {
    displayPreview();
    return;
  }

  myEditCurrentArgument->setText( "" );

  if ( IObjectCount() != 1 ) {
    if ( myEditCurrentArgument == GroupPlane->LineEdit1 )  myEdge = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1 
  Standard_Boolean aRes = Standard_False;
  TopAbs_ShapeEnum aNeedType = TopAbs_EDGE;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
    QString aName = GEOMBase::GetName( aSelectedObject );
    
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      if (GroupType->RadioButton2->isChecked())
	aNeedType = TopAbs_FACE;

      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes( firstIObject(), aMap );
      if ( aMap.Extent() == 1 ) { // Local Selection
	int anIndex = aMap( 1 );
	if ( aNeedType == TopAbs_EDGE )
          aName += QString( ":edge_%1" ).arg( anIndex );
        else
	  aName += QString( ":face_%1" ).arg( anIndex );

	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );

	if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	  GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
	  aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	}
	else
	  aSelectedObject = aFindedObject; // get Object from study
      }
      else { // Global Selection
        if ( aShape.ShapeType() != aNeedType ) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }

    myEditCurrentArgument->setText( aName );
    if ( myEditCurrentArgument == GroupPlane->LineEdit1 && aNeedType == TopAbs_EDGE )
      myEdge = aSelectedObject;
    else if ( myEditCurrentArgument == GroupPlane->LineEdit1 && aNeedType == TopAbs_FACE )
      myFace = aSelectedObject;
  }
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  globalSelection(); // close previous local contexts
  if ( send == GroupPlane->PushButton1 ) {
    myEditCurrentArgument = GroupPlane->LineEdit1;
    if (GroupType->RadioButton1->isChecked())
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    else if (GroupType->RadioButton1->isChecked())
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPlane->LineEdit1 )  myEditCurrentArgument = GroupPlane->LineEdit1;
  else return;
  GEOMBase_Skeleton::LineEditReturnPressed();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void PrimitiveGUI_FaceDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void PrimitiveGUI_FaceDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_FaceDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetI3DPrimOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_FaceDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 )
    return true;
  else if ( id == 1 ) {
    if (GroupType->RadioButton1->isChecked())
      return !myEdge->_is_nil();
    else if (GroupType->RadioButton2->isChecked())
      return !myFace->_is_nil();
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_FaceDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;
  switch ( getConstructorId() ) {
  case 0 :
    anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeFaceHW( GroupDimensions->SpinBox_DX->value(), GroupDimensions->SpinBox_DY->value(), myOrientationType );
    res = true;
    break;
  case 1 :
    if ( GroupType->RadioButton1->isChecked() )
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeFaceObjHW( myEdge, GroupPlane->SpinBox_DX->value(), GroupPlane->SpinBox_DY->value() );
    else if (GroupType->RadioButton2->isChecked())
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow( getOperation() )->MakeFaceObjHW( myFace, GroupPlane->SpinBox_DX->value(), GroupPlane->SpinBox_DY->value() );
    res = true;
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
void PrimitiveGUI_FaceDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  switch ( getConstructorId() ) {
  case 1 :
    if ( GroupType->RadioButton1->isChecked() )
      objMap[GroupPlane->LineEdit1->text()] = myEdge;
    if ( GroupType->RadioButton2->isChecked() )
      objMap[GroupPlane->LineEdit1->text()] = myFace;
    break;
  }
  addSubshapesToFather( objMap );
}
