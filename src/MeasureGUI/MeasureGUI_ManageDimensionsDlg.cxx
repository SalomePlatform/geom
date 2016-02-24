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
// File   : MeasureGUI_ManageDimensionsDlg.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_ManageDimensionsDlg.h"
#include "MeasureGUI_CreateDimensionDlg.h"
#include "MeasureGUI_DimensionFilter.h"

#include <GEOMGUI_DimensionProperty.h>
#include <GEOMGUI_TextTreeWdg.h>
#include <GEOMUtils.hxx>
#include <GEOMGUI_OCCSelector.h>
#include <GEOM_AISDimension.hxx>
#include <GEOM_Constants.h>
#include <GEOMBase.h>
#include <DlgRef.h>

#include <SOCC_ViewModel.h>
#include <SOCC_Prs.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_ListOfFilter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#include <QTreeWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
MeasureGUI_ManageDimensionsDlg::MeasureGUI_ManageDimensionsDlg( GeometryGUI* theGUI, QWidget* theParent )
: GEOMBase_Skeleton( theGUI, theParent ),
  myOperatedViewer( NULL ),
  myCurrentSelection( Selection_None ),
  myCreateDialog(0)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();

  QPixmap aSelectorIcon = aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) );

  setWindowTitle( tr( "MANAGE_DIMENSIONS_TITLE" ) );

  mainFrame()->GroupConstructors->setVisible( false );
  mainFrame()->GroupBoxName->setVisible( false );

  // construct object selector pane
  myObjectSelector = new DlgRef_1Sel( centralWidget() );
  myObjectSelector->TextLabel1->setText( tr( "OBJECT_LABEL" ) );
  myObjectSelector->LineEdit1->setReadOnly( true );
  myObjectSelector->PushButton1->setIcon( aSelectorIcon );
  myObjectSelector->PushButton1->setCheckable( true );

  // construct dimension list view pane
  myDimensionView = new MeasureGUI_1TreeWidget_4Button( centralWidget() );
  myDimensionView->GroupBox->setTitle( tr( "DIMENSIONS_GROUP" ) );
  myDimensionView->PushButton1->setText( tr( "ADD_BTN" ) );
  myDimensionView->PushButton2->setText( tr( "REMOVE_BTN" ) );
  myDimensionView->PushButton3->setText( tr( "SHOW_ALL_BTN" ) );
  myDimensionView->PushButton4->setText( tr( "HIDE_ALL_BTN" ) );
  myDimensionView->TreeWidget->setMinimumHeight( 250 );
  myDimensionView->setEnabled( false );

  connect( myDimensionView->PushButton1, SIGNAL( clicked() ), SLOT( OnAdd() ) );
  connect( myDimensionView->PushButton2, SIGNAL( clicked() ), SLOT( OnRemove() ) );
  connect( myDimensionView->PushButton3, SIGNAL( clicked() ), SLOT( OnShowAll() ) );
  connect( myDimensionView->PushButton4, SIGNAL( clicked() ), SLOT( OnHideAll() ) );
  connect( myDimensionView->TreeWidget, 
           SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
           SLOT( OnChangeItem( QTreeWidgetItem* ) ) );
  connect( myDimensionView->TreeWidget,
           SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ),
           SLOT( OnSelectItem() ) );

  // populate tree with top-level items
  myDimensionView->TreeWidget->setHeaderItem( new QTreeWidgetItem( (QTreeWidget*)NULL, QStringList( QString() ) ) );

  // construct main frame layout
  QVBoxLayout* aMainLayout = new QVBoxLayout( centralWidget() );
  aMainLayout->setMargin( 0 );
  aMainLayout->addWidget( myObjectSelector, 0 );
  aMainLayout->addWidget( myDimensionView, 1 );

  // signals and slots connections
  connect( myObjectSelector->PushButton1, SIGNAL( clicked() ), SLOT( StartObjectSelection() ) );
  connect( buttonOk(),    SIGNAL( clicked() ), SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), SLOT( ClickOnApply() ) );

  connect( this, SIGNAL( finished( int ) ), SLOT( OnFinish() ) );
  
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( OnDeactivateThisDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  myDimensionInteractor = new MeasureGUI_DimensionInteractor( theGUI, theParent ),

  SelectionIntoArgument( Selection_Object );

  if ( myEditObject.isNull() )
  {
    myObjectSelector->PushButton1->click();
  }
  myIsNeedRedisplay = false;
  setHelpFileName("managing_dimensions_page.html");
}

//=================================================================================
// function : Destructor
// purpose  :
//=================================================================================
MeasureGUI_ManageDimensionsDlg::~MeasureGUI_ManageDimensionsDlg()
{
  StopSelection();
}

//=================================================================================
// function : StartSelection
// purpose  : Starts specific selection
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::StartSelection( const Selection theSelection )
{
  StopSelection();

  myCurrentSelection = theSelection;

  if ( theSelection == Selection_Object && AllowedToCancelChanges() )
  {
    /* -----------------------------------------------  *
     *               selection of object                *
     * ------------------------------------------------ */

    globalSelection( GEOM_ALLSHAPES );
    connect( myGeomGUI->getApp()->selectionMgr(),
             SIGNAL( currentSelectionChanged() ),
             this,
             SLOT( OnSelection() ) );
  }
  else if ( theSelection == Selection_Dimension && !myEditObject.isNull() )
  {
    /* -----------------------------------------------  *
     *               selection of dimension             *
     * ------------------------------------------------ */

    SalomeApp_Application* anApp = myGeomGUI->getApp();
    if ( !anApp )
    {
      return;
    }

    SUIT_ViewManager* aViewMgr = anApp->activeViewManager();
    if ( aViewMgr->getType() != SOCC_Viewer::Type() )
    {
      return;
    }

    myOperatedViewer = dynamic_cast<SOCC_Viewer*>( aViewMgr->getViewModel() );

    globalSelection();

    Handle(AIS_InteractiveContext) anAISContext = myOperatedViewer->getAISContext();

    anAISContext->ClearCurrents( Standard_False );
    anAISContext->ClearSelected( Standard_False );
    anAISContext->OpenLocalContext( Standard_True, Standard_False );

    Handle(MeasureGUI_DimensionFilter) aFilter = new MeasureGUI_DimensionFilter( myEditObject->GetStudyEntry() );

    anAISContext->AddFilter( aFilter );

    LightApp_SelectionMgr* aSelectionMgr = myGeomGUI->getApp()->selectionMgr();

    QList<SUIT_Selector*> aSelectors;
    aSelectionMgr->selectors( aSelectors );
    QList<SUIT_Selector*>::iterator aSelectorIt = aSelectors.begin();
    for ( ; aSelectorIt != aSelectors.end(); ++aSelectorIt )
    {
      SUIT_Selector* aSelector = *aSelectorIt;

      GEOMGUI_OCCSelector* aGeomSelector = dynamic_cast<GEOMGUI_OCCSelector*>( aSelector );
      if ( !aGeomSelector )
      {
        continue;
      }

      aGeomSelector->setEnabled( false );
    }

    connect( myOperatedViewer,
             SIGNAL( selectionChanged() ),
             this,
             SLOT( OnSelection() ) );

    myDimensionInteractor->Enable();

    connect( myDimensionInteractor,
             SIGNAL( InteractionFinished( Handle(AIS_InteractiveObject) ) ),
             this,
             SLOT( OnInteractionFinished( Handle(AIS_InteractiveObject) ) ) );

    anAISContext->UpdateCurrentViewer();
  }
}

//=================================================================================
// function : StopObjectSelection
// purpose  : Stops specific selection
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::StopSelection()
{
  if ( myCurrentSelection == Selection_Object )
  {
    /* -----------------------------------------------  *
     *               selection of object                *
     * ------------------------------------------------ */

    myObjectSelector->PushButton1->setChecked( false );
    disconnect( myGeomGUI->getApp()->selectionMgr(),
                SIGNAL( currentSelectionChanged() ),
                this,
                SLOT( OnSelection() ) );
  }
  else if ( myCurrentSelection == Selection_Dimension && myOperatedViewer )
  {
    /* -----------------------------------------------  *
     *               selection of dimension             *
     * ------------------------------------------------ */

    Handle(AIS_InteractiveContext) anAISContext = myOperatedViewer->getAISContext();

    anAISContext->CloseLocalContext();

    LightApp_SelectionMgr* aSelectionMgr = myGeomGUI->getApp()->selectionMgr();

    QList<SUIT_Selector*> aSelectors;
    aSelectionMgr->selectors( aSelectors );
    QList<SUIT_Selector*>::iterator aSelectorIt = aSelectors.begin();
    for ( ; aSelectorIt != aSelectors.end(); ++aSelectorIt )
    {
      SUIT_Selector* aSelector = *aSelectorIt;

      GEOMGUI_OCCSelector* aGeomSelector = dynamic_cast<GEOMGUI_OCCSelector*>( aSelector );
      if ( !aGeomSelector )
      {
        continue;
      }

      aGeomSelector->setEnabled( true );
    }

    disconnect( myOperatedViewer,
                SIGNAL( selectionChanged() ),
                this,
                SLOT( OnSelection() ) );

    myDimensionInteractor->Disable();

    disconnect( myDimensionInteractor,
                SIGNAL( InteractionFinished( Handle(AIS_InteractiveObject) ) ),
                this,
                SLOT( OnInteractionFinished( Handle(AIS_InteractiveObject) ) ) );
  }

  myCurrentSelection = Selection_None;

  globalSelection();
}

//=================================================================================
// function : OnSelection
// purpose  : 
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnSelection()
{
  SelectionIntoArgument( myCurrentSelection );
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : 
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::SelectionIntoArgument( const Selection theSelection  )
{
  if ( theSelection == Selection_Object )
  {
    /* -----------------------------------------------  *
     *               selection of object                *
     * ------------------------------------------------ */

    LightApp_SelectionMgr* aSelectionMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelection;
    aSelectionMgr->selectedObjects( aSelection );

    GEOM::GeomObjPtr aSelected;

    if ( aSelection.Extent() == 1 )
    {
      aSelected = GEOMBase::ConvertIOinGEOMObject( aSelection.First() );
    }

    SetEditObject( aSelected );
    StartSelection( Selection_Dimension );
  }
  else if ( theSelection == Selection_Dimension && myOperatedViewer )
  {
    /* -----------------------------------------------  *
     *               selection of dimension             *
     * ------------------------------------------------ */

    QTreeWidget* aDimensionList = myDimensionView->TreeWidget;

    Handle(AIS_InteractiveContext) anAISContext = myOperatedViewer->getAISContext();

    // non-single selection is prohibited
    if ( anAISContext->NbSelected() != 1 )
    {
      aDimensionList->setCurrentIndex( QModelIndex() );
      return;
    }

    anAISContext->InitSelected();

    Handle(AIS_InteractiveObject) anAIS;

    if ( anAISContext->HasOpenedContext() )
    {
      Handle(SelectMgr_EntityOwner) anAISOwner = anAISContext->SelectedOwner();
      anAIS = Handle(AIS_InteractiveObject)::DownCast( anAISOwner->Selectable() );
    }
    else
    {
      anAIS = anAISContext->Current();
    }

    int aDimensionId = IdFromPrs( anAIS );

    SelectInList( aDimensionId );

    if ( aDimensionId >= 0 )
    {
      SelectInViewer( myOperatedViewer, aDimensionId );
    }
  }
}

//=================================================================================
// function : OnAdd
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnAdd()
{
  if(!myCreateDialog) {
    QWidget* aParent = qobject_cast<QWidget*>( this->parent() );
    this->Suspend();
    myCreateDialog = new MeasureGUI_CreateDimensionDlg( myEditObject, myGeomGUI, aParent );
    connect( myCreateDialog, SIGNAL( finished( int ) ), this, SLOT( Resume() ) );
    // this is necessary as the GEOMBase_Helper switches selection mode on destruction
    connect( myCreateDialog, SIGNAL( destroyed( QObject* ) ), this, SLOT( Resume() ) );
    connect( myCreateDialog, SIGNAL( applyClicked() ), this, SLOT( OnDimensionAdded() ) );
    myCreateDialog->updateGeometry();
    myCreateDialog->resize( myCreateDialog->minimumSizeHint() );
    myCreateDialog->show();
  } else {
    myCreateDialog->activateWindow();
  }
  myObjectSelector->setEnabled(false);
  myDimensionView->setEnabled(false);
}

//=================================================================================
// function : OnRemove
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnRemove()
{
  int aDimensionId = IdFromItem( myDimensionView->TreeWidget->currentItem() );
  if ( aDimensionId < 0 )
  {
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  aProp.RemoveRecord( aDimensionId );

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );

  PopulateList();

  RedisplayObject();
}

//=================================================================================
// function : OnChangeItem
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnChangeItem( QTreeWidgetItem* theItem )
{
  int aDimensionId = IdFromItem( theItem );
  if ( aDimensionId < 0 )
  {
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  // change property data
  bool isAnythingChanged = false;

  QString aName = theItem->text( 0 );
  if ( aProp.GetName( aDimensionId ) != aName )
  {
    isAnythingChanged = true;
    aProp.SetName( aDimensionId, aName );
  }

  bool isVisible = theItem->checkState( 0 ) == Qt::Checked;
  if ( aProp.IsVisible( aDimensionId ) != isVisible )
  {
    isAnythingChanged = true;
    aProp.SetVisible( aDimensionId, isVisible );
  }

  // store property data
  if ( !isAnythingChanged )
  {
    return;
  }

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );

  RedisplayObject();
}

//=================================================================================
// function : OnSelectItem
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnSelectItem()
{
  if ( !myOperatedViewer )
  {
    return;
  }

  QTreeWidgetItem* aSelectedItem = myDimensionView->TreeWidget->currentItem();

  SelectInViewer( myOperatedViewer, IdFromItem( aSelectedItem ) );
}

//=================================================================================
// function : OnShowAll
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnShowAll()
{
  // read propety data
  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  QTreeWidget* aDimensionList = myDimensionView->TreeWidget;

  bool isBlocked = aDimensionList->blockSignals( true );

  for ( int anIt = 0; anIt < aProp.GetNumber(); ++anIt )
  {
    aProp.SetVisible( anIt, true );
  }

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );

  PopulateList();

  RedisplayObject();

  aDimensionList->blockSignals( isBlocked );
}

//=================================================================================
// function : OnHideAll
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnHideAll()
{
  // read propety data
  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  QTreeWidget* aDimensionList = myDimensionView->TreeWidget;

  bool isBlocked = aDimensionList->blockSignals( true );

  for ( int anIt = 0; anIt < aProp.GetNumber(); ++anIt )
  {
    aProp.SetVisible( anIt, false );
  }

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );

  PopulateList();

  RedisplayObject();

  aDimensionList->blockSignals( isBlocked );
}

//=================================================================================
// function : OnInteractionFinished
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnInteractionFinished( Handle(AIS_InteractiveObject) theIO )
{
  // update property data
  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  gp_Ax3 aLCS;
  TopoDS_Shape anParentSh;
  if ( GEOMBase::GetShape( myEditObject.get(), anParentSh ) )
  {
    aLCS = gp_Ax3().Transformed( anParentSh.Location().Transformation() );
  }

  int aDimensionId = IdFromPrs( theIO );

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  aProp.SetRecord( aDimensionId, Handle(AIS_Dimension)::DownCast( theIO ), aLCS );

  // store modified property data
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             aProp );
}

//=================================================================================
// function : Suspend
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::Suspend()
{
  StopSelection();
}

//=================================================================================
// function : Suspend
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::Resume()
{
  if(myCreateDialog){
    myCreateDialog = 0;
    myObjectSelector->setEnabled(true);
    myDimensionView->setEnabled(true);
  }

  if ( !myEditObject.isNull() )
  {
    PopulateList();
    StartSelection( Selection_Dimension );
  }
}

//=================================================================================
// function : Suspend
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnDimensionAdded() {
  if ( !myEditObject.isNull() )
  {
    PopulateList();
  }
}

//=================================================================================
// function : ClickOnOk
// purpose  : 
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
  {
    ClickOnCancel();
  }
}

//=================================================================================
// function : ClickOnCancel
// purpose  : 
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::ClickOnCancel()
{
  if ( !AllowedToCancelChanges() )
  {
    return;
  }
  
  if( myCreateDialog ) {
    myCreateDialog->close();
    myCreateDialog = 0;
  }
  
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply
// purpose  : 
//=================================================================================
bool MeasureGUI_ManageDimensionsDlg::ClickOnApply()
{
  if ( myEditObject.isNull() )
  {
    return true;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return true;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return true;
  }

  mySavedPropertyState = 
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  mySavedPropertyState.SaveToAttribute( aStudy, myEditObject->GetStudyEntry() );

  myGeomGUI->emitDimensionsUpdated( QString( myEditObject->GetStudyEntry() ) );

  return true;
}

//=================================================================================
// function : OnFinish
// purpose  : 
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnFinish()
{
  if ( myEditObject.isNull() )
  {
    GEOMBase_Skeleton::ClickOnCancel();
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    GEOMBase_Skeleton::ClickOnCancel();
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    GEOMBase_Skeleton::ClickOnCancel();
    return;
  }

  // reset preview property state
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             QVariant() );

  if ( myIsNeedRedisplay ) {
    redisplay( myEditObject.get() );
  }
  
  myGeomGUI->emitDimensionsUpdated( QString( myEditObject->GetStudyEntry() ) );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
  {
    OnActivateThisDialog();
  }
}

//=================================================================================
// function : OnActivateThisDialog
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnActivateThisDialog()
{
  disconnect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( OnDeactivateThisDialog() ) );
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( OnDeactivateThisDialog() ) );

  SelectionIntoArgument( Selection_Object );

  if ( myEditObject.isNull() )
  {
    myObjectSelector->PushButton1->click();
  }
}

//=================================================================================
// function : OnDeactivateThisDialog
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::OnDeactivateThisDialog()
{
  if ( AllowedToSaveChanges() )
  {
    ClickOnApply();
  }

  if ( !myEditObject.isNull() )
  {
    SalomeApp_Study* aStudy = NULL;
    SalomeApp_Application* anApp = myGeomGUI->getApp();
    if ( anApp )
    {
      aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
    }

    if ( aStudy )
    {
      aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                                 myEditObject->GetStudyEntry(),
                                 GEOM::propertyName( GEOM::Dimensions ),
                                 QVariant() );
    }

    redisplay( myEditObject.get() );
  }

  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : SetEditObject
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::SetEditObject( const GEOM::GeomObjPtr& theObj )
{
  RestoreState();

  QString aName = GEOMBase::GetName( theObj.get() );

  myObjectSelector->LineEdit1->setText( aName );

  myEditObject = theObj;

  if ( myEditObject.isNull() )
  {
    myDimensionView->TreeWidget->clear();
    myDimensionView->setEnabled( false );
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SUIT_ViewManager* aViewMgr = anApp->activeViewManager();
  if ( aViewMgr->getType() != SOCC_Viewer::Type() )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  mySavedPropertyState.LoadFromAttribute( getStudy(), myEditObject->GetStudyEntry() );

  // set property state for preview
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             mySavedPropertyState );

  PopulateList();

  myDimensionView->setEnabled( true );
}

//=================================================================================
// function : RestoreState
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::RestoreState()
{
  if ( myEditObject.isNull() )
  {
    return;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  QString aEntry = myEditObject->GetStudyEntry();

  // reset preview property state
  aStudy->setObjectProperty( GEOM::sharedPropertiesId(),
                             myEditObject->GetStudyEntry(),
                             GEOM::propertyName( GEOM::Dimensions ),
                             QVariant() );
}

//=================================================================================
// function : PopulateList
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::PopulateList()
{
  QTreeWidget* aListView = myDimensionView->TreeWidget;

  // clear list completely if object is null
  if ( myEditObject.isNull() )
  {
    aListView->clear();
    return;
  }

  // clear only groups data if object is ok
  QTreeWidgetItem* aDistanceGroupItem = aListView->topLevelItem( GroupItem_Distance );
  QTreeWidgetItem* aDiameterGroupItem = aListView->topLevelItem( GroupItem_Diameter );
  QTreeWidgetItem* anAngleGroupItem   = aListView->topLevelItem( GroupItem_Angle );

  // create group items if not exist yet
  if ( !aDistanceGroupItem || !aDiameterGroupItem || !anAngleGroupItem )
  {
    aDistanceGroupItem = new QTreeWidgetItem( (QTreeWidget*)NULL, QStringList( tr( "DISTANCE_ITEM" ) ) );
    aDiameterGroupItem = new QTreeWidgetItem( (QTreeWidget*)NULL, QStringList( tr( "DIAMETER_ITEM" ) ) );
    anAngleGroupItem   = new QTreeWidgetItem( (QTreeWidget*)NULL, QStringList( tr( "ANGLE_ITEM" ) ) );

    aListView->clear();

    aListView->addTopLevelItem( aDistanceGroupItem );
    aListView->addTopLevelItem( aDiameterGroupItem );
    aListView->addTopLevelItem( anAngleGroupItem );
    aListView->expandAll();
  }

  qDeleteAll( aDistanceGroupItem->takeChildren() );
  qDeleteAll( aDiameterGroupItem->takeChildren() );
  qDeleteAll( anAngleGroupItem->takeChildren() );

  // collect all groupped items
  QList<QTreeWidgetItem*> aDistItems;
  QList<QTreeWidgetItem*> aDiamItems;
  QList<QTreeWidgetItem*> anAngItems;

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return;
  }

  QString aEntry = myEditObject->GetStudyEntry();

  // get property data to change
  GEOMGUI_DimensionProperty aProp =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  // read dimension records from property
  for ( int anIt = 0; anIt < aProp.GetNumber(); ++anIt )
  {
    QString aName  = aProp.GetName( anIt );
    bool isVisible = aProp.IsVisible( anIt );
    int aType      = aProp.GetType( anIt );

    QTreeWidgetItem* anItem = new QTreeWidgetItem;
    anItem->setText( 0, aName );
    anItem->setCheckState( 0, isVisible ? Qt::Checked : Qt::Unchecked );
    anItem->setData( 0, Qt::UserRole, anIt );
    anItem->setFlags( Qt::ItemIsUserCheckable
                    | Qt::ItemIsSelectable
                    | Qt::ItemIsEditable
                    | Qt::ItemIsEnabled );

    switch ( aType )
    {
      case GEOMGUI_DimensionProperty::DimensionType_Length   : aDistItems << anItem; break;
      case GEOMGUI_DimensionProperty::DimensionType_Diameter : aDiamItems << anItem; break;
      case GEOMGUI_DimensionProperty::DimensionType_Angle    : anAngItems << anItem; break;
    }
  }

  aDistanceGroupItem->addChildren( aDistItems );
  aDiameterGroupItem->addChildren( aDiamItems );
  anAngleGroupItem->addChildren( anAngItems );

  aListView->setItemHidden( aDistanceGroupItem, aDistItems.isEmpty() );
  aListView->setItemHidden( aDiameterGroupItem, aDiamItems.isEmpty() );
  aListView->setItemHidden( anAngleGroupItem,   anAngItems.isEmpty() );
}

//=================================================================================
// function : HasUnsavedChanges
// purpose  :
//=================================================================================
bool MeasureGUI_ManageDimensionsDlg::HasUnsavedChanges()
{
  if ( myEditObject.isNull() )
  {
    return false;
  }

  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return false;
  }

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
  if ( !aStudy )
  {
    return false;
  }

  GEOMGUI_DimensionProperty aCurrentState =
    aStudy->getObjectProperty( GEOM::sharedPropertiesId(),
                               myEditObject->GetStudyEntry(),
                               GEOM::propertyName( GEOM::Dimensions ),
                               QVariant() )
                               .value<GEOMGUI_DimensionProperty>();

  return aCurrentState != mySavedPropertyState;
}

//=================================================================================
// function : AllowedToCancelChanges
// purpose  :
//=================================================================================
bool MeasureGUI_ManageDimensionsDlg::AllowedToCancelChanges()
{
  if ( !HasUnsavedChanges() )
  {
    return true;
  }

  int aResponse = SUIT_MessageBox::warning( this,
                                            tr( "WRN_TITLE_UNSAVED" ),
                                            tr( "WRN_MSG_CHANGES_LOST" ),
                                            QMessageBox::Ok,
                                            QMessageBox::Cancel );
  myIsNeedRedisplay = ( aResponse == QMessageBox::Ok );
  return myIsNeedRedisplay;
}

//=================================================================================
// function : AllowedToSaveChanges
// purpose  :
//=================================================================================
bool MeasureGUI_ManageDimensionsDlg::AllowedToSaveChanges()
{
  if ( !HasUnsavedChanges() )
  {
    return false;
  }

  int aResponse = SUIT_MessageBox::warning( this,
                                            tr( "WRN_TITLE_UNSAVED" ),
                                            tr( "WRN_MSG_CHANGES_SAVE" ),
                                            QMessageBox::Ok,
                                            QMessageBox::Cancel );

  return aResponse == QMessageBox::Ok;
}

//=================================================================================
// function : IdFromItem
// purpose  :
//=================================================================================
int MeasureGUI_ManageDimensionsDlg::IdFromItem( QTreeWidgetItem* theItem )
{
  if ( !theItem )
  {
    return -1;
  }

  bool isIdOK = false;
  const int anId = theItem->data( 0, Qt::UserRole ).toInt( &isIdOK );

  return isIdOK ? anId : -1;
}

//=================================================================================
// function : IdFromPrs
// purpose  :
//=================================================================================
int MeasureGUI_ManageDimensionsDlg::IdFromPrs( const Handle(AIS_InteractiveObject)& theAIS )
{
  Handle(SALOME_InteractiveObject) anIO = Handle(SALOME_InteractiveObject)::DownCast( theAIS->GetOwner() );

  if ( anIO.IsNull() )
  {
    return -1;
  }

  QString anIOEntry   = anIO->getEntry();
  QString anEditEntry = myEditObject->GetStudyEntry();
  if ( anIOEntry != anEditEntry )
  {
    return -1;
  }

  Handle(GEOM_AISLength) aLength = Handle(GEOM_AISLength)::DownCast( theAIS );
  if ( !aLength.IsNull() )
  {
    return aLength->GetId();
  }

  Handle(GEOM_AISDiameter) aDiameter = Handle(GEOM_AISDiameter)::DownCast( theAIS );
  if ( !aDiameter.IsNull() )
  {
    return aDiameter->GetId();
  }

  Handle(GEOM_AISAngle) anAngle = Handle(GEOM_AISAngle)::DownCast( theAIS );
  if ( !anAngle.IsNull() )
  {
    return anAngle->GetId();
  }

  return -1;
}

//=================================================================================
// function : SelectInList
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::SelectInList( const Handle(AIS_InteractiveObject)& theIO )
{
  SelectInList( IdFromPrs( theIO ) );
}

//=================================================================================
// function : SelectInList
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::SelectInList( const int theId )
{
  QTreeWidget* aListOfDimension = myDimensionView->TreeWidget;

  QAbstractItemModel* aModel = aListOfDimension->model();
  QModelIndexList aIndexes = aModel->match( aModel->index(0, 0),
                                            Qt::UserRole,
                                            QVariant( theId ),
                                            -1, Qt::MatchRecursive );

  if ( aIndexes.isEmpty() )
  {
    aListOfDimension->setCurrentIndex( QModelIndex() );
    return;
  }

  bool isBlocked = aListOfDimension->blockSignals( true );
  aListOfDimension->setCurrentIndex( aIndexes.first() );
  aListOfDimension->blockSignals( isBlocked );
}

//=================================================================================
// function : SelectInViewer
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::SelectInViewer( SOCC_Viewer* theViewer, const int theId )
{
  Handle(AIS_InteractiveContext) anAISContext = theViewer->getAISContext();
  if ( anAISContext.IsNull() )
  {
    return;
  }

  Standard_Boolean isLocal = anAISContext->HasOpenedContext();
  if ( isLocal )
  {
    anAISContext->ClearSelected( Standard_False );
  }
  else
  {
    anAISContext->ClearCurrents( Standard_False );
  }

  SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>( theViewer->CreatePrs( myEditObject->GetStudyEntry() ) );

  AIS_ListOfInteractive aListOfIO;
  aPrs->GetObjects( aListOfIO );

  AIS_ListIteratorOfListOfInteractive anIt( aListOfIO );
  for ( ; anIt.More(); anIt.Next() )
  {
    const Handle(AIS_InteractiveObject)& anIO = anIt.Value();
    if ( IdFromPrs( anIO ) != theId )
    {
      if ( isLocal )
      {
        anAISContext->Deactivate( anIO, AIS_DSM_Line );
        anAISContext->Deactivate( anIO, AIS_DSM_Text );
      }
      continue;
    }

    if ( isLocal )
    {
      anAISContext->AddOrRemoveSelected( anIO, Standard_False );
      anAISContext->Activate( anIO, AIS_DSM_Line );
      anAISContext->Activate( anIO, AIS_DSM_Text );
    }
    else
    {
      anAISContext->AddOrRemoveCurrentObject( anIO, Standard_False );
    }

    anAISContext->UpdateCurrentViewer();
  }
}

//=================================================================================
// function : RedisplayObject
// purpose  :
//=================================================================================
void MeasureGUI_ManageDimensionsDlg::RedisplayObject()
{
  redisplay( myEditObject.get() );

  if ( !myOperatedViewer )
  {
    return;
  }

  QTreeWidgetItem* aSelectedItem = myDimensionView->TreeWidget->currentItem();
  if ( !aSelectedItem )
  {
    return;
  }

  StartSelection( myCurrentSelection );

  SelectInViewer( myOperatedViewer, IdFromItem( aSelectedItem ) );
}
