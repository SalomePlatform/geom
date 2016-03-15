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
// File   : OperationGUI_PartitionDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "OperationGUI_PartitionDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOMImpl_Types.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

// VSR 22/08/2012: issue 0021787: remove "Preview" button from BOP and Partition operations
// Comment next line to enable preview in Partition dialog box
#define NO_PREVIEW

//=================================================================================
// class    : OperationGUI_PartitionDlg()
// purpose  : Constructs a OperationGUI_PartitionDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_PartitionDlg::OperationGUI_PartitionDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_PARTITION" ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_PARTITION_PLANE" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_PARTITION_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_PARTITION" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  // Full partition (contains half-space partition)
  GroupPoints = new DlgRef_2Sel1List2Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_PARTITION" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_TOOL_OBJECTS" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_RECONSTRUCTION_LIMIT" ) );
  GroupPoints->PushButton1->setIcon( image2 );
  GroupPoints->PushButton2->setIcon( image2 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );
  GroupPoints->CheckButton1->setText( tr( "GEOM_KEEP_NONLIMIT_SHAPES" ) );
  GroupPoints->CheckButton2->setText( tr( "GEOM_NO_SELF_INTERSECTION" ) );

  mySelfInte = new QCheckBox(GroupPoints->GroupBox1);
  mySelfInte->setText(tr("GEOM_CHECK_SELF_INTERSECTIONS"));
  GroupPoints->gridLayout1->addWidget(mySelfInte, 5, 0, 1, 3);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/

  setHelpFileName( "partition_page.html" );

#ifdef NO_PREVIEW
  mainFrame()->CheckBoxPreview->setChecked( false );
  mainFrame()->CheckBoxPreview->hide();
#endif
  Init();
}


//=================================================================================
// function : ~OperationGUI_PartitionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_PartitionDlg::~OperationGUI_PartitionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void OperationGUI_PartitionDlg::SetListMaterials( GEOM::ListOfLong ListMaterials )
{
  myListMaterials = ListMaterials;
}

GEOM::ListOfLong OperationGUI_PartitionDlg::GetListMaterials()
{
  return myListMaterials;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::Init()
{
  /* type for sub-shape selection */
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_SOLID" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::SOLID);
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_SHELL" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::SHELL);
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_FACE" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::FACE);
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_WIRE" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::WIRE);
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_EDGE" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::EDGE);
  GroupPoints->ComboBox1->addItem( tr( "GEOM_RECONSTRUCTION_LIMIT_VERTEX" ) );
  GroupPoints->ComboBox1->setItemData(GroupPoints->ComboBox1->count()-1, GEOM::VERTEX);
  GroupPoints->CheckButton1->setChecked( false );
  mySelfInte->setChecked(false);

  mainFrame()->GroupBoxPublish->show();

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this, SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->ComboBox1, SIGNAL( activated( int ) ), this, SLOT( ComboTextChanged() ) );

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(processPreview()) );
  connect( GroupPoints->CheckButton2, SIGNAL(toggled(bool)), this, SLOT(processPreview()) );
  connect( GroupPoints->CheckButton2, SIGNAL(toggled(bool)), mySelfInte, SLOT(setEnabled(bool)) );
  connect( mySelfInte,                SIGNAL(toggled(bool)), this, SLOT(processPreview()) );

  mySelfInte->setEnabled(GroupPoints->CheckButton2->isChecked());
  initName( tr( "GEOM_PARTITION" ) );

  ConstructorsClicked( 0 );
  GroupPoints->PushButton1->click();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_PartitionDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  globalSelection();

  //myListShapes.length( 0 );
  //myListTools.length( 0 );
  //myListKeepInside.length( 0 );   // obsolete
  //myListRemoveInside.length( 0 ); // obsolete
  //myListMaterials.length( 0 );    // obsolete

  switch ( constructorId ) {
  case 0: /*Full partition */
    GroupPoints->GroupBox1->setTitle( tr( "GEOM_PARTITION" ) );
    GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
    GroupPoints->TextLabel2->setText( tr( "GEOM_TOOL_OBJECTS" ) );
    GroupPoints->TextLabel3->show();
    GroupPoints->ComboBox1->show();
    GroupPoints->ComboBox1->setCurrentIndex( 0 );
    GroupPoints->CheckButton1->show();
    GroupPoints->CheckButton2->show();
    mySelfInte->show();
    GroupPoints->PushButton1->setDown( true );
    GroupPoints->PushButton2->setDown( false );
    GroupPoints->LineEdit1->setEnabled(true);
    GroupPoints->LineEdit2->setEnabled(false);
    break;
  case 1: /*Half-space partition */
    GroupPoints->GroupBox1->setTitle( tr( "GEOM_PARTITION_HALFSPACE" ) );
    GroupPoints->TextLabel3->hide();
    GroupPoints->ComboBox1->hide();
    GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
    GroupPoints->TextLabel2->setText( tr( "GEOM_PLANE" ) );
    GroupPoints->CheckButton1->hide();
    GroupPoints->CheckButton2->hide();
    mySelfInte->hide();
    GroupPoints->PushButton1->setDown( true );
    GroupPoints->LineEdit1->setEnabled(true);
    break;
  }

  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  myEditCurrentArgument->setFocus();
  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  //  0020854: EDF 1398 GEOM: Ergonomy of Partition GUI window
  //  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_PartitionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = "";

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString, true);

  if ( getConstructorId() == 1 ) {
    // for half-selection, only one shape can be selected as an object
    // and only one plane as a tool
    if ( nbSel != 1 ) {
      if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
        myListShapes.length( 0 );
        // myListMaterials.length( 0 ); // obsolete
        return;
      }
      else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
        myListTools.length( 0 );
        return;
      }
    }
  }
  else {
    if ( nbSel < 1 ) {
      if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
        myListShapes.length( 0 );
        //myListMaterials.length( 0 ); // obsolete
        return;
      }
      else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
        myListTools.length( 0 );
        return;
      }
    }
  }

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myListShapes, true);
    //myListMaterials.length( 0 ); // obsolete

    GEOM::shape_type type = GEOM::SHAPE;
    for (int i = 0; i < myListShapes.length(); i++)
      type = qMin( type, myListShapes[i]->GetMaxShapeType() );
    int idx = qMax( 0, GroupPoints->ComboBox1->findData( type ) );
    GroupPoints->ComboBox1->setCurrentIndex( idx );

    if ( !myListShapes.length() )
      return;
  }
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myListTools, true);
    if ( !myListTools.length() )
      return;
    else if ( myListTools.length() == 1 && getConstructorId() == 1 ) {
      GEOM::GEOM_IMeasureOperations_var mOp = getGeomEngine()->GetIMeasureOperations( getStudyId() );
      GEOM::ListOfLong_var   intList;
      GEOM::ListOfDouble_var dblList;
      GEOM::GEOM_IKindOfShape::shape_kind kind = mOp->KindOfShape( myListTools[0].in(), intList.out(), dblList.out() );
      mOp->UnRegister();
      if ( kind < GEOM::GEOM_IKindOfShape::DISK_CIRCLE || kind > GEOM::GEOM_IKindOfShape::PLANAR ) {
        myListTools.length( 0 );
        return;
      }
    }
  }

  myEditCurrentArgument->setText( aString );
  processPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled(true);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myGeomGUI->getApp()->selectionMgr()->clearSelected(); //clear prewious selection
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(true);
    if ( getConstructorId() == 1 )
      globalSelection( GEOM_PLANE  );
  }

  globalSelection( GEOM_ALLSHAPES );

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
  send->setDown(true);
  processPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 ) {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
  processPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_PartitionDlg::createOperation()
{
  return getGeomEngine()->GetIBooleanOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::isValid (QString& )
{
  return (getConstructorId() == 0 ?
          myListShapes.length() :
          myListShapes.length() && myListTools.length());
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::execute (ObjectList& objects)
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBooleanOperations_var anOper = GEOM::GEOM_IBooleanOperations::_narrow(getOperation());

  switch ( getConstructorId() ) {
  case 0:
    {
      bool isDetectSelfInte    = mySelfInte->isChecked();
      int aLimit               = GetLimit();
      int aKeepNonlimitShapes  = GroupPoints->CheckButton1->isChecked();
      bool aNoSelfIntersection = GroupPoints->CheckButton2->isChecked();

      anObj = aNoSelfIntersection ?
        anOper->MakePartitionNonSelfIntersectedShape(myListShapes, myListTools,
                                                     myListKeepInside, myListRemoveInside,
                                                     aLimit, false, myListMaterials, aKeepNonlimitShapes,
                                                     isDetectSelfInte) :
        anOper->MakePartition(myListShapes, myListTools,
                              myListKeepInside, myListRemoveInside,
                              aLimit, false, myListMaterials, aKeepNonlimitShapes);
      res = true;
    }
    break;
  case 1:
    {
      anObj = anOper->MakeHalfPartition(myListShapes[0].in(), myListTools[0].in());
      res = true;
    }
    break;
  }

  if ( !anObj->_is_nil() ) {
    TopoDS_Shape aShape;
    GEOMBase::GetShape( anObj, aShape, TopAbs_SHAPE );
    TopoDS_Iterator It( aShape, Standard_True, Standard_True );
    int nbSubshapes = 0;
    for (; It.More(); It.Next())
      nbSubshapes++;

    if ( nbSubshapes )
      objects.push_back( anObj._retn() );
    else
      SUIT_MessageBox::warning( this,
                                QObject::tr( "GEOM_ERROR" ),
                                QObject::tr( "GEOM_WRN_PARTITION_RESULT_EMPTY" ) );
  }

  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::restoreSubShapes( SALOMEDS::Study_ptr   theStudy,
                                                  SALOMEDS::SObject_ptr theSObject )
{
  if ( mainFrame()->CheckBoxRestoreSS->isChecked() ) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO( theStudy, theSObject, GEOM::ListOfGO(),
                                         /*theFindMethod=*/GEOM::FSM_GetInPlaceByHistory,
                                         /*theInheritFirstArg=*/myListShapes.length() == 1 && myListTools.length() == 0,
                                         mainFrame()->CheckBoxAddPrefix->isChecked() ); // ? false
  }
}

//=======================================================================
//function : ComboTextChanged
//purpose  :
//=======================================================================
void OperationGUI_PartitionDlg::ComboTextChanged()
{
  //bool IsEnabled = GroupPoints->ComboBox1->currentItem() < 3;
  //GroupPoints->LineEdit3->setEnabled(IsEnabled);
  //GroupPoints->LineEdit4->setEnabled(IsEnabled);
  //GroupPoints->TextLabel4->setEnabled(IsEnabled);
  //GroupPoints->TextLabel5->setEnabled(IsEnabled);
  //GroupPoints->PushButton3->setEnabled(IsEnabled);
  //GroupPoints->PushButton4->setEnabled(IsEnabled);
  processPreview();
}

//=================================================================================
// function : GetLimit()
// purpose  :
//=================================================================================
int OperationGUI_PartitionDlg::GetLimit() const
{
  int aLimit = GroupPoints->ComboBox1->currentIndex();

  switch ( aLimit ) {
  case 0:  aLimit = GEOM::SOLID ; break;
  case 1:  aLimit = GEOM::SHELL ; break;
  case 2:  aLimit = GEOM::FACE  ; break;
  case 3:  aLimit = GEOM::WIRE  ; break;
  case 4:  aLimit = GEOM::EDGE  ; break;
  case 5:  aLimit = GEOM::VERTEX; break;
  default: aLimit = GEOM::SHAPE ;
  }

  return aLimit;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_PartitionDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  for (int i = 0; i < myListShapes.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListShapes[i]);
    res << aGeomObjPtr;
  }
  for (int i = 0; i < myListTools.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListTools[i]);
    res << aGeomObjPtr;
  }
  for (int i = 0; i < myListRemoveInside.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListRemoveInside[i]);
    res << aGeomObjPtr;
  }
  for (int i = 0; i < myListKeepInside.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListKeepInside[i]);
    res << aGeomObjPtr;
  }
  return res;
}
