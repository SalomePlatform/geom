// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : MeasureGUI_ShapeStatisticsDlg.cxx
// Author : Alexander KOVALEV, OPEN CASCADE S.A.S.

// internal includes
#include "MeasureGUI_ShapeStatisticsDlg.h"

// GEOM includes
#include <GEOMBase.h>
#include <GEOMUtils_ShapeStatistics.hxx>
#include <GeometryGUI.h>
#include <DlgRef.h>

// GUI includes
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>

#include <LightApp_SelectionMgr.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include <Plot2d_Histogram.h>
#include <Plot2d_ViewFrame.h>
#include <Plot2d_ViewModel.h>
#include <Plot2d_ViewWindow.h>

// Qt includes
#include <QIcon>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

// Qtx includes
#include <QtxValidator.h>

// OCC includes
#include <TopoDS_Shape.hxx>

#include <GEOMImpl_Types.hxx>

//===========================================================================
// class    : MeasureGUI_ShapeStatisticsDlg()
//===========================================================================
MeasureGUI_ShapeStatisticsDlg::MeasureGUI_ShapeStatisticsDlg( QWidget* parent, TopoDS_Shape aShape, TopAbs_ShapeEnum aSubShapeType )
  : GEOMBase_Helper( SUIT_Session::session()->activeApplication()->desktop() ),
    QDialog( parent ),
    myHistogram ( 0 )
{
  myShapes.push_back( aShape );

  QIcon iconSelect( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SHAPE_STATISTICS" ) );
  setAttribute( Qt::WA_DeleteOnClose );

  myApp = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );

  QVBoxLayout* topLayout = new QVBoxLayout( this );

  QGridLayout* settingsLayout = new QGridLayout();

  /**********************   Selected Objects    **********************/

  QLabel* objsLabel = new QLabel( tr( "GEOM_SELECTED_OBJECTS" ), this );
  QPushButton* selBtn = new QPushButton( this );
  selBtn->setIcon( iconSelect );
  myEditMainShape = new QLineEdit( this );
  myEditMainShape->setReadOnly(true);

  settingsLayout->addWidget( objsLabel, 0, 0 );
  settingsLayout->addWidget( selBtn, 0, 1 );
  settingsLayout->addWidget( myEditMainShape, 0, 2 );

  if ( !aShape.IsNull() ) {
    objsLabel->hide();
    selBtn->hide();
    myEditMainShape->hide();
  }

  /**********************   Type    **********************/

  QLabel* typeLabel = new QLabel( tr( "GEOM_SHAPE_STATISTICS_TYPE" ), this );
  myCBTypes = new QtxComboBox( this );
  myCBTypes->setCleared( true );
  if ( aSubShapeType != TopAbs_SHAPE ) {
    fillTypes( aSubShapeType == TopAbs_EDGE, 
	       aSubShapeType == TopAbs_FACE, 
	       aSubShapeType == TopAbs_SOLID );
    myCBTypes->setEnabled( false );
  }

  settingsLayout->addWidget( typeLabel, 1, 0 );
  settingsLayout->addWidget( myCBTypes, 1, 2 );

  /**********************   Number of intervals    **********************/

  QLabel* nbIntervalsLabel = new QLabel( tr( "GEOM_SHAPE_STATISTICS_NB_INTERVALS" ), this );
  myNbIntervals = new QtxIntSpinBox( 1, 1000, 1, this );
  myNbIntervals->setValue( 10 );

  settingsLayout->addWidget( nbIntervalsLabel, 2, 0 );
  settingsLayout->addWidget( myNbIntervals, 2, 2 );

  /**********************   Scalar Range    **********************/

  myScalarRangeBox = new QGroupBox( tr( "GEOM_SHAPE_STATISTICS_SCALAR_RANGE" ), this );
  myScalarRangeBox->setCheckable( true );
  myScalarRangeBox->setChecked( false );
  QLabel* minLabel = new QLabel( tr( "GEOM_SHAPE_STATISTICS_MIN" ), this );
  myMin = new QLineEdit( this );
  QtxDoubleValidator* aValid = new QtxDoubleValidator( this );
  aValid->setBottom( 0.0 );
  myMin->setValidator( aValid );
  QLabel* maxLabel = new QLabel( tr( "GEOM_SHAPE_STATISTICS_MAX" ), this );
  myMax = new QLineEdit( this );
  myMax->setValidator( aValid );
  
  QPushButton* buttonCompute = new QPushButton( tr( "GEOM_SHAPE_STATISTICS_COMPUTE" ), this );
  connect( buttonCompute, SIGNAL( clicked() ), this, SLOT( clickOnCompute() ) );

  QGridLayout* scalarRangeLayout = new QGridLayout();
  scalarRangeLayout->setMargin( 11 ); settingsLayout->setSpacing( 6 );

  scalarRangeLayout->addWidget( minLabel, 0, 0 );
  scalarRangeLayout->addWidget( myMin,    0, 1 );
  scalarRangeLayout->addWidget( maxLabel, 1, 0 );
  scalarRangeLayout->addWidget( myMax,    1, 1 );
  scalarRangeLayout->addWidget( buttonCompute, 0, 2, 2, 1 );

  myScalarRangeBox->setLayout( scalarRangeLayout );

  /**********************   Buttons    **********************/

  myButtonPlot   = new QPushButton( tr( "GEOM_PLOT_DISTRIBUTION" ), this );
  myButtonPlot->setDefault( true );
  myButtonCreateGr = new QPushButton( tr( "GEOM_SHAPE_STATISTICS_CREATE_GROUPS" ), this );
  QPushButton* buttonClose  = new QPushButton( tr( "GEOM_BUT_CLOSE" ), this );
  QPushButton* buttonHelp   = new QPushButton( tr( "GEOM_BUT_HELP" ), this );

  QHBoxLayout* buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget( myButtonPlot );
  buttonsLayout->addWidget( myButtonCreateGr );
  buttonsLayout->addWidget( buttonClose );
  buttonsLayout->addWidget( buttonHelp );

  if ( !aShape.IsNull() ) {
    myButtonCreateGr->hide();
  }
  /**********************   Layouting    **********************/

  topLayout->addLayout( settingsLayout );
  topLayout->addWidget( myScalarRangeBox );
  topLayout->addLayout( buttonsLayout );

  // Signals and slots connections

  connect( selBtn, SIGNAL( clicked() ), this, SLOT( onEditMainShape() ) );

  connect( myButtonPlot,     SIGNAL( clicked() ), this, SLOT( clickOnPlot() ) );
  connect( myButtonCreateGr, SIGNAL( clicked() ), this, SLOT( clickOnCreateGroups() ) );

  connect( buttonClose,    SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( buttonHelp,     SIGNAL( clicked() ), this, SLOT( clickOnHelp() ) );

  connect(myApp->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(onEditMainShape()));

  if ( aShape.IsNull() )
    onEditMainShape();
}

//===========================================================================
// function : ~MeasureGUI_ShapeStatisticsDlg()
// purpose  : Destroys the object and frees any allocated resources
//===========================================================================
MeasureGUI_ShapeStatisticsDlg::~MeasureGUI_ShapeStatisticsDlg()
{
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_ShapeStatisticsDlg::createOperation()
{
  return getGeomEngine()->GetIGroupOperations(getStudyId());
}

#define RETURN_WITH_MSG(a, b) \
  if (!(a)) { \
    theMessage += (b); \
    return false; \
  }

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr MeasureGUI_ShapeStatisticsDlg::getFather(GEOM::GEOM_Object_ptr theObj)
{
  GEOM::GEOM_Object_var aFatherObj;
  if (theObj->GetType() == GEOM_GROUP) {
    GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());
    aFatherObj = anOper->GetMainShape(theObj);
  }
  return aFatherObj._retn();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> MeasureGUI_ShapeStatisticsDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myMainObj;
  return res;
}

//=================================================================================
// function : onEditMainShape()
// purpose  : called when selection button was clicked
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::onEditMainShape()
{
  // restore initial parameters for dialog box
  myEditMainShape->setText("");
  myEditMainShape->setFocus();

  //get shapes from selection
  QList<GEOM::GeomObjPtr> selShapes = getSelected( TopAbs_SHAPE, -1 );

  myButtonPlot->setEnabled( !selShapes.isEmpty() );
  myButtonCreateGr->setEnabled( selShapes.count() == 1 );

  if ( !selShapes.isEmpty() ) {
    if ( selShapes.count() == 1 )
      myMainObj = selShapes[0];
    QString aName = selShapes.count() > 1 ? QString( "%1_objects").arg( selShapes.count() ) : GEOMBase::GetName( myMainObj.get() );
    myEditMainShape->setText( aName );
  }

  updateTypes( selShapes );
}

//=================================================================================
// function : currentType()
// purpose  : returns currently selected type of shapes in 'Type' combobox
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::fillTypes( bool hasEdges, bool hasFaces, bool hasSolids )
{
  if ( hasEdges )
    myCBTypes->addItem( tr("GEOM_SHAPE_STATISTICS_LENGTH"), (int)TopAbs_EDGE );
  if ( hasFaces )
    myCBTypes->addItem( tr("GEOM_SHAPE_STATISTICS_AREA"), (int)TopAbs_FACE );
  if ( hasSolids )
    myCBTypes->addItem( tr("GEOM_SHAPE_STATISTICS_VOLUME"), (int)TopAbs_SOLID );

  myCBTypes->setEnabled( myCBTypes->count() > 0 );
}

//=================================================================================
// function : updateTypes()
// purpose  : update 'Type' combobox with available types
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::updateTypes( QList<GEOM::GeomObjPtr> theShapes )
{
  myCBTypes->clear();
  myCBTypes->setEnabled( false );

  int hasEdges = -1, hasFaces = -1, hasSolids = -1;

  myShapes.clear();
  // get types of the shapes and its sub-shapes
  foreach( GEOM::GeomObjPtr aShapePtr, theShapes ) {
    if ( !aShapePtr )
      return;

    TopoDS_Shape aShape;
    if ( !GEOMBase::GetShape( aShapePtr.get(), aShape ) || aShape.IsNull() )
      return;

    myShapes.push_back( aShape );

    GEOM::ListOfLong_var aSubShapes;
    GEOM::GEOM_IShapesOperations_var aShOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
    if ( hasEdges != 0 )
      hasEdges = aShOp->NumberOfSubShapes( aShapePtr.get(), TopAbs_EDGE ) > 0;
    if ( hasFaces != 0 )
      hasFaces = aShOp->NumberOfSubShapes( aShapePtr.get(), TopAbs_FACE ) > 0;
    if ( hasSolids != 0 )
      hasSolids = aShOp->NumberOfSubShapes( aShapePtr.get(), TopAbs_SOLID ) > 0;
  }
  fillTypes( hasEdges, hasFaces, hasSolids );
}

//=================================================================================
// function : currentType()
// purpose  : returns currently selected type of shapes in 'Type' combobox
//=================================================================================
TopAbs_ShapeEnum MeasureGUI_ShapeStatisticsDlg::currentType()
{
  return (TopAbs_ShapeEnum)( myCBTypes->itemData( myCBTypes->currentIndex() ).toInt() );
}

//=================================================================================
// function : clickOnPlot()
// purpose  : called when Plot button was clicked
//=================================================================================
bool MeasureGUI_ShapeStatisticsDlg::isValid(QString& theMessage)
{
  if ( myScalarRangeBox->isChecked() ) {
    RETURN_WITH_MSG( !myMin->text().isEmpty(), tr("GEOM_SHAPE_STATISTICS_MIN_ERROR") )
    RETURN_WITH_MSG( !myMax->text().isEmpty(), tr("GEOM_SHAPE_STATISTICS_MAX_ERROR") )
    RETURN_WITH_MSG( myMin->text().toDouble() <= myMax->text().toDouble(), tr("GEOM_SHAPE_STATISTICS_MIN_MAX_ERROR") )
  }
  return true;
}
//=================================================================================
// function : clickOnPlot()
// purpose  : called when Plot button was clicked
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::clickOnPlot()
{
  GEOMUtils::Range aRange;
  if ( myScalarRangeBox->isChecked() ) {
    QString msg;
    if ( !isValid( msg ) ) {
      showError( msg );
      return;
    }
    aRange.min = myMin->text().toDouble();
    aRange.max = myMax->text().toDouble();
  } else {
    aRange.min = -1.0; // flag that range is empty
    aRange.max = -1.0; // flag that range is empty
  }

  GEOMUtils::Distribution aShapesDistr = 
    GEOMUtils::ComputeDistribution( myShapes, currentType(), myNbIntervals->value(), aRange );

  QList<double> xVals, yVals;
  double width = -1, min = -1;
  double xmin = 1e+32, xmax = 0.0, ymax = 0.0;
  int i=0;
  GEOMUtils::Distribution::const_iterator it;
  for (it = aShapesDistr.begin(); it != aShapesDistr.end(); it++) {
    GEOMUtils::Range ran = *it;
    if ( width < 0 ) width = ran.max - ran.min; // bar width
    if ( min < 0 ) min = ran.min; // global min
    xVals << width / 2. + i*width + min; // get a middle of bar
    yVals << ran.count;
    // get global boundary max values
    if ( ran.min < xmin ) xmin = ran.min;
    if ( ran.max > xmax ) xmax = ran.max;
    if ( ran.count > ymax ) ymax = ran.count;
    i++;
  }

  // plot the computed distribution
  SUIT_ViewManager* aViewManager = myApp->getViewManager( Plot2d_Viewer::Type(), true ); // create if necessary
  if( !aViewManager )
    return;
  Plot2d_ViewWindow* aViewWnd = dynamic_cast<Plot2d_ViewWindow*>(aViewManager->getActiveView());
  if( !aViewWnd )
    return;
  Plot2d_ViewFrame* aPlot = aViewWnd->getViewFrame();
  if ( !aPlot )
    return;

  aPlot->EraseAll();

  // create or reuse histogram
  if( !myHistogram )
    myHistogram = new Plot2d_Histogram();
  else
    myHistogram->clearAllPoints();
  // set histogram parameters
  myHistogram->setData( xVals, yVals );
  if ( width != 0.0 )
    myHistogram->setWidth( width );
  myHistogram->setAutoAssign(true);
  myHistogram->setName( myEditMainShape->text() );
  myHistogram->setHorTitle( myCBTypes->currentText() );
  myHistogram->setVerTitle( tr("GEOM_SHAPE_STATISTICS_DISTRIBUTION_NB_ENT") );
  myHistogram->setColor( QColor(0, 85, 0) );
  // display histogram
  aPlot->displayObject( myHistogram, true );
  if ( width == 0.0 ) // only one X value
    aPlot->fitAll();
  else
    aPlot->fitData( 0, xmin, xmax, 0.0, ymax );
}

//=================================================================================
// function : clickOnCompute()
// purpose  : called when Compute button was clicked
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::clickOnCompute()
{
  GEOMUtils::Range aRange;
  aRange.min = -1.0; // flag that range is empty
  aRange.max = -1.0; // flag that range is empty
  std::map<int,double> measures = GEOMUtils::ComputeMeasures( myShapes, currentType(), aRange );
  if ( measures.size() != 0 ) {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
    myMin->setText( DlgRef::PrintDoubleValue( aRange.min, aPrecision ) );
    myMax->setText( DlgRef::PrintDoubleValue( aRange.max, aPrecision ) );
  }
}

//=================================================================================
// function : clickOnCreateGroups()
// purpose  : called when Create Groups button was clicked
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::clickOnCreateGroups()
{
  onAccept(false, false, false);
}

//=================================================================================
// function : execute(ObjectList& objects)
// purpose  : 
//=================================================================================
bool MeasureGUI_ShapeStatisticsDlg::execute(ObjectList& objects)
{
  if ( myMainObj.isNull() )
    return false;

  GEOM::GroupOpPtr anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());

  GEOMUtils::Range aRange;
  if ( myScalarRangeBox->isChecked() ) {
    QString msg;
    if ( !isValid( msg ) ) {
      showError( msg );
      return false;
    }
    aRange.min = myMin->text().toDouble();
    aRange.max = myMax->text().toDouble();
  } else {
    aRange.min = -1.0; // flag that range is empty
    aRange.max = -1.0; // flag that range is empty
  }

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );
  QString aTypePrefix = myCBTypes->currentText().replace(' ', '_');
  QString objIOR, aMin, aMax, aGroupName;
  SalomeApp_Study* study = getStudy();

  GEOMUtils::Distribution aShapesDistr = 
    GEOMUtils::ComputeDistribution( myShapes, currentType(), myNbIntervals->value(), aRange );

  int nbGroups = 0;

  GEOMUtils::Distribution::const_iterator it;
  for (it = aShapesDistr.begin(); it != aShapesDistr.end(); it++) {
    std::list<long> idList = (*it).indices;
    int nn = idList.size();
    if ( nn > 0 ) {
      GEOM::ListOfLong_var aNewList = new GEOM::ListOfLong;
      aNewList->length(nn);
      int ii = 0;
      std::list<long>::const_iterator id_it;
      for ( id_it = idList.begin(); id_it != idList.end(); id_it++ ) {
	aNewList[ii++] = *id_it;
      }

      // Create an empty group
      GEOM::GEOM_Object_var aGroup;
      aGroup = anOper->CreateGroup( myMainObj.get(), currentType() );

      if (CORBA::is_nil(aGroup) || !anOper->IsDone())
	return false;

      // Add sub-shapes into group
      anOper->UnionIDs(aGroup, aNewList);
      if (!anOper->IsDone())
	return false;

      // publish group
      aMin = DlgRef::PrintDoubleValue( (*it).min, aPrecision );
      aMax = DlgRef::PrintDoubleValue( (*it).max, aPrecision );
      aGroupName =  aTypePrefix + "_" + aMin + "_" + aMax;
      GEOMBase::PublishSubObject( aGroup, aGroupName );

      // this is needed just to avoid error message
      objects.push_back(aGroup._retn());

      nbGroups++;
    }
  }

  SUIT_MessageBox::information( this, tr( "INF_INFO" ), tr( "GEOM_MSG_GROUPS_CREATED" ).arg( nbGroups ) );

  return true;
}

//=================================================================================
// function : clickOnHelp()
// purpose  : called when Help button was clicked
//=================================================================================
void MeasureGUI_ShapeStatisticsDlg::clickOnHelp()
{
  GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( myApp->module( "Geometry" ) );
  myApp->onHelpContextModule( aGeomGUI ? myApp->moduleName( aGeomGUI->moduleName() ) : QString(""), "shape_statistics_operation_page.html" );
}
