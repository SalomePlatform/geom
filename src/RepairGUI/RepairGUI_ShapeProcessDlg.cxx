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
// File   : RepairGUI_ShapeProcessDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "RepairGUI_ShapeProcessDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "GEOMImpl_Types.hxx"
#include "RepairGUI.h"

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_IntSpinBox.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SALOME_ListIO.hxx>
#include "utilities.h"

#include <Basics_Utils.hxx>
#include "utilities.h"

#include <TCollection_AsciiString.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <QListWidget>
#include <QStackedLayout>

//=================================================================================
// class    : RepairGUI_ShapeProcessDlg()
// purpose  : Constructs a RepairGUI_ShapeProcessDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_ShapeProcessDlg::RepairGUI_ShapeProcessDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                      bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal )
{
  setHelpFileName( "shape_processing_operation_page.html" );
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
        
  setWindowTitle( tr( "GEOM_SHAPEPROCESS_TITLE" ));

  mainFrame()->GroupConstructors->hide();
  
  // select widget on the top 
  mySelectWdgt = new DlgRef_1Sel( centralWidget() );
  mySelectWdgt->GroupBox1->setTitle( tr( "GEOM_SHAPE" ));
  mySelectWdgt->TextLabel1->setText( tr( "GEOM_SELECTED_OBJECTS" ));
  mySelectWdgt->PushButton1->setIcon( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" )));
  mySelectWdgt->LineEdit1->setReadOnly( true );

  // layout the two group boxes in the middle, add a list of operations
  QGroupBox* anOperGr = new QGroupBox( tr( "GEOM_OPERATIONS" ), centralWidget() );

  // "select all" button
  mySelectAll = new QCheckBox( tr( "SELECT_ALL" ), anOperGr );
  mySelectAll->setTristate( true );
  
  // operations list widget
  myOpList = new QListWidget( anOperGr );
  myOpList->setSortingEnabled( false );
  myOpList->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ));

  QVBoxLayout* aOperLay = new QVBoxLayout( anOperGr );
  aOperLay->setMargin( 9 );
  aOperLay->addWidget( mySelectAll );
  aOperLay->addWidget( myOpList );

  QGroupBox* aParamsGr = new QGroupBox( tr( "GEOM_PARAMETERS" ), centralWidget() );

  // add a widget stack to the parameters group box
  myStack = new QStackedLayout( aParamsGr );

  // continueties values..
  QStringList aContinueties = QString( "C0,G1,C1,G2,C2,C3,CN" ).split( "," );

  // fill in the widgets
  for ( int i = 0; i < myOpLst.count(); i++ ) {
    QListWidgetItem* item = new QListWidgetItem( myOpLst[i] );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
    item->setCheckState( Qt::Unchecked );
    myOpList->addItem( item );

    QWidget* w;
    if ( myOpLst[i] == "FixShape" ) {
      // FixShape
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );
      
      myFixShapeTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myFixShapeTol3D, 0., 100., 1e-7, "len_tol_precision" );
      myFixShapeMaxTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myFixShapeMaxTol3D, 0., 100., 1e-7, "len_tol_precision" );      
      
      aLay->addWidget( new QLabel( tr( "GEOM_3D_TOLERANCE" ), w ), 0, 0 );
      aLay->addWidget( myFixShapeTol3D, 0, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_MAX_3D_TOLERANCE" ), w ), 1, 0 );
      aLay->addWidget( myFixShapeMaxTol3D, 1, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "FixFaceSize" ) {
      // FixFaceSize  
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );
      
      myFixFaceSizeTol = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myFixFaceSizeTol, 0., 100., 1e-7, "len_tol_precision" );      
      
      aLay->addWidget( new QLabel( tr( "GEOM_TOLERANCE" ), w ), 0, 0 );
      aLay->addWidget( myFixFaceSizeTol, 0, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "DropSmallSolids" ) {
      // DropSmallSolids
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );

      myDropSmallSolidsWidChk = new QCheckBox( tr("WIDTH_FACTOR_TOL"), w );
      myDropSmallSolidsVolChk = new QCheckBox( tr("VOLUME_TOL"), w );
      myDropSmallSolidsWidTol = new SalomeApp_DoubleSpinBox( w );
      myDropSmallSolidsVolTol = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myDropSmallSolidsWidTol, 0., 1e3, 1., "len_tol_precision" );
      initSpinBox( myDropSmallSolidsVolTol, 0., 1e9, 1., "len_tol_precision" );
      myDropSmallSolidsWidTol->setValue( 1 );
      myDropSmallSolidsVolTol->setValue( 1e3 );
      myDropSmallSolidsVolChk->setChecked( true );
      myDropSmallSolidsWidTol->setEnabled( false );
      myDropSmallSolidsMergeChk = new QCheckBox( tr("TO_MERGE_SOLIDS"), w );
  
      aLay->addWidget( myDropSmallSolidsWidChk, 0, 0 );
      aLay->addWidget( myDropSmallSolidsWidTol, 0, 1 );
      aLay->addWidget( myDropSmallSolidsVolChk, 1, 0 );
      aLay->addWidget( myDropSmallSolidsVolTol, 1, 1 );
      aLay->addWidget( myDropSmallSolidsMergeChk, 2, 0, 1, 2 );

      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "DropSmallEdges" ) {
      // DropSmallEdges
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );
      
      myDropSmallEdgesTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myDropSmallEdgesTol3D, 0., 100., 1e-7, "len_tol_precision" );
  
      aLay->addWidget( new QLabel( tr( "GEOM_3D_TOLERANCE" ), w ), 0, 0 );
      aLay->addWidget( myDropSmallEdgesTol3D, 0, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "SplitAngle" ) {
      // SplitAngle
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );
      
      mySplitAngleAngle = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( mySplitAngleAngle, 0, 360, 1, "angle_precision" );      
      mySplitAngleMaxTol = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( mySplitAngleMaxTol, 0., 100., 1e-7, "ang_tol_precision" );      
      
      aLay->addWidget( new QLabel( tr( "GEOM_ANGLE_1" ), w ), 0, 0 );
      aLay->addWidget( mySplitAngleAngle, 0, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_MAX_TOLERANCE" ), w ), 1, 0 );
      aLay->addWidget( mySplitAngleMaxTol, 1, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "SplitClosedFaces" ) {
      // SplitClosedFaces
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );
      
      mySplitClosedFacesNum = new SalomeApp_IntSpinBox( w );
  
      aLay->addWidget( new QLabel( tr( "GEOM_NUM_SPLIT_POINTS" ), w ), 0, 0 );
      aLay->addWidget( mySplitClosedFacesNum, 0, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }      
    else if ( myOpLst[i] == "SplitContinuity" ) {
      // SplitContinuity
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );

      mySplitContTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( mySplitContTol3D, 0., 100., 1e-7, "len_tol_precision" );
      mySplitContSurfCont = new QComboBox( w );
      mySplitContSurfCont->addItems( aContinueties );
      mySplitContCurvCont = new QComboBox( w );
      mySplitContCurvCont->addItems( aContinueties );
      
      aLay->addWidget( new QLabel( tr( "GEOM_3D_TOLERANCE" ), w ), 0, 0 );
      aLay->addWidget( mySplitContTol3D, 0, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_SURFACE_CONTINUTY" ), w ), 1, 0 );
      aLay->addWidget( mySplitContSurfCont, 1, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_CURVE_CONTINUTY" ), w ), 2, 0 );
      aLay->addWidget( mySplitContCurvCont, 2, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "BSplineRestriction" ) {
      // BSplineRestriction
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );

      myBSplineSurfModeChk = new QCheckBox( tr("GEOM_SURFACE_MODE"), w );
      myBSpline3DCurveChk = new QCheckBox( tr("GEOM_3D_CURVE_MODE"), w );
      myBSpline2DCurveChk = new QCheckBox( tr("GEOM_2D_CURVE_MODE"), w );

      myBSplineTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myBSplineTol3D, 0., 100., 1e-7, "len_tol_precision" );
      
      myBSplineTol2D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myBSplineTol2D, 0., 100., 1e-7, "param_tol_precision" );      

      myBSplineDegree = new SalomeApp_IntSpinBox( w );
      myBSplineSegments = new SalomeApp_IntSpinBox( w );
      initSpinBox( myBSplineSegments, 1, 1000, 1);      
      myBSpline2DCont = new QComboBox( w );
      myBSpline2DCont->addItems( aContinueties );
      myBSpline3DCont = new QComboBox( w );
      myBSpline3DCont->addItems( aContinueties );
      
      myBSplineSurfModeChk->setChecked( true );
      myBSpline3DCurveChk->setChecked( true );
      myBSpline2DCurveChk->setChecked( true );

      aLay->addWidget( myBSplineSurfModeChk, 0, 0 );
      aLay->addWidget( myBSpline3DCurveChk,  1, 0 );
      aLay->addWidget( myBSpline2DCurveChk,  2, 0 );
      aLay->addWidget( new QLabel( tr( "GEOM_3D_TOLERANCE" ), w ), 3, 0 );
      aLay->addWidget( myBSplineTol3D, 3, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_2D_TOLERANCE" ), w ), 4, 0 );
      aLay->addWidget( myBSplineTol2D, 4, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_REQUIRED_DEGREE" ), w ), 5, 0 );
      aLay->addWidget( myBSplineDegree, 5, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_REQUIRED_NUM_SEGMENTS" ), w ), 6, 0 );
      aLay->addWidget( myBSplineSegments, 6, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_3D_CONTINUTY" ), w ), 7, 0 );
      aLay->addWidget( myBSpline3DCont, 7, 1 );
      aLay->addWidget( new QLabel( tr( "GEOM_2D_CONTINUTY" ), w ), 8, 0 );
      aLay->addWidget( myBSpline2DCont, 8, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "ToBezier" ) {
      // ToBezier
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );

      myToBezierSurfModeChk = new QCheckBox( tr("GEOM_SURFACE_MODE"), w );
      myToBezier3DCurveChk = new QCheckBox( tr("GEOM_3D_CURVE_MODE"), w );
      myToBezier2DCurveChk = new QCheckBox( tr("GEOM_2D_CURVE_MODE"), w );
  
      myToBezierMaxTol = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( myToBezierMaxTol, 0., 100., 1e-7, "len_tol_precision" );            

      aLay->addWidget( myToBezierSurfModeChk, 0, 0 );
      aLay->addWidget( myToBezier3DCurveChk, 1, 0 );
      aLay->addWidget( myToBezier2DCurveChk, 2, 0 );
      aLay->addWidget( new QLabel( tr( "GEOM_MAX_TOLERANCE" ), w ), 3, 0 );
      aLay->addWidget( myToBezierMaxTol, 3, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else if ( myOpLst[i] == "SameParameter" ) {
      // SameParameter
      w = new QWidget( aParamsGr );
      QGridLayout* aLay = new QGridLayout( w );
      aLay->setMargin( 9 ); aLay->setSpacing( 6 );

      mySameParameterTol3D = new SalomeApp_DoubleSpinBox( w );
      initSpinBox( mySameParameterTol3D, 0., 100., 1e-7, "len_tol_precision" );            
      
      aLay->addWidget( new QLabel( tr( "GEOM_3D_TOLERANCE" ), w ), 0, 0 );
      aLay->addWidget( mySameParameterTol3D, 0, 1 );
      aLay->setRowStretch( aLay->rowCount(), 5 );
    }
    else {
      w = new QWidget( aParamsGr ); // dumb widget
    }
    myStack->insertWidget( i, w );
  }

  QGridLayout* layout = new QGridLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( mySelectWdgt, 0, 0, 1, 2 );
  layout->addWidget( anOperGr,     1, 0 );
  layout->addWidget( aParamsGr,    1, 1 );

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( onOk() ));
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( onApply() ));

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ))->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( selectionChanged() ));

  connect( mySelectWdgt->PushButton1, SIGNAL( clicked() ),       this, SLOT( selectClicked() ));
  connect( mySelectWdgt->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( lineEditReturnPressed() ));

  connect( myToBezierSurfModeChk,     SIGNAL( toggled( bool )), this, SLOT( advOptionToggled( bool )));
  connect( myDropSmallSolidsWidChk,   SIGNAL( toggled( bool )), this, SLOT( advOptionToggled( bool )));
  connect( myDropSmallSolidsVolChk,   SIGNAL( toggled( bool )), this, SLOT( advOptionToggled( bool )));

  connect( myOpList, SIGNAL( currentRowChanged( int )),      myStack, SLOT( setCurrentIndex( int )));
  connect( myOpList, SIGNAL( itemChanged( QListWidgetItem* )),  this, SLOT( operatorChecked( QListWidgetItem* )));
  connect( mySelectAll, SIGNAL( stateChanged( int ) ), this, SLOT( onSelectAll( int )));

  adjustSize();
  loadDefaults(); // init dialog fields with values from resource file
  //myOpList->setCurrentRow( myOpList->findItem( 0 );
  reset();

  myOpList->setCurrentRow(0);

  initName( tr( "PROCESS_SHAPE_NEW_OBJ_NAME" ));
  selectionChanged();
  updateSelectAll();
}

//=================================================================================
// function : onOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_ShapeProcessDlg::onOk()
{
  setIsApplyAndClose( true );
  if ( onApply() )
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
        
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int i = 0;
  myObjects->length(aSelList.Extent());
  for (SALOME_ListIteratorOfListIO anIt (aSelList); anIt.More(); anIt.Next()) {
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIt.Value() );
    if ( !CORBA::is_nil( aSelectedObject ))
      myObjects[i++] = aSelectedObject;
  }
  myObjects->length( i );
  if ( i == 1 )
    mySelectWdgt->LineEdit1->setText( GEOMBase::GetName( myObjects[0] ));
  else if ( i > 0 )
    mySelectWdgt->LineEdit1->setText( QString::number( i ) + "_" + tr( "GEOM_OBJECTS" ));
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
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication( )))->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( selectionChanged() ));
        
  reset();
  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_ShapeProcessDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupBoxName->isEnabled() )
    activate();
}


//=================================================================================
// function : reset()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void RepairGUI_ShapeProcessDlg::reset()
{
  myObjects = new GEOM::ListOfGO();
  myObjects->length( 0 );       
  mySelectWdgt->LineEdit1->setText( "" );
}


//=================================================================================
// function : get_convert
// purpose  : conversion of angle values to radians (non-angle values are not converted)
//=================================================================================
const char* get_convert( const char* theParam, const QString& theValue )
{
  if ( !strcmp( theParam, "SplitAngle.Angle" )) {
    double doubleValue = theValue.toDouble() * M_PI / 180.;
    return CORBA::string_dup( QString::number( doubleValue ).toLatin1().constData() );
  }
  return CORBA::string_dup( theValue.toLatin1().constData() );
}

//=================================================================================
// function : set_convert
// purpose  : conversion of angle values to degrees (non-angle values are not converted)
//=================================================================================
QString set_convert( const char* theParam, const char* theValue )
{
  if ( !strcmp( theParam, "SplitAngle.Angle" )) {
    Kernel_Utils::Localizer loc;
    double doubleValue = atof( theValue ) * 180. / M_PI;
    return QString::number( doubleValue );
  }
  return QString( theValue );
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
  for ( int i = 0; i < anOperators->length(); i++ ) {
    //MESSAGE("-->"<<(const char*)anOperators[i]);
    QList<QListWidgetItem*> items = myOpList->findItems ( (const char*)anOperators[i], Qt::MatchFixedString );
    if ( items.count() ) 
      ( items[0] )->setCheckState( Qt::Checked );
  }

  // Retrieve default parameters for ALL operators
  for ( int i = 0; i < myOpList->count(); i++ )
  {
    QString anOperator = myOpList->item( i )->text();
    anOp->GetOperatorParameters( anOperator.toLatin1().constData(), aParams, aValues );

    // set default values of parameters
    if ( aParams->length() != aValues->length() )
      continue;

    for ( int j = 0; j < aParams->length(); j++ ) {
      QWidget* aCtrl = getControl( (const char*)aParams[j] );
      setValue( aCtrl, set_convert( (const char*)aParams[j], aValues[j] ));
    }
  }
}

//=================================================================================
// function : setValue()
// purpose  : set value in the proper way
//=================================================================================
void RepairGUI_ShapeProcessDlg::setValue( QWidget* theControl, const QString& theValue )
{
  if ( theControl && !theValue.isNull() ) {
    if ( qobject_cast<SalomeApp_DoubleSpinBox*>( theControl ))
      qobject_cast<SalomeApp_DoubleSpinBox*>( theControl )->setValue( theValue.toDouble() );
    else if ( qobject_cast<SalomeApp_IntSpinBox*>( theControl ))
      qobject_cast<SalomeApp_IntSpinBox*>( theControl )->setValue( theValue.toInt() );
    else if ( qobject_cast<QComboBox*>( theControl ))
      qobject_cast<QComboBox*>( theControl )->setEditText( theValue );
    else if ( qobject_cast<QCheckBox*>( theControl ))
      qobject_cast<QCheckBox*>( theControl )->setChecked( theValue.toInt() != 0 );
  }
}

//=================================================================================
// function : getValue()
// purpose  : get value in the proper way
//=================================================================================
QString RepairGUI_ShapeProcessDlg::getValue( QWidget* theControl ) const
{
  if ( theControl ) {
    if ( qobject_cast<SalomeApp_DoubleSpinBox*>( theControl )) {
      if ( ( theControl == myDropSmallSolidsWidTol || theControl == myDropSmallSolidsVolTol ) && !theControl->isEnabled() ) {
        // VSR: stupid workaround about ShapeProcessAPI:
        // specific processing for optional parameters of DropSmallSolids operator
        return QString::number( Precision::Infinite() );
      }
      return QString::number( qobject_cast<SalomeApp_DoubleSpinBox*>( theControl )->value() );
    }
    else if ( qobject_cast<SalomeApp_IntSpinBox*>( theControl )) {
      return QString::number( qobject_cast<SalomeApp_IntSpinBox*>( theControl )->value() );
    }
    else if ( qobject_cast<QComboBox*>( theControl )) {
      return qobject_cast<QComboBox*>( theControl )->currentText();
    }
    else if ( qobject_cast<QCheckBox*>( theControl )) {
      return qobject_cast<QCheckBox*>( theControl )->isChecked() ? "1" : "0";
    }
  }   
  return 0;
}

//=================================================================================
// function : getText()
// purpose  : get text in the proper way
//=================================================================================
QString RepairGUI_ShapeProcessDlg::getText( QWidget* theControl ) const
{
  if ( theControl ) {
    if ( qobject_cast<SalomeApp_DoubleSpinBox*>( theControl ))
      return qobject_cast<SalomeApp_DoubleSpinBox*>( theControl )->text();
    else if ( qobject_cast<SalomeApp_IntSpinBox*>( theControl ))
      return qobject_cast<SalomeApp_IntSpinBox*>( theControl )->text();
  }   
  return QString::null;
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
  bool ok = true;
  QMapIterator<QString,QStringList> aMapIter( myValMap );
  while( aMapIter.hasNext() ) {
    aMapIter.next();
    const QStringList& aList = aMapIter.value();
    QListIterator<QString> aListIter( aList );
    while( aListIter.hasNext() ) {
      const QString& aParam = aListIter.next();
      QWidget* aControl = getControl( aParam );
      if ( !aControl->isEnabled() ) continue;
      if ( qobject_cast<SalomeApp_DoubleSpinBox*>( aControl ))
        ok = qobject_cast<SalomeApp_DoubleSpinBox*>( aControl )->isValid( msg, !IsPreview() ) && ok;
      else if ( qobject_cast<SalomeApp_IntSpinBox*>( aControl ))
        ok = qobject_cast<SalomeApp_IntSpinBox*>( aControl )->isValid( msg, !IsPreview() ) && ok;
    }
  }

  bool error = false;
  GEOM::string_array_var anOperators = getActiveOperators();
  if ( !myObjects->length() ) {
    msg += tr( "ERROR_NO_OBJECTS" );
    error = true;
  }
  if ( !anOperators->length() ) {
    if ( error ) 
      msg += "\n";
    msg += tr( "ERROR_NO_OPERATORS" );
    error = true;
  }
  return !error && ok;
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
  for ( int i = 0; i < myObjects->length(); i++ ) {
    GEOM::GEOM_Object_var obj = myObjects[i];
    GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow( getOperation() );
    GEOM::GEOM_Object_var anObj = anOper->ProcessShape( obj, anOperators, aParams, aValues );
    if ( anObj->_is_nil() )
      anErrorObjNames << GEOMBase::GetName( obj );
    else
    {
      if ( !IsPreview() )
      {
        QStringList aParameters;

        for ( int i = 0; i < anOperators->length(); i++ )
          aParameters << QString( anOperators[i] );

        for ( int i = 0; i < aParams->length(); i++ )
          aParameters << QString( aParams[i] );

        aParameters << getTexts( aParams );
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());

        RepairGUI::ShowStatistics( anOper, this );
      }
      objects.push_back( anObj._retn() );
    }
  }

  if ( !anErrorObjNames.empty() )
    MESSAGE( "ERRORS occured while processing the following objects: " << anErrorObjNames.join( " " ).toLatin1().data() );
    
  return anErrorObjNames.size() < myObjects->length(); // true if at least one object was OK, false if ALL objects were nil after Healing.
}

//=================================================================================
// function : getActiveOperators
// purpose  :
//=================================================================================
GEOM::string_array* RepairGUI_ShapeProcessDlg::getActiveOperators()
{
  GEOM::string_array_var anOperators = new GEOM::string_array();

  int j = 0, n = myOpList->count();
  anOperators->length( n );
  for ( int i = 0; i < n; i++ ) {
    if ( myOpList->item( i )->checkState() == Qt::Checked )
      anOperators[j++] = myOpList->item( i )->text().toLatin1().constData();
  }
  anOperators->length( j );

  return anOperators._retn();
}

//=================================================================================
// function : getcontrol
// purpose  :
//=================================================================================
QWidget* RepairGUI_ShapeProcessDlg::getControl( const QString& theParam )
{
  if ( theParam == "SplitAngle.Angle" )                           return mySplitAngleAngle;
  else if ( theParam == "SplitAngle.MaxTolerance" )               return mySplitAngleMaxTol;
  else if ( theParam == "SplitClosedFaces.NbSplitPoints" )        return mySplitClosedFacesNum;
  else if ( theParam == "FixFaceSize.Tolerance" )                 return myFixFaceSizeTol;
  else if ( theParam == "DropSmallEdges.Tolerance3d" )            return myDropSmallEdgesTol3D;
  else if ( theParam == "DropSmallSolids.WidthFactorThreshold" )  return myDropSmallSolidsWidTol;
  else if ( theParam == "DropSmallSolids.VolumeThreshold" )       return myDropSmallSolidsVolTol;
  else if ( theParam == "DropSmallSolids.MergeSolids" )           return myDropSmallSolidsMergeChk;
  else if ( theParam == "BSplineRestriction.SurfaceMode" )        return myBSplineSurfModeChk;
  else if ( theParam == "BSplineRestriction.Curve3dMode" )        return myBSpline3DCurveChk;
  else if ( theParam == "BSplineRestriction.Curve2dMode" )        return myBSpline2DCurveChk;
  else if ( theParam == "BSplineRestriction.Tolerance3d" )        return myBSplineTol3D;
  else if ( theParam == "BSplineRestriction.Tolerance2d" )        return myBSplineTol2D;
  else if ( theParam == "BSplineRestriction.RequiredDegree" )     return myBSplineDegree;
  else if ( theParam == "BSplineRestriction.RequiredNbSegments" ) return myBSplineSegments;
  else if ( theParam == "BSplineRestriction.Continuity3d" )       return myBSpline3DCont;
  else if ( theParam == "BSplineRestriction.Continuity2d" )       return myBSpline2DCont;
  else if ( theParam == "SplitContinuity.Tolerance3d" )           return mySplitContTol3D;
  else if ( theParam == "SplitContinuity.SurfaceContinuity" )     return mySplitContSurfCont;
  else if ( theParam == "SplitContinuity.CurveContinuity" )       return mySplitContCurvCont;
  else if ( theParam == "ToBezier.SurfaceMode" )                  return myToBezierSurfModeChk;
  else if ( theParam == "ToBezier.Curve3dMode" )                  return myToBezier3DCurveChk;
  else if ( theParam == "ToBezier.Curve2dMode" )                  return myToBezier2DCurveChk;
  else if ( theParam == "ToBezier.MaxTolerance" )                 return myToBezierMaxTol;
  else if ( theParam == "SameParameter.Tolerance3d" )             return mySameParameterTol3D;
  else if ( theParam == "FixShape.Tolerance3d" )                  return myFixShapeTol3D;
  else if ( theParam == "FixShape.MaxTolerance3d" )               return myFixShapeMaxTol3D;
  return 0;
}

//=================================================================================
// function : getParameters
// purpose  :
//=================================================================================
void RepairGUI_ShapeProcessDlg::initParamsValues()
{
  if ( myOpLst.count() )
    return;  // already filled

  myOpLst << "FixShape";
  myValMap["FixShape"] << "FixShape.Tolerance3d";
  myValMap["FixShape"] << "FixShape.MaxTolerance3d";

  myOpLst << "FixFaceSize";
  myValMap["FixFaceSize"] << "FixFaceSize.Tolerance";

  myOpLst << "DropSmallEdges";
  myValMap["DropSmallEdges"] << "DropSmallEdges.Tolerance3d";

  myOpLst << "DropSmallSolids";
  myValMap["DropSmallSolids"] << "DropSmallSolids.WidthFactorThreshold";
  myValMap["DropSmallSolids"] << "DropSmallSolids.VolumeThreshold";
  myValMap["DropSmallSolids"] << "DropSmallSolids.MergeSolids";

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
  int i = 0, j = 0;

  // calculate the length of parameters
  for ( i = 0, j = 0; i < theOperators.length(); i++ )
    j += myValMap[ QString( theOperators[i].in() ) ].size();
  
  // set the new length of paremeters
  aParams->length( j );

  // fill the parameters
  for ( i = 0, j = 0; i < theOperators.length(); i++ ) {
    QStringList aParamLst = myValMap[ QString( theOperators[i].in() ) ];
    foreach ( QString aParam, aParamLst ) {
      aParams[j++] = CORBA::string_dup( aParam.toLatin1().constData() );
    }
  }

  aParams->length( j );

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

  for ( int i = 0; i < theParams.length(); i++ ) {
    QWidget* aCtrl = getControl( (const char*)theParams[i] );
    if ( aCtrl )
      aValues[i] = get_convert( (const char*)theParams[i], getValue( aCtrl ));
  }

  return aValues._retn();
}

//=================================================================================
// function : getTexts
// purpose  :
//=================================================================================
QStringList RepairGUI_ShapeProcessDlg::getTexts( const GEOM::string_array& theParams )
{
  QStringList aTexts;
    
  for ( int i = 0; i < theParams.length(); i++ ) {
    QWidget* aCtrl = getControl( (const char*)theParams[i] );
    if ( aCtrl )
    {
      QString aText = getText( aCtrl );
      if( !aText.isNull() )
        aTexts.append( aText );
    }
  }
    
  return aTexts;
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
  QAbstractButton* btn = (QAbstractButton*)sender();
  if ( btn == myToBezierSurfModeChk )
  {
    if ( on && btn->isCheckable() &&
         SUIT_MessageBox::warning( this,
                                   tr( "GEOM_WRN_WARNING" ), tr( "TIME_CONSUMING" ),
                                   SUIT_MessageBox::Yes | SUIT_MessageBox::No ) == SUIT_MessageBox::No )
      btn->toggle();
  }

  // either myDropSmallSolidsWidChk or myDropSmallSolidsVolChk must be checked
  if ( btn == myDropSmallSolidsWidChk )
  {
    myDropSmallSolidsWidTol->setEnabled( on );
    if ( !on ) {
      myDropSmallSolidsVolChk->setChecked( true );
      myDropSmallSolidsVolTol->setEnabled( true );
    }
  }
  if ( btn == myDropSmallSolidsVolChk )
  {
    myDropSmallSolidsVolTol->setEnabled( on );
    if ( !on ) {
      myDropSmallSolidsWidChk->setChecked( true );
      myDropSmallSolidsWidTol->setEnabled( true );
    }
  }
}

//=======================================================================
//function : operatorChecked
//purpose  : show parameters of a selected operator
//=======================================================================

void RepairGUI_ShapeProcessDlg::operatorChecked( QListWidgetItem * item )
{
  if ( item && item->checkState() == Qt::Checked )
  {
    item->setSelected(true);
    myStack->setCurrentIndex( myOpList->row( item ));
  }
  updateSelectAll();
}

void RepairGUI_ShapeProcessDlg::updateSelectAll()
{
  Qt::CheckState state = myOpList->count() > 0 ? myOpList->item(0)->checkState() : Qt::Unchecked;
  for ( int i = 1; i < myOpList->count(); i++ ) {
    if ( myOpList->item(i)->checkState() != state ) {
      state = Qt::PartiallyChecked;
      break;
    }
  }
  mySelectAll->blockSignals( true );
  mySelectAll->setCheckState( state );
  mySelectAll->blockSignals( false );
}

void RepairGUI_ShapeProcessDlg::onSelectAll( int state )
{
  if ( state == Qt::PartiallyChecked ) { 
    mySelectAll->setCheckState( Qt::Checked );
    return;
  }
  myOpList->blockSignals( true );
  for ( int i = 0; i < myOpList->count(); i++ ) {
    myOpList->item(i)->setCheckState( (Qt::CheckState)state  );
  }
  myOpList->blockSignals( false );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_ShapeProcessDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::ListOfGO aListPtr(myObjects);
  for (int i = 0; i < aListPtr.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(aListPtr[i]);
    res << aGeomObjPtr;
  }
  return res;
}
