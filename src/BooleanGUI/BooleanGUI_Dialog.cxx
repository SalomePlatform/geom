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
// File   : BooleanGUI_Dialog.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BooleanGUI.h"
#include "BooleanGUI_Dialog.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>

// VSR 22/08/2012: issue 0021787: remove "Preview" button from BOP and Partition operations
// Comment next line to enable preview in BOP dialog box
#define NO_PREVIEW

//=================================================================================
// class    : BooleanGUI_Dialog()
// purpose  : Constructs a BooleanGUI_Dialog which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BooleanGUI_Dialog::BooleanGUI_Dialog (const int theOperation, GeometryGUI* theGeometryGUI,
                                      QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myOperation(theOperation)
{
  QPixmap image0;
  QString aTitle, aCaption;
  switch (myOperation) {
  case BooleanGUI::COMMON:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_COMMON")));
    aTitle = tr("GEOM_COMMON");
    aCaption = tr("GEOM_COMMON_TITLE");
    setHelpFileName("common_operation_page.html");
    break;
  case BooleanGUI::CUT:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CUT")));
    aTitle = tr("GEOM_CUT");
    aCaption = tr("GEOM_CUT_TITLE");
    setHelpFileName("cut_operation_page.html");
    break;
  case BooleanGUI::FUSE:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FUSE")));
    aTitle = tr("GEOM_FUSE");
    aCaption = tr("GEOM_FUSE_TITLE");
    setHelpFileName("fuse_operation_page.html");
    break;
  case BooleanGUI::SECTION:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SECTION")));
    aTitle = tr("GEOM_SECTION");
    aCaption = tr("GEOM_SECTION_TITLE");
    setHelpFileName("section_opeartion_page.html");
    break;
  }
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(aCaption);

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(aTitle);
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  myGroup = new DlgRef_2Sel2Spin3Check(centralWidget());

  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  if (myOperation == BooleanGUI::CUT) {
    myGroup->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
    myGroup->TextLabel2->setText(tr("GEOM_TOOL_OBJECTS"));
  }
  else if (myOperation == BooleanGUI::SECTION) {
    myGroup->TextLabel1->setText(tr("GEOM_OBJECT_I").arg(1));
    myGroup->TextLabel2->setText(tr("GEOM_OBJECT_I").arg(2));
  } else { // Fuse or Common
    myGroup->TextLabel1->setText(tr( "GEOM_SELECTED_OBJECTS" ));
    myGroup->TextLabel2->hide();
    myGroup->PushButton2->hide();
    myGroup->LineEdit2->hide();

    if (myOperation == BooleanGUI::FUSE) {
      myGroup->CheckBox2->setText(tr("GEOM_BOOL_REMOVE_EXTRA_EDGES"));
    }
  }

  myGroup->PushButton1->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);

  if (myOperation != BooleanGUI::FUSE) {
    myGroup->CheckBox2->hide();

    if (myOperation != BooleanGUI::COMMON) {
      myGroup->PushButton2->setIcon(image1);
      myGroup->LineEdit2->setReadOnly(true);
    }
  }

  myGroup->TextLabel3->hide();
  myGroup->TextLabel4->hide();
  myGroup->SpinBox_DX->hide();
  myGroup->SpinBox_DY->hide();
  myGroup->CheckBox3->hide();
  myGroup->CheckBox1->setText(tr("GEOM_CHECK_SELF_INTERSECTIONS"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);
  /***************************************************************/

#ifdef NO_PREVIEW
  mainFrame()->CheckBoxPreview->setChecked( false );
  mainFrame()->CheckBoxPreview->hide();
#endif
  // Initialisation
  Init();
}

//=================================================================================
// function : ~BooleanGUI_Dialog()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BooleanGUI_Dialog::~BooleanGUI_Dialog()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::Init()
{
  mainFrame()->GroupBoxPublish->show();

  // init variables
  myEditCurrentArgument = myGroup->LineEdit1;

  myGroup->LineEdit1->setText("");
  myGroup->LineEdit2->setText("");
  myGroup->CheckBox1->setChecked(true);

  if (myOperation == BooleanGUI::FUSE) {
    myGroup->CheckBox2->setChecked(true);
  }

  myObject1.nullify();
  reset();
 
  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGroup->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  if (!myGroup->PushButton2->isHidden()) {
    connect(myGroup->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  }

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()), Qt::UniqueConnection);

  initName(mainFrame()->GroupConstructors->title());

  setTabOrder(mainFrame()->GroupConstructors, mainFrame()->GroupBoxName);
  setTabOrder(mainFrame()->GroupBoxName, mainFrame()->GroupMedium);
  setTabOrder(mainFrame()->GroupMedium, mainFrame()->GroupButtons);

  mainFrame()->RadioButton1->setFocus();

  globalSelection(GEOM_ALLSHAPES);
  //localSelection(TopAbs_SHAPE); // VSR 24/09/2015: dectivate local selection in BOP (CoTech decision)
  
  myGroup->PushButton1->click();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  myGroup->PushButton1->click();
  return true;
}

//=================================================================================
// function : reset()
// purpose  : 
//=================================================================================
void BooleanGUI_Dialog::reset()
{
  myObjects.clear();
}

//=================================================================================
// function : singleSelection
// purpose  : Performs single selection. Called from SelectionIntoArgument()
//=================================================================================
void BooleanGUI_Dialog::singleSelection()
{
  myEditCurrentArgument->setText("");

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_SHAPE );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );

    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()), Qt::UniqueConnection);

    if (myEditCurrentArgument == myGroup->LineEdit1) {
      myObject1 = aSelectedObject;
      if (!myGroup->PushButton2->isHidden() && !myObjects.count())
        myGroup->PushButton2->click();
    }
    else if (myEditCurrentArgument == myGroup->LineEdit2) {
      myObjects.clear();
      myObjects << aSelectedObject;
      if (!myObject1)
        myGroup->PushButton1->click();
    }
  }
  else {
    if      (myEditCurrentArgument == myGroup->LineEdit1) myObject1.nullify();
    else if (myEditCurrentArgument == myGroup->LineEdit2) reset();
  }
}

//=================================================================================
// function : multipleSelection
// purpose  : Performs multiple selection. Called from SelectionIntoArgument()
//=================================================================================
void BooleanGUI_Dialog::multipleSelection()
{
  myEditCurrentArgument->setText( "" );
  reset();
        
  myObjects = getSelected( TopAbs_SHAPE, -1 );

  int i = myObjects.count();
  if ( i == 1 ) {
    myEditCurrentArgument->setText( GEOMBase::GetName( myObjects.first().get() ) );
  } else if ( i > 0 ) {
    myEditCurrentArgument->setText( QString::number( i ) + "_" + tr( "GEOM_OBJECTS" ) );
  }
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void BooleanGUI_Dialog::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  if ( myOperation == BooleanGUI::SECTION ||
      (myOperation == BooleanGUI::CUT &&
       myEditCurrentArgument == myGroup->LineEdit1)) {
    singleSelection();
  } else {
    multipleSelection();
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myGroup->PushButton1) {
    myEditCurrentArgument = myGroup->LineEdit1;

    if (!myGroup->PushButton2->isHidden()) {
      myGroup->PushButton2->setDown(false);
      myGroup->LineEdit2->setEnabled(false);
    }
  }
  else if (send == myGroup->PushButton2) {
    myEditCurrentArgument = myGroup->LineEdit2;

    myGroup->PushButton1->setDown(false);
    myGroup->LineEdit1->setEnabled(false);
  }

  globalSelection(GEOM_ALLSHAPES);
  //localSelection(TopAbs_SHAPE); // VSR 24/09/2015: dectivate local selection in BOP (CoTech decision)

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ), Qt::UniqueConnection );
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void BooleanGUI_Dialog::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BooleanGUI_Dialog::createOperation()
{
  return getGeomEngine()->GetIBooleanOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::isValid (QString&)
{
  bool isOK = false;

  switch (myOperation) {
    case BooleanGUI::FUSE:
    case BooleanGUI::COMMON:
      isOK = myObjects.count() > 1;
    break;
  case BooleanGUI::CUT:
    isOK = myObject1 && myObjects.count();
    break;
  case BooleanGUI::SECTION:
    isOK = myObject1 && (myObjects.count() == 1);
    break;
  default:
    break;
  }

  return isOK;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBooleanOperations_var anOper = GEOM::GEOM_IBooleanOperations::_narrow(getOperation());
  const bool isCheckSelfInte = myGroup->CheckBox1->isChecked();

  GEOM::ListOfGO_var anObjects = new GEOM::ListOfGO();
  anObjects->length( myObjects.count() );
  for ( int i = 0; i < myObjects.count(); i++ )
    anObjects[i] = myObjects[i].copy();

  switch (myOperation) {
    case BooleanGUI::FUSE:
      {
        const bool isRmExtraEdges = myGroup->CheckBox2->isChecked();

        anObj = anOper->MakeFuseList
          (anObjects, isCheckSelfInte, isRmExtraEdges);
      }
    break;
    case BooleanGUI::COMMON:
      anObj = anOper->MakeCommonList(anObjects, isCheckSelfInte);
    break;
  case BooleanGUI::CUT:
      anObj =
        anOper->MakeCutList(myObject1.get(), anObjects, isCheckSelfInte);
    break;
  case BooleanGUI::SECTION:
      anObj = anOper->MakeBoolean
        (myObject1.get(), anObjects[0], myOperation, isCheckSelfInte);
    break;
  default:
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                          SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                         /*theFindMethod=*/GEOM::FSM_GetInPlace, // ? GEOM::FSM_GetSame
                                         /*theInheritFirstArg=*/myOperation == BooleanGUI::CUT,
                                         mainFrame()->CheckBoxAddPrefix->isChecked()); // ? false
  }
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BooleanGUI_Dialog::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myObject1.get() );
  for ( int i = 0; i < myObjects.count(); i++ )
    GEOMBase::PublishSubObject( myObjects[i].get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BooleanGUI_Dialog::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myObjects);
  res << myObject1;
  return res;
}
