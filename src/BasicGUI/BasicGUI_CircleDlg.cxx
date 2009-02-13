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
// File   : BasicGUI_CircleDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BasicGUI_CircleDlg.h"

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

//=================================================================================
// class    : BasicGUI_CircleDlg()
// purpose  : Constructs a BasicGUI_CircleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_CircleDlg::BasicGUI_CircleDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
					bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CIRCLE_PV" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CIRCLE_PNTS" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CIRCLE_C2P" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CIRCLE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CIRCLE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setIcon( image3 );

  GroupPntVecR = new DlgRef_2Sel1Spin( centralWidget() );
  GroupPntVecR->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  
  GroupPntVecR->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) + " (Origin by default)" );
  GroupPntVecR->TextLabel2->setText( tr( "GEOM_VECTOR" ) + " (Z axis by default)" );
  GroupPntVecR->TextLabel3->setText( tr( "GEOM_RADIUS" ) );
  GroupPntVecR->PushButton1->setIcon( image1 );
  GroupPntVecR->PushButton2->setIcon( image1 );
  GroupPntVecR->PushButton1->setDown( true );

  GroupPntVecR->LineEdit1->setReadOnly( true );
  GroupPntVecR->LineEdit2->setReadOnly( true );

  GroupPntVecR->LineEdit1->setEnabled( true );
  GroupPntVecR->LineEdit2->setEnabled( false );

  Group3Pnts = new DlgRef_3Sel( centralWidget() );

  Group3Pnts->GroupBox1->setTitle( tr( "GEOM_3_POINTS" ) );
  Group3Pnts->TextLabel1->setText( tr( "GEOM_POINT1" ) );
  Group3Pnts->TextLabel2->setText( tr( "GEOM_POINT2" ) );
  Group3Pnts->TextLabel3->setText( tr( "GEOM_POINT3" ) );
  Group3Pnts->PushButton1->setIcon( image1 );
  Group3Pnts->PushButton2->setIcon( image1 );
  Group3Pnts->PushButton3->setIcon( image1 );
  Group3Pnts->PushButton1->setDown( true );

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );
  Group3Pnts->LineEdit1->setEnabled( true );
  Group3Pnts->LineEdit2->setEnabled( false );
  Group3Pnts->LineEdit3->setEnabled( false );

  GroupCenter2Pnts = new DlgRef_3Sel( centralWidget() );
  GroupCenter2Pnts->GroupBox1->setTitle( tr( "GEOM_CENTER_2POINTS" ) );
  GroupCenter2Pnts->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) );
  GroupCenter2Pnts->TextLabel2->setText( tr( "GEOM_POINT1" ) );
  GroupCenter2Pnts->TextLabel3->setText( tr( "GEOM_POINT2" ) );
  GroupCenter2Pnts->PushButton1->setIcon( image1 );
  GroupCenter2Pnts->PushButton2->setIcon( image1 );
  GroupCenter2Pnts->PushButton3->setIcon( image1 );
  GroupCenter2Pnts->PushButton1->setDown( true );

  GroupCenter2Pnts->LineEdit1->setReadOnly( true );
  GroupCenter2Pnts->LineEdit2->setReadOnly( true );
  GroupCenter2Pnts->LineEdit3->setReadOnly( true );

  GroupCenter2Pnts->LineEdit1->setEnabled( true );
  GroupCenter2Pnts->LineEdit2->setEnabled( false );
  GroupCenter2Pnts->LineEdit3->setEnabled( false );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPntVecR );
  layout->addWidget( Group3Pnts );
  layout->addWidget( GroupCenter2Pnts );
  /***************************************************************/

  setHelpFileName( "create_circle_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_CircleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_CircleDlg::~BasicGUI_CircleDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPntVecR->LineEdit1;

  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  // myGeomGUI->SetState( 0 );

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPntVecR->SpinBox_DX, 0.000001, COORD_MAX, aStep, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupPntVecR->SpinBox_DX->setValue( 100 );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  
  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPntVecR->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPntVecR->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton1,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton2,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton3,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupCenter2Pnts->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupCenter2Pnts->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupCenter2Pnts->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPntVecR->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPntVecR->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPntVecR->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT(ValueChangedInSpinBox() ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_CIRCLE" ) );

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_CircleDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPntVecR->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_CircleDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  switch ( constructorId ) {
  case 0:
    {
      Group3Pnts->hide();
      GroupCenter2Pnts->hide();
      GroupPntVecR->show();
      
      myEditCurrentArgument = GroupPntVecR->LineEdit1;
      GroupPntVecR->LineEdit1->setText( "" );
      GroupPntVecR->LineEdit2->setText( "" );
      GroupPntVecR->PushButton1->setDown( true );
      GroupPntVecR->PushButton2->setDown( false );
      GroupPntVecR->LineEdit1->setEnabled( true );
      GroupPntVecR->LineEdit2->setEnabled( false );
      break;
    }
  case 1:
    {
      GroupPntVecR->hide();
      GroupCenter2Pnts->hide();
      Group3Pnts->show();
      
      myEditCurrentArgument = Group3Pnts->LineEdit1;
      Group3Pnts->LineEdit1->setText( "" );
      Group3Pnts->LineEdit2->setText( "" );
      Group3Pnts->LineEdit3->setText( "" );
      Group3Pnts->PushButton1->setDown( true );
      Group3Pnts->PushButton2->setDown( false );
      Group3Pnts->PushButton3->setDown( false );
      Group3Pnts->LineEdit1->setEnabled( true );
      Group3Pnts->LineEdit2->setEnabled( false );
      Group3Pnts->LineEdit3->setEnabled( false );
      break;
    }
  case 2:
    {
      GroupPntVecR->hide();
      Group3Pnts->hide();		
      GroupCenter2Pnts->show();
      
      myEditCurrentArgument = GroupCenter2Pnts->LineEdit1;
      GroupCenter2Pnts->LineEdit1->setText( "" );
      GroupCenter2Pnts->LineEdit2->setText( "" );
      GroupCenter2Pnts->LineEdit3->setText( "" );
      GroupCenter2Pnts->PushButton1->setDown( true );
      GroupCenter2Pnts->PushButton2->setDown( false );
      GroupCenter2Pnts->PushButton3->setDown( false );
      GroupCenter2Pnts->LineEdit1->setEnabled( true );
      GroupCenter2Pnts->LineEdit2->setEnabled( false );
      GroupCenter2Pnts->LineEdit3->setEnabled( false );
      break;
    }
  }
  
  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  myEditCurrentArgument->setFocus();
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  displayPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::ClickOnApply()
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
void BasicGUI_CircleDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aList;
  aSelMgr->selectedObjects( aList );

  if ( aList.Extent() != 1 ) {
    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 )     myPoint  = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 )     myDir    = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )       myPoint1 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )       myPoint2 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )       myPoint3 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit1 ) myPoint4 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit2 ) myPoint5 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit3 ) myPoint6 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Handle(SALOME_InteractiveObject) anIO = aList.First();

  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO, aRes);
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
    QString aName = GEOMBase::GetName( aSelectedObject );

    TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
    if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 )
      aNeedType = TopAbs_EDGE;

    // If selected Vertex or Edge on the some Shape Get selection Subshape
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(anIO, aMap);
      if ( aMap.Extent() == 1 ) { // Local Selection
        int anIndex = aMap(1);
        if ( aNeedType == TopAbs_EDGE )
          aName += QString( ":edge_%1" ).arg( anIndex );
        else
          aName += QString( ":vertex_%1" ).arg( anIndex );

	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );

	if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	  GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
	  aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	}
	else {
	  aSelectedObject = aFindedObject; // get Object from study
	}
      }
      else { // Global Selection
        if ( aShape.ShapeType() != aNeedType ) {
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

    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 ) {
      myPoint  = aSelectedObject;
      if ( !myPoint->_is_nil() && myDir->_is_nil() )
	GroupPntVecR->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) {
      myDir    = aSelectedObject;
      if ( !myDir->_is_nil() && myPoint->_is_nil() )
	GroupPntVecR->PushButton1->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 ) {
      myPoint1 = aSelectedObject;
      if ( !myPoint1->_is_nil() && myPoint2->_is_nil() )
	Group3Pnts->PushButton2->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 ) {
      myPoint2 = aSelectedObject;
      if ( !myPoint2->_is_nil() && myPoint3->_is_nil() )
	Group3Pnts->PushButton3->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 ) {
      myPoint3 = aSelectedObject;
      if ( !myPoint3->_is_nil() && myPoint1->_is_nil() )
	Group3Pnts->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit1 ) {
      myPoint4 = aSelectedObject;
      if ( !myPoint4->_is_nil() && myPoint5->_is_nil() )
	GroupCenter2Pnts->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit2 ) {
      myPoint5 = aSelectedObject;
      if ( !myPoint5->_is_nil() && myPoint6->_is_nil() )
	GroupCenter2Pnts->PushButton3->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit3 ) {
      myPoint6 = aSelectedObject;
      if ( !myPoint6->_is_nil() && myPoint4->_is_nil() )
	GroupCenter2Pnts->PushButton1->click();
    }
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPntVecR->PushButton1 ) {
    myEditCurrentArgument = GroupPntVecR->LineEdit1;
    GroupPntVecR->PushButton2->setDown(false);
    GroupPntVecR->LineEdit1->setEnabled( true );
    GroupPntVecR->LineEdit2->setEnabled( false );
  }
  else if ( send == GroupPntVecR->PushButton2 ) {
    myEditCurrentArgument = GroupPntVecR->LineEdit2;
    GroupPntVecR->PushButton1->setDown(false);
    GroupPntVecR->LineEdit1->setEnabled( false );
    GroupPntVecR->LineEdit2->setEnabled( true );
  }
  else if ( send == Group3Pnts->PushButton1 ) {
    myEditCurrentArgument = Group3Pnts->LineEdit1;
    Group3Pnts->PushButton2->setDown( false );
    Group3Pnts->PushButton3->setDown( false );
    Group3Pnts->LineEdit1->setEnabled( true );
    Group3Pnts->LineEdit2->setEnabled( false );
    Group3Pnts->LineEdit3->setEnabled( false );
  }
  else if ( send == Group3Pnts->PushButton2 )  {
    myEditCurrentArgument = Group3Pnts->LineEdit2;
    Group3Pnts->PushButton1->setDown( false );
    Group3Pnts->PushButton3->setDown( false );
    Group3Pnts->LineEdit1->setEnabled( false );
    Group3Pnts->LineEdit2->setEnabled( true );
    Group3Pnts->LineEdit3->setEnabled( false );
  }
  else if ( send == Group3Pnts->PushButton3 ) {
    myEditCurrentArgument = Group3Pnts->LineEdit3;
    Group3Pnts->PushButton1->setDown( false );
    Group3Pnts->PushButton2->setDown( false );
    Group3Pnts->LineEdit1->setEnabled( false );
    Group3Pnts->LineEdit2->setEnabled( false );
    Group3Pnts->LineEdit3->setEnabled( true );
  }
  else if ( send == GroupCenter2Pnts->PushButton1 ) {
    myEditCurrentArgument = GroupCenter2Pnts->LineEdit1;
    GroupCenter2Pnts->PushButton2->setDown( false );
    GroupCenter2Pnts->PushButton3->setDown( false );
    GroupCenter2Pnts->LineEdit1->setEnabled( true );
    GroupCenter2Pnts->LineEdit2->setEnabled( false );
    GroupCenter2Pnts->LineEdit3->setEnabled( false );
  }
  else if ( send == GroupCenter2Pnts->PushButton2 ) {
    myEditCurrentArgument = GroupCenter2Pnts->LineEdit2;
    GroupCenter2Pnts->PushButton1->setDown( false );
    GroupCenter2Pnts->PushButton3->setDown( false );
    GroupCenter2Pnts->LineEdit1->setEnabled( false );
    GroupCenter2Pnts->LineEdit2->setEnabled( true );
    GroupCenter2Pnts->LineEdit3->setEnabled( false );
    
  }
  else if ( send == GroupCenter2Pnts->PushButton3 ) {
    myEditCurrentArgument = GroupCenter2Pnts->LineEdit3;
    GroupCenter2Pnts->PushButton1->setDown( false );
    GroupCenter2Pnts->PushButton2->setDown( false );
    GroupCenter2Pnts->LineEdit1->setEnabled( false );
    GroupCenter2Pnts->LineEdit2->setEnabled( false );
    GroupCenter2Pnts->LineEdit3->setEnabled( true );
  }

  
  myEditCurrentArgument->setFocus();

  if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else {
    globalSelection(); // close local contexts, if any
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  }

  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  send->setDown(true);
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPntVecR->LineEdit1 ||
       send == GroupPntVecR->LineEdit2 ||
       send == Group3Pnts->LineEdit1 ||
       send == Group3Pnts->LineEdit2 ||
       send == Group3Pnts->LineEdit3 ||
       send == GroupCenter2Pnts->LineEdit1 ||
       send == GroupCenter2Pnts->LineEdit2 ||
       send == GroupCenter2Pnts->LineEdit3 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_CircleDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : getRadius()
// purpose  :
//=================================================================================
double BasicGUI_CircleDlg::getRadius() const
{
  double r = 0.;
  switch ( getConstructorId() ) {
  case 0:
    r = GroupPntVecR->SpinBox_DX->value(); break;
  }
  return r;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_CircleDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isEqual
// purpose  : it may also be needed to check for min distance between gp_Pnt-s...
//=================================================================================
static bool isEqual( const GEOM::GEOM_Object_var& thePnt1, const GEOM::GEOM_Object_var& thePnt2 )
{
  return thePnt1->_is_equivalent( thePnt2 );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 ) {
    //return !myPoint->_is_nil() && !myDir->_is_nil() && getRadius() > 0;
    //nil point means origin of global CS
    //nil vector means Z axis
    bool ok = GroupPntVecR->SpinBox_DX->isValid(msg, !IsPreview());
    return getRadius() > 0 && ok;
  }
  else if ( id == 1 )
    return !myPoint1->_is_nil() && !myPoint2->_is_nil() && !myPoint3->_is_nil() &&
      !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 );
  else if ( id == 2 )
    return !myPoint4->_is_nil() && !myPoint5->_is_nil() && !myPoint6->_is_nil() &&
      !isEqual( myPoint4, myPoint5 ) && !isEqual( myPoint5, myPoint6 ) && !isEqual( myPoint4, myPoint6 );
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() ) {
  case 0 :
    {
      QStringList aParameters;
      aParameters << GroupPntVecR->SpinBox_DX->text();
      anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeCirclePntVecR( myPoint, myDir, getRadius() );
      if ( !anObj->_is_nil() && !IsPreview() )
        anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
      res = true;
      break;
    }
  case 1 :
    anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeCircleThreePnt( myPoint1, myPoint2, myPoint3 );
    res = true;
    break;
  case 2:
    anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeCircleCenter2Pnt( myPoint4, myPoint5, myPoint6 );
    res = true;
    break;
  }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  else {
    MESSAGE( "Execute Object is NULL!" );
  }
  
  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_CircleDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch ( getConstructorId() ) {
  case 0:
    if (!CORBA::is_nil(myPoint))
      objMap[GroupPntVecR->LineEdit1->text()] = myPoint;
    if (!CORBA::is_nil(myDir))
      objMap[GroupPntVecR->LineEdit2->text()] = myDir;
    break;
  case 1:
    objMap[Group3Pnts->LineEdit1->text()] = myPoint1;
    objMap[Group3Pnts->LineEdit2->text()] = myPoint2;
    objMap[Group3Pnts->LineEdit3->text()] = myPoint3;
    break;
  case 2:
    objMap[GroupCenter2Pnts->LineEdit1->text()] = myPoint4;
    objMap[GroupCenter2Pnts->LineEdit2->text()] = myPoint5;
    objMap[GroupCenter2Pnts->LineEdit3->text()] = myPoint6;
    break;
  }
  addSubshapesToFather( objMap );
}
