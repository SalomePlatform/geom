//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : RepairGUI_ShapeProcessDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "RepairGUI_ShapeProcessDlg.h"

#include "GEOMImpl_Types.hxx"

#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SUIT_Session.h"
#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOME_ListIO.hxx"

#include <TCollection_AsciiString.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <qheader.h>
#include <qlabel.h>
#include <qmessagebox.h>

using namespace std;

//=================================================================================
// class    : RepairGUI_ShapeProcessDlg()
// purpose  : Constructs a RepairGUI_ShapeProcessDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_ShapeProcessDlg::RepairGUI_ShapeProcessDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                                     const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  setHelpFileName("shape_processing.htm");
  init();
}

//=================================================================================
// function : ~RepairGUI_ShapeProcessDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_ShapeProcessDlg::~RepairGUI_ShapeProcessDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::init()
{
  //myGeomGUI->SetState( 0 );

  initParamsValues();
  initSelection();
	
  setCaption(tr("GEOM_SHAPEPROCESS_TITLE"));

  GroupConstructors->hide();
  
  // select widget on the top 
  mySelectWdgt = new DlgRef_1Sel_QTD( this, "SelectedObjects" );
  mySelectWdgt->GroupBox1->setTitle( tr("GEOM_SHAPE") );
  mySelectWdgt->TextLabel1->setText( tr("GEOM_SELECTED_OBJECTS") );
  mySelectWdgt->PushButton1->setPixmap( SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")) );
  mySelectWdgt->LineEdit1->setReadOnly( true );

  Layout1->addWidget( mySelectWdgt, 0, 0 );

  // layout the two group boxes in the middle, add a list of operations
  QFrame* aMidFrame = new QFrame( this );
  QGridLayout* aMidLay = new QGridLayout( aMidFrame, 1, 3, 0, 5 );
  QGroupBox* anOperGr = new QGroupBox( tr("GEOM_OPERATIONS"), aMidFrame );
  QVBoxLayout* aOperLay = new QVBoxLayout( anOperGr, 0, 0 );

  myOpList = new QListView( anOperGr );
  myOpList->setSorting( -1 );
  myOpList->addColumn( "Operations" );
  myOpList->header()->hide();
  aOperLay->addWidget( myOpList );
  QStringList::ConstIterator it = myOpLst.end();
  do // iterating from end to begin so to keep the order of items in the listview.	
    new QCheckListItem ( myOpList, *(--it), QCheckListItem::CheckBox );
  while ( it != myOpLst.begin() );

  QGroupBox* aParamsGr = new QGroupBox( tr("GEOM_PARAMETERS"), aMidFrame );
  aMidLay->addWidget( anOperGr, 0, 0 );
  aMidLay->addMultiCellWidget( aParamsGr, 0, 0, 1, 3 );
  aMidLay->setColStretch( 0, 1 );
  aMidLay->setColStretch( 1, 2 );

  Layout1->addWidget( aMidFrame, 2, 0 );
  Layout1->setRowStretch( 0, 0 );
  Layout1->setRowStretch( 1, 1 );

  // add a widget stack to the parameters group box
  myStack = new QWidgetStack( aParamsGr );
  QVBoxLayout* aVParamsLay = new QVBoxLayout( aParamsGr );
  aVParamsLay->addWidget( myStack );

  // continueties values..
  QStringList aContinueties = QStringList::split( ",", "C0,G1,C1,G2,C2,C3,CN" );

  // FixShape
  QFrame* aFrame = new QFrame( myStack );
  QGridLayout* aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  myFixShapeTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myFixShapeTol3D->setPrecision( 10 );
  myFixShapeMaxTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myFixShapeMaxTol3D->setPrecision( 10 );

  aLay->addWidget( new QLabel( tr("GEOM_3D_TOLERANCE"), aFrame ), 0, 0 );
  aLay->addWidget( myFixShapeTol3D, 0, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_MAX_3D_TOLERANCE"), aFrame ), 1, 0 );
  aLay->addWidget( myFixShapeMaxTol3D, 1, 1 );
  aLay->setRowStretch( 9, 2 );

  myStack->addWidget( aFrame, myOpLst.findIndex( "FixShape" ) );

  // FixFaceSize  
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  myFixFaceSizeTol = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myFixFaceSizeTol->setPrecision( 10 );

  aLay->addWidget( new QLabel( tr("GEOM_TOLERANCE"), aFrame ), 0, 0 );
  aLay->addWidget( myFixFaceSizeTol, 0, 1 );
  aLay->setRowStretch( 9, 2 );

  myStack->addWidget( aFrame, myOpLst.findIndex( "FixFaceSize" ) );

  // DropSmallEdges
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  myDropSmallEdgesTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myDropSmallEdgesTol3D->setPrecision( 10 );
  
  aLay->addWidget( new QLabel( tr("GEOM_3D_TOLERANCE"), aFrame ), 0, 0 );
  aLay->addWidget( myDropSmallEdgesTol3D, 0, 1 );
  aLay->setRowStretch( 9, 2 );

  myStack->addWidget( aFrame, myOpLst.findIndex( "DropSmallEdges" ) );

  // SplitAngle
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  mySplitAngleAngle = new QtxDblSpinBox( 0, 360, 1, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 360, 1 );
  mySplitAngleMaxTol = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  mySplitAngleMaxTol->setPrecision( 10 );

  aLay->addWidget( new QLabel( tr("GEOM_ANGLE_1"), aFrame ), 0, 0 );
  aLay->addWidget( mySplitAngleAngle, 0, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_MAX_TOLERANCE"), aFrame ), 1, 0 );
  aLay->addWidget( mySplitAngleMaxTol, 1, 1 );
  aLay->setRowStretch( 9, 2 );

  myStack->addWidget( aFrame, myOpLst.findIndex( "SplitAngle" ) );

  // SplitClosedFaces
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  mySplitClosedFacesNum = new QSpinBox( aFrame );
  
  aLay->addWidget( new QLabel( tr("GEOM_NUM_SPLIT_POINTS"), aFrame ), 0, 0 );
  aLay->addWidget( mySplitClosedFacesNum, 0, 1 );
  aLay->setRowStretch( 9, 2 );

  myStack->addWidget( aFrame, myOpLst.findIndex( "SplitClosedFaces" ) );

  // SplitContinuity
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  mySplitContTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  mySplitContTol3D->setPrecision( 10 );
  mySplitContSurfCont = new QComboBox( aFrame );
  mySplitContSurfCont->insertStringList( aContinueties );
  mySplitContCurvCont = new QComboBox( aFrame );
  mySplitContCurvCont->insertStringList( aContinueties );

  aLay->addWidget( new QLabel( tr("GEOM_3D_TOLERANCE"), aFrame ), 0, 0 );
  aLay->addWidget( mySplitContTol3D, 0, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_SURFACE_CONTINUTY"), aFrame ), 1, 0 );
  aLay->addWidget( mySplitContSurfCont, 1, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_CURVE_CONTINUTY"), aFrame ), 2, 0 );
  aLay->addWidget( mySplitContCurvCont, 2, 1 );
  aLay->setRowStretch( 9, 2 );
  
  myStack->addWidget( aFrame, myOpLst.findIndex( "SplitContinuity" ) );

  // BSplineRestriction
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  myBSplineSurfModeChk = new QCheckBox( tr("GEOM_SURFACE_MODE"), aFrame );
  myBSpline3DCurveChk = new QCheckBox( tr("GEOM_3D_CURVE_MODE"), aFrame );
  myBSpline2DCurveChk = new QCheckBox( tr("GEOM_2D_CURVE_MODE"), aFrame );

  myBSplineTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myBSplineTol3D->setPrecision( 10 );

  myBSplineTol2D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myBSplineTol2D->setPrecision( 10 );

  myBSplineDegree = new QSpinBox( aFrame );
  myBSplineSegments = new QSpinBox( aFrame );
  myBSpline2DCont = new QComboBox( aFrame );
  myBSpline2DCont->insertStringList( aContinueties );
  myBSpline3DCont = new QComboBox( aFrame );
  myBSpline3DCont->insertStringList( aContinueties );

  myBSplineSurfModeChk->setChecked( true );
  myBSpline3DCurveChk->setChecked( true );
  myBSpline2DCurveChk->setChecked( true );

  aLay->addWidget( myBSplineSurfModeChk, 0, 0 );
  aLay->addWidget( myBSpline3DCurveChk, 1, 0 );
  aLay->addWidget( myBSpline2DCurveChk, 2, 0 );
  aLay->addWidget( new QLabel( tr("GEOM_3D_TOLERANCE"), aFrame), 3, 0 );
  aLay->addWidget( myBSplineTol3D, 3, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_2D_TOLERANCE"), aFrame), 4, 0 );
  aLay->addWidget( myBSplineTol2D, 4, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_REQUIRED_DEGREE"), aFrame), 5, 0 );
  aLay->addWidget( myBSplineDegree, 5, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_REQUIRED_NUM_SEGMENTS"), aFrame), 6, 0 );
  aLay->addWidget( myBSplineSegments, 6, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_3D_CONTINUTY"), aFrame), 7, 0 );
  aLay->addWidget( myBSpline3DCont, 7, 1 );
  aLay->addWidget( new QLabel( tr("GEOM_2D_CONTINUTY"), aFrame), 8, 0 );
  aLay->addWidget( myBSpline2DCont, 8, 1 );
  aLay->setRowStretch( 9, 2 );
  
  myStack->addWidget( aFrame, myOpLst.findIndex( "BSplineRestriction" ) );

  // ToBezier
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  myToBezierSurfModeChk = new QCheckBox( tr("GEOM_SURFACE_MODE"), aFrame );
  myToBezier3DCurveChk = new QCheckBox( tr("GEOM_3D_CURVE_MODE"), aFrame );
  myToBezier2DCurveChk = new QCheckBox( tr("GEOM_2D_CURVE_MODE"), aFrame );
  
  myToBezierMaxTol = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  myToBezierMaxTol->setPrecision( 10 );

  aLay->addWidget( myToBezierSurfModeChk, 0, 0 );
  aLay->addWidget( myToBezier3DCurveChk, 1, 0 );
  aLay->addWidget( myToBezier2DCurveChk, 2, 0 );
  aLay->addWidget( new QLabel( tr("GEOM_MAX_TOLERANCE"), aFrame), 3, 0 );
  aLay->addWidget( myToBezierMaxTol, 3, 1 );
  aLay->setRowStretch( 9, 2 );
  
  myStack->addWidget( aFrame, myOpLst.findIndex( "ToBezier" ) );

  // SameParameter
  aFrame = new QFrame( myStack );
  aLay = new QGridLayout( aFrame, 10, 2, 0, 5 );

  mySameParameterTol3D = new QtxDblSpinBox( 0, 100, 1e-7, aFrame );//QAD_SpinBoxDbl( aFrame, 0, 100, 1e-7, 10, 1e-10 );
  mySameParameterTol3D->setPrecision( 10 );

  aLay->addWidget( new QLabel( tr("GEOM_3D_TOLERANCE"), aFrame ), 0, 0 );
  aLay->addWidget( mySameParameterTol3D, 0, 1 );
  aLay->setRowStretch( 9, 2 );
  
  myStack->addWidget( aFrame, myOpLst.findIndex( "SameParameter" ) );

  // signals and slots connections
  connect( buttonOk,     SIGNAL(clicked()), this, SLOT(onOk()) );
  connect( buttonApply,  SIGNAL(clicked()), this, SLOT(onApply()) );

  connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),  
	   SIGNAL(currentSelectionChanged()), this, SLOT(selectionChanged()) );
  connect( myOpList,     SIGNAL(selectionChanged()), this, SLOT(operationChanged()) );

  connect( mySelectWdgt->PushButton1, SIGNAL(clicked()), this, SLOT(selectClicked()) );
  connect( mySelectWdgt->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(lineEditReturnPressed()) );

  connect( myToBezierSurfModeChk,     SIGNAL(toggled(bool)), SLOT(advOptionToggled(bool)) );

  adjustSize();
  loadDefaults(); // init dialog fields with values from resource file
  myOpList->setSelected( myOpList->findItem( "FixShape", 0 ), true );
  reset();

  initName( tr( "PROCESS_SHAPE_NEW_OBJ_NAME" ) );
}

//=================================================================================
// function : operationChanged()
// purpose  : 
//=================================================================================
void RepairGUI_ShapeProcessDlg::operationChanged()
{
  QListViewItem* anItem = myOpList->selectedItem();
  if ( anItem )
  {
    int id = myOpLst.findIndex( anItem->text(0) );
    if ( id > -1 )
      myStack->raiseWidget( id );
  }
}

//=================================================================================
// function : onOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_ShapeProcessDlg::onOk()
{
  if (onApply())
    ClickOnCancel();
}

//=================================================================================
// function : onApply()
// purpose  :
//=================================================================================
bool RepairGUI_ShapeProcessDlg::onApply()
{
  if ( !onAccept() )
    return false;

  initName();

  reset();
  initSelection();
  
  return true;
}


//=================================================================================
// function : selectionChanged()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_ShapeProcessDlg::selectionChanged()
{
	reset();
	
  Standard_Boolean aRes = Standard_False;
  int i = 0;
  myObjects->length( IObjectCount() );
  for ( SALOME_ListIteratorOfListIO anIt( selectedIO() ); anIt.More(); anIt.Next() )
  {
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIt.Value(), aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes )
	myObjects[i++] = aSelectedObject;
  }
  myObjects->length( i );
  if ( i == 1 )
    mySelectWdgt->LineEdit1->setText( GEOMBase::GetName( myObjects[0] ) );
  else if ( i > 0 )
    mySelectWdgt->LineEdit1->setText( QString::number( i ) + "_" + tr( "GEOM_OBJECTS" ) );
}


//=================================================================================
// function : selectClicked()
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::selectClicked()
{
  myEditCurrentArgument = mySelectWdgt->LineEdit1;
  mySelectWdgt->LineEdit1->setFocus();
  selectionChanged();
}


//=================================================================================
// function : lineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::lineEditReturnPressed()
{
  GEOMBase_Skeleton::LineEditReturnPressed();
}


//=================================================================================
// function : activate()
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::activate()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(selectionChanged()));
	
  reset();
  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_ShapeProcessDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    activate();
}


//=================================================================================
// function : closeEvent()
// purpose  : same than click on cancel button
//=================================================================================
void RepairGUI_ShapeProcessDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : reset()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void RepairGUI_ShapeProcessDlg::reset()
{
  myObjects = new GEOM::ListOfGO();
  myObjects->length( 0 );	
  mySelectWdgt->LineEdit1->setText("");
}


//=================================================================================
// function : get_convert
// purpose  : conversion of angle values to radians (non-angle values are not converted)
//=================================================================================
const char* get_convert( const char* theParam, const char* theValue )
{
  if ( !strcmp( theParam, "SplitAngle.Angle" ) )
  {
    double doubleValue = atof( theValue ) * PI / 180;
    TCollection_AsciiString str( doubleValue );
    return CORBA::string_dup( str.ToCString() );
  }
  return CORBA::string_dup( theValue );
}

//=================================================================================
// function : set_convert
// purpose  : conversion of angle values to degrees (non-angle values are not converted)
//=================================================================================
const char* set_convert( const char* theParam, const char* theValue )
{
  if ( !strcmp( theParam, "SplitAngle.Angle" ) )
  {
    double doubleValue = atof( theValue ) * 180 / PI;
    TCollection_AsciiString str( doubleValue );
    return CORBA::string_dup( str.ToCString() );
  }
  return CORBA::string_dup( theValue );
}

//=================================================================================
// function : loadDefaults()
// purpose  : initialize "values"-fields with default values retrieved from IHealingOperations
//=================================================================================
void RepairGUI_ShapeProcessDlg::loadDefaults()
{
  GEOM::GEOM_IHealingOperations_var anOp = myGeomGUI->GetGeomGen()->GetIHealingOperations( getStudyId() );
  GEOM::string_array_var anOperators, aParams, aValues;
  anOp->GetShapeProcessParameters( anOperators, aParams, aValues );

  // check the default items-operators
  int i;
  for ( i = 0; i < anOperators->length(); i++ )
  {
    //MESSAGE("-->"<<(const char*)anOperators[i]);
    QListViewItem* anItem = myOpList->findItem( (const char*)anOperators[i], 0 );
    if ( anItem /*&& anItem->inherits( "QCheckListItem" )*/ )
      ((QCheckListItem*) anItem)->setOn( true );
  }

  // Retrieve default parameters for ALL operators
  for ( QListViewItemIterator it(myOpList); it.current(); ++it ) {
    CORBA::String_var anOperator = CORBA::string_dup( it.current()->text( 0 ).latin1() );
    anOp->GetOperatorParameters( anOperator.in(), aParams, aValues );

    // set default values of parameters
    if ( aParams->length() != aValues->length() )
      continue;
    for ( i = 0; i < aParams->length(); i++ ) {
      QWidget* aCtrl = getControl( (const char*)aParams[i] );
      const char* aValue = set_convert( (const char*)aParams[i], aValues[i] );
      setValue( aCtrl, aValue );
    }
  }
}

//=================================================================================
// function : setValue()
// purpose  : set value in the proper way
//=================================================================================
void RepairGUI_ShapeProcessDlg::setValue( QWidget* theControl, const char* theValue )
{
  if ( theControl == NULL || theValue == NULL )
    return;

  if ( theControl->isA( "QtxDblSpinBox" ) )
    ((QtxDblSpinBox*)theControl)->setValue( QString( theValue ).toDouble() );
  else if ( theControl->isA( "QSpinBox" ) )
    ((QSpinBox*)theControl)->setValue( QString( theValue ).toInt() );
  else if ( theControl->isA( "QComboBox" ) )
    ((QComboBox*)theControl)->setCurrentText( QString( theValue ) );
  else if ( theControl->isA( "QCheckBox" ) )
    ((QCheckBox*)theControl)->setChecked( QString( theValue ).toInt() != 0 );
}

//=================================================================================
// function : getValue()
// purpose  : get value in the proper way
//=================================================================================
const char* RepairGUI_ShapeProcessDlg::getValue( QWidget* theControl ) const
{
  if ( theControl == NULL )
    return "";

  if ( theControl->isA( "QtxDblSpinBox" ) )
    return ((QtxDblSpinBox*)theControl)->text().latin1();
  else if ( theControl->isA( "QSpinBox" ) )
    return ((QSpinBox*)theControl)->text().latin1();
  else if ( theControl->isA( "QComboBox" ) )
    return ((QComboBox*)theControl)->currentText().latin1();
  else if ( theControl->isA( "QCheckBox" ) )
    return ((QCheckBox*)theControl)->isChecked() ? "1" : "0";
  
  return "";
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_ShapeProcessDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_ShapeProcessDlg::isValid( QString& msg )
{
  bool error = false;
  GEOM::string_array_var anOperators = getActiveOperators();
  if ( !myObjects->length() )
  {
    msg += tr( "ERROR_NO_OBJECTS" );
    error = true;
  }
  if ( !anOperators->length() )
  {
    if ( error ) 
      msg += "\n";
    msg += tr( "ERROR_NO_OPERATORS" );
    error = true;
  }
  return !error;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_ShapeProcessDlg::execute( ObjectList& objects )
{
  GEOM::string_array_var anOperators = getActiveOperators();
  GEOM::string_array_var aParams = getParameters( anOperators );
  GEOM::string_array_var aValues = getValues( aParams );

  /*//-- check --
  int z;
	MESSAGE("Objects : ");
	for ( z = 0; z < myObjects->length(); z++ )
 		MESSAGE(myObjects[z]->GetName() << " ");
	MESSAGE("\nOperators : ");
	for ( z = 0; z < anOperators->length(); z++ )
		MESSAGE(anOperators[z] << " ");
	MESSAGE("\nParameters : ");
	for ( z = 0; z < aParams->length(); z++ )
		MESSAGE(aParams[z] << " ");
	MESSAGE("\nValues : ");
	for ( z = 0; z < aValues->length(); z ++ )
		MESSAGE(aValues[z] << " ");
	MESSAGE("\n");
  */// -----------

  QStringList anErrorObjNames;
  for ( int i = 0; i < myObjects->length(); i++ )
  {
    GEOM::GEOM_Object_var obj = myObjects[i];
    GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->ProcessShape(
	obj, anOperators, aParams, aValues );
    if ( anObj->_is_nil() )
      anErrorObjNames << GEOMBase::GetName( obj );
    else
      objects.push_back( anObj._retn() );
  }

  if ( !anErrorObjNames.empty() )
    MESSAGE("ERRORS occured while processing the following objects: " << anErrorObjNames.join( " " ));
    
  return anErrorObjNames.size() < myObjects->length(); // true if at least one object was OK, false if ALL objects were nil after Healing.
}

//=================================================================================
// function : getActiveOperators
// purpose  :
//=================================================================================
GEOM::string_array* RepairGUI_ShapeProcessDlg::getActiveOperators()
{
  GEOM::string_array_var anOperators = new GEOM::string_array();
    QStringList aCheckedList;
  QStringList::Iterator it;
  for ( it = myOpLst.begin(); it != myOpLst.end(); ++it )
    if ( ((QCheckListItem*)myOpList->findItem( *it, 0 ))->isOn() )
      aCheckedList << *it;
  anOperators->length( aCheckedList.size() );
  int i = 0;
  for ( it = aCheckedList.begin(); it != aCheckedList.end(); ++it )
    anOperators[i++] = CORBA::string_dup( (*it).latin1() );		
	 	 
  return anOperators._retn();
}

//=================================================================================
// function : getcontrol
// purpose  :
//=================================================================================
QWidget* RepairGUI_ShapeProcessDlg::getControl( const char* theParam )
{
	string aParam = theParam;
  if ( aParam == "SplitAngle.Angle" ) 				return mySplitAngleAngle;
  else if ( aParam == "SplitAngle.MaxTolerance" ) 		return mySplitAngleMaxTol;
  else if ( aParam == "SplitClosedFaces.NbSplitPoints" )    	return mySplitClosedFacesNum;
  else if ( aParam == "FixFaceSize.Tolerance" )			return myFixFaceSizeTol;
  else if ( aParam == "DropSmallEdges.Tolerance3d" )        	return myDropSmallEdgesTol3D;
  else if ( aParam == "BSplineRestriction.SurfaceMode" )    	return myBSplineSurfModeChk;
  else if ( aParam == "BSplineRestriction.Curve3dMode" )    	return myBSpline3DCurveChk;
  else if ( aParam == "BSplineRestriction.Curve2dMode" )    	return myBSpline2DCurveChk;
  else if ( aParam == "BSplineRestriction.Tolerance3d" )    	return myBSplineTol3D;
  else if ( aParam == "BSplineRestriction.Tolerance2d" )    	return myBSplineTol2D;
  else if ( aParam == "BSplineRestriction.RequiredDegree" )	return myBSplineDegree;
  else if ( aParam == "BSplineRestriction.RequiredNbSegments" )	return myBSplineSegments;
  else if ( aParam == "BSplineRestriction.Continuity3d" )	return myBSpline3DCont;
  else if ( aParam == "BSplineRestriction.Continuity2d" )	return myBSpline2DCont;
  else if ( aParam == "SplitContinuity.Tolerance3d" )		return mySplitContTol3D;
  else if ( aParam == "SplitContinuity.SurfaceContinuity" )	return mySplitContSurfCont;
  else if ( aParam == "SplitContinuity.CurveContinuity" )	return mySplitContCurvCont;
  else if ( aParam == "ToBezier.SurfaceMode" )			return myToBezierSurfModeChk;
  else if ( aParam == "ToBezier.Curve3dMode" )			return myToBezier3DCurveChk;
  else if ( aParam == "ToBezier.Curve2dMode" )			return myToBezier2DCurveChk;
  else if ( aParam == "ToBezier.MaxTolerance" )			return myToBezierMaxTol;
  else if ( aParam == "SameParameter.Tolerance3d" )		return mySameParameterTol3D;
  else if ( aParam == "FixShape.Tolerance3d" )			return myFixShapeTol3D;
  else if ( aParam == "FixShape.MaxTolerance3d" )		return myFixShapeMaxTol3D;
  return NULL;
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::initParamsValues()
{
  myOpLst << "FixShape";
  myValMap["FixShape"] << "FixShape.Tolerance3d";
  myValMap["FixShape"] << "FixShape.MaxTolerance3d";

  myOpLst << "FixFaceSize";
  myValMap["FixFaceSize"] << "FixFaceSize.Tolerance";

  myOpLst << "DropSmallEdges";
  myValMap["DropSmallEdges"] << "DropSmallEdges.Tolerance3d";

  myOpLst << "SplitAngle";
  myValMap["SplitAngle"] << "SplitAngle.Angle";
  myValMap["SplitAngle"] << "SplitAngle.MaxTolerance";

  myOpLst << "SplitClosedFaces";
  myValMap["SplitClosedFaces"] << "SplitClosedFaces.NbSplitPoints";

  myOpLst << "SplitContinuity";
  myValMap["SplitContinuity"] << "SplitContinuity.Tolerance3d";
  myValMap["SplitContinuity"] << "SplitContinuity.SurfaceContinuity";
  myValMap["SplitContinuity"] << "SplitContinuity.CurveContinuity";

  myOpLst << "BSplineRestriction";
  myValMap["BSplineRestriction"] << "BSplineRestriction.SurfaceMode";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Curve3dMode";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Curve2dMode";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Tolerance3d";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Tolerance2d";
  myValMap["BSplineRestriction"] << "BSplineRestriction.RequiredDegree";
  myValMap["BSplineRestriction"] << "BSplineRestriction.RequiredNbSegments";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Continuity3d";
  myValMap["BSplineRestriction"] << "BSplineRestriction.Continuity2d";

  myOpLst << "ToBezier";
  myValMap["ToBezier"] << "ToBezier.SurfaceMode";
  myValMap["ToBezier"] << "ToBezier.Curve3dMode";
  myValMap["ToBezier"] << "ToBezier.Curve2dMode";
  myValMap["ToBezier"] << "ToBezier.MaxTolerance";

  myOpLst << "SameParameter";
  myValMap["SameParameter"] << "SameParameter.Tolerance3d";
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
GEOM::string_array* RepairGUI_ShapeProcessDlg::getParameters( const GEOM::string_array& theOperators )
{
	GEOM::string_array_var aParams = new GEOM::string_array();
	int i( 0 ), j( 0 );

	// calculate the length of parameters
	for ( i = 0, j = 0; i < theOperators.length(); i++ )
	  j+= myValMap[QString((const char*)theOperators[i])].size();

	// set the new length of paremeters
	aParams->length( j );

	// fill the parameters
	for ( i = 0, j = 0; i < theOperators.length(); i++ )
	{
		QStringList aValLst = myValMap[QString( (const char*)theOperators[i] )];
		for ( QStringList::Iterator it = aValLst.begin(); it != aValLst.end(); ++it )
     	aParams[j++] = CORBA::string_dup( (*it).latin1() );
	}

	return aParams._retn();
}


//=================================================================================
// function : getValues
// purpose  :
//=================================================================================
GEOM::string_array* RepairGUI_ShapeProcessDlg::getValues( const GEOM::string_array& theParams )
{
  GEOM::string_array_var aValues = new GEOM::string_array();
  aValues->length( theParams.length() );
    
  for ( int i = 0; i < theParams.length(); i++ )
  {
    QWidget* aCtrl = getControl( (const char*)theParams[i] );
    if ( aCtrl )
      aValues[i] = get_convert( (const char*)theParams[i], getValue( aCtrl ) );
  }
    
  return aValues._retn();
}

//=================================================================================
// function : initSelection
// purpose  : set selection of ALL shape types except vertexes
//=================================================================================
void RepairGUI_ShapeProcessDlg::initSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add( GEOM_COMPOUND );
  aTypes.Add( GEOM_SOLID );
  aTypes.Add( GEOM_SHELL );
  aTypes.Add( GEOM_FACE );
  aTypes.Add( GEOM_WIRE );
  aTypes.Add( GEOM_EDGE );
  globalSelection( aTypes );
}

//=================================================================================
// function : advOptionToggled
// purpose  : this slot is used to warn the user about possible consequences
//            of enabling some advanced options
//=================================================================================
void RepairGUI_ShapeProcessDlg::advOptionToggled( bool on )
{
  QButton* btn = (QButton*)sender();
  if ( on && btn->isToggleButton() &&
       QMessageBox::warning(myGeomGUI->getApp()->desktop(),
                            tr( "GEOM_WRN_WARNING" ), tr( "TIME_CONSUMING" ),
                            QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
    btn->toggle();
}
