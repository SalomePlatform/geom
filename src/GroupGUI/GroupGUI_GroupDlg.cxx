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
// File   : GroupGUI_GroupDlg.cxx
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//
#include "GroupGUI_GroupDlg.h"

#include <LightApp_SelectionMgr.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Displayer.h>

#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QMap>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>

enum { ALL_SUBSHAPES = 0, GET_IN_PLACE, SUBSHAPES_OF_SHAPE2, SUBSHAPES_OF_INVISIBLE_SHAPE2 };

GroupGUI_GroupDlg::GroupGUI_GroupDlg( Mode mode, GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false ),
    myMode( mode ),
    myBusy( false )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap image0     ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_VERTEX" ) ) );
  QPixmap image1     ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_EDGE" ) ) );
  QPixmap image2     ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_FACE" ) ) );
  QPixmap image3     ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_SOLID" ) ) );
  QPixmap iconSelect ( resMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( myMode == CreateGroup ? tr( "CREATE_GROUP_TITLE" ) : tr( "EDIT_GROUP_TITLE" ) );

  // Shape type button group
  mainFrame()->GroupConstructors->setEnabled( myMode == CreateGroup );
  mainFrame()->GroupConstructors->setTitle( tr( "SHAPE_TYPE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setIcon( image2 );
  mainFrame()->RadioButton4->setIcon( image3 );
  mainFrame()->RadioButton4->show();

  // Group name
  mainFrame()->GroupBoxName->setTitle( tr( "GROUP_NAME" ) );

  // Main shape and sub-shapes
  QGroupBox* GroupMedium = new QGroupBox( tr( "MAIN_SUB_SHAPES" ), centralWidget() );
  QGridLayout* aMedLayout = new QGridLayout( GroupMedium );
  aMedLayout->setMargin( 9 );
  aMedLayout->setSpacing( 6 );
  
  QLabel* aMainLabel = new QLabel( tr( "MAIN_SHAPE" ), GroupMedium );

  mySelBtn = new QPushButton( GroupMedium );
  mySelBtn->setIcon( iconSelect );
  mySelBtn->setEnabled( myMode == CreateGroup );

  myMainName = new QLineEdit( GroupMedium );
  myMainName->setReadOnly( true );
  myMainName->setEnabled( myMode == CreateGroup );

  QLabel* aSecondLabel = new QLabel( tr( "SECOND_SHAPE" ), GroupMedium );

  mySelBtn2 = new QPushButton( GroupMedium );
  mySelBtn2->setIcon( iconSelect );
  mySelBtn2->setEnabled( false );

  myShape2Name = new QLineEdit( GroupMedium );
  myShape2Name->setReadOnly( true );
  myShape2Name->setEnabled( false );

  mySelectionWayGroupBox = new QGroupBox( tr("SHAPE_SEL_RESTR"), GroupMedium );
  mySelectionWayGroup = new QButtonGroup( mySelectionWayGroupBox );
  QRadioButton* allSubs     = new QRadioButton( tr( "NO_RESTR" )            , mySelectionWayGroupBox );
  QRadioButton* inPlaceSubs = new QRadioButton( tr( "GEOM_PARTS_OF_SHAPE2" ), mySelectionWayGroupBox );
  QRadioButton* shape2Subs  = new QRadioButton( tr( "SUBSHAPES_OF_SHAPE2" ) , mySelectionWayGroupBox );
  QVBoxLayout* mySelWayLayout = new QVBoxLayout( mySelectionWayGroupBox );
  mySelWayLayout->setMargin( 9 );
  mySelWayLayout->setSpacing( 6 );
  mySelWayLayout->addWidget( allSubs );
  mySelWayLayout->addWidget( inPlaceSubs );
  mySelWayLayout->addWidget( shape2Subs );
  mySelectionWayGroup->addButton( allSubs,     ALL_SUBSHAPES );
  mySelectionWayGroup->addButton( inPlaceSubs, GET_IN_PLACE );
  mySelectionWayGroup->addButton( shape2Subs,  SUBSHAPES_OF_SHAPE2 );
  allSubs->setChecked( true );

  mySelAllBtn = new QPushButton( tr( "SELECT_ALL" ), GroupMedium );
  myAddBtn    = new QPushButton( tr( "ADD" ), GroupMedium );
  myRemBtn    = new QPushButton( tr( "REMOVE" ), GroupMedium );
  myIdList    = new QListWidget( GroupMedium );

  myIdList->setSelectionMode( QAbstractItemView::ExtendedSelection );
  myIdList->setFlow( QListView::TopToBottom );
  myIdList->setWrapping( true );

  aMedLayout->addWidget( aMainLabel,             0, 0 );
  aMedLayout->addWidget( mySelBtn,               0, 1 );
  aMedLayout->addWidget( myMainName,             0, 2, 1, 2 );
  aMedLayout->addWidget( aSecondLabel,           1, 0 );
  aMedLayout->addWidget( mySelBtn2,              1, 1 );
  aMedLayout->addWidget( myShape2Name,           1, 2, 1, 2 );
  aMedLayout->addWidget( mySelectionWayGroupBox, 2, 0, 3, 3 );
  aMedLayout->addWidget( mySelAllBtn,            2, 3 );
  aMedLayout->addWidget( myAddBtn,               3, 3 );
  aMedLayout->addWidget( myRemBtn,               4, 3 );
  aMedLayout->addWidget( myIdList,               5, 0, 1, 4 );
  aMedLayout->setRowStretch( 5, 1 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupMedium );

  setHelpFileName( "work_with_groups_page.html" );

  Init();
}

GroupGUI_GroupDlg::~GroupGUI_GroupDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::Init()
{
  // san -- TODO: clear selected sub-shapes...
  //mySelSubBtn->setChecked( true );
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();

  if ( myMode == CreateGroup ) {
    initName( tr( "GROUP_PREFIX" ) );

    // Get ready for main shape selection
    myEditCurrentArgument = myMainName;

    connect( this, SIGNAL( constructorsClicked( int ) ),  this, SLOT( ConstructorsClicked( int ) ) );
    connect( mySelBtn,          SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
    connect( mySelBtn2,         SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  }
  else if (myMode == EditGroup) {
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    if (aSelList.Extent()) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aResult);

      if ( aResult && !CORBA::is_nil( anObj ) && anObj->GetType() == GEOM_GROUP ) {
        myGroup = anObj;

        mainFrame()->ResultName->setText( GEOMBase::GetName( myGroup ) );

        GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow( getOperation() );
        myMainObj = anOp->GetMainShape( myGroup );
        if ( !CORBA::is_nil( myMainObj ) )
          myMainName->setText( GEOMBase::GetName( myMainObj ) );

        setShapeType( (TopAbs_ShapeEnum)anOp->GetType( myGroup ) );

        GEOM::ListOfLong_var aCurrList = anOp->GetObjects( myGroup );
        for ( int i = 0, n = aCurrList->length(); i < n; i++ )
          myIdList->addItem( new QListWidgetItem( QString( "%1" ).arg( aCurrList[i] ) ) );

        myEditCurrentArgument = 0;
      }
      connect( mySelBtn2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
    }
  }

  connect( aSelMgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()) );

  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( mySelectionWayGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( SetEditCurrentArgument() ) );
  connect( mySelAllBtn,         SIGNAL( clicked() ),            this, SLOT( SetEditCurrentArgument() ) );
  connect( myAddBtn,            SIGNAL( clicked() ),            this, SLOT( add() ) );
  connect( myRemBtn,            SIGNAL( clicked() ),            this, SLOT( remove() ) );
  connect( myIdList,            SIGNAL( selectionChanged() ),   this, SLOT( selectionChanged() ) );

  setInPlaceObj(GEOM::GEOM_Object::_nil());

  myBusy = true; // just activate but do not select in the list
  activateSelection();
  myBusy = false;
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::enterEvent( QEvent* e )
{
  if ( !buttonCancel()->isEnabled() )
    ActivateThisDialog();
}

//=======================================================================
//function : closeEvent
//purpose  : remove temporary geom object
//=======================================================================

void GroupGUI_GroupDlg::closeEvent( QCloseEvent* e )
{
  setInPlaceObj( GEOM::GEOM_Object::_nil() );

  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GroupGUI_GroupDlg::ClickOnApply()
{
  if ( !onAccept( myMode == CreateGroup, true ) )
    return false;

  if ( myMode == CreateGroup )
    {
      initName();
      myIdList->clear();
      ConstructorsClicked( getConstructorId() );
    }
  else
    activateSelection();
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::LineEditReturnPressed()
{
  QLineEdit* send = ( QLineEdit* )sender();

  if ( send == myMainName && !myEditCurrentArgument ) {
    myEditCurrentArgument = myMainName;
    activateSelection();
  }
  else
    GEOMBase_Skeleton::LineEditReturnPressed();

  updateState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == mySelBtn ) {
    myEditCurrentArgument = myMainName;
    myShape2Name->setText( "" );
  }
  else if ( /*(QRadioButton*)sender() == mySelSubBtn ||*/ send == mySelAllBtn )
    myEditCurrentArgument = 0;
  else if ( send == mySelBtn2 || sender() == mySelectionWayGroup ) {
    setInPlaceObj( GEOM::GEOM_Object::_nil() );
    //if ( myPlaceCheckBox->isChecked() )
    myShape2Name->setText( "" );
    if ( subSelectionWay() != ALL_SUBSHAPES )
    {
      myEditCurrentArgument = myShape2Name;
    }
    else {
      //myEditCurrentArgument = myMainName;
      myEditCurrentArgument = 0;
    }
  }

  activateSelection();

  if ( send == mySelAllBtn ) {
//     myShape2Name->setText( "" );
//     myPlaceCheckBox->setChecked( false );
//     mySelBtn2->setEnabled( false );
//     myShape2Name->setEnabled( false );
    selectAllSubShapes();
  }
  else
    updateState();
}

//=================================================================================
// function : onGetInPlace()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::onGetInPlace()
{
  setInPlaceObj( GEOM::GEOM_Object::_nil() );
  myEditCurrentArgument->setText( "" );

  bool isBlocked = myIdList->signalsBlocked();
  myIdList->blockSignals( true );
  myIdList->clearSelection();
  myIdList->blockSignals( isBlocked );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  Standard_Boolean aResult = Standard_False;
  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aResult);
  if (aResult && !anObj->_is_nil() && GEOMBase::IsShape(anObj)) {
    if (!anObj->_is_equivalent(myMainObj) && !anObj->_is_equivalent(myGroup)) {
      SUIT_OverrideCursor wc;
      myEditCurrentArgument->setText(GEOMBase::GetName(anObj));
      GEOM::GEOM_IShapesOperations_var aShapesOp =
        getGeomEngine()->GetIShapesOperations(getStudyId());
      if ( subSelectionWay() == GET_IN_PLACE ) {
        GEOM::GEOM_Object_var aGetInPlaceObj = aShapesOp->GetInPlace(myMainObj, anObj);
        setInPlaceObj( aGetInPlaceObj );
      }
      else {
        bool isVisible = true;
        if ( SALOME_View* view = GEOM_Displayer::GetActiveView() )
          isVisible = view->isVisible( aSelList.First() );
        setInPlaceObj( anObj, isVisible );
      }
      myEditCurrentArgument = 0;
      //myBusy = true; // just activate but do not select in the list
      activateSelection();
      //myBusy = false;
    }
  }
}

//=======================================================================
//function : setInPlaceObj
//purpose  : temporarily add an object to study and remove old InPlaceObj
//=======================================================================

void GroupGUI_GroupDlg::setInPlaceObj( GEOM::GEOM_Object_var theObj, const bool isVisible )
{
  if ( ! myInPlaceObj->_is_equivalent( theObj ) )
  {
    const char* tmpName = "__InPlaceObj__";
    // remove old InPlaceObj
    if ( !myInPlaceObj->_is_nil() ) {
      if ( myInPlaceObjSelectState == GET_IN_PLACE ||
           myInPlaceObjSelectState == SUBSHAPES_OF_INVISIBLE_SHAPE2 ) {
        // hide temporary object or initially invisible shape 2 (issue 0014047)
        GEOM_Displayer aDisplayer(getStudy());
        aDisplayer.Erase( myInPlaceObj, true );
      }
      if (_PTR(SObject) SO = getStudy()->studyDS()->FindObject( tmpName )) {
        getStudy()->studyDS()->NewBuilder()->RemoveObjectWithChildren( SO );
        getGeomEngine()->RemoveObject(myInPlaceObj);
      }
    }
    // publish InPlaceObj to enable localSelection(InPlaceObj)
    if ( !theObj->_is_nil() && subSelectionWay() == GET_IN_PLACE ) {
      SALOMEDS::Study_var aStudyDS = GeometryGUI::ClientStudyToStudy(getStudy()->studyDS());
      SALOMEDS::SObject_var aSO =
        getGeomEngine()->AddInStudy(aStudyDS, theObj, tmpName, myMainObj);
    }
    myInPlaceObj = theObj;
  }
  // build map of indices
  myMain2InPlaceIndices.Clear();
  if ( !myInPlaceObj->_is_nil() ) {
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );

    GEOM::ListOfGO_var aSubObjects = aShapesOp->MakeExplode( myInPlaceObj, getShapeType(), false);
    for (int i = 0; i < aSubObjects->length(); i++)
    {
      CORBA::Long aMainIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
      CORBA::Long aPlaceIndex = aLocOp->GetSubShapeIndex( myInPlaceObj, aSubObjects[i] );
      if ( aMainIndex >= 0 && aPlaceIndex > 0)
        myMain2InPlaceIndices.Bind( aMainIndex, aPlaceIndex );
    }
  }
  myInPlaceObjSelectState = subSelectionWay();
  if ( myInPlaceObjSelectState == SUBSHAPES_OF_SHAPE2 && !isVisible )
    myInPlaceObjSelectState = SUBSHAPES_OF_INVISIBLE_SHAPE2;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GroupGUI_GroupDlg::SelectionIntoArgument()
{
//   if (myPlaceCheckBox->isChecked() && myEditCurrentArgument == myShape2Name )
  if (subSelectionWay() != ALL_SUBSHAPES && myEditCurrentArgument == myShape2Name) {
    onGetInPlace();
    return;
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (myEditCurrentArgument == myMainName) {  // Selection of a main shape is active
    myEditCurrentArgument->setText("");
    myIdList->clear();

    if (aSelList.Extent() == 1) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
	GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aResult);

      if ( aResult && !anObj->_is_nil() && GEOMBase::IsShape( anObj ) ) {
	myMainObj = anObj;
	myEditCurrentArgument->setText( GEOMBase::GetName( anObj ) );
	// activate subshapes selection by default
	myEditCurrentArgument = 0;
	activateSelection();
	updateState();
	return;
      }
    }

    myMainObj = GEOM::GEOM_Object::_nil();
  }
  else { // an attempt to synchronize list box selection with 3d viewer
    if ( myBusy ) {
      return;
    }

    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals( true );
    myIdList->clearSelection();

    TColStd_IndexedMapOfInteger aMapIndex;

    LightApp_SelectionMgr::MapEntryOfMapOfInteger aMap;
    aSelMgr->selectedSubOwners( aMap );
    if ( aMap.Size() == 1 )
      aMapIndex = LightApp_SelectionMgr::MapEntryOfMapOfInteger::Iterator( aMap ).Value();
    bool subselected = aMapIndex.Extent();

    // convert inPlace indices to main indices
    //if ( subselected && myPlaceCheckBox->isChecked() )
    if ( subselected && subSelectionWay() != ALL_SUBSHAPES )
    {
      TColStd_IndexedMapOfInteger aMapIndex2;
      
      TColStd_DataMapIteratorOfDataMapOfIntegerInteger m2ip( myMain2InPlaceIndices );
      for ( ; m2ip.More(); m2ip.Next() ) {
        int inPlaceId = m2ip.Value();
        if ( aMapIndex.Contains( inPlaceId )) {
          aMapIndex2.Add( m2ip.Key() );
        }
      }
      aMapIndex = aMapIndex2;
    }

    // try to find out and process the object browser selection
    if ( !subselected ) {
      globalSelection( GEOM_ALLSHAPES );

      GEOM::ListOfGO anObjects;
      GEOMBase::ConvertListOfIOInListOfGO(aSelList, anObjects);

      GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
      GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

      for (int i = 0; i < anObjects.length(); i++)
      {
        GEOM::GEOM_Object_var aGeomObj = anObjects[i];
        GEOM::ListOfGO_var aSubObjects = new GEOM::ListOfGO();
        TopoDS_Shape aShape;
        if ( GEOMBase::GetShape(aGeomObj, aShape, getShapeType()) )
        {
          aSubObjects->length(1);
          aSubObjects[0] = aGeomObj;
        }
        else if (aGeomObj->GetType() == GEOM_GROUP)
          aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false);
        else
          continue;

        for (int i = 0; i < aSubObjects->length(); i++)
        {
          TopoDS_Shape aShape;
          if ( GEOMBase::GetShape(aSubObjects[i], aShape, getShapeType()) )
          {
	    CORBA::Long anIndex;
	    anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
            if ( anIndex >= 0 ) {
              //if ( myPlaceCheckBox->isChecked() && ! myMain2InPlaceIndices.IsBound( anIndex ))
              if ( subSelectionWay() != ALL_SUBSHAPES &&
                   ! myMain2InPlaceIndices.IsBound( anIndex ))
                continue;
              aMapIndex.Add( anIndex );
            }
          }
        }
      }
      if ( !myMainObj->_is_nil() /*&& mySelSubBtn->isChecked()*/)
        if ( subSelectionWay() == ALL_SUBSHAPES )
          localSelection( myMainObj, getShapeType() );
        else if ( !myInPlaceObj->_is_nil() )
          localSelection( myInPlaceObj, getShapeType() );
    }

    if (aMapIndex.Extent() >= 1) {
      QMap<int, int> aMap;
      for ( int i = 0, n = myIdList->count(); i < n; i++ )
        aMap.insert( myIdList->item( i )->text().toInt(), i );

      for ( int ii = 1, nn = aMapIndex.Extent(); ii <= nn; ii++ ) {
        if ( aMap.contains( aMapIndex( ii ) ) )
          myIdList->item( aMap[aMapIndex( ii )])->setSelected( true );
      }
    }
    myIdList->blockSignals( isBlocked );
  }

  updateState();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GroupGUI_GroupDlg::ConstructorsClicked( int constructorId )
{
  myIdList->clear();
  activateSelection();
  updateState();
  setInPlaceObj( myInPlaceObj ); // to rebuild myMain2InPlaceIndices
}

//=================================================================================
// function : selectAllSubShapes
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::selectAllSubShapes()
{
  if ( CORBA::is_nil( myMainObj ) )
    return;

  GEOM::ListOfLong_var aSubShapes;
//   if ( !myPlaceCheckBox->isChecked() )
  if ( subSelectionWay() == ALL_SUBSHAPES )
  {
    myIdList->clear();
    GEOM::GEOM_IShapesOperations_var aShOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
    aSubShapes = aShOp->SubShapeAllIDs(myMainObj, getShapeType(), false);

    if ( !aShOp->IsDone() )
      return;
  }
  else
  {
    aSubShapes = new GEOM::ListOfLong();
    aSubShapes->length( myMain2InPlaceIndices.Extent() );
    TColStd_DataMapIteratorOfDataMapOfIntegerInteger m2ip( myMain2InPlaceIndices );
    for ( int i = 0; m2ip.More(); i++, m2ip.Next() )
      aSubShapes[ i ] = m2ip.Key();
  }
  bool isBlocked = myIdList->signalsBlocked();
  myIdList->blockSignals( true );

  for ( int i = 0, n = aSubShapes->length(); i < n; i++ ) {
    CORBA::Long anIndex = aSubShapes[i];
    if ( anIndex < 0 )
      continue;

    QListWidgetItem* anItem = 0;
    QString text = QString( "%1" ).arg( anIndex );
    if ( !myInPlaceObj->_is_nil() ) {
      QList<QListWidgetItem*> found = myIdList->findItems( text, Qt::MatchExactly );
      if ( found.count() ) anItem = found[0];
    }
    if ( !anItem ) {
      anItem = new QListWidgetItem( text );
      myIdList->addItem( anItem );
    }
    anItem->setSelected( true );
  }

  myIdList->blockSignals( isBlocked );
  highlightSubShapes();
}

//=================================================================================
// function : add
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::add()
{
  TColStd_MapOfInteger aMap;
  for ( int i = 0, n = myIdList->count(); i < n; i++ )
    aMap.Add( myIdList->item( i )->text().toInt() );

  TColStd_IndexedMapOfInteger aMapIndex;

  SALOME_ListIO aSelIOs;
  SalomeApp_Application* app = myGeomGUI->getApp();
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if ( aSelMgr ) {
      LightApp_SelectionMgr::MapEntryOfMapOfInteger aMap;
      aSelMgr->selectedSubOwners( aMap );
      if ( aMap.Size() == 1 )
        aMapIndex = LightApp_SelectionMgr::MapEntryOfMapOfInteger::Iterator( aMap ).Value();
    }
  }
  GEOM::ListOfGO anObjects;
  // get selected sub-shapes of myInPlaceObj
  if ( aMapIndex.Extent() > 0 && !myInPlaceObj->_is_nil() )
  {
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

    anObjects.length( aMapIndex.Extent() );
    for ( int i = 1; i <= aMapIndex.Extent(); i++ )
      anObjects[ i-1 ] = aShapesOp->GetSubShape( myInPlaceObj, aMapIndex( i ));

    aMapIndex.Clear();
  }

  // try to find out and process the object browser selection or InPlace sub-shapes
  if ( !aMapIndex.Extent() )
  {
    if ( anObjects.length() == 0 )
      GEOMBase::ConvertListOfIOInListOfGO(aSelIOs, anObjects);

    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

    for (int i = 0; i < anObjects.length(); i++)
    {
      GEOM::GEOM_Object_var aGeomObj = anObjects[i];
      GEOM::ListOfGO_var aSubObjects  = new GEOM::ListOfGO();
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape(aGeomObj, aShape, getShapeType()) )
      {
        aSubObjects->length(1);
        aSubObjects[0] = aGeomObj;
      }
      else if (aGeomObj->GetType() == GEOM_GROUP)
        aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false);
      else
        break;

      for (int i = 0; i < aSubObjects->length(); i++)
      {
        TopoDS_Shape aShape;
        if ( GEOMBase::GetShape(aSubObjects[i], aShape, getShapeType()) )
        {
	  CORBA::Long anIndex;
	    anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
          if ( anIndex >= 0 )
            aMapIndex.Add( anIndex );
        }
      }
    }
  }

  if ( aMapIndex.Extent() >= 1 ) {
    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals( true );

    for ( int i = 1, n = aMapIndex.Extent(); i <= n; i++ ) {
      if ( aMap.Contains( aMapIndex( i ) ) )
        continue;

      QListWidgetItem* anItem = new QListWidgetItem( QString( "%1" ).arg( aMapIndex( i ) ) );
      myIdList->addItem( anItem );
      anItem->setSelected( true );
    }

    myIdList->blockSignals( isBlocked );
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
  myIdList->blockSignals( true );

  QListIterator<QListWidgetItem*> it( myIdList->selectedItems() );
  while ( it.hasNext() )
    delete it.next();

  myIdList->blockSignals( isBlocked );

  highlightSubShapes();
}

//=======================================================================
//function : subSelectionWay
//purpose  : 
//=======================================================================

int GroupGUI_GroupDlg::subSelectionWay() const
{
  return mySelectionWayGroup->checkedId();
}

//=================================================================================
// function : getShapeType()
// purpose  :
//=================================================================================
TopAbs_ShapeEnum GroupGUI_GroupDlg::getShapeType() const
{
  switch ( getConstructorId() ) {
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
void GroupGUI_GroupDlg::setShapeType( const TopAbs_ShapeEnum theType )
{
  int anId = 0;
  switch ( theType ) {
  case TopAbs_VERTEX: anId = 0; break;
  case TopAbs_EDGE:   anId = 1; break;
  case TopAbs_FACE:   anId = 2; break;
  case TopAbs_SOLID:  anId = 3; break;
  }
  setConstructorId( anId );
}


//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void GroupGUI_GroupDlg::activateSelection()
{
  globalSelection( GEOM_ALLSHAPES );

  // local selection
  if ( !myMainObj->_is_nil() && !myEditCurrentArgument/* && mySelSubBtn->isChecked()*/) {
//     if ( !myPlaceCheckBox->isChecked() )
    if ( subSelectionWay() == ALL_SUBSHAPES )
      localSelection( myMainObj, getShapeType() );
    else if ( !myInPlaceObj->_is_nil() )
      localSelection( myInPlaceObj, getShapeType() );
  }

  SelectionIntoArgument();
}

//=================================================================================
// function : updateState
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::updateState()
{
  bool isAdd = false;

  TColStd_IndexedMapOfInteger aMapIndex;

  SALOME_ListIO aSelIOs;
  SalomeApp_Application* app = myGeomGUI->getApp();
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if ( aSelMgr ) {
      LightApp_SelectionMgr::MapEntryOfMapOfInteger aMap;
      aSelMgr->selectedSubOwners( aMap );
      if ( aMap.Size() == 1 )
        aMapIndex = LightApp_SelectionMgr::MapEntryOfMapOfInteger::Iterator( aMap ).Value();
    }
  }

  // try to find out and process the object browser selection
  if ( !aMapIndex.Extent() && !CORBA::is_nil( myMainObj ) ) {
    GEOM::ListOfGO anObjects;
    //GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);
    GEOMBase::ConvertListOfIOInListOfGO(aSelIOs, anObjects);

    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

    isAdd = true;

    for (int i = 0; i < anObjects.length(); i++)
    {
      GEOM::GEOM_Object_var aGeomObj = anObjects[i];
      GEOM::ListOfGO_var aSubObjects = new GEOM::ListOfGO();
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape(aGeomObj, aShape, getShapeType()) ) {
        aSubObjects->length(1);
        aSubObjects[0] = aGeomObj;
      }
      else if (aGeomObj->GetType() == GEOM_GROUP) {
        aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false);
      }
      else {
        aMapIndex.Clear();
        break;
      }

      for (int i = 0; i < aSubObjects->length(); i++)
      {
        TopoDS_Shape aShape;
        aSubObjects[i];
        if ( GEOMBase::GetShape(aSubObjects[i], aShape, getShapeType()) )
        {
	  CORBA::Long anIndex;
	  anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
          if ( anIndex >= 0 )
            aMapIndex.Add( anIndex );
          else
            isAdd = false;
        }
        else
          isAdd = false;

        if ( !isAdd ) {
          aMapIndex.Clear();
          break;
        }
      }
      if ( !isAdd ) {
        aMapIndex.Clear();
        break;
      }
    }
  }

  isAdd = aMapIndex.Extent() > 0;

  myAddBtn->setEnabled( !myEditCurrentArgument && !CORBA::is_nil( myMainObj ) && isAdd );
  bool hasSel = myIdList->selectedItems().count() > 0;
  myRemBtn->setEnabled( hasSel );
  //mySelSubBtn->setEnabled( !CORBA::is_nil( myMainObj ) );
  //myPlaceCheckBox->setEnabled( !CORBA::is_nil( myMainObj ) );
  mySelectionWayGroupBox->setEnabled( !CORBA::is_nil( myMainObj ) );
  mySelAllBtn->setEnabled( !CORBA::is_nil( myMainObj ) );
//   mySelBtn2->setEnabled( myPlaceCheckBox->isChecked() );
//   myShape2Name->setEnabled( myPlaceCheckBox->isChecked() );
  mySelBtn2->setEnabled(    subSelectionWay() != ALL_SUBSHAPES );
  myShape2Name->setEnabled( subSelectionWay() != ALL_SUBSHAPES );
//   if ( !myPlaceCheckBox->isChecked() )
  if ( subSelectionWay() == ALL_SUBSHAPES )
    setInPlaceObj( GEOM::GEOM_Object::_nil() );
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
  if ( CORBA::is_nil( myMainObj ) )
    return;

  Standard_Boolean isOk;
  char* objIOR;

  if ( myInPlaceObj->_is_nil() )
    objIOR = GEOMBase::GetIORFromObject( myMainObj );
  else
    objIOR = GEOMBase::GetIORFromObject( myInPlaceObj );

  Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIORinGEOMAISShape( objIOR, isOk, true );
  free( objIOR );
  if ( !isOk || aSh.IsNull() )
    return;

  TColStd_MapOfInteger anIds;

  myBusy = true;

  int ii = 0, nn = myIdList->count();
  for ( ; ii < nn; ii++ )
  {
    if ( myIdList->item( ii )->isSelected() ) {
      int id = myIdList->item( ii )->text().toInt();
      //       if ( myPlaceCheckBox->isChecked() )
      if ( subSelectionWay() != ALL_SUBSHAPES )
      {
        if ( myMain2InPlaceIndices.IsBound( id ) )
          id = myMain2InPlaceIndices( id );
        else {
          myIdList->item( ii )->setSelected( false );
          continue;
        }
      }
      anIds.Add( id );
    }
  }
  SalomeApp_Application* app = myGeomGUI->getApp();
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  aSelMgr->clearSelected();

  aSelMgr->AddOrRemoveIndex(aSh->getIO(), anIds, false);

  myBusy = false;

  if (nn < 3000)
    updateState();
  else {
    myAddBtn->setEnabled( true );
    myRemBtn->setEnabled( true );
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GroupGUI_GroupDlg::createOperation()
{
  return getGeomEngine()->GetIGroupOperations( getStudyId() );
}

#define RETURN_WITH_MSG(a, b) \
  if ( !(a) ) { \
    theMessage += (b); \
    return false; \
  }

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool GroupGUI_GroupDlg::isValid( QString& theMessage )
{
  SalomeApp_Study* study = getStudy();
  ASSERT(study);
  RETURN_WITH_MSG  ( !study->studyDS()->GetProperties()->IsLocked(), tr( "GEOM_STUDY_LOCKED" ) )

  if ( myMode == CreateGroup ) {
    RETURN_WITH_MSG( !CORBA::is_nil( myMainObj ), tr( "NO_MAIN_OBJ" ) )
  }
  else {
    RETURN_WITH_MSG( !CORBA::is_nil( myMainObj ), tr( "NO_GROUP" ) )
  }

  QString aName (getNewObjectName());
  RETURN_WITH_MSG  ( !aName.trimmed().isEmpty(), tr( "EMPTY_NAME" ) )

  RETURN_WITH_MSG  ( myIdList->count(), tr( "EMPTY_LIST" ) )
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GroupGUI_GroupDlg::execute( ObjectList& objects )
{
  setInPlaceObj( GEOM::GEOM_Object::_nil() );

  GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow(getOperation());

  GEOM::GEOM_Object_var aGroup;
  if (myMode == CreateGroup)
    aGroup = anOp->CreateGroup(myMainObj, getShapeType());
  else if (myMode == EditGroup)
    aGroup = myGroup;

  if (CORBA::is_nil(aGroup) || (myMode == CreateGroup && !anOp->IsDone()))
    return false;

  GEOM::ListOfLong_var aCurrList = anOp->GetObjects(aGroup);
  if (!anOp->IsDone())
    return false;

  if (aCurrList->length() > 0)
  {
    anOp->DifferenceIDs(aGroup, aCurrList);
    if (!anOp->IsDone())
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
    anOp->UnionIDs(aGroup, aNewList);
    if (!anOp->IsDone())
      return false;
  }

  SalomeApp_Study* study = getStudy();
  if ( study ) {
    char* objIOR = GEOMBase::GetIORFromObject( aGroup );
    std::string IOR( objIOR );
    free( objIOR );
    if ( IOR != "" ) {
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( IOR ) );
      if ( SO ) {
        _PTR(StudyBuilder) aBuilder (study->studyDS()->NewBuilder());
        aBuilder->SetName( SO, getNewObjectName().toLatin1().constData() );
      }
    }
  }

  objects.push_back( aGroup._retn() );

  return true;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr GroupGUI_GroupDlg::getFather( GEOM::GEOM_Object_ptr theObj )
{
  GEOM::GEOM_Object_var aFatherObj;
  if ( theObj->GetType() == GEOM_GROUP ) {
    GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow( getOperation() );
    aFatherObj = anOp->GetMainShape( theObj );
  }
  return aFatherObj._retn();
}
