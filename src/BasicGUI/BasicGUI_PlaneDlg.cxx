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
// File   : BasicGUI_PlaneDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BasicGUI_PlaneDlg.h"

#include <DlgRef.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_PlaneDlg()
// purpose  : Constructs a BasicGUI_PlaneDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PlaneDlg::BasicGUI_PlaneDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_PLANE_PV" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_PLANE_3PNTS" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_PLANE_FACE" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image4( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_WPLANE_VECTOR" ) ) );
  QPixmap image5( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_WPLANE_ORIGIN" ) ) );

  setWindowTitle( tr( "GEOM_PLANE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_PLANE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setIcon( image2 );
  mainFrame()->RadioButton4->show();
  mainFrame()->RadioButton4->setIcon( image4 );
  mainFrame()->RadioButton5->show();
  mainFrame()->RadioButton5->setIcon( image5 );

  GroupPntDir = new DlgRef_2Sel1Spin( centralWidget() );
  GroupPntDir->GroupBox1->setTitle( tr( "GEOM_PLANE_PV" ) );
  GroupPntDir->TextLabel1->setText( tr( "GEOM_POINT" ) );
  GroupPntDir->TextLabel2->setText( tr( "GEOM_VECTOR" ) );
  GroupPntDir->TextLabel3->setText( tr( "GEOM_PLANE_SIZE" ) );
  GroupPntDir->PushButton1->setIcon( image3 );
  GroupPntDir->PushButton2->setIcon( image3 );
  GroupPntDir->LineEdit1->setReadOnly( true );
  GroupPntDir->LineEdit2->setReadOnly( true );
  GroupPntDir->PushButton1->setDown( true );
  GroupPntDir->LineEdit1->setEnabled( true );
  GroupPntDir->LineEdit2->setEnabled( false );

  Group3Pnts = new DlgRef_3Sel1Spin( centralWidget() );
  Group3Pnts->GroupBox1->setTitle( tr( "GEOM_3_POINTS" ) );
  Group3Pnts->TextLabel1->setText( tr( "GEOM_POINT1" ) );
  Group3Pnts->TextLabel2->setText( tr( "GEOM_POINT2" ) );
  Group3Pnts->TextLabel3->setText( tr( "GEOM_POINT3" ) );
  Group3Pnts->TextLabel4->setText( tr( "GEOM_PLANE_SIZE" ) );
  Group3Pnts->PushButton1->setIcon( image3 );
  Group3Pnts->PushButton2->setIcon( image3 );
  Group3Pnts->PushButton3->setIcon( image3 );
  Group3Pnts->PushButton1->setDown( true );

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );
  Group3Pnts->LineEdit1->setEnabled( true );
  Group3Pnts->LineEdit2->setEnabled( false );
  Group3Pnts->LineEdit3->setEnabled( false );

  GroupFace = new DlgRef_1Sel1Spin( centralWidget() );
  GroupFace->GroupBox1->setTitle( tr( "GEOM_FACE" ) );
  GroupFace->TextLabel1->setText( tr( "GEOM_SELECTION" ) );
  GroupFace->TextLabel2->setText( tr( "GEOM_PLANE_SIZE" ) );
  GroupFace->PushButton1->setIcon( image3 );
  GroupFace->PushButton1->setDown( true );

  GroupFace->LineEdit1->setReadOnly( true );

  Group2Vec = new DlgRef_2Sel1Spin( centralWidget() );

  Group2Vec->GroupBox1->setTitle( tr( "GEOM_WPLANE_VECTOR" ) );
  Group2Vec->TextLabel1->setText( tr( "GEOM_WPLANE_VX" ) );
  Group2Vec->TextLabel2->setText( tr( "GEOM_WPLANE_VZ" ) );
  Group2Vec->PushButton1->setIcon( image3 );
  Group2Vec->PushButton2->setIcon( image3 );
  Group2Vec->LineEdit1->setReadOnly( true );
  Group2Vec->LineEdit2->setReadOnly( true );
  Group2Vec->PushButton1->setDown( true );
  Group2Vec->LineEdit1->setEnabled( true );
  Group2Vec->LineEdit2->setEnabled( false );
  Group2Vec->TextLabel3->setText( tr( "GEOM_PLANE_SIZE" ) );

  GroupLCS = new DlgRef_3Radio1Sel1Spin( centralWidget() );
  GroupLCS->GroupBox1->setTitle( tr( "GEOM_LCS" ) );
  GroupLCS->TextLabel1->setText( tr( "GEOM_SELECTION" ) );
  GroupLCS->TextLabel2->setText( tr( "GEOM_PLANE_SIZE" ) );
  GroupLCS->RadioButton1->setText( tr( "GEOM_WPLANE_OXY" ) );
  GroupLCS->RadioButton2->setText( tr( "GEOM_WPLANE_OYZ" ) );
  GroupLCS->RadioButton3->setText( tr( "GEOM_WPLANE_OZX" ) );
  GroupLCS->PushButton1->setIcon( image3 );
  GroupLCS->PushButton1->setDown( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPntDir );
  layout->addWidget( Group3Pnts );
  layout->addWidget( GroupFace );
  layout->addWidget( Group2Vec );
  layout->addWidget( GroupLCS );
  /***************************************************************/

  setHelpFileName( "create_plane_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_PlaneDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_PlaneDlg::~BasicGUI_PlaneDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPntDir->LineEdit1;

  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = myFace = GEOM::GEOM_Object::_nil();

  // myGeomGUI->SetState( 0 );
  myOriginType = 1;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  double aTrimSize = 2000.0;

  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupPntDir->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
  GroupPntDir->SpinBox_DX->setValue( aTrimSize );
  initSpinBox( Group3Pnts->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
  Group3Pnts->SpinBox_DX->setValue( aTrimSize );
  initSpinBox( GroupFace->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
  GroupFace->SpinBox_DX->setValue( aTrimSize );
  initSpinBox( Group2Vec->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
  Group2Vec->SpinBox_DX->setValue( aTrimSize );
  initSpinBox( GroupLCS->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
  GroupLCS->SpinBox_DX->setValue( aTrimSize );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPntDir->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPntDir->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton1,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton2,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton3,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupFace->PushButton1,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2Vec->PushButton1,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2Vec->PushButton2,   SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupLCS->RadioButton1,   SIGNAL( clicked() ), this, SLOT( GroupClicked() ) );
  connect( GroupLCS->RadioButton2,   SIGNAL( clicked() ), this, SLOT( GroupClicked() ) );
  connect( GroupLCS->RadioButton3,   SIGNAL( clicked() ), this, SLOT( GroupClicked() ) );

  connect( GroupPntDir->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPntDir->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts->LineEdit1,  SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts->LineEdit2,  SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group3Pnts->LineEdit3,  SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupFace->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group2Vec->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( Group2Vec->LineEdit2,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPntDir->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group3Pnts->SpinBox_DX,  SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupFace->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( Group2Vec->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupLCS->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_PLANE" ) );

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_PlaneDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPntDir->SpinBox_DX->setSingleStep(step);
  Group3Pnts->SpinBox_DX->setSingleStep(step);
  GroupFace->SpinBox_DX->setSingleStep(step);
  Group2Vec->SpinBox_DX->setSingleStep(step);
  GroupLCS->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PlaneDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = myFace = GEOM::GEOM_Object::_nil();

  switch ( constructorId ) {
  case 0: /* plane from a point and a direction (vector, edge...) */
    {
      Group3Pnts->hide();
      GroupFace->hide();
      GroupPntDir->show();
      Group2Vec->hide();
      GroupLCS->hide();
      
      myEditCurrentArgument = GroupPntDir->LineEdit1;
      GroupPntDir->LineEdit1->setText( "" );
      GroupPntDir->LineEdit2->setText( "" );
      GroupPntDir->PushButton1->setDown( true );
      GroupPntDir->PushButton2->setDown( false );
      GroupPntDir->LineEdit1->setEnabled( true );
      GroupPntDir->LineEdit2->setEnabled( false );
      
      /* for the first argument */
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      break;
    }
  case 1: /* plane from 3 points */
    {
      GroupPntDir->hide();
      GroupFace->hide();
      Group3Pnts->show();
      Group2Vec->hide();
      GroupLCS->hide();
      
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
      
      /* for the first argument */
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      break;
    }
  case 2: /* plane from a planar face */
    {
      GroupPntDir->hide();
      Group3Pnts->hide();
      GroupFace->show();
      Group2Vec->hide();
      GroupLCS->hide();
      
      myEditCurrentArgument = GroupFace->LineEdit1;
      GroupFace->LineEdit1->setText( "" );
      GroupFace->PushButton1->setDown( true );
      
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
      break;
    }
  case 3: /* plane from a 2 Vectors */
    {
      GroupPntDir->hide();
      Group3Pnts->hide();
      GroupFace->hide();
      Group2Vec->show();
      GroupLCS->hide();
      
      myEditCurrentArgument = Group2Vec->LineEdit1;
      Group2Vec->LineEdit1->setText( "" );
      Group2Vec->PushButton1->setDown( true );
      
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
      break;
    }
  case 4: /* plane from a LCS */
    {
      GroupPntDir->hide();
      Group3Pnts->hide();
      GroupFace->hide();
      Group2Vec->hide();
      GroupLCS->show();
      
      myEditCurrentArgument = GroupLCS->LineEdit1;
      GroupLCS->LineEdit1->setText( "" );
      GroupLCS->PushButton1->setDown( true );
      GroupLCS->RadioButton1->setChecked( true );
      myOriginType = 1;
      
      globalSelection(GEOM_MARKER);
      break;
    }
  }
  
  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  myEditCurrentArgument->setFocus();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  displayPreview();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_PlaneDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}

//=================================================================================
// function : GroupClicked()
// purpose  : OX OY OZ Radio button management
//=================================================================================
void BasicGUI_PlaneDlg::GroupClicked()
{
  QRadioButton* send = (QRadioButton*)sender();

  if ( send == GroupLCS->RadioButton1 )
    myOriginType = 1;
  else if ( send == GroupLCS->RadioButton2 )
    myOriginType = 2;
  else if ( send == GroupLCS->RadioButton3 )
    myOriginType = 3;
  displayPreview();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BasicGUI_PlaneDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      ( myEditCurrentArgument == GroupPntDir->LineEdit1 ) myPoint  = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPntDir->LineEdit2 ) myDir    = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )  myPoint1 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )  myPoint2 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )  myPoint3 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupFace->LineEdit1 )   myFace   = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group2Vec->LineEdit1 )   myVec1   = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group2Vec->LineEdit2 )   myVec2   = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupLCS->LineEdit1 )    myLCS   = GEOM::GEOM_Object::_nil();
    displayPreview();
    return;
  }

  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aRes);
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
    QString aName = GEOMBase::GetName( aSelectedObject );
    TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
    if ( myEditCurrentArgument == GroupPntDir->LineEdit2 || myEditCurrentArgument == Group2Vec->LineEdit1 || myEditCurrentArgument == Group2Vec->LineEdit2)
      aNeedType = TopAbs_EDGE;
    else if ( myEditCurrentArgument == GroupFace->LineEdit1 )
      aNeedType = TopAbs_FACE;
    else if ( myEditCurrentArgument == GroupLCS->LineEdit1 )
      aNeedType = TopAbs_FACE;

    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if ( aMap.Extent() == 1 ) { // Local Selection
        int anIndex = aMap( 1 );
        if ( aNeedType == TopAbs_EDGE )
          aName += QString( ":edge_%1" ).arg( anIndex );
        else if (aNeedType == TopAbs_FACE)
          aName += QString( ":face_%1" ).arg( anIndex );
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

    /*    if (!aSelectedObject->_is_nil()) { // clear selection if something selected
      globalSelection();
      if ( myEditCurrentArgument == GroupFace->LineEdit1 ) {
        TColStd_MapOfInteger aMap;
        aMap.Add( GEOM_PLANE );
        aMap.Add( GEOM_MARKER );
        globalSelection( aMap );
      }
      else
        localSelection( GEOM::GEOM_Object::_nil(), aNeedType );
        }*/

    if      ( myEditCurrentArgument == GroupPntDir->LineEdit1 ) {
      myPoint  = aSelectedObject;
      if ( !myPoint->_is_nil() && myDir->_is_nil() )
        GroupPntDir->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPntDir->LineEdit2 ) {
      myDir    = aSelectedObject;
      if ( !myDir->_is_nil() && myPoint->_is_nil() )
        GroupPntDir->PushButton1->click();
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
    else if ( myEditCurrentArgument == GroupFace->LineEdit1 )
      myFace   = aSelectedObject;
    else if ( myEditCurrentArgument == Group2Vec->LineEdit1 ) {
      myVec1 = aSelectedObject;
      if ( !myVec1->_is_nil() && myVec2->_is_nil() )
        Group2Vec->PushButton2->click();
    } else if ( myEditCurrentArgument == Group2Vec->LineEdit2 ) {
      myVec2 = aSelectedObject;
      if ( !myVec2->_is_nil() && myVec1->_is_nil() )
        Group2Vec->PushButton1->click();
    } else if ( myEditCurrentArgument == GroupLCS->LineEdit1 )
      myLCS = aSelectedObject;

  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_POINT );

  if      ( send == GroupPntDir->PushButton1 ) {
    myEditCurrentArgument = GroupPntDir->LineEdit1;
    GroupPntDir->PushButton2->setDown( false );
    GroupPntDir->LineEdit1->setEnabled( true );
    GroupPntDir->LineEdit2->setEnabled( false );
  }
  else if ( send == GroupPntDir->PushButton2 ) {
    myEditCurrentArgument = GroupPntDir->LineEdit2;
    GroupPntDir->PushButton1->setDown( false );
    GroupPntDir->LineEdit1->setEnabled( false );
    GroupPntDir->LineEdit2->setEnabled( true );
  }
  else if ( send == Group3Pnts->PushButton1 ) {
    myEditCurrentArgument = Group3Pnts->LineEdit1;
    Group3Pnts->PushButton2->setDown( false );
    Group3Pnts->PushButton3->setDown( false );
    Group3Pnts->LineEdit1->setEnabled( true );
    Group3Pnts->LineEdit2->setEnabled( false );
    Group3Pnts->LineEdit3->setEnabled( false );
  }
  else if ( send == Group3Pnts->PushButton2 ) {
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
  else if ( send == GroupFace->PushButton1 ) {
    myEditCurrentArgument = GroupFace->LineEdit1;
    GroupFace->PushButton1->setDown( true );
  } else if ( send == Group2Vec->PushButton1 ) {
    myEditCurrentArgument = Group2Vec->LineEdit1;
    Group2Vec->PushButton2->setDown( false );
    Group2Vec->LineEdit1->setEnabled( true );
    Group2Vec->LineEdit2->setEnabled( false );
  } else if ( send == Group2Vec->PushButton2 ) {
    myEditCurrentArgument = Group2Vec->LineEdit2;
    Group2Vec->PushButton1->setDown( false );
    Group2Vec->LineEdit1->setEnabled( false );
    Group2Vec->LineEdit2->setEnabled( true );
  } else if ( send == GroupLCS->PushButton1 ) {
    myEditCurrentArgument = GroupLCS->LineEdit1;
    GroupLCS->LineEdit1->setEnabled( true );
  }

  myEditCurrentArgument->setFocus();

  if ( myEditCurrentArgument == GroupPntDir->LineEdit2 || 
       myEditCurrentArgument == Group2Vec->LineEdit1   ||
       myEditCurrentArgument == Group2Vec->LineEdit2 ) {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  } else if ( myEditCurrentArgument == GroupFace->LineEdit1 ) {
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_PLANE );
    aMap.Add( GEOM_MARKER );
    globalSelection( aMap );
  } else if ( myEditCurrentArgument == GroupLCS->LineEdit1 ) {
    globalSelection( GEOM_MARKER );
  }
  else { // 3 Pnts
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  }

  //  SelectionIntoArgument();
  myEditCurrentArgument->setFocus();
  send->setDown(true);
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPntDir->LineEdit1 ||
       send == GroupPntDir->LineEdit2 ||
       send == Group3Pnts->LineEdit1 ||
       send == Group3Pnts->LineEdit2 ||
       send == Group3Pnts->LineEdit3 ||
       send == GroupFace->LineEdit1  ||
       send == Group2Vec->LineEdit1  ||
       send == Group2Vec->LineEdit2  ||
       send == GroupLCS->LineEdit1  ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
  SelectionIntoArgument();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PlaneDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview();
}

//=================================================================================
// function : getSize()
// purpose  :
//=================================================================================
double BasicGUI_PlaneDlg::getSize() const
{
  switch ( getConstructorId() ) {
  case 0 : return GroupPntDir->SpinBox_DX->value();
  case 1 : return Group3Pnts->SpinBox_DX->value();
  case 2 : return GroupFace->SpinBox_DX->value();
  case 3 : return Group2Vec->SpinBox_DX->value();
  case 4 : return GroupLCS->SpinBox_DX->value();
  }
  return 0.;
}

//=================================================================================
// function : getSize()
// purpose  :
//=================================================================================
QString BasicGUI_PlaneDlg::getSizeAsString() const
{
  switch ( getConstructorId() ) {
  case 0 : return GroupPntDir->SpinBox_DX->text();
  case 1 : return Group3Pnts->SpinBox_DX->text();
  case 2 : return GroupFace->SpinBox_DX->text();
  case 3 : return Group2Vec->SpinBox_DX->text();
  }
  return QString();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_PlaneDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
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
bool BasicGUI_PlaneDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( getSize() <= 0 ) {
    msg = QString( "Please, enter size greater than 0." );
    return false;
  }

  if ( id == 0 ) { 
    bool ok = GroupPntDir->SpinBox_DX->isValid( msg, !IsPreview() );
    return !CORBA::is_nil( myPoint ) && !CORBA::is_nil( myDir ) && ok;
  }
  else if ( id == 1 ) {
    bool ok = Group3Pnts->SpinBox_DX->isValid( msg, !IsPreview() );
    return !CORBA::is_nil( myPoint1  ) && !CORBA::is_nil( myPoint2 ) && !CORBA::is_nil( myPoint3 ) &&
      !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 ) && ok;
  }
  else if ( id == 2 ) {
    bool ok = GroupFace->SpinBox_DX->isValid( msg, !IsPreview() );
    return !CORBA::is_nil( myFace ) && ok;
  }   else if ( id == 3 ) {
    bool ok = Group2Vec->SpinBox_DX->isValid( msg, !IsPreview() );
    return !CORBA::is_nil( myVec1  ) && !CORBA::is_nil( myVec2 ) && !isEqual( myVec1, myVec2 ) && ok;
  } else if ( id == 4 ) {
    bool ok = GroupLCS->SpinBox_DX->isValid( msg, !IsPreview() );
    return ok;
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_PlaneDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );

  switch ( getConstructorId() ) {
  case 0 :
    anObj = anOper->MakePlanePntVec( myPoint, myDir, getSize() );
    res = true;
    break;
  case 1 :
    anObj = anOper->MakePlaneThreePnt( myPoint1, myPoint2, myPoint3, getSize() );
    res = true;
    break;
  case 2 :
    anObj = anOper->MakePlaneFace( myFace, getSize() );
    res = true;
    break;
  case 3 :
    anObj = anOper->MakePlane2Vec( myVec1, myVec2, getSize() );
    res = true;
    break;
  case 4 :
    anObj = anOper->MakePlaneLCS( myLCS, getSize(), myOriginType );
    res = true;
    break;
  }
  
  if ( !anObj->_is_nil() ) {
    if ( !IsPreview() )
      anObj->SetParameters(getSizeAsString().toLatin1().constData());
    objects.push_back( anObj._retn() );
  }
  return res;
}
//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_PlaneDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch ( getConstructorId() ) {
  case 0:
    objMap[GroupPntDir->LineEdit1->text()] = myPoint;
    objMap[GroupPntDir->LineEdit2->text()] = myDir;
    break;
  case 1:
    objMap[Group3Pnts->LineEdit1->text()] = myPoint1;
    objMap[Group3Pnts->LineEdit2->text()] = myPoint2;
    objMap[Group3Pnts->LineEdit3->text()] = myPoint3;
    break;
  case 2:
    objMap[GroupFace->LineEdit1->text()] = myFace;
    break;
  case 3:
    objMap[Group2Vec->LineEdit1->text()] = myVec1;
    objMap[Group2Vec->LineEdit2->text()] = myVec2;
    break;
  }
  addSubshapesToFather( objMap );
}
