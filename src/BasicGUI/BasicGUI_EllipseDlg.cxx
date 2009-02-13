//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : BasicGUI_EllipseDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#include "BasicGUI_EllipseDlg.h"

#include <DlgRef.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_EllipseDlg()
// purpose  : Constructs a BasicGUI_EllipseDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_EllipseDlg::BasicGUI_EllipseDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
					  bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_ELLIPSE_PV" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_ELLIPSE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_ELLIPSE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel2Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_CENTER" ) + " (Origin by default)" );
  GroupPoints->TextLabel2->setText( tr( "GEOM_VECTOR" ) + " (Z axis by default)" );
  GroupPoints->TextLabel3->setText( tr( "GEOM_RADIUS_MAJOR" ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_RADIUS_MINOR" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );

  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  /***************************************************************/

  setHelpFileName( "create_ellipse_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_EllipseDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_EllipseDlg::~BasicGUI_EllipseDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  GroupPoints->PushButton1->setDown(true);

  myPoint = myDir = GEOM::GEOM_Object::_nil();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  double aMajorR( 200. ), aMinorR( 100. );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, 0.000001, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupPoints->SpinBox_DY, 0.000001, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupPoints->SpinBox_DX->setValue( aMajorR );
  GroupPoints->SpinBox_DY->setValue( aMinorR );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );
  
  connect( myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_ELLIPSE" ) );

  displayPreview();
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_EllipseDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupPoints->SpinBox_DY->setSingleStep(step);
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  // reset
  myPoint = myDir = GEOM::GEOM_Object::_nil();
  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  GroupPoints->PushButton1->setDown(true);
  GroupPoints->PushButton2->setDown(false);
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );
  myEditCurrentArgument = GroupPoints->LineEdit1;

  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  displayPreview();
  
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_EllipseDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myDir   = GEOM::GEOM_Object::_nil();
    return;
  }

  Standard_Boolean aRes = Standard_False;
  Handle(SALOME_InteractiveObject) anIO = aSelList.First();
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO, aRes);
  if (!CORBA::is_nil(aSelectedObject) && aRes) {
    QString aName = GEOMBase::GetName(aSelectedObject);
    
    // Get Selected object if selected subshape
    TopoDS_Shape aShape;

    TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
    if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
      aNeedType = TopAbs_EDGE;
    
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(anIO, aMap);
      if (aMap.Extent() == 1) {
        int anIndex = aMap(1);
        if (aNeedType == TopAbs_EDGE)
          aName += QString(":edge_%1").arg(anIndex);
        else
          aName += QString(":vertex_%1").arg(anIndex);

	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

	if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
          GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
          aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	} 
	else {
	  aSelectedObject = aFindedObject; // get Object from study
	}
      }
      else { // Global Selection
        if (aShape.ShapeType() != aNeedType) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }
    
    myEditCurrentArgument->setText( aName );

    if (!aSelectedObject->_is_nil()) { // clear selection if something selected
      globalSelection();
      localSelection( GEOM::GEOM_Object::_nil(), aNeedType );      
    }

    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
      myPoint = aSelectedObject;
      if ( !myPoint->_is_nil() && myDir->_is_nil() )
	GroupPoints->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myDir   = aSelectedObject;
      if ( !myDir->_is_nil() && myPoint->_is_nil() )
	GroupPoints->PushButton1->click();
    }
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled( true );
    GroupPoints->LineEdit2->setEnabled( false );
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled( false );
    GroupPoints->LineEdit2->setEnabled( true );
  }

  globalSelection(); // close local contexts, if any
  TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
  if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
    aNeedType = TopAbs_EDGE;
  localSelection( GEOM::GEOM_Object::_nil(), aNeedType );
  
  myEditCurrentArgument->setFocus();
  //SelectionIntoArgument();
  send->setDown(true);
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );
  
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );

  myPoint = myDir = GEOM::GEOM_Object::_nil();
  //globalSelection( GEOM_POINT );
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_EllipseDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_EllipseDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::isValid( QString& msg )
{
  double aMajorR = GroupPoints->SpinBox_DX->value();
  double aMinorR = GroupPoints->SpinBox_DY->value();
  if ( aMajorR < aMinorR ) {
    msg = tr( "GEOM_ELLIPSE_ERROR_1" );
    return false;
  }
  //return !myPoint->_is_nil() && !myDir->_is_nil();
  //nil point means origin of global CS
  //nil vector means Z axis
  bool ok = true;
  ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::execute( ObjectList& objects )
{
  double aMajorR = GroupPoints->SpinBox_DX->value();
  double aMinorR = GroupPoints->SpinBox_DY->value();

  QStringList aParameters;
  aParameters<<GroupPoints->SpinBox_DX->text();
  aParameters<<GroupPoints->SpinBox_DY->text();
  
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeEllipse( myPoint, myDir, aMajorR, aMinorR );
  if ( !anObj->_is_nil() ) {
    if ( !IsPreview() )
      anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
    objects.push_back( anObj._retn() );
  }
  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_EllipseDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  if (!CORBA::is_nil(myPoint))
    objMap[GroupPoints->LineEdit1->text()] = myPoint;
  if (!CORBA::is_nil(myDir))
    objMap[GroupPoints->LineEdit2->text()] = myDir;

  addSubshapesToFather( objMap );
}
