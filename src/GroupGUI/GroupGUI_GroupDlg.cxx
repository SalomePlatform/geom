//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GroupGUI_GroupDlg.cxx
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#include "GroupGUI_GroupDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "LightApp_SelectionMgr.h"

#include "GEOMBase.h"

#include "GEOMImpl_Types.hxx"

#include <qlabel.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qmap.h>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>


GroupGUI_GroupDlg::GroupGUI_GroupDlg(Mode mode,
                                     QWidget* parent,
                                     const char* name,
                                     bool modal,
                                     WFlags fl)
  :GEOMBase_Skeleton( parent, "GroupGUI_GroupDlg", false,
                      WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu),
   myMode( mode ),
   myBusy( false )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap image0     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_VERTEX")));
  QPixmap image1     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_EDGE")));
  QPixmap image2     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_FACE")));
  QPixmap image3     (resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_SOLID")));
  QPixmap iconSelect (resMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption( myMode == CreateGroup ? tr( "CREATE_GROUP_TITLE" ) : tr( "EDIT_GROUP_TITLE" ) );

  // Shape type button group
  GroupConstructors->setEnabled( myMode == CreateGroup );
  GroupConstructors->setTitle( tr( "SHAPE_TYPE" ) );
  RadioButton1->setPixmap( image0 );
  RadioButton2->setPixmap( image1 );
  RadioButton3->setPixmap( image2 );
  RadioButton4->setPixmap( image3 );
  RadioButton4->show();

  // Group name
  GroupBoxName->setTitle( tr( "GROUP_NAME" ) );

  // Main shape and sub-shapes
  GroupMedium = new QGroupBox( 1, Qt::Vertical, tr( "MAIN_SUB_SHAPES" ), this );
  GroupMedium->setInsideMargin( 10 );
  Layout1->addWidget( GroupMedium, 2, 0 );

  QWidget* aFrame = new QWidget( GroupMedium );
  QGridLayout* aMedLayout = new QGridLayout( aFrame, 4, 4, 0, 6 );

  QLabel* aMainLabel = new QLabel( tr( "MAIN_SHAPE" ), aFrame );

  mySelBtn = new QPushButton( aFrame );
  mySelBtn->setPixmap( iconSelect );
  mySelBtn->setEnabled( myMode == CreateGroup );

  myMainName = new QLineEdit( aFrame );
  myMainName->setReadOnly( true );
  myMainName->setEnabled( myMode == CreateGroup );

  mySelSubBtn = new QPushButton( tr( "SELECT_SUB_SHAPES" ), aFrame );
  mySelAllBtn = new QPushButton( tr( "SELECT_ALL" ), aFrame );
  myAddBtn    = new QPushButton( tr( "ADD" ), aFrame );
  myRemBtn    = new QPushButton( tr( "REMOVE" ), aFrame );
  myIdList    = new QListBox( aFrame );

  myIdList->setSelectionMode( QListBox::Extended );
  myIdList->setRowMode( QListBox::FitToWidth );

  aMedLayout->addWidget( aMainLabel, 0, 0 );
  aMedLayout->addWidget( mySelBtn, 0, 1 );
  aMedLayout->addMultiCellWidget( myMainName, 0, 0, 2, 3 );
  aMedLayout->addMultiCellWidget( mySelSubBtn, 1, 1, 0, 2 );
  aMedLayout->addWidget( mySelAllBtn, 1, 3 );
  aMedLayout->addMultiCellWidget( myIdList, 2, 3, 0, 2 );
  aMedLayout->addWidget( myAddBtn, 2, 3 );
  aMedLayout->addWidget( myRemBtn, 3, 3 );

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

  if ( myMode == CreateGroup ) {
    initName( tr( "GROUP_PREFIX" ) );

    // Get ready for main shape selection
    myEditCurrentArgument = myMainName;

    connect( GroupConstructors, SIGNAL( clicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );
    connect( mySelBtn,          SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  }
  else if ( myMode == EditGroup && IObjectCount() ) {
    Standard_Boolean aResult = Standard_False;
    GEOM::GEOM_Object_var anObj =
      GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

    if ( aResult && !CORBA::is_nil( anObj ) && anObj->GetType() == GEOM_GROUP ) {
      myGroup = anObj;

      ResultName->setText( GEOMBase::GetName( myGroup ) );

      GEOM::GEOM_IGroupOperations_var anOp = GEOM::GEOM_IGroupOperations::_narrow( getOperation() );
      myMainObj = anOp->GetMainShape( myGroup );
      if ( !CORBA::is_nil( myMainObj ) )
        myMainName->setText( GEOMBase::GetName( myMainObj ) );

      setShapeType( (TopAbs_ShapeEnum)anOp->GetType( myGroup ) );

      GEOM::ListOfLong_var aCurrList = anOp->GetObjects( myGroup );
      QListBoxItem* anItem;
      for ( int i = 0, n = aCurrList->length(); i < n; i++ ) {
        anItem = new QListBoxText( QString( "%1" ).arg(aCurrList[i] ) );
        myIdList->insertItem( anItem );
      }

      myEditCurrentArgument = 0;
    }
  }

  LightApp_SelectionMgr* aSelMgr =
    ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();

  connect( aSelMgr,     SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( buttonOk   , SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( mySelSubBtn, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( mySelAllBtn, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( myAddBtn,    SIGNAL( clicked() ), this, SLOT( add() ) );
  connect( myRemBtn,    SIGNAL( clicked() ), this, SLOT( remove() ) );
  connect( myIdList,    SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );

  activateSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GroupGUI_GroupDlg::enterEvent( QEvent* e )
{
  if ( !buttonCancel->isEnabled() )
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

  if ( myMode == CreateGroup )
    initName();
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

  if ( send == mySelBtn )
    myEditCurrentArgument = myMainName;
  else if ( send == mySelSubBtn || send == mySelAllBtn )
    myEditCurrentArgument = 0;

  activateSelection();

  if ( send == mySelAllBtn )
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
    myIdList->clear();

    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() && GEOMBase::IsShape( anObj ) ) {
        myMainObj = anObj;
        myEditCurrentArgument->setText( GEOMBase::GetName( anObj ) );
        updateState();
        return;
      }
    }

    myMainObj = GEOM::GEOM_Object::_nil();
  }
  else { // an attempt to synchronize list box selection with 3d viewer
    if ( myBusy )
      return;

    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals( true );
    myIdList->clearSelection();

    TColStd_IndexedMapOfInteger aMapIndex;

    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() )
        ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->
          selectionMgr()->GetIndexes( firstIObject(), aMapIndex );
    }

    // try to find out and process the object browser selection
    if ( !aMapIndex.Extent() ) {
      GEOM::ListOfGO anObjects;
      GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);
      GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
      for (int i = 0; i < anObjects.length(); i++) {
        TopoDS_Shape aShape;
        if ( GEOMBase::GetShape(anObjects[i], aShape, getShapeType()) ) {
          CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, anObjects[i] );
          if ( anIndex >= 0 )
            aMapIndex.Add( anIndex );
        }
      }
    }

    if (aMapIndex.Extent() >= 1) {
      QMap<int, int> aMap;
      for ( int i = 0, n = myIdList->count(); i < n; i++ )
        aMap.insert( myIdList->item( i )->text().toInt(), i );

      for ( int ii = 1, nn = aMapIndex.Extent(); ii <= nn; ii++ ) {
        if ( aMap.contains( aMapIndex( ii ) ) )
          myIdList->setSelected( aMap[aMapIndex( ii )], true );
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
  GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );

  GEOM::ListOfGO_var aSubShapes = aShOp->MakeExplode( myMainObj, getShapeType(), false );
  if ( !aShOp->IsDone() )
    return;

  bool isBlocked = myIdList->signalsBlocked();
  myIdList->blockSignals( true );
  myIdList->clear();

  QListBoxItem* anItem;
  for ( int i = 0, n = aSubShapes->length(); i < n; i++ ) {
    CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, aSubShapes[i] );
    if ( anIndex < 0 )
      continue;

    anItem = new QListBoxText( QString( "%1" ).arg( anIndex ) );
    myIdList->insertItem( anItem );
    myIdList->setSelected( anItem, true );
  }

  myIdList->blockSignals( isBlocked );
  highlightSubShapes();
  updateState();
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

  if ( IObjectCount() == 1 ) {
    Standard_Boolean aResult = Standard_False;
    GEOM::GEOM_Object_var anObj =
      GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

    if ( aResult && !anObj->_is_nil() )
      ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->
        selectionMgr()->GetIndexes( firstIObject(), aMapIndex );
  }

  // try to find out and process the object browser selection
  if ( !aMapIndex.Extent() ) {
    GEOM::ListOfGO anObjects;
    GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);
    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    for (int i = 0; i < anObjects.length(); i++) {
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape(anObjects[i], aShape, getShapeType()) ) {
        CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, anObjects[i] );
        if ( anIndex >= 0 )
          aMapIndex.Add( anIndex );
      }
    }
  }

  if ( aMapIndex.Extent() >= 1 ) {
    QListBoxItem* anItem;
    bool isBlocked = myIdList->signalsBlocked();
    myIdList->blockSignals( true );

    for ( int i = 1, n = aMapIndex.Extent(); i <= n; i++ ) {
      if ( aMap.Contains( aMapIndex( i ) ) )
        continue;

      anItem = new QListBoxText( QString( "%1" ).arg( aMapIndex( i ) ) );
      myIdList->insertItem( anItem );
      myIdList->setSelected( anItem, true );
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
  for ( int i = myIdList->count() - 1; i >= 0; i-- ) {
    if ( myIdList->isSelected( i ) )
      myIdList->removeItem( i );
  }
  updateState();
}


//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int GroupGUI_GroupDlg::getConstructorId() const
{
  return GroupConstructors->id( GroupConstructors->selected() );
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
  GroupConstructors->setButton( anId );
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

  if ( IObjectCount() == 1 ) {
    Standard_Boolean aResult = Standard_False;
    GEOM::GEOM_Object_var anObj =
      GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

    if ( aResult && !anObj->_is_nil() )
      ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->
        selectionMgr()->GetIndexes( firstIObject(), aMapIndex );
  }

  // try to find out and process the object browser selection
  if ( !aMapIndex.Extent() && !CORBA::is_nil( myMainObj ) ) {
    isAdd = true;
    GEOM::ListOfGO anObjects;
    GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), anObjects);
    GEOM::GEOM_ILocalOperations_var aLocOp = getGeomEngine()->GetILocalOperations( getStudyId() );
    for (int i = 0; i < anObjects.length(); i++) {
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape(anObjects[i], aShape, getShapeType()) ) {
        CORBA::Long anIndex = aLocOp->GetSubShapeIndex( myMainObj, anObjects[i] );
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
  }

  isAdd = aMapIndex.Extent() > 0;

  myAddBtn->setEnabled( !myEditCurrentArgument && !CORBA::is_nil( myMainObj ) && isAdd );
  bool hasSel = false;
  for ( int ii = 0, nn = myIdList->count(); !hasSel && ii < nn; ii++ )
    hasSel =  myIdList->isSelected( ii );
  myRemBtn->setEnabled( hasSel );
  mySelSubBtn->setEnabled( !CORBA::is_nil( myMainObj ) );
  mySelAllBtn->setEnabled( !CORBA::is_nil( myMainObj ) );
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
  Handle(GEOM_AISShape) aSh =
    GEOMBase::ConvertIORinGEOMAISShape( GEOMBase::GetIORFromObject( myMainObj ), isOk, true );
  if ( !isOk || aSh.IsNull() )
    return;

  TColStd_MapOfInteger anIds;

  myBusy = true;

  for ( int ii = 0, nn = myIdList->count(); ii < nn; ii++ )
    if ( myIdList->isSelected( ii ) )
      anIds.Add( myIdList->item( ii )->text().toInt() );

  LightApp_SelectionMgr* aSelMgr =
    ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();
  aSelMgr->clearSelected();
  aSelMgr->AddOrRemoveIndex( aSh->getIO(), anIds, false );

  myBusy = false;

  updateState();
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

  const char* aName = getNewObjectName();
  RETURN_WITH_MSG  ( aName && strlen( aName ), tr( "EMPTY_NAME" ) )

  RETURN_WITH_MSG  ( myIdList->count(), tr( "EMPTY_LIST" ) )
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

  for ( int i = 0, n = aCurrList->length(); i < n; i++ ) {
    anOp->RemoveObject( aGroup, aCurrList[i] );
    if ( !anOp->IsDone()  )
      return false;
  }

  for ( int ii = 0, nn = myIdList->count(); ii < nn; ii++ ) {
    anOp->AddObject( aGroup, myIdList->item( ii )->text().toInt() );
    if ( !anOp->IsDone()  )
      return false;
  }

  SalomeApp_Study* study = getStudy();
  if ( study ) {
    string IOR = GEOMBase::GetIORFromObject( aGroup );
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
