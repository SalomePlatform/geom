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

#include <DlgRef.h>

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
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT") ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_EDGE" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_REF" ) ) );
  QPixmap image4( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_LINES") ) );
  QPixmap image5( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_POINT_FACE" ) ) );

  setWindowTitle( tr( "GEOM_POINT_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_POINTS" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image3 );
  mainFrame()->RadioButton3->setIcon( image1 );
  mainFrame()->RadioButton4->show();
  mainFrame()->RadioButton4->setIcon( image4 );
  mainFrame()->RadioButton5->show();
  mainFrame()->RadioButton5->setIcon( image5 );

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

  GroupOnSurface = new DlgRef_1Sel2Spin( centralWidget() );
  GroupOnSurface->GroupBox1->setTitle( tr( "GEOM_PARAM_POINT" ) );
  GroupOnSurface->TextLabel1->setText( tr( "GEOM_FACE" ) );
  GroupOnSurface->TextLabel2->setText( tr( "GEOM_UPARAMETER" ) );
  GroupOnSurface->TextLabel3->setText( tr( "GEOM_VPARAMETER" ) );
  GroupOnSurface->PushButton1->setIcon( image2 );

  GroupRefPoint = new DlgRef_1Sel3Spin( centralWidget() );
  GroupRefPoint->GroupBox1->setTitle( tr( "GEOM_REF_POINT" ) );
  GroupRefPoint->TextLabel1->setText( tr( "GEOM_POINT" ) );
  GroupRefPoint->PushButton1->setIcon( image2 );
  GroupRefPoint->TextLabel2->setText( tr( "GEOM_DX" ) );
  GroupRefPoint->TextLabel3->setText( tr( "GEOM_DY" ) );
  GroupRefPoint->TextLabel4->setText( tr( "GEOM_DZ" ) );

  GroupLineIntersection = new DlgRef_2Sel( centralWidget() );
  GroupLineIntersection->GroupBox1->setTitle( tr( "GEOM_LINE_INTERSECTION" ) );
  GroupLineIntersection->TextLabel1->setText( tr( "GEOM_LINE1" ) );
  GroupLineIntersection->TextLabel2->setText( tr( "GEOM_LINE2" ) );
  GroupLineIntersection->PushButton1->setIcon( image2 );
  GroupLineIntersection->PushButton2->setIcon( image2 );
  GroupLineIntersection->LineEdit2->setEnabled(false);

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
  layout->addWidget( GroupOnSurface );
  layout->addWidget( GroupRefPoint );
  layout->addWidget( GroupLineIntersection );
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

  setHelpFileName( "create_point_page.html" );
 
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
  GroupOnSurface->LineEdit1->setReadOnly( true );
  GroupRefPoint->LineEdit1->setReadOnly( true );
  GroupLineIntersection->LineEdit1->setReadOnly( true );
  GroupLineIntersection->LineEdit2->setReadOnly( true );
  GroupLineIntersection->LineEdit1->setEnabled( true );
  GroupLineIntersection->LineEdit2->setEnabled( false );

  myEdge = GEOM::GEOM_Object::_nil();
  myRefPoint = GEOM::GEOM_Object::_nil();

  myEditCurrentArgument = 0;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100 );
  
  /* min, max, step and decimals for spin boxes */
  initSpinBox( GroupXYZ->SpinBox_DX, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupXYZ->SpinBox_DY, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupXYZ->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupXYZ->SpinBox_DX->setValue( 0.0 );
  GroupXYZ->SpinBox_DY->setValue( 0.0 );
  GroupXYZ->SpinBox_DZ->setValue( 0.0 );

  initSpinBox( GroupRefPoint->SpinBox_DX, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupRefPoint->SpinBox_DY, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  initSpinBox( GroupRefPoint->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 6 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupRefPoint->SpinBox_DX->setValue( 0.0 );
  GroupRefPoint->SpinBox_DY->setValue( 0.0 );
  GroupRefPoint->SpinBox_DZ->setValue( 0.0 );

  step = 0.1;

  initSpinBox( GroupOnCurve->SpinBox_DX, 0., 1., step, 6 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupOnCurve->SpinBox_DX->setValue( 0.5 );

  initSpinBox( GroupOnSurface->SpinBox_DX, 0., 1., step, 6 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupOnSurface->SpinBox_DX->setValue( 0.5 );
  initSpinBox( GroupOnSurface->SpinBox_DY, 0., 1., step, 6 ); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupOnSurface->SpinBox_DY->setValue( 0.5 );

  /* signals and slots connections */
  connect( myGeomGUI,      SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI,      SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupOnCurve->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupOnCurve->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupOnSurface->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupOnSurface->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupLineIntersection->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupLineIntersection->PushButton2, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupLineIntersection->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupLineIntersection->LineEdit2,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupOnCurve->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupOnSurface->SpinBox_DX, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupOnSurface->SpinBox_DY, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DX,       SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DY,       SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupXYZ->SpinBox_DZ,       SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DX,  SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DY,  SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupRefPoint->SpinBox_DZ,  SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this,  SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged() ),
	   this,  SLOT(SelectionIntoArgument() ) );

  
  initName( tr( "GEOM_VERTEX" ) );
  
  ConstructorsClicked( 0 );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_PointDlg::SetDoubleSpinBoxStep( double step )
{
  GroupOnCurve->SpinBox_DX->setSingleStep(step);
  GroupXYZ->SpinBox_DX->setSingleStep(step);
  GroupXYZ->SpinBox_DY->setSingleStep(step);
  GroupXYZ->SpinBox_DZ->setSingleStep(step);
  GroupRefPoint->SpinBox_DX->setSingleStep(step);
  GroupRefPoint->SpinBox_DY->setSingleStep(step);
  GroupRefPoint->SpinBox_DZ->setSingleStep(step);
  GroupOnSurface->SpinBox_DX->setSingleStep(step);
  GroupOnSurface->SpinBox_DY->setSingleStep(step);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PointDlg::ConstructorsClicked(int constructorId)
{  
  globalSelection(); // close local contexts, if any

  switch ( constructorId ) {
  case 0:
    {
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();

      myCoordGrp->hide();

      GroupXYZ->show();
      break;
    }
  case 1:
    {
      myEditCurrentArgument = GroupRefPoint->LineEdit1;
      myEditCurrentArgument->setText( "" );
      myRefPoint = GEOM::GEOM_Object::_nil();
      GroupRefPoint->PushButton1->setDown(true);      
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      
      GroupXYZ->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      
      GroupRefPoint->show();
      
      myCoordGrp->show();
      break;
    }
  case 2:
    {
      myEditCurrentArgument = GroupOnCurve->LineEdit1;
      myEditCurrentArgument->setText( "" );
      myEdge = GEOM::GEOM_Object::_nil();
      GroupOnCurve->PushButton1->setDown(true);
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();

      GroupOnCurve->show();
      
      myCoordGrp->show();
      break;
    }
  case 3:
    {
      myEditCurrentArgument = GroupLineIntersection->LineEdit1;
      GroupLineIntersection->LineEdit1->setText( "" );
      GroupLineIntersection->LineEdit2->setText( "" );
      GroupLineIntersection->LineEdit1->setEnabled( true );
      GroupLineIntersection->LineEdit2->setEnabled( false );
      myLine1 = GEOM::GEOM_Object::_nil();
      myLine2 = GEOM::GEOM_Object::_nil();
      GroupLineIntersection->PushButton1->setDown(true);
      GroupLineIntersection->PushButton2->setDown(false);

      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupOnSurface->hide();

      myCoordGrp->hide();

      GroupLineIntersection->show();
      break;
    }
  case 4:
    {
      myEditCurrentArgument = GroupOnSurface->LineEdit1;
      myEditCurrentArgument->setText( "" );
      myFace = GEOM::GEOM_Object::_nil();
      GroupOnSurface->PushButton1->setDown(true);
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();

      GroupOnSurface->show();

      myCoordGrp->show();
      break;
    }
  }

  myX->setText( "" );
  myY->setText( "" );
  myZ->setText( "" );

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

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

  if ( ( id == 1 || id == 2 || id == 4 ) && myEditCurrentArgument != 0 )
  {
    myEditCurrentArgument->setText( "" );
    myX->setText( "" );
    myY->setText( "" );
    myZ->setText( "" );
    myRefPoint = myEdge = myFace = GEOM::GEOM_Object::_nil();
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    Standard_Boolean aRes = Standard_False;
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
      QString aName = GEOMBase::GetName(aSelectedObject);
      TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
        if ( id == 2 || id == 3 )
          aNeedType = TopAbs_EDGE;
        else if ( id == 4 )
          aNeedType = TopAbs_FACE;

        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(anIO, aMap);
        if ( aMap.Extent() == 1 ) { // Local Selection
          int anIndex = aMap( 1 );
          if ( aNeedType == TopAbs_EDGE )
            aName += QString( ":edge_%1" ).arg( anIndex );
          else
            aName += QString( ":vertex_%1" ).arg( anIndex );

	  //Find SubShape Object in Father
	  GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);
	  
	  if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	    GEOM::GEOM_IShapesOperations_var aShapesOp =
              getGeomEngine()->GetIShapesOperations(getStudyId());
	    aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	  }
	  else {
	    aSelectedObject = aFindedObject; // get Object from study
	  }
	  GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE );
        }
        else { // Global Selection
          if ( aShape.ShapeType() != aNeedType ) {
            aSelectedObject = GEOM::GEOM_Object::_nil();
            aName = "";
            if ( id == 0 ) return;
          }
        }
      }

      if ( aShape.IsNull() || aShape.ShapeType() != aNeedType)
	return;

      if ( id == 0 ) {
        gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
        GroupXYZ->SpinBox_DX->setValue( aPnt.X() );
        GroupXYZ->SpinBox_DY->setValue( aPnt.Y() );
        GroupXYZ->SpinBox_DZ->setValue( aPnt.Z() );
      }
      else if ( id == 1 ) {
        myRefPoint = aSelectedObject;
        GroupRefPoint->LineEdit1->setText( aName );
      }
      else if ( id == 2 ) {
        myEdge = aSelectedObject;
        GroupOnCurve->LineEdit1->setText( aName );
      }
      else if ( id == 3 ) {
	myEditCurrentArgument->setText( aName );
	globalSelection();
	localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
        if ( myEditCurrentArgument == GroupLineIntersection->LineEdit1 ) {
          myLine1 = aSelectedObject;
	  if ( !myLine1->_is_nil() && myLine2->_is_nil() )
	    GroupLineIntersection->PushButton2->click();
        }
        else if ( myEditCurrentArgument == GroupLineIntersection->LineEdit2 ) {
          myLine2 = aSelectedObject;
	  if ( !myLine2->_is_nil() && myLine1->_is_nil() )
	    GroupLineIntersection->PushButton1->click();
        }
      }
      else if ( id == 4 )
      {
	myFace = aSelectedObject;
	GroupOnSurface->LineEdit1->setText( aName );
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
  if ( send == GroupRefPoint->LineEdit1         || 
       send == GroupOnCurve->LineEdit1          ||
       send == GroupLineIntersection->LineEdit1 || 
       send == GroupLineIntersection->LineEdit2  ) {
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
  globalSelection(); // close local contexts, if any

  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupRefPoint->PushButton1 ) {
    GroupRefPoint->LineEdit1->setFocus();
    myEditCurrentArgument = GroupRefPoint->LineEdit1;
    
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  }
  else if ( send == GroupOnCurve->PushButton1 ) {
    GroupOnCurve->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnCurve->LineEdit1;
    
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else if ( send == GroupOnSurface->PushButton1 )
  {
    GroupOnSurface->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnSurface->LineEdit1;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
  }
  else if ( send == GroupLineIntersection->PushButton1 ) {
    GroupLineIntersection->LineEdit1->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit1;
    GroupLineIntersection->PushButton2->setDown( false );
    GroupLineIntersection->LineEdit1->setEnabled(true);
    GroupLineIntersection->LineEdit2->setEnabled(false);
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else if ( send == GroupLineIntersection->PushButton2 ) {
    GroupLineIntersection->LineEdit2->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit2;
    GroupLineIntersection->PushButton1->setDown( false );
    GroupLineIntersection->LineEdit1->setEnabled(false);
    GroupLineIntersection->LineEdit2->setEnabled(true);
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  send->setDown(true);
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
// funcion  : getUParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getUParameter() const
{
  return GroupOnSurface->SpinBox_DX->value();
}


//=================================================================================
// funcion  : getVParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getVParameter() const
{
  return GroupOnSurface->SpinBox_DY->value();
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
  if ( id == 0 ) {
    bool ok = true;
    ok = GroupXYZ->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
    ok = GroupXYZ->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
    ok = GroupXYZ->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
    return ok;
  }
  else if ( id == 1 ) {
    bool ok = true;
    ok = GroupRefPoint->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
    ok = GroupRefPoint->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
    ok = GroupRefPoint->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;

    return !myRefPoint->_is_nil() && ok;
  }
  else if ( id == 2 ) {
    bool ok = GroupOnCurve->SpinBox_DX->isValid( msg, !IsPreview() );
    return !myEdge->_is_nil() && ok;
  }
  else if ( id == 3 )
    return ( !myLine1->_is_nil() && !myLine2->_is_nil() );
  else if ( id == 4 ) {
    bool ok = true;
    ok = GroupOnSurface->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
    ok = GroupOnSurface->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
    return !myFace->_is_nil() && ok;
    
  }
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
  QStringList aParameters;

  switch ( getConstructorId() ) {
  case 0 :
    {
      double x = GroupXYZ->SpinBox_DX->value();
      double y = GroupXYZ->SpinBox_DY->value();
      double z = GroupXYZ->SpinBox_DZ->value();

      aParameters << GroupXYZ->SpinBox_DX->text();
      aParameters << GroupXYZ->SpinBox_DY->text();
      aParameters << GroupXYZ->SpinBox_DZ->text();
      
      anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePointXYZ( x, y, z );
      res = true;
      break;
    }
  case 1 :
    {
      double dx = GroupRefPoint->SpinBox_DX->value();
      double dy = GroupRefPoint->SpinBox_DY->value();
      double dz = GroupRefPoint->SpinBox_DZ->value();

      aParameters << GroupRefPoint->SpinBox_DX->text();
      aParameters << GroupRefPoint->SpinBox_DY->text();
      aParameters << GroupRefPoint->SpinBox_DZ->text();
      
      anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
	MakePointWithReference( myRefPoint, dx, dy, dz );
      res = true;
      break;
    }
  case 2 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnCurve( myEdge, getParameter() );
    aParameters<<GroupOnCurve->SpinBox_DX->text();
    res = true;
    break;
  case 3 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnLinesIntersection( myLine1, myLine2 );
    res = true;
    break;
  case 4 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnSurface( myFace, getUParameter(), getVParameter() );
    aParameters<<GroupOnSurface->SpinBox_DX->text();
    aParameters<<GroupOnSurface->SpinBox_DY->text();
    res = true;
    break;
  }
  
  if(!anObj->_is_nil() && !IsPreview() && (getConstructorId()==0 || 
					   getConstructorId() == 1 ||
					   getConstructorId() == 2 ||
					   getConstructorId() == 4) ) {
    anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
  }
  

  if ( getConstructorId() == 1 || getConstructorId() == 2 ||
       getConstructorId() == 4 ) {
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( anObj, aShape ) && !aShape.IsNull() &&
         aShape.ShapeType() == TopAbs_VERTEX ) {
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
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_PointDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch ( getConstructorId() ) {
  case 0:
    break;
  case 1:
    objMap[GroupRefPoint->LineEdit1->text()] = myRefPoint;
    break;
  case 2:
    objMap[GroupOnCurve->LineEdit1->text()] = myEdge;
    break;
  case 3:
    objMap[GroupLineIntersection->LineEdit1->text()] = myLine1;
    objMap[GroupLineIntersection->LineEdit2->text()] = myLine2;
    break;
  case 4:
    objMap[GroupOnSurface->LineEdit1->text()] = myFace;
    break;
  }
  addSubshapesToFather( objMap );
}
