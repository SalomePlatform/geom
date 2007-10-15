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
// File   : BasicGUI_PointDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#include "BasicGUI_PointDlg.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOM_DlgRef.h>

#include <GEOMImpl_Types.hxx>

#include <QApplication>
#include <QLabel>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//=================================================================================
// class    : BasicGUI_PointDlg()
// purpose  : Constructs a BasicGUI_PointDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PointDlg::BasicGUI_PointDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
				      bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, "BasicGUI_PointDlg", modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT") ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_EDGE" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_REF" ) ) );

  setWindowTitle( tr( "GEOM_POINT_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_POINTS" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image3 );
  mainFrame()->RadioButton3->setIcon( image1 );

  GroupXYZ = new DlgRef_3Spin( centralWidget() );
  GroupXYZ->GroupBox1->setTitle( tr( "GEOM_COORDINATES" ) );
  GroupXYZ->TextLabel1->setText( tr( "GEOM_X" ) );
  GroupXYZ->TextLabel2->setText( tr( "GEOM_Y" ) );
  GroupXYZ->TextLabel3->setText( tr( "GEOM_Z" ) );

  GroupOnCurve = new DlgRef_1Sel1Spin( centralWidget() );
  GroupOnCurve->GroupBox1->setTitle( tr( "GEOM_PARAM_POINT" ) );
  GroupOnCurve->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  GroupOnCurve->TextLabel2->setText( tr( "GEOM_PARAMETER" ) );
  GroupOnCurve->PushButton1->setIcon( image2 );

  GroupRefPoint = new DlgRef_1Sel3Spin( centralWidget() );
  GroupRefPoint->GroupBox1->setTitle( tr( "GEOM_REF_POINT" ) );
  GroupRefPoint->TextLabel1->setText( tr( "GEOM_POINT" ) );
  GroupRefPoint->PushButton1->setIcon( image2 );
  GroupRefPoint->TextLabel2->setText( tr( "GEOM_DX" ) );
  GroupRefPoint->TextLabel3->setText( tr( "GEOM_DY" ) );
  GroupRefPoint->TextLabel4->setText( tr( "GEOM_DZ" ) );

  myCoordGrp = new QGroupBox( tr( "GEOM_COORDINATES" ), centralWidget() );
  QGridLayout* myCoordGrpLayout = new QGridLayout( myCoordGrp );
  myCoordGrpLayout->addWidget( new QLabel( tr( "GEOM_X" ), myCoordGrp ), 0, 0 );
  myX = new QLineEdit( myCoordGrp );
  myCoordGrpLayout->addWidget( myX, 0, 1 );
  myCoordGrpLayout->addWidget( new QLabel( tr( "GEOM_Y" ), myCoordGrp ), 1, 0 );
  myY = new QLineEdit( myCoordGrp );
  myCoordGrpLayout->addWidget( myY, 1, 1 );
  myCoordGrpLayout->addWidget( new QLabel( tr( "GEOM_Z" ), myCoordGrp ), 2, 0 );
  myZ = new QLineEdit( myCoordGrp );
  myCoordGrpLayout->addWidget( myZ, 2, 1 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupXYZ );
  layout->addWidget( GroupOnCurve );
  layout->addWidget( GroupRefPoint );
  layout->addWidget( myCoordGrp );

  myX->setReadOnly( true );
  myY->setReadOnly( true );
  myZ->setReadOnly( true );

  myX->setEnabled( false );
  myY->setEnabled( false );
  myZ->setEnabled( false );

  QPalette aPal = myX->palette();
  aPal.setColor( QPalette::Disabled, QPalette::Text, QColor( 0, 0, 0 ) ) ;
  myX->setPalette( aPal );
  myY->setPalette( aPal );
  myZ->setPalette( aPal );

  setHelpFileName( "point.htm" );
 
  Init();
}


//=================================================================================
// function : ~BasicGUI_PointDlg()
// purpose  : Destructor
//=================================================================================
BasicGUI_PointDlg::~BasicGUI_PointDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::Init()
{
  GroupOnCurve->LineEdit1->setReadOnly( true );
  GroupRefPoint->LineEdit1->setReadOnly( true );

  myEdge = GEOM::GEOM_Object::_nil();
  myRefPoint = GEOM::GEOM_Object::_nil();

  myEditCurrentArgument = 0;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupXYZ->SpinBox_DX, COORD_MIN, COORD_MAX, step, 3 );
  initSpinBox( GroupXYZ->SpinBox_DY, COORD_MIN, COORD_MAX, step, 3 );
  initSpinBox( GroupXYZ->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 3 );
  GroupXYZ->SpinBox_DX->setValue( 0.0 );
  GroupXYZ->SpinBox_DY->setValue( 0.0 );
  GroupXYZ->SpinBox_DZ->setValue( 0.0 );

  initSpinBox( GroupRefPoint->SpinBox_DX, COORD_MIN, COORD_MAX, step, 3 );
  initSpinBox( GroupRefPoint->SpinBox_DY, COORD_MIN, COORD_MAX, step, 3 );
  initSpinBox( GroupRefPoint->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 3 );
  GroupRefPoint->SpinBox_DX->setValue( 0.0 );
  GroupRefPoint->SpinBox_DY->setValue( 0.0 );
  GroupRefPoint->SpinBox_DZ->setValue( 0.0 );

  step = 0.1;

  initSpinBox( GroupOnCurve->SpinBox_DX, 0., 1., step, 3 );
  GroupOnCurve->SpinBox_DX->setValue( 0.5 );

  /* signals and slots connections */
  connect( myGeomGUI,      SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI,      SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupOnCurve->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupOnCurve->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupOnCurve->SpinBox_DX,  SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DX,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DY,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DZ,      SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DZ, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  // VSR: TODO ->>
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupOnCurve->SpinBox_DX,  SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupXYZ->SpinBox_DX,      SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupXYZ->SpinBox_DY,      SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupXYZ->SpinBox_DZ,      SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupRefPoint->SpinBox_DX, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupRefPoint->SpinBox_DY, SLOT( SetStep( double ) ) );
  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), GroupRefPoint->SpinBox_DZ, SLOT( SetStep( double ) ) );
  // <<-

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );  
  
  initName( tr( "GEOM_VERTEX" ) );
  
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PointDlg::ConstructorsClicked(int constructorId)
{  
  switch ( constructorId ) {
  case 0:
    {
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      
      GroupRefPoint->hide();
      GroupOnCurve->hide();

      myCoordGrp->hide();

      GroupXYZ->show();
      break;
    }
  case 1:
    {
      myEditCurrentArgument = GroupRefPoint->LineEdit1;
      myEditCurrentArgument->setText( "" );
      myRefPoint = GEOM::GEOM_Object::_nil();
      
      globalSelection( GEOM_POINT );
      
      GroupXYZ->hide();
      GroupOnCurve->hide();
      
      GroupRefPoint->show();
      
      myCoordGrp->show();
      break;
    }
  case 2:
    {
      myEditCurrentArgument = GroupOnCurve->LineEdit1;
      myEditCurrentArgument->setText( "" );
      myEdge = GEOM::GEOM_Object::_nil();

      globalSelection( GEOM_EDGE );

      GroupXYZ->hide();
      GroupRefPoint->hide();

      GroupOnCurve->show();
      
      myCoordGrp->show();
      break;
    }
  }

  myX->setText( "" );
  myY->setText( "" );
  myZ->setText( "" );

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnOk()
{
  if ( onAccept() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed (for constructors not using local context)
//=================================================================================
void BasicGUI_PointDlg::SelectionIntoArgument()
{
  const int id = getConstructorId();

  if ( ( id == 1 || id == 2 ) && myEditCurrentArgument != 0 ) {
    myEditCurrentArgument->setText( "" );
    myX->setText( "" );
    myY->setText( "" );
    myZ->setText( "" );
    myRefPoint = myEdge = GEOM::GEOM_Object::_nil();
  }

  if ( IObjectCount() == 1 ) {
    Standard_Boolean aRes = Standard_False;
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
      if ( id == 0 ) {
        // get CORBA reference to data object
        TopoDS_Shape aShape = myGeomGUI->GetShapeReader().GetShape( myGeomGUI->GetGeomGen(), aSelectedObject );
        if ( aShape.IsNull() )
          return;

        if ( aShape.ShapeType() != TopAbs_VERTEX ) {
          TColStd_IndexedMapOfInteger aMap;
          LightApp_Application* anApp =
            (LightApp_Application*)(SUIT_Session::session()->activeApplication());
          anApp->selectionMgr()->GetIndexes( anIO, aMap );
	  
          if ( aMap.Extent() == 1 ) {
            int anIndex = aMap( 1 );
            TopTools_IndexedMapOfShape aShapes;
            TopExp::MapShapes( aShape, aShapes );
            aShape = aShapes.FindKey( anIndex );
	    
            if ( aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX )
              return;
          }
          else
            return;
        }

        gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
        GroupXYZ->SpinBox_DX->setValue( aPnt.X() );
        GroupXYZ->SpinBox_DY->setValue( aPnt.Y() );
        GroupXYZ->SpinBox_DZ->setValue( aPnt.Z() );
      }
      else if ( id == 1 ) {
        myRefPoint = aSelectedObject;
        GroupRefPoint->LineEdit1->setText( GEOMBase::GetName( aSelectedObject ) );
      }
      else if ( id == 2 ) {
        myEdge = aSelectedObject;
        GroupOnCurve->LineEdit1->setText( GEOMBase::GetName( aSelectedObject ) );
      }
    }
  }
  
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupRefPoint->LineEdit1 || send == GroupOnCurve->LineEdit1 )
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupRefPoint->PushButton1 ) {
    GroupRefPoint->LineEdit1->setFocus();
    myEditCurrentArgument = GroupRefPoint->LineEdit1;
    
    globalSelection( GEOM_POINT );
  }
  else if ( send == GroupOnCurve->PushButton1 ) {
    GroupOnCurve->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnCurve->LineEdit1;
    
    globalSelection( GEOM_EDGE );
  }
}


//=================================================================================
// function : enterEvent()
// purpose  : to reactivate this dialog box when mouse enter onto the window
//=================================================================================
void BasicGUI_PointDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ActivateThisDialog( )
{
  GEOMBase_Skeleton::ActivateThisDialog();
  // myGeomGUI->SetState( 0 );
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PointDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview();
}

//=================================================================================
// funcion  : getParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getParameter() const
{
  return GroupOnCurve->SpinBox_DX->value();
}

//=================================================================================
// function : OnPointSelected
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::OnPointSelected( const gp_Pnt& thePnt )
{
  if ( getConstructorId() == 0 ) {
    GroupXYZ->SpinBox_DX->setValue( thePnt.X() );
    GroupXYZ->SpinBox_DY->setValue( thePnt.Y() );
    GroupXYZ->SpinBox_DZ->setValue( thePnt.Z() );

    displayPreview();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_PointDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 )
    return true;
  else if ( id == 1 )
    return !myRefPoint->_is_nil();
  else if ( id == 2 )
    return !myEdge->_is_nil();
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0 :
    {
      double x = GroupXYZ->SpinBox_DX->value();
      double y = GroupXYZ->SpinBox_DY->value();
      double z = GroupXYZ->SpinBox_DZ->value();
      
      anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePointXYZ( x, y, z );
      res = true;
      break;
    }
  case 1 :
    {
      double dx = GroupRefPoint->SpinBox_DX->value();
      double dy = GroupRefPoint->SpinBox_DY->value();
      double dz = GroupRefPoint->SpinBox_DZ->value();
      
      anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
	MakePointWithReference( myRefPoint, dx, dy, dz );
      res = true;
      break;
    }
  case 2 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnCurve( myEdge, getParameter() );
    res = true;
    break;
  }
  
  if ( getConstructorId() == 1 || getConstructorId() == 2 ) {
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( anObj, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX ) {
      gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
      myX->setText( QString( "%1" ).arg( aPnt.X() ) );
      myY->setText( QString( "%1" ).arg( aPnt.Y() ) );
      myZ->setText( QString( "%1" ).arg( aPnt.Z() ) );
    }
    else {
      myX->setText( "" );
      myY->setText( "" );
      myZ->setText( "" );
    }
  }
  
  if ( !anObj->_is_nil() ) {
    objects.push_back( anObj._retn() );
  }
  
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}
