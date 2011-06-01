// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : BasicGUI_CurveDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BasicGUI_CurveDlg.h"
#include "BasicGUI_ParamCurveWidget.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>

#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_ListIO.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_CurveDlg()
// purpose  : Constructs a BasicGUI_CurveDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_CurveDlg::BasicGUI_CurveDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POLYLINE" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SPLINE" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BEZIER" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CURVE_TITLE" ) );

  /***************************************************************/
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image3 );
  mainFrame()->RadioButton3->setIcon( image2 );

  QGroupBox* creationModeCroup = new QGroupBox(this);
  QButtonGroup* bg = new QButtonGroup(this);

  creationModeCroup->setTitle( tr( "GEOM_CURVE_CRMODE" ) );  
  QHBoxLayout * creationModeLayout = new QHBoxLayout(creationModeCroup);
  myBySelectionBtn = new QRadioButton(  tr( "GEOM_CURVE_SELECTION" ) ,creationModeCroup );
  myAnaliticalBtn = new QRadioButton(  tr( "GEOM_CURVE_ANALITICAL" ) ,creationModeCroup );

  bg->addButton(myBySelectionBtn);
  bg->addButton(myAnaliticalBtn);
  
  creationModeLayout->addWidget(myBySelectionBtn);
  creationModeLayout->addWidget(myAnaliticalBtn);

  GroupPoints = new DlgRef_1Sel3Check( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_NODES" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_POINTS" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton1->setDown( true );

  GroupPoints->LineEdit1->setReadOnly( true );

  GroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
  GroupPoints->CheckButton1->setChecked(false);
  //GroupPoints->CheckButton1->hide();

  GroupPoints->CheckButton2->setText( tr( "GEOM_IS_REORDER" ) );
  GroupPoints->CheckButton2->setChecked(false);
  GroupPoints->CheckButton2->hide();

  GroupPoints->CheckButton3->hide();

  myParams = new BasicGUI_ParamCurveWidget( centralWidget() );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( creationModeCroup );
  layout->addWidget( GroupPoints );
  layout->addWidget( myParams );
  /***************************************************************/

  setHelpFileName( "create_curve_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_CurveDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_CurveDlg::~BasicGUI_CurveDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myPoints.clear();

  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

  showOnlyPreviewControl();
  myBySelectionBtn->setChecked(true);

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr ? resMgr->doubleValue( "Geometry", "SettingsGeomStep", 10. ) : 10.;

  double aMax( 100. ), aMin( 0.0 );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( myParams->myPMin, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( myParams->myPMax, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( myParams->myPStep, COORD_MIN, COORD_MAX, step, "length_precision" );
  myParams->myPMin->setValue( aMin );
  myParams->myPMax->setValue( aMax );
  myParams->myPStep->setValue( step );
  myParams->myXExpr->setText("t");
  myParams->myYExpr->setText("t");
  myParams->myZExpr->setText("t");
  
  myParams->hide();

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog( ) ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->CheckButton1, SIGNAL( toggled(bool) ),  this, SLOT( CheckButtonToggled() ) );
  connect( GroupPoints->CheckButton2, SIGNAL( toggled(bool) ),  this, SLOT( CheckButtonToggled() ) );

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( myBySelectionBtn, SIGNAL( clicked() ), this, SLOT( CreationModeChanged() ) );
  connect( myAnaliticalBtn, SIGNAL( clicked() ), this, SLOT( CreationModeChanged() ) );

  connect(myParams->myPMin, SIGNAL(valueChanged(double)),    this, SLOT(ValueChangedInSpinBox(double)));
  connect(myParams->myPMax, SIGNAL(valueChanged(double)),    this, SLOT(ValueChangedInSpinBox(double)));
  connect(myParams->myPStep, SIGNAL(valueChanged(double)),    this, SLOT(ValueChangedInSpinBox(double)));

  connect(myParams->myXExpr, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
  connect(myParams->myYExpr, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));
  connect(myParams->myZExpr, SIGNAL(editingFinished()), this, SLOT(OnEditingFinished()));

  initName( tr( "GEOM_CURVE" ) );
  resize(100,100);
  ConstructorsClicked( 0 );  
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ConstructorsClicked( int id )
{
  QString aTitle = tr( id == 0 ? "GEOM_POLYLINE" : id == 1 ? "GEOM_BEZIER" : "GEOM_INTERPOL" );
  mainFrame()->GroupConstructors->setTitle( aTitle );

  if (id == 0) { // polyline (wire)
    //GroupPoints->CheckButton1->hide();
    GroupPoints->CheckButton1->setText( tr( "GEOM_BUILD_CLOSED_WIRE" ) );
    GroupPoints->CheckButton2->hide();
  }
  else if (id == 1) { // bezier
    //GroupPoints->CheckButton1->hide();
    GroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
    GroupPoints->CheckButton2->hide();
  }
  else { // b-spline
    //GroupPoints->CheckButton1->show();
    GroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
    GroupPoints->CheckButton2->show();
  }

  myPoints.clear();

  myEditCurrentArgument->setText( "" );
  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );
  SelectionIntoArgument();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::SetEditCurrentArgument()
{
  if ( sender() == GroupPoints->PushButton1 )
    myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : CheckButtonToggled()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::CheckButtonToggled()
{
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  return true;
}

static void synchronize( QList<GEOM::GeomObjPtr>& left, QList<GEOM::GeomObjPtr>& right )
{
  // 1. remove items from the "left" list that are not in the "right" list
  QMutableListIterator<GEOM::GeomObjPtr> it1( left );
  while ( it1.hasNext() ) {
    GEOM::GeomObjPtr o1 = it1.next();
    bool found = false;
    QMutableListIterator<GEOM::GeomObjPtr> it2( right );
    while ( it2.hasNext() && !found )
      found = o1 == it2.next();
    if ( !found )
      it1.remove();
  }
  // 2. add items from the "right" list that are not in the "left" list (to keep selection order)
  it1 = right;
  while ( it1.hasNext() ) {
    GEOM::GeomObjPtr o1 = it1.next();
    bool found = false;
    QMutableListIterator<GEOM::GeomObjPtr> it2( left );
    while ( it2.hasNext() && !found )
      found = o1 == it2.next();
    if ( !found )
      left << o1;
  }
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_CurveDlg::SelectionIntoArgument()
{
  QList<GEOM::GeomObjPtr> points = getSelected( TopAbs_VERTEX, -1 );
  synchronize( myPoints, points );
  if ( !myPoints.isEmpty()  )
    GroupPoints->LineEdit1->setText( QString::number( myPoints.count() ) + "_" + tr( "GEOM_POINT" ) + tr( "_S_" ) );
  else
    GroupPoints->LineEdit1->setText( "" );
  processPreview();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_CurveDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_CurveDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::isValid( QString& msg )
{
  if( myBySelectionBtn->isChecked() )
    return myPoints.count() > 1;
  else {
    bool ok = myParams->myPMin->isValid( msg, !IsPreview() ) &&
              myParams->myPMax->isValid( msg, !IsPreview() ) &&
              myParams->myPStep->isValid( msg, !IsPreview() );
    ok &= !myParams->myXExpr->text().isEmpty();
    ok &= !myParams->myYExpr->text().isEmpty();
    ok &= !myParams->myZExpr->text().isEmpty();
    return ok;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() );

  GEOM::ListOfGO_var points = new GEOM::ListOfGO();
  points->length( myPoints.count() );
  for ( int i = 0; i < myPoints.count(); i++ )
    points[i] = myPoints[i].copy();

  switch ( getConstructorId() ) {
  case 0 :
    if( myBySelectionBtn->isChecked() )
      anObj = anOper->MakePolyline( points.in(), GroupPoints->CheckButton1->isChecked() );
    else
      anObj = anOper->MakeCurveParametric(qPrintable(myParams->myXExpr->text()),
					  qPrintable(myParams->myYExpr->text()),
					  qPrintable(myParams->myZExpr->text()),
					  myParams->myPMin->value(),
					  myParams->myPMax->value(),
					  myParams->myPStep->value(),
					  GEOM::Polyline);
    res = true;
    break;
  case 1 :
    if( myBySelectionBtn->isChecked() )
      anObj = anOper->MakeSplineBezier( points.in(), GroupPoints->CheckButton1->isChecked() );
    else
      anObj = anOper->MakeCurveParametric(qPrintable(myParams->myXExpr->text()),
					  qPrintable(myParams->myYExpr->text()),
					  qPrintable(myParams->myZExpr->text()),
					  myParams->myPMin->value(),
					  myParams->myPMax->value(),
					  myParams->myPStep->value(),
					  GEOM::Bezier);

    res = true;
    break;
  case 2 :
    if( myBySelectionBtn->isChecked() )
      anObj = anOper->MakeSplineInterpolation( points.in(), GroupPoints->CheckButton1->isChecked(),
					       GroupPoints->CheckButton2->isChecked() );
    else
      anObj = anOper->MakeCurveParametric(qPrintable(myParams->myXExpr->text()),
					  qPrintable(myParams->myYExpr->text()),
					  qPrintable(myParams->myZExpr->text()),
					  myParams->myPMin->value(),
					  myParams->myPMax->value(),
					  myParams->myPStep->value(),
					  GEOM::Interpolation);
    res = true;
    break;
  }

  if ( !anObj->_is_nil() ) {
    if(myAnaliticalBtn->isChecked() && !IsPreview()) {
      QStringList aParameters;
      aParameters<<myParams->myPMin->text();
      aParameters<<myParams->myPMax->text();
      aParameters<<myParams->myPStep->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    objects.push_back( anObj._retn() );
  }
  
  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_CurveDlg::addSubshapesToStudy()
{
  for ( int i = 0; i < myPoints.count(); i++ )
    GEOMBase::PublishSubObject( myPoints[i].get() );
}

//=================================================================================
// function : CreationModeChanged
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::CreationModeChanged() {
  const QObject* s = sender();
  GroupPoints->setVisible(myBySelectionBtn == s);
  myParams->setVisible(myBySelectionBtn != s);
  
  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ValueChangedInSpinBox(double/*theValue*/)
{
  processPreview();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::OnEditingFinished() {
  processPreview();
}
