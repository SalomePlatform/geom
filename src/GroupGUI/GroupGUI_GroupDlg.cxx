// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : GroupGUI_GroupDlg.cxx
// Author : Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)
//

#include "GroupGUI_GroupDlg.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <GEOMImpl_Types.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

GroupGUI_GroupDlg::GroupGUI_GroupDlg( Mode mode, GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false ),
    myMode( mode ),
    myBusy( false )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap image0    ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_VERTEX" ) ) );
  QPixmap image1    ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_EDGE" ) ) );
  QPixmap image2    ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_FACE" ) ) );
  QPixmap image3    ( resMgr->loadPixmap( "GEOM", tr( "ICON_OBJBROWSER_SOLID" ) ) );
  QPixmap iconSelect( resMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

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
  myGroupBox = new DlgRef_1Sel1List1Check3Btn( centralWidget() );
  myGroupBox->GroupBox1->setTitle( tr( "MAIN_SUB_SHAPES" ) );
  myGroupBox->TextLabel1->setText( tr( "MAIN_SHAPE" ) );
  myGroupBox->PushButton1->setIcon( iconSelect );
  myGroupBox->PushButton1->setEnabled( myMode == CreateGroup );
  myGroupBox->LineEdit1->setReadOnly( true );
  myGroupBox->LineEdit1->setEnabled( myMode == CreateGroup );
  myGroupBox->CheckButton1->setText( tr( "SELECT_SUB_SHAPES" ) );
  myGroupBox->PushButton2->setText( tr( "SELECT_ALL" ) );
  myGroupBox->PushButton3->setText( tr( "ADD" ) );
  myGroupBox->PushButton4->setText( tr( "REMOVE" ) );
  myGroupBox->ListView1->setSelectionMode( QAbstractItemView::ExtendedSelection );
  myGroupBox->ListView1->setFlow( QListView::TopToBottom );
  myGroupBox->ListView1->setWrapping( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGroupBox );

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
  myGroupBox->CheckButton1->setChecked( true );

  if ( myMode == CreateGroup ) {
    initName( tr( "GROUP_PREFIX" ) );

    // Get ready for main shape selection
    myEditCurrentArgument = myGroupBox->LineEdit1;

    connect( this, SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );
    connect( myGroupBox->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  }
  else if ( myMode == EditGroup && IObjectCount() ) {
    Standard_Boolean aResult = Standard_False;
    GEOM::GEOM_Object_var anObj =
      GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

    if ( aResult && !CORBA::is_nil( anObj ) && anObj->GetType() == GEOM_GROUP ) {
      myGroup = anObj;

      mainFrame()->ResultName->setText( GEOMBase::GetName( myGroup ) );

      GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow( getOperation() );
      myMainObj = anOp->GetMainShape( myGroup );
      if ( !CORBA::is_nil( myMainObj ) )
        myGroupBox->LineEdit1->setText( GEOMBase::GetName( myMainObj ) );

      setShapeType( (TopAbs_ShapeEnum)anOp->GetType( myGroup ) );

      GEOM::ListOfLong_var aCurrList = anOp->GetObjects( myGroup );
      for ( int i = 0, n = aCurrList->length(); i < n; i++ )
        myGroupBox->ListView1->addItem( new QListWidgetItem( QString( "%1" ).arg( aCurrList[i] ) ) );

      myEditCurrentArgument = 0;
    }
  }

  LightApp_SelectionMgr* aSelMgr =
    ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr();

  connect( aSelMgr,     SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGroupBox->CheckButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupBox->PushButton2,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupBox->PushButton3,  SIGNAL( clicked() ), this, SLOT( add() ) );
  connect( myGroupBox->PushButton4,  SIGNAL( clicked() ), this, SLOT( remove() ) );
  connect( myGroupBox->ListView1,    SIGNAL( itemSelectionChanged() ), this, SLOT( selectionChanged() ) );

  activateSelection();
  // activate subshapes selection if Main Shape is Selected
  if ( !CORBA::is_nil( myMainObj ) ) {
    myEditCurrentArgument = 0;
    activateSelection();
    updateState();
  }
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::enterEvent( QEvent* )
{
  if ( !buttonCancel()->isEnabled() )
    this->ActivateThisDialog();
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

  if ( myMode == CreateGroup ) {
    initName();
    myGroupBox->ListView1->clear();
  }
  else {
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

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
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

  if ( send == myGroupBox->LineEdit1 && !myEditCurrentArgument ) {
    myEditCurrentArgument = myGroupBox->LineEdit1;
    activateSelection();
  }
  else {
    GEOMBase_Skeleton::LineEditReturnPressed();
  }

  updateState();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();

  if ( send == myGroupBox->PushButton1 )
    myEditCurrentArgument = myGroupBox->LineEdit1;
  else if ( send == myGroupBox->CheckButton1 || send == myGroupBox->PushButton2 )
    myEditCurrentArgument = 0;

  activateSelection();

  if ( send == myGroupBox->PushButton2 )
    selectAllSubShapes();
  else
    updateState();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GroupGUI_GroupDlg::SelectionIntoArgument()
{
  if ( myEditCurrentArgument ) {  // Selection of a main shape is active
    myEditCurrentArgument->setText( "" );
    myGroupBox->ListView1->clear();

    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

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
    if ( myBusy )
      return;

    bool isBlocked = myGroupBox->ListView1->signalsBlocked();
    myGroupBox->ListView1->blockSignals( true );
    myGroupBox->ListView1->clearSelection();

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
    if ( !aMapIndex.Extent() ) {
      globalSelection( GEOM_ALLSHAPES );
      
      GEOM::ListOfGO anObjects;
      GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);

      GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
      GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

      for (int i = 0; i < anObjects.length(); i++) {
	GEOM::GEOM_Object_var aGeomObj = anObjects[i];
	GEOM::ListOfGO_var aSubObjects = new GEOM::ListOfGO();
	TopoDS_Shape aShape;
	if ( GEOMBase::GetShape( aGeomObj, aShape, getShapeType() ) ) {
	  aSubObjects->length(1);
	  aSubObjects[0] = aGeomObj;
	}
	else if (aGeomObj->GetType() == GEOM_GROUP)
	  aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false);
	else
	  continue;

	for (int i = 0; i < aSubObjects->length(); i++) {
	  TopoDS_Shape aShape;
	  if ( GEOMBase::GetShape( aSubObjects[i], aShape, getShapeType() ) ) {
	    CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
	    if ( anIndex >= 0 )
	      aMapIndex.Add( anIndex );
	  }
	}
      }
      
      if ( !myMainObj->_is_nil() )
      	localSelection( myMainObj, getShapeType() );
    }

    if ( aMapIndex.Extent() >= 1 ) {
      QMap<int, int> aMap;
      for ( int i = 0, n = myGroupBox->ListView1->count(); i < n; i++ )
        aMap.insert( myGroupBox->ListView1->item( i )->text().toInt(), i );

      for ( int ii = 1, nn = aMapIndex.Extent(); ii <= nn; ii++ ) {
        if ( aMap.contains( aMapIndex( ii ) ) )
          myGroupBox->ListView1->item( aMap[aMapIndex( ii )])->setSelected( true );
      }
    }
    myGroupBox->ListView1->blockSignals( isBlocked );
  }

  updateState();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GroupGUI_GroupDlg::ConstructorsClicked( int )
{
  myGroupBox->ListView1->clear();
  activateSelection();
  updateState();
}

//=================================================================================
// function : selectAllSubShapes
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::selectAllSubShapes()
{
  if ( CORBA::is_nil( myMainObj ) )
    return;

  GEOM::GEOM_IShapesOperations_var aShOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

  GEOM::ListOfLong_var aSubShapes = aShOp->SubShapeAllIDs( myMainObj, getShapeType(), false );
  if ( !aShOp->IsDone() )
    return;

  bool isBlocked = myGroupBox->ListView1->signalsBlocked();
  myGroupBox->ListView1->blockSignals( true );
  myGroupBox->ListView1->clear();

  for ( int i = 0, n = aSubShapes->length(); i < n; i++ ) {
    CORBA::Long anIndex = aSubShapes[i];
    if ( anIndex < 0 )
      continue;

    QListWidgetItem* anItem = new QListWidgetItem( QString( "%1" ).arg( anIndex ) );
    myGroupBox->ListView1->addItem( anItem );
    anItem->setSelected( true );
  }

  myGroupBox->ListView1->blockSignals( isBlocked );
  highlightSubShapes();
  //updateState(); // already done in highlightSubShapes()
}

//=================================================================================
// function : add
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::add()
{
  TColStd_MapOfInteger aMap;
  for ( int i = 0, n = myGroupBox->ListView1->count(); i < n; i++ )
    aMap.Add( myGroupBox->ListView1->item( i )->text().toInt() );

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
  if ( !aMapIndex.Extent() ) {
    GEOM::ListOfGO anObjects;
    GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);
    
    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
    

    for ( int i = 0; i < anObjects.length(); i++ ) {
      GEOM::GEOM_Object_var aGeomObj = anObjects[i];
      GEOM::ListOfGO_var aSubObjects  = new GEOM::ListOfGO();
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aGeomObj, aShape, getShapeType() ) ) {
	aSubObjects->length( 1 );
	aSubObjects[0] = aGeomObj;
      }
      else if ( aGeomObj->GetType() == GEOM_GROUP )
	aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false );
      else
	break;
	
      for ( int i = 0; i < aSubObjects->length(); i++ ) {
	TopoDS_Shape aShape;
	if ( GEOMBase::GetShape( aSubObjects[i], aShape, getShapeType() ) ) {
	  CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
	  if ( anIndex >= 0 )
	    aMapIndex.Add( anIndex );
	}
      }
    }
  }

  if ( aMapIndex.Extent() >= 1 ) {
    bool isBlocked = myGroupBox->ListView1->signalsBlocked();
    myGroupBox->ListView1->blockSignals( true );

    for ( int i = 1, n = aMapIndex.Extent(); i <= n; i++ ) {
      if ( aMap.Contains( aMapIndex( i ) ) )
        continue;

      QListWidgetItem* anItem = new QListWidgetItem( QString( "%1" ).arg( aMapIndex( i ) ) );
      myGroupBox->ListView1->addItem( anItem );
      anItem->setSelected( true );
    }

    myGroupBox->ListView1->blockSignals( isBlocked );
  }

  updateState();
}

//=================================================================================
// function : remove
// purpose  : 
//=================================================================================
void GroupGUI_GroupDlg::remove()
{
  bool isBlocked = myGroupBox->ListView1->signalsBlocked();
  myGroupBox->ListView1->blockSignals( true );

  QListIterator<QListWidgetItem*> it( myGroupBox->ListView1->selectedItems() );
  while ( it.hasNext() )
    delete it.next();

  myGroupBox->ListView1->blockSignals( isBlocked );

  highlightSubShapes();
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

  if ( !myMainObj->_is_nil() && !myEditCurrentArgument ) {
    localSelection( myMainObj, getShapeType() );
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
    GEOMBase::ConvertListOfIOInListOfGO( aSelIOs, anObjects );

    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );

    isAdd = true;

    for ( int i = 0; i < anObjects.length(); i++ ) {
      GEOM::GEOM_Object_var aGeomObj = anObjects[i];
      GEOM::ListOfGO_var aSubObjects = new GEOM::ListOfGO();
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aGeomObj, aShape, getShapeType() ) ) {
        aSubObjects->length(1);
        aSubObjects[0] = aGeomObj;
      }
      else if ( aGeomObj->GetType() == GEOM_GROUP ) {
        aSubObjects = aShapesOp->MakeExplode( aGeomObj, getShapeType(), false );
      }
      else {
        aMapIndex.Clear();
        break;
      }

      for ( int i = 0; i < aSubObjects->length(); i++ ) {
        TopoDS_Shape aShape;
        aSubObjects[i];
        if ( GEOMBase::GetShape( aSubObjects[i], aShape, getShapeType() ) ) {
          CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubObjects[i] );
          if ( anIndex >= 0 )
            aMapIndex.Add( anIndex );
          else
            isAdd = false;
        }
        else {
          isAdd = false;
	}

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

  myGroupBox->PushButton3->setEnabled( !myEditCurrentArgument && !CORBA::is_nil( myMainObj ) && isAdd );
  bool hasSel = myGroupBox->ListView1->selectedItems().count() > 0;
  myGroupBox->PushButton4->setEnabled( hasSel );
  myGroupBox->CheckButton1->setEnabled( !CORBA::is_nil( myMainObj ) );
  myGroupBox->PushButton2->setEnabled( !CORBA::is_nil( myMainObj ) );
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
  char* objIOR = GEOMBase::GetIORFromObject( myMainObj );
  Handle(GEOM_AISShape) aSh =
    GEOMBase::ConvertIORinGEOMAISShape( objIOR, isOk, true );
  free( objIOR );
  if ( !isOk || aSh.IsNull() )
    return;

  TColStd_MapOfInteger anIds;

  myBusy = true;

  int ii = 0, nn = myGroupBox->ListView1->count();
  for ( ; ii < nn; ii++ )
    if ( myGroupBox->ListView1->item( ii )->isSelected() )
      anIds.Add( myGroupBox->ListView1->item( ii )->text().toInt() );

  SalomeApp_Application* app = myGeomGUI->getApp();
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  aSelMgr->clearSelected();
  //if (nn < 3000) aSelMgr->AddOrRemoveIndex(aSh->getIO(), anIds, false);
  aSelMgr->AddOrRemoveIndex( aSh->getIO(), anIds, false );

  myBusy = false;

  //updateState();
  if ( nn < 3000 ) {
    updateState();
  }
  else {
    myGroupBox->PushButton3->setEnabled( true );
    myGroupBox->PushButton4->setEnabled( true );
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

  QString aName( getNewObjectName() );
  RETURN_WITH_MSG  ( !aName.trimmed().isEmpty(), tr( "EMPTY_NAME" ) )

  RETURN_WITH_MSG  ( myGroupBox->ListView1->count(), tr( "EMPTY_LIST" ) )
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GroupGUI_GroupDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow( getOperation() );

  GEOM::GEOM_Object_var aGroup;
  if ( myMode == CreateGroup )
    aGroup = anOp->CreateGroup( myMainObj, getShapeType() );
  else if ( myMode == EditGroup )
    aGroup = myGroup;

  if ( CORBA::is_nil( aGroup ) || ( myMode == CreateGroup && !anOp->IsDone() ) )
    return false;

  GEOM::ListOfLong_var aCurrList = anOp->GetObjects( aGroup );
  if ( !anOp->IsDone()  )
    return false;

  if ( aCurrList->length() > 0 ) {
    anOp->DifferenceIDs( aGroup, aCurrList );
    if ( !anOp->IsDone() )
      return false;
  }

  int ii, nn = myGroupBox->ListView1->count();
  if ( nn > 0 ) {
    GEOM::ListOfLong_var aNewList = new GEOM::ListOfLong;
    aNewList->length( nn );
    for ( ii = 0; ii < nn; ii++ ) {
      aNewList[ii] = myGroupBox->ListView1->item( ii )->text().toInt();
    }
    anOp->UnionIDs( aGroup, aNewList );
    if ( !anOp->IsDone() )
      return false;
  }

  SalomeApp_Study* study = getStudy();
  if ( study ) {
    char* objIOR = GEOMBase::GetIORFromObject( aGroup );
    string IOR( objIOR );
    free( objIOR );
    if ( IOR != "" ) {
      _PTR(SObject) SO ( study->studyDS()->FindObjectIOR( IOR ) );
      if ( SO ) {
        _PTR(StudyBuilder) aBuilder (study->studyDS()->NewBuilder());
        aBuilder->SetName( SO, getNewObjectName() );
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
