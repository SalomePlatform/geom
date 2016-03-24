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
// File   : MeasureGUI_CreateDimensionDlg.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_CreateDimensionDlg.h"
#include "MeasureGUI_DimensionCreateTool.h"

#include <GEOM_Constants.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <DlgRef.h>
#include <GEOMUtils.hxx>
#include <GEOMGUI_DimensionProperty.h>

#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SOCC_ViewModel.h>
#include <SOCC_Prs.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>

#include <Aspect_PolygonOffsetMode.hxx>

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
MeasureGUI_CreateDimensionDlg::MeasureGUI_CreateDimensionDlg( const GEOM::GeomObjPtr& theObj,
                                                              GeometryGUI* theGUI,
                                                              QWidget* theParent )
: GEOMBase_Skeleton( theGUI, theParent ),
  myParentObj( theObj ),
  myEditingViewer( NULL )
{
  setWindowTitle( tr( "CREATE_DIMENSION_TITLE" ) );

  // init "dimension type" radio buttons
  QAbstractButton* aTypeButtonLength   = mainFrame()->RadioButton1;
  QAbstractButton* aTypeButtonDiameter = mainFrame()->RadioButton2;
  QAbstractButton* aTypeButtonAngle    = mainFrame()->RadioButton3;

  mainFrame()->GroupConstructors->setTitle( tr( "DIMENSIONS" ) );
  aTypeButtonDiameter->setText( tr( "DIAMETER" ) );
  aTypeButtonLength->setText( tr( "LENGTH" ) );
  aTypeButtonAngle->setText( tr( "ANGLE" ) );

  // construct "arguments" pane
  QGroupBox*   aGroupArgs       = new QGroupBox( tr( "ARGUMENTS" ), centralWidget() );
  QVBoxLayout* aGroupArgsLayout = new QVBoxLayout( aGroupArgs );

  myLengthArgs   = new LengthPane  ( centralWidget() );
  myDiameterArgs = new DiameterPane( centralWidget() );
  myAngleArgs    = new AnglePane   ( centralWidget() );

  // connect selector pane signals
  connect( myLengthArgs,   SIGNAL( StartSelection( const QList<TopAbs_ShapeEnum>& ) ),
                           SLOT( OnStartSelection( const QList<TopAbs_ShapeEnum>& ) ) );
  connect( myDiameterArgs, SIGNAL( StartSelection( const QList<TopAbs_ShapeEnum>& ) ),
                           SLOT( OnStartSelection( const QList<TopAbs_ShapeEnum>& ) ) );
  connect( myAngleArgs,    SIGNAL( StartSelection( const QList<TopAbs_ShapeEnum>& ) ), 
                           SLOT( OnStartSelection( const QList<TopAbs_ShapeEnum>& ) ) );

  connect( myLengthArgs,   SIGNAL( StopSelection() ), SLOT( OnStopSelection() ) );
  connect( myDiameterArgs, SIGNAL( StopSelection() ), SLOT( OnStopSelection() ) );
  connect( myAngleArgs,    SIGNAL( StopSelection() ), SLOT( OnStopSelection( ) ) );

  connect( myLengthArgs,   SIGNAL( SelectionDone() ), SLOT( OnSelectionDone() ) );
  connect( myDiameterArgs, SIGNAL( SelectionDone() ), SLOT( OnSelectionDone() ) );
  connect( myAngleArgs,    SIGNAL( SelectionDone() ), SLOT( OnSelectionDone() ) );

  aGroupArgsLayout->addWidget( myLengthArgs );
  aGroupArgsLayout->addWidget( myDiameterArgs );
  aGroupArgsLayout->addWidget( myAngleArgs );

  // construct main layout
  QVBoxLayout* aCustomWidLayout = new QVBoxLayout( centralWidget() );
  aCustomWidLayout->setMargin( 0 ); 
  aCustomWidLayout->setSpacing( 6 );
  aCustomWidLayout->addWidget( aGroupArgs );

  // add visibility rules for structural widgets
  connect( aTypeButtonLength,   SIGNAL( toggled(bool) ), myLengthArgs,   SLOT( setVisible(bool) ) );
  connect( aTypeButtonDiameter, SIGNAL( toggled(bool) ), myDiameterArgs, SLOT( setVisible(bool) ) );
  connect( aTypeButtonAngle,    SIGNAL( toggled(bool) ), myAngleArgs,    SLOT( setVisible(bool) ) );

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  // map functional signals
  myRBGroup->setId( aTypeButtonLength,   TypeButtonID_Length );
  myRBGroup->setId( aTypeButtonDiameter, TypeButtonID_Diameter );
  myRBGroup->setId( aTypeButtonAngle,    TypeButtonID_Angle );

  connect( this, SIGNAL( constructorsClicked( int ) ), SLOT( ConstructTypeChanged( int ) ) );
  connect( myLengthArgs,   SIGNAL( TabChanged() ), SLOT( OnArgumentTabChanged() ) );
  connect( myDiameterArgs, SIGNAL( TabChanged() ), SLOT( OnArgumentTabChanged() ) );
  connect( myAngleArgs,    SIGNAL( TabChanged() ), SLOT( OnArgumentTabChanged() ) );

  myDimensionInteractor = new MeasureGUI_DimensionInteractor( theGUI, theParent );

  myDiameterArgs->setVisible( false );
  myAngleArgs   ->setVisible( false );
  myLengthArgs  ->setVisible( true );

  myRBGroup->button( TypeButtonID_Length )->click();

  Init();

  setHelpFileName("add_dimension_page.html");
}

//=================================================================================
// function : Destructor
// purpose  :
//=================================================================================
MeasureGUI_CreateDimensionDlg::~MeasureGUI_CreateDimensionDlg()
{
}

//=================================================================================
// function : ActiveArgs
// purpose  : 
//=================================================================================
MeasureGUI_CreateDimensionDlg::BaseSelectorPane* MeasureGUI_CreateDimensionDlg::ActiveArgs()
{
  switch ( getConstructorId() )
  {
    case TypeButtonID_Length   : return myLengthArgs;
    case TypeButtonID_Diameter : return myDiameterArgs;
    case TypeButtonID_Angle    : return myAngleArgs;
    default:
      return NULL;
  }
}

//=================================================================================
// function : GenerateName
// purpose  : 
//=================================================================================
QString MeasureGUI_CreateDimensionDlg::GenerateName( const QString& thePrefix )
{
  QRegExp anExtractIds( "^" + thePrefix + "_([0-9]+)$" );

  QSet<int> anOccupiedIds;

  GEOMGUI_DimensionProperty aProp =
    getStudy()->getObjectProperty( GEOM::sharedPropertiesId(),
                                   myParentObj->GetStudyEntry(),
                                   GEOM::propertyName( GEOM::Dimensions ),
                                   QVariant() )
                                   .value<GEOMGUI_DimensionProperty>();

  // get names and convert to QStrings to perform index matching
  for( int anIt = 0; anIt < aProp.GetNumber(); ++anIt )
  {
    if ( anExtractIds.indexIn( aProp.GetName( anIt ) ) == -1 )
    {
      continue;
    }

    anOccupiedIds.insert( anExtractIds.cap( 1 ).toInt() );
  }

  int aSearchId = 1;
  while ( true )
  {
    if ( !anOccupiedIds.contains( aSearchId ) )
    {
      break;
    }

    aSearchId++;
  }

  return QString( thePrefix + "_%1" ).arg( aSearchId );
}

//=================================================================================
// function : ConstructTypeChanged
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::ConstructTypeChanged( int theType )
{
  ActiveArgs()->Reset();

  updateGeometry();

  QString aName;
  switch ( theType )
  {
    case TypeButtonID_Length   : aName = GenerateName( tr("NAME_LENGTH") );   break;
    case TypeButtonID_Diameter : aName = GenerateName( tr("NAME_DIAMETER") ); break;
    case TypeButtonID_Angle    : aName = GenerateName( tr("NAME_ANGLE") );    break;
  }

  myMainFrame->ResultName->setText( aName );

  StopLocalEditing();
  erasePreview();

  updateGeometry();
  resize( minimumSizeHint() );
}

//=================================================================================
// function : OnArgumentTabChanged
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::OnArgumentTabChanged()
{
  StopLocalEditing();
  erasePreview();
}

//=================================================================================
// function : OnStartSelection
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::OnStartSelection( const QList<TopAbs_ShapeEnum>& theModes )
{
  OnStopSelection();

  StopLocalEditing();

  QList<TopAbs_ShapeEnum>::const_iterator aModeIt = theModes.constBegin();
  for ( ; aModeIt != theModes.constEnd(); ++aModeIt )
  {
    localSelection( myParentObj.get(), *aModeIt );
  }

  mySelectionModes = theModes;

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ),
           SLOT( SelectionIntoArgument() ) );
}

//=================================================================================
// function : OnStopSelection
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::OnStopSelection()
{
  globalSelection();

  disconnect( myGeomGUI->getApp()->selectionMgr(),
              SIGNAL( currentSelectionChanged() ),
              this,
              SLOT( SelectionIntoArgument() ) );
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : Handle model selection by active selector pane
//=================================================================================
void MeasureGUI_CreateDimensionDlg::SelectionIntoArgument()
{
  LightApp_SelectionMgr* aSelectionMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelection;
  aSelectionMgr->selectedObjects( aSelection );

  if ( aSelection.Extent() != 1 )
  {
    ActiveArgs()->SelectionIntoArguments( GEOM::GeomObjPtr() );
    return;
  }

  GEOM::GeomObjPtr aSelected = getSelected( mySelectionModes );
  if ( aSelected.isNull() )
  {
    ActiveArgs()->SelectionIntoArguments( GEOM::GeomObjPtr() );
    return;
  }

  GEOM::GeomObjPtr aSelectedMain = 
    !aSelected->IsMainShape() 
      ? aSelected->GetMainShape() 
      : GEOM::GeomObjPtr();

  if ( myParentObj != aSelected && myParentObj != aSelectedMain )
  {
    ActiveArgs()->SelectionIntoArguments( GEOM::GeomObjPtr() );
    return;
  }

  StopLocalEditing();

  erasePreview();

  ActiveArgs()->SelectionIntoArguments( aSelected );
}

//=================================================================================
// function : OnSelectionDone
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::OnSelectionDone()
{
  // create new dimension presentation
  myDimension = CreateDimension();

  if ( myDimension.IsNull() )
  {
    // can not create preview, repeat selection
    SUIT_MessageBox::warning( this,
                              tr( "WARNING_TITLE_CANNOT_CREATE_DIMENSION" ),
                              tr( "WARNING_MSG_INVALID_ARGUMENTS" ) );

    ActiveArgs()->Reset();

    return;
  }

  // show preview of dimension object
  SUIT_ViewWindow* aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  SOCC_Prs* aSalomePrs = dynamic_cast<SOCC_Prs*>( ( (SOCC_Viewer*)( aViewWindow->getViewManager()->getViewModel() ) )->CreatePrs( 0 ) );

  aSalomePrs->AddObject( myDimension );

  displayPreview( aSalomePrs );

  StartLocalEditing();
}

//=================================================================================
// function : ClickOnOk
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
  {
    ClickOnCancel();
  }
}

//=================================================================================
// function : ClickOnApply
// purpose  : 
//=================================================================================
bool MeasureGUI_CreateDimensionDlg::ClickOnApply()
{
  StopLocalEditing();

  if ( myDimension.IsNull() )
  {
    return true;
  }

  if ( !AddDimensionToOwner() )
  {
    return false;
  }

  redisplay( myParentObj.get() );

  if ( !isApplyAndClose() )
  {
    emit applyClicked();
    Init();
    ConstructTypeChanged( getConstructorId() );
  }

  return true;
}

//=================================================================================
// function : StartLocalEditing
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::StartLocalEditing()
{
  StopLocalEditing();

  myDimensionInteractor->Enable();

  if ( myDimension.IsNull() )
  {
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();

  SUIT_ViewManager* aViewMgr = anApp->activeViewManager();

  if ( aViewMgr->getType() != OCCViewer_Viewer::Type() )
  {
    return;
  }

  SOCC_Viewer* anOCCViewer = dynamic_cast<SOCC_Viewer*>( aViewMgr->getViewModel() );

  myEditingViewer = anOCCViewer;
  if ( !myEditingViewer )
  {
    return;
  }

  Handle(AIS_InteractiveContext) anAISContext = myEditingViewer->getAISContext();
  Handle(V3d_Viewer)             aViewer3d    = myEditingViewer->getViewer3d();

  aViewer3d->AddZLayer( myEditingLayer );

  anAISContext->OpenLocalContext( Standard_False, Standard_False );
  anAISContext->Load( myDimension, AIS_DSM_All );
  anAISContext->SetZLayer( myDimension, myEditingLayer );
  anAISContext->Activate( myDimension, AIS_DSM_Line );
  anAISContext->Activate( myDimension, AIS_DSM_Text );
  anAISContext->Redisplay( myDimension );
}

//=================================================================================
// function : StopLocalEditing
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::StopLocalEditing()
{
  myDimensionInteractor->Disable();

  if ( !myEditingViewer )
  {
    return;
  }

  Handle(AIS_InteractiveContext) anAISContext = myEditingViewer->getAISContext();
  Handle(V3d_Viewer)             aViewer3d    = myEditingViewer->getViewer3d();

  aViewer3d->RemoveZLayer( myEditingLayer );
  anAISContext->CloseLocalContext();

  myEditingViewer = NULL;
}

//=================================================================================
// function : Init
// purpose  : 
//=================================================================================
void MeasureGUI_CreateDimensionDlg::Init()
{
  myDimension = NULL;

  StopLocalEditing();

  erasePreview();

  myDiameterArgs->Reset();
  myLengthArgs->Reset();
  myAngleArgs->Reset();
}

//=================================================================================
// function : CreateDimensionPrs
// purpose  : 
//=================================================================================
Handle(AIS_Dimension) MeasureGUI_CreateDimensionDlg::CreateDimension()
{
  Handle(AIS_Dimension) aDimensionIO;

  // prepare dimension styling
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  QColor  aQColor       = aResMgr->colorValue  ( "Geometry", "dimensions_color", QColor( 0, 255, 0 ) );
  int     aLineWidth    = aResMgr->integerValue( "Geometry", "dimensions_line_width", 1 );
  QFont   aFont         = aResMgr->fontValue   ( "Geometry", "dimensions_font", QFont("Y14.5M-2009", 14) );
  double  anArrowLength = aResMgr->doubleValue ( "Geometry", "dimensions_arrow_length", 5 );
  double  aDefFlyout    = aResMgr->doubleValue ( "Geometry", "dimensions_default_flyout", 20 );
  bool    isUnitsShown  = aResMgr->booleanValue( "Geometry", "dimensions_show_units", false );
  QString aUnitsLength  = aResMgr->stringValue ( "Geometry", "dimensions_length_units", "m" );
  QString aUnitsAngle   = aResMgr->stringValue ( "Geometry", "dimensions_angle_units", "deg" );
  bool    aUseText3d    = aResMgr->booleanValue( "Geometry", "dimensions_use_text3d", false );

  OCCViewer_ViewWindow* anActiveView = NULL;

  SalomeApp_Application* anApp = myGeomGUI->getApp();

  if ( anApp )
  {
    OCCViewer_ViewManager* aViewMgr = (OCCViewer_ViewManager*) anApp->getViewManager( OCCViewer_Viewer::Type(), false );
    if ( aViewMgr )
    {
      anActiveView = (OCCViewer_ViewWindow*) aViewMgr->getActiveView();
    }
  }

  MeasureGUI_DimensionCreateTool aTool;

  aTool.Settings.DefaultFlyout = aDefFlyout;
  aTool.Settings.ActiveView    = anActiveView ? anActiveView->getViewPort()->getView() : NULL;

  switch ( getConstructorId() )
  {
    // create length dimension
    case TypeButtonID_Length :
    {
      LengthPane* aLengthPane = qobject_cast<LengthPane*>( ActiveArgs() );

      switch (aLengthPane->ActiveTab())
      {
        case LengthPane::TabID_SingleEdge:
        {
          const GEOM::GeomObjPtr& anEdge = aLengthPane->GetSingleEdge();

          aDimensionIO = aTool.LengthOnEdge( anEdge );
          break;
        }

        case LengthPane::TabID_TwoPoints:
        {
          const GEOM::GeomObjPtr& aPoint1 = aLengthPane->GetPoint1();
          const GEOM::GeomObjPtr& aPoint2 = aLengthPane->GetPoint2();

          aDimensionIO = aTool.LengthByPoints( aPoint1, aPoint2 );
          break;
        }

        case LengthPane::TabID_ParallelEdges:
        {
          const GEOM::GeomObjPtr& anEdge1 = aLengthPane->GetEdge1();
          const GEOM::GeomObjPtr& anEdge2 = aLengthPane->GetEdge2();

          aDimensionIO = aTool.LengthByParallelEdges( anEdge1, anEdge2 );
          break;
        }
      }
    }
    break;

    // create diameter dimension
    case TypeButtonID_Diameter :
    {
      DiameterPane* aDiameterPane = qobject_cast<DiameterPane*>( ActiveArgs() );

      const GEOM::GeomObjPtr& aShape = aDiameterPane->GetShape();

      aDimensionIO = aTool.Diameter( aShape );
      break;
    }

    // create angle dimension
    case TypeButtonID_Angle :
    {
      AnglePane* anAnglePane = qobject_cast<AnglePane*>( ActiveArgs() );

      switch (ActiveArgs()->ActiveTab())
      {
        case AnglePane::TabID_TwoEdges:
        {
          const GEOM::GeomObjPtr& anEdge1 = anAnglePane->GetEdge1();
          const GEOM::GeomObjPtr& anEdge2 = anAnglePane->GetEdge2();

          aDimensionIO = aTool.AngleByTwoEdges( anEdge1, anEdge2 );
          break;
        }

        case AnglePane::TabID_ThreePoints:
        {
          const GEOM::GeomObjPtr& aPoint1 = anAnglePane->GetPoint1();
          const GEOM::GeomObjPtr& aPoint2 = anAnglePane->GetPoint2();
          const GEOM::GeomObjPtr& aPoint3 = anAnglePane->GetPoint3();

          aDimensionIO = aTool.AngleByThreePoints( aPoint1, aPoint2, aPoint3 );
          break;
        }
      }
      break;
    }
  }

  if ( aDimensionIO.IsNull() )
  {
    return NULL;
  }

  Quantity_Color aColor( aQColor.redF(), aQColor.greenF(), aQColor.blueF(), Quantity_TOC_RGB );

  Handle(Prs3d_DimensionAspect) aStyle = new Prs3d_DimensionAspect();

  aStyle->SetCommonColor( aColor );
  aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
  aStyle->MakeText3d( aUseText3d );
  aStyle->MakeTextShaded( Standard_True );
  int fsize = aFont.pixelSize() != -1 ? aFont.pixelSize() : aFont.pointSize();
  aStyle->SetExtensionSize( fsize * 0.5 );
  aStyle->TextAspect()->SetFont( aFont.family().toLatin1().data() );
  aStyle->TextAspect()->SetHeight( fsize );
  aStyle->ArrowAspect()->SetLength( anArrowLength );
  aStyle->LineAspect()->SetWidth( aLineWidth );

  if ( aDimensionIO->IsKind( STANDARD_TYPE(AIS_AngleDimension) ) )
  {
    // show degree symbol for dimension instead of label "deg"
    if ( aUnitsAngle == "deg" )
    {
      aDimensionIO->SetSpecialSymbol(0xB0);
      aDimensionIO->SetDisplaySpecialSymbol( isUnitsShown ? AIS_DSS_After : AIS_DSS_No );
      aStyle->MakeUnitsDisplayed(Standard_False);
    }
    else
    {
      aDimensionIO->SetDisplaySpecialSymbol(AIS_DSS_No);
      aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
    }
  }
  else
  {
    aStyle->MakeUnitsDisplayed( (Standard_Boolean) isUnitsShown );
  }

  aDimensionIO->Attributes()->SetDimLengthDisplayUnits( aUnitsLength.toLatin1().data() );
  aDimensionIO->Attributes()->SetDimAngleDisplayUnits( aUnitsAngle.toLatin1().data() );
  aDimensionIO->SetDimensionAspect( aStyle );
  aDimensionIO->SetPolygonOffsets( Aspect_POM_Fill, -1.0, -1.0 );

  return aDimensionIO;
}

//=================================================================================
// class    : AddDimensionToOwner
// purpose  : 
//=================================================================================
bool MeasureGUI_CreateDimensionDlg::AddDimensionToOwner()
{
  if ( myDimension.IsNull() )
  {
    return false;
  }

  gp_Ax3 aLCS;
  TopoDS_Shape anParentSh;
  if ( GEOMBase::GetShape( myParentObj.get(), anParentSh ) )
  {
    aLCS = gp_Ax3().Transformed( anParentSh.Location().Transformation() );
  }

  QString aName = getNewObjectName();

  SalomeApp_Study* aStudy = getStudy();

  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myParentObj->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  // append new dimension record to data
  aProp.AddRecord( myDimension, aLCS );
  aProp.SetName( aProp.GetNumber() - 1, aName );
  aProp.SetVisible( aProp.GetNumber() - 1, true );

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myParentObj->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );

  return true;
}

/* ------------------------------------------------------------------------------*
 *                                                                               *
 *                    Argument shape selector layout                             *
 *                                                                               *
 * ------------------------------------------------------------------------------*/

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::LengthPane
// purpose  : Constructor
//=================================================================================
MeasureGUI_CreateDimensionDlg::LengthPane::LengthPane( QWidget* theParent )
: BaseSelectorPane( theParent )
{
  myTabs                   = new QTabWidget( this );
  mySingleEdgeSelectors    = new MeasureGUI_1Sel_Frame( this );
  myTwoPointsSelectors     = new MeasureGUI_2Sel_Frame( this );
  myParallelEdgesSelectors = new MeasureGUI_2Sel_Frame( this );

  myTabs->addTab( mySingleEdgeSelectors,    MeasureGUI_CreateDimensionDlg::tr( "EDGE_LENGTH" ) );
  myTabs->addTab( myTwoPointsSelectors,     MeasureGUI_CreateDimensionDlg::tr( "TWO_POINTS" ) );
  myTabs->addTab( myParallelEdgesSelectors, MeasureGUI_CreateDimensionDlg::tr( "PARALLEL_EDGES" ) );

  mySingleEdgeSelectors->PushButton1->setIcon( mySelectorIcon );
  mySingleEdgeSelectors->TextLabel1->setText( MeasureGUI_CreateDimensionDlg::tr( "EDGE" ) );
  mySingleEdgeSelectors->LineEdit1->setReadOnly( true );

  myTwoPointsSelectors->PushButton1->setIcon( mySelectorIcon );
  myTwoPointsSelectors->PushButton2->setIcon( mySelectorIcon );
  myTwoPointsSelectors->TextLabel1->setText( MeasureGUI_CreateDimensionDlg::tr( "POINT_1" ) );
  myTwoPointsSelectors->TextLabel2->setText( MeasureGUI_CreateDimensionDlg::tr( "POINT_2" ) );
  myTwoPointsSelectors->LineEdit1->setReadOnly( true );
  myTwoPointsSelectors->LineEdit2->setReadOnly( true );

  myParallelEdgesSelectors->PushButton1->setIcon( mySelectorIcon );
  myParallelEdgesSelectors->PushButton2->setIcon( mySelectorIcon );
  myParallelEdgesSelectors->TextLabel1 ->setText( MeasureGUI_CreateDimensionDlg::tr( "EDGE_1" ) );
  myParallelEdgesSelectors->TextLabel2 ->setText( MeasureGUI_CreateDimensionDlg::tr( "EDGE_2" ) );
  myParallelEdgesSelectors->LineEdit1->setReadOnly( true );
  myParallelEdgesSelectors->LineEdit2->setReadOnly( true );

  QVBoxLayout* aLayout = new QVBoxLayout( this );

  aLayout->setMargin( 0 );
  aLayout->addWidget( myTabs );

  SelectionModes anEdgeModes;
  SelectionModes aPointModes;
  anEdgeModes << TopAbs_EDGE;
  aPointModes << TopAbs_VERTEX;

  SetTabWidget( myTabs );
  RegisterSelector( mySingleEdgeSelectors->LineEdit1,    mySingleEdgeSelectors->PushButton1,    anEdgeModes, TabID_SingleEdge );
  RegisterSelector( myTwoPointsSelectors->LineEdit1,     myTwoPointsSelectors->PushButton1,     aPointModes, TabID_TwoPoints );
  RegisterSelector( myTwoPointsSelectors->LineEdit2,     myTwoPointsSelectors->PushButton2,     aPointModes, TabID_TwoPoints );
  RegisterSelector( myParallelEdgesSelectors->LineEdit1, myParallelEdgesSelectors->PushButton1, anEdgeModes, TabID_ParallelEdges );
  RegisterSelector( myParallelEdgesSelectors->LineEdit2, myParallelEdgesSelectors->PushButton2, anEdgeModes, TabID_ParallelEdges );
}

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::DiameterPane
// purpose  : Constructor
//=================================================================================
MeasureGUI_CreateDimensionDlg::DiameterPane::DiameterPane( QWidget* theParent )
: BaseSelectorPane( theParent )
{
  myShapeSelector = new MeasureGUI_1Sel_Frame( this );
  myShapeSelector->PushButton1->setIcon( mySelectorIcon );
  myShapeSelector->TextLabel1 ->setText( MeasureGUI_CreateDimensionDlg::tr( "OBJECT" ) );

  QVBoxLayout* aLayout = new QVBoxLayout( this );

  aLayout->setMargin( 0 );
  aLayout->addWidget( myShapeSelector );

  SelectionModes aDiamModes;
  aDiamModes << TopAbs_SHAPE
             << TopAbs_FACE
             << TopAbs_EDGE;

  RegisterSelector( myShapeSelector->LineEdit1, myShapeSelector->PushButton1, aDiamModes );
}

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::AnglePane
// purpose  : Constructor
//=================================================================================
MeasureGUI_CreateDimensionDlg::AnglePane::AnglePane( QWidget* theParent )
: BaseSelectorPane( theParent )
{
  myTabs                 = new QTabWidget( this );
  myTwoEdgesSelectors    = new MeasureGUI_2Sel_Frame( this );
  myThreePointsSelectors = new MeasureGUI_3Sel_Frame( this );

  myTabs->addTab( myTwoEdgesSelectors,    MeasureGUI_CreateDimensionDlg::tr( "TWO_EDGES" ) );
  myTabs->addTab( myThreePointsSelectors, MeasureGUI_CreateDimensionDlg::tr( "THREE_POINTS" ) );

  myTwoEdgesSelectors->PushButton1->setIcon( mySelectorIcon );
  myTwoEdgesSelectors->PushButton2->setIcon( mySelectorIcon );
  myTwoEdgesSelectors->TextLabel1 ->setText( MeasureGUI_CreateDimensionDlg::tr( "EDGE_1" ) );
  myTwoEdgesSelectors->TextLabel2 ->setText( MeasureGUI_CreateDimensionDlg::tr( "EDGE_2" ) );

  myThreePointsSelectors->PushButton1->setIcon( mySelectorIcon );
  myThreePointsSelectors->PushButton2->setIcon( mySelectorIcon );
  myThreePointsSelectors->PushButton3->setIcon( mySelectorIcon );
  myThreePointsSelectors->TextLabel1 ->setText( MeasureGUI_CreateDimensionDlg::tr( "POINT_1" ) );
  myThreePointsSelectors->TextLabel2 ->setText( MeasureGUI_CreateDimensionDlg::tr( "POINT_2" ) );
  myThreePointsSelectors->TextLabel3 ->setText( MeasureGUI_CreateDimensionDlg::tr( "POINT_3" ) );

  QVBoxLayout* aLayout = new QVBoxLayout( this );

  aLayout->setMargin( 0 );
  aLayout->addWidget( myTabs );

  SelectionModes anEdgeModes;
  SelectionModes aPointModes;
  anEdgeModes << TopAbs_EDGE;
  aPointModes << TopAbs_VERTEX;

  SetTabWidget( myTabs );
  RegisterSelector( myTwoEdgesSelectors->LineEdit1,    myTwoEdgesSelectors->PushButton1,    anEdgeModes, TabID_TwoEdges );
  RegisterSelector( myTwoEdgesSelectors->LineEdit2,    myTwoEdgesSelectors->PushButton2,    anEdgeModes, TabID_TwoEdges );
  RegisterSelector( myThreePointsSelectors->LineEdit1, myThreePointsSelectors->PushButton1, aPointModes, TabID_ThreePoints );
  RegisterSelector( myThreePointsSelectors->LineEdit2, myThreePointsSelectors->PushButton2, aPointModes, TabID_ThreePoints );
  RegisterSelector( myThreePointsSelectors->LineEdit3, myThreePointsSelectors->PushButton3, aPointModes, TabID_ThreePoints );
}

/* ------------------------------------------------------------------------------*
 *                                                                               *
 *                   Argument shape selection logics                             *
 *                                                                               *
 * ------------------------------------------------------------------------------*/

//=================================================================================
// class    : MeasureGUI_CreateDimensionDlg::BaseSelectorPane
// purpose  : Constructor
//=================================================================================
MeasureGUI_CreateDimensionDlg::BaseSelectorPane::BaseSelectorPane( QWidget* theParent )
: QWidget( theParent ),
  myTabs( NULL )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  mySelectorIcon = aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) );
}

//=================================================================================
// function : BaseSelectorPane::Reset
// purpose  : Reset selector line edits and controls
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::Reset( bool theOpenDefaultTab )
{
  if ( theOpenDefaultTab && myTabs )
  {
    myTabs->setCurrentIndex( 0 );
  }

  QMap< int, QList<QLineEdit*> >::iterator aTabsIt = mySelectors.begin();
  for ( ; aTabsIt != mySelectors.end(); ++aTabsIt )
  {
    QList<QLineEdit*>& aSelectors = *aTabsIt;
    QList<QLineEdit*>::iterator aSelectorIt = aSelectors.begin();
    for ( ; aSelectorIt != aSelectors.end(); ++aSelectorIt )
    {
      QLineEdit* aSelector = *aSelectorIt;
      aSelector->clear();
      mySelectedShapes[aSelector] = GEOM::GeomObjPtr();
    }
  }

  QLineEdit* aFirstSelector = mySelectors[ ActiveTab() ].first();

  mySelectionButtons[aFirstSelector]->click();
}

//=================================================================================
// function : BaseSelectorPane::ActiveTab
// purpose  : Returns active tab
//=================================================================================
int MeasureGUI_CreateDimensionDlg::BaseSelectorPane::ActiveTab() const
{
  return myTabs != NULL ? myTabs->currentIndex() : 0;
}

//=================================================================================
// function : BaseSelectorPane::SelectionIntoArgument
// purpose  : Populates current selector
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::SelectionIntoArguments( const GEOM::GeomObjPtr& theSelected )
{
  if ( theSelected.isNull() )
  {
    myCurrentSelector->clear();
    mySelectedShapes[myCurrentSelector] = GEOM::GeomObjPtr();
    return;
  }

  QString aName = GEOMBase::GetName( theSelected.get() );

  myCurrentSelector->setText( aName );

  mySelectedShapes[myCurrentSelector] = theSelected;

  // find next empty selector (iterate to the last)
  QList<QLineEdit*>& aCurrentSelectors = mySelectors[ ActiveTab() ];
  int aCurrentPos = aCurrentSelectors.indexOf( myCurrentSelector );
  int aSelectorIt = aCurrentPos + 1;
  for ( ; aSelectorIt != aCurrentSelectors.size(); ++aSelectorIt )
  {
    QLineEdit* aNextSelector = aCurrentSelectors[aSelectorIt];
    if ( mySelectedShapes[aNextSelector].isNull() )
    {
      mySelectionButtons[aNextSelector]->click();
      return;
    }
  }

  // find next empty selector (itearte from the first)
  aSelectorIt = 0;
  for ( ; aSelectorIt != aCurrentPos; ++aSelectorIt )
  {
    QLineEdit* aNextSelector = aCurrentSelectors[aSelectorIt];
    if ( mySelectedShapes[aNextSelector].isNull() )
    {
      mySelectionButtons[aNextSelector]->click();
      return;
    }
  }

  // stop selector switching
  myCurrentSelector->setEnabled( false );
  mySelectionButtons[myCurrentSelector]->setDown( false );

  // every selector is populated - construct presentation
  emit StopSelection();
  emit SelectionDone();
}

//=================================================================================
// function : BaseSelectorPane::GetSelection
// purpose  : 
//=================================================================================
GEOM::GeomObjPtr MeasureGUI_CreateDimensionDlg::BaseSelectorPane::GetSelection( QLineEdit* theSelector ) const
{
  return mySelectedShapes[theSelector];
}

//=================================================================================
// function : BaseSelectorPane::OnSelectorClicked
// purpose  : Handle activation of selector controls pair {edit, button}
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::OnSelectorClicked()
{
  // clicked "selector" button
  QAbstractButton* aSender = qobject_cast<QAbstractButton*>( sender() );
  if ( !aSender )
  {
    return;
  }

  // get "selector" controls on the active tab
  QList<QLineEdit*>& aCurrentSelectors = mySelectors[ ActiveTab() ];

  // iterate over controls on the tab and process them
  for ( int aSelectorIt = 0; aSelectorIt < aCurrentSelectors.size(); ++aSelectorIt )
  {
    QLineEdit* aSelector = aCurrentSelectors[aSelectorIt];
    QPushButton* aButton = mySelectionButtons[aSelector];

    bool isClickedOnes = (aButton == aSender);

    aSelector->setEnabled( isClickedOnes );

    if ( isClickedOnes )
    {
      myCurrentSelector = aSelector;
      myCurrentSelector->setFocus();
    }

    aButton->setDown( isClickedOnes );
  }

  emit StartSelection( mySelectionModes[myCurrentSelector] );
}

//=================================================================================
// function : BaseSelectorPane::OnTabChanged
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::OnTabChanged()
{
  QList<QLineEdit*>& aSelectors = mySelectors[ ActiveTab() ];
  QList<QLineEdit*>::iterator aSelectorIt = aSelectors.begin();
  for ( ; aSelectorIt != aSelectors.end(); ++aSelectorIt )
  {
    QLineEdit* aSelector = *aSelectorIt;

    aSelector->clear();

    mySelectedShapes[aSelector] = GEOM::GeomObjPtr();
  }

  QLineEdit* aFirstSelector = mySelectors[ ActiveTab() ].first();

  mySelectionButtons[aFirstSelector]->click();

  emit TabChanged();
}

//=================================================================================
// function : BaseSelectorPane::SetTabWidget
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::SetTabWidget( QTabWidget* theTabs )
{
  myTabs = theTabs;
  connect( myTabs, SIGNAL( currentChanged( int ) ), this, SLOT( OnTabChanged() ) );
}

//=================================================================================
// function : BaseSelectorPane::RegisterSelector
// purpose  :
//=================================================================================
void MeasureGUI_CreateDimensionDlg::BaseSelectorPane::RegisterSelector( QLineEdit* theSelectorEdit,
                                                                        QPushButton* theSelectorButton,
                                                                        const SelectionModes& theSelectorModes,
                                                                        const int theTab )
{
  if ( !mySelectors.contains( theTab ) )
  {
    mySelectors.insert( theTab, QList<QLineEdit*>() );
  }
  mySelectors[theTab].append( theSelectorEdit );

  mySelectionButtons[theSelectorEdit] = theSelectorButton;
  mySelectionModes  [theSelectorEdit] = theSelectorModes;

  connect( theSelectorButton, SIGNAL( clicked() ), this, SLOT( OnSelectorClicked() ) );
}
