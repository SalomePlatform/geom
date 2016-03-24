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

  GroupPoints = new DlgRef_3Sel2Spin( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "%1 (%2)" ).arg( tr( "GEOM_CENTER" ), tr( "ORIGIN_DEFAULT" ) ) );
  GroupPoints->TextLabel2->setText( tr( "%1 (%2)" ).arg( tr( "GEOM_VECTOR" ), tr( "Z_AXIS_DEFAULT" ) ) );
  GroupPoints->TextLabel3->setText( tr( "%1 (%2)" ).arg( tr( "GEOM_VECTOR_MAJOR" ), tr( "X_AXIS_DEFAULT" ) ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_RADIUS_MAJOR" ) );
  GroupPoints->TextLabel5->setText( tr( "GEOM_RADIUS_MINOR" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );
  GroupPoints->PushButton3->setIcon( image1 );

  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit3->setReadOnly( true );
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );
  GroupPoints->LineEdit3->setEnabled( false );

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
  GroupPoints->PushButton1->setDown(true);

  myPoint.nullify();
  myDir.nullify();
  myMajor.nullify();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  double aMajorR( 200. ), aMinorR( 100. );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox_DX, 0.000001, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupPoints->SpinBox_DY, 0.000001, COORD_MAX, step, "length_precision" );
  GroupPoints->SpinBox_DX->setValue( aMajorR );
  GroupPoints->SpinBox_DY->setValue( aMinorR );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );
  
  connect( myGeomGUI->getApp()->selectionMgr(), 
          SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_ELLIPSE" ) );

  globalSelection(); // close local contexts, if any
  localSelection( TopAbs_VERTEX );

  resize( minimumSizeHint() );
  SelectionIntoArgument();
  displayPreview(true);
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
  setIsApplyAndClose( true );
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
  myPoint.nullify();
  myDir.nullify();
  myMajor.nullify();
  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  GroupPoints->LineEdit3->setText( "" );
  GroupPoints->PushButton1->setDown(true);
  GroupPoints->PushButton2->setDown(false);
  GroupPoints->PushButton3->setDown(false);
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );
  GroupPoints->LineEdit3->setEnabled( false );
  myEditCurrentArgument = GroupPoints->LineEdit1;

  globalSelection(); // close local contexts, if any
  localSelection( TopAbs_VERTEX );
  displayPreview(true);
  
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
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myDir.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit3) myMajor.nullify();
    return;
  }

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPoints->LineEdit2 || 
                                 myEditCurrentArgument == GroupPoints->LineEdit3 ) ?
    TopAbs_EDGE : TopAbs_VERTEX;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
      myPoint = aSelectedObject;
      if ( myPoint && !myDir )
        GroupPoints->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myDir = aSelectedObject;
      if ( myDir && !myMajor )
        GroupPoints->PushButton3->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit3 ) {
      myMajor = aSelectedObject;
      if ( myMajor && !myPoint )
        GroupPoints->PushButton1->click();
    }
  }

  displayPreview(true);
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
    GroupPoints->PushButton3->setDown(false);
    GroupPoints->LineEdit1->setEnabled( true );
    GroupPoints->LineEdit2->setEnabled( false );
    GroupPoints->LineEdit3->setEnabled( false );
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton3->setDown(false);
    GroupPoints->LineEdit1->setEnabled( false );
    GroupPoints->LineEdit2->setEnabled( true );
    GroupPoints->LineEdit3->setEnabled( false );
  }
  else if ( send == GroupPoints->PushButton3 ) {
    myEditCurrentArgument = GroupPoints->LineEdit3;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled( false );
    GroupPoints->LineEdit2->setEnabled( false );
    GroupPoints->LineEdit3->setEnabled( true );
  }

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPoints->LineEdit2 || 
                                 myEditCurrentArgument == GroupPoints->LineEdit3 ) ? 
    TopAbs_EDGE : TopAbs_VERTEX;
  globalSelection(); // close local contexts, if any
  localSelection( aNeedType );
  
  myEditCurrentArgument->setFocus();
  //SelectionIntoArgument();
  send->setDown(true);
  displayPreview(true);
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
  GroupPoints->LineEdit3->setText( "" );

  myPoint.nullify();
  myDir.nullify();
  //globalSelection( GEOM_POINT );
  globalSelection(); // close local contexts, if any
  localSelection( TopAbs_VERTEX );
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
  displayPreview(true);
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
  // nil point means origin of global CS
  // nil vector means Z axis
  bool ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) &&
            GroupPoints->SpinBox_DY->isValid( msg, !IsPreview() );
  if ( ok ) {
    double aMajorR = GroupPoints->SpinBox_DX->value();
    double aMinorR = GroupPoints->SpinBox_DY->value();
    if ( aMajorR < aMinorR ) {
      msg = tr( "GEOM_ELLIPSE_ERROR_1" );
      ok = false;
    }
  }
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
  
  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = myMajor ? 
    anOper->MakeEllipseVec( myPoint.get(), myDir.get(), aMajorR, aMinorR, myMajor.get() ) :
    anOper->MakeEllipse   ( myPoint.get(), myDir.get(), aMajorR, aMinorR );
  if ( !anObj->_is_nil() ) {
    if ( !IsPreview() )
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
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
  GEOMBase::PublishSubObject( myPoint.get() );
  GEOMBase::PublishSubObject( myDir.get() );
  GEOMBase::PublishSubObject( myMajor.get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_EllipseDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint << myDir << myMajor;
  return res;
}
