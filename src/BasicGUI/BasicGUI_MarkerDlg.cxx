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
// File   : BasicGUI_MarkerDlg.cxx
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)
//
#include "BasicGUI_MarkerDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <QLabel>

#include <GEOMImpl_Types.hxx>

// OCCT Includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <Geom_Plane.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

//=================================================================================
// class    : BasicGUI_MarkerDlg()
// purpose  : Constructor
//=================================================================================
BasicGUI_MarkerDlg::BasicGUI_MarkerDlg( GeometryGUI* theGeometryGUI, QWidget* theParent )
  : GEOMBase_Skeleton( theGeometryGUI, theParent, false,
                       Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  QPixmap iconCS1   ( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_MARKER" ) ) );
  QPixmap iconCS2   ( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_MARKER2" ) ) );
  QPixmap iconCS3   ( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_MARKER3" ) ) );
  QPixmap iconSelect( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "CAPTION" ) );

  mainFrame()->GroupConstructors->setTitle( tr( "LOCALCS" ) );
  mainFrame()->RadioButton1->setIcon( iconCS1 );
  mainFrame()->RadioButton2->setIcon( iconCS2 );
  mainFrame()->RadioButton3->setIcon( iconCS3 );

  Group1 = new DlgRef_1Sel( centralWidget() );

  Group1->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  Group1->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  Group1->PushButton1->setIcon( iconSelect );
  Group1->PushButton1->setDown( true );

  Group2 = new DlgRef_3Sel( centralWidget() );

  Group2->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  Group2->TextLabel1->setText( tr( "GEOM_POINT" ) );
  Group2->TextLabel2->setText( tr( "XDIR" ) );
  Group2->TextLabel3->setText( tr( "YDIR" ) );
  Group2->PushButton1->setIcon( iconSelect );
  Group2->PushButton2->setIcon( iconSelect );
  Group2->PushButton3->setIcon( iconSelect );
  Group2->PushButton1->setDown( true );

  aMainGrp = new QFrame( centralWidget() );
  aMainGrp->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
  aMainGrp->setContentsMargins( 0, 0, 0, 0 );
  QHBoxLayout* aMainGrpLayout = new QHBoxLayout( aMainGrp );
  aMainGrpLayout->setMargin( 0 );
  
  QGroupBox* anOriGrp = new QGroupBox( tr( "ORIGIN" ), aMainGrp );
  QVBoxLayout* anOriGrpLayout = new QVBoxLayout( anOriGrp );

  anOriGrpLayout->addWidget( new QLabel( tr( "GEOM_X" ), anOriGrp ) );
  myData[ X ] = new SalomeApp_DoubleSpinBox( anOriGrp );
  anOriGrpLayout->addWidget( myData[ X ] );
  anOriGrpLayout->addWidget( new QLabel( tr( "GEOM_Y" ), anOriGrp ) );
  myData[ Y ] = new SalomeApp_DoubleSpinBox( anOriGrp );
  anOriGrpLayout->addWidget( myData[ Y ] );
  anOriGrpLayout->addWidget( new QLabel( tr( "GEOM_Z" ), anOriGrp ) );
  myData[ Z ] = new SalomeApp_DoubleSpinBox( anOriGrp );
  anOriGrpLayout->addWidget( myData[ Z ] );

  aMainGrpLayout->addWidget( anOriGrp );

  QGroupBox* aXAxisGrp = new QGroupBox( tr( "XDIR" ), aMainGrp );
  QVBoxLayout* aXAxisGrpLayout = new QVBoxLayout( aXAxisGrp );

  aXAxisGrpLayout->addWidget( new QLabel( tr( "DX" ), aXAxisGrp ) );
  myData[ DX1 ] = new SalomeApp_DoubleSpinBox( aXAxisGrp );
  aXAxisGrpLayout->addWidget( myData[ DX1 ] );
  aXAxisGrpLayout->addWidget( new QLabel( tr( "DY" ), aXAxisGrp ) );
  myData[ DY1 ] = new SalomeApp_DoubleSpinBox( aXAxisGrp );
  aXAxisGrpLayout->addWidget( myData[ DY1 ] );
  aXAxisGrpLayout->addWidget( new QLabel( tr( "DZ" ), aXAxisGrp ) );
  myData[ DZ1 ] = new SalomeApp_DoubleSpinBox( aXAxisGrp );
  aXAxisGrpLayout->addWidget( myData[ DZ1 ] );

  aMainGrpLayout->addWidget( aXAxisGrp );

  QGroupBox* anYAxisGrp = new QGroupBox( tr( "YDIR" ), aMainGrp );
  QVBoxLayout* anYAxisGrpLayout = new QVBoxLayout( anYAxisGrp );

  anYAxisGrpLayout->addWidget( new QLabel( tr( "DX" ), anYAxisGrp ) );
  myData[ DX2 ] = new SalomeApp_DoubleSpinBox( anYAxisGrp );
  anYAxisGrpLayout->addWidget( myData[ DX2 ] );
  anYAxisGrpLayout->addWidget( new QLabel( tr( "DY" ), anYAxisGrp ) );
  myData[ DY2 ] = new SalomeApp_DoubleSpinBox( anYAxisGrp );
  anYAxisGrpLayout->addWidget( myData[ DY2 ] );
  anYAxisGrpLayout->addWidget( new QLabel( tr( "DZ" ), anYAxisGrp ) );
  myData[ DZ2 ] = new SalomeApp_DoubleSpinBox( anYAxisGrp );
  anYAxisGrpLayout->addWidget( myData[ DZ2 ] );

  aMainGrpLayout->addWidget( anYAxisGrp );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( aMainGrp );
  layout->addWidget( Group1 );
  layout->addWidget( Group2 );

  setHelpFileName( "create_lcs_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_MarkerDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_MarkerDlg::~BasicGUI_MarkerDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::Init()
{
  myShape.nullify();
  myPoint.nullify();
  myVectorX.nullify();
  myVectorY.nullify();
  myBlockPreview = false;
  myConstructorId = -1;
  myEditCurrentArgument = Group1->LineEdit1;
  Group1->LineEdit1->setReadOnly( true );
  Group2->LineEdit1->setReadOnly( true );
  Group2->LineEdit2->setReadOnly( true );
  Group2->LineEdit3->setReadOnly( true );
  Group2->LineEdit1->setEnabled( true );
  Group2->LineEdit2->setEnabled( false );
  Group2->LineEdit3->setEnabled( false );

  connect( this,                SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( Group1->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2->PushButton2, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( Group2->PushButton3, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );

  connect( myGeomGUI,           SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( onDeactivate() ) );
  connect( myGeomGUI,           SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),          SIGNAL( clicked() ), this, SLOT( onOk() ) );
  connect( buttonApply(),       SIGNAL( clicked() ), this, SLOT( onApply() ) );

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( onSelectionDone() ) );

  initName( tr( "LCS_NAME" ) );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  for ( DataMap::iterator anIter = myData.begin(); anIter != myData.end(); ++anIter ) {
    initSpinBox( anIter.value(), COORD_MIN, COORD_MAX, step, "length_precision" );
    connect( anIter.value(), SIGNAL( valueChanged( double ) ),
             this, SLOT( onValueChanged( double ) ) );
  }
  
  myBlockPreview = true;
  for ( DataMap::iterator anIter = myData.begin(); anIter != myData.end(); ++anIter )
    anIter.value()->setValue( 0 );
  myData[ DX1 ]->setValue( 1 );
  myData[ DY2 ]->setValue( 1 );
  myBlockPreview = false;

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_MarkerDlg::ConstructorsClicked( int constructorId )
{
  if ( myConstructorId == constructorId && myConstructorId == 0 ) {
    globalSelection(); // close local contexts, if any
    localSelection( TopAbs_VERTEX );
    activate( GEOM_MARKER );
    return;
  }

  myConstructorId = constructorId;

  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );

  switch ( constructorId ) {
  case 0:
    {
      Group1->hide();
      Group2->hide();
      aMainGrp->show();
      globalSelection(); // close local contexts, if any
      localSelection( TopAbs_VERTEX );
      activate( GEOM_MARKER );
      break;
    }
  case 1:
    {
      aMainGrp->hide();
      Group2->hide();
      Group1->show();
      Group1->PushButton1->setDown( true );
      globalSelection( GEOM_ALLGEOM );
      myEditCurrentArgument = Group1->LineEdit1;
      Group1->LineEdit1->setText( "" );
      myShape.nullify();
      break;
    }
  case 2:
    {
      aMainGrp->hide();
      Group1->hide();
      Group2->show();
      Group2->PushButton1->setDown( true );
      Group2->PushButton2->setDown( false );
      Group2->PushButton3->setDown( false );
      globalSelection(); // close local contexts, if any
      localSelection( TopAbs_VERTEX );
      myEditCurrentArgument = Group2->LineEdit1;
      Group2->LineEdit1->setText( "" );
      Group2->LineEdit2->setText( "" );
      Group2->LineEdit3->setText( "" );
      Group2->LineEdit1->setEnabled( true );
      Group2->LineEdit2->setEnabled( false );
      Group2->LineEdit3->setEnabled( false );
      myPoint.nullify();
      myVectorX.nullify();
      myVectorY.nullify();
      break;
    }
  }

  displayPreview();

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );
  
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( onSelectionDone() ) );
  onSelectionDone();
}

//=================================================================================
// function : onOk()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onOk()
{
  setIsApplyAndClose( true );
  if ( onApply() )
    ClickOnCancel();
}

//=================================================================================
// function : onApply()
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::onApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );

  return true;
}

//=================================================================================
// function : onSelectionDone0()
// purpose  : Reaction on selection when first method of creation is current one
//=================================================================================
void BasicGUI_MarkerDlg::onSelectionDone0()
{
  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_FACE );
  TopoDS_Shape aShape;
  if ( aSelectedObject && aSelectedObject->GetType() == GEOM_MARKER && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    // Existing LCS selected
    TopoDS_Face aFace = TopoDS::Face( aShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    
    if ( !aPlane.IsNull() ) {
      gp_Ax3 anAx3 = aPlane->Pln().Position();
      gp_Pnt aLoc = anAx3.Location();
      gp_Dir aXDir = anAx3.XDirection();
      gp_Dir aYDir = anAx3.YDirection();
      
      myData[ X ]->setValue( aLoc.X() );
      myData[ Y ]->setValue( aLoc.Y() );
      myData[ Z ]->setValue( aLoc.Z() );
      
      myData[ DX1 ]->setValue( aXDir.X() );
      myData[ DY1 ]->setValue( aXDir.Y() );
      myData[ DZ1 ]->setValue( aXDir.Z() );
      
      myData[ DX2 ]->setValue( aYDir.X() );
      myData[ DY2 ]->setValue( aYDir.Y() );
      myData[ DZ2 ]->setValue( aYDir.Z() );
    }
    displayPreview();
    return;
  }
  aSelectedObject = getSelected( TopAbs_VERTEX );
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
    myData[ X ]->setValue( aPnt.X() );
    myData[ Y ]->setValue( aPnt.Y() );
    myData[ Z ]->setValue( aPnt.Z() );
    displayPreview();
    return;
  }
}


//=================================================================================
// function : onSelectionDone()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_MarkerDlg::onSelectionDone()
{
  if ( getConstructorId() == 0 ) {
    onSelectionDone0();
    return;
  }

  TopAbs_ShapeEnum aNeedType;
  if ( getConstructorId() == 1 ) // by shape position
    aNeedType = TopAbs_SHAPE;
  else if ( getConstructorId() == 2 ) // by point and two vectors
    aNeedType = myEditCurrentArgument == Group2->LineEdit1 ? TopAbs_VERTEX : TopAbs_EDGE;
  
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    if ( getConstructorId() == 1 ) { // by shape position
      myShape = aSelectedObject;
    }
    else if ( getConstructorId() == 2 ) { // by point and two vectors
      if ( myEditCurrentArgument == Group2->LineEdit1 ) {
        myPoint = aSelectedObject;
        if (myPoint && !myVectorX)
          Group2->PushButton2->click();
      }
      else if (myEditCurrentArgument == Group2->LineEdit2) {
        myVectorX = aSelectedObject;
        if (myVectorX && !myVectorY)
          Group2->PushButton3->click();
      }
      else if ( myEditCurrentArgument == Group2->LineEdit3 ) {
        myVectorY = aSelectedObject;
        if (myVectorY && !myPoint)
          Group2->PushButton1->click();
      }
    }
  }
  else {
    if ( getConstructorId() == 1 ) {
      myShape.nullify();
    }
    else if ( getConstructorId() == 2 ) {
      if ( myEditCurrentArgument == Group2->LineEdit1 )
        myPoint.nullify();
      else if ( myEditCurrentArgument == Group2->LineEdit2 )
        myVectorX.nullify();
      else if ( myEditCurrentArgument == Group2->LineEdit3 )
        myVectorY.nullify();
    }
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection(); // close local contexts, if any

  if ( send == Group1->PushButton1 ) {
    myEditCurrentArgument = Group1->LineEdit1;
    globalSelection( GEOM_ALLGEOM );
  }
  else if ( send == Group2->PushButton1 ) {
    myEditCurrentArgument = Group2->LineEdit1;
    localSelection( TopAbs_VERTEX );
    Group2->PushButton2->setDown( false );
    Group2->PushButton3->setDown( false );
    Group2->LineEdit1->setEnabled( true );
    Group2->LineEdit2->setEnabled( false );
    Group2->LineEdit3->setEnabled( false );
  }
  else if ( send == Group2->PushButton2 ) {
    myEditCurrentArgument = Group2->LineEdit2;
    localSelection( TopAbs_EDGE );
    Group2->PushButton1->setDown( false );
    Group2->PushButton3->setDown( false );
    Group2->LineEdit1->setEnabled( false );
    Group2->LineEdit2->setEnabled( true );
    Group2->LineEdit3->setEnabled( false );
  }
  else if ( send == Group2->PushButton3 ) {
    myEditCurrentArgument = Group2->LineEdit3;
    localSelection( TopAbs_EDGE );
    Group2->PushButton1->setDown( false );
    Group2->PushButton2->setDown( false );
    Group2->LineEdit1->setEnabled( false );
    Group2->LineEdit2->setEnabled( false );
    Group2->LineEdit3->setEnabled( true );
  }
  
  myEditCurrentArgument->setFocus();
  send->setDown(true);
  onSelectionDone();
}

//=================================================================================
// function : onActivate()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onActivate()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( onSelectionDone() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : onDeactivate()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_MarkerDlg::onDeactivate()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    onActivate();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_MarkerDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::isValid( QString& msg )
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0: {
    gp_Vec v1( myData[ DX1 ]->value(), myData[ DY1 ]->value(), myData[ DZ1 ]->value() ),
      v2( myData[ DX2 ]->value(), myData[ DY2 ]->value(), myData[ DZ2 ]->value() );
      
    // we will got exception if the magnitude of any of the 2 vectors <= gp::Resolution()
    // Vectors shouldn't be checked for being orthogonal here!
    if ( v1.Magnitude() > gp::Resolution() && v2.Magnitude() > gp::Resolution() ) {
      ok = !v1.IsParallel( v2, Precision::Angular() );
      if ( !ok )
        msg += tr( "VEC_PARALLEL" );
    }
    for ( DataMap::iterator anIter = myData.begin(); anIter != myData.end() && ok; ++anIter )
      ok = anIter.value()->isValid( msg, !IsPreview()) && ok;
    break;
  }
  case 1:
    ok = myShape;
    break;
  case 2:
    ok = myPoint && myVectorX && myVectorY;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_nil();
  QStringList aParameters;
  const int id = getConstructorId();
  if (id == 0) {
    anObj = anOper->MakeMarker( myData[ X   ]->value(), myData[ Y   ]->value(), 
                                myData[ Z   ]->value(), myData[ DX1 ]->value(),
                                myData[ DY1 ]->value(), myData[ DZ1 ]->value(),
                                myData[ DX2 ]->value(), myData[ DY2 ]->value(),
                                myData[ DZ2 ]->value() );
    aParameters<<myData[X]->text();
    aParameters<<myData[Y]->text();
    aParameters<<myData[Z]->text();
    aParameters<<myData[ DX1 ]->text(); 
    aParameters<<myData[ DY1 ]->text(); 
    aParameters<<myData[ DZ1 ]->text();
    aParameters<<myData[ DX2 ]->text();
    aParameters<<myData[ DY2 ]->text();
    aParameters<<myData[ DZ2 ]->text();
  } else if (id == 1) {
    anObj = anOper->MakeMarkerFromShape( myShape.get() ); 
  } else if (id == 2) {
    anObj = anOper->MakeMarkerPntTwoVec( myPoint.get(), myVectorX.get(), myVectorY.get() ); 
  }
  
  if ( !anObj->_is_nil() ) {
    if ( !IsPreview() && id == 0)
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back( anObj._retn() );
  }

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_MarkerDlg::addSubshapesToStudy()
{
  switch ( getConstructorId() ) {
  case 0 :
    break;
  case 1 :
    GEOMBase::PublishSubObject( myShape.get() );
    break;
  case 2:
    GEOMBase::PublishSubObject( myPoint.get() );
    GEOMBase::PublishSubObject( myVectorX.get() );
    GEOMBase::PublishSubObject( myVectorY.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_MarkerDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myShape << myPoint << myVectorX << myVectorY;
  return res;
}

//=================================================================================
// function : onValueChanged
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onValueChanged( double )
{
  displayPreview();
}

//=================================================================================
// function : displayPreview
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::displayPreview ( const bool   activate,
                                          const bool   update,
                                          const bool   toRemoveFromEngine,
                                          const double lineWidth )
{
  if ( !myBlockPreview ) {
    GEOMBase_Skeleton::displayPreview( true, activate, update, toRemoveFromEngine, lineWidth );
    if ( myConstructorId == 0 )
      GEOMBase_Skeleton::activate( GEOM_MARKER );
  }
}
