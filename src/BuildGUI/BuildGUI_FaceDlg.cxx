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
// File   : BuildGUI_FaceDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BuildGUI_FaceDlg.h"

#include <GEOMImpl_Types.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>

#include <QTreeWidget>

//=================================================================================
// class    : BuildGUI_TreeWidgetItem
// purpose  : class for constraint(Edge-Face) creation
//=================================================================================
class BuildGUI_FaceDlg::TreeWidgetItem : public QTreeWidgetItem
{
public:
  TreeWidgetItem( QTreeWidget*, const GEOM::GeomObjPtr, int = Type );
  TreeWidgetItem( QTreeWidgetItem*, const GEOM::GeomObjPtr, int = Type );
  ~TreeWidgetItem();
  void             setFace( const GEOM::GeomObjPtr );
  GEOM::GeomObjPtr getFace() const;
  GEOM::GeomObjPtr getEdge() const;
private:
  GEOM::GeomObjPtr myEdge;
  GEOM::GeomObjPtr myFace;
};

BuildGUI_FaceDlg::TreeWidgetItem::TreeWidgetItem( QTreeWidget* view, const GEOM::GeomObjPtr edge, int type )
:QTreeWidgetItem( view, QStringList()<<GEOMBase::GetName( edge.get() ), type ),
  myEdge( edge ),
  myFace( 0 )
{
}

BuildGUI_FaceDlg::TreeWidgetItem::TreeWidgetItem( QTreeWidgetItem* parent, const GEOM::GeomObjPtr edge, int type )
:QTreeWidgetItem( parent, QStringList()<<GEOMBase::GetName( edge.get() ), type ),
  myEdge( edge ),
  myFace( 0 )
{
}

BuildGUI_FaceDlg::TreeWidgetItem::~TreeWidgetItem()
{
}

void BuildGUI_FaceDlg::TreeWidgetItem::setFace( const GEOM::GeomObjPtr face )
{
  QString aName = GEOMBase::GetName( face.get() );
  setText( 1, aName );
  treeWidget()->resizeColumnToContents(1);
  myFace = face;
}

GEOM::GeomObjPtr BuildGUI_FaceDlg::TreeWidgetItem::getFace() const
{
  return myFace;
}

GEOM::GeomObjPtr BuildGUI_FaceDlg::TreeWidgetItem::getEdge() const
{
  return myEdge;
}

//=================================================================================
// class    : BuildGUI_FaceDlg()
// purpose  : Constructs a BuildGUI_FaceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_FaceDlg::BuildGUI_FaceDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent ),
    myGroupWire(0),
    myGroupSurf(0),
    myGroupWireConstraints(0)
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE_SURFACE" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE_CONSTRAINTS" ) ) );

  setWindowTitle( tr( "GEOM_FACE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_FACE" ) );
  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setIcon( image3 );

  // Face creation from wires and/or edges

  myGroupWire = new DlgRef_1Sel1Check( centralWidget() );

  myGroupWire->GroupBox1->setTitle( tr( "GEOM_FACE_FFW" ) );
  myGroupWire->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  myGroupWire->CheckButton1->setText( tr( "GEOM_FACE_OPT" ) );
  myGroupWire->PushButton1->setIcon( image0 );

  // Face creation from surface

  myGroupSurf = new DlgRef_2Sel(centralWidget());

  myGroupSurf->GroupBox1->setTitle(tr("GEOM_FACE_FROM_SURFACE" ) );
  myGroupSurf->TextLabel1->setText(tr("GEOM_FACE"));
  myGroupSurf->TextLabel2->setText(tr("GEOM_WIRE"));
  myGroupSurf->PushButton1->setIcon(image0);
  myGroupSurf->PushButton2->setIcon(image0);
  
  // Face creation from wire and constraints

  myGroupWireConstraints = new DlgRef_1SelExt( centralWidget() );
  myGroupWireConstraints->GroupBox1->setTitle( tr( "GEOM_FACE_FFWC" ) );
  myGroupWireConstraints->TextLabel1->setText( tr( "GEOM_WIRE" ) );
  myGroupWireConstraints->PushButton1->setIcon( image0 );

  QLabel* aLabel = new QLabel( tr( "GEOM_CONSTRAINTS" ) );
  myTreeConstraints = new QTreeWidget( myGroupWireConstraints->Box );
  myTreeConstraints->setColumnCount(2);
  QStringList columnNames;
  columnNames.append( tr( "GEOM_EDGE" ));
  columnNames.append( tr( "GEOM_FACE_CONSTRAINT" ) );
  myTreeConstraints->setHeaderLabels( columnNames );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  myTreeConstraints->header()->setMovable( false );
  myTreeConstraints->header()->setResizeMode( QHeaderView::ResizeToContents );
#else
  myTreeConstraints->header()->setSectionsMovable( false );
  myTreeConstraints->header()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
  myTreeConstraints->setMinimumHeight( 140 );

  QHBoxLayout* l = new QHBoxLayout( myGroupWireConstraints->Box );
  l->setMargin( 0 ); l->setSpacing( 6 );
  l->addWidget( aLabel);
  l->addWidget( myTreeConstraints );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGroupWire );
  layout->addWidget( myGroupSurf );
  layout->addWidget( myGroupWireConstraints );
  /***************************************************************/

  setHelpFileName("create_face_page.html");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_FaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_FaceDlg::~BuildGUI_FaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = myGroupWire->LineEdit1;
  myGroupWire->LineEdit1->setReadOnly( true );
  myGroupSurf->LineEdit1->setReadOnly( true );
  myGroupSurf->LineEdit2->setReadOnly( true );
  myGroupWireConstraints->LineEdit1->setReadOnly( true );

  myGroupWire->CheckButton1->setChecked( true );
  myWires.clear();
  myFace.nullify();
  myWire.nullify();
  myCurrentItem = 0;

  /* signals and slots connections */
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(this, SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( myGroupWire->LineEdit1,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupWire->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupSurf->LineEdit1,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupSurf->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupSurf->LineEdit2,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupSurf->PushButton2, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupWireConstraints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupWireConstraints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( myTreeConstraints, SIGNAL( itemClicked( QTreeWidgetItem*, int) ), this, SLOT( onItemClicked( QTreeWidgetItem*, int ) ) );

  initName( tr( "GEOM_FACE" ) );

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_FaceDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      TColStd_MapOfInteger aMap;

      aMap.Add(GEOM_EDGE);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_FACE);
      aMap.Add(GEOM_SHELL);
      aMap.Add(GEOM_SOLID);
      aMap.Add(GEOM_COMPOUND);
      globalSelection(aMap);

      myEditCurrentArgument = myGroupWire->LineEdit1;
      myGroupWire->LineEdit1->setText("");
      myGroupWire->show();
      myGroupSurf->hide();
      myGroupWireConstraints->hide();
      break;
    }
  case 1:
    {
      globalSelection(GEOM_FACE); // For the first element.
      localSelection( TopAbs_FACE );

      myEditCurrentArgument = myGroupSurf->LineEdit1;
      myGroupSurf->LineEdit1->setText("");
      myGroupSurf->PushButton1->setDown(true);
      myGroupSurf->PushButton2->setDown(false);
      myGroupWire->hide();
      myGroupSurf->show();
      myGroupWireConstraints->hide();
      break;
    }
  case 2:
    {
      globalSelection();
      localSelection( TopAbs_WIRE );
    
      myTreeConstraints->clear();
      myCurrentItem = 0;
      myEditCurrentArgument = myGroupWireConstraints->LineEdit1;
      myGroupWireConstraints->LineEdit1->setText("");
      myGroupWireConstraints->LineEdit1->setEnabled(true);
      myGroupWire->hide();
      myGroupSurf->hide();
      myGroupWireConstraints->show();
      break;
    }
  }

  myWires.clear();
  myFace.nullify();
  myWire.nullify();
  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
  SelectionIntoArgument();
}

//=================================================================================
// function : updateConstraintsTree
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::updateConstraintsTree()
{
  if ( myEditCurrentArgument != myGroupWireConstraints->LineEdit1 || myWire.isNull() )
    return;

  myTreeConstraints->clear();

  GEOM::GEOM_IShapesOperations_ptr anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());
  GEOM::ListOfGO_var aList = anOper->ExtractSubShapes( myWire.get(), TopAbs_EDGE, false );
  if ( !aList->length() )
    return;

  for ( int i = 0, n = aList->length(); i < n; i++ ) {
    TreeWidgetItem* item = new TreeWidgetItem( myTreeConstraints,
                                               GEOM::GeomObjPtr( aList[i] ) );
  }

  myEditCurrentArgument->setEnabled(false);
  globalSelection();
  localSelection( TopAbs_FACE );
  
  myTreeConstraints->resizeColumnToContents(0);
  QTreeWidgetItem* firstItem = myTreeConstraints->topLevelItem(0);
  firstItem->setSelected( true );
  onItemClicked( firstItem, 0 );
}

//=================================================================================
// function : findEmptyTreeItem()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::findEmptyTreeItem()
{
  if ( isTreeFull() )
    return;

  QTreeWidgetItem* nextItem = 0;
  QTreeWidgetItem* item;
  for ( item = myTreeConstraints->itemBelow( myCurrentItem );
        item && !nextItem;
        item = myTreeConstraints->itemBelow( item ) ) {
    if ( (dynamic_cast<TreeWidgetItem*>(item))->getFace().isNull() )
      nextItem = item;
  }
  for ( item = myTreeConstraints->topLevelItem(0);
        item && item != myCurrentItem && !nextItem;
        item = myTreeConstraints->itemBelow( item ) ) {
    if ( (dynamic_cast<TreeWidgetItem*>(item))->getFace().isNull() )
      nextItem = item;
  }

  if ( nextItem && nextItem != myCurrentItem ) {
    myCurrentItem->setSelected( false );
    nextItem->setSelected( true );
    onItemClicked( nextItem, 0 );
  }
}

//=================================================================================
// function : isTreeFull()
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::isTreeFull()
{
  bool ok = true;
  QTreeWidgetItem* item;
  for ( item = myTreeConstraints->topLevelItem(0);
        item && ok;
        item = myTreeConstraints->itemBelow( item ) ) {
    ok = !(dynamic_cast<TreeWidgetItem*>(item))->getFace().isNull();
  }
  return ok;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument->setText("");
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_FaceDlg::SelectionIntoArgument()
{
  erasePreview();
  if ( myEditCurrentArgument == myGroupWire->LineEdit1 ) {
    myEditCurrentArgument->setText( "" );

    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE  << TopAbs_WIRE  << TopAbs_FACE
          << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPOUND;
    myWires = getSelected( types, -1 );

    if ( !myWires.isEmpty() ) {
      QString aName = myWires.count() > 1 ? QString( "%1_objects").arg( myWires.count() ) : GEOMBase::GetName( myWires[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if (myEditCurrentArgument == myGroupSurf->LineEdit1 ||
           myEditCurrentArgument == myGroupSurf->LineEdit2) {
    const bool isEditFace = myEditCurrentArgument == myGroupSurf->LineEdit1;
    const TopAbs_ShapeEnum aType = isEditFace ? TopAbs_FACE : TopAbs_WIRE;
    GEOM::GeomObjPtr aSelectedObject = getSelected(aType);
    GEOM::GeomObjPtr &anObj = isEditFace ? myFace : myWire;

    if (aSelectedObject) {
      myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject.get()));
      anObj = aSelectedObject;
    } else {
      myEditCurrentArgument->setText("");
      anObj.nullify();
    }

    if (isEditFace) {
      if (myFace && !myWire) {
        myGroupSurf->PushButton2->click();
      }
    } else {
      if (!myFace && myWire) {
        myGroupSurf->PushButton1->click();
      }
    }
  }
  else if ( myEditCurrentArgument == myGroupWireConstraints->LineEdit1 ) {
    if ( myCurrentItem ) {
      GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_FACE );
      TopoDS_Shape aFaceShape;
      GEOM::GEOM_IShapesOperations_ptr anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());
      if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aFaceShape ) && !aFaceShape.IsNull()
          && anOper->IsSubShapeBelongsTo( myCurrentItem->getEdge().get(), 0, aSelectedObject.get(), 0 ) ) {
        myCurrentItem->setFace( aSelectedObject );
        findEmptyTreeItem();
      }
      else {
        myCurrentItem->setFace( 0 );
        displayPreview( myCurrentItem->getEdge().get(), true, false, true, 5, -1, Quantity_NOC_RED );
      }
    }
    else {
      myWire.nullify();
      myEditCurrentArgument->setText( "" );
      GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_WIRE );
      TopoDS_Shape aWireShape;
      if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aWireShape ) && !aWireShape.IsNull() ) {
        QString aName = GEOMBase::GetName( aSelectedObject.get() );
        myEditCurrentArgument->setText( aName );
        myWire = aSelectedObject;
        updateConstraintsTree();
      }
      else {
        myTreeConstraints->clear();
        erasePreview( true );
      }
    }
  }
  displayPreview( true, false, true, true, -1, -1, -1, true );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send == myGroupWire->PushButton1 ) {
    TColStd_MapOfInteger aMap;
  
    aMap.Add(GEOM_EDGE);
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_FACE);
    aMap.Add(GEOM_SHELL);
    aMap.Add(GEOM_SOLID);
    aMap.Add(GEOM_COMPOUND);
    globalSelection(aMap);
    myEditCurrentArgument = myGroupWire->LineEdit1;
  }
  else if (send == myGroupSurf->PushButton1) {
    globalSelection(GEOM_FACE);
    localSelection( TopAbs_FACE );
    myEditCurrentArgument = myGroupSurf->LineEdit1;
    myGroupSurf->PushButton2->setDown(false);
    myGroupSurf->LineEdit2->setEnabled(false);
  }
  else if (send == myGroupSurf->PushButton2) {
    globalSelection(GEOM_WIRE);
    localSelection( TopAbs_WIRE );
    myEditCurrentArgument = myGroupSurf->LineEdit2;
    myGroupSurf->PushButton1->setDown(false);
    myGroupSurf->LineEdit1->setEnabled(false);
  }
  else if (send == myGroupWireConstraints->PushButton1) {
    globalSelection();
    localSelection( TopAbs_WIRE );
    myEditCurrentArgument = myGroupWireConstraints->LineEdit1;
    myCurrentItem = 0;
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : onItemClicked()
// purpose  : called when tree item was clicked
//=================================================================================
void BuildGUI_FaceDlg::onItemClicked( QTreeWidgetItem* theItem, int theColumn )
{
  if ( !theItem || !( theItem->flags() & Qt::ItemIsSelectable ) )
    return;

  myCurrentItem = dynamic_cast<TreeWidgetItem*>( theItem );
  displayPreview( true );
  displayPreview( myCurrentItem->getEdge().get(), true, false, true, 5, -1, Quantity_NOC_RED );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog(); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_FaceDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::isValid( QString& )
{
  bool ok = false;

  switch (getConstructorId()) {
  case 0:
    ok = !myWires.isEmpty();
    break;
  case 1:
    ok = myFace && myWire;
    break;
  case 2:
    ok = myWire;
    break;
  default:
    break;
  }

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
  case 0:
    {
      GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
      
      objlist->length( myWires.count() );

      for ( int i = 0; i < myWires.count(); i++ ) {
        objlist[i] = myWires[i].copy();
      }

      anObj = anOper->MakeFaceWires( objlist.in(), myGroupWire->CheckButton1->isChecked() );
      res = true;
    }
    break;
  case 1:
    anObj = anOper->MakeFaceFromSurface(myFace.get(), myWire.get());
    res = true;
    break;
  case 2:
    {
      int numberOfItems = myTreeConstraints->topLevelItemCount();
      GEOM::ListOfGO_var constraints = new GEOM::ListOfGO();
      constraints->length( 2 * numberOfItems );
      int j = 0;
      for ( int i = 0; i < numberOfItems; i++ ) {
        TreeWidgetItem* item = dynamic_cast<TreeWidgetItem*>( myTreeConstraints->topLevelItem(i) );
        constraints[j++] = item->getEdge().copy();
        if ( item->getFace() )
          constraints[j++] = item->getFace().copy();
      }
      constraints->length(j);
      anObj = anOper->MakeFaceWithConstraints( constraints.in() );
      res = true;
    }
    break;
  default:
    break;
  }

  if (!anObj->_is_nil()) {
    if ( !anOper->IsDone() && QString(anOper->GetErrorCode()) == "MAKE_FACE_TOLERANCE_TOO_BIG") {
      if ( !IsPreview() ) {
        SUIT_OverrideCursor wc;
        wc.suspend();
        QString msgw = QObject::tr(anOper->GetErrorCode());
        SUIT_MessageBox::warning(this, tr("WRN_WARNING"), msgw, tr("BUT_OK"));
      }
      anOper->SetErrorCode("PAL_NO_ERROR");
    }
    else if ( anObj->GetShapeType() == GEOM::COMPOUND ) {
      if ( !IsPreview() ) {
        SUIT_MessageBox::warning(this,
                                 QObject::tr("GEOM_WRN_WARNING"),
                                 QObject::tr("GEOM_WRN_FACES_NOT_FACE"));
      }
    }
    objects.push_back(anObj._retn());
  }

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BuildGUI_FaceDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    break;
  case 1:
    break;
  case 2:
    for( int i = 0; i < myTreeConstraints->topLevelItemCount(); i++ ) {
      TreeWidgetItem* item = dynamic_cast<TreeWidgetItem*>( myTreeConstraints->topLevelItem(i) );
      if ( item->getFace().get() )
        GEOMBase::PublishSubObject( item->getFace().get() );
      GEOMBase::PublishSubObject( myWire.get() );
    }
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BuildGUI_FaceDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myWires);
  res << myFace << myWire;
  return res;
}
