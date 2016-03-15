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
#include "utilities.h"

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
  
  GroupPntVecR->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) + " " + tr( "GEOM_CENTER_DEFAULT" )  );
  GroupPntVecR->TextLabel2->setText( tr( "GEOM_VECTOR" ) + " " + tr( "GEOM_AXIS_DEFAULT" ) );
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

  myPoint.nullify();
  myDir.nullify();
  myPoint1.nullify();
  myPoint2.nullify();
  myPoint3.nullify();
  myPoint4.nullify();
  myPoint5.nullify();
  myPoint6.nullify();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPntVecR->SpinBox_DX, 0.000001, COORD_MAX, aStep, "length_precision" );
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

  myPoint.nullify();
  myDir.nullify();
  myPoint1.nullify();
  myPoint2.nullify();
  myPoint3.nullify();
  myPoint4.nullify();
  myPoint5.nullify();
  myPoint6.nullify();

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
  localSelection( TopAbs_VERTEX );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
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
    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 )     myPoint.nullify();
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 )     myDir.nullify();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )       myPoint1.nullify();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )       myPoint2.nullify();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )       myPoint3.nullify();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit1 ) myPoint4.nullify();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit2 ) myPoint5.nullify();
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit3 ) myPoint6.nullify();
    return;
  }

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) ?
    TopAbs_EDGE : TopAbs_VERTEX;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    
    myEditCurrentArgument->setText( aName );

    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 ) {
      myPoint  = aSelectedObject;
      if ( myPoint && !myDir )
        GroupPntVecR->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) {
      myDir    = aSelectedObject;
      if ( myDir && !myPoint )
        GroupPntVecR->PushButton1->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 ) {
      myPoint1 = aSelectedObject;
      if ( myPoint1 && !myPoint2 )
        Group3Pnts->PushButton2->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 ) {
      myPoint2 = aSelectedObject;
      if ( myPoint2 && !myPoint3 )
        Group3Pnts->PushButton3->click();
    }
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 ) {
      myPoint3 = aSelectedObject;
      if ( myPoint3 && !myPoint1 )
        Group3Pnts->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit1 ) {
      myPoint4 = aSelectedObject;
      if ( myPoint4 && !myPoint5 )
        GroupCenter2Pnts->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit2 ) {
      myPoint5 = aSelectedObject;
      if ( myPoint5 && !myPoint6 )
        GroupCenter2Pnts->PushButton3->click();
    }
    else if ( myEditCurrentArgument == GroupCenter2Pnts->LineEdit3 ) {
      myPoint6 = aSelectedObject;
      if ( myPoint6 && !myPoint4 )
        GroupCenter2Pnts->PushButton1->click();
    }
  }

  displayPreview(true);
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

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) ?
    TopAbs_EDGE : TopAbs_VERTEX;
  globalSelection(); // close local contexts, if any
  localSelection( aNeedType );

  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  send->setDown(true);
  displayPreview(true);
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
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ValueChangedInSpinBox()
{
  displayPreview(true);
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
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::isValid( QString& msg )
{
  bool ok = false;
  switch( getConstructorId() ) {
  case 0:
    // nil point means origin of global CS
    // nil vector means Z axis
    ok = GroupPntVecR->SpinBox_DX->isValid(msg, !IsPreview()) && getRadius() > 0;
    break;
  case 1:
    ok = myPoint1 && myPoint2 && myPoint3 && myPoint1 != myPoint2 && 
         myPoint1 != myPoint3 && myPoint2 != myPoint3;
    break;
  case 2:
    ok = myPoint4 && myPoint5 && myPoint6 && myPoint4 != myPoint5 && 
         myPoint5 != myPoint6 && myPoint4 != myPoint6;
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
bool BasicGUI_CircleDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() );

  switch ( getConstructorId() ) {
  case 0 :
    {
      QStringList aParameters;
      aParameters << GroupPntVecR->SpinBox_DX->text();
      anObj = anOper->MakeCirclePntVecR( myPoint.get(), myDir.get(), getRadius() );
      if ( !anObj->_is_nil() && !IsPreview() )
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      res = true;
      break;
    }
  case 1 :
    anObj = anOper->MakeCircleThreePnt( myPoint1.get(), myPoint2.get(), myPoint3.get() );
    res = true;
    break;
  case 2:
    anObj = anOper->MakeCircleCenter2Pnt( myPoint4.get(), myPoint5.get(), myPoint6.get() );
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
  switch ( getConstructorId() ) {
  case 0:
    GEOMBase::PublishSubObject( myPoint.get() );
    GEOMBase::PublishSubObject( myDir.get() );
    break;
  case 1:
    GEOMBase::PublishSubObject( myPoint1.get() );
    GEOMBase::PublishSubObject( myPoint2.get() );
    GEOMBase::PublishSubObject( myPoint3.get() );
    break;
  case 2:
    GEOMBase::PublishSubObject( myPoint4.get() );
    GEOMBase::PublishSubObject( myPoint5.get() );
    GEOMBase::PublishSubObject( myPoint6.get() );
    break;
 default:
   break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_CircleDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint << myDir << myPoint1 << myPoint2 << myPoint3
      << myPoint4 << myPoint5 << myPoint6;
  return res;
}
