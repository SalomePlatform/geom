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
// File   : BasicGUI_VectorDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BasicGUI_VectorDlg.h"

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
// class    : BasicGUI_VectorDlg()
// purpose  : Constructs a BasicGUI_VectorDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_VectorDlg::BasicGUI_VectorDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                        bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_VECTOR_2P" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_VECTOR_DXYZ" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_VECTOR_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_VECTOR"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );
  GroupPoints->PushButton1->setIcon( image2 );
  GroupPoints->PushButton2->setIcon( image2 );
  GroupPoints->PushButton1->setDown( true );

  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );

  GroupDimensions = new DlgRef_3Spin1Check( centralWidget() );
  GroupDimensions->GroupBox1->setTitle( tr( "GEOM_COORDINATES" ) );
  GroupDimensions->TextLabel1->setText( tr( "GEOM_DX" ) );
  GroupDimensions->TextLabel2->setText( tr( "GEOM_DY" ) );
  GroupDimensions->TextLabel3->setText( tr( "GEOM_DZ" ) );
  GroupDimensions->CheckButton1->setText( tr( "GEOM_REVERSE_VECTOR" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupDimensions );
  /***************************************************************/

  setHelpFileName( "create_vector_page.html" );

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BasicGUI_VectorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_VectorDlg::~BasicGUI_VectorDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::Init()
{
    /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myPoint1.nullify();
  myPoint2.nullify();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );

  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupDimensions->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupDimensions->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( GroupDimensions->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );

  double dx( 0. ), dy( 0. ), dz( 200. );
  GroupDimensions->SpinBox_DX->setValue( dx );
  GroupDimensions->SpinBox_DY->setValue( dy );
  GroupDimensions->SpinBox_DZ->setValue( dz );

  GroupDimensions->CheckButton1->setChecked( false );

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupDimensions->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupDimensions->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( GroupDimensions->CheckButton1, SIGNAL( stateChanged( int ) ), this, SLOT( ReverseVector( int ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr("GEOM_VECTOR") );

  setConstructorId( 1 ); // simplest constructor
  ConstructorsClicked( 1 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_VectorDlg::SetDoubleSpinBoxStep( double step )
{
  GroupDimensions->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DY->setSingleStep(step);
  GroupDimensions->SpinBox_DZ->setSingleStep(step);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_VectorDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  myPoint1.nullify();
  myPoint2.nullify();

  switch ( constructorId ) {
  case 0:
    {
      GroupDimensions->hide();
      GroupPoints->show();

      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      GroupPoints->PushButton1->setDown( true );
      GroupPoints->PushButton2->setDown( false );
      GroupPoints->LineEdit1->setEnabled( true );
      GroupPoints->LineEdit2->setEnabled( false );

      globalSelection(); // close local contexts, if any
      localSelection( TopAbs_VERTEX );
      connect( myGeomGUI->getApp()->selectionMgr(),
               SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
      break;
    }
  case 1:
    {
      GroupPoints->hide();
      GroupDimensions->show();
      globalSelection(); // close local contexts, if any

      double dx( 0. ), dy( 0. ), dz( 0. );
      GroupDimensions->SpinBox_DX->setValue( dx );
      GroupDimensions->SpinBox_DY->setValue( dy );
      GroupDimensions->SpinBox_DZ->setValue( dz );

      GroupDimensions->CheckButton1->setChecked( false );
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );
  SelectionIntoArgument();

  displayPreview(true);
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_VectorDlg::ClickOnApply()
{
  buttonApply()->setFocus();

  if ( !onAccept() )
    return false;

  initName();
  if ( getConstructorId() != 1 )
    ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_VectorDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if (myEditCurrentArgument == GroupPoints->LineEdit1)
      myPoint1.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2)
      myPoint2.nullify();
    return;
  }

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_VERTEX );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText(aName);
    if (myEditCurrentArgument == GroupPoints->LineEdit1) {
      myPoint1 = aSelectedObject;
      if (myPoint1 && !myPoint2)
        GroupPoints->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
      myPoint2 = aSelectedObject;
      if (myPoint2 && !myPoint1)
        GroupPoints->PushButton1->click();
    }
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if      ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled(true);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(true);
  }
  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  globalSelection(); // close local selection to clear it
  localSelection( TopAbs_VERTEX );
  send->setDown(true);
  displayPreview(true);
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ActivateThisDialog()
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
void BasicGUI_VectorDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_VectorDlg::ValueChangedInSpinBox( double newValue )
{
  displayPreview(true);
}

//=================================================================================
// function : ReverseVector()
// purpose  : 'state' not used here
//=================================================================================
void BasicGUI_VectorDlg::ReverseVector( int state )
{
  double dx = -GroupDimensions->SpinBox_DX->value();
  double dy = -GroupDimensions->SpinBox_DY->value();
  double dz = -GroupDimensions->SpinBox_DZ->value();

  GroupDimensions->SpinBox_DX->setValue( dx );
  GroupDimensions->SpinBox_DY->setValue( dy );
  GroupDimensions->SpinBox_DZ->setValue( dz );

  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_VectorDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_VectorDlg::isValid( QString& msg )
{
  bool ok = false;
  if(getConstructorId() == 0) 
    ok = myPoint1 && myPoint2;
  else if(getConstructorId() == 1)
  {
    ok = GroupDimensions->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DY->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DZ->isValid( msg, !IsPreview() );
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_VectorDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );

  switch ( getConstructorId() ) {
  case 0 :
    {
      anObj = anOper->MakeVectorTwoPnt( myPoint1.get(), myPoint2.get() );
      res = true;
      break;
    }
  case 1 :
    {
      double dx = GroupDimensions->SpinBox_DX->value();
      double dy = GroupDimensions->SpinBox_DY->value();
      double dz = GroupDimensions->SpinBox_DZ->value();
      
      QStringList aParameters;
      aParameters << GroupDimensions->SpinBox_DX->text();
      aParameters << GroupDimensions->SpinBox_DY->text();
      aParameters << GroupDimensions->SpinBox_DZ->text();
      anObj = anOper->MakeVectorDXDYDZ( dx, dy, dz );

      if ( !anObj->_is_nil() && !IsPreview() )
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      
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
void BasicGUI_VectorDlg::addSubshapesToStudy()
{
  if ( getConstructorId() == 0 ) {
    GEOMBase::PublishSubObject( myPoint1.get() );
    GEOMBase::PublishSubObject( myPoint2.get() );
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_VectorDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint1 << myPoint2;
  return res;
}
