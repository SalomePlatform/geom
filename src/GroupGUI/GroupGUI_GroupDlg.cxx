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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GroupGUI_GroupDlg.cxx
//  Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#include "GroupGUI_GroupDlg.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOM_Displayer.h>
#include <GEOMUtils.hxx>
#ifndef DISABLE_PLOT2DVIEWER
#include <MeasureGUI_ShapeStatisticsDlg.h>
#endif

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>

#include <LightApp_SelectionMgr.h>
#include "utilities.h"

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SALOME_Prs.h>
#include <SALOME_ListIO.hxx>
#include "utilities.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>

#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QMap>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>

#include <GEOMImpl_Types.hxx>

#define GROUP_IDLST_COLOR Qt::blue // Specific color for the IDs of subShapes in the dialog box
#define GROUP_NEWIDLST_COLOR Qt::red // Specific color for the new IDs of subShapes in the dialog box

namespace {
  enum { ALL_SUBSHAPES = 0, GET_IN_PLACE, SUBSHAPES_OF_SHAPE2, SUBSHAPES_OF_INVISIBLE_SHAPE2 };
  enum { Filter_LT, Filter_LE, Filter_GT, Filter_GE };
}

GroupGUI_GroupDlg::GroupGUI_GroupDlg (Mode mode, GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myMode(mode),
    myBusy(false),
    myIsShapeType(false),
    myIsHiddenMain(false),
    myWasHiddenMain(true),
    myIsAccept(false)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap image0     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_VERTEX")));
  QPixmap image1     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_EDGE")));
  QPixmap image2     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_FACE")));
  QPixmap image3     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_SOLID")));
  QPixmap iconSelect (resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(myMode == CreateGroup ? tr("CREATE_GROUP_TITLE") : tr("EDIT_GROUP_TITLE"));

  // Shape type button group
  mainFrame()->GroupConstructors->setEnabled(myMode == CreateGroup);
  mainFrame()->GroupConstructors->setTitle(tr("SHAPE_TYPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);
  mainFrame()->RadioButton4->setIcon(image3);
  mainFrame()->RadioButton4->show();

  // Group name
  mainFrame()->GroupBoxName->setTitle(tr("GROUP_NAME"));

  // Main shape and sub-shapes
  QGroupBox* GroupMedium = new QGroupBox(tr("MAIN_SUB_SHAPES"), centralWidget());
  QGridLayout* aMedLayout = new QGridLayout(GroupMedium);
  aMedLayout->setMargin(9);
  aMedLayout->setSpacing(6);

  QLabel* aMainLabel = new QLabel(tr("MAIN_SHAPE"), GroupMedium);

  mySelBtn = new QPushButton(GroupMedium);
  mySelBtn->setIcon(iconSelect);
  mySelBtn->setEnabled(myMode == CreateGroup);

  myMainName = new QLineEdit(GroupMedium);
  myMainName->setReadOnly(true);
  myMainName->setEnabled(myMode == CreateGroup);

  myRestrictGroupBox = new QGroupBox(tr("SHAPE_SEL_RESTR"), GroupMedium);
  myRestrictGroup = new QButtonGroup(myRestrictGroupBox);
  QRadioButton* allSubs     = new QRadioButton(tr("NO_RESTR")            , myRestrictGroupBox);
  QRadioButton* inPlaceSubs = new QRadioButton(tr("GEOM_PARTS_OF_SHAPE2"), myRestrictGroupBox);
  QRadioButton* shape2Subs  = new QRadioButton(tr("SUBSHAPES_OF_SHAPE2") , myRestrictGroupBox);
  QGridLayout* aRestrictLayout = new QGridLayout(myRestrictGroupBox);

  QLabel* aSecondLabel = new QLabel(tr("SECOND_SHAPE"), myRestrictGroupBox);
  mySelBtn2 = new QPushButton(myRestrictGroupBox);
  mySelBtn2->setIcon(iconSelect);
  mySelBtn2->setEnabled(false);
  myShape2Name = new QLineEdit(myRestrictGroupBox);
  myShape2Name->setReadOnly(true);
  myShape2Name->setEnabled(false);

  aRestrictLayout->setMargin(9);
  aRestrictLayout->setSpacing(6);
  aRestrictLayout->addWidget(allSubs,      0, 0, 1, 3);
  aRestrictLayout->addWidget(inPlaceSubs,  1, 0, 1, 3);
  aRestrictLayout->addWidget(shape2Subs,   2, 0, 1, 3);
  aRestrictLayout->addWidget(aSecondLabel, 3, 0);
  aRestrictLayout->addWidget(mySelBtn2,    3, 1);
  aRestrictLayout->addWidget(myShape2Name, 3, 2);
  myRestrictGroup->addButton(allSubs,      ALL_SUBSHAPES);
  myRestrictGroup->addButton(inPlaceSubs,  GET_IN_PLACE);
  myRestrictGroup->addButton(shape2Subs,   SUBSHAPES_OF_SHAPE2);
  myRestrictGroupBox->setEnabled(!CORBA::is_nil(myMainObj));
  allSubs->setChecked(true);

  myShowOnlyBtn = new QPushButton(tr("SHOW_ONLY_SELECTED"), GroupMedium);
  myHideSelBtn  = new QPushButton(tr("HIDE_SELECTED"), GroupMedium);
  myShowAllBtn  = new QPushButton(tr("SHOW_ALL_SUB_SHAPES"), GroupMedium);

  mySelAllBtn   = new QPushButton(tr("SELECT_ALL"), GroupMedium);
  myAddBtn      = new QPushButton(tr("ADD"), GroupMedium);
  myRemBtn      = new QPushButton(tr("REMOVE"), GroupMedium);

  myIdList    = new QListWidget(GroupMedium);

  myIdList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myIdList->setFlow(QListView::TopToBottom);
  myIdList->setWrapping(true);

  aMedLayout->addWidget(aMainLabel,         0, 0);
  aMedLayout->addWidget(mySelBtn,           0, 1);
  aMedLayout->addWidget(myMainName,         0, 2, 1, 2);
  aMedLayout->addWidget(myRestrictGroupBox, 1, 0, 4, 3);

  aMedLayout->addWidget(myShowOnlyBtn,      1, 3);
  aMedLayout->addWidget(myHideSelBtn,       2, 3);
  aMedLayout->addWidget(myShowAllBtn,       3, 3);

  aMedLayout->addWidget(myIdList,           5, 0, 4, 3);
  aMedLayout->addWidget(mySelAllBtn,        5, 3);
  aMedLayout->addWidget(myAddBtn,           6, 3);
  aMedLayout->addWidget(myRemBtn,           7, 3);

  aMedLayout->setColumnStretch(2, 5);
  aMedLayout->setRowStretch(5, 5);
  aMedLayout->setRowStretch(8, 5);

  //filter group

  myFilterGrp = new QGroupBox(tr("GEOM_FILTER"), centralWidget());
  myLessFilterCheck = new QCheckBox(myFilterGrp);
  myLessFilterCombo = new QComboBox(myFilterGrp);
  myLessFilterCombo->addItem( tr("GEOM_LESS_THAN"), Filter_LT );
  myLessFilterCombo->addItem( tr("GEOM_LESSOREQUAL_THAN"), Filter_LE );
  myGreaterFilterCheck = new QCheckBox(myFilterGrp);
  myGreaterFilterCombo = new QComboBox(myFilterGrp);
  myGreaterFilterCombo->addItem( tr("GEOM_GREAT_THAN"), Filter_GT );
  myGreaterFilterCombo->addItem( tr("GEOM_GREATOREQUAL_THAN"), Filter_GE );
  myLessFilterSpin = new SalomeApp_DoubleSpinBox(myFilterGrp);
  myGreaterFilterSpin = new SalomeApp_DoubleSpinBox(myFilterGrp);
  myApplyFilterButton = new QPushButton(tr("GEOM_BUT_APPLY"), myFilterGrp);
#ifndef DISABLE_PLOT2DVIEWER
  myPlotDistributionButton = new QPushButton(tr("GEOM_PLOT_DISTRIBUTION"), myFilterGrp);
#endif

  QGridLayout* filterLayout = new QGridLayout(myFilterGrp);
  filterLayout->addWidget(myLessFilterCheck,    0, 0);
  filterLayout->addWidget(myLessFilterCombo,    0, 1);
  filterLayout->addWidget(myLessFilterSpin,     0, 2);
  filterLayout->addWidget(myGreaterFilterCheck, 1, 0);
  filterLayout->addWidget(myGreaterFilterCombo, 1, 1);
  filterLayout->addWidget(myGreaterFilterSpin,  1, 2);
  filterLayout->addWidget(myApplyFilterButton,  0, 3);
#ifndef DISABLE_PLOT2DVIEWER
  filterLayout->addWidget(myPlotDistributionButton,  1, 3);
#endif

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupMedium);
  layout->addWidget(myFilterGrp);

  setHelpFileName("work_with_groups_page.html");

  Init();
  updateState();
}

GroupGUI_GroupDlg::~GroupGUI_GroupDlg()
{
  GEOM_Displayer* aDisplayer = getDisplayer();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  bool isHideObjects = resMgr->booleanValue( "Geometry", "hide_input_object", true);
  if (myWasHiddenMain || ( isHideObjects && myIsAccept ) ) {
    myIsHiddenMain = true;
  }
  else {
    aDisplayer->Display(myMainObj);
    myIsHiddenMain = false;
  }
  aDisplayer->Display(myGroup);
  myDmMode = -1;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes
  initSpinBox(myLessFilterSpin, 0., COORD_MAX, step, "length_precision" );
  initSpinBox(myGreaterFilterSpin, 0., COORD_MAX, step, "length_precision" );
  myLessFilterSpin->setValue( 0. );
  myGreaterFilterSpin->setValue( 0. );

  myDmMode = -1;
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();

  //unset shape type to avoid preparation of selection before exact user shape type selection
  unsetConstructorId();
  myIsShapeType = false;

  if (myMode == CreateGroup) {
    initName(tr("GROUP_PREFIX"));

    // Get ready for main shape selection
    myEditCurrentArgument = myMainName;

    connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));
    connect(mySelBtn,         SIGNAL(clicked()),    this, SLOT(SetEditCurrentArgument()));
    connect(mySelBtn2,        SIGNAL(clicked()),    this, SLOT(SetEditCurrentArgument()));
  }
  else if (myMode == EditGroup) {
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    if (aSelList.Extent()) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First());

      if (!CORBA::is_nil(anObj) && anObj->GetType() == GEOM_GROUP) {
        myGroup = anObj;

        mainFrame()->ResultName->setText(GEOMBase::GetName(myGroup));

        GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());
        myMainObj = anOper->GetMainShape(myGroup);
        if (!CORBA::is_nil(myMainObj)) {
          myMainName->setText(GEOMBase::GetName(myMainObj));
          SALOME_View* view = GEOM_Displayer::GetActiveView();
          if (view) {
            CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
            Handle(SALOME_InteractiveObject) io =
              new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
            if (view->isVisible(io)) myWasHiddenMain = false;
          }
        }

        setShapeType((TopAbs_ShapeEnum)anOper->GetType(myGroup));

        GEOM::ListOfLong_var aCurrList = anOper->GetObjects(myGroup);
        for (int i = 0, n = aCurrList->length(); i < n; i++) {
          QListWidgetItem* itm = new QListWidgetItem( QString( "%1" ).arg( aCurrList[ i ] ) );
          myGroupIdList.append( aCurrList[ i ] );
          itm->setTextColor( QColor( GROUP_IDLST_COLOR ) );
          myIdList->addItem( itm );
        }
        myEditCurrentArgument = 0;
      }
      connect(mySelBtn2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
    }
  }

  connect(aSelMgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myRestrictGroup, SIGNAL(buttonClicked(int)),     this, SLOT(SetEditCurrentArgument()));
  connect(mySelAllBtn,     SIGNAL(clicked()),              this, SLOT(selectAllSubShapes()));
  connect(myAddBtn,        SIGNAL(clicked()),              this, SLOT(add()));
  connect(myRemBtn,        SIGNAL(clicked()),              this, SLOT(remove()));
  connect(myShowOnlyBtn,   SIGNAL(clicked()),              this, SLOT(showOnlySelected()));
  connect(myHideSelBtn,    SIGNAL(clicked()),              this, SLOT(showOnlySelected()));
  connect(myShowAllBtn,    SIGNAL(clicked()),              this, SLOT(showOnlySelected()));
  connect(myIdList,        SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

  connect(myApplyFilterButton, SIGNAL(clicked()),         this, SLOT(ClickOnOkFilter()));
#ifndef DISABLE_PLOT2DVIEWER
  connect(myPlotDistributionButton, SIGNAL(clicked()),    this, SLOT(ClickOnPlot()));
#endif
  connect(myLessFilterCheck,   SIGNAL(stateChanged(int)), this, SLOT(MeasureToggled()));
  connect(myGreaterFilterCheck,   SIGNAL(stateChanged(int)), this, SLOT(MeasureToggled()));

  setInPlaceObj(GEOM::GEOM_Object::_nil());

  myBusy = true; // just activate but do not select in the list
  activateSelection();
  myBusy = false;
  MeasureToggled();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::enterEvent(QEvent* e)
{
  if (!buttonCancel()->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
//function : closeEvent
//purpose  : remove temporary geom object
//=================================================================================
void GroupGUI_GroupDlg::closeEvent(QCloseEvent* e)
{
  setInPlaceObj(GEOM::GEOM_Object::_nil());
  erasePreview(true);

  GEOMBase_Skeleton::closeEvent(e);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
  setIsApplyAndClose(false);
  getDisplayer()->UnsetDisplayMode();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GroupGUI_GroupDlg::ClickOnApply()
{
  if(!isApplyAndClose()) {
    setIsDisableBrowsing( true );
    setIsDisplayResult( false );
  }
    
  myIsAccept = onAccept(myMode == CreateGroup, true, isApplyAndClose());
  if (!myIsAccept)
    return false;

  if(!isApplyAndClose()) {
    setIsDisableBrowsing( false );
    setIsDisplayResult( true );
  }

  if (myMode == CreateGroup)
  {
    initName();
    myIdList->clear();
    ConstructorsClicked(getConstructorId());
  }
  else
  {
    int n = myIdList->count();
    myGroupIdList.clear();
    if (n > 0)
    {
      for (int i = 0; i < n; i++) {
        QListWidgetItem* anItem  = myIdList->item( i );
        myGroupIdList.append( anItem->text().toInt() );
        anItem->setTextColor( GROUP_IDLST_COLOR );
      }
    }
    activateSelection();
  }
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::SetEditCurrentArgument()
{
  QPushButton* send = qobject_cast<QPushButton*>( sender() );

  if (send == mySelBtn) {
    myEditCurrentArgument = myMainName;
    myShape2Name->setText("");
    mySelBtn->setDown(true);
    mySelBtn2->setDown(false);
  }
  else if (send == mySelBtn2 || sender() == myRestrictGroup) {
    setInPlaceObj(GEOM::GEOM_Object::_nil());
    myShape2Name->setText("");
    if ( send == mySelBtn2 ) {
      mySelBtn2->setDown(true);
      mySelBtn->setDown(false);
    }
  if (subSelectionWay() != ALL_SUBSHAPES) {
      myEditCurrentArgument = myShape2Name;
    }
    else {
      myEditCurrentArgument = 0;
    }
  }

  //  activateSelection();
  if(myEditCurrentArgument) {
    myEditCurrentArgument->setFocus();
    if ( send )
      send->setDown(true);
  }

  updateState();
}

//=================================================================================
// function : onGetInPlace()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::onGetInPlace()
{
  setInPlaceObj(GEOM::GEOM_Object::_nil());
  myEditCurrentArgument->setText("");

  bool isBlocked = myIdList->signalsBlocked();
  myIdList->blockSignals(true);
  myIdList->clearSelection();
  myIdList->blockSignals(isBlocked);

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject(aSelList.First());
  if (GEOMBase::IsShape(anObj)) {
    if (!anObj->_is_equivalent(myMainObj) && !anObj->_is_equivalent(myGroup)) {
      SUIT_OverrideCursor wc;
      myEditCurrentArgument->setText(GEOMBase::GetName(anObj));
      GEOM::GEOM_IShapesOperations_var aShapesOp =
        getGeomEngine()->GetIShapesOperations(getStudyId());
      if (subSelectionWay() == GET_IN_PLACE) {
        GEOM::GEOM_Object_var aGetInPlaceObj = aShapesOp->GetInPlace(myMainObj, anObj);
        setInPlaceObj(aGetInPlaceObj);
      }
      else {
        bool isVisible = true;
        if (SALOME_View* view = GEOM_Displayer::GetActiveView())
          isVisible = view->isVisible(aSelList.First());
        setInPlaceObj(anObj, isVisible);
      }
      myEditCurrentArgument = 0;
      //myBusy = true; // just activate but do not select in the list
      activateSelection();
      //myBusy = false;
    }
  }
}

//=================================================================================
//function : setInPlaceObj
//purpose  : temporarily add an object to study and remove old InPlaceObj
//=================================================================================
void GroupGUI_GroupDlg::setInPlaceObj(GEOM::GEOM_Object_var theObj, const bool isVisible)
{
  if (!myInPlaceObj->_is_equivalent(theObj))
  {
    myInPlaceObj = theObj;
  }

  // build map of indices
  myMain2InPlaceIndices.Clear();
  if (!myInPlaceObj->_is_nil()) {
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations(getStudyId());

    GEOM::ListOfGO_var aSubObjects = aShapesOp->MakeExplode(myInPlaceObj, getShapeType(), false);
    for ( int i = 0; i < aSubObjects->length(); i++ )
    {
      GEOM::ListOfLong_var aCurrList = aShapesOp->GetSameIDs( myMainObj, aSubObjects[i] );
      if( aCurrList->length() > 1 ) {
        //rnv : To Fix the 21561: EDF 2184 GEOM: Group with second shape restriction.
        //      In case if GetSameIDs(...) method return more then one ID use 
        //      GetSharedShapes(...) method to get sub-shapes of the second shape.
        GEOM::ListOfGO_var aSubObjects2 = aShapesOp->GetSharedShapes( myMainObj, aSubObjects[i], getShapeType() );
        for( int j = 0; j < aSubObjects2->length(); j++ ) {
          CORBA::Long aMainIndex =  aLocOp->GetSubShapeIndex( myMainObj, aSubObjects2[j] );
          CORBA::Long aPlaceIndex = aLocOp->GetSubShapeIndex( myInPlaceObj, aSubObjects[i]);
          if ( aMainIndex >= 0 && aPlaceIndex > 0 ) {
            myMain2InPlaceIndices.Bind( aMainIndex, aPlaceIndex );
          }
        }
      } else if(aCurrList->length() > 0 ) {
        CORBA::Long aMainIndex = aCurrList[0];
        CORBA::Long aPlaceIndex = aLocOp->GetSubShapeIndex( myInPlaceObj, aSubObjects[i] );
        if ( aMainIndex >= 0 && aPlaceIndex > 0) {
          myMain2InPlaceIndices.Bind( aMainIndex, aPlaceIndex );
        }
      }
    }
  }
  myInPlaceObjSelectState = subSelectionWay();
  if (myInPlaceObjSelectState == SUBSHAPES_OF_SHAPE2 && !isVisible)
    myInPlaceObjSelectState = SUBSHAPES_OF_INVISIBLE_SHAPE2;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GroupGUI_GroupDlg::SelectionIntoArgument()
{
  if (subSelectionWay() != ALL_SUBSHAPES && myEditCurrentArgument == myShape2Name) {
    onGetInPlace();
    if( !myInPlaceObj->_is_nil() ) {
      mySelBtn2->setDown(false);
    }
    return;
  }

  if (myEditCurrentArgument == myMainName) {  // Selection of a main shape is active
    myEditCurrentArgument->setText("");
    myIdList->clear();

    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);
    int nbSel = aSelList.Extent();

    if (nbSel == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First());

      if (GEOMBase::IsShape(anObj)) {
        if (myIsHiddenMain) {
          GEOM_Displayer* aDisplayer = getDisplayer();
          aDisplayer->Display(myMainObj);
          myIsHiddenMain = false;
        }
        myMainObj = anObj;
        if (!CORBA::is_nil(myMainObj)) {
	  mySelBtn->setDown(false);
          SALOME_View* view = GEOM_Displayer::GetActiveView();
          if (view) {
            CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
            Handle(SALOME_InteractiveObject) io =
              new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
            if (view->isVisible(io)) myWasHiddenMain = false;
          }
        }
        myEditCurrentArgument->setText(GEOMBase::GetName(anObj));
        // activate sub-shapes selection by default
        myEditCurrentArgument = 0;
        activateSelection();
        updateState();
      }
    }
    else {
      if (myIsHiddenMain) {
        GEOM_Displayer* aDisplayer = getDisplayer();
        aDisplayer->Display(myMainObj);
        myIsHiddenMain = false;
      }
      myMainObj = GEOM::GEOM_Object::_nil();
    }
  }
  else { // an attempt to synchronize list box selection with 3d viewer
    if (myBusy || myMainObj->_is_nil()) {
      return;
    }

    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals(true);
    myIdList->clearSelection();

    TColStd_IndexedMapOfInteger aMapIndex;
    int nbSel = getSelectedSubshapes(aMapIndex);

    if (nbSel) {
      QMap<int, int> aMap;
      for (int i = 0, n = myIdList->count(); i < n; i++)
        aMap.insert(myIdList->item(i)->text().toInt(), i);

      bool highlight = false;
      for (int ii = 1, nn = aMapIndex.Extent(); ii <= nn; ii++) {
        if (aMap.contains(aMapIndex(ii))) {
          myIdList->item(aMap[aMapIndex(ii)])->setSelected(true);
          highlight = true;
        }
      }
      if (highlight)
        highlightSubShapes();
    }
    myIdList->blockSignals(isBlocked);

    updateState(nbSel);
  }
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GroupGUI_GroupDlg::ConstructorsClicked(int constructorId)
{
  if (getConstructorId() != constructorId)
    setConstructorId(constructorId);

  myIsShapeType = true;
  myIdList->clear();
  myEditCurrentArgument = 0;

  setInPlaceObj(myInPlaceObj); // to rebuild myMain2InPlaceIndices
  activateSelection();
  updateState();
}

//=================================================================================
// function : selectAllSubShapes
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::selectAllSubShapes()
{
  if (CORBA::is_nil(myMainObj) || !myIsShapeType)
    return;

  GEOM::ListOfLong_var aSubShapes;
  GEOM::GEOM_IShapesOperations_var aShOp = getGeomEngine()->GetIShapesOperations(getStudyId());
  aSubShapes = aShOp->SubShapeAllIDs(myMainObj, getShapeType(), false);
  if (aSubShapes->length() > 0) {
    if (subSelectionWay() == ALL_SUBSHAPES)
    {
//      myIdList->clear(); // for sorted final list?

      if (!aShOp->IsDone())
        return;
    }
    else
    {
      aSubShapes = new GEOM::ListOfLong();
      aSubShapes->length(myMain2InPlaceIndices.Extent());
      TColStd_DataMapIteratorOfDataMapOfIntegerInteger m2ip (myMain2InPlaceIndices);
      for (int i = 0; m2ip.More(); i++, m2ip.Next())
        aSubShapes[i] = m2ip.Key();
    }

    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals(true);

    for (int i = 0, n = aSubShapes->length(); i < n; i++) {
      CORBA::Long anIndex = aSubShapes[i];
      if (anIndex < 0)
        continue;

      QListWidgetItem* anItem = 0;
      QString text = QString("%1").arg(anIndex);
//      if (!myInPlaceObj->_is_nil()) {
        QList<QListWidgetItem*> found = myIdList->findItems(text, Qt::MatchExactly);
        if (found.count()) anItem = found[0];
//      }
      if (!anItem) {
        anItem = new QListWidgetItem(text);
        anItem->setTextColor( myGroupIdList.contains(anIndex) ? QColor( GROUP_IDLST_COLOR ) : QColor( GROUP_NEWIDLST_COLOR ) );
        myIdList->addItem(anItem);
      }
      anItem->setSelected(true);
    }

    myIdList->blockSignals(isBlocked);
    highlightSubShapes();
  }
}

//=================================================================================
// function : showOnlySelected
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::showOnlySelected()
{
  if (CORBA::is_nil(myMainObj) || !myIsShapeType)
    return;

  QPushButton* send = (QPushButton*)sender();
  if (send == myShowAllBtn) {
    activateSelection();
    return;
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  GEOM_Displayer* aDisplayer = getDisplayer();

  if (send == myHideSelBtn) {
    aDisplayer->Erase(aSelList, /*forced=*/false, /*updateViewer=*/true);
  }
  else {
    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    SALOME_View* view = GEOM_Displayer::GetActiveView();
    if (view) {
      CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
      Handle(SALOME_InteractiveObject) io =
        new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
      if (view->isVisible(io)) myIsHiddenMain = true;
      
      //keep the selected entry and IO in the map for checking
      std::map<QString, Handle(SALOME_InteractiveObject)> aSelEntriesMap;
      SALOME_ListIteratorOfListIO aSelIt(aSelList);
      for ( ; aSelIt.More(); aSelIt.Next() ) {
        Handle(SALOME_InteractiveObject) anSelIO = aSelIt.Value();
        aSelEntriesMap[anSelIO->getEntry()] = anSelIO;
      }
      //get the displayed sub-shapes
      SALOME_ListIO displayed;
      view->GetVisible(displayed);
      // Erase all, except the selected sub-shapes
      std::map<QString, Handle(SALOME_InteractiveObject)>::iterator 
        aSelDispIter = aSelEntriesMap.end();
      SALOME_ListIteratorOfListIO aDispIt( displayed );
      for ( ; aDispIt.More(); aDispIt.Next() ) {
        Handle(SALOME_InteractiveObject) anIO = aDispIt.Value();
        aSelDispIter = aSelEntriesMap.find( anIO->getEntry() );
        if ( aSelDispIter != aSelEntriesMap.end() ) {
          //sub-shape is selected, so erase it's record from map to keep in it not displayed, but selected sub-shapes only
          aSelEntriesMap.erase(aSelDispIter);
        } else {
          //sub-shape is not in the map of selected, then erase it from view
          aDisplayer->Erase( anIO, /*forced = */false, /*updateViewer = */false );
        }
      }

      if ( !aSelEntriesMap.empty() ) {
        // Build a presentation of the selected, but not displayed sub-shapes
        TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myMainObj);
        TopTools_IndexedMapOfShape aSubShapesMap;
        TopExp::MapShapes(aMainShape, aSubShapesMap);
        QString anEntryBase = aMainEntry.in();

        TopExp_Explorer anExp (aMainShape, getShapeType());
        for (; anExp.More(); anExp.Next()) {
          TopoDS_Shape aSubShape = anExp.Current();
          int index = aSubShapesMap.FindIndex(aSubShape);
          QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);
          if ( aSelEntriesMap.find( anEntry ) == aSelEntriesMap.end() ) {
            //skip not selected sub-shapes
            continue;
          }
          SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, view);
          if (aPrs) {
            displayPreview(aPrs, true, false); // append, do not update
          }
        }
      }
      aDisplayer->UpdateViewer();
    }

    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    if (getShapeType() == TopAbs_VERTEX && myIsHiddenMain) {
      aDisplayer->Display(myMainObj);
    }

    // for the case when selected ids were not displayed in the viewer: Mantis issue 0021367
    highlightSubShapes();
  }
}

//=================================================================================
// function : getSelectedSubshapes
// purpose  :
//=================================================================================
int GroupGUI_GroupDlg::getSelectedSubshapes (TColStd_IndexedMapOfInteger& theMapIndex)
{
  theMapIndex.Clear();

  SalomeApp_Application* app = myGeomGUI->getApp();
  if (!app || myMainObj->_is_nil())
    return 0;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // try to find out and process the global selection
  // (of not published objects and of published sub-shapes)
  {
    SALOME_ListIteratorOfListIO anIter (aSelList);
    for (int i = 0; anIter.More(); anIter.Next(), i++)
    {
      Handle(SALOME_InteractiveObject) anIObj = anIter.Value();
      QString anEntry = anIObj->getEntry();
      QString str = "_";
      int index = anEntry.lastIndexOf(str);
      if (index > 0) // selection among special preview
      {
        anEntry.remove(0, index+1);
        int anIndex = anEntry.toInt();
        if (anIndex)
          theMapIndex.Add(anIndex);
      }
      else // selection among published shapes
      {
        SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
        if (!appStudy) return 0;
        _PTR(Study) aStudy = appStudy->studyDS();

        _PTR(SObject) aSObj (aStudy->FindObjectID(anEntry.toLatin1().constData()));
        GEOM::GEOM_Object_var aGeomObj =
          GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aGeomObj, aShape)) {
          if (aGeomObj->GetType() == GEOM_GROUP || aShape.ShapeType() == getShapeType()) {
            if (subSelectionWay() != ALL_SUBSHAPES &&
                GEOMBase::GetName(aGeomObj) == myShape2Name->text()) {
              // Skip selected in place object.
              continue;
            }

            TopTools_IndexedMapOfShape aMainMap;
            TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myMainObj);
            TopExp::MapShapes(aMainShape, aMainMap);

            TopExp_Explorer anExp (aShape, getShapeType());
            bool isShowWarning = true;
            for (; anExp.More(); anExp.Next()) {
              TopoDS_Shape aSubShape = anExp.Current();
              int anIndex = aMainMap.FindIndex(aSubShape);
              if (anIndex == 0) {
                if (isShowWarning) {
                  SUIT_MessageBox::warning(app->desktop(), QObject::tr("WRN_WARNING"),
                                         tr("WRN_NOT_SUBSHAPE"));
                  isShowWarning = false;
                }
              }
              else {
                if (subSelectionWay() != ALL_SUBSHAPES &&
                    !myMain2InPlaceIndices.IsBound(anIndex))
                  continue;
                theMapIndex.Add(anIndex);
              }
            }
          }
        }
      }
    } // for aSelList
  }

  return theMapIndex.Extent();
}

//=================================================================================
// function : add
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::add()
{
  TColStd_IndexedMapOfInteger aMapIndex;
  int nbSel = getSelectedSubshapes(aMapIndex);

  TColStd_MapOfInteger aMap;
  for (int i = 0, n = myIdList->count(); i < n; i++)
    aMap.Add(myIdList->item(i)->text().toInt());

  if (nbSel > 0) {
    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals(true);

    for (int i = 1, n = aMapIndex.Extent(); i <= n; i++) {
      if (aMap.Contains(aMapIndex(i)))
        continue;

      QListWidgetItem* anItem = new QListWidgetItem(QString("%1").arg(aMapIndex(i)));
      anItem->setTextColor( myGroupIdList.contains( aMapIndex( i ) ) ? QColor( GROUP_IDLST_COLOR ) : QColor( GROUP_NEWIDLST_COLOR ) );
      myIdList->addItem(anItem);
      anItem->setSelected(true);
    }

    myIdList->blockSignals(isBlocked);
  }

  updateState();
}

//=================================================================================
// function : remove
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::remove()
{
  bool isBlocked = myIdList->signalsBlocked();
  myIdList->blockSignals(true);

  QListIterator<QListWidgetItem*> it (myIdList->selectedItems());
  while (it.hasNext())
    delete it.next();

  myIdList->blockSignals(isBlocked);

  highlightSubShapes();
}

//=================================================================================
//function : subSelectionWay
//purpose  :
//=================================================================================
int GroupGUI_GroupDlg::subSelectionWay() const
{
  return myRestrictGroup->checkedId();
}

//=================================================================================
// function : getShapeType()
// purpose  :
//=================================================================================
TopAbs_ShapeEnum GroupGUI_GroupDlg::getShapeType() const
{
  switch (getConstructorId()) {
  case 0:  return TopAbs_VERTEX;
  case 1:  return TopAbs_EDGE;
  case 2:  return TopAbs_FACE;
  case 3:  return TopAbs_SOLID;
  default: return TopAbs_SHAPE;
  }
}

//=================================================================================
// function : setShapeType()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::setShapeType(const TopAbs_ShapeEnum theType)
{
  int anId = 0;
  switch (theType) {
  case TopAbs_VERTEX: anId = 0; break;
  case TopAbs_EDGE:   anId = 1; break;
  case TopAbs_FACE:   anId = 2; break;
  case TopAbs_SOLID:  anId = 3; break;
  }
  setConstructorId(anId);
  if (!myIsShapeType)
  {
    myIsShapeType = true;
    // workaround to avoid set checked button 0
    setConstructorId(anId);
  }
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void GroupGUI_GroupDlg::activateSelection()
{
  bool isApply = ((QPushButton*)sender() == buttonApply());
  if(!isApplyAndClose())
    erasePreview(false);

  // local selection
  if (!myMainObj->_is_nil() &&
      !myEditCurrentArgument &&
      myIsShapeType) // check if shape type is already choosen by user
  {
    GEOM_Displayer* aDisplayer = getDisplayer();

    //display mode for main shape
    if ( myDmMode == -1 ) {
    SALOME_View* view = GEOM_Displayer::GetActiveView();
      if (view) {
        CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
        Handle(SALOME_InteractiveObject) io =
          new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
        if ( view->isVisible( io ) ) {
          Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIOinGEOMAISShape( io, true );
          if(!aSh.IsNull()) {
            myDmMode = aSh->isTopLevel() ? aSh->prevDisplayMode() : aSh->DisplayMode();
          }
        }
        else
          myDmMode = SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "display_mode" );
      }
    }
    aDisplayer->SetDisplayMode(myDmMode);

    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    if(getShapeType() != TopAbs_VERTEX) {
      aDisplayer->Erase(myMainObj, false, false);
      myIsHiddenMain = true;
    }
    else
      aDisplayer->Display(myMainObj);

    aDisplayer->Erase(myGroup, false, false);

    QColor aColor = SUIT_Session::session()->resourceMgr()->colorValue( "Geometry", "editgroup_color" );
    Quantity_NameOfColor aCol = SalomeApp_Tools::color( aColor ).Name();

    if(!isApplyAndClose()) {
      SUIT_ViewWindow* aViewWindow = 0;
      SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
      if (activeStudy)
        aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
      if (aViewWindow == 0) return;

      SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
      if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
          aViewManager->getType() != SVTK_Viewer::Type())
        return;

      SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
      SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
      if (aView == 0) return;

      TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myMainObj);
      TopoDS_Shape aRestrictionShape;

      if (subSelectionWay() == ALL_SUBSHAPES) {
        aRestrictionShape = aMainShape;

        TopTools_IndexedMapOfShape aSubShapesMap;
        TopExp::MapShapes(aMainShape, aSubShapesMap);
        CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
        QString anEntryBase = aMainEntry.in();

        TopExp_Explorer anExp (aRestrictionShape, getShapeType());
        for (; anExp.More(); anExp.Next()) {
          TopoDS_Shape aSubShape = anExp.Current();
          int index = aSubShapesMap.FindIndex(aSubShape);
          QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);
          Handle(SALOME_InteractiveObject) io =
            new SALOME_InteractiveObject(anEntry.toLatin1(), "GEOM", "TEMP_IO");
          if ( myGroupIdList.contains( index ) ) {
            aDisplayer->SetColor( aCol );
          }
          else {
            aDisplayer->UnsetColor();
          }
          SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, aView);
          if (aPrs) {
            displayPreview(aPrs, true, false); // append, do not update
            // TODO: map or delete Prs
          }
        }
      }
      else if (!myInPlaceObj->_is_nil()) {
        TopTools_IndexedMapOfShape aSubShapesMap;
        TopExp::MapShapes(aMainShape, aSubShapesMap);
        CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
        QString anEntryBase = aMainEntry.in();

        TColStd_DataMapIteratorOfDataMapOfIntegerInteger aM2IPit (myMain2InPlaceIndices);
        for (; aM2IPit.More(); aM2IPit.Next()) {
          int index = aM2IPit.Key();
          TopoDS_Shape aSubShape = aSubShapesMap.FindKey(index);
          QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);

          if ( myGroupIdList.contains( index ) ) {
            aDisplayer->SetColor( aCol );
          }
          else {
            aDisplayer->UnsetColor();
          }

          SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, aView);
          if (aPrs) {
            displayPreview(aPrs, true, false); // append, do not update
          }
        }
      }
      else ;
      aDisplayer->UnsetDisplayMode();
      aDisplayer->UnsetColor();
      aDisplayer->UpdateViewer();
    }
  }

  globalSelection(GEOM_ALLSHAPES);

  SelectionIntoArgument();
}

//=================================================================================
// function : updateState
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::updateState (bool isAdd)
{
  myAddBtn->setEnabled(!myEditCurrentArgument && !CORBA::is_nil(myMainObj) && isAdd);
  //myShowOnlyBtn->setEnabled(!myEditCurrentArgument && !CORBA::is_nil(myMainObj) && isAdd);

  bool hasSel = myIdList->selectedItems().count() > 0;

  myRemBtn->setEnabled(hasSel);
  myRestrictGroupBox->setEnabled(!CORBA::is_nil(myMainObj));
  mySelAllBtn->setEnabled(!CORBA::is_nil(myMainObj));
  
  mySelBtn2->setEnabled(subSelectionWay() != ALL_SUBSHAPES);
  myShape2Name->setEnabled(subSelectionWay() != ALL_SUBSHAPES);
  myFilterGrp->setEnabled(!CORBA::is_nil(myMainObj) &&
                          subSelectionWay() == ALL_SUBSHAPES &&
                          myIsShapeType &&
                          getShapeType() != TopAbs_VERTEX);
  // manage of 'Plot' button access
  GEOM::GEOM_IShapesOperations_var aShOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
  GEOM::ListOfLong_var aSubShapes = aShOp->SubShapeAllIDs( myMainObj, getShapeType(), false );
  bool hasCurrentEntities = aSubShapes->length() > 0;
#ifndef DISABLE_PLOT2DVIEWER
  myPlotDistributionButton->setEnabled( myFilterGrp->isEnabled() &&
					myIsShapeType &&
					( getShapeType() == TopAbs_EDGE || 
					  getShapeType() == TopAbs_FACE ||
					  getShapeType() == TopAbs_SOLID ) &&
					hasCurrentEntities );
#endif
  if (subSelectionWay() == ALL_SUBSHAPES)
    setInPlaceObj(GEOM::GEOM_Object::_nil());
}

//=================================================================================
// function : selectionChanged
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::selectionChanged()
{
  highlightSubShapes();
}

//=================================================================================
// function : highlightSubShapes
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::highlightSubShapes()
{
  if (CORBA::is_nil(myMainObj))
    return;

  TColStd_MapOfInteger anIds;

  myBusy = true;

  int ii = 0, nn = myIdList->count();
  for (; ii < nn; ii++)
  {
    if (myIdList->item(ii)->isSelected()) {
      int id = myIdList->item(ii)->text().toInt();
      if (subSelectionWay() != ALL_SUBSHAPES &&
          !myMain2InPlaceIndices.IsBound(id)) {
        //myIdList->item(ii)->setSelected(false);
      }
      else {
        anIds.Add(id);
      }
    }
  }
  SalomeApp_Application* app = myGeomGUI->getApp();
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  aSelMgr->clearSelected();

  SUIT_ViewWindow* aViewWindow = 0;
  SUIT_Study* activeStudy = app->activeStudy();
  if (activeStudy)
    aViewWindow = app->desktop()->activeWindow();
  if (aViewWindow == 0) return;

  SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
  if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
      aViewManager->getType() != SVTK_Viewer::Type())
    return;

  SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
  SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
  if (aView == 0) return;

  // TODO??: use here GEOMBase_Helper::myPreview instead of ic->DisplayedObjects()

  OCCViewer_Viewer* v3d = ((OCCViewer_ViewManager*)aViewManager)->getOCCViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();
  AIS_ListOfInteractive List;
  //ic->DisplayedObjects(List);
  ic->ObjectsInside(List); // Mantis issue 0021367

  SALOME_ListIO aSelList;

  // To highlight the selected sub-shape in Object Browser, if it's already published under the main shape
  GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations(getStudyId());
  QMap<int, QString> childsMap;
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  if (appStudy) {
    _PTR(Study) aStudy = appStudy->studyDS();
    CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
    QString anEntry = aMainEntry.in();
    _PTR(SObject) aSObj (aStudy->FindObjectID(anEntry.toLatin1().constData()));
    _PTR(ChildIterator) anIt (aStudy->NewChildIterator(aSObj));
    for (anIt->InitEx(true); anIt->More(); anIt->Next()) {
      GEOM::GEOM_Object_var aChild = GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(anIt->Value()));
      if (!CORBA::is_nil(aChild)) {
        int index = aLocOp->GetSubShapeIndex(myMainObj, aChild);
        CORBA::String_var aChildEntry = aChild->GetStudyEntry();
        QString anEntry = aChildEntry.in();
        childsMap.insert(index, anEntry);
      }
    }
  }

  AIS_ListIteratorOfListOfInteractive ite (List);
  for (; ite.More(); ite.Next()) {
    if (ite.Value()->IsInstance(STANDARD_TYPE(GEOM_AISShape))) {
      Handle(GEOM_AISShape) aSh = Handle(GEOM_AISShape)::DownCast(ite.Value());
      if (aSh->hasIO()) {
        Handle(SALOME_InteractiveObject) anIO = aSh->getIO();
        QString anEntry = anIO->getEntry();
        int index = anEntry.lastIndexOf("_");
        anEntry.remove(0, index+1);
        int anIndex = anEntry.toInt();
        if (anIds.Contains(anIndex)) {
          aSelList.Append(anIO);
          if (childsMap.contains (anIndex)) {
            Handle(SALOME_InteractiveObject) tmpIO = new SALOME_InteractiveObject(childsMap.value(anIndex).toLatin1().constData(), "GEOM", "TEMP_IO");
            aSelList.Append(tmpIO);
          }
        }
      }
    }
  }
  aSelMgr->setSelectedObjects(aSelList);

  myBusy = false;

  if (nn < 3000)
    updateState(aSelList.Extent() > 0);
  else {
    myAddBtn->setEnabled(true);
    myAddBtn->setEnabled(true);
    myRemBtn->setEnabled(true);
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GroupGUI_GroupDlg::createOperation()
{
  return getGeomEngine()->GetIGroupOperations(getStudyId());
}

#define RETURN_WITH_MSG(a, b) \
  if (!(a)) { \
    theMessage += (b); \
    return false; \
  }

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool GroupGUI_GroupDlg::isValid(QString& theMessage)
{
  SalomeApp_Study* study = getStudy();
  ASSERT(study);
  RETURN_WITH_MSG  (!study->studyDS()->GetProperties()->IsLocked(), tr("GEOM_STUDY_LOCKED"))

  if (myMode == CreateGroup) {
    RETURN_WITH_MSG(!CORBA::is_nil(myMainObj), tr("NO_MAIN_OBJ"))
  }
  else {
    RETURN_WITH_MSG(!CORBA::is_nil(myMainObj), tr("NO_GROUP"))
  }

  QString aName (getNewObjectName());
  RETURN_WITH_MSG  (!aName.trimmed().isEmpty(), tr("EMPTY_NAME"))

  RETURN_WITH_MSG  (myIdList->count(), tr("EMPTY_LIST"))
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GroupGUI_GroupDlg::execute(ObjectList& objects)
{
  GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());

  GEOM::GEOM_Object_var aGroup;
  if (myMode == CreateGroup)
    aGroup = anOper->CreateGroup(myMainObj, getShapeType());
  else if (myMode == EditGroup)
    aGroup = myGroup;

  if (CORBA::is_nil(aGroup) || (myMode == CreateGroup && !anOper->IsDone()))
    return false;

  GEOM::ListOfLong_var aCurrList = anOper->GetObjects(aGroup);
  if (!anOper->IsDone())
    return false;

  if (aCurrList->length() > 0)
  {
    anOper->DifferenceIDs(aGroup, aCurrList);
    if (!anOper->IsDone())
      return false;
  }

  int ii, nn = myIdList->count();
  if (nn > 0)
  {
    GEOM::ListOfLong_var aNewList = new GEOM::ListOfLong;
    aNewList->length(nn);
    for (ii = 0; ii < nn; ii++) {
      aNewList[ii] = myIdList->item(ii)->text().toInt();
    }
    anOper->UnionIDs(aGroup, aNewList);
    if (!anOper->IsDone())
      return false;
  }

  SalomeApp_Study* study = getStudy();
  if (study) {
    QString objIOR = GEOMBase::GetIORFromObject(aGroup);
    if (objIOR != "") {
      _PTR(SObject) SO (study->studyDS()->FindObjectIOR(objIOR.toLatin1().constData()));
      if (SO) {
        _PTR(StudyBuilder) aBuilder (study->studyDS()->NewBuilder());
        aBuilder->SetName(SO, getNewObjectName().toLatin1().constData());
      }
    }
  }

  objects.push_back(aGroup._retn());

  return true;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr GroupGUI_GroupDlg::getFather(GEOM::GEOM_Object_ptr theObj)
{
  GEOM::GEOM_Object_var aFatherObj;
  if (theObj->GetType() == GEOM_GROUP) {
    GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());
    aFatherObj = anOper->GetMainShape(theObj);
  }
  return aFatherObj._retn();
}

void GroupGUI_GroupDlg::ClickOnOkFilter()
{
  if (CORBA::is_nil(myMainObj) || subSelectionWay() != ALL_SUBSHAPES || !myIsShapeType || getShapeType() == TopAbs_VERTEX)
    return;
  
  TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myMainObj);
  TopTools_IndexedMapOfShape aSubShapesMap;
  TopExp::MapShapes(aMainShape, aSubShapesMap);
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  getDisplayer()->Erase(myMainObj, false, false);
  CORBA::String_var aMainEntry = myMainObj->GetStudyEntry();
  QString anEntryBase = aMainEntry.in();

  SALOME_ListIO toSelect;

  TopExp_Explorer anExp (aMainShape, (TopAbs_ShapeEnum)getShapeType());
  for (; anExp.More(); anExp.Next())
  {
    TopoDS_Shape aSubShape = anExp.Current();
    int index = aSubShapesMap.FindIndex(aSubShape);
    QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);
    if ( !getDisplayer()->IsDisplayed( anEntry ) )
      continue;

    double factor = GEOMUtils::ShapeToDouble(aSubShape).second;
    double v1 = myLessFilterSpin->value();
    double v2 = myGreaterFilterSpin->value();
    bool isLess = myLessFilterCombo->itemData(myLessFilterCombo->currentIndex()).toInt() == Filter_LT ? factor < v1 : factor <= v1;
    bool isGreater = myGreaterFilterCombo->itemData(myGreaterFilterCombo->currentIndex()).toInt() == Filter_GT ? factor > v2 : factor >= v2;
    if ( ( myLessFilterCheck->isChecked() && myGreaterFilterCheck->isChecked() && isLess && isGreater ) ||
         ( myLessFilterCheck->isChecked() && !myGreaterFilterCheck->isChecked() && isLess ) ||
         ( myGreaterFilterCheck->isChecked() && !myLessFilterCheck->isChecked() && isGreater ) ) {
      Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject();
      io->setEntry( anEntry.toLatin1().constData() );
      toSelect.Append(io);
    }
  }
  if ( toSelect.Extent() > 0 ) {
    myGeomGUI->getApp()->selectionMgr()->setSelectedObjects(toSelect);
    SUIT_MessageBox::information( this,
                                  tr( "INF_INFO" ),
                                  tr( "GEOM_SOME_SHAPES_SELECTED").arg( toSelect.Extent() ),
                                  tr( "BUT_OK" ) );
  }
  else {
    SUIT_MessageBox::information( this,
                                  tr( "INF_INFO" ),
                                  tr( "GEOM_NO_SHAPES_SELECTED" ),
                                  tr( "BUT_OK" ) );
  }
  updateState(true);
}

#ifndef DISABLE_PLOT2DVIEWER
//=================================================================================
// function : ClickOnPlot()
// purpose  : opens "Shape Statistics" dialog box in order to plot sub-shapes distribution.
//=================================================================================
void GroupGUI_GroupDlg::ClickOnPlot()
{
  TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myMainObj);
  QDialog* dlg = new MeasureGUI_ShapeStatisticsDlg( this, aMainShape, getShapeType() );
  if ( dlg ) {
    dlg->show();
  }
}
#endif

void GroupGUI_GroupDlg::MeasureToggled()
{
  myLessFilterSpin->setEnabled(myLessFilterCheck->isChecked());
  myLessFilterCombo->setEnabled(myLessFilterCheck->isChecked());
  myGreaterFilterSpin->setEnabled(myGreaterFilterCheck->isChecked());
  myGreaterFilterCombo->setEnabled(myGreaterFilterCheck->isChecked());
  myApplyFilterButton->setEnabled(myLessFilterCheck->isChecked() || myGreaterFilterCheck->isChecked());
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GroupGUI_GroupDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr1(myMainObj), aGeomObjPtr2(myInPlaceObj);
  res << aGeomObjPtr1 << aGeomObjPtr2;
  return res;
}
