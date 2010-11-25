//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : BlocksGUI_BlockDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI_BlockDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>

// OCCT Includes
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BlocksGUI_BlockDlg()
// purpose  : Constructs a BlocksGUI_BlockDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_BlockDlg::BlocksGUI_BlockDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent),
    myInitial(true)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_2F")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_6F")));
  QPixmap imageS (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_BLOCK_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_BLOCK"));

  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  // Create first group
  Group2F = new DlgRef_2Sel (centralWidget());
  Group2F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group2F->TextLabel1->setText(tr("FACE_1"));
  Group2F->TextLabel2->setText(tr("FACE_2"));
  Group2F->PushButton1->setIcon(imageS);
  Group2F->PushButton2->setIcon(imageS);

  // Create second group
  Group6F = new DlgRef_6Sel (centralWidget());
  Group6F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group6F->TextLabel1->setText(tr("FACE_1"));
  Group6F->TextLabel2->setText(tr("FACE_2"));
  Group6F->TextLabel3->setText(tr("FACE_3"));
  Group6F->TextLabel4->setText(tr("FACE_4"));
  Group6F->TextLabel5->setText(tr("FACE_5"));
  Group6F->TextLabel6->setText(tr("FACE_6"));
  Group6F->PushButton1->setIcon(imageS);
  Group6F->PushButton2->setIcon(imageS);
  Group6F->PushButton3->setIcon(imageS);
  Group6F->PushButton4->setIcon(imageS);
  Group6F->PushButton5->setIcon(imageS);
  Group6F->PushButton6->setIcon(imageS);

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(Group2F);
  layout->addWidget(Group6F);
  /***************************************************************/

  setHelpFileName("build_by_blocks_page.html#hexa_solid_anchor");

  Init();
}

//=================================================================================
// function : ~BlocksGUI_BlockDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_BlockDlg::~BlocksGUI_BlockDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::Init()
{
  // init variables
  Group2F->LineEdit1->setReadOnly(true);
  Group2F->LineEdit2->setReadOnly(true);

  Group6F->LineEdit1->setReadOnly(true);
  Group6F->LineEdit2->setReadOnly(true);
  Group6F->LineEdit3->setReadOnly(true);
  Group6F->LineEdit4->setReadOnly(true);
  Group6F->LineEdit5->setReadOnly(true);
  Group6F->LineEdit6->setReadOnly(true);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group2F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group6F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton5, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton6, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  // init controls and fields
  initName(tr("GEOM_BLOCK"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_BlockDlg::ConstructorsClicked (int constructorId)
{
  if (myConstructorId == constructorId)
    return;

  myConstructorId = constructorId;

  // init fields
  myFace1 = myFace2 = GEOM::GEOM_Object::_nil();
  myFace3 = myFace4 = myFace5 = myFace6 = myFace1;

  switch (constructorId) {
  case 0:
    Group6F->hide();
    Group2F->show();

    Group2F->LineEdit1->setText(tr(""));
    Group2F->LineEdit2->setText(tr(""));

    Group2F->PushButton1->click();
    break;
  case 1:
    Group2F->hide();
    Group6F->show();

    Group6F->LineEdit1->setText(tr(""));
    Group6F->LineEdit2->setText(tr(""));
    Group6F->LineEdit3->setText(tr(""));
    Group6F->LineEdit4->setText(tr(""));
    Group6F->LineEdit5->setText(tr(""));
    Group6F->LineEdit6->setText(tr(""));

    Group6F->PushButton1->click();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  // on dialog initialization we init the first field with a selected object (if any)
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_BlockDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void BlocksGUI_BlockDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == Group2F->LineEdit1) myFace1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group2F->LineEdit2) myFace2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit1) myFace1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit2) myFace2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit3) myFace3 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit4) myFace4 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit5) myFace5 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit6) myFace6 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

  if (!testResult || CORBA::is_nil(aSelectedObject))
    return;

  QString aName = GEOMBase::GetName(aSelectedObject);

  // Get Selected object if selected subshape
  TopoDS_Shape aShape;
  if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
  {
    TColStd_IndexedMapOfInteger aMap;
    aSelMgr->GetIndexes(aSelList.First(), aMap);
    if (aMap.Extent() == 1) // Local Selection
    {
      int anIndex = aMap(1);
      aName += QString(":face_%1").arg(anIndex);

      //Find SubShape Object in Father
      GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

      if (aFindedObject->_is_nil()) { // Object not found in study
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
      }
      else {
        aSelectedObject = aFindedObject; // get Object from study
      }
    }
    else // Global Selection
    {
      if (aShape.ShapeType() != TopAbs_FACE) {
        aSelectedObject = GEOM::GEOM_Object::_nil();
        aName = "";
      }
    }
  }

  myEditCurrentArgument->setText(aName);

  if (myEditCurrentArgument == Group2F->LineEdit1) {
    myFace1 = aSelectedObject;
    if (!myFace1->_is_nil() && myFace2->_is_nil())
      Group2F->PushButton2->click();
  }
  else if (myEditCurrentArgument == Group2F->LineEdit2) {
    myFace2 = aSelectedObject;
    if (!myFace2->_is_nil() && myFace1->_is_nil())
      Group2F->PushButton1->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit1) {
    myFace1 = aSelectedObject;
    if (!myFace1->_is_nil() && myFace2->_is_nil())
      Group6F->PushButton2->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit2) {
    myFace2 = aSelectedObject;
    if (!myFace2->_is_nil() && myFace3->_is_nil())
      Group6F->PushButton3->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit3) {
    myFace3 = aSelectedObject;
    if (!myFace3->_is_nil() && myFace4->_is_nil())
      Group6F->PushButton4->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit4) {
    myFace4 = aSelectedObject;
    if (!myFace4->_is_nil() && myFace5->_is_nil())
      Group6F->PushButton5->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit5) {
    myFace5 = aSelectedObject;
    if (!myFace5->_is_nil() && myFace6->_is_nil())
      Group6F->PushButton6->click();
  }
  else if (myEditCurrentArgument == Group6F->LineEdit6) {
    myFace6 = aSelectedObject;
    if (!myFace6->_is_nil() && myFace1->_is_nil())
      Group6F->PushButton1->click();
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (myInitial)
    myInitial = false;
  else
    myGeomGUI->getApp()->selectionMgr()->clearSelected();

  // disable all
  switch (myConstructorId) {
  case 0:
    Group2F->PushButton1->setDown(false);
    Group2F->PushButton2->setDown(false);

    Group2F->LineEdit1->setEnabled(false);
    Group2F->LineEdit2->setEnabled(false);
    break;
  case 1:
    Group6F->PushButton1->setDown(false);
    Group6F->PushButton2->setDown(false);
    Group6F->PushButton3->setDown(false);
    Group6F->PushButton4->setDown(false);
    Group6F->PushButton5->setDown(false);
    Group6F->PushButton6->setDown(false);

    Group6F->LineEdit1->setEnabled(false);
    Group6F->LineEdit2->setEnabled(false);
    Group6F->LineEdit3->setEnabled(false);
    Group6F->LineEdit4->setEnabled(false);
    Group6F->LineEdit5->setEnabled(false);
    Group6F->LineEdit6->setEnabled(false);
    break;
  default:
    break;
  }

  // set line edit as current argument
  if (aSender == Group2F->PushButton1) {
    myEditCurrentArgument = Group2F->LineEdit1;
  }
  else if (aSender == Group2F->PushButton2) {
    myEditCurrentArgument = Group2F->LineEdit2;
  }
  else if (aSender == Group6F->PushButton1) {
    myEditCurrentArgument = Group6F->LineEdit1;
  }
  else if (aSender == Group6F->PushButton2) {
    myEditCurrentArgument = Group6F->LineEdit2;
  }
  else if (aSender == Group6F->PushButton3) {
    myEditCurrentArgument = Group6F->LineEdit3;
  }
  else if (aSender == Group6F->PushButton4) {
    myEditCurrentArgument = Group6F->LineEdit4;
  }
  else if (aSender == Group6F->PushButton5) {
    myEditCurrentArgument = Group6F->LineEdit5;
  }
  else if (aSender == Group6F->PushButton6) {
    myEditCurrentArgument = Group6F->LineEdit6;
  }

  // enable push button and line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  aSender->setDown(true);

  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_FACE); //Select Faces on All Shapes
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_FACE); //Select Faces on All Shapes
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
  displayPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_BlockDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BlocksGUI_BlockDlg::isValid (QString&)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = !(myFace1->_is_nil() || myFace2->_is_nil());
    break;
  case 1:
    ok =  !(myFace1->_is_nil() || myFace2->_is_nil() ||
            myFace3->_is_nil() || myFace4->_is_nil() ||
            myFace5->_is_nil() || myFace6->_is_nil());
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
bool BlocksGUI_BlockDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    if (!CORBA::is_nil(myFace1) && !CORBA::is_nil(myFace2)) {
      anObj = anOper->MakeHexa2Faces(myFace1, myFace2);
      res = true;
    }
    break;
  case 1:
    if (!CORBA::is_nil(myFace1) && !CORBA::is_nil(myFace2) &&
        !CORBA::is_nil(myFace3) && !CORBA::is_nil(myFace4) &&
        !CORBA::is_nil(myFace5) && !CORBA::is_nil(myFace6)) {
      anObj = anOper->MakeHexa(myFace1, myFace2, myFace3, myFace4, myFace5, myFace6);
      res = true;
    }
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BlocksGUI_BlockDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch (getConstructorId()) {
  case 0:
    objMap[Group2F->LineEdit1->text()] = myFace1;
    objMap[Group2F->LineEdit2->text()] = myFace2;
    break;
  case 1:
    objMap[Group6F->LineEdit1->text()] = myFace1;
    objMap[Group6F->LineEdit2->text()] = myFace2;
    objMap[Group6F->LineEdit3->text()] = myFace3;
    objMap[Group6F->LineEdit4->text()] = myFace4;
    objMap[Group6F->LineEdit5->text()] = myFace5;
    objMap[Group6F->LineEdit6->text()] = myFace6;
    break;
  }
  addSubshapesToFather(objMap);
}
